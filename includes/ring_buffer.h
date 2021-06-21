#pragma once

#include <memory>
#include <mutex>
#include <cinttypes>
#include <unistd.h>
#include <cstring>

typedef uint8_t B;

struct read_status {
  int code{0};
  size_t length{0};
};

struct write_status {
  int code{0};
};


class ring_buffer {
public:

  ring_buffer(size_t max_bytes, B* buffer) 
    : max_bytes(max_bytes), buffer(buffer) {}
  ~ring_buffer() = default;

  size_t capacity_bytes() const {
    return max_bytes;
  }

  size_t get_size() const {
    return size;
  }

  bool has_capacity(size_t data_size) const {
    return size + data_size <= max_bytes;
  }

  void debug() {
    printf("buffer: %p\n", buffer);
    printf("head: %p %li\n", &head, head);
    printf("tail: %p %li\n", &tail, tail);
    printf("size: %p %li\n", &size, size);
  }

  write_status put(B* data, size_t length) noexcept {
    std::lock_guard<std::mutex> lock(buf_mutex);
 
    if (!has_capacity(length + sizeof(size_t))) return write_status{.code=-1};

    _put((B*)&length, sizeof(length));
    _put(data, length);
    
    return write_status{.code=0};
  }

  read_status get(B* data_buf, size_t length) noexcept {
    std::lock_guard<std::mutex> lock(buf_mutex);

    if (size == 0) {
      return read_status{.code=-1}; // empty
    }

    size_t data_size{0};
    _get((B*)&data_size, sizeof(data_size), false);

    if (data_size > length) {
      return read_status{.code=-2, .length=data_size}; // no room
    }
    
    _get(data_buf, data_size+sizeof(size_t), true);

    return read_status{.code=0, .length=data_size};
  }

private:

  void get_data(B* data, size_t data_size, bool pop=false) noexcept;
  void put_data(B* data, size_t data_size) noexcept;

  void _get(B* data, size_t data_size, bool pop=false) noexcept {
    size_t new_head = head;

    const bool wrap = (head + data_size) >= max_bytes;

    if (!wrap) {
      std::memcpy(data, buffer + head, data_size);
      new_head = head + data_size;
    }

    if (pop) {
      head = new_head;
      size = size - data_size;
    }
  }

  void _put(B* data, size_t data_size) noexcept {
    const bool wrap = tail + data_size >= max_bytes;

    //printf("put wrap %i\n", wrap);
    if (!wrap) {
      std::memcpy(buffer + tail, data, data_size);
      tail += data_size;
    }

    size += data_size;
  }

  const size_t max_bytes;
  volatile size_t size{0};
  volatile size_t head{0};
  volatile size_t tail{0};
  B* buffer{nullptr};
  std::mutex buf_mutex;
};

