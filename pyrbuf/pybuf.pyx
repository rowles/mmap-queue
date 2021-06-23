# cython: language_level=3
# cython: boundscheck=False
# cython: infer_types=False

from libcpp.vector cimport vector
import cython
import mmap as stdmmap

import ctypes

from libc.stdlib cimport malloc, free

cimport ringbuf

import os
import queue as std_queue


cdef size_t bytes_to_ptr(b):
    ptr = ctypes.cast(b, ctypes.POINTER(ctypes.c_byte))
    return ctypes.addressof(ptr.contents)


cdef class PyRingBuf:
    cdef ringbuf.mmap_ring_buffer *thisptr
    cdef ringbuf.mmap_region *mem_region

    cdef object file_path

    # reuseable buffer for message passing
    cdef object msg_buffer
    cdef size_t msg_buffer_len
    cdef object memview

    def __cinit__(self, size_t size=1024**3, str file_path=None, cleanup=True):
        """ Initialize a mmap backed ring buffer

        Parameters
        ----------
        size: size_t (default 1 MB)
          Size of ring buffer in bytes
        file_path: str, optional
          File path for a file backed mmap. Memory map will be anonymous if not provided.
        cleanup: bool, optional
          Clean file based mmap after collection
        """
        if file_path:
            self.file_path = file_path.encode('utf-8')
            self.mem_region = new ringbuf.mmap_region(self.file_path, ringbuf.Mode.SHARED, size)
        else:
            self.mem_region = new ringbuf.mmap_region(ringbuf.Mode.ANON, size)
        self.thisptr = ringbuf.from_mmap(self.mem_region)

        self.msg_buffer_len = 0 # Starting length of buffer in bytes
        self.msg_buffer = None
        self.memview = None

    def put_bytes(self, data):
        """
        Put bytes into buffer

        Parameters
        ----------
        data: bytes
          Byte sequence to put into buffer.
        """
        cdef unsigned char* casted_data = <unsigned char*>bytes_to_ptr(data)
        cdef size_t length = len(casted_data)
        res = self.thisptr.put(casted_data, length)

        if res['code'] == -1:
            raise std_queue.Full()

    def get_bytes(self):
        """
        Get bytes from buffer

        Returns
        -------
        memoryview of data
        """
        if self.msg_buffer is None:
            self._realloc_buffer(32)

        cdef ringbuf.read_status res
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
        if self.file_path:
            os.remove(self.file_path)

    def size(self):
        """
        Get size of queue in bytes
        
        Returns
        -------
        size of queue as bytes
        """
        return self.thisptr.get_size()


