#include "utils.h"

int pizza() {
  return 3;
}

ring_buffer* from_mem() {
  const size_t s = 500*(1024*1024);
  B* buf = new B[s];
  return new ring_buffer(s, buf);
}

ring_buffer* from_mmap(mmapped::mmap_t* region) {
  printf("%p\n", region);
  printf("%li\n", region->size());
  const ptrdiff_t buf_size = region->size() - sizeof(ring_buffer);
  unsigned char* base_addr = (unsigned char*)region->address().get() + sizeof(ring_buffer);
  ring_buffer* rg = new(region->address().get()) ring_buffer(
      buf_size,
      base_addr
    );

  return rg;
}

/*
mmap_ring_buf from_mmap_file(const std::string& path, const size_t length) {
  //mmap_file* xx = new mmap_file(path, mmap_file::Mode::SHARED, size);
  std::unique_ptr<mmap_file> xx = std::make_unique<mmapped::mmap_file>(path, mmap_file::Mode::SHARED, length);
  xx->open(); // calls mmap

  const ptrdiff_t buf_size = length - sizeof(ring_buffer);
  // reserve reset of mmap
  unsigned char* base_addr = (unsigned char*)xx->address().get() + sizeof(ring_buffer);
  ring_buffer* rg = new(xx->address().get()) ring_buffer(buf_size, base_addr);

  return mmap_ring_buf{.region=xx, .ring_buf=rg};
}*/
