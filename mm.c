#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in below _AND_ in the
 * struct that follows.
 *
 * === User information ===
 * Group: 
 * User 1: 
 * SSN: X
 * User 2: 
 * SSN: X
 * === End User Information ===
 ********************************************************/
team_t team = {
    /* Group name */
    "Malloc_me_baby",
    /* First member's full name */
    "Hjalti Leifsson",
    /* First member's email address */
    "Hjaltil13@ru.is",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    "",
    /* Leave blank */
    "",
    /* Leave blank */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

// Shorthand for struct header
typedef struct header blockHeader;

// The header in each block is a struct
struct header {
    size_t size;
    blockHeader *next_p;
    blockHeader *prev_p;
};

#define BLOCK_HEADER_SIZE ALIGN(sizeof(blockHeader))

// Helper functions
void *first_fit(size_t size);
void place(blockHeader *bp, size_t size);


/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // Allocate space for the size of the header.
    blockHeader *bp = mem_sbrk(BLOCK_HEADER_SIZE);
    
    bp->size = BLOCK_HEADER_SIZE;
    
    // Set its next and prev to itself,
    // So we get a circular list right away
    bp->next_p = bp;
    bp->prev_p = bp;

    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // Since we need extra space for the block header,
    // add the size of the block header to the size param,
    // and align the result.
    int newsize = ALIGN(BLOCK_HEADER_SIZE + size);

    // Ignore spurious requests
    if(size == 0){
        return NULL;
    }

    // Find the first block that is big enough.
    blockHeader *bp = first_fit(newsize);

    if(bp == NULL) { // No block was found, request more heap memory.
        
        bp = mem_sbrk(newsize);
    
        if((long)bp == -1){
            // If no more heap memory, shut down.
            return NULL;
        }
        else {
            // Found heap memory, set the size to newsize
            // and mark it as allocated
           bp->size = newsize | 1;
        }
    }
    else { // A block was found.

        bp->size |= 1; 
        bp->prev_p->next_p = bp->next_p;
        bp->next_p->prev_p = bp->prev_p;

    }
    return (char *)bp + BLOCK_HEADER_SIZE;    
}

void place(blockHeader *bp, size_t size){
    
}

void *first_fit(size_t size)
{
    // Set a dummy pointer to the first block on the list.
    blockHeader *p = ((blockHeader *)mem_heap_lo())->next_p;

    // Since we have a circular list, advance the list
    // until we hit a block whose size is greater
    // than the needed size, and not the first 
    // block on the list.
    while(p != mem_heap_lo() && (p->size < size)){
        p = p->next_p;
    }

    // Make sure we are not returning the first item on the list.
    if(p != mem_heap_lo()){
        return p;
    } else {
        return NULL;
    }
}
/*
 * 
 */
void mm_free(void *ptr)
{

    if(!ptr){
        return;
    }
    blockHeader *bp = ptr-BLOCK_HEADER_SIZE;
    blockHeader *head = mem_heap_lo();

    bp->size &= ~1;
    bp->next_p = head->next_p;
    bp->prev_p = head;
    head->next_p = bp;
    bp->next_p->prev_p = bp;

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    blockHeader *bp = ptr - BLOCK_HEADER_SIZE;
    void *newptr = mm_malloc(size);
    
    if(newptr == NULL)
    {
        return NULL;
    }
    int copySize = bp->size - BLOCK_HEADER_SIZE;
    if(size < copySize)
    {
        copySize = size;
    }
    memcpy(newptr, ptr, copySize);
    mm_free(ptr);
    return newptr;
}
