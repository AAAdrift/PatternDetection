#include <stdio.h>
#include <stdlib.h>
#include "PatternDetection.h"

int max(int a, int b);

//建立坏字符表
void badCharInit(char modelStr[],int modelStrLen, int patternStrIndex[]);

//好后缀数组的建立,suffix为后缀字符对应前面的位置，prefix存储：是否存在匹配的前缀字串
void generateGS(char patterncontent[], int patternlen, int suffix[], bool prefix[]);

//计算坏字符对应的移动位数
int badChar(char badChr,int badCharIndex,int modelStrIndex[],int modelStrLen);

//计算好后缀对应的移动位数,index为坏字符位置-其后面就是好后缀，size为str大小
int getGsMove(int suffix[], bool prefix[], int index, int size);


//使用BM匹配算法匹配单个模式与数据包内容
int matchpattern_BM(ATTACKPATTERN *pOnepattern, POnepOnepacket *pOnepacket, int i);

//初始化BM算法所需的数据结构
void init_BM(ATTACKPATTERN *pPatternHeader);