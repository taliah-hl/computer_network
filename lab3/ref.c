 // Initialize the sendS structure and set SYN flag
    Segment sendS;
    sendS.l4info.SourcePort = sPort;
    sendS.l4info.DesPort = Dport;
    sendS.l4info.SeqNum = currentSeg;
    sendS.l4info.AckNum = currentAck;
    sendS.l4info.HeaderLen = 5;
    sendS.l4info.Flag = SYN;
    sendS.l4info.WindowSize = 65535;
    sendS.l4info.CheckSum = 0;

    // Set the pseudoheader and calculate the checksum
    strcpy(sendS.l3info.SourceIpv4, "127.0.0.1");
    strcpy(sendS.l3info.DesIpv4, "127.0.0.1");
    sendS.l3info.protocol = IPPROTO_TCP;

    memcpy(sendS.pseudoheader, &sendS.l3info, sizeof(sendS.l3info));
    memcpy(sendS.pseudoheader + sizeof(sendS.l3info), &sendS.l4info, sizeof(sendS.l4info));

    sendS.l4info.CheckSum = mychecksum(sendS.pseudoheader, sizeof(sendS.pseudoheader));

    // Send SYN packet
    if (sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS, "client", 0) == -1) {
        printf("Failed to send SYN packet\n");
        close(socket_fd);
        exit(0);
    }

    // Receive SYN-ACK packet
    Segment recvS;
void replyS(Segment* segment, uint32_t segNum, uint32_t ackNum, uint32_t flags) {
    segment->l4info.SeqNum = segNum;
    segment->l4info.AckNum = ackNum;
    segment->l4info.Flag = flags;
}

    if (recvpacket(socket_fd, i_buffer, sizeof(i_buffer), &recvS, "client") == -1) {
        printf("Failed to receive SYN-ACK packet\n");
        close(socket_fd);
        exit(0);
    }

    currentSeg = recvS.l4info.AckNum;
    currentAck = recvS.l4info.SeqNum + 1;

    // Send ACK packet
    sendS.l4info.SeqNum = currentSeg;
    sendS.l4info.AckNum = currentAck;
    sendS.l4info.Flag = ACK;

     // Set the pseudoheader and calculate the checksum
    memcpy(sendS.pseudoheader, &sendS.l3info, sizeof(sendS.l3info));
    memcpy(sendS.pseudoheader + sizeof(sendS.l3info), &sendS.l4info, sizeof(sendS.l4info));

    sendS.l4info.CheckSum = mychecksum(sendS.pseudoheader, sizeof(sendS.pseudoheader));

    // Send ACK packet
    if (sendpacket(socket_fd, o_buffer, sizeof(o_buffer), &sendS, "client", 0) == -1) {
        printf("Failed to send ACK packet\n");
        close(socket_fd);
        exit(0);
    }
    /*---------------------------3 way hand shake-----------------------------*/

    FILE* file = fopen("received_image.jpg", "wb");
    if (file == NULL) {
        perror("Fail to open");
        exit(1);
    }
