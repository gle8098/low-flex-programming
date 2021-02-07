#include "acpi.h"
#include "common.h"
#include "paging.h"
#include "panic.h"

static struct acpi_rsdp* find_rsdp_in_region(void* start, size_t len) {
    for (size_t i = 0; i < len - 8; i++) {
        void* addr = (uint8_t*)start + i;
        if (memcmp("RSD PTR ", (const char*)addr, 8) == 0) {
            return (struct acpi_rsdp*)addr;
        }
    }
    return NULL;
}

struct acpi_sdt* acpi_find_rsdt() {
    // 1KB of EBDA.
    void* ebda_addr = phys2virt((void*)((*(uint16_t*)phys2virt((void*)0x40e)) << 4));
    struct acpi_rsdp* rsdp = find_rsdp_in_region(ebda_addr, 1024);
    if (!rsdp) {
        // Static memory region.
        rsdp = find_rsdp_in_region(phys2virt((void*)0xe0000), 0xfffff - 0xe0000);
    }

    if (!rsdp) {
        return NULL;
    }

    void* rsdt_addr = (void*)rsdp->rsdt_addr;
    identity_map(rsdt_addr, PAGE_SIZE);
    return (struct acpi_sdt*)rsdt_addr;
}

struct acpi_sdt* acpi_find_sdt(struct acpi_sdt* root, const char* signature) {
    size_t sz = (root->header.length - sizeof(root->header)) / 4;
    for (size_t i = 0; i < sz; i++) {
        if (memcmp(signature, &root->entries[i]->header.signature, 4) == 0) {
            identity_map(root->entries[i], 2 * PAGE_SIZE);
            return root->entries[i];
        }
    }
    return NULL;
}

int acpi_validate_sdt(struct acpi_sdt* entry) {
    unsigned char sum = 0;

    struct acpi_sdt_header* header = &entry->header;
    for (uint32_t i = 0; i < header->length; i++) {
        sum += ((char *) header)[i];
    }

    return sum == 0;
}

void acpi_validate_rsdt(struct acpi_sdt* root) {
    uint32_t size = (root->header.length - sizeof(struct acpi_sdt_header)) / sizeof(struct acpi_sdt*);
    for (uint32_t i = 0; i < size; ++i) {
        if (!acpi_validate_sdt(root->entries[i])) {
            panic("SDT table #%d corrupted", i);
        }
    }
}
