#pragma once
#ifndef GRPAHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../../../kernel/include/multitask/mman.h"

#define WIDTH 1024
#define HEIGHT 768

namespace Graphics
{
    struct Window
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    void Init();
    void DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void Terminate();

    void WritePixel(const uint32_t x, const uint32_t y, const uint32_t colour);
}

#endif