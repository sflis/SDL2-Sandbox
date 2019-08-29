#ifndef GRAPH_H_
#define GRAPH_H_

// #include "FontCache.h"
#include "Coordinates.h"
#include "Utils.h"
#include "GIW_SDL2.h"

#include <SDL2/SDL.h>

#include <vector>



struct Line
{    
    std::vector<double> xData;
    std::vector<double> yData;
    uint32_t c;
    uint32_t alpha;
    // color c;
    // SDL_Color c;
    double width;
};

class FontCache;

//=============================================================================
class Axis{
    public:
        enum AxType{xaxis,yaxis};
        Axis(AxType type, 
            Range range,
            FontCache *font, 
            int maxMajorTicks=10):range(range),
                                type(type),
                                maxMajorTicks(maxMajorTicks),
                                font(font),
                                tickLabelFontSize(20){}
        Range &GetRange(){return range;}
        void Render(SDL_Renderer* gRenderer, 
                  BoxCoordinates window_coord, 
                  BoxCoordinates box_coord, 
                  CoordinateTrans cotrans);
        void SetTickLabelColor(SDL_Color c){tickLabelColor = c;}
        void SetTickColor(SDL_Color c){tickColor = c;} 
    private:
        Range range;
        AxType type;
        int maxMajorTicks;
        FontCache *font;
        int tickLabelFontSize;
        SDL_Color tickLabelColor = {0,0,0,0};
        SDL_Color tickColor = {0,0,0,0};

};


//=============================================================================
/// A class that provides functunallity to plot a graph 
class Figure {
    public:
        
        Figure(sdl2::Renderer_ptr_t_shr renderer, FontCache *font);
        ~Figure();

        Line & Plot(std::vector<double> &x, std::vector<double> &y,uint32_t c);

        void Render(SDL_Rect rect);
        void SetYLim(Range r){ylim = r;}
        Axis &GetYaxis(){return yaxis;}
        Axis &GetXaxis(){return xaxis;}
        void SetCanvasBackgroundColor(SDL_Color c){canvasBackgroundColor = c;}
        void SetWindowBackgroundColor(SDL_Color c){windowBackgroundColor = c;}
        void Update();
    protected:
        std::vector<Line> lines;
        int width;
        int height;
        Range xlim;
        Range ylim;
        FontCache *font;
        bool font_shared;
        double paddingTop;
        double paddingBottom;
        double paddingRight;
        double paddingLeft;
        Axis xaxis;
        Axis yaxis;
        SDL_Color canvasBackgroundColor;
        SDL_Color windowBackgroundColor;
        sdl2::Renderer_ptr_t_shr renderer;
        // SDL_Texture* texture;
                    
};


class InteractiveFigure: public Figure{
    public:
        InteractiveFigure(sdl2::Renderer_ptr_t_shr renderer, FontCache *font, SDL_Rect rect);
        void Render(SDL_Rect rect);
        void HandleInput(SDL_Event* e);

    private:
        SDL_Rect loc;
        int mouse_x;
        int mouse_y;
};

#endif