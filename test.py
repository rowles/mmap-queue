import pybuf
q = pybuf.Queue()

for i in range(200000):
    r = q.put(b'the sky is falling ')
#r = q.get()
#print(r.tobytes())
print(q.size())
q.ring_buf.debug()
