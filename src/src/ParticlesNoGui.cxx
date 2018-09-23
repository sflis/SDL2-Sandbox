#include "ParticleSimulation.h"
#include "Graph.h"
#include "FontCache.h"
#include "GIW_SDL2.h"
#include "Utils.h"
#include "QuadTree.h"

#include <cstdlib>
#include <vector> 
#include <iostream>
#include <fstream>
#include <math.h>
// #include <random>
#include <chrono>

double uniform(double min,double max){
    double r = (std::rand()%RAND_MAX)/double(RAND_MAX);
    double range = max-min;
    return min +r * range;
}

int main(int argc, char **argv){
    srand(1);
    std::vector<Particle> particles;
    // std::random_device rd;
    // std::mt19937 e2(rd());
    // std::uniform_real_distribution<> dist(0,1);
    // std::uniform_real_distribution<> veldist(3.7,3.7);
    // std::uniform_real_distribution<> th(0.0,2*M_PI);
    double radius = 0.01;
    // for(int i=0; i<1250;i++){
    //     double theta = uniform(0,2 * M_PI);//th(e2);
    //     double d = uniform(0,1);//dist(e2);//veldist(e2);
    //     double r =sqrt(radius * radius * d);
    //     double vel = 0.00051;//veldist(e2);
    //     particles.push_back({{20+r * cos(theta),10+r * sin(theta),0},
    //                             {vel * (1500 * r) * cos(theta+M_PI/2),vel * (1500 * r) * sin(theta+M_PI/2),0},
    //                             {0,0,0},
    //                             {0,0,0},
    //                             10.0,
    //                             1.0,
    //                             i
    //                             });
    // }
    for(int i=0; i<129;i++){
        double theta = uniform(0,2 * M_PI);//th(e2);
        double d = uniform(0,1);//dist(e2);//veldist(e2);
        double r =sqrt(radius * radius * d);
        double vel = 0.00051;//veldist(e2);
        particles.push_back({{20+r * cos(theta),10+r * sin(theta),0},
                                {vel * (1500 * r) * cos(theta+M_PI/2),vel * (1500 * r) * sin(theta+M_PI/2),0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                1.0,
                                i
                                });
    }
    particles.push_back({{-20,10,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
    particles.push_back({{-20,-10,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
    particles.push_back({{2,1,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
    particles.push_back({{20.01,1.01,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
        particles.push_back({{20.02,1.02,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
            particles.push_back({{20.03,1.03,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });

    particles.push_back({{20,10,0},
                                {0,0,0},
                                {0,0,0},
                                {0,0,0},
                                10.0,
                                100,
                                10000
                                });
    std::cout << "Number of particles: " <<particles.size() << std::endl;
    ParticleSimulation *sim;
    
    sim = new ParticleSimulation(-100,100,-100,100,particles);    
    while(1){
        auto start = std::chrono::high_resolution_clock::now();
        double dt = 0.0005;
        for(int i = 0; i<1;i++){
                sim->BarnesHutSum(dt);
            // else
                // sim->Step(dt);
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
        std::cout << "FPS: " << 1/elapsed.count() << " s\n";
        break;
    }   

}