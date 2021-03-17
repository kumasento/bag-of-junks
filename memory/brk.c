#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  void *buf;
  printf("brk = %p\n", sbrk(0));

  // Internal fragmentation.
  buf = malloc(sizeof(char));
  printf("buf = %p\n", buf);
  buf = malloc(sizeof(char));
  printf("buf = %p\n", buf);

  buf = malloc(sizeof(char) * 32 * 1024L * 1024L * 1024L);
  printf("buf = %p\n", buf);
  buf = malloc(sizeof(char) * 32 * 1024L * 1024L * 1024L);
  printf("buf = %p\n", buf);
  buf = malloc(sizeof(char) * 32 * 1024L * 1024L * 1024L);
  printf("buf = %p\n", buf);
  printf("brk = %p\n", sbrk(0));

  return 0;
}
