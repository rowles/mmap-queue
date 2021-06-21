#ifndef MY_V1_H
#define MY_V1_H

#include <memory>

#include "ring_buffer.h"
#include "mapped.h"

ring_buffer* from_mem();
ring_buffer* from_mmap(mmapped::mmap_t* region);

#endif
