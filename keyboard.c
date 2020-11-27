#include "irq.h"
#include "vga.h"
#include "apic.h"

void keyboard_irq(struct regs* regs) {
    (void)regs;
    // ...
    apic_eoi();
}
