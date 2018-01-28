#include "QuadTree.h"
#include "ParticleSimulation.h"

#include "math.h"
#include <iostream>
#include <list>
#include <map>

ParticleSimulation::ParticleSimulation(double x1, 
                                    double x2, 
                                    double y1, 
                                    double y2, 
                                    std::vector<Particle> & p):
                                    ready(false),
                                    particles(p),
                                    x1(x1),
                                    x2(x2), 
                                    y1(y1),
                                    y2(y2)
                                    // eng(rd) 
                                    {   
                                        // std::random_device rd; // obtain a random number from hardware
                                         // seed the generator
                                        // eng = std::mt19937(rd());
                                        nThreads = 1;
                                        for(int i = 0; i<nThreads; i++){
                                            workers.push_back(new Worker(nThreads,
                                                                            i,  
                                                                            particles,
                                                                            cv,
                                                                            lock,
                                                                            ready,
                                                                            particlePool));
                                            workers[i]->Start();
                                            // workers[i]->holdMutex.lock();
                                        }
                                    }
                                                                        
double const forceField(const double r){

    return -1.0/(r*r*r+1e-3);//+1.0/(r*r*r*r+1e-5);
}

struct Position
{
    double x;
    double y;
    double xacc;
    double yacc;
    double mass;
};

void computeAcc1(std::vector<Particle> *p, int start, int step){

    auto &particles = *p;
    const int n = particles.size()/double(step)*(1+start)+0.5;
    int i=0;
    // std::cout<<"HERE "<<n<<"  "<<step<<"  "<<particles.size()<<"  "<<start<<std::endl;
    for(int count=0, i = particles.size()/step*start; i<n; i++,count++){
        auto const p1c = particles[i];
        double localAcc[2] = {0,0};
        for(int j = 0, n = particles.size();j<n;j++){
            // if(i==j){
                // continue;
            // }
            
            const auto &p2 =particles[j];

            double dx = p1c.pos[0]-p2.pos[0];
            double dy = p1c.pos[1]-p2.pos[1];//p2.pos[1];
            double r = sqrt(dx*dx+dy*dy);
            if(r<1e-4)
                continue;

            double norm = p2.mass;
            double acc1 = norm*forceField(r);            
            localAcc[0] += acc1*dx;
            localAcc[1] += acc1*dy;

        }
        // std::cout<<localAcc[0]<<std::endl;
        auto &p1 = particles[i];
        p1.oldacc[0] = p1c.acc[0];
        p1.oldacc[1] = p1c.acc[1];
        p1.acc[0]=localAcc[0];
        p1.acc[1]=localAcc[1]; 

    }

}

void computeAcc3(std::vector<Particle> *p, int start, int step){

    auto &particles = *p;
    const int n = particles.size();// /double(step)*(1+start)+0.5;

    auto const p1c = particles[start];
    double localAcc[2] = {0,0};
    for(int j = 0;j<n;j++){
        if(start==j)
        
            continue;
        const auto &p2 =particles[j];

        double dx = p1c.pos[0]-p2.pos[0];
        double dy = p1c.pos[1]-p2.pos[1];//p2.pos[1];
        double r = sqrt(dx*dx+dy*dy);
        double norm = p2.mass;
        double acc1 = norm*forceField(r);            
        localAcc[0] += acc1*dx;
        localAcc[1] += acc1*dy;

    }
    auto &p1 = particles[start];
    p1.oldacc[0] = p1c.acc[0];
    p1.oldacc[1] = p1c.acc[1];
    p1.acc[0]=localAcc[0];
    p1.acc[1]=localAcc[1]; 

}

void SumBarnesHut(const QuadTree::Node &node, Particle &p, int depth){
    if(node.type == QuadTree::Node::EmptyLeaf){
        return;
    }
    double thetaEps = 0.7;
    Particle CoMp = node.GetCenterOfMassParticle();
    double dx = p.pos[0] - CoMp.pos[0];
    double dy = p.pos[1] - CoMp.pos[1];
    double distance = sqrt(dx*dx + dy*dy);
    double theta = node.size/distance; 
    // std::cout<<depth<<" "<<p.id<<" "<<CoMp.pos[0]<<" "<<theta<<"  "<<p.pos[0]<<std::endl;
    if(isnan(theta)){
        // std::cout<<depth<<" "<<p.id<<" "<<CoMp.pos[0]<<std::endl;
        return;
    }

    if(theta<thetaEps){
        double norm = CoMp.mass;
        // if(distance<1e-4){
            double acc = norm*forceField(distance);
            p.acc[0] += acc*dx;
            p.acc[1] += acc*dy;
        // }
    }
    else{
        if(node.type == QuadTree::Node::node){
            for(const auto &n: node.nodes){
                if(!n){
                    // std::cout<<depth<<" "<<n<<std::endl;
                }
                SumBarnesHut(*n,p,depth+1);
            }
        }
        else{
            // std::cout<<node.particles.size()<<std::endl;
            for(const auto & tp: node.particles){
                // std::cout<<&tp<<std::endl;
                double dx = p.pos[0] - tp->pos[0];
                double dy = p.pos[1] - tp->pos[1];
                double r = sqrt(dx*dx + dy*dy);
                if(r<1e-5)
                    continue;
                double norm = tp->mass;
                double acc = norm*forceField(r);
                p.acc[0] += acc*dx;
                p.acc[1] += acc*dy;
            }
        }
    }
}

void ParticleSimulation::BarnesHutSum(double dt){

    QuadTree tree;
    tree.BuildTree(particles);
    std::map<Particle*,QuadTree::Node*> particleMap;
     for(int i = 0,n = particles.size();i<n;i++){
        particlePool.indexQueue.push(i);
    }
    
    for(auto &w: workers)
        w->SetTree(&tree);

    {
        std::unique_lock<std::mutex> l(lock);
        for(auto &w: workers)
            w->ready = true;
    }

    cv.notify_all();

    for(auto &w: workers){
        std::unique_lock<std::mutex> l(w->llock);
        w->cv.wait(l, [w]{return w->finished;});
        w->finished=false;
    }

    // for(auto &particle : particles){
    //     particle.oldacc[0] = particle.acc[0];
    //     particle.oldacc[1] = particle.acc[1];
    //     particle.acc[0] = 0;
    //     particle.acc[1] = 0;
    //     SumBarnesHut(tree.GetRoot(), particle);
    // }     

    for(auto &p: particles){
        // std::cout<<p.acc[0]<<"  "<<p.acc[1]<<std::endl;
        p.acc[1] -=0;
        p.pos[0] += p.vel[0]*dt + 0.5*p.acc[0]*dt*dt;
        p.pos[1] += p.vel[1]*dt + 0.5*p.acc[1]*dt*dt;
        p.vel[0] += 0.5*(p.oldacc[0]+p.acc[0])*dt;
        p.vel[1] += 0.5*(p.oldacc[1]+p.acc[1])*dt;
        
        // if(p.pos[0]<x1  || p.pos[0]>x2){
        //     p.vel[0] *=-1;

        // }
        // if(p.pos[1]<y1  || p.pos[1]>y2){
        //     p.vel[1] *=-1;   
        // }
    }
}

void computeAccBarnesHut(QuadTree &tree, Particle &particle){

    particle.oldacc[0] = particle.acc[0];
    particle.oldacc[1] = particle.acc[1];
    particle.acc[0] = 0;
    particle.acc[1] = 0;
    SumBarnesHut(tree.GetRoot(), particle,0);

}


void ParticleSimulation::Step(double dt){
    std::vector<std::thread> threads;
    int nthreads = 4;
    for(int i = 0; i<nthreads-1;i++){
        threads.push_back(std::thread(computeAcc1,&particles,i,nthreads));
        // std::cout<<"Thread "<<i<<" started"<<std::endl;
    }
    // std::cout<<"Thread "<<nthreads-1<<" started"<<std::endl;    
    computeAcc1(&particles, nthreads-1, nthreads);

    for(auto &t:threads)
        t.join();

    for(auto &p: particles){
        // std::cout<<p.acc[0]<<"  "<<p.acc[1]<<std::endl;
        p.acc[1] -=0;
        p.pos[0] += p.vel[0]*dt + 0.5*p.acc[0]*dt*dt;
        p.pos[1] += p.vel[1]*dt + 0.5*p.acc[1]*dt*dt;
        p.vel[0] += 0.5*(p.oldacc[0]+p.acc[0])*dt;
        p.vel[1] += 0.5*(p.oldacc[1]+p.acc[1])*dt;
        
        if(p.pos[0]<x1  || p.pos[0]>x2){
            p.vel[0] *=-1;

        }
        if(p.pos[1]<y1  || p.pos[1]>y2){
            p.vel[1] *=-1;   
        }
    }
}

void ParticleSimulation::Step1(double dt){

    for(int i = 0,n = particles.size();i<n;i++){
        particlePool.indexQueue.push(i);
    }


    {
        std::unique_lock<std::mutex> l(lock);
        for(auto &w: workers)
            w->ready = true;
    }

    cv.notify_all();

    for(auto &w: workers){
        std::unique_lock<std::mutex> l(w->llock);
        w->cv.wait(l, [w]{return w->finished;});
        w->finished=false;
    }

    for(auto &p: particles){
        // std::cout<<p.acc[0]<<"  "<<p.acc[1]<<std::endl;
        p.acc[1] -=0;
        p.pos[0] += p.vel[0]*dt + 0.5*p.acc[0]*dt*dt;
        p.pos[1] += p.vel[1]*dt + 0.5*p.acc[1]*dt*dt;
        p.vel[0] += 0.5*(p.oldacc[0]+p.acc[0])*dt;
        p.vel[1] += 0.5*(p.oldacc[1]+p.acc[1])*dt;
        
        if(p.pos[0]<x1  || p.pos[0]>x2){
            p.vel[0] *=-1;

        }
        if(p.pos[1]<y1  || p.pos[1]>y2){
            p.vel[1] *=-1;   
        }
    }
}



void ParticleSimulation::Step2(double dt){
    QuadTree tree;
    tree.BuildTree(particles);
      

    for(auto &p: particles){
        // std::cout<<p.acc[0]<<"  "<<p.acc[1]<<std::endl;
        p.acc[1] -=0;
        p.pos[0] += p.vel[0]*dt + 0.5*p.acc[0]*dt*dt;
        p.pos[1] += p.vel[1]*dt + 0.5*p.acc[1]*dt*dt;
        p.vel[0] += 0.5*(p.oldacc[0]+p.acc[0])*dt;
        p.vel[1] += 0.5*(p.oldacc[1]+p.acc[1])*dt;
        
        if(p.pos[0]<x1  || p.pos[0]>x2){
            p.vel[0] *=-1;

        }
        if(p.pos[1]<y1  || p.pos[1]>y2){
            p.vel[1] *=-1;   
        }
    }
}


void threadWrapper(Worker *w){
    w->Run();
}

void threadWrapper2(Worker *w){
    w->Run2();
}
Worker::Worker(int nThreads, 
                int threadIndex, 
                std::vector<Particle> &p, 
                std::condition_variable &c, 
                std::mutex &l, 
                bool &r,
                ParticlePool &pPool):
                                    glock(l),
                                    gcv(c),
                                    // ready(r),
                                    finished(false),
                                    nThreads(nThreads),
                                    threadIndex(threadIndex),
                                    particles(p),
                                    stop(false),
                                    particlePool(pPool){
    
}

void Worker::Run(){
    while(!stop){

        std::unique_lock<std::mutex> gl(glock);
        gcv.wait(gl, [this]{return this->ready;});
        ready =false;
        std::unique_lock<std::mutex> ll(llock);
        gl.unlock();
        {
            std::unique_lock<std::mutex> ql(particlePool.lock);
            bool queue_full = !particlePool.indexQueue.empty();
            
            while(queue_full){
                std::queue<int> indexQueue;
                int i=0;
                while(i<4 & !particlePool.indexQueue.empty()){
                    indexQueue.push(particlePool.indexQueue.front());
                    particlePool.indexQueue.pop();
                    i++;
                }

                // int index1 = particlePool.indexQueue.front();
                // particlePool.indexQueue.pop();
                ql.unlock();
                for(int j =0; j<i; j++){
                    computeAcc3(&particles, indexQueue.front(), nThreads);
                    indexQueue.pop();
                }
                ql.lock();
                queue_full = !particlePool.indexQueue.empty();
            }
        }   

        // computeAcc1(&particles, threadIndex, nThreads);
        finished=true;
        ll.unlock();
        cv.notify_one();
    }
}

void Worker::Run2(){
    while(!stop){
        // std::cout<<"Thread "<<threadIndex <<" running"<<std::endl;
        std::unique_lock<std::mutex> gl(glock);
        gcv.wait(gl, [this]{return this->ready;});
        ready =false;
        std::unique_lock<std::mutex> ll(llock);
        gl.unlock();
        {
            std::unique_lock<std::mutex> ql(particlePool.lock);
            bool queue_full = !particlePool.indexQueue.empty();
            
            while(queue_full){
                std::queue<int> indexQueue;
                int i=0;
                while(i<250 & !particlePool.indexQueue.empty()){

                    indexQueue.push(particlePool.indexQueue.front());
                    particlePool.indexQueue.pop();
                    i++;
                }

                // int index1 = particlePool.indexQueue.front();
                // particlePool.indexQueue.pop();
                ql.unlock();
                for(int j =0; j<i; j++){
                    // std::cout<<"Thread "<<threadIndex<<" processing "<<j<<std::endl;
                    computeAccBarnesHut(*tree, particles[indexQueue.front()]);
                    indexQueue.pop();
                }
                ql.lock();
                queue_full = !particlePool.indexQueue.empty();
            }
        }   

        // computeAcc1(&particles, threadIndex, nThreads);
        finished=true;
        ll.unlock();
        cv.notify_one();
    }
}