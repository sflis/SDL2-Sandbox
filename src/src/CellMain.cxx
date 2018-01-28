#include "CellSimulation2.h"
#include "Graph.h"
#include "FontCache.h"
#include "GIW_SDL2.h"
#include <cstdlib>


#include <SDL.h>
#include <vector> 
#include <iostream>
#include <fstream>
#include <math.h>

int xcenter = 300;
int ycenter = 300;
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
        sdl2::window_ptr_t_shr window;

        //The window renderer
        sdl2::Renderer_ptr_t_shr renderer;

        int screen_width;
        int screen_height;

        std::vector< unsigned char > pixels;

        SDL_Texture* texture;
        
        World world;
        int steps;
        std::ofstream myfile;
        uint64_t now= 0;
        uint64_t last = 0;
        double deltaTime = 0;
        std::vector<double> ncreatures;
        std::vector<double> npred;
        std::vector<double> npray;
        std::vector<double> stepsdata;
        std::vector<double> fps;
        FontCache *tbox;
};




CellApp::CellApp() : running(true),
                    screen_width(1000),
                    screen_height(800),
                    world(screen_width,screen_height),
                    steps(0)
                    {};

CellApp::~CellApp(){

    //Quit SDL subsystems
    SDL_Quit();
} 


int CellApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }
    SDL_Event Event;
 
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

bool CellApp::OnInit() {
    std::srand(3);
    pixels.resize( screen_height * screen_width * 4);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){

        return false;
    }
    else{
        //Create window

        window = sdl2::make_window("Graph2", SDL_WINDOWPOS_UNDEFINED, 
                          SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
        renderer = std::make_shared<sdl2::Renderer>(window,-1, SDL_RENDERER_ACCELERATED);

        renderer->SetDrawColor(0,0,0,0);
        // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); 
        // SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); 
        texture = SDL_CreateTexture(
                                renderer->Get(),
                                    SDL_PIXELFORMAT_ARGB8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    screen_width, screen_height
                                    );

        
    }

    for( unsigned int i = 0; i < 50; i++ ){
        const unsigned int xx = rand() % screen_width;
        const unsigned int yy = rand() % screen_height;
        for(unsigned int i = 0; i < 180; i++ ){
            unsigned int x = xx + rand() % 15-15;
            unsigned int y = yy + rand() % 15-15;
            if(x>=screen_width)
                x = screen_width-1;
            if(y>=screen_height)
                y = screen_height-1;
            if(!world.cells[x*screen_height+y].occupied)
                world.AddCreature(x,y,new Creature(Creature::Game,0.6*(rand()%1000)/1000.0,0.01,1,&world.cells[x*screen_height+y],&world));
        }
        for(unsigned int i = 0; i < 3; i++ ){
            unsigned int x = xx + rand() % 15-15;
            unsigned int y = yy + rand() % 15-15;
            if(x>=screen_width)
                x = screen_width-1;
            if(y>=screen_height)
                y = screen_height-1;
            if(!world.cells[x*screen_height+y].occupied)
                world.AddCreature(x,y,new Creature(Creature::Predator,0.6*(rand()%1000)/1000.0,-0.02,1,&world.cells[x*screen_height+y],&world));
        }
    }
    std::cout<<"Initialized creatures"<<std::endl;
    
    myfile.open ("CellSimData.txt");
    myfile<<"predators,game, total"<<std::endl;
    SDL_Color c = {0,0,0};
    tbox = new FontCache(renderer->Get(),c);
    return true;
}

void CellApp::OnEvent(SDL_Event* event){
    
        if(event->type == SDL_QUIT){
            
                running = false;

        }
        //User presses a key
        else if( event->type == SDL_KEYDOWN ){
            //Select surfaces based on key press
            switch( event->key.keysym.sym )
            {
                case SDLK_ESCAPE:
                    running = false;
                    break;
            }
        }

    
}
 
void CellApp::OnLoop(){
    int predCount = 0,gameCount = 0;

    for(int i = 0; i<world.creatures.size();i++){
        world.creatures[i]->Step();
        switch(world.creatures[i]->type){
            case Creature::Predator:
                predCount++;
                break;
            case Creature::Game:
                gameCount++;
                break;
        }
    }

    world.RemoveDeadCreatures();
    stepsdata.push_back(steps);
    steps++;
    std::cout<<"Steps: "<<steps<<std::endl;
    std::cout<<"Predators: "<<predCount<<" Games: "<<gameCount<<" Total: "<<predCount+gameCount<<std::endl;
    std::cout<<world.creatures.size()<<std::endl;
    myfile<<predCount<<","<<gameCount<<","<<gameCount+predCount<<std::endl;
    ncreatures.push_back(log10(gameCount+predCount));
    npray.push_back(log10(gameCount));
    npred.push_back(log10(predCount));

}

void CellApp::OnRender(){
    
    SDL_Rect mainViewport = {0,
                            int(screen_height*0.2),
                            screen_width,
                            int(screen_height*0.8)};
    renderer->SetDrawColor( 0, 0, 0, 0xFF );
    renderer->Clear();
    renderer->SetViewPort(mainViewport);
    
    last = now;
    now = SDL_GetPerformanceCounter();
    deltaTime = (double)((now - last)*1000 / SDL_GetPerformanceFrequency() );
    // std::cout<<1000.0/deltaTime<<" fps"<<std::endl;
    // std::cout<<now<<" "<<last<<"  "<<SDL_GetPerformanceFrequency()<<std::endl;

    int occupied = 0;
    for(auto &cell:world.cells){
        int x = cell.x;
        int y = cell.y;
        const unsigned int offset = ( screen_width * 4 * y ) + x * 4;
        int g = 0;
        int r = 0;
        // if(cell->occupied)
            // occupied++;
        switch(cell.stat){
            // case Cell::Empty:
            //     g = 0;
            //     r = 0;
            //     break;
            case Cell::Predator:
                r = cell.cellContent->health*255;
                break;
            case Cell::Pray:
                g = cell.cellContent->health*255;   
                break;
        }
        
        pixels[ offset + 0 ] = 0;        // b
        pixels[ offset + 1 ] = g;        // g
        pixels[ offset + 2 ] = r;        // r
        pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
    }
    // std::cout<<"occupied cells "<<occupied<<std::endl;

    SDL_UpdateTexture(texture,
        NULL,
        &pixels[0],
        screen_width * 4
        );

    SDL_Rect plotViewport = {0,0,screen_width,int(screen_height*0.2)};

    SDL_RenderCopy( renderer->Get(), texture, NULL, NULL );
    renderer->SetViewPort( plotViewport);
    plotViewport.w = screen_width/2;

    auto fig = Figure(renderer,tbox);
    
    auto fig2 = Figure(renderer,tbox);
    fps.push_back((1000.0/deltaTime));

    fig.Plot(stepsdata,ncreatures,blue);
    fig.Plot(stepsdata,npred,red);
    fig.Plot(stepsdata,npray,green);
    fig2.Plot(stepsdata,fps,blue);
    fig.Render(plotViewport);
    plotViewport.x = screen_width/2;
    Range ylim = {0, 1.5*fps[fps.size()-1]};
    fig2.SetYLim(ylim);
    SDL_Color c = {255,0,0};

    fig2.GetYaxis().SetTickLabelColor(c);
    c = {0,155,50};
    fig2.GetYaxis().SetTickColor(c);
    fig2.Render(plotViewport);
    char str[100];
    sprintf(str,"%.2g fps",1000.0/deltaTime);
    SDL_Rect box ={0,0,800,400};
    SDL_Color fontColor = {0,0,0};
    tbox->Render(std::string(str),box,renderer->Get(),fontColor,40);
    //Update screen
    renderer->Present() ;


}

void CellApp::OnCleanup(){

}

int main(int argc, char **argv){

    CellApp theApp;
 
    return theApp.OnExecute();

}