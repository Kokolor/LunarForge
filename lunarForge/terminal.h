#ifndef TERMINAL_H
#define TERMINAL_H

#include "graphics.h"

typedef struct
{
    uint8_t *font;
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

terminal term;

#endif