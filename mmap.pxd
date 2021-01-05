from libcpp.string cimport string

#cdef extern from "mapped.h" namespace "mapped::map_t":
#    cdef enum Mode:
#        RO
#        CR
#        SHARD
#        ANON

cdef extern from "mapped.h" namespace "mmapped":
    cdef enum Mode 'mmapped::mmap_t':
        RO 'mmapped::mmap_t::Mode::RO'
        CR 'mmapped::mmap_t::Mode::CR'
        SHARED 'mmapped::mmap_t::Mode::SHARED'
        ANON 'mmapped::mmap_t::Mode::ANON'

    cdef cppclass mmap_t:
        mmap_t(Mode _mode, size_t len)
        mmap_t(string path, Mode _mode, size_t len)
        void open()
        void sync()
        void close()


