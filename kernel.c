#include <stdbool.h>

#include "interrupts.h"
#include "kernel.h"
#include "printf.h"

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len]) {
		len++;
    }
	return len;
}

size_t terminal_row;
size_t terminal_column;
uint16_t* terminal_buffer;

void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	uint8_t color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', color);
		}
	}
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_on_rows_overflow() {
    uint16_t* buf_index = terminal_buffer + /* line no */ 1 * VGA_WIDTH;
    for (int line = 1; line < VGA_HEIGHT; ++line) {
        for (int col = 0; col < VGA_WIDTH; ++col, ++buf_index) {
            *(buf_index - VGA_WIDTH) = *buf_index;
        }
    }
    buf_index -= VGA_WIDTH;
    for (int i = 0; i < VGA_WIDTH; ++i, ++buf_index) {
        *buf_index = 0;
    }
    --terminal_row;
}

void terminal_putchar_color(char c, uint8_t color) {
    switch (c) {
    case '\n':
        terminal_row++;
        terminal_column = 0;
        if (terminal_row == VGA_HEIGHT) {
            terminal_on_rows_overflow();
        }
        break;

    default:
        terminal_putentryat(c, color, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
            if (terminal_row == VGA_HEIGHT) {
                terminal_on_rows_overflow();
            }
        }
    }
}

void terminal_write(const char* data, size_t size, uint8_t color) {
	for (size_t i = 0; i < size; i++) {
        terminal_putchar_color(data[i], color);
    }
}

void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data), vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}

void terminal_writestring_color(const char* data, uint8_t color) {
    terminal_write(data, strlen(data), color);
}


__attribute__ ((interrupt)) void isr0(struct iframe* frame) {
    terminal_writestring_color("Hello world!\n", vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));
    asm volatile ("outb %1, %0": : "dN"(0x20), "a"(0x20));

    (void)frame;
}

void kernel_main(void) {
    init_idt();

	terminal_initialize();
    asm volatile ("sti");
    asm volatile ("int $0x80");
    asm volatile ("cli");

    terminal_writestring_color("(c) carzil\n", vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK));
    printf("Int 42 = %d\n", 42);

    int timer = 0;
    const int delay = 100000000;
    while (true) {
        if (timer % delay == 0) {
            printf("Tick %d\n", timer / delay);
        }

        ++timer;
    }
}
