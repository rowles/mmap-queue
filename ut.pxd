
cimport rb
cdef extern from "utils.h":
    rb.ring_buffer* from_mem()
    rb.ring_buffer* from_mmap()
    int pizza()
