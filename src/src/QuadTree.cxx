#include "QuadTree.h"
#include <iostream>

QuadTree::QuadTree(int maxDepth,
                     double minSize):root({0,0},1,QuadTree::Node::EmptyLeaf,0),
                                            maxDepth(maxDepth),
                                            minSize(minSize){


}

void QuadTree::BuildTree(std::vector<Particle> &particles){
    max = {-1e-300,-1e-300};
    min = {1e-300,1e-300};
    for(const auto  &p: particles){
        if(max.x<p.pos[0])
            max.x=p.pos[0];
        if(max.y<p.pos[1])
            max.y=p.pos[1];
        
        if(min.x>p.pos[0])
            min.x=p.pos[0];
        if(min.y>p.pos[1])
            min.y=p.pos[1];
    }

    Point range = {max.x-min.x,max.y-min.y};
    Point origo = {(max.x+min.x)/2,(max.y+min.y)/2};
    double side = (range.x>range.y ? range.x : range.y)*1.1;
    root.p = origo;
    root.size = side;

    for(auto  &p: particles){
        root.AddParticle(&p, *this);
    }
    root.UpdateActiveNodes(0);
}

bool QuadTree::CondParticleAdd(Particle *par, double dist){
    
    par++;
    dist+=1;
    // Point range = {max.x-min.x,max.y-min.y};
    // Point origo = {(max.x+min.x)/2,(max.y+min.y)/2};
    // double side = (range.x>range.y ? range.x : range.y)*1.1;
    // root.p = origo;
    // root.size = side;
    
    // if( fabs(root.p[0]-par->pos[0]) > root.size ||
    //     fabs(root.p[1]-par->pos[1]) > root.size){
    //     Point origo = {(max.x+min.x)/2,(max.y+min.y)/2};
    //     if(root.size==0){
    //         double side = (range.x>range.y ? range.x : range.y)*1.1;
    //     }
    //     double side = root.size * 2;
    //     origo = root.p;
    //     root.size;    

    //     auto oldRoot = std::shared_ptr<Node>(new Node(root));

        
    // }

    // bool success;
    // for(auto  &p: particles){
    //     auto node = root.AddParticle(&p, *this)
        
    //     if(){
    //         success=true;
    //         break;
    //     }
    // }
    // return success;
    return false;
}




QuadTree::Node& QuadTree::Node::DryAdd(Particle *par, QuadTree &tree){
    if(type==Node::EmptyLeaf){
        
        return *this;
    }
    else if(type==Node::node){

        if(par->pos[0]>=p.x){
            if(par->pos[1]>=p.y)
                nodes[0]->DryAdd(par,tree);
            else
                nodes[1]->DryAdd(par,tree);
        }
        else{
            if(par->pos[1]<p.y)
                nodes[2]->DryAdd(par,tree);
            else
                nodes[3]->DryAdd(par,tree);       
        }       
    }
    else{
        return *this;

    }    
}

void QuadTree::Node::AddParticle(Particle *par, QuadTree &tree){
    totalMass += par->mass;
    centerOfMass.x += par->mass * par->pos[0];
    centerOfMass.y += par->mass * par->pos[1];
       
    switch(type){

        case Node::EmptyLeaf:
            particles.push_back(par);
            type = ParticleLeaf;
            return;
       
        case Node::ParticleLeaf:{
            if(depth>tree.maxDepth or size<tree.minSize){
                particles.push_back(par);   
                return;
            }
            //Add nodes beneath the current leaf and change the type to node
            double halfsize = size * 0.5;
            auto newdepth = depth+1;
            nodes.push_back(std::make_shared<Node>( Point{p.x-halfsize, p.y-halfsize}, halfsize, EmptyLeaf, newdepth));
            nodes.push_back(std::make_shared<Node>( Point{p.x+halfsize, p.y-halfsize}, halfsize, EmptyLeaf, newdepth));
            nodes.push_back(std::make_shared<Node>( Point{p.x-halfsize, p.y+halfsize}, halfsize, EmptyLeaf, newdepth));
            nodes.push_back(std::make_shared<Node>( Point{p.x+halfsize, p.y+halfsize}, halfsize, EmptyLeaf, newdepth));
            type = node;
            
            //determining the index of the quadrant in which the old particle lands
            //the four quadrants are nicely encoded in two bits:
            uint16_t quadrandIndexp = 0;
            quadrandIndexp = uint16_t(particles[0]->pos[0]>=p.x);
            quadrandIndexp = quadrandIndexp | uint16_t(particles[0]->pos[1]>=p.y)<<1;    
            nodes[quadrandIndexp]->AddParticle(particles[0],tree);
            particles.clear();
        }
        case Node::node:{   
            //determining the index of the quadrant in which the new particle lands
            uint16_t quadrandIndex = 0;
            quadrandIndex = uint16_t(par->pos[0]>=p.x);
            quadrandIndex = quadrandIndex | uint16_t(par->pos[1]>=p.y)<<1;
            nodes[quadrandIndex]->AddParticle(par,tree);
            return;
        }
        default:
            std::cout<<"ERROR"<<std::endl;
    
    }
}

int QuadTree::Node::UpdateActiveNodes(int count){
    activeNodes.clear();
    if(type == Node::ParticleLeaf){
        count++;
    }
    for(size_t i = 0; i<nodes.size(); i++){
        switch(nodes[i]->type){
             case Node::node:
             case Node::ParticleLeaf:
                activeNodes.push_back(i);
                count += nodes[i]->UpdateActiveNodes(0);
            default:
            break;
        }
    }
        
    return count;
}

Particle QuadTree::Node::GetCenterOfMassParticle() const{
    return {{centerOfMass.x/totalMass,centerOfMass.y/totalMass,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            0,
            totalMass,
            int(0xffffffff),
            0
        };
}
QuadTree::Node::Node(Point point, 
                    double size, 
                    NodeType type, 
                    int depth):
                                p(point),
                                size(size),
                                type(type),
                                depth(depth),
                                totalMass(0),
                                centerOfMass({0,0}){


}

