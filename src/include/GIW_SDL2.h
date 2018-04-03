#ifndef GIW_SDL2_H_
#define GIW_SDL2_H_

#include "GIW_utils.h"

#include <string>
#include <memory>
#include <SDL2/SDL.h>



namespace sdl2{

    

    using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
    using window_ptr_t_shr = std::shared_ptr<SDL_Window>;

    inline window_ptr_t make_window(const char* title, 
                                    int x, int y, int w, int h, Uint32 flags) {
        return make_resource(SDL_CreateWindow, SDL_DestroyWindow, 
                             title, x, y, w, h, flags);
    }


    using renderer_ptr_t = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
    using renderer_ptr_t_shr = std::shared_ptr<SDL_Renderer>;

    
    inline renderer_ptr_t make_renderer(window_ptr_t_shr window, 
                                    int index, int32_t flags) {
        return make_resource(SDL_CreateRenderer, SDL_DestroyRenderer, 
                             window.get(),index,flags);
    }

    using texture_ptr_t = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
    using texture_ptr_t_shr = std::shared_ptr<SDL_Texture>;




    inline texture_ptr_t make_texture(SDL_Renderer *renderer,
                            uint32_t format,
                            int access,
                            int w,
                            int h) {
        return make_resource(SDL_CreateTexture, SDL_DestroyTexture,
                            renderer, format, access, w, h);
    }    

    class Renderer{
        public:
            Renderer():window(make_window("App Name", SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED, 640, 480, 0)),
                       renderer(make_renderer(window,-1,SDL_RENDERER_ACCELERATED ))
                               {}

            Renderer(window_ptr_t_shr window, 
                    int index, 
                    int32_t flags):window(window),
                                   renderer(make_renderer(window,index,flags))
                                    {}
            ~Renderer(){};

            int SetDrawColor(uint8_t r,int8_t g,int8_t b, uint8_t a=0){return SDL_SetRenderDrawColor(renderer.get(),r,g,b,a);}
            
            int Clear(){return SDL_RenderClear(renderer.get());}

            void Present(){SDL_RenderPresent(renderer.get());}
            
            int SetViewPort(const SDL_Rect & r){return SDL_RenderSetViewport(renderer.get(),&r);}
            
            SDL_Renderer* Get(){return renderer.get();}
        
        private:
            window_ptr_t_shr window;
            renderer_ptr_t renderer;
    };
    using Renderer_ptr_t_shr = std::shared_ptr<Renderer>;

}// end sdl

#endif