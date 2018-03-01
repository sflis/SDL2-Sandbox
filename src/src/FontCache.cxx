#include "FontCache.h"
#include <SDL2/SDL_ttf.h>
#include <vector> 
#include <iostream>
#include <algorithm>

FontCache::FontCache(SDL_Renderer* gRenderer,
                    SDL_Color color):
                    gRenderer(gRenderer),
                    defaultCachedFontSizes({5,
                                            7, 
                                            9,
                                            11,
                                            13,
                                            15,
                                            17,
                                            19,
                                            21,
                                            23,
                                            25,
                                            27,
                                            28,
                                            31,
                                            33,
                                            35,
                                            37,
                                            39,
                                            41}){

    TTF_Init();
    CacheFont(gRenderer, color);

}

void FontCache::CacheFont(SDL_Renderer* gRenderer, SDL_Color color){
    for(auto size : defaultCachedFontSizes){
        TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", size);
        if (font == NULL) {
            std::cout<<"Exiting... Falied to open font"<<std::endl;
            // fprintf(stderr, "error: font not found\n");
            exit(EXIT_FAILURE);
        }

        //populating cache
        char charbuff[2] = {0,0};//empty c-string
        auto &ftc_s = fontTextureCache[color];
        auto &ftc = ftc_s[size]; 
        //iterating over ascii codes that are actual characters
        for(int i = 32; i<126;i++){
            charbuff[0] = i;
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, charbuff, color);
            SDL_Texture* text = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            ftc[charbuff[0]] = {text,textSurface->w,textSurface->h};
            SDL_FreeSurface(textSurface);   
        }
    TTF_CloseFont(font);
    }
}

FontCache::~FontCache(){
    for(auto &cachemap:fontTextureCache){
        for(auto &sizemap: cachemap.second){
            for(auto &cache: sizemap.second)
                SDL_DestroyTexture(cache.second.texture);
        }
    }
}

void FontCache::Render(std::string text,
                    SDL_Rect box, 
                    SDL_Renderer* gRenderer,
                    SDL_Color color, 
                    int size,
                    TextAnchor tanchor,
                    bool wrap){
    
    //Find color
    auto colorSearch = fontTextureCache.find(color);
    if(colorSearch == fontTextureCache.end()){
        //if color not found then cache fonts with the desired color
        FontCache::CacheFont(gRenderer,color);
        colorSearch = fontTextureCache.find(color);
    }
    //find closes matching cached font size
    auto sizeSearch = colorSearch->second.lower_bound(size);
    sizeSearch--;
    if(sizeSearch == colorSearch->second.end()){
        sizeSearch = colorSearch->second.upper_bound(size);
    }
    
    
    std::vector<char> tokens;//For now
    for(auto &c : text){
        tokens.push_back(c);
    }

    int n_tokens = tokens.size();
    bool math_mode = false;
    for(int i = 0, row_pos=0, line=0; i<n_tokens; i++){
        char t = tokens[i];
        if(t=='$'){
            if(math_mode)
                math_mode = false;
            else
                math_mode = true;    
        }
        double scale = 1.0;
        
        switch(t){
            case '$':
                if(math_mode)
                    math_mode = false;
                else
                    math_mode = true;   
                break;
            case '\n':
                line++;
                row_pos=0;
                continue;
            case '^':
                if(math_mode)
                    scale = 0.2;
        }

        if(row_pos > box.w){
            line++;
            row_pos=0;
        }

        if(line * (size) > box.h)
            break;

        TextTextureCacheData &data =  sizeSearch->second[t];
        
        //Scaling the font texture to sizes between cached font sizes
        float cachedSizeRatio  = size/sizeSearch->first;
        SDL_Rect renderQuad = {box.x + row_pos, 
                                int(box.y + line * cachedSizeRatio * data.h), 
                                int(cachedSizeRatio * data.w), 
                                int(cachedSizeRatio * data.h)};    
        SDL_RenderCopy(gRenderer, data.texture, NULL, &renderQuad);
        row_pos += data.w;
    }
}