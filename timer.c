#include "irq.h"
#include "vga.h"
#include "apic.h"
#include "timer.h"
#include "sched.h"
#include "ata.h"

volatile uint64_t timer_ticks;

void timer_irq(struct regs* regs) {
    (void)regs;
    __sync_fetch_and_add(&timer_ticks, 1);
    apic_eoi();

    ata_tick();
    scheduler_tick(regs);
}
