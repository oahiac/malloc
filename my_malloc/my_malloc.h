/* The header file includes functions prototypes:
 * ff_malloc
 * bf_malloc
 * ff_free
 * bf_free
 * get_largest_free_data_segment_size
 * get_total_free_size
 * Helper Function prototypes:
 * update_max_free_block_size
 * split_block
 * extend_heap
 * my_free
 */
#ifndef __MY_ALLOC_H__
#define __MY_ALLOC_H__


typedef struct _block {
  size_t size;
  int is_free;
  struct _block * prev;
  struct _block * next;
} block ;

#define BLOCK_INFO_SIZE sizeof(block)

/* First Fit malloc */
void * ff_malloc(size_t size);

/* Best Fit malloc */
void * bf_malloc(size_t size);

/* First Fit free */
void ff_free(void* ptr);

/* Best Fit free */
void bf_free(void* ptr);

/* For Performance study report */
unsigned long get_largest_free_data_segment_size();
unsigned long get_total_free_size();

void check_memory();
void check_free();


#endif
