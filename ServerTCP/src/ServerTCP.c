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
#include <stdlib.h>
#include <time.h>
#include "header.h"

#define NO_ERROR 0

// Function to cleanup Windows sockets
void clearwinsock() {
#if defined WIN32
    WSACleanup();
#endif
}

// Generate numeric password (0-9)
void generate_numeric(char* password, int length) {
    const char charset[] = "0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 10];
    }
    password[length] = '\0';
}

// Generate alphabetic password (a-z)
void generate_alpha(char* password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 26];
    }
    password[length] = '\0';
}

// Generate mixed password (a-z, 0-9)
void generate_mixed(char* password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 36];
    }
    password[length] = '\0';
}

// Generate secure password (a-z, A-Z, 0-9, symbols)
void generate_secure(char* password, int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    for (int i = 0; i < length; i++) {
        password[i] = charset[rand() % 70];
    }
    password[length] = '\0';
}

// Handle individual client connections
void handleClient(int client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char password[MAX_PASSWORD_LENGTH + 1];
    char type;
    int length;
    int request_count = 0;

    while (1) {
        // Receive client request
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        // Check for client disconnection
        if (bytes_received <= 0) {
            printf("Client %s:%d disconnected\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));
            fflush(stdout);
            break;
        }

        // Parse client request
        if (sscanf(buffer, "%c %d", &type, &length) != 2) {
            continue;
        }

        // Validate password length
        if (length < MIN_PASSWORD_LENGTH || length > MAX_PASSWORD_LENGTH) {
            continue;
        }

        // Generate password based on requested type
        switch (type) {
            case 'n':
                generate_numeric(password, length);
                printf("\nClient %s:%d requested numeric password: %s\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       password);
                break;
            case 'a':
                generate_alpha(password, length);
                printf("\nClient %s:%d requested alphabetic password: %s\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       password);
                break;
            case 'm':
                generate_mixed(password, length);
                printf("\nClient %s:%d requested mixed password: %s\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       password);
                break;
            case 's':
                generate_secure(password, length);
                printf("\nClient %s:%d requested secure password: %s\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       password);
                break;
            default:
                continue;
        }
        fflush(stdout);

        // Send generated password to client
        send(client_socket, password, strlen(password), 0);
        request_count++;
    }
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

    // Initialize random number generator
    srand(time(NULL));

    // Create TCP socket
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        printf("Error creating socket\n");
        clearwinsock();
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(DEFAULT_SERVER_PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        closesocket(server_socket);
        clearwinsock();
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_socket, QLEN) < 0) {
        printf("Listen failed\n");
        closesocket(server_socket);
        clearwinsock();
        return -1;
    }

    printf("Waiting for connections...\n\n");
    fflush(stdout);

    // Main server loop
    while (1) {
        // Accept new client connection
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            printf("Accept failed\n");
            fflush(stdout);
            continue;
        }

        // Log new connection
        printf("New connection from %s:%d\n\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));
        fflush(stdout);

        // Handle client requests
        handleClient(client_socket, client_addr);
        closesocket(client_socket);
    }

    // Cleanup and exit
    closesocket(server_socket);
    clearwinsock();
    return 0;
}
