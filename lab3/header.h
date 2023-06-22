#pragma once
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SERVER_PORT 45525
#define SYN 0X2
#define ACK 0X10 
#define SYNACK 0X12 
#define ACKPSH 0X18

typedef struct l4info{
    uint32_t SourcePort,DesPort,SeqNum,AckNum,HeaderLen,Flag,WindowSize,CheckSum;
}L4info;
typedef struct l3info{
    char SourceIpv4[16], DesIpv4[16];
    uint32_t protocol;
}L3info;
typedef struct Segment{
    char header[20];
    char pseudoheader[12];
    char payload[1000];
    int p_len;
    L3info l3info;
    L4info l4info;
}Segment;
uint16_t mychecksum(char* buffer, int packet_len);
void myheadercreator(Segment* s);

uint16_t mychecksum(char* buffer, int buffer_len){
    if((buffer_len)%2==1) buffer_len++;
    uint32_t checksum = 0;
    uint16_t* p = (uint16_t*)buffer;
    for(int i =0;i<(buffer_len/2);i++){    
        checksum += (*(p+i)); 
        while(checksum>0xffff){
            checksum = (checksum&0xffff) + (checksum>>16);
            
        }
    }
    
    checksum = (~(checksum))&0xffff;
    return (uint16_t)ntohs(checksum);
}


void myheadercreator(Segment* s){

    //print and check info
    unsigned int src_ip = ntohl(inet_addr(s->l3info.SourceIpv4));
    unsigned int  dst_ip = ntohl(inet_addr(s->l3info.DesIpv4));
    printf("dst ip: %x\t %x\n", (dst_ip & 0xffff0000)>>(4*4), dst_ip & 0x0000ffff);
    printf("source port: (decimal) %d\t (hex) %x\t", s->l4info.SourcePort, s->l4info.SourcePort );
   
     
    //  extract, seperate bit and calculate header  //

    //print to check if bit extraction is correct
   

    //calculate header 

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

    printf("flags:(decimal) %d\t(hex) %x %x\n", s->l4info.Flag, 20, s->l4info.Flag);

    char header_ch[20]= {
        ((s->l4info.SourcePort & 0xff00)>>(4*2)), (s->l4info.SourcePort & 0x00ff),
        ((s->l4info.DesPort & 0xff00)>>(4*2)) ,  (s->l4info.DesPort & 0x00ff),
        ((s->l4info.SeqNum &0xff000000)>>(4*6)), ((s->l4info.SeqNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.SeqNum &0x0000ff00)>>(4*2)), (s->l4info.SeqNum &0x000000ff),
        ((s->l4info.AckNum &0xff000000)>>(4*6)), ((s->l4info.AckNum &0x00ff0000)>>(4*4)), 
        ((s->l4info.AckNum &0x0000ff00)>>(4*2)), (s->l4info.AckNum &0x000000ff),
        0xa0, s->l4info.Flag,
        ((s->l4info.WindowSize & 0xff00) >> (4*2)), (s->l4info.WindowSize & 0x00ff), 
         ((s->l4info.CheckSum & 0xff00)>>8) , (s->l4info.CheckSum & 0x00ff) ,0, 0
        };


    bzero(s->header, 20);
    strcpy(s->header, header_ch);

    printf("s->pseudoheader: %s\n", s->pseudoheader);
    printf("header_ch: %s\n", header_ch);
    printf("s->header: %s\n", s->header);


}


