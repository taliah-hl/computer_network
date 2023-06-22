#include "header.h"
#include <netinet/in.h>

void initS(Segment *sendS, uint16_t sPort, uint16_t Dport,uint32_t currentSeg, uint32_t currentAck){
    sendS->l4info.SourcePort = sPort;
    sendS->l4info.DesPort = Dport;
    sendS->l4info.SeqNum = currentSeg;
    sendS->l4info.AckNum = currentAck;
    sendS->l4info.HeaderLen = 5;
    sendS->l4info.Flag = SYN;
    sendS->l4info.WindowSize = 65535;
    sendS->l4info.CheckSum = 0;

    // Set the pseudoheader and calculate the checksum
    strcpy(sendS->l3info.SourceIpv4, "127.0.0.1");
    strcpy(sendS->l3info.DesIpv4, "127.0.0.1");
    sendS->l3info.protocol = IPPROTO_TCP;

    memcpy(sendS->pseudoheader, &sendS->l3info, sizeof(sendS->l3info));
    memcpy(sendS->pseudoheader + sizeof(sendS->l3info), &sendS->l4info, sizeof(sendS->l4info));

    sendS->l4info.CheckSum = mychecksum(sendS->pseudoheader, sizeof(sendS->pseudoheader));


}

void replyS(Segment* segment, uint32_t segNum, uint32_t ackNum, uint32_t flags) {
    segment->l4info.SeqNum = segNum;
    segment->l4info.AckNum = ackNum;
    segment->l4info.Flag = flags;
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
    /*---------------------------UDT SERVER-----------------------------------*/

    /*---------------------------3 way hand shake-----------------------------*/
    char o_buffer[20]={0};
    char i_buffer[1020]={0}; 
    uint32_t currentSeg, currentAck;
    uint16_t sPort, Dport;
    currentSeg = rand();
    currentAck = 0;
    sPort = (rand()%65535)+1;
    Dport =  45525;
    Segment sendS;
    initS(&sendS, sPort, Dport, currentSeg, currentAck);

    //send to server
    
    if(sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS, "client", 0)==-1){
        printf("Failed to send SYN packet\n");
        close(socket_fd);
        exit(0);
    }

    //start reply
    Segment recvS;

    replyS(&sendS, currentSeg, currentAck, SYN);
    if(recvpacket(socket_fd, i_buffer, sizeof(i_buffer), &recvS, "client")==-1){
        printf("Failed to receive SYN-ACK packet\n");
        close(socket_fd);
        exit(0);
    }
    currentSeg = recvS.l4info.AckNum;
    currentAck = recvS.l4info.SeqNum +1;

    replyS(&sendS, currentSeg, currentAck, ACK);
    if(recvpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS, "client")==-1){
        printf("Failed to receive SYN-ACK packet\n");
        close(socket_fd);
        exit(0);
    }

    


    /*---------------------------3 way hand shake-----------------------------*/

    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }

        /*----------------------------receive data--------------------------------*/
    while (1) {
        // Receive packet
        memset(i_buffer, 0, sizeof(i_buffer));
        if (recvpacket(socket_fd, i_buffer, sizeof(i_buffer), &recvS, "client") == -1) {
            printf("Failed to receive data packet\n");
            close(socket_fd);
            fclose(file);
            exit(0);
        }
    
        // Check for end of transmission
        if (recvS.l4info.Flag == 1) {
            printf("Received FIN packet. Transmission complete.\n");
            break;
        }
    
        // Verify checksum
        memcpy(sendS.pseudoheader, &recvS.l3info, sizeof(recvS.l3info));
        memcpy(sendS.pseudoheader + sizeof(recvS.l3info), &recvS.l4info, sizeof(recvS.l4info));
        if (mychecksum(sendS.pseudoheader, sizeof(sendS.pseudoheader)) != 0) {
            printf("Corrupted packet received. Discarding...\n");
            continue;
        }
    
        // Send ACK
        replyS(&sendS, recvS.l4info.AckNum, recvS.l4info.SeqNum + 1000, ACK);
        if (sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS, "client", 0) == -1) {
            printf("Failed to send ACK packet\n");
            close(socket_fd);
            fclose(file);
            exit(0);
        }

    // Write payload to file
    fwrite(recvS.payload, sizeof(char), recvS.l4info.WindowSize - 20, file);
}

fclose(file);
                                    
    /*----------------------------receive data--------------------------------*/

}
