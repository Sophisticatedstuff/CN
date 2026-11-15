#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_SIZE 100

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    int N;
    printf("Enter the order N of the square matrix (max %d): ", MAX_SIZE);
    scanf("%d", &N);

    double matrix[MAX_SIZE][MAX_SIZE];
    printf("Enter the elements for the %dx%d matrix row by row:\n", N, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("Element [%d][%d]: ", i, j);
            scanf("%lf", &matrix[i][j]);
        }
    }

    // Send N to server
    send(sock, &N, sizeof(N), 0);

    // Send matrix rows to server
    for (int i = 0; i < N; i++) {
        send(sock, matrix[i], N * sizeof(double), 0);
    }

    // Receive response from server
    char buffer[100] = {0};
    read(sock, buffer, 100);
    printf("\n[CLIENT] Matrix Type received from Server: %s\n", buffer);

    close(sock);
    return 0;
}
