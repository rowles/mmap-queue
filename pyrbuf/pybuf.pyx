# cython: language_level=3
# cython: boundscheck=False
# cython: infer_types=False

from libcpp.vector cimport vector
import cython

import ctypes

from libc.stdlib cimport malloc, free

cimport rb
cimport ut
cimport mmap

import queue as std_queue


cdef size_t bytes_to_ptr(b):
    ptr = ctypes.cast(b, ctypes.POINTER(ctypes.c_byte))
    return ctypes.addressof(ptr.contents)


cdef class PyRingBuf:
    cdef rb.ring_buffer *thisptr
    cdef mmap.mmap_t *mem_region

    cdef object msg_buffer
    cdef size_t msg_buffer_len
    cdef object memview

    def __cinit__(self, size_t size=10000, str file_path=None):
        if file_path:
            self.mem_region = new mmap.mmap_t(file_path, mmap.Mode.SHARED, size)
        else:
            self.mem_region = new mmap.mmap_t(mmap.Mode.ANON, size)

        self.thisptr = ut.from_mmap(self.mem_region)

        self.msg_buffer_len = 100
        self.msg_buffer = (ctypes.c_ubyte * self.msg_buffer_len)()
        self.memview = memoryview(self.msg_buffer)

    def put_bytes(self, b):
        cdef unsigned char* data = <unsigned char*>bytes_to_ptr(b)
        cdef size_t length = len(data)

        res = self.thisptr.put(data, length)

        if res['code'] == -1:
            raise std_queue.Full()

    def get_bytes(self):
        cdef rb.read_status res
        cdef size_t new_size
        cdef size_t buffer_adr = ctypes.addressof(self.msg_buffer)
        cdef unsigned char* buffer_ptr = <unsigned char*> buffer_adr
        
        while True:
            with nogil:
                res = self.thisptr.get(buffer_ptr, self.msg_buffer_len)

            if res.code == -2:
                new_size = (int)((res.length+sizeof(size_t))*2)
                self._realloc_buffer(new_size)
            else:
                break

        if res.code == -1:
            raise std_queue.Empty()
        elif res.code == 0:
            view = self.memview[sizeof(size_t):res.length+sizeof(size_t)]
            return view

    cdef _realloc_buffer(self, size_t new_size):
        self.msg_buffer_len = new_size
        self.msg_buffer = (ctypes.c_ubyte * self.msg_buffer_len)()
        self.memview = memoryview(self.msg_buffer)

    def __dealloc__(self):
        del self.mem_region 

    def size(self):
        return self.thisptr.get_size()

    def debug(self):
        self.thisptr.debug()

