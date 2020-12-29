#include "mapped.h"

namespace mmapped {
// wrap mmap around smart ptr
std::shared_ptr<void> mmap_ptr(void *addr, size_t length, int prot,
                                        int flags, int fd,
                                        off_t offset) noexcept {
  auto deleter = [length](void *p) { munmap(p, length); };

  return {
      mmap(addr, length, prot, flags, fd, offset),
      deleter};
}

void mmap_file::open() {
  int flags, prot;

  if (mode == Mode::CR) {
    flags = O_RDWR | O_CREAT;
    prot = PROT_READ | PROT_WRITE;
  } else if (mode == Mode::RO) {
    flags = O_RDONLY;
    prot = PROT_READ;
  } else if (mode == Mode::SHARED) {
    flags = O_CREAT | O_RDWR;
    prot = PROT_READ | PROT_WRITE;
  } else {
    throw mmap_error("Mode not supported");
  }

  fd = ::open(path.c_str(), flags, 0644);

  if (fd == -1) {
    throw mmap_error("Failed to open file descriptor");
  }

  if (mode == Mode::CR || mode == Mode::SHARED) {
    truncate(path.c_str(), len);
  }

  struct stat stat_t;
  const int fstat_err = fstat(fd, &stat_t);
  if (fstat_err == -1) {
    throw mmap_error("Failed fstat");
  }
  len = stat_t.st_size;

  addr = mmap_ptr(nullptr, len, prot, MAP_SHARED | MAP_ANONYMOUS, fd, 0u);
  if (addr == nullptr) {
    throw mmap_error("mmap failed");
  }
}

void mmap_file::sync() { msync(addr.get(), len, MS_SYNC); }

void mmap_file::close() {
  if (mode == Mode::CR)
    sync();

  addr.reset(); // calls munmap
  ::close(fd);
}

bool mmap_file::is_open() const noexcept {
  return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

std::shared_ptr<void> mmap_file::address() const { return addr; }

size_t mmap_file::size() const noexcept { return len; }

void mmap_file::set_size(const size_t len) {
  if (is_open()) {
    truncate(path.c_str(), len);
    //throw mmap_error("cannot resize open mmap");
  }
  this->len = len;
}

} // namespace mmapped
