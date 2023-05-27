#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "multiboot.h"
#include "font.h"
#include "string.h"

multiboot_info_t *mb_info;

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

#endif