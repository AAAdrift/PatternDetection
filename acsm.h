#ifndef _ACSM_H_
#define _ACSM_H_

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <pcap.h>
#include "PatternDetection.h"

#define ASCIITABLE_SIZE    (256)     

#define PATTERN_MAXLEN   (1024) 

#define ACSM_FAIL_STATE  (-1)     


typedef struct acsm_queue_s {
    struct acsm_queue_s  *prev;
    struct acsm_queue_s  *next;
} acsm_queue_t;

typedef struct {
    int          state;
    acsm_queue_t queue;
} acsm_state_queue_t;


typedef struct acsm_pattern_s {
    u_char        *string;
    size_t         len;
    int pattern_id; // 新增字段，用于存储模式串的序号
    struct acsm_pattern_s *next;
} acsm_pattern_t;


typedef struct {
    int next_state[ASCIITABLE_SIZE];
    int fail_state;

    /* output */
    acsm_pattern_t *match_list;

} acsm_state_node_t;


typedef struct {
    unsigned max_state;
    unsigned num_state;

    acsm_pattern_t    *patterns;
    acsm_state_node_t *state_table;

    void *pool;

    acsm_state_queue_t work_queue;
    acsm_state_queue_t free_queue;

    unsigned no_case;

} acsm_context_t;

typedef struct {
    int *patterns; // 存储模式串序号的数组
    size_t count;  // 数组中序号的数量
} match_result_t;


#define acsm_tolower(c)      (u_char) ((c >= 'A' && c <= 'Z') ? (c | 0x20) : c)

#define acsm_strlen(s)       strlen((const char *) s)


#define NO_CASE 0x01

extern acsm_context_t *acsm_alloc(int flag);
extern void acsm_free(acsm_context_t *ctx);
extern int acsm_add_pattern(acsm_context_t *ctx, u_char *string, size_t len, int pattern_id); 
extern int acsm_compile(acsm_context_t *ctx);
extern match_result_t acsm_search(acsm_context_t *ctx, u_char *string, size_t len);

#define acsm_queue_init(q)                                                    \
    (q)->prev = q;                                                            \
    (q)->next = q


#define acsm_queue_empty(h)                                                   \
    (h == (h)->prev)


#define acsm_queue_insert_head(h, x)                                          \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x


#define acsm_queue_insert_tail(h, x)                                          \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x


#define acsm_queue_head(h)                                                    \
    (h)->next


#define acsm_queue_last(h)                                                    \
    (h)->prev


#define acsm_queue_sentinel(h)                                                \
    (h)


#define acsm_queue_next(q)                                                    \
    (q)->next


#define acsm_queue_prev(q)                                                    \
    (q)->prev


#define acsm_queue_remove(x)                                                  \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next


#define acsm_queue_data(q, type, link)                                        \
    (type *) ((u_char *) q - offsetof(type, link))


#endif /* _ACSM_H_ */
