#include "header.h"

void initSegment(Segment *sendS, , uint32_t currentSeg, uint32_t currentAck){
    sendS->l4info.SourcePort = sPort;
    sendS->l4info.DesPort = Dport;
    sendS->l4info.SeqNum = currentSeg;
    sendS->l4info.AckNum = currentAck;
    sendS->l4info.HeaderLen = 5;
    sendS->l4info.Flag = SYN;
    sendS->l4info.WindowSize = 65535;
    sendS->l4info.CheckSum = 0;

    // Set the pseudoheader and calculate the checksum
    strcpy(sendS.l3info.SourceIpv4, "127.0.0.1");
    strcpy(sendS.l3info.DesIpv4, "127.0.0.1");
    sendS.l3info.protocol = IPPROTO_TCP;

    memcpy(sendS.pseudoheader, &sendS.l3info, sizeof(sendS.l3info));
    memcpy(sendS.pseudoheader + sizeof(sendS.l3info), &sendS.l4info, sizeof(sendS.l4info));

    sendS.l4info.CheckSum = mychecksum(sendS.pseudoheader, sizeof(sendS.pseudoheader));


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
    currentSeg = rand[];
    currentAck = 0;
    sPort = (rand()%65535)+1;
    Dport =  45525;
    Segment sendS;
    initS(&sendS, sPort, Dport, currentSeg, currentAck);
    
    if(sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS. "client", 0)==-1){
        printf("Failed to send SYN packet\n");
        close(socket_fd);
        exit(0);
    }
    Segment 

    replyS(&sendS, currentSeg, currentAck, SYN);
    if(recvpacket(socket_fd, i_buffer, sizeof(i_buffer), &recvS. "client")==-1){
        printf("Failed to receive SYN-ACK packet\n");
        close(socket_fd);
        exit(0);
    }
    currentSeg = recvS.l4info.AckNum;
    currentAck = recvS.l4info.SeqNum +1;

    //not sure if this part completed

    



    /*---------------------------3 way hand shake-----------------------------*/

    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }

    /*----------------------------receive data--------------------------------*/
    /*                                                                        */                                              
    /* TODO: Receive data from the RDT server.                                */
    /*       Each packet will be 20bytes TCP header + 1000bytes paylaod       */
    /*       exclude the last one. (the payload may not be exactly 1000bytes) */
    /*                                                                        */
    /* TODO: Once you receive the packets, you should check whether it's      */                                                            
    /*       corrupt or not (checksum) , and send the corresponding ack       */                                                  
    /*       packet (also a char[20] ) back to the server.                    */
    /*                                                                        */
    /* TODO: fwrite the payload into a .jpg file, and check the picture.      */
    /*                                                                        */                                              
    /*----------------------------receive data--------------------------------*/



    /*-------------------------Something important----------------------------*/
    /* NOTE: TO make lab3 simple                                              */
    /*                                                                        */                                              
    /*       1. The SrcIP and DesIP are both 127.0.0.1,                       */
    /*          Tcp header length will be 20byts, windowsize = 65535 bytes    */                                              
    /*       2. The Handshake packets won't be corrupt.                       */
    /*       3. The packet will only corrupt but not miss or be disordered.   */                                              
    /*       4. Only the packets come from server may corrupt.(don't have to  */
    /*          worry that the ack sent by client will corrupt.)              */
    /*       5. We offer mychecksum() for you to verify the checksum, and     */
    /*          don't forget to verify pseudoheader part.                     */
    /*       6. Once server finish transmit the file, it will close the       */
    /*          client socket.                                                */                                              
    /*       7. You can adjust server by                                      */                                              
    /*          ./server {timeout duration} {corrupt probability}             */                                              
    /*                                                                        */                                              
    /*-------------------------Something important----------------------------*/

}
