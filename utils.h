#ifndef MY_V1_H
#define MY_V1_H

#include <tuple>
#include <memory>

#include "ring_buffer.h"
#include "mapped.h"

ring_buffer* from_mem();
ring_buffer* from_mmap(mmapped::mmap_t* region);

//std::tuple<mmapped_ptr, ring_buffer*> from_mmap_file(const std::string& path, const size_t length);
//mmap_ring_buf from_mmap_file(const std::string& path, const size_t length);
//std::tuple<mmapped_ptr, ring_buffer*> from_mmap(const size_t size);


int pizza();

#endif
