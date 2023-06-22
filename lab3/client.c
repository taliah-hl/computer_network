#include "header.h"
#include <netinet/in.h>
#include <errno.h>



int recvpacket(int socket_fd, char* buffer, int buffer_len, Segment* segment) {
    // Receive packet from the socket
    ssize_t num_bytes = recv(socket_fd, buffer, buffer_len, 0);
    if (num_bytes == -1) {
        perror("recv");
        return -1;
    } else if (num_bytes == 0) {
        printf("received from server failed\n");
        return -1;
    }

    // Extract the received segment information
    memcpy(segment->header, buffer, sizeof(segment->header));
    memcpy(segment->payload, buffer + sizeof(segment->header), num_bytes - sizeof(segment->header));
    segment->p_len = num_bytes - sizeof(segment->header);

    // Extract the header fields from the segment
    memcpy(&(segment->l4info), segment->header, sizeof(segment->l4info));
    memcpy(&(segment->l3info), segment->header + sizeof(segment->l4info), sizeof(segment->l3info));
    printf("packet received from server\n");
    return 0;
}

int sendpacket(int socket_fd, char* buffer, int buffer_len, Segment* segment, int flag) {
   // Construct the segment header
  myheadercreator(segment);
  memcpy(buffer, segment->header, sizeof(segment->header));
    printf("client sending: %s \n", buffer);
    // Send the packet through the socket
    ssize_t num_bytes = send(socket_fd, buffer, buffer_len, 0);
    if (num_bytes == -1) {
        perror("send");
        return -1;
    }

    else{
        printf("sent SYN to server\n");
        return 0;
    }
}

void initS(Segment *sendS, uint16_t sPort, uint16_t Dport){
    sendS->l4info.SourcePort = sPort;
    sendS->l4info.DesPort = 45525;
    sendS->l4info.HeaderLen = 5;
    sendS->l4info.WindowSize = 65535;
    sendS->l4info.CheckSum = 0;
    sendS->p_len = 1000;

    // Set the pseudoheader and calculate the checksum
    strcpy(sendS->l3info.SourceIpv4, "127.0.0.1");
    strcpy(sendS->l3info.DesIpv4, "127.0.0.1");
    

    memcpy(sendS->pseudoheader, &sendS->l3info, sizeof(sendS->l3info));
    memcpy(sendS->pseudoheader + sizeof(sendS->l3info), &sendS->l4info, sizeof(sendS->l4info));

    sendS->l4info.CheckSum = mychecksum(sendS->pseudoheader, sizeof(sendS->pseudoheader));
    printf("init S finish\n");


}

void replyS(Segment* segment, uint32_t segNum, uint32_t ackNum, uint32_t flags) {
    segment->l4info.SeqNum = segNum;
    segment->l4info.AckNum = ackNum;
    segment->l4info.Flag = flags;
    
    printf("set reply S finish\n");
}


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

    // struct sockaddr_in clientAddr;

    // // Bind the client's socket to a specific port 
    // // if (bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) { 
    // //     perror("bind"); close(socket_fd); exit(0); 
    // // }

    // socklen_t clientAddrLen = sizeof(clientAddr);
    // if(getsockname(socket_fd, (struct sockaddr*)&clientAddr, &clientAddrLen)==-1){
    //     perror("getsockname");
    //     close(socket_fd);
    //     exit(0);
    // }

    // printf("client's port number: %d\n", ntohs(clientAddr.sin_port));
    /*---------------------------UDT SERVER-----------------------------------*/

    /*---------------------------3 way hand shake-----------------------------*/
    char o_buffer[20]={0};
    char i_buffer[1020]={0}; 
    uint32_t currentSeg, currentAck;
    uint16_t sPort, Dport;
    currentSeg = rand();
    currentAck = 0;
    sPort = (rand()%65535)+1;
    Dport =  SERVER_PORT; //45525
    printf("success start\n");
    Segment sendS;
    Segment recvS;
    initS(&sendS, sPort, Dport);
    replyS(&sendS, currentSeg, currentAck, SYN);

    
    //exit(0);
    //client sd SYN to server

    // char syn_msg[3]={SYN};
    // syn_msg = SYN;
    
    // memcpy(o_buffer,  &(sendS.l4info.Flag), sizeof(sendS.l4info.Flag));

    // if(send(socket_fd, o_buffer, sizeof(o_buffer), 0)<0){
    //     perror("failed to send SYN");
    //     return 1;
    // }

    // if(recv(socket_fd, i_buffer, sizeof(i_buffer), 0)<0){
    //     perror("failed to received SYNACK");
    //     return 1;
    // }

    // if(i_buffer != SYNACK){
    //     fprintf(stderr, "invalid SYNACK received");
    //     return 1;
    // }

    // //send ACK to server
    // strcpy(o_buffer, ACK);
    // if(send(socket_fd, o_buffer, sizeof(o_buffer), 0)<0){
    //     perror("failed to send ACK");
    //     return 1;
    // }




    if(sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS,  0)==-1){
        printf("Send SYN packet failed!\n");
        close(socket_fd);
        exit(0);
    }

    if(recvpacket(socket_fd, i_buffer, 1020, &recvS)==-1){
        printf("receive SYN-ACK packet failed!\n");
        close(socket_fd);
        exit(0);
    }

     currentSeg = recvS.l4info.AckNum;
     currentAck = recvS.l4info.SeqNum +1;
     replyS(&sendS, currentSeg, currentAck, ACK);

    if(sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS,  0)==-1){
        printf("send ACK packet failed!\n");
        close(socket_fd);
        exit(0);
    }

    printf("3 way hankshake established!");
    




//     /*---------------------------3 way hand shake-----------------------------*/

    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
    else{
        printf("image recieved\n");
    }

//         /*----------------------------receive data--------------------------------*/
    

fclose(file);
                                    
    /*----------------------------receive data--------------------------------*/

}
