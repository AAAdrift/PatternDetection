
#include <stdarg.h>
#include "acsm.h"
#include <pcap.h>



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


int acsm_add_pattern(acsm_context_t *ctx, u_char *string, size_t len) 
{
    u_char ch;
    size_t i;
    acsm_pattern_t *p;

    p = malloc(sizeof(acsm_pattern_t));
    if (p == NULL) {
        return -1;
    }

    p->len = len;

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


int acsm_search(acsm_context_t *ctx, u_char *text, size_t len)
{
    int state = 0;
    u_char *p, *last, ch;

    p = text;
    last = text + len;
    int matches_found = 0;

    while (p < last) {
        ch = ctx->no_case ? acsm_tolower((*p)) : (*p);

        while (ctx->state_table[state].next_state[ch] == ACSM_FAIL_STATE) {
            state = ctx->state_table[state].fail_state;
        }

        state = ctx->state_table[state].next_state[ch];

        if (ctx->state_table[state].match_list) {
            matches_found++;
        }

        p++;
    }
    return matches_found;  // 返回当前搜索找到的匹配数量

}
