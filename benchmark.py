import concurrent.futures
import functools
import time
import queue

import pybuf


def sum_worker():
    my_sum = 0
    while True:
        try:
            res = q.get_bytes().tobytes()
            my_sum += int(res)
            #time.sleep(0.001)
        except queue.Empty:
            break
    return my_sum


def setup(num_procs, num_objects):
    """ test anonymous mmap backed buffer
    """

    expected = 0

    start_ts = time.time()
    for n in range(num_objects):
        payload = str(n+1).encode()
        q.put_bytes(payload)
        expected += n+1
    end_ts = time.time()
    print(f'Added {num_objects} in {end_ts-start_ts:.6f}s')

    executor = concurrent.futures.ProcessPoolExecutor(max_workers=num_procs)

    fn = functools.partial(sum_worker)

    procs = []
    for _ in range(num_procs):
        p = executor.submit(fn,)
        procs.append(p)

    total = 0

    for _ in range(num_procs):
        res = procs[_].result()
        total += res

    assert total == expected


def bench_inserts(num_objects):
    start_ts = time.time()

    test_object = b'1'*32

    for _ in range(num_objects):
        q.put_bytes(test_object)
    end_ts = time.time()
    elasped = end_ts-start_ts
    print(f'Added {num_objects} in {elasped:.6f}s, {num_objects/elasped:.2f} inserts/second')



if __name__ == '__main__':
    # use queue on global scope to prevent pickling
    q = pybuf.PyRingBuf(1*1024**3)
    setup(num_procs=5, num_objects=1000)

    del q
    
    q = pybuf.PyRingBuf(50*1024**3, file_path='/tmp/bench.buf')
    setup(num_procs=5, num_objects=1_000_000)

    del q

    q = pybuf.PyRingBuf(50*1024**3, file_path='/tmp/bench.buf')
    bench_inserts(10_000_000)
    
    del q

    #q = pybuf.PyRingBuf(50*1024**3)
    #bench_inserts(10_000_000)



