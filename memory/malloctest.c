#include <assert.h>
#include <stdio.h>

#include "malloc.h"

extern void mem_init(void);
extern void *mem_sbrk(int);

extern int mm_init(void);
extern void mm_free(void *);
extern void *mm_malloc(size_t);

int main(int argc, char *argv[]) {
  char *bp;

  mem_init();

  printf("Testing initialized memory ...\n");
  printf("mem_heap: %p\n", mem_sbrk(0));
  printf("mem_brk:  %p\n", mem_sbrk(0));
  printf("\n");

  printf("Testing heap size ...\n");
  assert(mem_sbrk(MAX_HEAP + 1) == (void *)-1);
  printf("\n");

  printf("Testing macors ...\n");

  unsigned int unallocated = 0x27D50, allocated = 0x67CA1;

  assert(GET_SIZE(&unallocated) == 0x27D50);
  assert(!GET_ALLOC(&unallocated));
  assert(GET_SIZE(&allocated) == 0x67CA0);
  assert(GET_ALLOC(&allocated));

  printf("PASSED\n");
  printf("\n");

  // --------- Memory allocator test ----------
  printf("Testing mm_init() ...\n");
  void *old_brk = mem_sbrk(0);
  assert(mm_init() == 0);

  printf("mem_brk = %p\n", mem_sbrk(0));
  printf("heap extended by %lu bytes\n",
         ((char *)mem_sbrk(0) - (char *)old_brk));
  assert(((char *)mem_sbrk(0) - (char *)old_brk) == CHUNKSIZE + 4 * WSIZE);
  printf("\n");

  printf("Testing mm_malloc() ...\n");
  bp = mm_malloc(WSIZE);
  // The first block should be allocated at 2 * DSIZE after the initial brk.
  // One DSIZE for the padding and header of prologue,
  // one DSIZE for the footer of the prologue and the new header.
  assert(bp == old_brk + (WSIZE << 2));
  bp = mm_malloc(WSIZE);
  assert(bp == old_brk + (WSIZE << 2) + (DSIZE << 1));
  bp = mm_malloc(4 * WSIZE);
  assert(bp == old_brk + (WSIZE << 2) + (DSIZE << 1) + (DSIZE << 1));
  bp = mm_malloc(4 * WSIZE);
  assert(bp ==
         old_brk + (WSIZE << 2) + (DSIZE << 1) + (DSIZE << 1) + 3 * DSIZE);

  // Freed space is big enough.
  mm_free(old_brk + (WSIZE << 2));
  bp = mm_malloc(WSIZE);
  assert(bp == old_brk + (WSIZE << 2));

  // Freed space is not enough.
  mm_free(old_brk + (WSIZE << 2));
  bp = mm_malloc(4 * WSIZE);
  assert(bp != old_brk + (WSIZE << 2));

  // Should trigger new allocaton.
  old_brk = mem_sbrk(0);
  bp = mm_malloc(CHUNKSIZE);
  assert(mem_sbrk(0) != old_brk);

  printf("PASSED\n");
  printf("\n");

  return 0;
}
