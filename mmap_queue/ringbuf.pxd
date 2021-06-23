from libcpp.string cimport string


cdef extern from "ring_buffer.h" namespace "ring_buffer":
    
    cdef struct read_status:
        int status
        int length

    cdef struct write_status:
        int status
    
    cdef cppclass mmap_ring_buffer:
        int capacity_bytes()
        size_t get_size()
        write_status put(unsigned char* data, size_t length) nogil
        read_status get(unsigned char* data, size_t length) nogil

    int STATUS_OK "ring_buffer::STATUS_OK"
    int STATUS_EMPTY "ring_buffer::STATUS_EMPTY"
    int STATUS_FULL "ring_buffer::STATUS_FULL"


cdef extern from "mmapped.h" namespace "mmapped":
    cdef enum mode_t 'mmapped::mmap_region':
        RO 'mmapped::mmap_region::mode_t::RO'
        CR 'mmapped::mmap_region::mode_t::CR'
        SHARED 'mmapped::mmap_region::mode_t::SHARED'
        ANON 'mmapped::mmap_region::mode_t::ANON'

    cdef cppclass mmap_region:
        mmap_region(mode_t _mode, size_t len)
        mmap_region(string path, mode_t _mode, size_t len)
        void open()
        void sync()
        void close()


cdef extern from "utils.h" namespace "utils":
    mmap_ring_buffer* from_mmap(mmap_region* region)
    mmap_ring_buffer* from_ptr(void* region, size_t length)


