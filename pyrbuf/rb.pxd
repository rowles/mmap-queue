
cdef extern from "ring_buffer.h":
    cdef cppclass ring_buffer:
        int capacity_bytes()
        size_t get_size()
        void debug()
        write_status put(unsigned char* data, size_t length) nogil
        read_status get(unsigned char* data, size_t length) nogil
    
    cdef struct read_status:
        int code
        int length

    cdef struct write_status:
        int code

