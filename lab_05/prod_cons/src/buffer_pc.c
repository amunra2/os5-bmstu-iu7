#include "buffer_pc.h"


// буфер не обязательно должен быть кольцевым, а лучше, чтобы не был кольцевым


int init_buf(buf_t *const buffer)
{
    if (!buffer)
    {
        return -1;
    }

    memset(buffer, 0, sizeof(buf_t));

    return 0;
}


int read_buf(buf_t *const buffer, char *const dst)
{
    if (!buffer)
    {
        return -1;
    }

    *dst = buffer->data[buffer->r_pos++];
    buffer->r_pos %= N; // читается только в пределах буфера

    return 0;
}


int write_buf(buf_t *const buffer, const char elem)
{
    if (!buffer)
    {
        return -1;
    }

    buffer->data[buffer->w_pos++] = elem;
    buffer->w_pos %= N; // записывается только в пределах буфера

    return 0;
}