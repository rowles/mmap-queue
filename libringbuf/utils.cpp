#include "utils.h"


ring_buffer* from_mem() {
  const size_t s = 500*(1024*1024);
  B* buf = new B[s];
  return new ring_buffer(s, buf);
}

ring_buffer* from_mmap(mmapped::mmap_t* region) {
  const ptrdiff_t buf_size = region->size() - sizeof(ring_buffer);
  unsigned char* base_addr = (unsigned char*)region->address().get() + sizeof(ring_buffer);
  ring_buffer* rg = new(region->address().get()) ring_buffer(
      buf_size,
      base_addr
    );

  return rg;
}


