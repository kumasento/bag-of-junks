#include "malloc.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --------------- Memory modelling -----------------
static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;

void mem_init(void) {
  mem_heap = (char *)malloc(sizeof(char) * MAX_HEAP);
  mem_brk = (char *)mem_heap;
  mem_max_addr = (char *)(mem_heap + MAX_HEAP);
}

void *mem_sbrk(int incr) {
  char *old_brk = (char *)mem_brk;

  if ((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
    errno = ENOMEM;
    fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory ...\n");
    return (void *)-1;
  }

  mem_brk += incr;
  return (void *)old_brk;
}

// --------------- Memory allocate -----------------
static char *heap_listp;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void place(void *bp, size_t size);

int mm_init(void) {
  // Allocate 4 words: 1 for padding, 2 for prologue, 1 for epilogue header.
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1) return -1;

  // Prologue and epilogue blocks are all allocated.
  PUT(heap_listp, 0);
  PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  // header: size DSIZE.
  PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  // footer
  PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      // epilogue of size 0.
  heap_listp += (2 * WSIZE);  // Point to the blkp of the prologue block.

  // If cannot extend the heap by CHUNKSIZE of bytes.
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL) return -1;

  return 0;
}

// Assuming that bp is an allocated block.
void mm_free(void *bp) {
  size_t size;

  size = GET_SIZE(HDRP(bp));
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));

  coalesce(bp);
}

void *mm_malloc(size_t size) {
  size_t asize;       // Adjusted block size
  size_t extendsize;  // To extend heap.
  char *bp;

  if (size == 0) return NULL;

  // One DSIZE is for the header and footer of the new block.
  // The other one is for the required space.
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);

  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
  place(bp, asize);
  return bp;
}

static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  // Failed to extend the heap by size "size"
  if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

  // The new block is freed.
  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  // new epilogue (allocated).

  return coalesce(bp);
}

/// Coalesce the current block with the previous one?
// Will look at the blocks before and after.
static void *coalesce(void *bp) {
  // PREV_BLKP uses the footer word.
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  // Cannot coalesce
  if (prev_alloc && next_alloc) return bp;

  if (prev_alloc && !next_alloc) {
    // Enlarge block size, update the footer of the next block.
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));  // footer position is updated.
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(FTRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));  // footer position is updated.
    bp = PREV_BLKP(bp);
  } else {
    size += GET_SIZE(FTRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  return bp;
}

/// Implements the first-fit search.
static void *find_fit(size_t size) {
  char *bp = (char *)heap_listp;
  size_t curr_size;

  while (1) {
    curr_size = GET_SIZE(HDRP(bp));
    if (curr_size >= size && !GET_ALLOC(HDRP(bp))) return bp;
    if (curr_size == 0) return NULL;  // epilogue

    bp = NEXT_BLKP(bp);
  }

  return NULL;
}

static void place(void *bp, size_t size) {
  // bp is the current free block.
  size_t old_size = GET_SIZE(HDRP(bp));
  size_t new_size = old_size - size;

  PUT(HDRP(bp), PACK(size, 1));  // Allocated.
  PUT(FTRP(bp), PACK(size, 1));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(new_size, 0));
  PUT(FTRP(NEXT_BLKP(bp)), PACK(new_size, 0));
}
