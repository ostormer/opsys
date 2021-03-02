#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int has_initialized = 0;

// our memory area we can allocate from, here 64 kB
#define MEM_SIZE (64*1024)
uint8_t heap[MEM_SIZE];

// start and end of our own heap memory area
void *managed_memory_start; 

// this block is stored at the start of each free and used block
struct mem_control_block {
  int size;
  struct mem_control_block *next;
};

// pointer to start of our free list
struct mem_control_block *free_list_start;      

void mymalloc_init() { 

  // our memory starts at the start of the heap array
  managed_memory_start = &heap;

  // allocate and initialize our memory control block 
  // for the first (and at the moment only) free block
  struct mem_control_block *m = (struct mem_control_block *)managed_memory_start;
  m->size = MEM_SIZE - sizeof(struct mem_control_block);

  // no next free block
  m->next = (struct mem_control_block *)0;

  // initialize the start of the free list
  free_list_start = m;

  // We're initialized and ready to go
  has_initialized = 1;
}

void *mymalloc(long numbytes) {
  if (has_initialized == 0) {
     mymalloc_init();
  }
  long size = free_list_start->size;
  struct mem_control_block *block_to_replace = free_list_start;
  struct mem_control_block *prev_block = free_list_start;

  while (size < numbytes + sizeof(struct mem_control_block)){
    prev_block = block_to_replace;
    block_to_replace = block_to_replace->next;
    size = block_to_replace->size;
    printf("Inne i while prev %p og next %p \n", prev_block, block_to_replace);
  }

  if (prev_block->next == (struct mem_control_block *)0){
    printf("Prev free list next er  %p \n", prev_block->next);
    void *p = (void *)prev_block;
    printf("p %p \n", p);
    prev_block->next = p + numbytes + (8-numbytes%8);
    printf("Prev block %p \n", prev_block->next);
  }
  else{
    void *p = (void *)prev_block;
    printf("p %p \n", p);
    prev_block->next = p + numbytes + (8-numbytes%8);
    printf("Prev free list next er  %p \n", prev_block->next);
  }

  struct mem_control_block *new = (struct mem_control_block *) prev_block->next;
  new->size = block_to_replace->size-numbytes;
  new->next = (struct mem_control_block*)block_to_replace->next;
}

void myfree(void *firstbyte) {

  /* add your code here! */

}

int main(int argc, char **argv) {
    mymalloc(3);
    mymalloc(5);

  }