#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <minlib.h>
#include "widget.h"
#include "font.h"

namespace Graphics
{
    class Text: public Widget
    {
    public:
        Text() {}
        
        Text(char const* text);
        
        Text(char const* text, const unsigned int x, const unsigned int y);

        ~Text() {}

        uint32_t GetPixel(const uint32_t x, const uint32_t y) const override;

        bool IsPixelSet(const uint32_t screenX, const uint32_t screenY) const override;

    private:
        char m_Text[32];
    };
}

#endif
