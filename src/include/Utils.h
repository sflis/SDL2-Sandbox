#ifndef _UTILS_H
#define _UTILS_H

#include "GIW_SDL2.h"
#include "Coordinates.h"

#include <SDL2/SDL.h>

#include <vector>

#include <iostream>
///Converts rgba colors to hex color
inline uint32_t rgb2hex(uint8_t r,int8_t g,int8_t b, int8_t a){
    return (r << 24) + (g << 16) + (b << 8) + a;
}

///Converts an SDL_Color rgba struct to hex color
inline uint32_t rgb2hex(SDL_Color color){
    return (color.r << 24) + (color.g << 16) + (color.b << 8) + color.a;
}

///Converts an SDL_Color rgba struct to reverse order hex color
inline uint32_t rgb2hex_r(SDL_Color color){
    return (color.a << 24) + (color.b << 16) + (color.g << 8) + color.r;
}


inline SDL_Color hex2rgb(int32_t hexcolor){
    uint8_t r = ( hexcolor >> 24 ) & 0xFF;

    uint8_t g = ( hexcolor >> 16 ) & 0xFF;

    uint8_t b = (hexcolor >> 8) & 0xFF;

    uint8_t a = hexcolor & 0xFF;
    
    SDL_Color color = {r,g,b,a};
    return color;
}

inline SDL_Color hex2rgb_r(int32_t hexcolor){
    uint8_t r =  hexcolor        & 0xFF;

    uint8_t g = (hexcolor >> 8 ) & 0xFF;

    uint8_t b = (hexcolor >> 16) & 0xFF;

    uint8_t a = (hexcolor >> 24) & 0xFF;
    
    SDL_Color color = {r,g,b,a};
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
            
            return SetPixel({x,y},color);
        }

        void SetPixel(Pix p, SDL_Color color){
            
            if(p.x>=width || p.y>=height)
                return;
            
            const unsigned int offset = (width *  p.y) + (p.x );
            pixelmap[offset] = rgb2hex(color);

        }

        uint32_t& operator[](Pix p){
                        const unsigned int offset = width *  p.y + p.x;
                        return pixelmap[offset];
                    }

        std::vector<uint32_t> &GetMap(){return pixelmap;}

    private:

        uint32_t width;
        uint32_t height;
        sdl2::texture_ptr_t_shr texture;
        std::vector<uint32_t> pixelmap;
        uint32_t size;
};

using RawPixelMap_ptr_t_shr = std::shared_ptr<RawPixelMap>;


class View{
    public:
        View(sdl2::RendererPtr renderer, 
            uint32_t width, 
            uint32_t height,
            Pix anchor,            
            Range x,
            Range y
            ):
        width(width),
        height(height),
        texture(sdl2::make_texture(renderer->Get(),
                            SDL_PIXELFORMAT_BGRA8888,
                            SDL_TEXTUREACCESS_STREAMING,
                            width, 
                            height)),
        // pixelmap(uint32_t(width * height * 4)),
        pixelmap(uint32_t(width * height)),
        size(pixelmap.size()),
        trans(Rect({anchor.x,anchor.y,width,height}),x,y),
        anchor(anchor)
        {}

        void SetPixel(uint32_t x, uint32_t y, SDL_Color color){
            
            return SetPixel({x,y},color);
        }

        void SetPixel(Pix p, SDL_Color color){
            
            if(p.x>=width || p.y>=height)
                return;
            
            const unsigned int offset = (width *  p.y)  + (p.x );
            pixelmap[offset] = rgb2hex_r(color);

        }

        std::vector<unsigned char> &GetMap(){return pixelmap;}
        std::vector<uint32_t> &GetMap2(){return pixelmap2;}

    private:

        uint32_t width;
        uint32_t height;
        sdl2::texture_ptr_t_shr texture;
        std::vector<unsigned char> pixelmap;
        std::vector<uint32_t> pixelmap2;
        uint32_t size;
        CoordinateTrans trans;
        Pix anchor;
};

#endif