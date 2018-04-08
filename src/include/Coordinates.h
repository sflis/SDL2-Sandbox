#ifndef _COORDINATES_H_
#define _COORDINATES_H_

#include <SDL2/SDL.h>

struct Range{
    double min;
    double max;
};

struct Rect{
    Rect(const SDL_Rect &sdl_rect){
        x = sdl_rect.x;
        y = sdl_rect.y;
        h = sdl_rect.h;
        w = sdl_rect.w;
    }
   double x;
   double y;
   double w;
   double h; 
};

struct Pix{
    // Pix(int x, int y):x(x),y(y){}
    // Pix():Pix(0,0){}
    int x;
    int y;
};

struct Point2d{
    double x;
    double y;
};

//=============================================================================
class BoxCoordinates{
    public:
        BoxCoordinates(Rect outer, Rect inner):outer(outer),inner(inner){}

        Pix operator()(Pix pix);
    // private:
        Rect outer;
        Rect inner;
};

inline Pix BoxCoordinates::operator()(Pix pix){
    Pix outpix;
    outpix.x = inner.x + pix.x;
    outpix.y = inner.y + pix.y;
    return outpix;
}
//=============================================================================
class CoordinateTrans{
    public:
        CoordinateTrans(Rect outer, Range x, Range y):outer(outer),x(x),y(y){}
        Pix operator()(Point2d pos);
        Pix operator()(double x, double y);
        Pix Coord2d2Pix(Point2d pos);
        Point2d Pix2Coord2d(Pix pix);
    
    private:
        Rect outer;
        Range x;
        Range y;
};

inline Pix CoordinateTrans::Coord2d2Pix(Point2d pos){
    Pix outpix;
    double xrange = x.max-x.min;
    double yrange = y.max-y.min;
    outpix.x = (pos.x-x.min )/ xrange * outer.w;
    outpix.y = outer.h-(pos.y-y.min )/ yrange * outer.h;
    return outpix;
}

inline Pix CoordinateTrans::operator()(Point2d pos){
    return Coord2d2Pix(pos);
}



inline Pix CoordinateTrans::operator()(double xx, double yy){
    // Pix outpix;
    // double xrange = x.max-x.min;
    // double yrange = y.max-y.min;
    // outpix.x = (xx-x.min )/xrange*outer.w;
    // outpix.y = outer.h-(yy-y.min )/yrange*outer.h;
    return Coord2d2Pix({xx,yy});//outpix;
}

inline Point2d CoordinateTrans::Pix2Coord2d(Pix pix){
    Point2d outCoord;
    // double xrange = x.max-x.min;
    // double yrange = y.max-y.min; 
    outCoord.x = pix.x-outer.x;
    return outCoord;
}

#endif