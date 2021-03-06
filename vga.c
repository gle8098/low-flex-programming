#include "vga.h"
#include "paging.h"
#include "keyboard.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

#define VGA_INDEX(x, y) ((y) * VGA_WIDTH + (x))
#define VGA_PTR(x, y) (terminal_buffer + VGA_INDEX(x, y))

static size_t terminal_row;
static size_t terminal_column;
static uint16_t* terminal_buffer;

void handle_keypress(keyboard_event_t* event, void* _) {
    char str[] = {event->keycode, '\0'};
    terminal_writestring(str);
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    uint8_t color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = phys2virt((uint16_t*) 0xB8000);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', color);
        }
    }

    register_event_callback(EVENT_KEY_PRESS, (callback_t) handle_keypress, NULL);
}

void terminal_shift_buffer() {
    for (size_t y = 1; y <= VGA_HEIGHT; ++y) {
        memcpy(VGA_PTR(0, y - 1), VGA_PTR(0, y), VGA_WIDTH * sizeof(*terminal_buffer));
    }
    --terminal_row;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = VGA_INDEX(x, y);
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar_color(char c, uint8_t color) {
    switch (c) {
    case '\n':
        terminal_row++;
        terminal_column = 0;
        if (terminal_row == VGA_HEIGHT) {
            terminal_shift_buffer();
        }
        break;

    case '\r':
        terminal_column = 0;
        break;

    default:
        terminal_putentryat(c, color, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            terminal_row++;
            if (terminal_row == VGA_HEIGHT) {
                terminal_shift_buffer();
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
