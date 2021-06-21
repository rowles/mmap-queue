import pybuf



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


test_put_get()
