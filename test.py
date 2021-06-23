import mmap_queue

import pytest
import os
import queue as buildin_q


def test_put_get():
    q = mmap_queue.RingBuffer()

    q.put_bytes(b'the')
    q.put_bytes(b'sky')
    q.put_bytes(b'is')
    q.put_bytes(b'falling')

    assert q.size() == 47
    
    assert q.get_bytes().tobytes() == b'the'
    assert q.get_bytes().tobytes() == b'sky'
    assert q.get_bytes().tobytes() == b'is'
    assert q.get_bytes().tobytes() == b'falling'

    assert q.size() == 0


def test_fifo():
    q = mmap_queue.RingBuffer()

    q.put_bytes(b'1')
    q.put_bytes(b'2')
    q.put_bytes(b'3')

    assert q.size() == (8 + 1) * 3

    assert q.get_bytes() == b'1'
    assert q.get_bytes() == b'2'
    assert q.get_bytes() == b'3'

    assert q.size() == 0


def test_empty():
    q = mmap_queue.RingBuffer()

    with pytest.raises(buildin_q.Empty):
        q.get_bytes()

    assert q.size() == 0


def test_full():
    q = mmap_queue.RingBuffer(100)

    with pytest.raises(buildin_q.Full):
        q.put_bytes(b'a'*1000)

    assert q.size() == 0


def test_filebacked():
    testfile = '/tmp/test.buf'

    q = mmap_queue.RingBuffer(1000, file_path=testfile)

    assert os.path.isfile(testfile)

    q.put_bytes(b'123456')
    assert q.get_bytes() == b'123456'

    del q

    assert not os.path.isfile(testfile)


