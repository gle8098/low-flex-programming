#include "irq.h"
#include "apic.h"
#include "ps2_kbd_defs.h"
#include "io.h"
#include "keyboard.h"
#include "event_bus.h"

int readcode() {
    static unsigned int shift;
    static unsigned char *charcode[4] = {
            normalmap, shiftmap, ctlmap, ctlmap
    };
    unsigned st, data, c;

    st = inb(KBSTATP);
    if ((st & KBS_DIB) == 0)
        return -1;
    data = inb(KBDATAP);

    if (data == 0xE0) {
        shift |= E0ESC;
        return 0;
    } else if (data & 0x80) {
        // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(shiftcode[data] | E0ESC);
        return 0;
    } else if (shift & E0ESC) {
        // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= shiftcode[data];
    shift ^= togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];
    if (shift & CAPSLOCK) {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    return c;
}

void keyboard_irq(struct regs *regs) {
    (void) regs;
    int keycode = readcode();
    apic_eoi();

    struct keyboard_event event = {
            .header.event_type = EVENT_KEY_PRESS,
            .header.length = sizeof(struct keyboard_event),
            .keycode = keycode
    };
    fire_event((event_t *) &event);
}

void turn_on_capslock_light() {
    outb(KBSTATP, 0xED);
    outb(KBDATAP, 2);

    for (;;) {
        uint8_t resp = inb(KBSTATP);
        if (resp == 0xFA /*ACK*/ || resp == 0xFE /*Resend*/) {
            return;
        }
    }
}
