#include "CellApp.h"
#include <cstdlib>


CellApp::CellApp() : running(true),
                    screen_width(800),
                    screen_height(800),
                    world(800,800),
                    steps(0)
                    {};

CellApp::~CellApp(){

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

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
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN );
        if( gWindow == NULL ){
            return false;
        }
        else{
           //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL ){
                return false;
            }
            else{
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            }
            // Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );

            texture = SDL_CreateTexture(
                                        gRenderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        screen_width, screen_height
                                        );

        }
    }

    for( unsigned int i = 0; i < 50; i++ ){
        const unsigned int xx = rand() % screen_width;
        const unsigned int yy = rand() % screen_height;
        for(unsigned int i = 0; i < 60; i++ ){
            unsigned int x = xx + rand() % 15-15;
            unsigned int y = yy + rand() % 15-15;
            if(x>=screen_width)
                x = screen_width-1;
            if(y>=screen_height)
                y = screen_height-1;
            if(world.cells[x*screen_height+y]->Empty())
                world.AddCreature(x,y,new Creature(Creature::Game,0.6*(rand()%1000)/1000.0,0.01,1,world.cells[x*screen_height+y],&world));
        }
        for(unsigned int i = 0; i < 3; i++ ){
            unsigned int x = xx + rand() % 15-15;
            unsigned int y = yy + rand() % 15-15;
            if(x>=screen_width)
                x = screen_width-1;
            if(y>=screen_height)
                y = screen_height-1;
            if(world.cells[x*screen_height+y]->Empty())
                world.AddCreature(x,y,new Creature(Creature::Predator,0.6*(rand()%1000)/1000.0,-0.02,1,world.cells[x*screen_height+y],&world));
        }
    }


    // for( unsigned int i = 0; i < 6000; i++ ){
    //     const unsigned int x = rand() % screen_width;
    //     const unsigned int y = rand() % screen_height;
         
    //     if(world.cells[x*screen_height+y]->Empty())
    //         world.AddCreature(x,y,new Creature(Creature::Predator,0.6 *(rand()%1000)/1000.0,-0.02,1,world.cells[x*screen_height+y],&world));
        
    // }

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
    // for(auto *c:world.creatures){
    //     c->Step();
    // }
    world.RemoveDeadCreatures();
    steps++;
    std::cout<<"Steps: "<<steps<<std::endl;
    std::cout<<"Predators: "<<predCount<<" Games: "<<gameCount<<" Total: "<<predCount+gameCount<<std::endl;
}

void CellApp::OnRender(){
    SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );
    //Clear screen
    SDL_RenderClear( gRenderer );
    
        for(auto *cell:world.cells){
            int x = cell->x;
            int y = cell->y;
            const unsigned int offset = ( screen_width * 4 * y ) + x * 4;
            int g = 255;
            int r = 255;
            if(cell->Empty()){
                g = 0;
                r = 0;
            }
            else if(cell->Game())
                r = 0;
            else
                g = 0;
            pixels[ offset + 0 ] = 0;        // b
            pixels[ offset + 1 ] = g;        // g
            pixels[ offset + 2 ] = r;        // r
            pixels[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
        }


        SDL_UpdateTexture
            (
            texture,
            NULL,
            &pixels[0],
            screen_width * 4
            );

        SDL_RenderCopy( gRenderer, texture, NULL, NULL );

    //Update screen
    SDL_RenderPresent( gRenderer );

}

void CellApp::OnCleanup(){

}