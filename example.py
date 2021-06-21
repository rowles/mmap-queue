
A mmap backed RingBuffer for 


# create 50MB buffer backed by anonymous mmap
r = RingBuffer(maxsize=50*1024**2)

# create 50MB buffer backed by file-based mmap
r = RingBuffer(
    file_path='/tmp/mymagicringbuf',
    maxsize=50*1024**2,
    remove_on_close=True # delete file on dealloc
)

# common methods
# mirroring multiprocessing.Queue
# https://docs.python.org/3/library/multiprocessing.html#multiprocessing.Queue
r.qsize()
r.capacity()
r.full()

# get/put works with pickle serialization
r.put_nowait({'a':1, 'b': 5})
r.put(object(), timeout=10.5)
r.put(object(), block=True)

r.get_nowait()
r.get(timeout=10.5)
r.get(block=True)

# low level get/put bytes methods
r.put_bytes(b'abc')
r.get_bytes()

# closes mmap
# optionally removes mmap file
del r


