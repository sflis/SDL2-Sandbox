#ifndef QUADTREE_H_
#define QUADTREE_H_
#include "ParticleSimulation.h"

// #include <SDL.h>

#include <memory>
#include <array>
#include <vector>
#include <set>

struct Point{
    double x;
    double y;
};

class QuadTree{
    public:
        QuadTree(int maxDepth = 1150, double minSize=1e-16);
        void BuildTree(std::vector<Particle> &particles);
        bool CondParticleAdd(Particle *par, double dist);    
        enum Quadrant{first=3,second=2,third=1,fourth=0};
        class Node{
            public:
                enum NodeType{uninitialized,EmptyLeaf,node, ParticleLeaf};
                Node(Point p, double size, NodeType type, int depth); 
                Point p;
                double size;
                void AddParticle(Particle *par,QuadTree &tree);
                Node& DryAdd(Particle *par,QuadTree &tree);
                void UpdateActiveNodes();
                std::vector<std::shared_ptr<Node> > nodes;// std::array<std::shared_ptr<Node>,4> nodes;
                std::vector<int> activeNodes;
                std::vector<Particle*> particles;
                NodeType type;
                int depth;
                Particle GetCenterOfMassParticle() const;
            private:
                double totalMass;
                Point centerOfMass;               
        };
        Node& GetRoot(){return root;} 
    private:
        
        Point max;
        Point min;
        Node root;
        int maxDepth;
        double minSize;
 
};

#endif