#ifndef LUNAR_H
#define LUNAR_H

#include "font.h"

multiboot_info_t *mb_info;

// Common

void *memcpy(char *dst, char *src, int n)
{
    char *p = dst;
    while (n--)
        *dst++ = *src++;
    return p;
}

void *memset(void *ptr, int value, lunar_uint16_t num)
{
    lunar_uint8_t *u8Ptr = (lunar_uint8_t *)ptr;

    for (lunar_uint16_t i = 0; i < num; i++)
        u8Ptr[i] = (lunar_uint8_t)value;

    return ptr;
}

static inline void outb(unsigned short port, unsigned char val)
{
    asm volatile("outb %0, %1"
                 :
                 : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));
    return ret;
}

// Graphics

void set_pixel(int x, int y, unsigned int color)
{
    unsigned *backbuffer = (unsigned *)(mb_info->framebuffer_addr + (y + mb_info->framebuffer_height) * mb_info->framebuffer_pitch);
    backbuffer[x] = color;
}

void clear_screen(unsigned int color)
{
    for (unsigned y = 0; y < mb_info->framebuffer_height; ++y)
    {
        for (unsigned x = 0; x < mb_info->framebuffer_width; ++x)
            set_pixel(x, y, color);
    }
}

void set_rect(int x, int y, int width, int height, unsigned int color)
{
    for (int j = y; j < (y + height); j++)
    {
        for (int i = x; i < (x + width); i++)
        {
            set_pixel(i, j, color);
        }
    }
}

void flush()
{
    memcpy(mb_info->framebuffer_addr, mb_info->framebuffer_addr + mb_info->framebuffer_height * mb_info->framebuffer_pitch, mb_info->framebuffer_pitch * mb_info->framebuffer_height);
}

// Terminal

typedef struct
{
    lunar_uint8_t *font;
    int cursor_x;
    int cursor_y;
    unsigned int color;
} terminal;

void init_terminal(terminal *term, int cursor_x, int cursor_y, unsigned int color)
{
    term->font = psf_font;
    term->cursor_x = cursor_x;
    term->cursor_y = cursor_y;
    term->color = color;
}

void scroll(terminal *term)
{
    for (int y = 0; y < mb_info->framebuffer_height - 16; y++)
    {
        memcpy((void *)(mb_info->framebuffer_addr + y * mb_info->framebuffer_pitch),
               (void *)(mb_info->framebuffer_addr + (y + 16) * mb_info->framebuffer_pitch),
               mb_info->framebuffer_width * 4);
    }
    memset((void *)(mb_info->framebuffer_addr + (mb_info->framebuffer_height - 16) * mb_info->framebuffer_pitch),
           0,
           mb_info->framebuffer_width * 4);
}

void set_char(char c, int x, int y, terminal *term)
{
    uint8_t *offset = term->font + sizeof(font_header) + 16 * c;

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (offset[i] & (1 << j))
            {
                set_pixel(x + 8 - j, y + i, term->color);
            }
        }
    }
}

void set_string(char *string, terminal *term)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        if (string[i] == '\n')
        {
            term->cursor_x = 0;
            term->cursor_y += 16;
            if (term->cursor_y >= mb_info->framebuffer_height)
            {
                scroll(term);
                term->cursor_y -= 16;
            }
        }
        else
        {
            set_char(string[i], term->cursor_x, term->cursor_y, term);
            term->cursor_x += 8;
            if (term->cursor_x >= mb_info->framebuffer_width)
            {
                term->cursor_x = 0;
                term->cursor_y += 16;
                if (term->cursor_y >= mb_info->framebuffer_height)
                {
                    scroll(term);
                    term->cursor_y -= 16;
                }
            }
        }
    }
}

// CPU

struct gdt_entry
{
    lunar_uint16_t limit_low;
    lunar_uint16_t base_low;
    lunar_uint8_t base_middle;
    lunar_uint8_t access;
    lunar_uint8_t granularity;
    lunar_uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr
{
    lunar_uint16_t limit;
    lunar_uint32_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gdt_pointer;

void gdt_set_gate(int num, lunar_uint32_t base, lunar_uint32_t limit, lunar_uint8_t access, lunar_uint8_t gran)
{
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;

    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F);

    gdt[num].granularity |= (gran & 0xF0);
    gdt[num].access = access;
}

void init_gdt()
{
    gdt_pointer.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_pointer.base = (lunar_uint32_t)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    asm volatile("lgdtl (gdt_pointer)");
}

#endif