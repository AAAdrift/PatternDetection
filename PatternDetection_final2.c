#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "acsm.h"
#include <sys/time.h>
#include <mysql/mysql.h>
#include <limits.h>
#include "bm_alg.h"
#include "PatternDetection.h"


#define MAX_FRAG 1024
POnepOnepacket packets[MAX_FRAG];
int frag_num = 0;



ATTACKPATTERN* pPatternHeader;//全局变量，保存攻击模式链表头
int minpattern_len;    //最短模式的长度

pcap_t *phandle;

// 声明二维数组指针
extern int **suffix;
extern bool **prefix;
extern int **modelStrIndex;


extern acsm_context_t  *ctx;
void acsm_free(acsm_context_t *ctx);

int choose_alg;
int rules;

bool cvt_path(char *src, char *dest) {
	char *pos = strchr(src, '/');
	if(pos == NULL) return false;
	
}
int hex_char_to_int(char c) {
	if (c >= '0' && c <=  '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c -'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
}
char *url_decode(char *src_string) {
	if(strlen(src_string) < 3) return src_string;
	
	char *ret = (char *)malloc(strlen(src_string) + 1);
	char *j = ret;
	for(int i = 0; i < (strlen(src_string) - 2); ++i) {
		if(src_string[i] == '%') {
			*j = (char)((hex_char_to_int(src_string[i + 1]) << 4) | hex_char_to_int(src_string[i + 2]));
			j++;
			i += 2;
		} else {
			*j = src_string[i];
			j++;
		}
	}
	// printf("ret = %s\n", ret);
	for(int i = strlen(src_string) - 2; i < strlen(src_string); ++i) {
		*j = src_string[i];
		j++;
	}
	// printf("\n原始长度为：%d\n现在长度为:%d\n\n", strlen(src_string), j - ret);
	*j = '\0';
	// printf("现在长度为：%d\n",strlen(ret));
	// printf("\n解码结果：\n%s\n\n",ret);
	return ret;
}
// 重组数据包
int reassemble_packet(POnepOnepacket *pkt, u_int16_t id) {
    // printf("Reassembling...\n");
    int total_length = 0;
    char *content = NULL;
    // 找到所有ID匹配的分片并重组
    for (size_t i = 0; i <= frag_num; ++i) {
        if (packets[i].id == id && packets[i].packetcontent != NULL) {
			// printf("Matched!\n");
			// printf("新增长度为%d\n内容为%s\n", strlen(packets[i].packetcontent), packets[i].packetcontent);

            total_length += strlen(packets[i].packetcontent);
            // 填充POnepOnepacket结构体
			for(int j = 0; j <= 3; ++ j) {
				pkt->src_ip[j] = packets[i].src_ip[j];
				pkt->dest_ip[j] = packets[i].dest_ip[j];
			}
        //     printf("reassemble: Source IP: %u.%u.%u.%u, Destination IP: %u.%u.%u.%u\n", pkt->src_ip[0], pkt->src_ip[1], pkt->src_ip[2], pkt->src_ip[3],
        // pkt->dest_ip[0], pkt->dest_ip[1], pkt->dest_ip[2], pkt->dest_ip[3]);
        }
    }
    
	if(total_length <= 0) return 0;
    content = malloc(total_length + 1);
	
    if (content == NULL) {
        printf("Memory allocation failed\n");
        return 0;
    }
	char *j = content;
    for (size_t i = 0; i <= frag_num; ++i) {
        if (packets[i].id == id && packets[i].packetcontent != NULL) {
			memcpy(j, packets[i].packetcontent, strlen(packets[i].packetcontent));
			j +=  strlen(packets[i].packetcontent);
			// printf("i = %d, Yes\n", i);
            // memcpy(content + total_length, packets[i].packetcontent, strlen(packets[i].packetcontent));
        }
    }
    pkt->totallen = total_length;
    pkt->contentlen = strlen(content);
	pkt->packetcontent = malloc(strlen(content) + 1);
    memcpy(pkt->packetcontent, content, strlen(content));
    frag_num = 0;
    
	return 1;
}

int parse_para(int argc,char*argv[],char *filename){
	if(argc != 2) {
		printf("Usage %s : patternfile \n",argv[0]);
		return 1;
	}else{
		bzero(filename, 256);
		strncpy(filename,argv[1],255);
  		return 0;
	}
}

int readpattern(char *patternfile){
	FILE *file;
	char linebuffer[256];

	file = fopen(patternfile,"r");
	if ( file == NULL) {
		printf("Cann't open the pattern file! Please check it and try again! \n");
		return 1;
	}
	bzero(linebuffer,256);
	

	while(fgets(linebuffer,255,file)){
		ATTACKPATTERN *pOnepattern;
		int deslen;
		char *pchar;
		pchar = strchr(linebuffer,'#');
		if (pchar == NULL)
			continue;
		pOnepattern = (ATTACKPATTERN *)malloc(sizeof(ATTACKPATTERN) + 1);
		deslen = pchar - linebuffer;
		pOnepattern->patternlen = strlen(linebuffer) - deslen -1 -1 ;
		pchar ++;
		memcpy(pOnepattern->attackdes, linebuffer, deslen);
		memcpy(pOnepattern->patterncontent, pchar, pOnepattern->patternlen);
		memcpy(pOnepattern->src, "any", 3);
		memcpy(pOnepattern->des, "any", 3);
		if (pOnepattern->patternlen < minpattern_len)
			minpattern_len = pOnepattern->patternlen;
		pOnepattern->next = NULL;

		if (pPatternHeader == NULL)
			pPatternHeader = pOnepattern;
		else{
			pOnepattern->next = pPatternHeader;
			pPatternHeader = pOnepattern;
		}
		bzero(linebuffer,256);
		
		//进行解码
		char *decoded_content = url_decode(pOnepattern->patterncontent);
		memcpy(pOnepattern->patterncontent, decoded_content, strlen(decoded_content));
		pOnepattern->patterncontent[strlen(decoded_content)] = '\0';
		//printf("特征串的解码结果为：%s\n", pOnepattern->patterncontent);
		pOnepattern->patternlen = strlen(pOnepattern->patterncontent);
	}
	
	if (pPatternHeader == NULL) 
		return 1;
	return 0;
}

int readpattern_sql(){
	MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    const char *server = "localhost";
    const char *user = "root";
    const char *password = "198929"; // 替换为你的密码
    const char *database = "ids_rules"; // 替换为你的数据库名

    conn = mysql_init(NULL);

	
    // 连接到 MySQL
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
        fprintf(stderr, "Error connecting to database: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    printf("Connected to database successfully\n");

    // 执行查询
    if (mysql_query(conn, "SELECT * FROM tcp_rules_2")) { // 替换为你的表名
        fprintf(stderr, "Error executing query: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    // 获取结果
    res = mysql_store_result(conn);

    // 打印查询结果
    while ((row = mysql_fetch_row(res)) != NULL) {
		ATTACKPATTERN *pOnepattern;
		pOnepattern = (ATTACKPATTERN *)malloc(sizeof(ATTACKPATTERN) + 1);

		pOnepattern->patternlen=strlen(row[1]);
		memcpy(pOnepattern->attackdes, row[0], strlen(row[0]));
		memcpy(pOnepattern->patterncontent, row[1], pOnepattern->patternlen);
		memcpy(pOnepattern->src, row[2], strlen(row[2]));
		memcpy(pOnepattern->des, row[3], strlen(row[3]));
		//添加源端口和目的端口

		if (pOnepattern->patternlen < minpattern_len)
			minpattern_len = pOnepattern->patternlen;
		pOnepattern->next = NULL;

		if (pPatternHeader == NULL)
			pPatternHeader = pOnepattern;
		else{
			pOnepattern->next = pPatternHeader;
			pPatternHeader = pOnepattern;
		}
	}

    // 清理
    mysql_free_result(res);
    mysql_close(conn);
	if (pPatternHeader == NULL) 
		return 1;

    return 0;
}




int matchpattern(ATTACKPATTERN *pOnepattern, POnepOnepacket *pOnepacket){
	// printf("匹配中...");
    // printf("待匹配内容：\n%s\n", pOnepacket->packetcontent);
	// printf("特征串%s\n", pOnepattern->patterncontent);
	int leftlen;
	char *leftcontent;
	
	leftcontent = pOnepacket -> packetcontent;
	leftlen = strlen(leftcontent);//pOnepacket-> contentlen;
    
	while(leftlen >= pOnepattern->patternlen){
		if (strncmp(leftcontent,pOnepattern->patterncontent,pOnepattern->patternlen) == 0) {
            // printf("匹配到字符串：%s\n", pOnepattern->patterncontent);
            return 1;
        }   
            
		leftlen --;
		leftcontent ++;
	}
	return 0;
}


void output_alert_0(ATTACKPATTERN *pOnepattern,POnepOnepacket *pOnepacket,const struct pcap_pkthdr *header)
{
	// 日志文件路径
    const char *logFilePath = "attack_log.txt";

    // 打开或创建日志文件
    FILE *logFile = fopen(logFilePath, "a"); // 使用 "a" 模式来追加写入
    if (logFile == NULL) {
        perror("Error opening log file");
        return;
    }

    // 写入日志文件

	fprintf(logFile, "[Time: %lld.%lld]\n",header->ts.tv_sec, header->ts.tv_usec);
    fprintf(logFile, "[%s]\t", pOnepattern->attackdes);
    fprintf(logFile, "%u.%u.%u.%u ==> ", pOnepacket->src_ip[0], pOnepacket->src_ip[1], pOnepacket->src_ip[2], pOnepacket->src_ip[3]);
    fprintf(logFile, "%u.%u.%u.%u\n", pOnepacket->dest_ip[0], pOnepacket->dest_ip[1], pOnepacket->dest_ip[2], pOnepacket->dest_ip[3]);
  	fprintf(logFile, "%s\n\n",pOnepacket->packetcontent);
	printf("\033[1;33m");
    printf("Intrusion Detected:\n     Type:  %s   ", pOnepattern->attackdes);
  	printf("%d.%d.%d.%d ==> ",pOnepacket->src_ip[0],pOnepacket->src_ip[1],pOnepacket->src_ip[2],pOnepacket->src_ip[3]);
  	printf("%d.%d.%d.%d\n",pOnepacket->dest_ip[0],pOnepacket->dest_ip[1],pOnepacket->dest_ip[2],pOnepacket->dest_ip[3]);
    printf("\033[0m");

	// 关闭日志文件
    fclose(logFile);
}









void help()
{
    printf("\033[1;33m");
    printf("-----------Welcome to superIPS !!!!!!! Type in h/m/q:-----------\n");
    printf("\033[1;34m");
	printf("h: help\n");
	printf("m: the working mode\n(0:Default Rulebase   1:add your own patternfile   2:use only your patternfile)\n");
	printf("q: quit\n");
    printf("\033[0m");
}

// libpcap回调函数
void pcap_callback(u_char *user, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	//printf("this\n");
    
    IPHEADER *ip_header;
	// POnepOnepacket onepacket;
	ATTACKPATTERN *pOnepattern;
  	bzero(&packets[frag_num],sizeof(POnepOnepacket));

  	if(header->len >= 14)
    	ip_header=(IPHEADER*)(pkt_data+14);
	else 
		return;
  	if(ip_header->proto == 6){
		
        packets[frag_num].totallen = ip_header->total_len;
		packets[frag_num].contentlen = ip_header->total_len - 20 - 20;
		if (packets[frag_num].contentlen < minpattern_len)
			return;
        packets[frag_num].packetcontent = (char *)(pkt_data + 14 + 20 + 20);
        
   		strncpy(packets[frag_num].src_ip,ip_header->sourceIP,4);
    	strncpy(packets[frag_num].dest_ip,ip_header->destIP,4);
        
        packets[frag_num].offset = ip_header->flags & 0x20;
        packets[frag_num].is_last_frag = (packets[frag_num].offset == 0);
        packets[frag_num].id = ip_header->ident;
        
        // printf("\n新数据包：\nSource IP: %u.%u.%u.%u, Destination IP: %u.%u.%u.%u\n", onepacket.src_ip[0], onepacket.src_ip[1], onepacket.src_ip[2], onepacket.src_ip[3],
        // onepacket.dest_ip[0], onepacket.dest_ip[1], onepacket.dest_ip[2], onepacket.dest_ip[3]);
               
        if(packets[frag_num].is_last_frag) {
            POnepOnepacket finalPack;
            if(!reassemble_packet(&finalPack, packets[frag_num].id)) return;
            //printf("已完成TCP报文重组。结果为:\n");
            //printf("%s\n\n", finalPack.packetcontent);

            //解码
			char *tmp = url_decode(finalPack.packetcontent);
			memcpy(finalPack.packetcontent, tmp, strlen(tmp));
			finalPack.packetcontent[strlen(tmp) + 1] = '\0';
			//printf("\n解码后的内容为：%s\n\n", finalPack.packetcontent);
			finalPack.contentlen = strlen(finalPack.packetcontent);
			finalPack.totallen = finalPack.contentlen + 40;

			if(choose_alg == 1) {
				//printf("111\n");
				//暴力匹配
				ATTACKPATTERN *pOnepattern = pPatternHeader;
				while(pOnepattern != NULL){
					if (matchpattern(pOnepattern, &finalPack)){
						output_alert_0(pOnepattern, &finalPack, header);
						
					}
					pOnepattern = pOnepattern->next;
				}
			} else if(choose_alg==2||(rules==2&&choose_alg==0)) {
				// BM算法匹配
				
				struct ip * ip_header2=(struct ip *)(pkt_data+14);

				struct tcphdr *tcp_header = (struct tcphdr *)(pkt_data + 14 + (ip_header2->ip_hl * 4));
				sprintf(finalPack.src, "%d", ntohs(tcp_header->th_sport));
				sprintf(finalPack.des, "%d", ntohs(tcp_header->th_dport));
				
				
				ATTACKPATTERN *pOnepattern1 = pPatternHeader;
				int i=0;
				while(pOnepattern1 != NULL){
					if (matchpattern_BM(pOnepattern1, &finalPack, i)){
						output_alert_0(pOnepattern1, &finalPack, header);
						//printf("%s\n",onepacket.packetcontent);
					}
					pOnepattern1 = pOnepattern1->next;
					i++;
				}
			} else {
				// AC自动机
				
				struct ip * ip_header2=(struct ip *)(pkt_data+14);

				struct tcphdr *tcp_header = (struct tcphdr *)(pkt_data + 14 + (ip_header2->ip_hl * 4));
				sprintf(finalPack.src, "%d", ntohs(tcp_header->th_sport));
				sprintf(finalPack.des, "%d", ntohs(tcp_header->th_dport));
				ATTACKPATTERN *pOnepattern = pPatternHeader;
				
				pOnepattern = pPatternHeader;
				if(pOnepattern != NULL){
					int match2=matchpattern_AC(&finalPack, header);
				}
			}
        } 
		// else {
        //     printf("\033[1;34m");
        //     printf("\nDetected New Fragment，ID = %d\n\n", packets[frag_num].id);
        //     printf("\033[0m");
        // }
		
    }
	frag_num ++;
	if (frag_num == MAX_FRAG) frag_num = 0;
}

int main(int argc,char *argv[])
{
    help();
	char c;
	
	do{
		scanf("%c",&c);
		if(c=='h')help();
		else if(c=='q')return 0;
		else if(c=='m'){
            printf("\033[1;33m");
            printf("Type in 0/1/2 to choose your IDS database:\n");
            printf("\033[1;34m");
			printf("0:Default Rulebase\n1:add your own patternfile\n2:use only your patternfile\n");
            
            printf("\033[0m");
			scanf("%d",&rules);
		}
	}while (c!='m');
	
  	char *device;
  	char errbuf[PCAP_ERRBUF_SIZE];
  	pcap_t *phandle;
  	bpf_u_int32 ipaddress, ipmask;
  	struct bpf_program fcode;

// 读取模式串
	pPatternHeader = NULL;
	minpattern_len = 1000;
    char patternfile[256];  //保存攻击模式文件名
	if(rules==1||rules==2){
        printf("\033[1;33m");
		printf("please input your file\n");
        printf("\033[0m");
		scanf("%s",&patternfile);
		if (readpattern(patternfile))
			exit(0);
	}

	if(rules==1||rules==0){
		if (readpattern_sql()) exit(0);
	}
		
	

// 初始化设备
  	if((device = pcap_lookupdev(errbuf)) == NULL) exit(0);                 //获得可用的网络设备名.
	if(pcap_lookupnet(device,&ipaddress,&ipmask,errbuf)==-1)    //获得ip和子网掩码
   	exit(0);
  	else {
    	char net[INET_ADDRSTRLEN],mask[INET_ADDRSTRLEN];
    	if(inet_ntop(AF_INET,&ipaddress,net,sizeof(net)) == NULL) 
			exit(0);
      else 
			if(inet_ntop(AF_INET,&ipmask,mask,sizeof(mask)) == NULL)	exit(0);
    	}
  	phandle = pcap_open_live(device,200,1,500,errbuf);                    //打开设备
	if(phandle == NULL)
	  	exit(0);

	if(pcap_compile(phandle,&fcode,"ip and tcp",0,ipmask) == -1) exit(0);         //设置过滤器，只捕获ip&tcp报头的包

	if(pcap_setfilter(phandle,&fcode) == -1)
	 	exit(0);


    printf("\033[1;33m");
	printf("Please choose the String Matching Algorithm\n0 for Default Algorithm, 1 for Brute Force, 2 for Boyer-Moore, 3 for AC Automation:\n");
    printf("\033[0m");

    scanf("%c",&c);
    scanf("%d",&choose_alg);

	if(choose_alg==2||(rules==2&&choose_alg==0)){
		init_BM(pPatternHeader);
	}
	else if(choose_alg==3||(rules!=2&&choose_alg==0)){
		init_AC();
	}
    

    printf("\033[1;34m");
 	printf("Start to detect intrusion........\n");  //此函数设置过滤器并开始进行数据包的捕捉
    printf("\033[0m");
  	pcap_loop(phandle,-1,pcap_callback,NULL); 

	if(choose_alg==3||(rules!=2&&choose_alg==0)) acsm_free(ctx);

	pcap_close(phandle); 
}
