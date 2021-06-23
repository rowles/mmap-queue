#pragma once

#include "ring_buffer.h"
#include "mmapped.h"

namespace utils {
ring_buffer::mmap_ring_buffer* from_ptr(void* region, size_t length);
ring_buffer::mmap_ring_buffer* from_mmap(mmapped::mmap_region* region);
}; // namespace utils
