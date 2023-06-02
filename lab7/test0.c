/* Runs some basic single-thread tests on pgm.  
 * Pages from different page tables are not interleaved.
 * Compile with:   gcc -o test0 test0.c pgm.c -pthread
 */

#include "pgm.h"
#include <stdio.h>

struct page_table *tab;
char ch[10000];    // for writing/reading blocks of data.
int big[0x100000]; // size is same as virtual mem address space. 2**20

void try_one(int addr) {
   int n = 17;
   int x = pgm_put_int(tab, addr, n);
   if (x != 4) {
      printf("   Got error (%d) writing 17 to %d.\n", x, addr);
      return;
   }
   x = pgm_get_int(tab, addr, &n);
   // printf(" address of n %d\n",&n);
   if (x != 4) {
      printf("   Got error (%d) reading from %d.\n", x, addr);
      return;
   }
   if (n != 17) {
      printf("   Got wrong value (%d) reading 17 from %d.\n", n, addr);
      return;
   }
   printf("   Successfully wrote and read 17 at address %d.\n", addr);
}

void try_array(int addr) {
   for (int i = 0; i < 10000; i++) {
      ch[i] = (char)i;
   }
   int x = pgm_put(tab, addr, ch, 10000);
   if (x != 10000) {
      printf("   Got error (%d) writing array to %d.\n", x, addr);
      return;
   }
   x = pgm_get(tab, addr, ch, 10000);
   if (x != 10000) {
      printf("   Got error (%d) reading from %d.\n", x, addr);
      return;
   }
   for (int i = 0; i < 10000; i++) {
      if (ch[i] != (char)i) {
         printf("   Got wrong value (%d) reading array element %d from %d.\n", ch[i], i, addr);
         return;
      }
   }
   printf("   Successfully wrote and read array at address %d.\n", addr);
}

void try_all_vmem() {
   for (int i = 0; i < 0x100000; i++) {
       big[i] = i + 12345;
   }
   int c = pgm_put(tab, 0, big, 0x400000);
   if (c != 0x400000) {
      printf("   Error writing data, %d bytes written.\n", c);
      return;
   }
   c = pgm_get(tab, 0, big, 0x400000);
   if (c != 0x400000) {
      printf("   Error reading data, %d bytes read.\n", c);
      return;
   }
   for (int i = 0; i < 0x100000; i++) {
      if (big[i] != i + 12345) {
         printf("    Error, got the wrong value for integer number %d.\n", i);
         return;
      }
   }
   printf("   Successful.\n");
}

void try_all_physmem() {
   for (int i = 0; i < 0x100000; i++) {
       big[i] = i + 12345;
   }
   for (int i = 0; i < 3; i++) {
      // fill 3 more copies of virtual memory, except for 1 page each.
      // which should fill physical memory except for 3 pages
      struct page_table *foo = pgm_create();
      int x = pgm_put(foo, 0, big, 0x400000 - 5000);
      if (x != 0x400000 - 5000) {
           printf("   Error after writing %d bytes.\n", i*(0x400000-5000) + x);
           return;
      }
   }
//   printf("Free pages %d\n", free_page_count());  // should be 3
   printf("   Wrote enough data to almost fill physical memory.\n");
   printf("   Attempting to write more than should fit in remaining memory...\n");
   struct page_table *bar = pgm_create();
   int x = pgm_put(bar, 0, big, 20000);  // should'nt fit, but 3*4096 should
   if (x < 3*4096) {
       printf("   Memory filled too soon.\n");
       return;
   }
   if (x > 3*4096) {
       printf("   Didn't get expected error when writing too much data.\n");
       return;
   }
   printf("   Wrote expected amount of data before failure.\n");
   printf("   Successful.\n");
}

int main() {
   pgm_init();
//   printf("Free pages %d\n", free_page_count()); // free_page_count() is not a required function
   tab = pgm_create();
   
   printf("\nTry writing and reading 17s to various addresses...\n");
   // total space is 2**20 = 1048576 bytes
   try_one(42);
   try_one(384888);
   try_one(23894788);

   printf("\nTry writing and reading array of 10000 chars to various addresses...\n");
   try_array(42);
   try_array(384888);
   try_array(23894788);
   printf("\nTry to read and write all of virtual memory...\n");
   try_all_vmem();
//   printf("Free pages %d\n", free_page_count());
   
   // pgm_discard(tab);
   printf("\nTry filling physical memory, then try to write more...\n");
   try_all_physmem();
   
}

