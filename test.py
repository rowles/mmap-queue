import pybuf

import pytest
import queue as buildin_q


def test_put_get():
    q = pybuf.PyRingBuf()

    q.put_bytes(b'the')
    q.put_bytes(b'sky')
    q.put_bytes(b'is')
    q.put_bytes(b'falling')

    assert q.size() == 47
    
    assert q.get_bytes().tobytes() == b'the'
    assert q.get_bytes().tobytes() == b'sky'
    assert q.get_bytes().tobytes() == b'is'
    assert q.get_bytes().tobytes() == b'falling'



def test_fifo():
    q = pybuf.PyRingBuf()

    q.put_bytes(b'1')
    q.put_bytes(b'2')
    q.put_bytes(b'3')

    assert q.size() == (8 + 1) * 3

    assert q.get_bytes() == b'1'
    assert q.get_bytes() == b'2'
    assert q.get_bytes() == b'3'

    assert q.size() == 0


def test_empty():
    q = pybuf.PyRingBuf()

    with pytest.raises(buildin_q.Empty):
        q.get_bytes()


