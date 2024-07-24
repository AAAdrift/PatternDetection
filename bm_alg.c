#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bm_alg.h"

int **suffix;
bool **prefix;
int **modelStrIndex;


int max(int a, int b) {  
    return (a > b) ? a : b;  
} 

//建立坏字符表
void badCharInit(char modelStr[],int modelStrLen, int patternStrIndex[]) {
    
    //-1表示该字符在匹配串中没有出现过
    for (int i = 0 ; i < 256 ; i ++) {
        patternStrIndex[i] = -1;
    }
    for (int i = 0 ; i < modelStrLen ; i++) {
        //直接依次存入，出现相同的直接覆盖，
        //保证保存的是靠后出现的位置
        patternStrIndex[modelStr[i]] = i;
    }
}

//好后缀数组的建立,suffix为后缀字符对应前面的位置，prefix存储：是否存在匹配的前缀字串
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

//计算坏字符对应的移动位数
int badChar(char badChr,int badCharIndex,int modelStrIndex[],int modelStrLen) {
    //查看坏字符在匹配串中出现的位置
    if (badChr>=0 && badChr<=255 && modelStrIndex[badChr] > -1) {
        //出现过			
        return badCharIndex - modelStrIndex[badChr];
    }
    return badCharIndex + 1;
}

//计算好后缀对应的移动位数,index为坏字符位置-其后面就是好后缀，size为str大小
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
			return i;//移动当前位置离前缀位置
	}

	return size;

}

/*************************************关键函数***********************************/

/*
函数matchpattern_BM - 使用BM匹配算法匹配单个模式与数据包内容
参数1：模式串链表头指针
参数2：捕获的数据包指针
参数3：模式串序号
Return：
    匹配成功返回1
    匹配失败返回2
*/
int matchpattern_BM(ATTACKPATTERN *pOnepattern, POnepOnepacket *pOnepacket, int patternID){
    // 获取当前模式的索引数组
    int *patternstrIndex = modelStrIndex[patternID];
    // 获取模式的长度
    int m = pOnepattern->patternlen;
    // 获取数据包的内容
    char *leftcontent = pOnepacket->packetcontent;
    // 获取模式的内容
    char *pcontent = pOnepattern->patterncontent;
    // 获取后缀数组
    int *suffix_i = suffix[patternID];
    // 获取前缀数组
    bool *prefix_i = prefix[patternID];

    // 计算数据包内容的长度
    int leftlen;
    leftlen = pOnepacket->contentlen;
    // 如果数据包长度小于等于0，直接返回0
    if(leftlen <= 0) return 0;
    // 初始化搜索的起始位置
    int start = 0;
    // 用于保存不匹配字符
    char badChr = '\0';
    // 初始化模式的最后一个字符的位置
    int n = m - 1;
    // 进行搜索循环
    while(start + m - 1 < leftlen){
        // 从后向前比较字符
        while(n >= 0){
            // 如果字符不匹配，保存不匹配的字符并跳出循环
            if (leftcontent[start + n] != pcontent[n]){
                badChr = leftcontent[start + n];
                break;
            }
            // 向前移动比较位置
            n--;
        }
        // 如果整个模式都匹配，返回1表示匹配成功
        if(n == -1){
            return 1;
        }
        // 计算坏字符规则下的移动距离
        int bad_mv = badChar(badChr, n, patternstrIndex, m);
        // 计算好后缀规则下的移动距离
        int good_mv = getGsMove(suffix_i, prefix_i, n, m);
        // 取两者中的最大值作为移动距离
        int mv = max(bad_mv, good_mv);
        // 更新起始位置
        start = start + mv;
        // 重置模式的最后一个字符的位置
        n = pOnepattern->patternlen - 1;
    }
    // 如果搜索结束还没有匹配成功，返回0
    return 0;
}


/*
函数init_BM - 用于初始化BM算法所需的数据结构
参数：模式串链表头指针
*/
void init_BM(ATTACKPATTERN *pPatternHeader){
    // 遍历模式链表，计算模式数量并分配内存
    ATTACKPATTERN *pOnepattern = pPatternHeader;
    int pattern_len = 0;
    while(pOnepattern != NULL){
        pattern_len++;
        pOnepattern = pOnepattern->next;
    }
    pOnepattern = pPatternHeader;

    // 分配后缀数组的内存空间
    suffix = (int **)malloc(pattern_len * sizeof(int *));
    if (suffix == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // 分配前缀数组的内存空间
    prefix = (bool **)malloc(pattern_len * sizeof(bool *));
    if (prefix == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // 分配模式字符串索引数组的内存空间
    modelStrIndex = (int **)malloc(pattern_len * sizeof(int *));
    if (modelStrIndex == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    // 初始化每个模式的后缀、前缀和模式字符串索引
    int i = 0;
    while(pOnepattern != NULL){
        int m = pOnepattern->patternlen;
        char *pcontent = pOnepattern->patterncontent;
        // 分配单个模式的后缀数组内存
        suffix[i] = (int *)malloc(m * sizeof(int));
        if (suffix[i] == NULL) {
            printf("Memory allocation failed.\n");
            // 如果分配失败，则释放之前分配的后缀数组内存
            for (int j = 0; j < i; j++) {
                free(suffix[j]);
            }
            free(suffix);
            return;
        }
        // 分配单个模式的前缀数组内存
        prefix[i] = (bool *)malloc(m * sizeof(bool));
        if (prefix[i] == NULL) {
            printf("Memory allocation failed.\n");
            // 如果分配失败，则释放之前分配的前缀数组内存
            for (int j = 0; j < i; j++) {
                free(prefix[j]);
            }
            free(prefix);
            return;
        }
        // 分配单个模式的模式字符串索引数组内存
        modelStrIndex[i] = (int *)malloc(256 * sizeof(int));
        if (modelStrIndex[i] == NULL) {
            printf("Memory allocation failed.\n");
            // 释放之前分配的模式字符串索引数组内存
            for (int j = 0; j < i; j++) {
                free(modelStrIndex[j]);
            }
            free(modelStrIndex);
            return;
        }
        // 初始化模式字符串索引
        badCharInit(pcontent, m, modelStrIndex[i]);
        // 生成好后缀和前缀数组
        generateGS(pcontent, m, suffix[i], prefix[i]);
        // 移动到下一个模式
        pOnepattern = pOnepattern->next;
        i++;
    }
}