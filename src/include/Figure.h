#ifndef _FIGURE_H_
#define _FIGURE_H_
// #include <CellSimulation.h>

#include <SDL.h>
#include <vector> 
#include <tuple>


enum color{blue,red,green,magenta};

struct Line
{    
    std::vector<double> xData;
    std::vector<double> yData;
    color c;
    double width;
};

/// A class that provides functunallity to plot a graph 
class Figure {
    public:
        
        Figure(int width=300, int height=300);

        void Plot(std::vector<double> &x, std::vector<double> &y,color c);

        void Render(SDL_Renderer* gRenderer);

    private:
        std::vector<Line> lines;
        Range xlim;
        Range ylim;
        int width;
        int height;
};
 
#endif