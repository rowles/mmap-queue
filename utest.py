import pybuf
import pytest
import queue as buildin_q


def test_fifo():
    q = pybuf.Queue()

    q.put(b'1')
    q.put(b'2')
    q.put(b'3')

    assert q.size() == (8 + 1) * 3

    assert q.get() == b'1'
    assert q.get() == b'2'
    assert q.get() == b'3'

    assert q.size() == 0

def test_2():
    q = pybuf.Queue()

    with pytest.raises(buildin_q.Empty):
        q.get()


test_fifo()
test_2()
print('good')
