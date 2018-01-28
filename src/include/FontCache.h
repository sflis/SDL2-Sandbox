#ifndef _FONTCACHE_H_
#define _FONTCACHE_H_

#include <SDL.h>
#include <map>
#include <string>

#include "Utils.h"


class FontCache{
    
    struct TextTextureCacheData{
        SDL_Texture* texture;
        int w;
        int h;
    }; 

    public:
        enum TextAnchor{left,center,right};
        FontCache(SDL_Renderer* gRenderer, SDL_Color color={255,255,255,0});
        ~FontCache();
        
        void Render(std::string text,
                    SDL_Rect box, 
                    SDL_Renderer* gRenderer, 
                    SDL_Color color,
                    int size=10,
                    TextAnchor tanchor = FontCache::left,
                    bool wrap= true);

    private:
        void CacheFont(SDL_Renderer* gRenderer, SDL_Color color);
        std::map<SDL_Color, std::map<char, TextTextureCacheData> > fontTextureCache;  
        SDL_Renderer* gRenderer;
};


#endif