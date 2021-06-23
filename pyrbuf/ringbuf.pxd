from libcpp.string cimport string


cdef extern from "ring_buffer.h" namespace "ring_buffer":
    
    cdef struct read_status :
        int code
        int length

    cdef struct write_status :
        int code
    
    cdef cppclass mmap_ring_buffer:
        int capacity_bytes()
        size_t get_size()
        write_status put(unsigned char* data, size_t length) nogil
        read_status get(unsigned char* data, size_t length) nogil


cdef extern from "mmapped.h" namespace "mmapped":
    cdef enum Mode 'mmapped::mmap_region':
        RO 'mmapped::mmap_region::Mode::RO'
        CR 'mmapped::mmap_region::Mode::CR'
        SHARED 'mmapped::mmap_region::Mode::SHARED'
        ANON 'mmapped::mmap_region::Mode::ANON'

    cdef cppclass mmap_region:
        mmap_region(Mode _mode, size_t len)
        mmap_region(string path, Mode _mode, size_t len)
        void open()
        void sync()
        void close()


cdef extern from "utils.h" namespace "utils":
    mmap_ring_buffer* from_mmap(mmap_region* region)
    mmap_ring_buffer* from_ptr(void* region, size_t length)


