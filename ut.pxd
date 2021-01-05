
cimport rb
cimport mmap

cdef extern from "utils.h":
    rb.ring_buffer* from_mem()
    rb.ring_buffer* from_mmap(mmap.mmap_t* region)
    int pizza()
