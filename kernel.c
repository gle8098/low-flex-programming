#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "multiboot.h"

#include "idt.h"
#include "gdt.h"
#include "acpi.h"
#include "apic.h"
#include "irq.h"
#include "vga.h"
#include "panic.h"
#include "paging.h"
#include "sched.h"
#include "pci.h"
#include "ata.h"
#include "memory_map.h"

void kernel_main(multiboot_info_t* mbd_lmem_ptr) {
    init_gdt();
    init_idt();

    init_kalloc_early();
    init_kernel_paging();

    terminal_initialize();
    print_mmap(mbd_lmem_ptr);

    struct acpi_sdt* rsdt = acpi_find_rsdt();
    if (!rsdt) {
        panic("RSDT not found!");
    } else {
        printk("RSDT found at %x\n", (uint32_t) rsdt);
        acpi_validate_rsdt(rsdt);
    }

    apic_init(rsdt);
    // TODO: ata_init();

    printk("Hell OS is loaded\n");

    scheduler_start();
}
