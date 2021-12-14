#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <string.h>

#include "const.h"


typedef struct buf_s
{
    size_t r_pos;
    size_t w_pos;

    char data[N];
} buf_t;


int init_buf(buf_t *const buffer);
int read_buf(buf_t *const buffer, char *const dst);
int write_buf(buf_t *const buffer, const char elem);


#endif
