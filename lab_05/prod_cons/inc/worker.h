#ifndef WORKER_H
#define WORKER_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/sem.h>

#include "buffer_pc.h"
#include "const.h"


int run_producer(buf_t *const buffer, const int sid, const int producer_id);
int run_consumer(buf_t *const buffer, const int sid, const int consumer_id);


#endif