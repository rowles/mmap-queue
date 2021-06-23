#include "ring_buffer.h"

namespace ring_buffer {

size_t mmap_ring_buffer::capacity_bytes() const noexcept {
  return max_bytes;
}

size_t mmap_ring_buffer::get_size() const noexcept {
  return size;
}

bool mmap_ring_buffer::has_capacity(size_t data_size) const noexcept {
  return size + data_size <= max_bytes;
}

write_status mmap_ring_buffer::put(void* data, size_t length) noexcept {
  process_mutex::guard g{&mutex};

  if (!has_capacity(length + sizeof(size_t)))
    return write_status{.status=STATUS_FULL};

  // put the header and data
  put_buffer((void*)&length, sizeof(length));
  put_buffer(data, length);

  return write_status{.status=STATUS_OK};
}


read_status mmap_ring_buffer::get(void* recv_buf, size_t length) noexcept {
  process_mutex::guard g{&mutex};

  // queue is empty
  if (size <= 0) {
    return read_status{.status=STATUS_EMPTY};
  }

  // read size of data first
  size_t data_size{0};
  get_buffer((void*)&data_size, sizeof(data_size), false);

  // compare to recieve buffer
  if (data_size > length) {
    return read_status{.status=STATUS_FULL, .length=data_size};
  }

  // get the data
  get_buffer(recv_buf, data_size+sizeof(size_t), true);

  return read_status{.status=STATUS_OK, .length=data_size};
}

void mmap_ring_buffer::get_buffer(void* data, size_t data_size, bool pop) noexcept {
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

void mmap_ring_buffer::put_buffer(void* recv_buf, size_t data_size) noexcept {
  const bool wrap = tail + data_size >= max_bytes;

  if (!wrap) {
    std::memcpy(buffer + tail, recv_buf, data_size);
    tail += data_size;
  }

  size += data_size;
}


} // namespace ring_buffer
