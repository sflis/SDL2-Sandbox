#ifndef _CELLAPP_H_
#define _CELLAPP_H_
#include <CellSimulation.h>

#include <SDL.h>
#include <vector> 
class CellApp {
 
    public:
 
        CellApp();

        ~CellApp();
 
        int OnExecute();
 
        bool OnInit();
 
        void OnEvent(SDL_Event* Event);
 
        void OnLoop();
 
        void OnRender();
 
        void OnCleanup();
    private:
        bool running;

        //The window we'll be rendering to
        SDL_Window* gWindow;
    
        //The surface contained by the window
        SDL_Surface* gScreenSurface;

        //The window renderer
        SDL_Renderer* gRenderer;

        int screen_width;
        int screen_height;

        std::vector< unsigned char > pixels;

        SDL_Texture* texture;
        World world;
        int steps;
};
 
#endif