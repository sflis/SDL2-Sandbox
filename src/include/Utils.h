#ifndef _UTILS_H
#define _UTILS_H

#include "GIW_SDL2.h"

#include <SDL2/SDL.h>

#include <vector>

#include <iostream>
inline uint32_t rgb2hex(uint8_t r,int8_t g,int8_t b){
    return (r << 16) + (g << 8) + b;
}

inline uint32_t rgb2hex(SDL_Color color){
    return (color.r << 16) + (color.g << 8) + color.b;
}


inline SDL_Color hex2rgb(int32_t hexcolor){
    uint8_t r = ( hexcolor >> 16 ) & 0xFF;

    uint8_t g = ( hexcolor >> 8 ) & 0xFF;

    uint8_t b = hexcolor & 0xFF;
    
    SDL_Color color = {r,g,b,0xFF};
    return color;
}


inline bool operator==(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return rgb2hex(lhs)==rgb2hex(rhs);
}

inline bool operator<=(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return rgb2hex(lhs)<=rgb2hex(rhs);
}

inline bool operator>=(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return rgb2hex(lhs)>=rgb2hex(rhs);
}

inline bool operator<(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return rgb2hex(lhs)<rgb2hex(rhs);
}

inline bool operator>(const SDL_Color& lhs, const SDL_Color& rhs)
{
    return rgb2hex(lhs)>rgb2hex(rhs);
}


class RawPixelMap{
    public:
        RawPixelMap(SDL_Renderer *renderer, uint32_t width, uint32_t height):
        width(width),
        height(height),
        texture(sdl2::make_texture(renderer,
                            SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_STREAMING,
                            width, 
                            height)),
        pixelmap(uint32_t(width * height * 4)),
        size(pixelmap.size())
        {}

        void SetPixel(uint32_t x, uint32_t y, SDL_Color color){
            
            if(x>=width || y>=height)
                return;
            const unsigned int offset = (width * 4 * y)  + (x * 4);
            
            if(offset >= size)
                return;
            pixelmap[offset + 0] = color.r;
            pixelmap[offset + 1] = color.g;
            pixelmap[offset + 2] = color.b;
            pixelmap[offset + 3] = color.a;
        }
        std::vector<unsigned char> &GetMap(){return pixelmap;}
    private:

        uint32_t width;
        uint32_t height;
        sdl2::texture_ptr_t_shr texture;
        std::vector<unsigned char> pixelmap;
        uint32_t size;
};

using RawPixelMap_ptr_t_shr = std::shared_ptr<RawPixelMap>;
#endif