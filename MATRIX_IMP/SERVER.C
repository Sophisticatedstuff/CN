#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_SIZE 100

void check_matrix_type(double matrix[MAX_SIZE][MAX_SIZE], int N, char *result_buf) {
    int is_diagonal = 1;
    int is_upper = 1;
    int is_lower = 1;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double val = matrix[i][j];
            // Check for Diagonal
            if (i != j && val != 0.0) {
                is_diagonal = 0;
            }
            // Check for Upper Triangular (elements below main diagonal must be 0)
            if (i > j && val != 0.0) {
                is_upper = 0;
            }
            // Check for Lower Triangular (elements above main diagonal must be 0)
            if (i < j && val != 0.0) {
                is_lower = 0;
            }
        }
    }

    if (is_diagonal) {
        strcpy(result_buf, "Diagonal Matrix");
    } else if (is_upper) {
        strcpy(result_buf, "Upper Triangular Matrix");
    } else if (is_lower) {
        strcpy(result_buf, "Lower Triangular Matrix");
    } else {
        strcpy(result_buf, "General Matrix");
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Server is listening on port %d...\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("[SERVER] Client connected.\n");

    int N;
    // Receive N
    read(new_socket, &N, sizeof(N));

    double matrix[MAX_SIZE][MAX_SIZE];
    // Receive matrix elements row by row
    for (int i = 0; i < N; i++) {
        read(new_socket, matrix[i], N * sizeof(double));
    }

    printf("[SERVER] Received %dx%d matrix from client:\n", N, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%.1f ", matrix[i][j]);
        }
        printf("\n");
    }

    // Identify matrix type
    char response[100];
    check_matrix_type(matrix, N, response);
    printf("[SERVER] Identified type: %s\n", response);

    // Send result back to client
    send(new_socket, response, strlen(response) + 1, 0);

    close(new_socket);
    close(server_fd);
    return 0;
}
