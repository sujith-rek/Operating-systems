#include <stdio.h>
#include "pgm.h"
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


// (SPECIFICATIONS FOR aFUNCTIONS ARE IN pgm.h !!)

// Physical memory is 2^24 (0x1000000 OR 16777216) bytes, with 2^12 (4096) physical pages.

static const int physical_page_count = 4096;  // Number of physical pages.
static const int physical_page_size = 4096;   // Size of each page.
static char physmem[4096*4096];  // Storage for physical memory.

// Mutex for locking and unlocking for thread safety
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 

// Virtual addresses are 22 bits, 10 bits for page number, 12 bits for offset in page.
// The macros PAGE(n) and OFFSET(n) extract the page number and the offset within the
// page from a virtual address.

#define PHYS_ADDR(phys_page, offset) (((phys_page) << 12) + (offset))
#define OFFSET(n) ((n) & 0xFFF)
#define PAGE(n) ( ((n) >> 12) & 0x3FF )

// Page table maps virtual page numbers to physical page addresses in physmem.
struct page_table {
   int page_table[1024];  // Array of 1024 page table entries.
};

// Initialize physical memory to all zeros.
void pgm_init() {
   memset(physmem, 0, sizeof(physmem));
}

// Function to count number of free pages in physical memory
// As we are using first byte of each page to indicate whether page is free or not
// We can count number of free pages by counting number of pages whose first byte is 0
int free_page_count(){
   int count = 0;
   for(int i=0;i<physical_page_count;i++){
      if(physmem[i*physical_page_size] == 0){
         count++;
      }
   }
   return count;
}

// Function to find which physical page is free
// Returns first page number which is free
int find_free_page(){
   for(int i=0;i<physical_page_count;i++){
      // If first byte of page is 0, then page is free
      // Return page number which is found to be free first
      if(physmem[i*physical_page_size] == 0){
         return i;
      }
   }
   return -1;
}

// Creates and returns a new empty page table
struct page_table *pgm_create() {
   struct page_table *pgtab = (struct page_table *) malloc(sizeof(struct page_table));
   if (pgtab == NULL) {
      return NULL;
   }
   // Initialize all entries in the page table to zero.
   memset(pgtab->page_table, 0, sizeof(pgtab->page_table));
   return pgtab;
}

// Discards a page table and frees all associated resources.
void pgm_discard(struct page_table *pgtab) {
   if (pgtab == NULL) {
      return;
   }

   // Free all physical pages associated with this page table.
   // Set the first byte of each physical page to zero to indicate that it is free.
   for(int i=0;i<1024;i++){
      int mapped_addr = pgtab->page_table[i];
      int phy_page_num = PAGE(mapped_addr);
      physmem[phy_page_num*physical_page_size] = 0;
   }

   // Free the page table itself.
   free(pgtab);
}

/* Writing into physical memory is done in 4 steps:
 * 1. Find a free physical page
 * 2. Map the virtual page to the physical page address in page table
 * 3. Write data into physical page
 * 4. Return number of bytes written
 * 
 * This should be done atomically, so that no other thread can access the page table
 * while we are doing this i.e. only one write should be done one time, so we use
 * mutex lock and unlock to protect the writing process
 * 
 * physmem is an char array and char takes up 1 byte so we copy data byte by byte
*/
int pgm_put(struct page_table *pgtab, int start_address, void *data_source, int byte_count) {

   // Lock mutex to protect writing process
	pthread_mutex_lock(&mutex);
   int v_page_num = PAGE(start_address);  // Virtual page number of start address
   int offset = OFFSET(start_address);    // Offset of start address
   int byte_counter = 0;                  // Number of bytes written, will be returned
   char *source = (char *) data_source;   // Source data to be written into physical memory
   int free_phy_page = find_free_page();  // First free physical page into which data will be written
   int total_free_pages = free_page_count();// Total number of free pages in physical memory
  
   if (pgtab == NULL || data_source == NULL || byte_count < 0 || free_phy_page == -1) {
      return 0;
   }

   // Map virtual page to physical page address in page table
   // This page is where we write data into
   pgtab->page_table[v_page_num] = PHYS_ADDR(free_phy_page, 0);

   // Calculate number of bytes to be written
   // If requested bytes are more than free space in physical memory, write only free space
   // Ignore the rest of the bytes
   if ((byte_count > physical_page_size*total_free_pages)) {
      byte_count = physical_page_size*total_free_pages;
   }

   /* Write data into physical memory byte by byte
    * The steps are: 
    * 1. Calculate physical address of the byte to be written
    *    Physical address = (physical page number * physical page size) + offset
    *    This is calculated by PHYS_ADDR macro
    * 
    *    Physical page number is the free physical page we found earlier
    * 
    * 2. Write data into physical address
    * 3. Increment byte_counter indicating that one byte has been written
   */
   for(int i=0;i<byte_count;i++){
      int phys_addr = PHYS_ADDR(free_phy_page, offset + i);
   	physmem[phys_addr] = source[i];     
      byte_counter++;
   }

   pthread_mutex_unlock(&mutex);     // Unlock mutex after writing is done
   return byte_counter;
}


int pgm_get(struct page_table *pgtab, int start_address, void *data_destination, int byte_count) {
   // pthread_mutex_lock(&mutex);

   int byte_counter = 0;                  // Number of bytes read, will be returned
   int v_page_num = PAGE(start_address);  // Virtual page number of start address
   int offset = OFFSET(start_address);    // Offset of start address
   char *dest = (char *) data_destination;// Destination where data will be written into

   if (pgtab == NULL || data_destination == NULL || byte_count < 0) {
      return 0;
   }

   // We fetch the physical page address from page table
   // Using the virtual page number of start address
   // Then we calculate the physical page number from the physical page address
   int mapped_addr = pgtab->page_table[v_page_num];
   int phy_page_num = PAGE(mapped_addr);

   /* We started Writing from PHYS_ADDR(phy_page_num, offset)
    *
    * So we start reading from the same address
    * We read byte by byte and increment byte_counter
    * We read byte by byte because char takes up 1 byte
    * 
    * The steps are: 
    * 1. Calculate physical address of the byte to be read
    *    Physical address = (physical page number * physical page size) + offset
    *    This is calculated by PHYS_ADDR macro
    * 
    *    Physical page number is the free physical page we found earlier
    * 
    * 2. Read data from physical address into destination
    * 3. Increment byte_counter indicating that one byte has been read
   */
   for(int i=0;i<byte_count;i++){
      int phys_addr = PHYS_ADDR(phy_page_num, offset+i);
      dest[i] = physmem[phys_addr];
      byte_counter++;
   }

   // pthread_mutex_unlock(&mutex);
   return byte_counter;
}

/* A convenience routine for copying an int value into paged memory, at the specified
 * virtual memory address.  The return value is the number of bytes copied; a value
 * less than 4 indicates an error.
 */
int pgm_put_int(struct page_table *pgtab, int address, int value) {
   return pgm_put(pgtab, address, &value, sizeof(int));
}


/* A convenience routine for fetching an int value from paged memory, at the specified
 * virtual memory address.  The return value is the number of bytes copied; a value
 * less than 4 indicates an error.
 */
int pgm_get_int(struct page_table *pgtab, int address, int *pvalue) {
   return pgm_get(pgtab, address, pvalue, sizeof(int));
}



