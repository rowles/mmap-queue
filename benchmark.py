import concurrent.futures
import functools
import time
import queue

import mmap_queue


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


def bench_sum_workers(num_procs, num_objects):
    expected = 0

    start_ts = time.time()
    for n in range(num_objects):
        payload = str(n+1).encode()
        q.put_bytes(payload)
        expected += n+1
    end_ts = time.time()

    executor = concurrent.futures.ProcessPoolExecutor(max_workers=num_procs)

    procs = []
    for _ in range(num_procs):
        p = executor.submit(sum_worker,)
        procs.append(p)

    total = 0

    for _ in range(num_procs):
        res = procs[_].result()
        total += res

    assert total == expected


def bench_inserts(num_objects):
    test_object = b'1'*32
    start_ts = time.time()

    for _ in range(num_objects):
        q.put_bytes(test_object)

    end_ts = time.time()
    elasped = end_ts-start_ts
    insert_rate = num_objects/elasped
    mb_size = (len(test_object) * num_objects) / (1024**2)
    bytes_rate = mb_size / elasped
    print(f'inserted {num_objects} in {elasped:.6f}s, {num_objects/elasped:.2f} inserts/second, '
            f'{mb_size:.2f} MB, {bytes_rate:.2f} MB/s')


def put_bytes(test_object, num_objects):
    for _ in range(num_objects):
        q.put_bytes(test_object)


def batch_inserts_multiprocess(num_procs, num_objects, payload_size=32):
    executor = concurrent.futures.ProcessPoolExecutor(max_workers=num_procs)

    procs = []
    test_object = b'1'*payload_size

    start_ts = time.time()

    for _ in range(num_procs):
        p = executor.submit(put_bytes, test_object, num_objects // num_procs)
        procs.append(p)

    for _ in range(num_procs):
        procs[_].result()
    
    end_ts = time.time()
    elasped = end_ts-start_ts
    insert_rate = num_objects/elasped
    mb_size = (len(test_object) * num_objects) / (1024**2)
    bytes_rate = mb_size / elasped
    print(f'{num_procs} workers inserted {num_objects} in {elasped:.6f}s, {num_objects/elasped:.2f} inserts/second, '
            f'{mb_size:.2f} MB, {bytes_rate:.2f} MB/s')


if __name__ == '__main__':
    # use queue on global scope to prevent pickling
    print('bench_sum_workers anonymous mmap 1M')
    q = mmap_queue.RingBuffer(1*1024**2)
    bench_sum_workers(num_procs=2, num_objects=1000)

    del q
    
    print('-'*30)
    print('bench_sum_workers file-based mmap 1M')
    q = mmap_queue.RingBuffer(50*1024**2, file_path='bench.buf')
    bench_sum_workers(num_procs=2, num_objects=1_000_000)

    del q

    print('-'*30)
    print('bench_inserts file-based mmap 1M, 32 B payload')
    q = mmap_queue.RingBuffer(500*1024**2, file_path='bench.buf')
    bench_inserts(1_000_000)
    
    del q

    print('-'*30)
    print('batch_inserts_multiprocess anonymous mmap, 32 B payload')
    q = mmap_queue.RingBuffer(500*1024**2, )
    batch_inserts_multiprocess(4, 1_000_000)

    del q

    print('-'*30)
    print('batch_inserts_multiprocess file-backed mmap, 32 B payload')
    q = mmap_queue.RingBuffer(500*1024**2, file_path='bench.buf')
    batch_inserts_multiprocess(4, 1_000_000)

    del q

    print('-'*30)
    print('batch_inserts_multiprocess anonymous mmap 1 KB payload')
    q = mmap_queue.RingBuffer(500*1024**2)
    batch_inserts_multiprocess(4, 100_000, payload_size=1024)

    del q

    print('-'*30)
    print('batch_inserts_multiprocess file-backed mmap 1 KB payload')
    q = mmap_queue.RingBuffer(500*1024**2, file_path='bench.buf')
    batch_inserts_multiprocess(4, 100_000, payload_size=1024)

    print('-'*30)
    print('batch_inserts_multiprocess anonymous mmap 10 KB payload')
    q = mmap_queue.RingBuffer(500*1024**2)
    batch_inserts_multiprocess(4, 10000, payload_size=10*1024)

    del q

    print('-'*30)
    print('batch_inserts_multiprocess file-backed mmap 10 KB payload')
    q = mmap_queue.RingBuffer(500*1024**2, file_path='bench.buf')
    batch_inserts_multiprocess(4, 10000, payload_size=10*1024)





