#include <stdarg.h>
#include "acsm.h"
#include <pcap.h>

extern ATTACKPATTERN* pPatternHeader;
acsm_context_t  *ctx;


#define DEBUG 0

void debug_printf(const char *fmt, ...)
{

#if (DEBUG)
    va_list ap;

    va_start(ap, fmt);
    (void) vprintf(fmt, ap);
    va_end(ap);
#endif

}


acsm_state_queue_t *acsm_alloc_state_queue(acsm_context_t *ctx)
{
    acsm_queue_t       *q;
    acsm_state_queue_t *sq;

    if (acsm_queue_empty(&ctx->free_queue.queue)) {
        sq = malloc(sizeof(acsm_state_queue_t)); 
    }
    else {
        q = acsm_queue_last(&ctx->free_queue.queue);
        acsm_queue_remove(q);
        sq = acsm_queue_data(q, acsm_state_queue_t, queue);
    }

    return sq;
}


void acsm_free_state_queue(acsm_context_t *ctx, acsm_state_queue_t *sq)
{
    acsm_queue_remove(&sq->queue);
    acsm_queue_insert_tail(&ctx->free_queue.queue, &sq->queue);

    sq->state = ACSM_FAIL_STATE;
}


int acsm_add_state(acsm_context_t *ctx, int state)
{
    acsm_state_queue_t *sq;

    sq = acsm_alloc_state_queue(ctx);
    if (sq == NULL) {
        return -1;
    }

    sq->state = state;
    acsm_queue_insert_head(&ctx->work_queue.queue, &sq->queue);

    return 0;
}


int acsm_next_state(acsm_context_t *ctx)
{
    int                 state;
    acsm_queue_t       *q;
    acsm_state_queue_t *sq;

    if (acsm_queue_empty(&ctx->work_queue.queue)) {
        return ACSM_FAIL_STATE;
    }

    q = acsm_queue_last(&ctx->work_queue.queue);
    acsm_queue_remove(q);
    sq = acsm_queue_data(q, acsm_state_queue_t, queue);

    state = sq->state;

    acsm_free_state_queue(ctx, sq);

    return state;
}


void acsm_free_state(acsm_context_t *ctx)
{
    acsm_queue_t       *q;
    acsm_state_queue_t *sq;

    while ((q = acsm_queue_last(&ctx->free_queue.queue))) {
        if (q == acsm_queue_sentinel(&ctx->free_queue.queue)) {
            break;
        }

        acsm_queue_remove(q);
        sq = acsm_queue_data(q, acsm_state_queue_t, queue);

        free(sq);
    }
}


int acsm_state_add_match_pattern(acsm_context_t *ctx, 
        int state, acsm_pattern_t *p)
{
    acsm_pattern_t *copy;

    copy = malloc(sizeof(acsm_pattern_t));
    if (copy == NULL) {
        return -1;
    }
    memcpy(copy, p, sizeof(acsm_pattern_t));

    copy->next = ctx->state_table[state].match_list; 
    ctx->state_table[state].match_list = copy; 

    return 0;
}


int acsm_state_union_match_pattern(acsm_context_t *ctx, 
        int state, int fail_state)
{
    acsm_pattern_t *p;

    p = ctx->state_table[fail_state].match_list;

    while (p) {
        acsm_state_add_match_pattern(ctx, state, p);
        p = p->next;
    }

    return 0;
}


acsm_context_t *acsm_alloc(int flag)
{
    int no_case = 0;
    acsm_context_t *ctx;

    if (flag & NO_CASE) {
        no_case = 1;
    }

    ctx = calloc(1, sizeof(acsm_context_t));
    if (ctx == NULL) {
        return NULL;
    }

    ctx->no_case = no_case;
    ctx->max_state = 1;
    ctx->num_state = 0;

    acsm_queue_init(&ctx->work_queue.queue);
    acsm_queue_init(&ctx->free_queue.queue);

    return ctx;
}


void acsm_free(acsm_context_t *ctx)
{
    unsigned        i;
    acsm_pattern_t *p, *op;

    for (i = 0; i <= ctx->num_state; i++) {
        if (ctx->state_table[i].match_list) {
            p = ctx->state_table[i].match_list;
            
            while(p) {
                op = p;
                p = p->next;

                free(op);
            }
        }
    }
    
    p = ctx->patterns;
    while (p) {
        op = p;
        p = p->next;

        if (op->string) {
            free(op->string);
        }

        free(op);
    }

    free(ctx->state_table);
    
    free(ctx);
}


int acsm_add_pattern(acsm_context_t *ctx, u_char *string, size_t len, int pattern_id) 
{
    u_char ch;
    size_t i;
    acsm_pattern_t *p;

    p = malloc(sizeof(acsm_pattern_t));
    if (p == NULL) {
        return -1;
    }

    p->len = len;
    p->pattern_id = pattern_id; // 存储模式串的序号

    if (len > 0) {
        p->string = malloc(len);
        if (p->string == NULL) {
            return -1;
        }

        for(i = 0; i < len; i++) {
            ch = string[i];
            p->string[i] = ctx->no_case ? acsm_tolower(ch) : ch;
        }
    }

    p->next = ctx->patterns;
    ctx->patterns = p;

    debug_printf("add_pattern: \"%.*s\"\n", p->len, string);

    ctx->max_state += len;

    return 0;
}


int acsm_compile(acsm_context_t *ctx)
{
    int state, new_state, r, s;
    u_char ch;
    unsigned int i, j, k;
    acsm_pattern_t *p;

    ctx->state_table = malloc(ctx->max_state * sizeof(acsm_state_node_t));
    if (ctx->state_table == NULL) {
        return -1;
    }

    for (i = 0; i < ctx->max_state; i++) {

        ctx->state_table[i].fail_state = 0;
        ctx->state_table[i].match_list = NULL;

        for (j = 0; j < ASCIITABLE_SIZE; j++) {
            ctx->state_table[i].next_state[j] = ACSM_FAIL_STATE;
        }
    }

    debug_printf("goto function\n");

    /* Construction of the goto function */
    new_state = 0;
    p = ctx->patterns;
    while (p) {
        state = 0;
        j = 0;

        while(j < p->len) {

            ch = p->string[j];
            if (ctx->state_table[state].next_state[ch] == ACSM_FAIL_STATE) {
                break;
            }

            state = ctx->state_table[state].next_state[ch];
            j++;
        }

        for (k = j; k < p->len; k++) {
            new_state = ++ctx->num_state;
            ch = p->string[k];

            debug_printf("add_match_pattern: state=%d, new_state=%d\n", state, new_state);
            ctx->state_table[state].next_state[ch] = new_state;
            state = new_state;
        }

        debug_printf("add_match_pattern: state=%d, s=%.*s\n", state, p->len, p->string);

        acsm_state_add_match_pattern(ctx, state, p);

        p = p->next;
    }

    for (j = 0; j < ASCIITABLE_SIZE; j++) {
        if (ctx->state_table[0].next_state[j] == ACSM_FAIL_STATE) {
            ctx->state_table[0].next_state[j] = 0;
        }
    }
    debug_printf("failure function\n");

    /* Construction of the failure function */
    for (j = 0; j < ASCIITABLE_SIZE; j++) {
        if (ctx->state_table[0].next_state[j] != 0) {
            s = ctx->state_table[0].next_state[j];

            if (acsm_add_state(ctx, s) != 0) {
                return -1;
            }

            ctx->state_table[s].fail_state = 0;
        }
    }

    while (!acsm_queue_empty(&ctx->work_queue.queue)) {

        r = acsm_next_state(ctx);
        debug_printf("next_state: r=%d\n", r);

        for (j = 0; j < ASCIITABLE_SIZE; j++) {
            if (ctx->state_table[r].next_state[j] != ACSM_FAIL_STATE) {
                s = ctx->state_table[r].next_state[j];

                acsm_add_state(ctx, s);

                state = ctx->state_table[r].fail_state;

                while(ctx->state_table[state].next_state[j] == ACSM_FAIL_STATE) {
                    state = ctx->state_table[state].fail_state;
                }

                ctx->state_table[s].fail_state = ctx->state_table[state].next_state[j];
                debug_printf("fail_state: f[%d] = %d\n", s, ctx->state_table[s].fail_state);

                acsm_state_union_match_pattern(ctx, s, ctx->state_table[s].fail_state);
            }
            else {
                state = ctx->state_table[r].fail_state;
                ctx->state_table[r].next_state[j] = ctx->state_table[state].next_state[j];
            }
        }
    }

    acsm_free_state(ctx);
    debug_printf("end of compile function\n");

    return 0;
}


match_result_t acsm_search(acsm_context_t *ctx, u_char *text, size_t len)
{
    match_result_t result = {NULL, 0};
    int state = 0;
    u_char *p, *last, ch;

    p = text;
    last = text + len;


    while (p < last) {
        ch = ctx->no_case ? acsm_tolower((*p)) : (*p);

        while (ctx->state_table[state].next_state[ch] == ACSM_FAIL_STATE) {
            state = ctx->state_table[state].fail_state;
        }

        state = ctx->state_table[state].next_state[ch];
        acsm_pattern_t *match = ctx->state_table[state].match_list;

        if (match) {
            while (match) {
                // 假设每个模式串的序号存储在match->pattern_id中
                result.patterns = realloc(result.patterns, (result.count + 1) * sizeof(int));
                result.patterns[result.count] = match->pattern_id;
                result.count++;
                match = match->next;
            }

        }

        p++;
    }
    return result;  // 返回当前搜索找到的匹配数量

}


/*************************************关键函数***********************************/

/*
函数matchpattern_AC - 使用AC匹配算法匹配单个模式与数据包内容
参数1：捕获的数据包指针
参数2：捕获的数据包基本信息
Return：
    匹配成功返回1
    匹配失败返回2
*/
int matchpattern_AC(POnepOnepacket *pOnepacket, const struct pcap_pkthdr *header){
    // 指向攻击模式链表的指针
    ATTACKPATTERN *pOnepattern = pPatternHeader;
    // 获取数据包内容
    char *text = pOnepacket->packetcontent; 
    // 获取数据包内容的长度
    char text_len = pOnepacket->contentlen;
    // 使用Aho-Corasick算法搜索匹配项
    match_result_t matches = acsm_search(ctx, (u_char *)text, acsm_strlen(text));
    // 存储匹配到的模式数量
    int match_full = matches.count;
    // 如果有匹配项，则进行处理
    if (matches.count > 0) {
        for (size_t i = 0; i < matches.count; ++i) {
            // 获取匹配到的模式的ID
            int count = matches.patterns[i];
            pOnepattern = pPatternHeader;
            // 根据模式ID找到对应的攻击模式
            while(pOnepattern != NULL && count != 0){
                pOnepattern = pOnepattern->next;
                count--;
            }
            int max_len = max(strlen(pOnepattern->src), strlen(pOnepacket->src));
            // 同时添加了对端口的判断
            if(((strncmp(pOnepattern->src, "any", 3) == 0) || 
                (strncmp(pOnepattern->src, pOnepacket->src, max_len) == 0)) &&
               ((strncmp(pOnepattern->des, "any", 3) == 0) || 
                (strncmp(pOnepattern->des, pOnepacket->des, max_len) == 0)))
            {
                // 如果匹配，则输出警告
                output_alert_0(pOnepattern, pOnepacket, header);
            }
            else {
                // 如果不匹配，则减少匹配数量
                match_full--;
            }
        }
    }
    // 如果有匹配，则返回1
    if (match_full > 0) return 1;
    else {
        // 如果没有匹配，则返回0
        return 0;
    }
}

/*
函数init_AC - 用于初始化BM算法所需的数据结构
参数：模式串链表头指针
*/
void init_AC(){
    // 指向攻击模式链表的指针
    ATTACKPATTERN *pOnepattern = pPatternHeader;
    // 分配Aho-Corasick算法的上下文
    ctx = acsm_alloc(0);
    if (ctx == NULL) {
        // 如果分配失败，输出错误并返回-1
        fprintf(stderr, "acsm_alloc() error.\n");
        return -1;
    }
    // 初始化模式串序号
    int pattern_id = 0;
    // 遍历攻击模式链表
    while(pOnepattern != NULL) {
        // 将模式添加到Aho-Corasick算法的上下文中
        if (acsm_add_pattern(ctx, (u_char *)(pOnepattern->patterncontent), 
                             acsm_strlen((u_char *)(pOnepattern->patterncontent)), pattern_id) != 0) {
            // 如果添加失败，输出错误并返回-1
            fprintf(stderr, "acsm_add_pattern() with pattern \"%s\" error.\n", 
                    (u_char *)(pOnepattern->patterncontent));
            return -1;
        }
        // 移动到下一个模式
        pOnepattern = pOnepattern->next;
        // 增加模式ID
        pattern_id++;
    }
    // 输出调试信息
    debug_printf("after add_pattern: max_state=%d\n", ctx->max_state);
    // 编译Aho-Corasick算法的上下文
    if (acsm_compile(ctx) != 0) {
        // 如果编译失败，输出错误
        fprintf(stderr, "acsm_compile() error.\n");
        return -1;
    }    
}