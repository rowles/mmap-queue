import pybuf
q = pybuf.PyRingBuf()

for i in range(20):
    r = q.put_bytes(b'the sky is falling ')
#r = q.get()
#print(r.tobytes())
print(q.size())
q.debug()
