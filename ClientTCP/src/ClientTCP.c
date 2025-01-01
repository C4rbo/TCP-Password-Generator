#if defined WIN32
#include <winsock.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <stdio.h>
#include "header.h"

#define NO_ERROR 0

// Function to cleanup Windows sockets
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Display the main menu with available options
void print_menu() {
    printf("\n=========== Password Generator ===========\n\n");
    printf("Format: <type> <length> (Example: 'n 8')\n");
    printf("Password types available:\n");
    printf("  n | Numeric (only digits)\n");
    printf("  a | Alphabetic (lowercase letters)\n");
    printf("  m | Mixed (letters and numbers)\n");
    printf("  s | Secure (uppercase, lowercase, numbers, symbols)\n");
    printf("Length must be between %d and %d characters\n", MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
    printf("Type 'q' to quit\n\n");
    fflush(stdout);
}

// Validate user input for password type and length
int validate_input(char type, int length) {
    // Check if the password type is valid
    if (type != 'n' && type != 'a' && type != 'm' && type != 's') {
        printf("Attention: invalid password type. You need to use the command: n, a, m, or s\n\n");
        fflush(stdout);
        return 0;
    }

    // Ensure password length is within the allowed range
    if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
        printf("Attention: password length must be between %d and %d\n\n",
               MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH);
        fflush(stdout);
        return 0;
    }

    return 1;  // Input is valid
}

int main(int argc, char *argv[]) {
    // Initialize Windows sockets if on Windows platform
#if defined WIN32
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (result != NO_ERROR) {
        printf("Error at WSAStartup()\n");
        fflush(stdout);
        return -1;
    }
#endif

    // Create TCP socket
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket < 0) {
        printf("Error creating socket\n");
        fflush(stdout);
        clearwinsock();
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);
    server_addr.sin_port = htons(DEFAULT_SERVER_PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Failed to connect to server\n");
        closesocket(client_socket);
        clearwinsock();
        return -1;
    }

    // Display the menu to the user
    char buffer[BUFFER_SIZE];
    print_menu();

    // Main loop for user interaction

    while (1) {
        printf("\n=========================================\n\n");
        printf("Insert the command to generate a password: ");
        fflush(stdout);

        // Read user input
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Handle quit command
        if (buffer[0] == 'q') {
            printf("Quitting...\n");
            fflush(stdout);
            break;
        }

        // Parse user input (type and length)
        char type;
        int length;
        if (sscanf(buffer, "%c %d", &type, &length) != 2) {
            printf("Attention: invalid input format. Use <type> <length> (Example:'n 8')\n");
            fflush(stdout);
            continue;
        }

        // Validate input
        if (!validate_input(type, length)) {
            continue;
        }

        // Send the user's request to the server
        send(client_socket, buffer, strlen(buffer), 0);

        // Wait for the server's response
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            printf("Server disconnected...\n");
            fflush(stdout);
            break;
        }

        // Display generated password
        printf("%s\n", buffer);
        fflush(stdout);
    }

    // Cleanup and exit
    closesocket(client_socket);
    clearwinsock();
    return 0;
}
