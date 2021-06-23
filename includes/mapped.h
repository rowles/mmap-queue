#pragma once

#include <memory>
#include <string>
#include <string_view>

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace mmapped {


class mmap_error : public std::exception {
  const std::string msg{};

public:
  mmap_error() = default;
  mmap_error(std::string_view msg_) : msg(msg_) {}

  const char *what() const noexcept override { return msg.data(); }
};

// wrap mmap around smart ptr
std::shared_ptr<void> mmap_ptr(void *addr, size_t length, int prot,
                                        int flags, int fd,
                                        off_t offset) noexcept;

class mmap_region {
public:
  enum Mode {
    RO, // read only
    CR, // create
    SHARED, // shared file backed
    ANON, // shared anon
  };

  mmap_region(const Mode _mode, const size_t _len = 0) : mode(_mode), len(_len) {
    open();
  };
  mmap_region(const std::string_view _path, const Mode _mode,
            const size_t _len = 0)
      : path(_path), mode(_mode), len(_len) {
    open();
  };
  ~mmap_region() {
    if (is_fd_open())
      close();
  }

  void open();
  void sync();
  void close();
  bool is_fd_open() const noexcept;

  std::shared_ptr<void> address() const;
  size_t size() const noexcept;

private:
  const std::string path{};
  const Mode mode{RO};

  size_t len{0};

  int fd{-1};
  std::shared_ptr<void> addr{};
};

} // namespace mmapped

