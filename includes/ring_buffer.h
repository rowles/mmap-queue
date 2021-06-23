#pragma once

#include <memory>
#include <mutex>
#include <cinttypes>
#include <unistd.h>
#include <cstring>

namespace ring_buffer {
/*
 * Shared process mutex and guard (linux only)
 **/
struct process_mutex {
  struct guard {
    guard(process_mutex *m) : m(m) {
        pthread_mutex_lock(&m->mutex);
    }

    ~guard() {
        pthread_mutex_unlock(&m->mutex);
    }

  private:
    process_mutex *m;
  };

  pthread_mutex_t mutex{};
  pthread_mutexattr_t mutexattr{};

  process_mutex() {
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&mutex, &mutexattr);
  }
};

// Return struct for reads
struct read_status {
  int8_t code{0};
  size_t length{0};
};

// Return struct for writes
struct write_status {
  int8_t code{0};
};

class mmap_ring_buffer {
public:

  constexpr static int8_t STATUS_OK = 0;
  constexpr static int8_t STATUS_EMPTY = -1;
  constexpr static int8_t STATUS_FULL = -2;

  mmap_ring_buffer(size_t max_bytes, void* buffer) 
    : max_bytes(max_bytes), buffer(buffer) {}
  ~mmap_ring_buffer() = default;

  size_t capacity_bytes() const {
    return max_bytes;
  }

  size_t get_size() const {
    return size;
  }

  bool has_capacity(size_t data_size) const {
    return size + data_size <= max_bytes;
  }

  /*void debug() {
    printf("buffer: %p\n", buffer);
    printf("head: %p %li\n", &head, head);
    printf("tail: %p %li\n", &tail, tail);
    printf("size: %p %li\n", &size, size);
    printf("max size: %li\n", max_bytes);
  }*/

  write_status put(void* data, size_t length) noexcept {
    process_mutex::guard g{&mutex};
 
    if (!has_capacity(length + sizeof(size_t))) return write_status{.code=-1};

    // put the header and data
    _put((void*)&length, sizeof(length));
    _put(data, length);
    
    return write_status{.code=STATUS_OK};
  }

  // recv_buf, recv_len
  read_status get(void* data_buf, size_t length) noexcept {
    process_mutex::guard g{&mutex};

    // queue is empty
    if (size <= 0) {
      return read_status{.code=STATUS_EMPTY}; // empty
    }

    // read size of data first
    size_t data_size{0};
    _get((void*)&data_size, sizeof(data_size), false);

    // compare to recieve buffer
    if (data_size > length) {
      return read_status{.code=STATUS_FULL, .length=data_size}; // no room
    }
    
    // get the data
    _get(data_buf, data_size+sizeof(size_t), true);

    return read_status{.code=STATUS_OK, .length=data_size};
  }

private:

  void _get(void* data, size_t data_size, bool pop=false) noexcept {
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

  void _put(void* data, size_t data_size) noexcept {
    const bool wrap = tail + data_size >= max_bytes;

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
  process_mutex mutex{};
  void* buffer{nullptr};
};

// must be true to be allocated on mmap
static_assert(std::is_trivially_copyable<mmap_ring_buffer>::value);

} // namespace ring_buffer

