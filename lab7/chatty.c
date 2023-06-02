#include "pgm.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

// (SPECIFICATIONS FOR FUNCTIONS ARE IN pgm.h !!)

// Physical memory is 2^24 (0x1000000 OR 16777216) bytes, with 2^12 (4096) physical pages.

static const int physical_page_count = 4096;    // Number of physical pages.
static const int physical_page_size = 4096;     // Size of each page.

static char physmem[4096 * 4096];   // Storage for physical memory.
static bool physmem_used[4096];

// Mutex for thread-safe access to the physmem_used array
static pthread_mutex_t physmem_mutex = PTHREAD_MUTEX_INITIALIZER;

// Virtual addresses are 22 bits, 10 bits for page number, 12 bits for offset in page.
// The macros PAGE(n) and OFFSET(n) extract the page number and the offset within the
// page from a virtual address.

#define PHYS_ADDR(phys_page, offset) (((phys_page) << 12) + (offset))
#define OFFSET(n) ((n) & 0xFFF)
#define PAGE(n) (((n) >> 12) & 0x3FF)

// Page table structure
struct page_table {
    int page_mapping[1024];
};

// Initialize the memory management system
void pgm_init() {
    memset(physmem_used, 0, sizeof(physmem_used));
}

// Create a new page table
struct page_table *pgm_create() {
    struct page_table *pgtab = (struct page_table *)malloc(sizeof(struct page_table));
    for (int i = 0; i < 1024; i++) {
        pgtab->page_mapping[i] = -1;
    }
    return pgtab;
}

// Discard a page table and release the allocated physical pages
void pgm_discard(struct page_table *pgtab) {
    for (int i = 0; i < 1024; i++) {
        if (pgtab->page_mapping[i] != -1) {
            physmem_used[pgtab->page_mapping[i]] = false;
        }
    }
    free(pgtab);
}

// Allocate a physical page in a thread-safe manner
int allocate_physical_page() {
    pthread_mutex_lock(&physmem_mutex);
    int allocated_page = -1;
    for (int i = 0; i < physical_page_count; i++) {
        if (!physmem_used[i]) {
            physmem_used[i] = true;
            allocated_page = i;
            break;
        }
    }
    pthread_mutex_unlock(&physmem_mutex);
    return allocated_page;
}

// Write data to the virtual memory
int pgm_put(struct page_table *pgtab, int start_address, void *data_source, int byte_count) {
    if (!pgtab || !data_source || byte_count <= 0) {
        return 0;
    }

    int bytes_copied = 0;
    int current_address = start_address;

    while (bytes_copied < byte_count) {
        int virtual_page = PAGE(current_address);
        int offset = OFFSET(current_address);
        int remaining_bytes_in_page = physical_page_size - offset;
        int bytes_to_copy = remaining_bytes_in_page < (byte_count - bytes_copied) ? remaining_bytes_in_page : (byte_count - bytes_copied);

        if (pgtab->page_mapping[virtual_page] == -1) {
            int physical_page = allocate_physical_page();
            if (physical_page == -1) {
                break;
            }
            pgtab->page_mapping[virtual_page] = physical_page;
        }

        memcpy(physmem + PHYS_ADDR(pgtab->page_mapping[virtual_page], offset), (char *)data_source + bytes_copied, bytes_to_copy);

        bytes_copied += bytes_to_copy;
        current_address += bytes_to_copy;
    }

    return bytes_copied;
}

// Read data from the virtual memory
int pgm_get(struct page_table *pgtab, int start_address, void *data_destination, int byte_count) {
    if (!pgtab || !data_destination || byte_count <= 0) {
        return 0;
    }

    int bytes_copied = 0;
    int current_address = start_address;

    while (bytes_copied < byte_count) {
        int virtual_page = PAGE(current_address);
        int offset = OFFSET(current_address);
        int remaining_bytes_in_page = physical_page_size - offset;
        int bytes_to_copy = remaining_bytes_in_page < (byte_count - bytes_copied) ? remaining_bytes_in_page : (byte_count - bytes_copied);

        if (pgtab->page_mapping[virtual_page] == -1) {
            memset((char *)data_destination + bytes_copied, 0, bytes_to_copy);
        } else {
            memcpy((char *)data_destination + bytes_copied, physmem + PHYS_ADDR(pgtab->page_mapping[virtual_page], offset), bytes_to_copy);
        }

        bytes_copied += bytes_to_copy;
        current_address += bytes_to_copy;
    }

    return bytes_copied;
}

// Write an integer value to the virtual memory
int pgm_put_int(struct page_table *pgtab, int address, int value) {
    return pgm_put(pgtab, address, &value, sizeof(int));
}

// Read an integer value from the virtual memory
int pgm_get_int(struct page_table *pgtab, int address, int *pvalue) {
    return pgm_get(pgtab, address, pvalue, sizeof(int));
}