#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd;
  int i;
  int ret;
  off_t fsize;

  assert(argc >= 2);

  printf("Openning file: %s\n", argv[1]);
  if ((fd = open(argv[1], O_RDONLY)) == -1) {
    fprintf(stderr, "File %s cannot be opened\n", argv[1]);
    return -1;
  }
  printf("Opened, fd = %d\n", fd);

  fsize = lseek(fd, 0L, SEEK_END);
  printf("File size: %lld bytes\n", fsize);

  // Allocate a virtual memory space and map the file content to that area.
  void *bufp = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
  printf("Allocated buffer: %p\n", bufp);

  // Buffer content byte by byte
  for (i = 0; i < fsize; i++) {
    printf("%c", *((char *)bufp + i));
  }

  if ((ret = munmap(bufp, fsize)) == -1) {
    fprintf(stderr, "Error unallocating virtual memory: %p\n", bufp);
    exit(-1);
  }

  return 0;
}
