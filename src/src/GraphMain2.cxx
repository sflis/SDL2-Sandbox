    
#define _USE_MATH_DEFINES


#include "FontCache.h"
#include "Coordinates.h"
#include "Graph.h"
#include "GIW_SDL2.h"

#include <cmath>
#include <cstdlib>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <vector> 
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <map>

int xcenter = 300;
int ycenter = 300;





class GraphApp {
 
    public:
 
        GraphApp();

        ~GraphApp();
 
        int OnExecute();
 
        bool OnInit();
 
        void OnEvent(SDL_Event* Event);
 
        void OnLoop();
 
        void OnRender();
 
        void OnCleanup();
    private:
        bool running;

        //The window we'll be rendering to
        sdl2::window_ptr_t_shr window;

        //The window renderer
        sdl2::Renderer_ptr_t_shr renderer;

        int screen_width;
        int screen_height;

        std::vector< unsigned char > pixels;

        SDL_Texture* texture;

        TTF_Font *font;

        int steps;
        std::ofstream myfile;
        uint64_t now= 0;
        uint64_t last = 0;
        double deltaTime = 0;

        std::vector<double> x;
        std::vector<double> y;
        std::vector<double> y2;
        std::vector<double> y3;

        FontCache * tbox;
        InteractiveFigure *figure;
};



GraphApp::GraphApp() : running(true),
                    screen_width(1700),
                    screen_height(800),
                    steps(0)
                    {};

GraphApp::~GraphApp(){

    //Quit SDL subsystems
    SDL_Quit();
} 


int GraphApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }
    SDL_Event Event;
    std::cout<<"Enter loop"<<std::endl;
    while(running) {
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }
 
        OnLoop();
        OnRender();
    }
 
    OnCleanup();
 
    return 0;
}

bool GraphApp::OnInit() {
    std::srand(3);


    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
        std::cout<<"Failed to init SDL"<<std::endl;
        return false;
    }
    else{
            window = sdl2::make_window("Graph2", SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
            renderer = std::make_shared<sdl2::Renderer>(window,-1, SDL_RENDERER_ACCELERATED);

            renderer->SetDrawColor(0,0,0,0);
            texture = SDL_CreateTexture(
                                        renderer->Get(),
                                        SDL_PIXELFORMAT_RGBA8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        screen_width, screen_height
                                        );

    }
    
    for(int i = 0; i<300; i++){
        y.push_back(1);//rand()%100);
        x.push_back(i);
        y2.push_back(1);  
        y3.push_back(1);   
    }
    SDL_Color c = {0,0,0};
    tbox = new FontCache(renderer->Get(),c);
    figure = new InteractiveFigure(renderer,tbox,{0,0,1,1});
    return true;
}

void GraphApp::OnEvent(SDL_Event* event){
    
        if(event->type == SDL_QUIT){
            
                running = false;
                std::cout<<"Exiting"<<std::endl;
        }
        
        

        //User presses a key
        else if( event->type == SDL_KEYDOWN ){
            //Select surfaces based on key press
            switch( event->key.keysym.sym )
            {
                case SDLK_ESCAPE:
                    running = false;
                    std::cout<<"Exiting"<<std::endl;
                    break;
            }
        }
        figure->HandleInput(event);
    
}
 
void GraphApp::OnLoop(){


}
double t = 0;
int count = 0;
void GraphApp::OnRender(){
    // std::cout<<"Enter OnRender"<<std::endl;
    last = now;
    now = SDL_GetPerformanceCounter();
    deltaTime = (double)((now - last)*1000 / SDL_GetPerformanceFrequency() );
    count++;
    if(int(count)%200==0)
        std::cout<<1000.0/deltaTime<<" fps"<<std::endl;
    // std::cout<<now<<" "<<last<<"  "<<SDL_GetPerformanceFrequency()<<std::endl;
    char str[100];
    sprintf(str,"%.3g fps",1000.0/deltaTime);
    SDL_Rect box ={100,0,800,400};
    
    renderer->SetDrawColor( 0, 0, 0, 0);
    renderer->Clear();
    SDL_Color fontColor = {255,255,255};
    tbox->Render(std::string(str),box,renderer->Get(),fontColor,30);
    auto fig = Figure(renderer,tbox);
    // std::cout<<"Created Figure"<<std::endl;
    t += 0.01;
    sprintf(str,"t =- %.3g",t);
    tbox->Render(std::string("ja\nas dfghjklzxcvbnm"),
                {100,50,80,400},
                renderer->Get(),
                {255,0,255},
                25);
    
    for(int i = 0; i<300; i++){
        y[i] = cos(2*i/300.0*M_PI+t)+0.1*cos(33*i/300.0*M_PI+t)+0.1*cos(35*i/300.0*M_PI+t)+0.1*cos(37*i/300.0*M_PI+t);//+= 0.1*(0.1*(rand()%1000/1000.0)-0.05);
        y2[i] = cos(3*i/300.0*M_PI+t);
        y3[i] = cos(5*i/300.0*M_PI+t);
        x[i] = i/100.0*M_PI;       
    }
    fig.Plot(x,y,blue);
    fig.Plot(x,y2,red);
    fig.Plot(x,y3,green);
    figure->Plot(x,y,blue);

    Range ylim = {-8.0,8.0};
    // fig.SetYLim(ylim);
    SDL_Rect r ={200,100,800,200};
    fig.Render(r);
    figure->Render({200,350,800,200});
    renderer->Present();
    SDL_Delay(30);
}

void GraphApp::OnCleanup(){

}

int main(int argc, char **argv){

    GraphApp theApp;
 
    return theApp.OnExecute();

}   