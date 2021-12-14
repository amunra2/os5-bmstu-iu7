#ifndef READWRITE_H
#define READWRITE_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/sem.h>

#include "const.h"


int run_reader(int *const buffer, const int sid, const int reader_id);
int run_writer(int *const buffer, const int sid, const int writer_id);


#endif