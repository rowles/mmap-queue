#include "mapped.h"
#include "ring_buffer.h"


int main(int argc, char *argv[]) {
  B* buf = new B[100];
  buf[0] = 0;
  auto rbuf = ring_buffer(100, buf);

  char* msg0 = "this is a testing";
  rbuf.put((B*)msg0, sizeof(char)*strlen(msg0));

  char* msg1 = "second";
  rbuf.put((B*)msg1, sizeof(char)*strlen(msg1));
  /*printf("print\n");
  for(int i = 0; i < 99; i++) {
    buf[i];
    //printf("%i", buf[i]);
  }*/

  B* out = new B[100];

  auto res = rbuf.get(out, 100);
  char* e0 = (char*)out;
  printf("got: %s\n", e0);
  
  res = rbuf.get(out, 100);
  char* e1 = (char*)out;
  printf("got: %s\n", e1);
  return 0;
}
