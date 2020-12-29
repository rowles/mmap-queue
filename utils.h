#ifndef MY_V1_H
#define MY_V1_H


#include "ring_buffer.h"
#include "mapped.h"

ring_buffer* from_mem();
ring_buffer* from_mmap();


int pizza();

#endif
