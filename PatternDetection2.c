#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "acsm.h"
#include <sys/time.h>
#include <mysql.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>


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


typedef struct Packetinfo{
	u_char src_ip[4];
	u_char dest_ip[4];
	char *packetcontent;
	int contentlen;
}PACKETINFO;

typedef struct AttackPattern{
	char attackdes[256];
	char patterncontent[256];
	int patternlen;
	struct AttackPattern *next;
}ATTACKPATTERN;

typedef struct Packetinfo2{
	u_char src_ip[4];
	u_char dest_ip[4];
	char *packetcontent;
	int contentlen;
	char src[10];
	char des[10];
}PACKETINFO2;

typedef struct AttackPattern2{
	char attackdes[256];
	char patterncontent[256];
	int patternlen;
	char src[10];
	char des[10];
	struct AttackPattern *next;
}ATTACKPATTERN2;

ATTACKPATTERN* pPatternHeader;//全局变量，保存攻击模式链表头
int minpattern_len;    //最短模式的长度

ATTACKPATTERN2* pPatternHeader2;//全局变量，保存攻击模式链表头
int minpattern_len2;    //最短模式的长度

// 声明二维数组指针
int **suffix;
bool **prefix;
int **modelStrIndex;
acsm_context_t  *ctx;

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

int readpattern_sql(){
	MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    const char *server = "localhost";
    const char *user = "root";
    const char *password = "198929"; // 替换为你的密码
    const char *database = "ids_rules"; // 替换为你的数据库名

    conn = mysql_init(NULL);

	pPatternHeader2 = NULL;
	minpattern_len2 = 1000;
	
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
		ATTACKPATTERN2 *pOnepattern;
		pOnepattern = malloc(sizeof(ATTACKPATTERN2));

		pOnepattern->patternlen=strlen(row[1]);
		memcpy(pOnepattern->attackdes, row[0], strlen(row[0]));
		memcpy(pOnepattern->patterncontent, row[1], pOnepattern->patternlen);
		memcpy(pOnepattern->src, row[2], strlen(row[2]));
		memcpy(pOnepattern->des, row[3], strlen(row[3]));
		//添加源端口和目的端口

		if (pOnepattern->patternlen < minpattern_len2)
			minpattern_len2 = pOnepattern->patternlen;
		pOnepattern->next = NULL;

		if (pPatternHeader2 == NULL)
			pPatternHeader2 = pOnepattern;
		else{
			pOnepattern->next = pPatternHeader2;
			pPatternHeader2 = pOnepattern;
		}
	}

    // 清理
    mysql_free_result(res);
    mysql_close(conn);
	if (pPatternHeader2 == NULL) 
		return 1;

    return 0;
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
	pPatternHeader = NULL;
	minpattern_len = 1000;

	while(fgets(linebuffer,255,file)){
		ATTACKPATTERN *pOnepattern;
		int deslen;
		char *pchar;
		pchar = strchr(linebuffer,'#');
		if (pchar == NULL)
			continue;
		pOnepattern = malloc(sizeof(ATTACKPATTERN));
		deslen = pchar - linebuffer;
		pOnepattern->patternlen = strlen(linebuffer) - deslen -1 -1 ;
		pchar ++;
		memcpy(pOnepattern->attackdes, linebuffer, deslen);
		memcpy(pOnepattern->patterncontent, pchar, pOnepattern->patternlen);
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
	}
	if (pPatternHeader == NULL) 
		return 1;
	return 0;
}

int readpattern2(char *patternfile){
	FILE *file;
	char linebuffer[256];

	file = fopen(patternfile,"r");
	if ( file == NULL) {
		printf("Cann't open the pattern file! Please check it and try again! \n");
		return 1;
	}
	bzero(linebuffer,256);
	pPatternHeader2 = NULL;
	minpattern_len2 = 1000;

	while(fgets(linebuffer,255,file)){
		ATTACKPATTERN *pOnepattern;
		int deslen;
		char *pchar;
		pchar = strchr(linebuffer,'#');
		if (pchar == NULL)
			continue;
		pOnepattern = malloc(sizeof(ATTACKPATTERN));
		deslen = pchar - linebuffer;
		pOnepattern->patternlen = strlen(linebuffer) - deslen -1 -1;
		pchar ++;
		memcpy(pOnepattern->attackdes, linebuffer, deslen);
		memcpy(pOnepattern->patterncontent, pchar, pOnepattern->patternlen);
		if (pOnepattern->patternlen < minpattern_len2)
			minpattern_len2 = pOnepattern->patternlen;
		pOnepattern->next = NULL;

		if (pPatternHeader2 == NULL)
			pPatternHeader2 = pOnepattern;
		else{
			pOnepattern->next = pPatternHeader2;
			pPatternHeader2 = pOnepattern;
		}
		bzero(linebuffer,256);
	}
	if (pPatternHeader2 == NULL) 
		return 1;
	return 0;
}



int matchpattern2(ATTACKPATTERN *pOnepattern, PACKETINFO2 *pOnepacket){
	int leftlen;
	char *leftcontent;
	char *patterncontent;
	leftlen = pOnepacket-> contentlen;
	leftcontent = pOnepacket -> packetcontent;
	patterncontent = pOnepattern -> patterncontent;
	char *pchar2;
	pchar2 = strchr(patterncontent,'#');
	if (pchar2 == NULL)
		return -1;
	int patternlen1;
	int patternlen2;
	patternlen1 = pchar2 - patterncontent;
	patternlen2 = pOnepattern->patternlen - patternlen1 -1 ;

	pchar2 ++;
	char *ppart1=patterncontent;
	char *ppart2=pchar2;

	//printf("%c\n",leftcontent[0]);
	while(leftlen >= patternlen1){
		if (strncmp(leftcontent,ppart1,patternlen1) == 0)
			break;
		leftlen --;
		leftcontent ++;
	}
	if(leftlen < patternlen1) return 0;
	leftlen = pOnepacket-> contentlen;
	leftcontent = pOnepacket -> packetcontent;
	while(leftlen >= patternlen2){
		if (strncmp(leftcontent,ppart2,patternlen2) == 0)
			return 1;
		leftlen --;
		leftcontent ++;
	}
	return 0;
}

int matchpattern(ATTACKPATTERN *pOnepattern, PACKETINFO2 *pOnepacket){
	int leftlen;
	char *leftcontent;
	leftlen = pOnepacket-> contentlen;
	leftcontent = pOnepacket -> packetcontent;
	//printf("%c\n",leftcontent[0]);
	while(leftlen >= pOnepattern->patternlen){
		if (strncmp(leftcontent,pOnepattern->patterncontent,pOnepattern->patternlen) == 0)
			return 1;
		leftlen --;
		leftcontent ++;
	}
	return 0;
}

int max(int a, int b) {  
    return (a > b) ? a : b;  
}  

void badCharInit(char modelStr[],int modelStrLen, int modelStrIndex[]) {
    
    //-1表示该字符在匹配串中没有出现过
    for (int i = 0 ; i < 256 ; i ++) {
        modelStrIndex[i] = -1;
    }
    for (int i = 0 ; i < modelStrLen ; i++) {
        //直接依次存入，出现相同的直接覆盖，
        //保证保存的时候靠后出现的位置
        modelStrIndex[modelStr[i]] = i;
    }
}

int badChar(char badChr,int badCharIndex,int modelStrIndex[],int modelStrLen) {
        /*
		//坏字符位置
        int badCharIndex = -1;
        char badChar = '\0';
        //开始从匹配串后往前进行匹配
        for (int i = modelStr.length - 1 ; i >= 0 ; i --) {
            int mainStrIndex = start + i;
            //第一个出现不匹配的即为坏字符
            if (mainStr[mainStrIndex] != modelStr[i]) {
                badCharIndex = i;
                badChar = mainStr[mainStrIndex];
                break;
            }
        }
		*/
        //if (-1 == badCharIndex) {
            //不存在坏字符,需匹配成功，要移动距离为0
        //    return 0;
        //}
        //查看坏字符在匹配串中出现的位置
        if (badChr>=0 && badChr<=255 && modelStrIndex[badChr] > -1) {
            //出现过
			
            return badCharIndex - modelStrIndex[badChr];
        }
        return modelStrLen;
    }

//2.好后缀数组的建立,suffix为后缀字符对应前面的位置，prefix存储：是否存在匹配的前缀字串
void generateGS(char patterncontent[], int patternlen, int suffix[], bool prefix[])
{
	int n = patternlen;
	for (int i = 0; i < n - 1; i++)
	{
		suffix[i] = -1;
		prefix[i] = false;
	}

	for (int i = 0; i < n - 1; i++)
	{
		int j = i;//从第一个字符开始遍历，str[j]
		int k = 0;//最后一个字符的变化，对应下面的str[n - 1 - k]
		while (j >= 0 && patterncontent[j] == patterncontent[n - 1 - k])//和最后一个字符对比，相等则倒数第二个
		{
			j--;
			k++;
			suffix[k] = j + 1;//如果k=1，则是一个字符长度的后缀对应匹配位置的值
		}
		if (j == -1)//说明有前缀字符对应
			prefix[k] = true;
	}

}
//3.返回好后缀移动的次数,index为坏字符位置-其后面就是好后缀，size为str大小
int getGsMove(int suffix[], bool prefix[], int index, int size)
{
	int len = size - index - 1;//好字符的长度，因为index为坏字符位置，所以要多减1
	if(len==0) return 1;
	if (suffix[len] != -1)//当前len长度的后缀坏字符串前边有匹配的字符
	{
		return index + 1 - suffix[len];//后移位数 = 好后缀的位置(index + 1) - 搜索词中的上一次出现位置
	}

	//index为坏字符，index+1为好后缀，index+2为子好后缀
	for (int i = index + 2; i < size; i++)
	{
		if (prefix[size - i])//因为prefix从1开始
			return i;//移动当前位置离前缀位置，acba-对应a移动3
	}

	return size;

}


int matchpattern_BM(ATTACKPATTERN *pOnepattern, PACKETINFO2 *pOnepacket, int i){
	//printf("%s\n",pOnepacket -> packetcontent);
	int *patternIndex = modelStrIndex[i];
	int m = pOnepattern->patternlen;
	char *leftcontent = pOnepacket -> packetcontent;
	char *pcontent = pOnepattern -> patterncontent;
	int *suffix_i = suffix[i];
	bool *prefix_i = prefix[i];

	int leftlen;
	//char *leftcontent;
	leftlen = pOnepacket-> contentlen;
	if(leftlen<=0)return 0;
	//leftcontent = pOnepacket -> packetcontent;
	int start=0;
	char badChr = '\0';
	int n=m-1;
	while(start+m-1<leftlen){
		while(n>=0){
			if (leftcontent[start+n] != pcontent[n]){
				badChr = leftcontent[start+n];
				//printf("%c,%c;",badChr,pcontent[n]);
				break;
			}
			n--;
		}
		//printf("%d;",n);
		if(n==-1){
			return 1;
		}
		//printf("%d;",n);
		int bad_mv = badChar(badChr,n,patternIndex,m);
		int good_mv = getGsMove(suffix_i, prefix_i, n, m);
		//printf("%d.%d;",bad_mv,good_mv);
		int mv=max(bad_mv,good_mv);
		start=start+mv;
		n = pOnepattern->patternlen-1;
	}

	return 0;


}

int matchpattern_AC(int pattern_len, PACKETINFO2 *pOnepacket, const struct pcap_pkthdr *header){
	ATTACKPATTERN2 *pOnepattern2 = pPatternHeader2;
	char *text = pOnepacket -> packetcontent; 
	char text_len = pOnepacket ->contentlen;
	match_result_t matches = acsm_search(ctx, (u_char *)text, acsm_strlen(text));
	int match_full = matches.count;
    if (matches.count > 0) {
		for (size_t i = 0; i < matches.count; ++i) {
            int count = matches.patterns[i];
			pOnepattern2 = pPatternHeader2;
			while(pOnepattern2 != NULL && count!=0){
			pOnepattern2 = pOnepattern2->next;
			count--;
			}
			int max_len=max(strlen(pOnepattern2->src),strlen(pOnepacket->src));
			//添加对端口的判断
			if(((strncmp(pOnepattern2->src,"any",3)==0)||(strncmp(pOnepattern2->src,pOnepacket->src,max_len)==0))&&((strncmp(pOnepattern2->des,"any",3)==0)||(strncmp(pOnepattern2->des,pOnepacket->des,max_len)==0)))
			{
				output_alert_2(pOnepattern2, pOnepacket,header);
			}
			else {
				match_full--;
			}
        }
    }
	if (match_full > 0) return 1;
    else {
		return 0;
    }

}


void output_alert(ATTACKPATTERN *pOnepattern,PACKETINFO2 *pOnepacket,const struct pcap_pkthdr *header)
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
	printf("发现特征串攻击:\n     攻击类型  %s   ", pOnepattern->attackdes);
  	printf("%d.%d.%d.%d ==> ",pOnepacket->src_ip[0],pOnepacket->src_ip[1],pOnepacket->src_ip[2],pOnepacket->src_ip[3]);
  	printf("%d.%d.%d.%d\n",pOnepacket->dest_ip[0],pOnepacket->dest_ip[1],pOnepacket->dest_ip[2],pOnepacket->dest_ip[3]);


	// 关闭日志文件
    fclose(logFile);
}

void output_alert_2(ATTACKPATTERN2 *pOnepattern,PACKETINFO2 *pOnepacket,const struct pcap_pkthdr *header)
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
	printf("发现特征串攻击:\n     攻击类型  %s   ", pOnepattern->attackdes);
  	printf("%d.%d.%d.%d ==> ",pOnepacket->src_ip[0],pOnepacket->src_ip[1],pOnepacket->src_ip[2],pOnepacket->src_ip[3]);
  	printf("%d.%d.%d.%d\n",pOnepacket->dest_ip[0],pOnepacket->dest_ip[1],pOnepacket->dest_ip[2],pOnepacket->dest_ip[3]);


	// 关闭日志文件
    fclose(logFile);
}


void pcap_callback(u_char *user,const struct pcap_pkthdr *header,const u_char *pkt_data)
{
	IPHEADER *ip_header;
	PACKETINFO2 onepacket;
  	bzero(&onepacket,sizeof(PACKETINFO2));

  	if(header->len >= 14)
    	ip_header=(IPHEADER*)(pkt_data+14);
	else 
		return;
  	if(ip_header->proto == 6){
		onepacket.contentlen = ip_header->total_len - 20 - 20;
		if (onepacket.contentlen < minpattern_len)
			return;
		onepacket.packetcontent = (char *)(pkt_data + 14 + 20 + 20);
   		strncpy(onepacket.src_ip,ip_header->sourceIP,4);
    	strncpy(onepacket.dest_ip,ip_header->destIP,4);
		struct ip * ip_header2=(struct ip *)(pkt_data+14);

		struct tcphdr *tcp_header = (struct tcphdr *)(pkt_data + 14 + (ip_header2->ip_hl * 4));
		sprintf(onepacket.src, "%d", ntohs(tcp_header->th_sport));
		sprintf(onepacket.des, "%d", ntohs(tcp_header->th_dport));
		
		
		ATTACKPATTERN *pOnepattern = pPatternHeader;
		int i=0;
		while(pOnepattern != NULL){
			if (matchpattern_BM(pOnepattern, &onepacket, i)){
				output_alert(pOnepattern, &onepacket, header);
				//printf("%s\n",onepacket.packetcontent);
			}
			pOnepattern = pOnepattern->next;
			i++;
    	}
		ATTACKPATTERN2 *pOnepattern2 = pPatternHeader2;
		int pattern_len2 = 0;
		while(pOnepattern2 != NULL){
			pattern_len2++;
			pOnepattern2 = pOnepattern2->next;
		}
		pOnepattern2 = pPatternHeader2;
		if(pOnepattern2 != NULL){
			int match2=matchpattern_AC(pattern_len2, &onepacket, header);
		}
		/*if (match2==1){
			output_alert(pOnepattern2, &onepacket);
		}
		int i=0;
    	ATTACKPATTERN *pOnepattern = pPatternHeader;
		while(pOnepattern != NULL){
			if (matchpattern_BM(pOnepattern, &onepacket, i)){
				output_alert(pOnepattern, &onepacket);
			}
			pOnepattern = pOnepattern->next;
			i++;
    		}
		*/
	}
}

void help()
{
	printf("h: help\n");
	printf("m: the working mode (0:Default Rulebase 1:add your own rules 2:use only your rules)\n");
	printf("q: quit\n");
}


int main(int argc,char *argv[])
{ 
   	help();
	char c;
	int mode;
	do{
		scanf("%c",&c);
		if(c=='h')help();
		else if(c=='q')return 0;
		else if(c=='m'){
			printf("0:Default Rulebase\n1:add your own rules\n2:use only your rules\n");
			scanf("%d",&mode);
		}
	}while (c!='m');
	
  	char *device;
  	char errbuf[PCAP_ERRBUF_SIZE];
  	pcap_t *phandle;
  	bpf_u_int32 ipaddress, ipmask;
  	struct bpf_program fcode;

	//if(parse_para(argc,argv,patternfile))  //解析参数
	//	exit(0);
	char patternfile[256];  //保存攻击模式文件名
	if(mode==1||mode==2){
		printf("please input your file\n");
		scanf("%s",&patternfile);
		if (readpattern(patternfile))
			exit(0);
	}

	//if (readpattern2(patternfile2))
	//	exit(0);
	if(mode==1||mode==0){
		if (readpattern_sql()) exit(0);

		ATTACKPATTERN2 *pOnepattern2 = pPatternHeader2;
		
		ctx = acsm_alloc(0);
		if (ctx == NULL) {
			fprintf(stderr, "acsm_alloc() error.\n");
			return -1;
		}
		int pattern_id = 0; // 初始化模式串序号
	
		while(pOnepattern2 != NULL) {
			if (acsm_add_pattern(ctx, (u_char *)(pOnepattern2 ->patterncontent), acsm_strlen((u_char *)(pOnepattern2 ->patterncontent)), pattern_id) != 0) {
				fprintf(stderr, "acsm_add_pattern() with pattern \"%s\" error.\n", 
						(u_char *)(pOnepattern2 ->patterncontent));
				return -1;
			}

			pOnepattern2 = pOnepattern2->next;
			pattern_id++;
		}

		debug_printf("after add_pattern: max_state=%d\n", ctx->max_state);
		if (acsm_compile(ctx) != 0) {
			fprintf(stderr, "acsm_compile() error.\n");
			return -1;
		}    
	}
    

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

	if(mode==1||mode==2){
		ATTACKPATTERN *pOnepattern = pPatternHeader;
		int pattern_len = 0;
		while(pOnepattern != NULL){
			pattern_len++;
			pOnepattern = pOnepattern->next;
		}
		pOnepattern = pPatternHeader;


		// 分配内存空间
		suffix = (int **)malloc(pattern_len * sizeof(int *));
		if (suffix == NULL) {
			printf("Memory allocation failed.\n");
			return 1;
		}

		// 分配内存空间
		prefix = (bool **)malloc(pattern_len * sizeof(bool *));
		if (prefix == NULL) {
			printf("Memory allocation failed.\n");
			return 1;
		}

		// 分配内存空间
		modelStrIndex = (int **)malloc(pattern_len * sizeof(int *));
		if (modelStrIndex == NULL) {
			printf("Memory allocation failed.\n");
			return 1;
		}

		int i=0;
		while(pOnepattern != NULL){
			int m = pOnepattern->patternlen;
			char *pcontent = pOnepattern -> patterncontent;
			suffix[i] = (int *)malloc(m * sizeof(int));
			if (suffix[i] == NULL) {
				printf("Memory allocation failed.\n");
				// 释放之前分配的内存
				for (int j = 0; j < i; j++) {
					free(suffix[j]);
				}
				free(suffix);
				return 1;
			}
			prefix[i] = (bool *)malloc(m * sizeof(bool));
			if (prefix[i] == NULL) {
				printf("Memory allocation failed.\n");
				// 释放之前分配的内存
				for (int j = 0; j < i; j++) {
					free(prefix[j]);
				}
				free(prefix);
				return 1;
			}
			modelStrIndex[i] = (int *)malloc(256 * sizeof(int));
			if (modelStrIndex[i] == NULL) {
				printf("Memory allocation failed.\n");
				// 释放之前分配的内存
				for (int j = 0; j < i; j++) {
					free(modelStrIndex[j]);
				}
				free(modelStrIndex);
				return 1;
			}
			badCharInit(pcontent, m, modelStrIndex[i]);
			generateGS(pcontent, m, suffix[i], prefix[i]);
			pOnepattern = pOnepattern->next;
			i++;
		}
	}	

 	printf("开始特征串攻击检测.....\n");  //此函数设置过滤器并开始进行数据包的捕捉
  	pcap_loop(phandle,-1,pcap_callback,NULL); 

	acsm_free(ctx);    
}
