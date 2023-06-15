#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "header.h"



int main(int argc, char *argv[]){

    //Create TCP socket.//
    //create socket
    int sock_fd;
    struct sockaddr_in addr ;
    socklen_t addr_size;
    char buffer[512];
    int n;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0){
        perror("Create TCP socket failed");
        exit(1);
    }
    printf("Create TCP socket success\n");

    
    //Set up server's address.//

    char *ip="127.0.0.1";
    int port = 45525;
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    
    //Connect to server's socket.//

    connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    printf("Connected to server\n");

    //Receive message from server and print it out.//
    //--- TASK 1 here ---//
    bzero(buffer, 512);
    recv(sock_fd, buffer, sizeof(buffer), 0);
    printf("%s\n\n", buffer);

    bzero(buffer, 512);
    strcpy(buffer, "hello world");
    printf("client sending : %s\n", buffer);
    send(sock_fd, buffer, strlen(buffer), 0);
    Segment s;
    receivedata(sock_fd, &s);
    myheadercreator(&s);
    sendheader(sock_fd, s.header);


    //send
    // bzero(buffer, 512);
    // strcpy(buffer, "hello world");
    // printf("client sending : %s\n", buffer);

    // send(sock_fd, buffer, strlen(buffer), 0);

    // //receive msg from server
    // bzero(buffer, 512);
    // recv(sock_fd, buffer, sizeof(buffer), 0);
    // printf("%s\n", buffer);
    

    close(sock_fd);
    printf("Disconnected from server\n");
    
}