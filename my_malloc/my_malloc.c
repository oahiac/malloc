/* This library contains functions to malloc and free
 * in both first fit logic and best fit logic.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "my_malloc.h"

void * bds = NULL; /* base data segment */

/* To judge if the prev block and next block is adjacent
 * Return 1 if adjacent
 * Return 0 if not
 */
int
is_adjacent(
    const block * prev,
    const block * next
    ) {
  void * p = (void*) prev;
  void * n = (void*) next;
  size_t size = prev->size;
  assert(prev != NULL && next != NULL);
  if (p + BLOCK_INFO_SIZE + size == n) return 1;
  else return 0; 
}

/* To merge the three adjacent free blocks */
void
merge(
    block * curr,
    block * prev,
    block * next) {
  if (
      prev != (block*)bds &&
      next != NULL &&
      is_adjacent(prev, curr) &&
      is_adjacent(curr, next)
     ) {
    /* merge three block */
    prev->size += 2*BLOCK_INFO_SIZE + curr->size + next->size;
    prev->is_free = 1;
    prev->next = next->next;
    if (next->next) {
      next->next->prev = prev;
    }
 }
  else if (
      prev != (block*)bds &&
      is_adjacent(prev, curr)
      ) {
    /* merge prev and curr */
    prev->size += BLOCK_INFO_SIZE + curr->size;
    prev->is_free = 1;
    prev->next = next;
    if (next) {
      next->prev = prev;
    }

  }
  else if (
      next != NULL &&
      is_adjacent(curr, next)
      ) {
    /* merge curr and next */
    curr->size += BLOCK_INFO_SIZE + next->size;
  curr->is_free = 1;
  curr->prev = prev;
  curr->next = next->next;
  prev->next = curr;
  if (next->next) next->next->prev = curr;
 
  }
  else {
    /* do not merge */
    curr->is_free = 1;
    curr->prev = prev;
    curr->next = next;
    prev->next = curr;
    if (next) next->prev = curr;
  }
}

/* To create the initial head block */
void
initialize_heap() {
  bds = sbrk(BLOCK_INFO_SIZE);
  block * head = (block*)bds;
  head->size = 0;
  head->is_free = 1;
  head->prev = NULL;
  head->next = NULL;
}

/* To split the block and set the second part as free */
void
split_block(
    block * curr,
    size_t size
    ) {
  assert(curr != NULL);
  assert(curr->is_free);
  assert(curr->size >= size);

  /* no need to split, just change flag */
  if (curr->size >= size && 
      curr->size <= size + BLOCK_INFO_SIZE) {
    curr->is_free = 0;
    curr->prev->next = curr->next;
    if (curr->next) {
      curr->next->prev = curr->prev;
    }
  }
  else {
    /* split the block */
    void * start_ptr = (void*) curr;
    block * new_block = (block*)(start_ptr + BLOCK_INFO_SIZE + size);
    block * prev = curr->prev;
    block * next = curr->next;

    new_block->size = curr->size - size - BLOCK_INFO_SIZE;
    new_block->is_free = 1;
    new_block->prev = prev;
    new_block->next = next;

    curr->size = size;
    curr->is_free = 0;

    prev->next = new_block;
    if (next) {
      next->prev = new_block;
    }
  }

}

/* extend the top of the heap 
 * Return the address of the new allocated memory is success
 * Return NULL if fail
 */
void *
extend_heap(size_t size) {
  void * extend_res = sbrk(BLOCK_INFO_SIZE + size);
  if (extend_res == (void*)(-1)) {
    return NULL;
  }
  block * new_block = extend_res;
  new_block->size = size;
  new_block->is_free = 0;
  new_block->prev = NULL;
  new_block->next = NULL;
  return (extend_res + BLOCK_INFO_SIZE);
}

/* To iterate through the free block linked list to find the block and change the flag
 */
void
my_free(void * ptr) {
  if (ptr == NULL) return;
  block * curr = (block*)(ptr - BLOCK_INFO_SIZE);
  if (curr->is_free) return;

  block * iter = bds;
  while (iter) {
    if (iter->next == NULL) break;
    else if (iter->next > curr) break;
    else if (iter == iter->next) break;
    else iter = iter->next;
  }
  assert(iter != NULL);
  assert(iter <= curr && (iter->next > curr || iter->next == NULL));
  assert(iter->is_free);
  if (iter->next) assert(iter->next->is_free);
  merge(curr, iter, iter->next);
}

/* To find the first fit block to allocate */
void *
ff_malloc(size_t size) {
  if (bds == NULL) initialize_heap();
  block * iter = (block*)bds;
  while (iter) {
    if (iter->size >= size) break;
    iter = iter->next;
  }
  if (iter == NULL) {
    return extend_heap(size);
  }
  else {
    split_block(iter, size);
    return iter+1;
  }
}

/* To find the best fit block to allocate */
void *
bf_malloc(size_t size) {
  if (bds == NULL) initialize_heap();
  block * iter = (block*)bds;
  int redundancy = INT_MAX;
  block * best = NULL;

  while (iter) {
    if (iter->size == size) {
      best = iter;
      break;
    }
    if (iter->size >= size &&
        iter->size - size < redundancy) {
      redundancy = iter->size - size;
      best = iter;
    }
    iter = iter->next;
  }
  if (best == NULL) {
    return extend_heap(size);
  }
  else {
    split_block(best, size);
    return best+1;
  }
}

void
ff_free(void * ptr) {
  my_free(ptr);
}

void
bf_free(void * ptr) {
  my_free(ptr);
}

unsigned long
get_largest_free_data_segment_size() {
  block * iter = bds;
  unsigned long res = 0;
  while (iter) {
    if (res < iter->size) res = iter->size;
    iter = iter->next;
  }
  return res;

}

unsigned long
get_total_free_size() {
  block * iter = bds;
  size_t res = 0;
  while (iter) {
    res += iter->size;
    iter = iter->next;
  }
  return res;
}










