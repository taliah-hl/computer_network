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
    
    int server_sock , client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[512];
    int bind_sock;

    //Create TCP socket.//
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0){
        perror("Create TCP socket failed");
        exit(1);
    }
    printf("Create TCP socket success\n");

    //Set up server's address.//
    char *ip="127.0.0.1";
    int port = 45525;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    


    //Bind socket to the address.//
    //bind()->bind addr to port number
    bind_sock = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_sock < 0){
        perror("Bind socket failed");
        exit(1);
    }
    printf("Bind socket, bound to the port number: %d \n", port);

    //Listening the socket.//
    listen(server_sock, 5); //listen to 5 clients???
    printf("listening..\n");
    // here waiting for client

    //Accept the connect request.//
    //accpet()
    while(1){   //infinite loop accpet connection from client
    addr_size= sizeof(client_addr);
    client_fd = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    printf("Client conntected\n");

    //Send message to client.//
    //--- TASK 1 here ---//
    bzero(buffer, 512);
    strcpy(buffer, "Hi, I'm server 110062361");
    send(client_fd, buffer, strlen(buffer),0);
    printf("Server sending: %s", buffer);

    //receive
    bzero(buffer, 512);
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("client: %s\n", buffer);
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("client: %s\n", buffer);

    serverfunction(client_fd);

    

    // //send back to client

    // bzero(buffer, 512);    //clear buffer

    // strcpy(buffer, "2nd message from server");
    // printf("server: %s\n", buffer);
    // send(client_fd, buffer, strlen(buffer),0);

    close(client_fd);
    printf("client disconnected\n");
    

    }

    

    return 0;


}
