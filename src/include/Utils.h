#ifndef _UTILS_H
#define _UTILS_H

#include <SDL2/SDL.h>

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
    
    SDL_Color color = {r,g,b};
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

#endif