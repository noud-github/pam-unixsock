#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/pam_redirector.sock"

// Function to prompt for TOTP token
static int prompt_for_totp(pam_handle_t *pamh, const char *prompt, char **totp_token) {
    struct pam_message msg;
    struct pam_response *resp = NULL;
    const struct pam_message *msgp;
    struct pam_conv *conv;
    int retval;

    retval = pam_get_item(pamh, PAM_CONV, (const void **)&conv);
    if (retval != PAM_SUCCESS) {
        return retval;
    }

    msg.msg_style = PAM_PROMPT_ECHO_OFF; // Hide the TOTP input
    msg.msg = prompt;
    msgp = &msg;

    retval = conv->conv(1, &msgp, &resp, conv->appdata_ptr);
    if (retval == PAM_SUCCESS && resp != NULL) {
        *totp_token = strdup(resp->resp); // Copy the TOTP token
        free(resp->resp);
        free(resp);
    }

    return retval;
}

// Function to send data to the Unix socket
static int send_to_socket(const char *username, const char *password, const char *totp_token) {
    int sockfd;
    struct sockaddr_un server_addr;
    char buffer[1024];

    // Create the socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return PAM_SYSTEM_ERR;
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        return PAM_SYSTEM_ERR;
    }

    // Prepare the message (username:password:totp)
    if (totp_token) {
        snprintf(buffer, sizeof(buffer), "%s:%s:%s", username, password, totp_token);
    } else {
        snprintf(buffer, sizeof(buffer), "%s:%s", username, password);
    }

    // Send the message
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        close(sockfd);
        return PAM_SYSTEM_ERR;
    }

    close(sockfd);
    return PAM_SUCCESS;
}

// Function to check if TOTP is enabled in the PAM configuration
static int is_totp_enabled(pam_handle_t *pamh) {
    const char *config_value;
    int retval;

    retval = pam_get_item(pamh, PAM_SERVICE, (const void **)&config_value);
    if (retval == PAM_SUCCESS && config_value != NULL) {
        // Check if the configuration contains "totp_enabled=1"
        if (strstr(config_value, "totp_enabled=1") != NULL) {
            return 1;
        }
    }

    return 0;
}

// PAM authentication function
PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *username;
    const char *password;
    char *totp_token = NULL;
    int retval;

    // Get the username
    retval = pam_get_user(pamh, &username, NULL);
    if (retval != PAM_SUCCESS) {
        return retval;
    }

    // Get the password
    retval = pam_get_authtok(pamh, PAM_AUTHTOK, &password, NULL);
    if (retval != PAM_SUCCESS) {
        return retval;
    }

    // Check if TOTP is enabled
    if (is_totp_enabled(pamh)) {
        retval = prompt_for_totp(pamh, "Enter TOTP token: ", &totp_token);
        if (retval != PAM_SUCCESS) {
            return retval;
        }
    }

    // Send the data to the Unix socket
    retval = send_to_socket(username, password, totp_token);
    if (totp_token != NULL) {
        free(totp_token);
    }

    return retval;
}

// PAM session management (stubs)
PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

// PAM account management (stubs)
PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

// PAM password management (stubs)
PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
