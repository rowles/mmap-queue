#include "mmapped.h"


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

void mmap_region::open() {
  int flags, prot, mmap_flags;

  switch (mode) {
    case mode_t::CR:
      flags = O_RDWR | O_CREAT;
      prot = PROT_READ | PROT_WRITE;
      mmap_flags = MAP_SHARED;
      break;
    case mode_t::RO:
      flags = O_RDONLY;
      prot = PROT_READ;
      mmap_flags = MAP_SHARED;
      break;
    case mode_t::SHARED:
      flags = O_CREAT | O_RDWR;
      prot = PROT_READ | PROT_WRITE;
      mmap_flags = MAP_SHARED;
      break;
    case mode_t::ANON:
      prot = PROT_READ | PROT_WRITE;
      mmap_flags = MAP_ANONYMOUS | MAP_SHARED;
      break;
    default:
      throw mmap_error("mode_t not supported");
  }

  // do not create files for anon maps
  if (mode != mode_t::ANON) {
    fd = ::open(path.c_str(), flags, 0644);

    if (fd == -1) {
      throw mmap_error("Failed to open file descriptor");
    }

    if (mode == mode_t::CR || mode == mode_t::SHARED) {
      truncate(path.c_str(), len);
    }
  
    struct stat stat_t;
    const int fstat_err = fstat(fd, &stat_t);
    if (fstat_err == -1) {
      throw mmap_error("Failed fstat");
    }
    len = stat_t.st_size;
  }

  addr = mmap_ptr(nullptr, len, prot, mmap_flags, fd, 0u);
  if (addr == nullptr) {
    throw mmap_error("Failed mmap");
  }
}

void mmap_region::sync() { msync(addr.get(), len, MS_SYNC); }

void mmap_region::close() {
  if (mode == mode_t::CR)
    sync();

  addr.reset(); // calls munmap
  if (mode != mode_t::ANON)
    ::close(fd);
}

bool mmap_region::is_fd_open() const noexcept {
  return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}

std::shared_ptr<void> mmap_region::address() const { return addr; }

size_t mmap_region::size() const noexcept { return len; }


} // namespace mmapped
