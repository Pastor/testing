#ifndef _CIRCBUF_H_
#define _CIRCBUF_H_

typedef struct {
    unsigned char *buffer;
    unsigned short head;
    unsigned short tail;
    unsigned short max_len;
} circle_buf_t;

#define CIRCLE_BUF_DEF(x, y)                   \
    unsigned char x##_data_space[y];        \
    circle_buf_t x = {                      \
        .buffer = x##_data_space,           \
        .head = 0,                          \
        .tail = 0,                          \
        .max_len = (y)                      \
    }
    
char circle_buf_push(circle_buf_t *c, unsigned char  data);
char circle_buf_pop (circle_buf_t *c, unsigned char *data);

#endif
