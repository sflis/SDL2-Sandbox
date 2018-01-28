#include "Figure.h"
#include <algorithm>

Figure::Figure(int width, int height):width(width),height(height),xlim({0.0,0.0}),ylim({0.0,0.0}){
}

void Figure::Plot(std::vector<double> &x, std::vector<double> &y,color c){
    Line newLine{x,y,c,1.0};
    lines.push_back(newLine);
    auto maxX=std::max_element(x.begin(),x.end());
    auto minX=std::min_element(x.begin(),x.end());
    auto maxY=std::max_element(y.begin(),y.end());
    auto minY=std::min_element(y.begin(),y.end());

    if(*maxX>xlim.max){
        xlim.max = *maxX;
    }
    if(*minX<xlim.min){
        xlim.min=*minX;
    }

    if(*maxY>ylim.max){
        ylim.max = *maxY;
    }
    if(*minY<ylim.min){
        ylim.min=*minY;
    }

}

void Figure::Render(SDL_Renderer* gRenderer){
    double dx = width/(xlim.max-xlim.min);
    double dy = height/(ylim.max-ylim.min); 

    
    for(auto &line: lines){
        int r,b,g;
        switch(line.c){
            case blue:
                r = 0;b = 255;g = 0;
                break;
            case green:
                r = 0;b = 0;g = 255;
                break;
            case red:
                r = 255;b = 0;g = 0;
                break;
        }
        SDL_SetRenderDrawColor( gRenderer, r, g, b, 0xFF );

        for(uint64_t i = 1; i<line.xData.size();i++){
            SDL_RenderDrawLine(gRenderer, dx*line.xData[i-1], height- dy*line.yData[i-1],  dx*line.xData[i], height-dy*line.yData[i]);
        }
            
            
    }
}