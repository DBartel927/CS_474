#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MEM_SIZE 16384  // MUST equal PAGE_SIZE * PAGE_COUNT
#define PAGE_SIZE 256  // MUST equal 2^PAGE_SHIFT
#define PAGE_COUNT 64
#define PAGE_SHIFT 8  // Shift page number this much

#define PTP_OFFSET 64 // How far offset in page 0 is the page table pointer table

#define PAGE_FREE 0
#define PAGE_USED 1
#define NO_PAGE -1

// Simulated RAM
unsigned char mem[MEM_SIZE];

//
// Convert a page,offset into an address
//
int get_address(int page, int offset)
{
    return (page << PAGE_SHIFT) | offset;
}

//
// Initialize RAM
//
void initialize_mem(void)
{
    memset(mem, 0, MEM_SIZE);

    int zpfree_addr = get_address(0, 0);
    mem[zpfree_addr] = 1;  // Mark zero page as allocated
}

int allocate_page(void)
{
    for (int page = 0; page < PAGE_COUNT; page++) {
        int free_map_addr = get_address(0, page);

        if (mem[free_map_addr] == PAGE_FREE) {
            mem[free_map_addr] = PAGE_USED;
            return page;
        }
    }

    return NO_PAGE;
}

//
// Get the page table page for a given process
//
unsigned char get_page_table(int proc_num)
{
    int ptp_addr = get_address(0, PTP_OFFSET + proc_num);
    return mem[ptp_addr];
}

void free_page(int page)
{
    int addr = get_address(0, page);
    mem[addr] = 0;
}

int virtual_to_physical(int proc_num, int vaddr)
{
    int page_table = get_page_table(proc_num);

    int virtual_page = vaddr >> PAGE_SHIFT;
    int offset = vaddr & (PAGE_SIZE - 1);
    int page_table_addr = get_address(page_table, virtual_page);
    int physical_page = mem[page_table_addr];

    return get_address(physical_page, offset);
}

void kill_process(int proc_num)
{
    int page_table = get_page_table(proc_num);

    for (int virtual_page = 0; virtual_page < PAGE_COUNT; virtual_page++) {
        int page_table_addr = get_address(page_table, virtual_page);
        int physical_page = mem[page_table_addr];

        if (physical_page != 0) {
            free_page(physical_page);
        }
    }

    free_page(page_table);

    int ptp_addr = get_address(0, PTP_OFFSET + proc_num);
    mem[ptp_addr] = 0;
}

void store_byte(int proc_num, int vaddr, int val)
{
    int addr = virtual_to_physical(proc_num, vaddr);
    mem[addr] = val;

    printf("Store proc %d: %d => %d, value=%d\n", proc_num, vaddr, addr, val);
}

void load_byte(int proc_num, int vaddr)
{
    int addr = virtual_to_physical(proc_num, vaddr);
    int val = mem[addr];

    printf("Load proc %d: %d => %d, value=%d\n", proc_num, vaddr, addr, val);
}

//
// Allocate pages for a new process
//
// This includes the new process page table and page_count data pages.
//
void new_process(int proc_num, int page_count)
{
    int page_table_page = allocate_page();

    if (page_table_page == NO_PAGE) {
        printf("OOM: proc %d: page table\n", proc_num);
        return;
    }

    int ptp_addr = get_address(0, PTP_OFFSET + proc_num);
    mem[ptp_addr] = page_table_page;

    for (int vir_page = 0; vir_page < page_count; vir_page++) {
        int data_page = allocate_page();

        if (data_page == NO_PAGE) {
            printf("OOM: proc %d: data page\n", proc_num);
            return;
        }

        int page_table_entry_addr = get_address(page_table_page, vir_page);
        mem[page_table_entry_addr] = data_page;
    }
}

//
// Print the free page map
//
// Don't modify this
//
void print_page_free_map(void)
{
    printf("--- PAGE FREE MAP ---\n");

    for (int i = 0; i < 64; i++) {
        int addr = get_address(0, i);

        printf("%c", mem[addr] == 0? '.': '#');

        if ((i + 1) % 16 == 0)
            putchar('\n');
    }
}

//
// Print the address map from virtual pages to physical
//
// Don't modify this
//
void print_page_table(int proc_num)
{
    printf("--- PROCESS %d PAGE TABLE ---\n", proc_num);

    // Get the page table for this process
    int page_table = get_page_table(proc_num);

    // Loop through, printing out used pointers
    for (int i = 0; i < PAGE_COUNT; i++) {
        int addr = get_address(page_table, i);

        int page = mem[addr];

        if (page != 0) {
            printf("%02x -> %02x\n", i, page);
        }
    }
}

//
// Main -- process command line
//
int main(int argc, char *argv[])
{
    assert(PAGE_COUNT * PAGE_SIZE == MEM_SIZE);

    if (argc == 1) {
        fprintf(stderr, "usage: ptsim commands\n");
        return 1;
    }
    
    initialize_mem();

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "pfm") == 0) {
            print_page_free_map();
        }
        else if (strcmp(argv[i], "ppt") == 0) {
            int proc_num = atoi(argv[++i]);
            print_page_table(proc_num);
        }
        else if (strcmp(argv[i], "np") == 0) {
            int proc_num = atoi(argv[++i]);
            int page_count = atoi(argv[++i]);
            new_process(proc_num, page_count);
        } else if (strcmp(argv[i], "kp") == 0) {
            int proc_num = atoi(argv[++i]);
            kill_process(proc_num);
        } else if (strcmp(argv[i], "sb") == 0) {
            int proc_num = atoi(argv[++i]);
            int vaddr = atoi(argv[++i]);
            int val = atoi(argv[++i]);
            store_byte(proc_num, vaddr, val);
        } else if (strcmp(argv[i], "lb") == 0) {
            int proc_num = atoi(argv[++i]);
            int vaddr = atoi(argv[++i]);
            load_byte(proc_num, vaddr);
        } else {
            fprintf(stderr, "%s Unknown command: %s\n", argv[0], argv[i]);
            return 1;
        }

        // TODO: more command line arguments
    }
    return 0;
}
