#include "ParticleSimulation.h"
#include "Graph.h"
#include "FontCache.h"
#include "GIW_SDL2.h"
#include "Utils.h"
#include "QuadTree.h"
#include "colormap.h"

#include <cstdlib>
#include <SDL2/SDL.h>
#include <vector> 
#include <iostream>
#include <fstream>
#include <math.h>
#include <random>


class ParticleApp {
 
    public:
 
        ParticleApp();

        ~ParticleApp();
 
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

        // std::vector< unsigned char > pixels;

        SDL_Texture* texture;
        ParticleSimulation *sim;
        int steps;
        std::ofstream myfile;
        uint64_t now= 0;
        uint64_t last = 0;
        double deltaTime = 0;
        double x;
        double y;
        double zoom;
        Range xrange;
        Range yrange;
        CoordinateTrans trans;
        std::vector<double> fps;
        std::vector<double> kineticEnergy;
        std::vector<double> timeVector;
        double time;
        FontCache *tbox;
        bool animateTree;
        bool traces;
        bool barnesHut;
        int visualizationState;
        int nVisualizationStates;
        bool particleTag;
        uint32_t particleID;
        RawPixelMap_ptr_t_shr rpixmap;
        

};




ParticleApp::ParticleApp():
                    running(true),
                    screen_width(1600),
                    screen_height(900),
                    steps(0),
                    x(.0),
                    y(.0),
                    zoom(100.0),
                    xrange({-zoom*double(screen_width)/screen_height+x,zoom*double(screen_width)/screen_height+x}),
                    yrange({y-zoom,y+zoom}),
                    trans({0,0,screen_width,int(screen_height * 0.8)},xrange,yrange),
                    time(0),
                    animateTree(true),
                    traces(false),
                    barnesHut(true),
                    visualizationState(0),
                    nVisualizationStates(4),
                    particleTag(false),
                    particleID(0)
                    {};
//=============================================================================
ParticleApp::~ParticleApp(){

    //Quit SDL subsystems
    SDL_Quit();
} 

//=============================================================================
int ParticleApp::OnExecute() {
    if(OnInit() == false) {
        return -1;
    }
    SDL_Event Event;
    int i= 0;
    while(running) {
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }
        // if(i>170)
            // break;
        OnLoop();
        OnRender();
        i++;
    }
 
    OnCleanup();
 
    return 0;
}
//=============================================================================
double uniform(double min,double max){
    double r = (std::rand()%RAND_MAX)/double(RAND_MAX);
    double range = max-min;
    return min +r*range;
}
//=============================================================================

bool ParticleApp::OnInit() {
    std::srand(3);
    // pixels.resize( int(screen_height*0.8) * screen_width * 4);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){

        return false;
    }
    else{
        //Create window

        window = sdl2::make_window("Particles", SDL_WINDOWPOS_UNDEFINED, 
                          SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_SHOWN);
        renderer = std::make_shared<sdl2::Renderer>(window,-1, SDL_RENDERER_ACCELERATED);

        renderer->SetDrawColor(0,0,0,0);
        // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"); 
        // SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"); 
        rpixmap = std::make_shared<RawPixelMap>(renderer->Get(),screen_width, screen_height);

        texture = SDL_CreateTexture(
                                renderer->Get(),
                                    SDL_PIXELFORMAT_BGRA8888,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    screen_width, screen_height*0.8
                                    );

        
    }
    std::vector<Particle> particles;
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0,1);
    std::uniform_real_distribution<> veldist(3.7,3.7);
    std::uniform_real_distribution<> th(0.0,2*M_PI);
    double radius = 30;
    // for(int i=0; i<16500;i++){
    //     double theta = th(e2);
    //     double d = dist(e2);//veldist(e2);
    //     double r =sqrt(radius*radius*d);
    //     double vel = 0.00051;//veldist(e2);
    //     particles.push_back({{20+r*cos(theta),10+r*sin(theta),0},
    //                             {vel*(1500*r)*cos(theta+M_PI/2),vel*(1500*r)*sin(theta+M_PI/2),0},
    //                             {0,0,0},
    //                             {0,0,0},
    //                             10.0,
    //                             1.0,
    //                             i
    //                             });
    // }
    for(int i=0; i<3650;i++){
        double theta = uniform(0,2*M_PI);//th(e2);
        double d = uniform(0,1);//dist(e2);//veldist(e2);
        double r =sqrt(radius*radius*d);
        double vel = 0.00015;//veldist(e2);
        particles.push_back({{20+r*cos(theta),10+r*sin(theta),0},
                                {vel*(1500*r)*cos(theta+M_PI/2)-10,vel*(1500*r)*sin(theta+M_PI/2),0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                1.0,
                                i
                                });
    }

    for(int i=0; i<3500;i++){
        double theta = th(e2);
        double r = 20*dist(e2);//veldist(e2);
        double vel = 0.11;//veldist(e2);
        particles.push_back({{-20+r*cos(theta),-10+r*sin(theta),0},
                                {vel*sqrt(1500*r)*cos(theta+M_PI/2)+10,vel*sqrt(1500*r)*sin(theta+M_PI/2),0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                1.0,
                                i
                                });
    }
    


    particles.push_back({{20,10,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                200,
                                10000
                                });
    sim = new ParticleSimulation(xrange.min,xrange.max,yrange.min,yrange.max,particles);    
    SDL_Color c = {0,0,0,0xFF};
    tbox = new FontCache(renderer->Get(),c);
    return true;
}
//=============================================================================
void ParticleApp::OnEvent(SDL_Event* event){
    
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
                case SDLK_q:
                    animateTree = animateTree ? false:true;
                    break;
                case SDLK_t:
                    traces = traces ? false:true;
                    break;
                case SDLK_b:    
                    barnesHut = barnesHut ? false:true;
                    break;
                case SDLK_p:    
                    particleTag = particleTag ? false:true;
                    break;                    
                case SDLK_LEFT:
                    if(particleTag){
                        if(particleID>0)
                            particleID--;
                        else
                            particleID = sim->GetParticles().size()-1;
                    }
                    break;
                case SDLK_RIGHT:
                    if(particleTag){
                        particleID++;
                        if(particleID>=sim->GetParticles().size())
                            particleID = 0;
                    }
                    break;
                case SDLK_c:    
                    visualizationState++;
                    if(visualizationState>nVisualizationStates)
                        visualizationState = 0;
                    break;
                case SDLK_k:
                    zoom +=0.1;
                    xrange = {-zoom*double(screen_width)/screen_height+x,zoom*double(screen_width)/screen_height+x};
                    yrange = {y-zoom,y+zoom};
                    trans =  CoordinateTrans({0,0,screen_width,int(screen_height * 0.8)},xrange,yrange);
                    break;
                case SDLK_m:
                    zoom -=0.1;
                    xrange = {-zoom*double(screen_width)/screen_height+x,zoom*double(screen_width)/screen_height+x};
                    yrange = {y-zoom,y+zoom};
                    trans =  CoordinateTrans({0,0,screen_width,int(screen_height * 0.8)},xrange,yrange);
                    break;
            }
        }

    
}
//=============================================================================
void ParticleApp::OnLoop(){
    double dt = 0.0005;
    for(int i = 0; i<10;i++){
        if(barnesHut)
            sim->BarnesHutSum(dt);
        else
            sim->Step(dt);
        // 
        time+=dt;
    }
    timeVector.push_back(time);
}
//=============================================================================
void RenderNode(QuadTree::Node &node, sdl2::Renderer &renderer,CoordinateTrans trans){

    if(node.type == QuadTree::Node::EmptyLeaf or node.type == QuadTree::Node::ParticleLeaf){
        return;
    }
    SDL_SetRenderDrawColor( renderer.Get(), node.depth*30, 255, 255-node.depth*30, 0xFF );
    auto p1 = trans(node.p.x+node.size,node.p.y);
    auto p2 = trans(node.p.x-node.size,node.p.y);
  
    SDL_RenderDrawLine(renderer.Get(),p1.x,p1.y,  
                                      p2.x,p2.y);
    p1 = trans(node.p.x,node.p.y+node.size);
    p2 = trans(node.p.x,node.p.y-node.size);
    SDL_RenderDrawLine(renderer.Get(),p1.x,p1.y,  
                                      p2.x,p2.y);
    for(auto &n: node.nodes)
        RenderNode(*n,renderer,trans);
}
//=============================================================================
void RenderTree(sdl2::Renderer &renderer, QuadTree &tree, CoordinateTrans trans){
    QuadTree::Node &root = tree.GetRoot();
    SDL_SetRenderDrawColor( renderer.Get(), 115, 115, 115, 0xFF );
    RenderNode(root, renderer,trans);

}
//=============================================================================
void renderTaggedParticle(int particleID, 
                            const std::vector<Particle> &particles, 
                            CoordinateTrans trans,
                            int screen_height,
                            int screen_width,
                            sdl2::Renderer &renderer,
                            FontCache &tbox ){
    auto p = particles[particleID];
    auto pix = trans(p.pos[0],p.pos[1]);
        
    if(pix.x >=screen_width || pix.y>=screen_height || pix.x<0 || pix.y<0)
        return;
    renderer.SetDrawColor(0,255,0,0xFF);
    for(int i = -1;i<2;i++){
        for(int j = -1;j<2;j++){
                SDL_RenderDrawPoint(renderer.Get(),pix.x+j,pix.y+i);
        }
    }
    char str[300];
    sprintf(str,"Particle ID: %d \nMass: %.3g \nPosition: (%5.3g, %5.3g)\nVelocity: (%5.3g,%5.3g).",
        particleID,p.mass,p.pos[0],p.pos[1],p.vel[0],p.vel[1]);
    tbox.Render(std::string(str),{0,0,300,100},renderer.Get(),{255,255,25,0xFF},14);

}

//=============================================================================
void ParticleApp::OnRender(){
    last = now;
    now = SDL_GetPerformanceCounter();
    deltaTime = (double)((now - last) * 1000 / SDL_GetPerformanceFrequency() );
    SDL_Rect mainViewport = {0,
                            int(screen_height * 0.2),
                            screen_width,
                            int(screen_height * 0.8)};


    auto particles =sim->GetParticles();
    QuadTree tree;

    if(animateTree)
        tree.BuildTree(particles);


    auto &pixels = rpixmap->GetMap();
    if(!traces){
        std::fill(pixels.begin(), pixels.end(), 0);
    }

    int i = 0;
    double energy = 0;
    double maxVel = 0, minVel=1e300;
    for(auto &p:particles){
        double vel = sqrt(p.vel[0]*p.vel[0]+p.vel[1]*p.vel[1]);
        if(maxVel<vel)
            maxVel=vel;
        if(minVel>vel)
            minVel=vel;

    }
    double velRange = maxVel - minVel;
    
    int n = 1028;
    unsigned char colormap[n*3];
    ColorMap::PLSequentialBlackBody(n, &colormap[0]);
    std::vector<double> histogram(pixels.size(),0);
    
    switch(visualizationState){
        case 0:
        case 3:
            {
               
                for(auto &p:particles){
                    auto pix = trans(p.pos[0],p.pos[1]);
                    if(pix.x >=screen_width || pix.y>=screen_height )
                        continue;
                    for(int i = -1;i<2;i++){
                        for(int j = -1;j<2;j++){
                            const unsigned int index = ( screen_width  * (pix.y+i) ) + (pix.x+j) ;
                            if(index<histogram.size() ){
                                histogram[index]+=70.0/(1+abs(i)+abs(j));
                            }
                        }
                    }                            
                }
            }
            break;

        default:
        break;
    }
    

    for(auto &p:particles){
        double vel = sqrt(p.vel[0]*p.vel[0]+p.vel[1]*p.vel[1]);
        energy += vel*vel*p.mass/2;     
        auto pix = trans(p.pos[0],p.pos[1]);
        
        if(pix.x >=screen_width || pix.y>=screen_height || pix.x<0 || pix.y<0)
            continue;
                
        switch(visualizationState){
            case 0:
                for(int i = -1;i<2;i++){
                    for(int j = -1;j<2;j++){    
                        if(pix.y+i<0 || pix.x+j<0)
                            continue;
                        const  uint64_t index = ( screen_width  * (pix.y+i) ) + (pix.x+j) ;
                        if(index<histogram.size()){                            
                            int k = histogram[index];
                            if(k>=n)
                                k = n-1;
                            rpixmap->SetPixel(pix.x+j, pix.y+i, 
                                {float(k)/n*255,
                                 float(k)/n*255,
                                 float(k)/n*255,
                                SDL_ALPHA_OPAQUE});
                        }
                    }
                }
                break;
            case 1:
                {
                    rpixmap->SetPixel(pix, 
                                {uint8_t(255*((vel-minVel)/velRange)),
                                vel>minVel+velRange*0.5 ? uint8_t(255*((maxVel-vel)/velRange)) :uint8_t(255*((vel-minVel)/velRange)),
                                uint8_t(255*((maxVel-vel)/velRange)),
                                SDL_ALPHA_OPAQUE});
                }   
                break;
            case 2:
                {
                    uint32_t i = int(n*(vel-minVel)/velRange);
                    rpixmap->SetPixel(pix, 
                                {colormap[i*3+2],
                                colormap[i*3+1],
                                colormap[i*3],
                                SDL_ALPHA_OPAQUE});

                }   
                break;
            case 3:
                {
                for(int i = -1;i<2;i++){
                    for(int j = -1;j<2;j++){    
                        if(pix.y+i<0 || pix.x+j<0)
                            continue;
                        const  uint64_t index = ( screen_width  * (pix.y+i) ) + (pix.x+j) ;
                        if(index<histogram.size()){                            
                            int k = histogram[index];
                            if(k>=n)
                                k = n-1;
                            rpixmap->SetPixel(pix.x+j, pix.y+i, 
                                {colormap[k*3+2],
                                colormap[k*3+1],
                                colormap[k*3],
                                SDL_ALPHA_OPAQUE});
                        }
                    }
                }
                }
                break;
            default:
                visualizationState =0;
        }

        i++;
    }

    kineticEnergy.push_back(log10(energy));

    // std::cout<<"Rendered particles "<<std::endl;
    // std::cout<<energy<<std::endl;
    renderer->SetViewPort(mainViewport);


    SDL_UpdateTexture(texture,
        NULL,
        &pixels[0],
        screen_width * 4
        );

    SDL_RenderCopy( renderer->Get(), texture, NULL, NULL );
    if(particleTag) 
        renderTaggedParticle(particleID, particles, 
                            trans,
                            screen_height,
                            screen_width,
                            *renderer,
                            *tbox);

    if(animateTree) 
        RenderTree(*renderer, tree,trans);

    // std::cout<<"Rendered Tree"<<std::endl;

    SDL_Rect plotViewport = {0,0,screen_width,int(screen_height*0.2)};
    renderer->SetViewPort( plotViewport);
    plotViewport.w = screen_width/2;

    auto fig = Figure(renderer,tbox);
    fig.Plot(timeVector,kineticEnergy,blue);
    fig.SetYLim({4.8,log10(energy)*1.1});
    fig.Render(plotViewport);
    

    
    
    auto fig2 = Figure(renderer,tbox);
    fps.push_back((1000.0/deltaTime));
    fig2.Plot(timeVector,fps,blue);
    plotViewport.x = screen_width/2;
    fig2.SetYLim({0, 1.5*fps[fps.size()-1]});
    fig2.Render(plotViewport);

    char str[100];
    sprintf(str,"%.2g fps",1000.0/deltaTime);
    SDL_Rect box ={0,0,800,400};
    SDL_Color fontColor = {0,0,0,0};
    tbox->Render(std::string(str),box,renderer->Get(),fontColor,40);
    //Update screen
    renderer->Present() ;
    SDL_Delay(5);
}
//=============================================================================
void ParticleApp::OnCleanup(){

}
//=============================================================================
int main(int argc, char **argv){

    ParticleApp theApp;
 
    return theApp.OnExecute();

}