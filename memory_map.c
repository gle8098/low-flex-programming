#include "memory_map.h"
#include "vga.h"
#include "panic.h"
#include "defs.h"
#include "paging.h"

#define MBI_FLAG_MEM (1 << 0)
#define MBI_FLAG_MMAP (1 << 6)


void print_mmap(multiboot_info_t* mbd) {
    mbd = phys2virt(mbd);

    printk("Memory map from multiboot info\n");
    if ((mbd->flags & MBI_FLAG_MEM) > 0) {
        printk("mem_lower = %x, mem_upper = %x\n", mbd->mem_lower, mbd->mem_upper);
    }

    if ((mbd->flags & MBI_FLAG_MMAP) > 0) {
        multiboot_uint32_t len = mbd->mmap_length;
        char* mmap_addr = phys2virt((void *) mbd->mmap_addr);

        multiboot_memory_map_t* entry = (multiboot_memory_map_t *) mmap_addr;
        for (; entry < (multiboot_memory_map_t*) (mmap_addr + len); ++entry) {
            printk("[MME] base_addr = %x %x, len = %x %x, type = %x\n",
                   entry->addr_high, entry->addr_low, entry->len_high, entry->len_low, entry->type);
        }
    }
}
