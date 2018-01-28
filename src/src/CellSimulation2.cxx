#include "CellSimulation2.h"



void Creature::Duplicate(int nEmptyCells, Cell *emptyCells[]){
    // std::cout<<"Duplicate"<<std::endl;
    int i = 0;
    if(nEmptyCells>1)
        i = rand()%(nEmptyCells);
    // emptyCells +=i;
    health = (rand()%500)/1000.0+0.2;
    Creature *c = new Creature(type,1-health,dHealth,speed,emptyCells[i],world);
    // if(emptyCells[i]->occupied){
    //     std::cout<<"Empty cell occupied!"<<std::endl;
    //     exit(1);
    // }
    emptyCells[i]->EnterCell(c);
    world->creatures.push_back(c);
}   


void Creature::Step(){
    if(dead)
        return;
    // if(!currentCell->occupied){
    //     std::cout<<"occupied cell not occupied!"<<std::endl;
    //     std::cout<<type<<std::endl;
    //     exit(1);
    // }
    // std::cout<<"Step"<<std::endl;
    Cell *emptyCells[4];
    int nEmptyCells = 0;
    for(auto *cell: currentCell->neighbors){
        if(!cell->occupied){
            emptyCells[nEmptyCells] = cell;
            nEmptyCells++;
        }
    }
    // std::cout<<"Empty cells"<<nEmptyCells<<std::endl;
    if(health >= 1.0 && nEmptyCells>3){
        Duplicate(nEmptyCells, emptyCells);
        return;   
    }
        
     
    if(type == Predator){
        // std::cout<<"Predator"<<std::endl;
        int nGameCells=0;
        Cell *gameCells[4];
        for(auto *cell: currentCell->neighbors){
            if(cell->occupied && cell->cellContent->type==Creature::Game){
                gameCells[nGameCells] = cell;
                nGameCells++;
            }
        }
        // std::cout<<nGameCells<<std::endl;       
        if(nGameCells>0){
            
            int i = 0;
            if(nGameCells>1)
                i = rand()%(nGameCells);
            //Moving 
            currentCell->LeaveCell();
            Cell *c = gameCells[i];
            Creature *game = c->cellContent;
            health += game->health*0.3;
            game->Kill();
            // c->RemoveCreture(game);
            c->EnterCell(this);
            currentCell = c;            
        }
        else{
            // std::cout<<"predator walk "<<nEmptyCells<<std::endl;
            int i = 0;
            if(nEmptyCells>0){
                if(nEmptyCells>1)
                    i = rand()%(nEmptyCells);
                currentCell->LeaveCell();
                emptyCells[i]->EnterCell(this);
                currentCell = emptyCells[i];
            }
            // std::cout<<"predator walked"<<std::endl;
        }

        if(health<0){
            dead = true;
            if(currentCell!=nullptr)
                currentCell->LeaveCell();
        }
    }
    else{
        

        // std::cout<<"Game"<<std::endl;
        if(nEmptyCells>0){
            // std::cout<<"Game walk "<<nEmptyCells<<std::endl;
            int i = 0;
            if(nEmptyCells>1)
                i = rand()%(nEmptyCells);
            // std::cout<<i<<std::endl;
            // std::cout<<emptyCells[i]->x<<" "<<emptyCells[i]->y<<std::endl;
            currentCell->LeaveCell();
            emptyCells[i]->EnterCell(this);
            currentCell = emptyCells[i];
            // std::cout<<"moved"<<std::endl;
        }
    }
    if(health<1)
        health += dHealth;
    if(health>1)
        health = 1;

    if(rand()%1000>995){
        dead = true;
        currentCell->LeaveCell();
    }


}


void World::RemoveDeadCreatures(){
    // std::cout<<"Remove dead creatures"<<std::endl;
    int n = creatures.size();
    for(int i =0; i<n; i++){
        if(creatures[i]->dead){
            delete creatures[i];
            n--;
            std::swap(creatures[i], creatures.back());
            creatures.pop_back();
        }
    }
 
}