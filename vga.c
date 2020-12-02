#include "vga.h"
#include "paging.h"
#include "keyboard.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static const size_t BUFFER_HEIGHT = 200;

#define VGA_INDEX(x, y) ((y) * VGA_WIDTH + (x))

static size_t terminal_row;
static size_t terminal_column;
static uint16_t terminal_buffer[200 * 80];

static uint16_t* terminal_out;
static size_t terminal_scroll;
static int terminal_autoscroll;

void handle_keypress(keyboard_event_t* event, void* _) {
    char str[] = {event->keycode, '\0'};
    terminal_writestring(str);
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_scroll = 0;
    memset(terminal_buffer, '\0', sizeof(terminal_buffer));

    uint8_t color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_out = phys2virt((uint16_t*) 0xB8000);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_out[index] = vga_entry(' ', color);
        }
    }

    register_event_callback(EVENT_KEY_PRESS, (callback_t) handle_keypress, NULL);
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = VGA_INDEX(x, y);
    const uint16_t entry = vga_entry(c, color);
    terminal_buffer[index] = entry;

    if (terminal_scroll <= y && y <= terminal_scroll + VGA_HEIGHT) {
        terminal_out[VGA_INDEX(x, y - terminal_scroll)] = entry;
    }
}

void terminal_shift_buffer() {
    for (size_t y = )
}

void terminal_newline_buffer() {
    if (terminal_row == BUFFER_HEIGHT) {
        terminal_shift_buffer();
    }
}

void terminal_putchar_color(char c, uint8_t color) {
    switch (c) {
    case '\n':
        terminal_row++;
        terminal_column = 0;
        if (terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
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
                terminal_row = 0;
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
