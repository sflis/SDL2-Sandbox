#include "CellSimulation.h"



bool Cell::Game(){
    if(cellContent.size()>0)
        if(cellContent[0]->type==Creature::Game)
            return true;
    return false;
}


std::vector<Cell*> Cell::GetEmptyNeighbors(){

    // std::cout<<"EnterEmpty "<<neighbors.size()<<std::endl;
    // std::cout<<x<<" "<<y<<std::endl;
    std::vector<Cell*> empty;
    for(auto *c:neighbors)
        if(c->Empty())
            empty.push_back(c);
        // std::cout<<"Worked"<<std::endl;
    return empty;
}
        
std::vector<Cell*> Cell::GetGameNeighbors(){
    std::vector<Cell*> game;
    for(auto *c:neighbors)
        if(c->Game())
            game.push_back(c);
    return game;   
}

void Creature::Duplicate(std::vector<Cell*> emptyCells){
    // std::cout<<"Duplicate"<<std::endl;
    int i = 0;
    int nEmptyCells = emptyCells.size();
    if(nEmptyCells>1)
        i = rand()%(nEmptyCells);
    health = (rand()%500)/1000.0+0.2;
    Creature *c = new Creature(type,1-health,dHealth,speed,emptyCells[i],world);
    emptyCells[i]->InsertCreature(c);
    world->creatures.push_back(c);
    health /=2;
}   


void Creature::Step(){
    if(dead)
        return;
    // std::cout<<"Enter step"<<std::endl;
    // std::cout<<currentCell->x<<" "<<currentCell->y<<std::endl;
    auto emptyCells = currentCell->GetEmptyNeighbors();
    int nEmptyCells = emptyCells.size();
    // std::cout<<"Enter step "<<nEmptyCells<<std::endl;
    if(health >= 1.0 && nEmptyCells>0){
        
        Duplicate(emptyCells);   
    }
    
     // std::cout<<"Step"<<std::endl;
    if(type == Predator){
        // std::cout<<"Predator"<<std::endl;
        auto gameCells = currentCell->GetGameNeighbors();
        int nGameCells = gameCells.size();       
        if(nGameCells>0){
            // std::cout<<nGameCells<<std::endl;
            int i = 0;
            if(nGameCells>1)
                i = rand()%(nGameCells);
            //Moving 
            currentCell->RemoveCreture(this);
            Cell *c = gameCells[i];
            Creature *game = c->cellContent[0];
            health += game->health*0.5;
            game->Kill();
            c->RemoveCreture(game);
            c->InsertCreature(this);
            currentCell = c;            
        }
        else{
            // std::cout<<"predator walk "<<nEmptyCells<<std::endl;
            int i = 0;
            if(nEmptyCells>0){
                if(nEmptyCells>1)
                    i = rand()%(nEmptyCells);
                currentCell->RemoveCreture(this);
                emptyCells[i]->InsertCreature(this);
                currentCell = emptyCells[i];
            }
            // std::cout<<"predator walked"<<std::endl;
        }

        if(health<0){
            dead = true;
            currentCell->RemoveCreture(this);
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
            currentCell->RemoveCreture(this);
            emptyCells[i]->InsertCreature(this);
            currentCell = emptyCells[i];
            // std::cout<<"moved"<<std::endl;
        }
    }

    health += dHealth;


}


void World::RemoveDeadCreatures(){
    // std::cout<<"Remove dead creatures"<<std::endl;
    for (auto it=creatures.begin(); it!=creatures.end();){
        if((*it)->dead){
            delete (*it);
            it = creatures.erase(it);
        }
        else 
          ++it;
    }
}