#pragma once
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

typedef struct l4info{
    uint32_t AckNum,SeqNum,SourcePort,DesPort,Flag,HeaderLen,WindowSize;
}L4info;

typedef struct l3info{
    char SourceIpv4[16], DesIpv4[16];
    uint32_t protocol;
}L3info;

typedef struct Segment{
    char header[20];
    char pseudoheader[12];
    L3info l3info;
    L4info l4info;
}Segment;


void serverfunction(int clientfd);
void receivedata(int sockfd,Segment* s);
void sendheader(int sockfd, char* header);
void myheadercreator(Segment* s){

    //print and check info
    unsigned int src_ip = ntohl(inet_addr(s->l3info.SourceIpv4));
    unsigned int  dst_ip = ntohl(inet_addr(s->l3info.DesIpv4));


    printf("\n//  print and check header  //\n");
    
    printf("dst ip: %x\t %x\n", (dst_ip & 0xffff0000)>>(4*4), dst_ip & 0x0000ffff);
    printf("source port: (decimal) %d\t (hex) %x\t", s->l4info.SourcePort, s->l4info.SourcePort );
    printf("%x %x\n", ((s->l4info.SourcePort & 0xff00)>>(4*2)) ,  (s->l4info.SourcePort & 0x00ff));
    printf("dest port: (decimal) %d\t(hex) %x", s->l4info.DesPort, s->l4info.DesPort );
    printf("%x %x\n", ((s->l4info.DesPort & 0xff00)>>(4*2)) ,  (s->l4info.DesPort & 0x00ff));
    printf("Seq:(decimal) %d\t(hex) %x %x %x %x\n", s->l4info.SeqNum, 
        ((s->l4info.SeqNum &0xff000000)>>(4*6)), ((s->l4info.SeqNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.SeqNum &0x0000ff00)>>(4*2)), (s->l4info.SeqNum &0x000000ff));
    printf("ACK:(decimal) %d\t(hex) %x %x %x %x\n", s->l4info.AckNum, 
        ((s->l4info.AckNum &0xff000000)>>(4*6)), ((s->l4info.AckNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.AckNum &0x0000ff00)>>(4*2)), (s->l4info.AckNum &0x000000ff));
    printf("flags:(decimal) %d\t(hex) %x %x\n", s->l4info.Flag, 20, s->l4info.Flag);
    printf("window size:(decimal) %d\t(hex) %x %x\n", s->l4info.WindowSize,
        ((s->l4info.WindowSize & 0xff00)>>(4*2)), (s->l4info.WindowSize& 0x00ff));
    
   

    
     
    //  extract, seperate bit and calculate header  //

    //print to check if bit extraction is correct
    printf("\n/// print extracted bit ///\n");
    printf("src ip: %x\t", src_ip);
    printf("src ip: %x %x\n", (src_ip & 0xffff0000)>>(4*4), src_ip & 0x0000ffff);
    printf("dest ip: %x\t", dst_ip);
    printf("dst ip: %x %x\n", (dst_ip & 0xffff0000)>>(4*4), dst_ip & 0x0000ffff);

    //calculate header 

    
    // uint16_t* ptr = header_ch;
    // unsigned int tcp_header_sum = 0;
    // for(int i=0;i<16;i++){

    //     printf("ptr: %u %hn\n" ,ptr, ptr);
    //     tcp_header_sum += (unsigned int ) (*ptr);
    //     printf("tcp sum: %X %X" ,tcp_header_sum, tcp_header_sum);
    //     ptr++;
    // }

   

    unsigned int  pseudo_header_val = (
        ((src_ip & 0xffff0000)>>(4*4) ) + (src_ip & 0x0000ffff) 
        + ((dst_ip & 0xffff0000)>>(4*4)) + (dst_ip & 0x0000ffff)  + 6 +20 );
    
    unsigned int tcp_header_val =  (
        s->l4info.SourcePort + s->l4info.DesPort
        + ((s->l4info.SeqNum & 0xffff0000)>>(4*4)) + (s->l4info.SeqNum & 0x0000ffff) 
        + ((s->l4info.AckNum & 0xffff0000)>>(4*4)) + (s->l4info.AckNum & 0x0000ffff) 
       + ((s->l4info.HeaderLen<<12) | s->l4info.Flag)
        + s->l4info.WindowSize);

     unsigned int sum = pseudo_header_val + tcp_header_val;
     unsigned int carry = (sum & 0xff0000)>>(4*4);
    
    unsigned int endround = carry + (sum & 0x000ffff);
    unsigned int  checksum = endround ^ 0xffff;

    //printf to check
    printf("pseudo_header_val: %x\n" , pseudo_header_val);
    printf("tcp_header_val: %x \n" , tcp_header_val);
    printf("sum: %x\n" , sum);
    printf("carry: %x\n" , carry);
    printf("end-round carry: %x + %x = %x\n" ,carry, (sum & 0x00ffff), endround);
    printf("after 1's complement: %x\n" , checksum);
  

    char header_ch[20]= {
        ((s->l4info.SourcePort & 0xff00)>>(4*2)), (s->l4info.SourcePort & 0x00ff),
        ((s->l4info.DesPort & 0xff00)>>(4*2)) ,  (s->l4info.DesPort & 0x00ff),
        ((s->l4info.SeqNum &0xff000000)>>(4*6)), ((s->l4info.SeqNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.SeqNum &0x0000ff00)>>(4*2)), (s->l4info.SeqNum &0x000000ff),
        ((s->l4info.AckNum &0xff000000)>>(4*6)), ((s->l4info.AckNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.AckNum &0x0000ff00)>>(4*2)), (s->l4info.AckNum &0x000000ff),
        0x50, s->l4info.Flag,
        ((s->l4info.WindowSize & 0xff00) >> (4*2)), (s->l4info.WindowSize & 0x00ff), 
        ((checksum & 0xff00)>>8) , (checksum & 0x00ff) ,0, 0
        };

    
    
    

    bzero(s->header, 20);
    strcpy(s->header, header_ch);

    printf("s->pseudoheader: %s\n", s->pseudoheader);
    printf("header_ch: %s\n", header_ch);
    printf("s->header: %s\n", s->header);


}
