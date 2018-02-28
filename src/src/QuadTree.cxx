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

}

void QuadTree::Node::AddParticle(Particle *par,QuadTree &tree){
    totalMass += par->mass;
    centerOfMass.x += par->mass * par->pos[0];
    centerOfMass.y += par->mass * par->pos[1];
    if(type==Node::EmptyLeaf){
        particles.push_back(par);
        // particle = par;
        type = ParticleLeaf;
        return;
    }
    else if(type==Node::node){

        if(par->pos[0]>=p.x){
            if(par->pos[1]>=p.y)
                nodes[0]->AddParticle(par,tree);
            else
                nodes[1]->AddParticle(par,tree);
        }
        else{
            if(par->pos[1]<p.y)
                nodes[2]->AddParticle(par,tree);
            else
                nodes[3]->AddParticle(par,tree);       
        }       
    }
    else{
        if(depth>tree.maxDepth or size<tree.minSize){
            particles.push_back(par);   
            return;
        }
        nodes.push_back(std::shared_ptr<Node>(new Node({p.x+size * 0.5,p.y+size * 0.5},size * 0.5, EmptyLeaf,depth+1)));
        nodes.push_back(std::shared_ptr<Node>(new Node({p.x+size * 0.5,p.y-size * 0.5},size * 0.5, EmptyLeaf,depth+1)));
        nodes.push_back(std::shared_ptr<Node>(new Node({p.x-size * 0.5,p.y-size * 0.5},size * 0.5, EmptyLeaf,depth+1)));
        nodes.push_back(std::shared_ptr<Node>(new Node({p.x-size * 0.5,p.y+size * 0.5},size * 0.5, EmptyLeaf,depth+1)));
        type = node;
        
        if(particles[0]->pos[0]>=p.x){
            if(particles[0]->pos[1]>=p.y)
                nodes[0]->AddParticle(particles[0],tree);
            else
                nodes[1]->AddParticle(particles[0],tree);
        }
        else{
            if(particles[0]->pos[1]<p.y)
                nodes[2]->AddParticle(particles[0],tree);
            else
                nodes[3]->AddParticle(particles[0],tree);       
        }


        if(par->pos[0]>=p.x){
            if(par->pos[1]>=p.y)
                nodes[0]->AddParticle(par,tree);
            else
                nodes[1]->AddParticle(par,tree);
        }
        else{
            if(par->pos[1]<p.y)
                nodes[2]->AddParticle(par,tree);
            else
                nodes[3]->AddParticle(par,tree);       
        }
    }
}

Particle QuadTree::Node::GetCenterOfMassParticle() const{
    return {{centerOfMass.x/totalMass,centerOfMass.y/totalMass,0},
            {0,0,0},
            {0,0,0},
            {0,0,0},
            0,
            totalMass
        };
}
QuadTree::Node::Node(Point point, 
                    double size, 
                    NodeType type, 
                    int depth):p(point),
                                size(size),
                                type(type),
                                depth(depth),
                                totalMass(0),
                                centerOfMass({0,0}){


}