#include "utils.h"

int pizza() {
  return 3;
}

ring_buffer* from_mem() {
  const size_t s = 500*(1024*1024);
  B* buf = new B[s];
  return new ring_buffer(s, buf);
}

ring_buffer* from_mmap() {
  const size_t s = 100*(1024*1024);
  using namespace mmapped;
  auto file_path = "/tmp/123";
  mmap_file* xx = new mmap_file(
      file_path,
      mmap_file::Mode::ANON, s);
  xx->open();

  ring_buffer* rg = new(xx->address().get()) ring_buffer(
      s - sizeof(ring_buffer),
      (unsigned char*)xx->address().get() + sizeof(ring_buffer)
    );

  return rg;
}
