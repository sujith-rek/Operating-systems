/* Runs some additional single-thread tests on pgm.  
 * Pages from different page tables are interleaved.
 * Compile with:   gcc -o test1 test1.c pgm.c -pthread
 */

#include "pgm.h"
#include <stdlib.h>
#include <stdio.h>

struct testinfo {
   struct page_table *pgtab;
   int val;   // starting value stored into virtual address
   int ct;    // number of items stored
   int vmem;  // starting virtual address
};

struct testinfo info[8];

int big[2 << 18];  // as big as the virtual memory address space

void store(struct testinfo *info) {
   int val = info->val;
   int ct = info->ct;
   int addr = info->vmem;
   struct page_table *tbl = info->pgtab;
   for (int i = 0; i < ct; i++) {
       int x = pgm_put_int(tbl, addr+(4*i), val+i);
       if ( x != 4 ) {
          printf("Error writing int number %d: %d.\n", i, x);
          return;
       }
   }
   printf("Store of %d values succeeded.\n", ct);
}

void read_back(struct testinfo *info) {
   int val = info->val;
   int ct = info->ct;
   int addr = info->vmem;
   struct page_table *tbl = info->pgtab;
   int num;
   for (int i = 0; i < ct; i++) {
       int x = pgm_get_int(tbl, addr+(4*i), &num);
       if ( x != 4 ) {
          printf("Error getting int number %d: %d.", i, x);
          return;
       }
       if (num != val+i) {
          printf("Got wrong number; got %d expected %d on read number %d.\n", num, val+i, i);
          return;
       }
   }
   printf("Read of %d values succeeded.\n", ct);
}

int main(int argc, char *argv[]) {
   int seed = 1;
   if (argc > 1) {
      int s = atoi(argv[1]);
      if (s)
         seed = s;
   }
   srandom(seed);
   
   pgm_init();
//   printf("%d free pages\n", free_page_count());

   for (int i = 0; i < 8; i++) {
      info[i].vmem = random() % 0x3FFFFF;
      info[i].val = random();
      info[i].ct = 10000 + random()%100000;
      info[i].pgtab = pgm_create();
   }
   
   printf("\nStoring data using 8 page tables...\n");
   for (int i = 0; i < 8; i++) {
      printf("   %d. ",i);
      store(&info[i]);
   }
   
   printf("\nReading back the data...\n");
   for (int i = 0; i < 8; i++) {
      printf("   %d. ",i);
      read_back(&info[i]);
   }
   
   printf("\nWriting interleaved data using 8 page tables...\n");
   for (int i = 0; i < 100000; i++) {
      big[i] = i;
   }
   int ok = 1;
   for (int i = 0; i < 10 && ok; i++) {
      int *source = &big[1000*i];
      int destination = 10000*i * 4;
      for (int j = 0; j < 8; j++) {
         int x = pgm_put(info[j].pgtab, destination, source, 40000);
         if (x != 40000) {
            printf("Error writing block %d for page table %d\n", j, i);
            ok = 0;
            break;
         }
      }
   }
   if (ok) {
      printf("   Data written successfully.  Reading back data...\n");
      for (int i = 0; i < 8 && ok; i++) {
         for (int j = 0; j < 100000; j++)
             big[j] = -1;
         int x = pgm_get(info[i].pgtab, 0, big, 40000);
         if (x != 40000) {
             printf("Error reading data for page table %d.\n", i);
             ok = 0;
             break;
         }
         for (int j = 0; j < 10000; j++)
            if (big[j] != j) {
                printf("   Incorrect value (%d expecting %d) read for page table %d.\n", big[j], j, i);
                ok = 0;
                break;
            }
      }
      if (ok) {
          printf("   Successful.\n");
      }
   }
//   printf("%d free pages.\n",free_page_count());
   
   printf("\nDiscarding half of the page tables...\n");
   for (int i = 4; i < 8; i++)
      pgm_discard(info[i].pgtab);
//   printf("%d free pages.\n",free_page_count());

   printf("\nCreating another page table, then writing and reading lots of data...\n");
   struct page_table *tab = pgm_create();
   int size = 2 << 18; // number of ints in big
   for (int i = 0; i < size; i++) {
      big[i] = i;
   }
   int x = pgm_put(tab, 0, big, size*4);
   if (x != size*4) {
      printf("   Error writing data.\n");
   }
   else {
      for (int j = 0; j < size; j++)
         big[j] = -1;
      x = pgm_get(tab, 0, big, size*4);
      if (x != size*4) {
          printf("    Error reading data.\n");
      }
      else {
          ok = 1;
          for (int j = 0; j < size; j++)
              if (big[j] != j) {
                  printf("   Incorrect value read back.\n");
                  ok = 0;
                  break;
              }
           if (ok)
               printf("   Successful.\n");
      }
   }
//   printf("%d free pages.\n",free_page_count());

}






