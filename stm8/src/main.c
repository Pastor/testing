#include <assert.h>
#include <circle_buf.h>

#define BUF_LEN   64

static CIRCLE_BUF_DEF(buf, BUF_LEN);

static void
circle_buf_tests(unsigned char offset) {
    unsigned char data = {0};
    char ret;
    unsigned char i;

    for (i = 0; i < BUF_LEN - 1; ++i) {
        ret = circle_buf_push(&buf, (i + offset));
        assert(ret == 0);
    }
    ret = circle_buf_push(&buf, 'p');
    assert(ret != 0);
    for (i = 0; i < BUF_LEN - 1; ++i) {
        ret = circle_buf_pop(&buf, &data);
        assert(ret == 0);
        assert(data == (i + offset));
    }
    ret = circle_buf_pop(&buf, &data);
    assert(ret != 0);
}

int
main(void) {
    unsigned char offset = 0;
    circle_buf_tests(offset++);
    circle_buf_tests(offset++);
    circle_buf_tests(offset++);
    circle_buf_tests(offset);
    return 0;
}
