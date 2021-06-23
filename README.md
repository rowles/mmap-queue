# mmap-queue
[![Unit Test](https://github.com/rowles/py-ringbuffer/actions/workflows/utest.yml/badge.svg)](https://github.com/rowles/py-ringbuffer/actions/workflows/utest.yml)

mmap backed ring buffer implemented in C++ with Cython bindings.

The mmap can be either shared anonymous or file-backed to allow the queue to exceed memory limits.

Ring buffers uses one lock (shared process mutex) for puts/gets.

Only tested using Linux mmap/mutex syscalls.

```
# Setup virtual environment
$ make setup_venv

# Build libringbuf and mmap_queue
$ make build

# Test python bindings
$ make utest

# Run benchmark.py
$ make benchmark
```

Not quite production ready. 
