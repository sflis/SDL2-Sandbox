#include <vector>
#include <deque>
#include <algorithm>
#include <iostream>

class Cell;
class Creature;
class World;

class Cell{
    public:
        Cell(int x, int y):x(x),y(y){}
        void SetNeighbors(std::vector<Cell*> v){neighbors=v;}

        void InsertCreature(Creature *c){cellContent.push_back(c);}
        
        void RemoveCreture(Creature *c){
                                    auto c_it = std::find(cellContent.begin(),cellContent.end(),c);
                                    cellContent.erase(c_it);
                                }
        
        bool Empty(){return cellContent.empty();}
        bool Game();
        std::vector<Cell*> GetEmptyNeighbors();
        std::vector<Cell*> GetGameNeighbors();
        int x;
        int y;
        std::deque<Creature*> cellContent;
    private:

        std::vector<Cell*> neighbors;
};


class Creature{
    public:
        enum CretureType{Predator,Game};
        CretureType type;
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
        void Kill(){dead=true;}
        void Duplicate(std::vector<Cell*> emptyCells);
        bool dead;
    private:
        double dHealth;
        double health;
        double speed;
        Cell* currentCell;
        
        World *world;
};



struct World{
    World(int width, int height): width(width), height(height){
        cells.resize(width*height);
        //Initializing cells
        for(int x = 0; x < width; x++){
            for(int y = 0; y < height; y++){
                cells[x*height + y] = new Cell(x,y);
            }
        }

        for(int x = 0; x<width; x++){
            for(int y = 0; y<height; y++){
                std::vector<Cell*> neighbors;
                //down
                if(y<(height-1))neighbors.push_back(cells[x*height + y + 1]);    
                
                //right
                if(x<(width-1))neighbors.push_back(cells[(x+1)*height+y]);   
                
                
                //up
                if(y>0)neighbors.push_back(cells[x*height+y-1]);
                //left
                if(x>0)neighbors.push_back(cells[(x-1)*height+y]);
                
                for(auto *c: neighbors)
                    c->Empty();
                cells[x*height + y]->SetNeighbors(neighbors);
            }
        }
        Cell *c = cells[0];
        // std::cout<<c->x<<" "<<c->y<<c->GetEmptyNeighbors().size()<<std::endl;

    }  
    void AddCreature(int x,int y,Creature *c){
        cells[x*height+y]->InsertCreature(c); 
        creatures.push_back(c);
    }
    void RemoveDeadCreatures();
    int width;
    int height;
    std::vector<Cell*> cells;
    std::deque<Creature*> creatures;
};