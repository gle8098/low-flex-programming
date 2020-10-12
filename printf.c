#include "printf.h"
#include <stdarg.h>
#include "kernel.h"

void putchar(char c) {
    const uint8_t VGA_DEFAULT_COLOR = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    terminal_putchar_color(c, VGA_DEFAULT_COLOR);
}

void print_decimal(int dec) {
    if (dec < 0) {
        putchar('-');
    }
    char int_buf[16] = {};  // 16 is enough to store int
    int int_len = 0;
    while (dec != 0) {
        int_buf[int_len] = '0' + (dec % 10);
        ++int_len;
        dec /= 10;
    }
    for (int i = 1; i <= int_len; ++i) {
        putchar(int_buf[int_len - i]);
    }
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == '%') {
            ++fmt;

            switch (*fmt) {
            case 'd': ;
                int dec = va_arg(args, int);
                print_decimal(dec);
                break;
            }
        } else {
            putchar(*fmt);
        }

        ++fmt;
    }

    va_end(args);
}
