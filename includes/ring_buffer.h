#pragma once

#include <mutex>
#include <type_traits>
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
  int8_t status{0};
  size_t length{0};
};

// Return struct for writes
struct write_status {
  int8_t status{0};
};

constexpr static int8_t STATUS_OK{0};
constexpr static int8_t STATUS_EMPTY{1};
constexpr static int8_t STATUS_FULL{2};


class mmap_ring_buffer {
public:
  mmap_ring_buffer(size_t max_bytes, void* buffer) 
    : max_bytes(max_bytes), buffer(buffer) {}
  ~mmap_ring_buffer() = default;

  size_t capacity_bytes() const noexcept;
  size_t get_size() const noexcept;
  bool has_capacity(size_t data_size) const noexcept;

  write_status put(void* data, size_t length) noexcept;
  read_status get(void* recv_buf, size_t length) noexcept;

  /*void debug() {
    printf("buffer: %p\n", buffer);
    printf("head: %p %li\n", &head, head);
    printf("tail: %p %li\n", &tail, tail);
    printf("size: %p %li\n", &size, size);
    printf("max size: %li\n", max_bytes);
  }*/

private:

  void get_buffer(void* data, size_t data_size, bool pop=false) noexcept;
  void put_buffer(void* recv_buf, size_t data_size) noexcept;

  const size_t max_bytes;
  volatile size_t size{0};
  volatile size_t head{0};
  volatile size_t tail{0};
  process_mutex mutex{};
  void* buffer{nullptr};
};

// must be true to be allocated on mmap
static_assert(std::is_trivially_copyable<process_mutex>::value);
static_assert(std::is_trivially_copyable<mmap_ring_buffer>::value);

} // namespace ring_buffer

