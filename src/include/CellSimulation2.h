#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>

class Cell;
class Creature;
class World;

class Cell{
    public:
        enum CellStat{Empty,Predator,Pray};
        Cell():x(0),y(0),occupied(true){}
        Cell(int x, int y):x(x),y(y),occupied(false),stat(Cell::Empty){}
        void SetNeighbors(std::vector<Cell*> v){neighbors=v;}
        void EnterCell(Creature *c);
        void LeaveCell(){occupied=false;stat = Cell::Empty;}
        Creature *cellContent;        
        int x;
        int y;
        bool occupied;
        std::vector<Cell*> neighbors;
        CellStat stat;

};


class Creature{
    public:
        enum CretureType{Predator,Game};
        CretureType type;
        Creature():type(Predator),
                        health(0),
                        dHealth(0),
                        speed(0),
                        currentCell(nullptr),
                        dead(true),
                        world(nullptr){}

        Creature(CretureType type, 
                double health,
                double dHealth, 
                double speed, 
                Cell *cell,
                World *world): type(type),
                            health(health),
                            dHealth(dHealth),
                            speed(speed),
                            currentCell(cell),
                            dead(false),
                            world(world){}        
        void Step();
        void Kill(){dead=true; currentCell->LeaveCell(); currentCell=nullptr;}
        void Duplicate(int nEmptyCells, Cell *emptyCells[]);
        void SetCellStat(Cell &c){  switch(type){ 
                                        case Creature::Predator:
                                            c.stat = Cell::Predator;
                                            break;
                                        case Creature::Game:
                                            c.stat = Cell::Pray;
                                            break;
                                        }
                                }
        bool dead;
        double health;
    private:
        double dHealth;
        
        double speed;
        Cell *currentCell;
        
        World *world;
};



struct World{
    World(int width, int height): width(width), height(height){
        cells.resize(width * height);
        //Initializing cells
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                cells[x * height + y] = Cell(x,y);
            }
        }

        for(int x = 0; x<width; x++){
            for(int y = 0; y<height; y++){
                std::vector<Cell*> neighbors;
                //down
                if(y<(height-1))neighbors.push_back(&cells[x * height + y + 1]);    
                
                //right
                if(x<(width-1))neighbors.push_back(&cells[(x+1) * height+y]);   
                
                
                //up
                if(y>0)neighbors.push_back(&cells[x * height+y-1]);
                //left
                if(x>0)neighbors.push_back(&cells[(x-1) * height+y]);
                
                cells[x*height + y].SetNeighbors(neighbors);
            }
        }
        // Cell *c = cells[0];
        // std::cout<<c->x<<" "<<c->y<<c->GetEmptyNeighbors().size()<<std::endl;

    }  
    void AddCreature(int x,int y,Creature *c){
        cells[x * height+y].EnterCell(c); 
        creatures.push_back(c);
    }
    void RemoveDeadCreatures();
    int width;
    int height;
    std::vector<Cell> cells;
    std::deque<Creature*> creatures;
};



inline void Cell::EnterCell(Creature *c){cellContent=c; occupied=true;c->SetCellStat(*this);}