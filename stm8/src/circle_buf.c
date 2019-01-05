#include "circle_buf.h"

   
char circle_buf_push(circle_buf_t *c, unsigned char data) {
    unsigned short next = (unsigned char)(c->head + 1);
    if (next >= c->max_len)
        next = 0;

    if (next == c->tail)
        return -1;       

    c->buffer[c->head] = data; 
    c->head = next;            
    return 0;
}

char circle_buf_pop(circle_buf_t *c, unsigned char *data) {
  unsigned short next;  
  if (c->head == c->tail)
      return -1;

    next = (unsigned char)(c->tail + 1);
    if(next >= c->max_len)
        next = 0;

    *data = c->buffer[c->tail]; 
    c->tail = next;             
    return 0; 
}
