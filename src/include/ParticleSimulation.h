#ifndef PARTICLESIMULATION_H_
#define PARTICLESIMULATION_H_

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
// #include <random>

// #include <QuadTree.h>
class QuadTree;

struct Particle{
    double pos[3];
    double vel[3];
    double acc[3];
    double oldacc[3];
    double charge;
    double mass;
    int id;
};

struct Accelerations
{
    double acc[3];
    double oldacc[3];
    double b;
    double c;
};

struct ParticlePool{

    std::mutex lock;
    std::queue<int> indexQueue;
};

class Worker;
void threadWrapper(Worker *w);
void threadWrapper2(Worker *w);
class Worker{
    public:
        Worker(int nThreads, 
                int threadIndex, 
                std::vector<Particle> &p, 
                std::condition_variable &c, 
                std::mutex &l, 
                bool &r, 
                ParticlePool &pPool);
        ~Worker(){stop = true; thread.join();}
        void Start(){thread = std::thread(threadWrapper2,this);}    
        void Run();
        void Run2();
        void SetTree(QuadTree *t){tree=t;}
        std::mutex &glock;
        std::mutex llock;
        // std::conditional_variable finished;
        std::condition_variable &gcv;
        std::condition_variable cv;
        bool ready;
        bool finished;
    
    private:
        std::thread thread;
        int nThreads;
        int threadIndex;
        std::vector<Particle> &particles;
        bool stop;
        ParticlePool &particlePool;
        QuadTree *tree;

};




class ParticleSimulation{
    public:
        ParticleSimulation(double x1, double x2, double y1, double y2, std::vector<Particle> &p);

        void Step(double dt);
        void BarnesHutSum(double dt);
        const std::vector<Particle> &GetParticles()const{return particles;}     

    private:
        bool ready;
        std::mutex lock;
        std::condition_variable cv;
        std::vector<Particle> particles;
        std::vector<Accelerations> accs;
        double x1,x2,y1,y2;
        std::vector<Worker*> workers;
        ParticlePool particlePool;
        // std::random_device rd;
        // std::mt19937 eng;//(rd());
        int nThreads;
};

#endif