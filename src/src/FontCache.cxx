#include "FontCache.h"
#include <SDL2/SDL_ttf.h>
#include <vector> 
#include <iostream>


FontCache::FontCache(SDL_Renderer* gRenderer,SDL_Color color):gRenderer(gRenderer){

    TTF_Init();
    CacheFont(gRenderer, color);

}

void FontCache::CacheFont(SDL_Renderer* gRenderer, SDL_Color color){
    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 80);
    if (font == NULL) {
        std::cout<<"Exiting... Falied to open font"<<std::endl;
        // fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }

    //populating cache
    char charbuff[2] = {0,0};//empty c-string
    auto &ftc = fontTextureCache[color]; 
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

FontCache::~FontCache(){
    for(auto &cachemap:fontTextureCache){
        for(auto &cache: cachemap.second)
            SDL_DestroyTexture(cache.second.texture);
    }
}

void FontCache::Render(std::string text,
                    SDL_Rect box, 
                    SDL_Renderer* gRenderer,
                    SDL_Color color, 
                    int size,
                    TextAnchor tanchor,
                    bool wrap){

    auto search = fontTextureCache.find(color);
    if(search == fontTextureCache.end()){
        FontCache::CacheFont(gRenderer,color);
        search = fontTextureCache.find(color);
    }
    
    std::vector<char> tokens;//For now
    for(auto &c : text){
        tokens.push_back(c);
    }

    int n_tokens = tokens.size();
    bool math_mode = false;
    for(int i = 0,row_pos=0,line=0; i<n_tokens; i++){
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

        if(line*(size)>box.h)
            break;

        TextTextureCacheData &data =  search->second[t];
        //To handle non-monospaced fonts correctly
        //the width of the rendered character is scaled by the
        //ratio of the width and height of the font character.
        double dwdh = double(data.w)/data.h;
        
        SDL_Rect renderQuad = {box.x+row_pos, box.y+line*size, size*dwdh, size };    
        SDL_RenderCopy(gRenderer, data.texture, NULL, &renderQuad);
        row_pos += size*dwdh;
    }
    
}