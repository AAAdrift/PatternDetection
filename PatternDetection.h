#include <pcap.h>
#include <stdio.h>

#ifndef PATTERNDETECTION_H

typedef struct { //ip头格式
	u_char version:4;
	u_char header_len:4;
	u_char tos:8;
	u_int16_t total_len:16;
	u_int16_t ident:16;
	u_char flags:8;
	u_char fragment:8;
	u_char ttl:8;
	u_char proto:8;
	u_int16_t checksum;
	u_char sourceIP[4];
	u_char destIP[4];
}IPHEADER;

// 定义用于保存重组后数据包的结构体
typedef struct POnepOnepacket {
    u_char src_ip[4];
    u_char dest_ip[4];
    char *packetcontent;
    int contentlen;
    int is_last_frag;
    int offset;
    u_int16_t id;
    int totallen;
    char src[10];
	char des[10];
} POnepOnepacket;

typedef struct ATTACKPATTERN{
	char attackdes[256];
	char patterncontent[5000];
	int patternlen;
	char src[10];
	char des[10];
	struct ATTACKPATTERN *next;
}ATTACKPATTERN;



#define PATTERNDETECTION_H
#endif

void output_alert_0(ATTACKPATTERN *pOnepattern,POnepOnepacket *pOnepacket,const struct pcap_pkthdr *header);



