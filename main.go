package main

import (
	"fmt"
	"net"
	"os"

	"github.com/msteinert/pam/v2"
)

// authenticateUser authenticates a user using PAM.
func authenticateUser(username, password string) error {
	transaction, err := pam.StartFunc("", username, func(s pam.Style, msg string) (string, error) {
		switch s {
		case pam.PromptEchoOff:
			return password, nil
		case pam.PromptEchoOn:
			return username, nil
		case pam.ErrorMsg:
			fmt.Fprintf(os.Stderr, "Error: %s\n", msg)
			return "", nil
		case pam.TextInfo:
			fmt.Fprintf(os.Stdout, "%s\n", msg)
			return "", nil
		}
		return "", fmt.Errorf("unexpected message style")
	})
	if err != nil {
		return fmt.Errorf("failed to start PAM transaction: %w", err)
	}
	defer transaction.End()

	if err := transaction.Authenticate(0); err != nil {
		return fmt.Errorf("authentication failed: %w", err)
	}

	if err := transaction.AcctMgmt(0); err != nil {
		return fmt.Errorf("account management failed: %w", err)
	}

	fmt.Println("User authenticated successfully")
	return nil
}

// writeToUnixSocket writes data to a Unix socket.
func writeToUnixSocket(socketPath string, data string) error {
	conn, err := net.Dial("unix", socketPath)
	if err != nil {
		return fmt.Errorf("failed to connect to Unix socket: %w", err)
	}
	defer conn.Close()

	_, err = conn.Write([]byte(data))
	if err != nil {
		return fmt.Errorf("failed to write data to Unix socket: %w", err)
	}

	fmt.Println("Data written to Unix socket successfully")
	return nil
}

func main() {
	// Replace with actual username and password
	username := "testuser"
	password := "testpassword"

	// Authenticate the user
	if err := authenticateUser(username, password); err != nil {
		fmt.Fprintf(os.Stderr, "Authentication error: %v\n", err)
		os.Exit(1)
	}

	// Replace with actual Unix socket path and data to write
	socketPath := "/tmp/example.sock"
	data := "Hello, Unix socket!"

	// Write data to the Unix socket
	if err := writeToUnixSocket(socketPath, data); err != nil {
		fmt.Fprintf(os.Stderr, "Error writing to Unix socket: %v\n", err)
		os.Exit(1)
	}
}
