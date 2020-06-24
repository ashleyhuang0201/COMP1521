// COMP1521 19t2 ... Assignment 2: heap management system

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "myHeap.h"

/** minimum total space for heap */
#define MIN_HEAP 4096
/** minimum amount of space for a free Chunk (excludes Header) */
#define MIN_CHUNK 32


#define ALLOC 0x55555555
#define FREE  0xAAAAAAAA

/// Types:

typedef unsigned int  uint;
typedef unsigned char byte;

typedef uintptr_t     addr; // an address as a numeric type

/** The header for a chunk. */
typedef struct header {
	uint status;    /**< the chunk's status -- ALLOC or FREE */
	uint size;      /**< number of bytes, including header */
	byte data[];    /**< the chunk's data -- not interesting to us */
} header;

/** The heap's state */
struct heap {
	void  *heapMem;     /**< space allocated for Heap */
	uint   heapSize;    /**< number of bytes in heapMem */
	void **freeList;    /**< array of pointers to free chunks */
	uint   freeElems;   /**< number of elements in freeList[] */
	uint   nFree;       /**< number of free chunks */
};


/// Variables:

/** The heap proper. */
static struct heap Heap;


/// Functions:
static int roundUp(int num, int multiple); //MINE
static addr heapMaxAddr (void);
static void updateFreeList (); //MINE

static void combineChunks(header *a, header *b); //MINE


/** Initialise the Heap. */
int initHeap (int size)
{
	Heap.nFree = 0;
	Heap.freeElems = 0;
    //if size is less than 4096 (minimum heap size), set size to 4096
    if (size < MIN_HEAP) {
        size = MIN_HEAP;
    } 
    
    size = roundUp(size, 4); //round up to multiple of 4
    Heap.heapSize = size; //set heapSize to size
    Heap.heapMem = calloc(Heap.heapSize, Heap.heapSize); //allocate memory, set all to 0
    if (Heap.heapMem == NULL) { //if unable to allocate memory, returns -1
        return -1;
    }
    size = roundUp(size, MIN_CHUNK); //round up to multiple of 32
    Heap.freeList = malloc((size/MIN_CHUNK) * sizeof(addr)); //allocate memory
    
    if (Heap.freeList == NULL) { //if unable to allocate memory, returns -1
        return -1;
    }
    
    header *new = Heap.heapMem; //make a new header
    new->status = FREE; 
    new->size = Heap.heapSize; 
    
    Heap.freeList[0] = new;
    Heap.freeElems = size/MIN_CHUNK; //number of elements in freeList
    Heap.nFree = 1; //one free chunk currently
    updateFreeList();
    int i = 1;
    while (i < Heap.freeElems) { //sets all the rest of the remaining list to NULL
        Heap.freeList[i] = NULL;
        i++;
    }

	return 0; // this just keeps the compiler quiet
}

/** Release resources associated with the heap. */
void freeHeap (void)
{
	free (Heap.heapMem);
	free (Heap.freeList);
}

/** Allocate a chunk of memory large enough to store `size' bytes. */
void *myMalloc (int size)
{
    if (size < 1) {
        return NULL;
    }
    
    size = roundUp(size, 4);
  
    
    addr smallest = (addr)NULL; //address of the smallest chunk
    uint smallestSize = Heap.heapSize + 1; //make it larger than biggest possible
    uint minSize = size + sizeof(header); //smallest eligible size N + HeaderSize
    addr current = (addr)Heap.heapMem; //currently checking pointer
    while (current < heapMaxAddr()) { //while inside the heap
        header *currHead = (header *)current; //header pointer to get info of chunk
        
        if (currHead->size >= minSize && currHead->status == FREE) { //if size is greater than or equal to eligible size
            if (currHead->size < smallestSize) { //if size is less than stored smallest size, update to new size
                smallestSize = currHead->size;
                smallest = (addr)current;
            }
        }
        current += currHead->size; //increment
    }
    
    if (smallest == (addr)NULL) { //if no valid chunk is found
        return NULL;
    } 
    header *smallestHead = (header *)smallest;
    void *returnAddress = (byte *)smallest + sizeof(header);
    //if free chunk is smaller than N + sizeof(header) + MIN_CHUNK
    if (smallestSize <= minSize + MIN_CHUNK) {
        //allocate the whole chunk
        smallestHead->status = ALLOC;
        Heap.nFree--;
    } else {
        //split into two chunks, lower chunk allocated for request
        //upper chunk being a new free chunk
        uint originalSize = smallestHead->size;
        smallestHead->status = ALLOC;
        smallestHead->size = minSize;
        
        
        smallest += minSize; //moves it to the start of new head
        
        header *newHead = (header *)smallest;
        newHead->status = FREE; //set status of upper chunk
        newHead->size = originalSize - minSize; //leftovers

    }
    
    updateFreeList();
    
    return returnAddress;
    
       
}

/** Deallocate a chunk of memory. */
void myFree (void *obj)
{
	header *toFree = obj - sizeof(header);
	header **allocHeads = malloc((roundUp(Heap.heapSize, MIN_CHUNK) / MIN_CHUNK) * sizeof(addr)); //make array of allocated heads
    int j = 0;
    addr update = (addr)Heap.heapMem;
    while (update < heapMaxAddr()) { //while inside the heap
        header *currHead = (header *)update; //header pointer to get info of chunk
        if (currHead->status == ALLOC) {
            allocHeads[j] = currHead; //next allocated head
            j++;
        }
        update += currHead->size;//next head
    }    
    //check if passed in is an allocated chunk
    int k = 0;
    bool found = false;
    while (k < j) {
        if (allocHeads[k] == toFree) {
            found = true;
          
            
        }
        k++;  
    }
    //not an allocated chunk
	if (found == false) {
	    fprintf(stderr, "Attempt to free unallocated chunk\n");
	    exit(1);
    } 
    
    toFree->status = FREE; //change status to free
    Heap.nFree++;
    
    updateFreeList();
    
    
    
    int i = 0;
    while (i < Heap.nFree-1) {
        if (Heap.freeList[i] + ((header *)Heap.freeList[i])->size == Heap.freeList[i+1]) { //if two adjacent free chunks 
        //combine the chunks
            combineChunks(Heap.freeList[i], Heap.freeList[i+1]);
            updateFreeList();
            i = 0; //scan from beginning again
        } else {
        i++;
        }
    }
    
   
   
    
    
}

/** Return the first address beyond the range of the heap. */
static addr heapMaxAddr (void)
{
	return (addr) Heap.heapMem + Heap.heapSize;
}

/** Convert a pointer to an offset in the heap. */
int heapOffset (void *obj)
{
	addr objAddr = (addr) obj;
	addr heapMin = (addr) Heap.heapMem;
	addr heapMax =        heapMaxAddr ();
	if (obj == NULL || !(heapMin <= objAddr && objAddr < heapMax))
		return -1;
	else
		return (int) (objAddr - heapMin);
}

/** Dump the contents of the heap (for testing/debugging). */
void dumpHeap (void)
{
	int onRow = 0;

	// We iterate over the heap, chunk by chunk; we assume that the
	// first chunk is at the first location in the heap, and move along
	// by the size the chunk claims to be.
	addr curr = (addr) Heap.heapMem;
	while (curr < heapMaxAddr ()) {
		header *chunk = (header *) curr;

		char stat;
		switch (chunk->status) {
		case FREE:  stat = 'F'; break;
		case ALLOC: stat = 'A'; break;
		default:
			fprintf (
				stderr,
				"myHeap: corrupted heap: chunk status %08x\n",
				chunk->status
			);
			exit (1);
		}

		printf (
			"+%05d (%c,%5d)%c",
			heapOffset ((void *) curr),
			stat, chunk->size,
			(++onRow % 5 == 0) ? '\n' : ' '
		);

		curr += chunk->size;
	}

	if (onRow % 5 > 0)
		printf ("\n");
}

static int roundUp(int num, int multiple) {
    
    int returnValue;
    if (num % multiple == 0) {
        returnValue = num;
    } else {
        returnValue = ((num / 4) + 1) * 4;
    }
    return returnValue;
   
}

static void updateFreeList () {
    int i = 0; //reset freeList array to NULL
    while (i < Heap.freeElems) { //sets all the rest of the remaining list to NULL
        Heap.freeList[i] = NULL;
        i++;
    }
    
    int j = 0;
    addr update = (addr)Heap.heapMem; //currently checking pointer
    while (update < heapMaxAddr()) { //while inside the heap
        header *currHead = (header *)update; //header pointer to get info of chunk
        if (currHead->status == FREE) {
            Heap.freeList[j] = (void *)currHead; //next free head
            j++;
        }
        update += currHead->size;//next head
    }    
    Heap.nFree = j;
    
}

static void combineChunks(header *a, header *b) {
   
    a->size = a->size + b->size; //size of header a = header a + b 
    memset(b, 0, b->size);
    


}
