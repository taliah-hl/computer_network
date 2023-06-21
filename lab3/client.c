#include "header.h"

int main() {
    /*---------------------------UDT SERVER----------------------------------*/
    srand(getpid());
    // Create socket.
    int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        printf("Create socket fail!\n");
        return -1;
    }

    // Set up server's address.
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = inet_addr("127.0.0.1"),
        .sin_port = htons(45525)
    };
    int server_len = sizeof(serverAddr);

    // Connect to server's socket.
    if (connect(socket_fd, (struct sockaddr*)&serverAddr, server_len) == -1) {
        printf("Connect server failed!\n");
        close(socket_fd);
        exit(0);
    }
    /*---------------------------UDT SERVER-----------------------------------*/

    /*---------------------------3 way hand shake-----------------------------*/
    char header[20] = {0}; // Initialize header to all zeros

    // Set source and destination ports
    int srcPort = rand() % 65536; // Random source port number
    int destPort = 45525;
    memcpy(header, &srcPort, sizeof(srcPort));
    memcpy(header + sizeof(srcPort), &destPort, sizeof(destPort));

    // Set sequence number and ACK number
    int seqNum = 0;
    int ackNum = 0;
    memcpy(header + 8, &seqNum, sizeof(seqNum));
    memcpy(header + 12, &ackNum, sizeof(ackNum));

    // Set the SYN flag to initiate the handshake
    int flags = 0x02; // SYN flag
    memcpy(header + 13, &flags, sizeof(flags));

    // Send the SYN packet to the server
    if (send(socket_fd, header, sizeof(header), 0) == -1) {
        printf("Failed to send SYN packet!\n");
        close(socket_fd);
        exit(0);
    }

    // Receive the SYN-ACK packet from the server
    char synAckHeader[20] = {0};
    if (recv(socket_fd, synAckHeader, sizeof(synAckHeader), 0) == -1) {
        printf("Failed to receive SYN-ACK packet!\n");
        close(socket_fd);
        exit(0);
    }

    // Extract the ACK number from the received packet
    int receivedAckNum;
    memcpy(&receivedAckNum, synAckHeader + 12, sizeof(receivedAckNum));

    // Set the ACK flag and ACK number to complete the handshake
    flags = 0x10; // ACK flag
    memcpy(header + 13, &flags, sizeof(flags));
    memcpy(header + 12, &receivedAckNum, sizeof(receivedAckNum));

    // Send the final ACK packet to the server
    if (send(socket_fd, header, sizeof(header), 0) == -1) {
        printf("Failed to send ACK packet!\n");
        close(socket_fd);
        exit(0);
    }
    /*---------------------------3 way hand shake-----------------------------*/

    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }

