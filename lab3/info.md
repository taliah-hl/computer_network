// Bind the client's socket to a specific port
if (bind(socket_fd, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == -1) {
    perror("bind");
    close(socket_fd);
    exit(0);
}
