#include "memory_map.h"
#include "vga.h"
#include "panic.h"
#include "defs.h"

#define ADD_KERNEL_HIGH(ptr) (ptr)
//((uint32_t) ptr + KERNEL_HIGH)

void print_mmap(multiboot_info_t* mbd) {
    mbd = (multiboot_info_t*) ADD_KERNEL_HIGH(mbd);

    printk("Memory map from multiboot info\n");
    if ((mbd->flags & (1 << 1)) > 0) {
        printk("mem_lower = %x, mem_upper = %x\n", mbd->mem_lower, mbd->mem_upper);
    }

    if ((mbd->flags & (1 << 6)) > 0) {
        multiboot_uint32_t len = mbd->mmap_length;
        char* mmap_addr = (multiboot_memory_map_t*) ADD_KERNEL_HIGH(mbd->mmap_addr);

        multiboot_memory_map_t* entry = mmap_addr;
        for (; entry < (multiboot_memory_map_t*) (mmap_addr + len); ++entry) {
            printk("[MME] base_addr = %x %x, len = %x %x, type = %x\n",
                   entry->addr_high, entry->addr_low, entry->len_high, entry->len_low, entry->type);
        }
    }
}
