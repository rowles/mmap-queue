import pybuf
from multiprocessing import Process

q = pybuf.Queue()

import time
s=time.time()
for i in range(200000*2):
    q.put(b'this is mine')
e0 = time.time()
def test(myq, i):
    #print('test', i)
    #print('test', myq)
    #print('testrg', myq.ring_buf)
    #q.ring_buf.debug()
    for i in range(200000):
        #q.put(b'this is mine')
        q.get()

    #print('size', i, q.size())
    #print('testrg-done', myq.ring_buf)
    #q.ring_buf.debug()

n = 2
procs = []
for i in range(2):
    p = Process(target=test, args=(q,i))
    p.start()
    procs.append(p)
    procs[i].join()

for i in range(2):
    procs[i].join()
print(time.time()-s)
print(e0-s)
#r = q.get()
#print(r.tobytes())
#r = q.get()
#print(r.tobytes())

#for i in range(200000):
#    r = q.put(b'the sky is falling ')
#r = q.get()
#print(r.tobytes())
#print(q.size())
q.ring_buf.debug()
