#define _GNU_SOURCE
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#define DEFAULT_TIMEOUT 2

static int connect_to_socket(const char *socket_path, int timeout) {
    int sockfd;
    struct sockaddr_un addr;
    struct timeval tv;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sockfd);
        return -1;
    }

    return sockfd;
}

static int send_credentials(int sockfd, const char *username, const char *password, const char *prompt_response) {
    dprintf(sockfd, "%s\n%s\n%s\n", username, password, prompt_response ? prompt_response : "");
    char response;
    if (read(sockfd, &response, 1) == 1 && response == '1') {
        return PAM_SUCCESS;
    }
    return PAM_AUTH_ERR;
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *socket_path = "/var/run/unix.sock";
    const char *prompt = NULL;
    bool hidden = false;
    int timeout = DEFAULT_TIMEOUT;

    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "prompt=", 7) == 0) {
            prompt = argv[i] + 7;
        } else if (strcmp(argv[i], "hidden") == 0) {
            hidden = true;
        } else if (strncmp(argv[i], "timeout=", 8) == 0) {
            timeout = atoi(argv[i] + 8);
        } else {
            socket_path = argv[i];
        }
    }

    const char *username, *password, *prompt_response = "";
    pam_get_user(pamh, &username, NULL);
    pam_get_authtok(pamh, PAM_AUTHTOK, &password, NULL);

    if (prompt) {
        pam_prompt(pamh, hidden ? PAM_PROMPT_ECHO_OFF : PAM_PROMPT_ECHO_ON, (char **)&prompt_response, "%s", prompt);
    }

    int sockfd = connect_to_socket(socket_path, timeout);
    if (sockfd < 0) {
        return PAM_SUCCESS; // Assume success on timeout or failure to connect
    }

    int result = send_credentials(sockfd, username, password, prompt_response);
    close(sockfd);
    return result;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
