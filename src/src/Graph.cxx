#include "Graph.h"
#include "FontCache.h"

#include <math.h>
#include <algorithm>
#include <iostream>





void Axis::Render(SDL_Renderer* gRenderer, 
                  BoxCoordinates window_coord, 
                  BoxCoordinates box_coord, 
                  CoordinateTrans cotrans){
    
    SDL_SetRenderDrawColor( gRenderer, tickColor.r, tickColor.g, tickColor.b, 0xFF );
    SDL_Color fontColor=tickLabelColor;

    
    if(type==xaxis){
        //bottom
        Pix p1 = box_coord({0,int(box_coord.inner.h)});
        Pix p2 = box_coord({int(box_coord.inner.w),int(box_coord.inner.h)});
        
        SDL_RenderDrawLine(gRenderer, p1.x, 
                                         p1.y,  
                                         p2.x, 
                                         p2.y
                                         );
        double r = range.max - range.min;
        int power = std::log10(r);
        double half = 0.5;
        if(range.min < 0)
            half = -0.5;
        double minMajorTick = (int(range.min/std::pow(10,power)+half)) * std::pow(10,power);
        char buf[100];
        double scale=1;

        if(r/std::pow(10,power)<2){
            power--;
            scale = 2;
        }
        double majorTickDistance =  std::pow(10,power) * scale;
        for(double i = minMajorTick; i<range.max;i+=majorTickDistance ){
            Pix p = cotrans({i,0.0});
            Pix p1 = box_coord({p.x,box_coord.inner.h});
            Pix p2 = box_coord({p.x,box_coord.inner.h-8});
            SDL_RenderDrawLine(gRenderer, p1.x, 
                                             p1.y,  
                                             p2.x, 
                                             p2.y
                                             );
            std::sprintf(buf,"%.4g",i);
            std::string number(buf);
            Pix window_p = box_coord({p.x, box_coord.inner.h+1});
            Pix xSize = cotrans({majorTickDistance,0.0});
            SDL_Rect box = {int(window_p.x-tickLabelFontSize * 0.5),
                            window_p.y,
                            xSize.x,
                            tickLabelFontSize};
            font->Render(number,
                        box,
                        gRenderer,
                        fontColor,
                        tickLabelFontSize,
                        FontCache::center,
                        false);

        }

        power--;
        double minMinorTick = (int(range.min/std::pow(10,power))) * std::pow(10,power);
        //always start minor ticks on an even number
        if(int(minMinorTick/std::pow(10,power))%2!=0){
            minMinorTick += std::pow(10,power);
        }
        scale *=2;
        for(double i = minMinorTick; i<range.max;i+=std::pow(10,power) * scale ){
            Pix p = cotrans({i,0.0});
            Pix p1 = box_coord({p.x,box_coord.inner.h});
            Pix p2 = box_coord({p.x,box_coord.inner.h-4});
            SDL_RenderDrawLine(gRenderer, p1.x, 
                                             p1.y,  
                                             p2.x, 
                                             p2.y
                                             );
        }

        //top
        p1 = box_coord({0,0});
        p2 = box_coord({int(box_coord.inner.w),0});
        
        SDL_RenderDrawLine(gRenderer, p1.x, 
                                         p1.y,  
                                         p2.x, 
                                         p2.y
                                         );
    }else if(type==yaxis){
        
        //Right y-axis
        Pix p1 = box_coord({0,int(box_coord.inner.h)});
        Pix p2 = box_coord({0,0});
        
        SDL_RenderDrawLine(gRenderer, p1.x, 
                                         p1.y,  
                                         p2.x, 
                                         p2.y
                                         );
        double r = range.max - range.min;
        int power = std::log10(r);
        double half = 0.5;
        if(range.min < 0)
            half = -0.5;
        double minMajorTick = (int(range.min/std::pow(10,power)+half)) * std::pow(10,power);
        char buf[100];
        double scale=1;
        if(r/std::pow(10,power)<2){
            power--;
            scale = 2;
        }

        double majorTickDistance =  std::pow(10,power) * scale;
        for(double i = minMajorTick; i<range.max;i+= majorTickDistance){
            Pix p = cotrans({0.0, i});
            Pix p1 = box_coord({0, p.y});
            Pix p2 = box_coord({8, p.y});
            SDL_RenderDrawLine(gRenderer, p1.x, 
                                             p1.y,  
                                             p2.x, 
                                             p2.y
                                             );
            std::sprintf(buf,"%.3g",i);
            std::string number(buf);
            Pix py = box_coord({0, p.y});
            Pix window_p = window_coord({5, 0});
            SDL_Rect box = {window_p.x,py.y-tickLabelFontSize/2,tickLabelFontSize,tickLabelFontSize};
            font->Render(number,box,gRenderer,fontColor,tickLabelFontSize);
        }
        
        power--;
        double minMinorTick = (int(range.min/std::pow(10,power))) * std::pow(10,power);
        scale *=2;

        if(int(minMinorTick/std::pow(10,power))%2!=0){
            minMinorTick += std::pow(10,power);
        }
        for(double i = minMinorTick; i<range.max;i+=std::pow(10,power) * scale ){
            Pix p = cotrans({0.0,i});
            Pix p1 = box_coord({0,p.y});
            Pix p2 = box_coord({4,p.y});
            SDL_RenderDrawLine(gRenderer, p1.x, 
                                             p1.y,  
                                             p2.x, 
                                             p2.y
                                             );
        }

        //Left y-axis
        p1 = box_coord({int(box_coord.inner.w),int(box_coord.inner.h)});
        p2 = box_coord({int(box_coord.inner.w),0});
        
        SDL_RenderDrawLine(gRenderer, p1.x, 
                                         p1.y,  
                                         p2.x, 
                                         p2.y
                                         );
    }
}




//=============================================================================
Figure::~Figure(){

}

//=============================================================================
Figure::Figure(sdl2::Renderer_ptr_t_shr renderer, FontCache *font):width(0),
                                              height(0),
                                              xlim({0.0,0.0}),
                                              ylim({0.0,0.0}),
                                              font(font),
                                              font_shared(true),
                                              paddingTop(20),
                                              paddingBottom(40),
                                              paddingRight(20),
                                              paddingLeft(30),
                                              xaxis(Axis::xaxis,xlim,font),
                                              yaxis(Axis::yaxis,ylim,font),
                                              renderer(renderer){
    


}
//=============================================================================
Line & Figure::Plot(std::vector<double> &x, std::vector<double> &y,uint32_t c){
    Line newLine{x,y,c,0,1.0};
    lines.push_back(newLine);
    Update();
    return lines.back();
    
}
//=============================================================================
void Figure::Render(SDL_Rect rect){
    // std::cout<<"ENTER figure render"<<std::endl;

    width = rect.w;
    height = rect.h;
    if(ylim.min==ylim.max){
        ylim.max +=1.0;
    }
    yaxis.GetRange().min = ylim.min;
    yaxis.GetRange().max = ylim.max;
    xaxis.GetRange().min = xlim.min;
    xaxis.GetRange().max = xlim.max;
    // double dx = width/(xlim.max-xlim.min);
    // double dy = height/(ylim.max-ylim.min); 
    

    BoxCoordinates window_coord(rect,rect); //FIXME: Add window box
    SDL_Rect canvas = rect;
    canvas.x += paddingLeft;
    canvas.y += paddingTop;
    canvas.h -= paddingTop+paddingBottom;
    canvas.w -= paddingLeft+paddingRight;

    BoxCoordinates box_coord(canvas,canvas);
    CoordinateTrans cotrans(canvas,xlim,ylim);
    // std::cout<<"Draw lines"<<std::endl;
    renderer->SetDrawColor(255, 255, 255, 0xFF );
    SDL_RenderFillRect(renderer->Get(), &rect);

    renderer->SetDrawColor(220, 220, 220, 0xFF );
    SDL_RenderFillRect(renderer->Get(), &canvas);
    
    for(auto &line: lines){
      
        renderer->SetDrawColor(hex2rgb(line.c));
        SDL_Point points[line.xData.size()];
        for(uint64_t i = 0; i<line.xData.size();i++){
            Pix p = box_coord(cotrans({line.xData[i], line.yData[i]}));
            if(p.y>canvas.y+canvas.h){
                p.y=canvas.y+canvas.h;     
            }
            else if(p.y<canvas.y){
                p.y=canvas.y;     
            }

            points[i].x =p.x;
            points[i].y =p.y;

        }
        SDL_RenderDrawLines(renderer->Get(), points,line.xData.size());
            
    }

    //Render axis
    yaxis.Render(renderer->Get(),window_coord,box_coord,cotrans);
    xaxis.Render(renderer->Get(),window_coord,box_coord,cotrans);
    renderer->SetDrawColor(255, 255, 255, 0xFF );    
    //draw Outer box border
    SDL_RenderDrawRect(renderer->Get(), &rect);

}

//=============================================================================
void Figure::Update(){
  for(auto &l: lines){
    auto maxX=std::max_element(l.xData.begin(),l.xData.end());
    auto minX=std::min_element(l.xData.begin(),l.xData.end());
    auto maxY=std::max_element(l.yData.begin(),l.yData.end());
    auto minY=std::min_element(l.yData.begin(),l.yData.end());

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
}
//=============================================================================
InteractiveFigure::InteractiveFigure(sdl2::Renderer_ptr_t_shr renderer, 
                                    FontCache *font, 
                                    SDL_Rect rect):
                                    Figure(renderer,font),
                                    loc(rect){

}
//=============================================================================
void InteractiveFigure::Render(SDL_Rect rect){
    Figure::Render(rect);
    char buf[100];
    std::sprintf(buf,"(%.4d,%.4d)",mouse_x,mouse_y);
    font->Render(buf,
                        rect,
                        renderer->Get(),
                        hex2rgb(0xfffff),
                        10,
                        FontCache::center,
                        false);

}
//=============================================================================
void InteractiveFigure::HandleInput(SDL_Event* e){
  
  SDL_GetMouseState( &mouse_x, &mouse_y );

  // Pix = {x,y};
  

}
