#include "utils.h"


ring_buffer::mmap_ring_buffer* utils::from_mmap(mmapped::mmap_region* region) {
  return from_ptr(region->address().get(), region->size());
}


ring_buffer::mmap_ring_buffer* utils::from_ptr(void* region, size_t length) {
  constexpr size_t rg_size = sizeof(ring_buffer::mmap_ring_buffer);

  const size_t buf_size = length - rg_size;
  void* base_addr = region + rg_size;

  ring_buffer::mmap_ring_buffer* rg = new(region) ring_buffer::mmap_ring_buffer(
      buf_size,
      base_addr
    );

  return rg;
}


