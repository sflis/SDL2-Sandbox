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
                                    {   
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
                                        }
                                    }
                                                                        
double const forceField(const double r){

    return -1.0/(r * r * r+1e-1)+1.0/(r*r*r*r+1e-2);
}


void SumBarnesHut(const QuadTree::Node &node, Particle &p, int depth){
    // #if(deph)
    // std::cout<<"Depth: "<<depth<<std::endl;
    if(node.type == QuadTree::Node::EmptyLeaf){
        return;
    }
    double thetaEps = 0.7;
    Particle CoMp = node.GetCenterOfMassParticle();
    double dx = p.pos[0] - CoMp.pos[0];
    double dy = p.pos[1] - CoMp.pos[1];
    double distance = sqrt(dx * dx + dy * dy);
    double theta = node.size/distance; 
    // std::cout<<depth<<" "<<p.id<<" "<<CoMp.pos[0]<<" "<<theta<<"  "<<p.pos[0]<<std::endl;
    if(isnan(theta)){
        // std::cout<<depth<<" "<<p.id<<" "<<CoMp.pos[0]<<std::endl;
        return;
    }

    if(theta<thetaEps){
        double norm = CoMp.mass;
        // if(distance<1e-4){
            double acc = norm * forceField(distance);
            p.acc[0] += acc * dx;
            p.acc[1] += acc * dy;
        // }
    }
    else{
        switch(node.type){
            case QuadTree::Node::node:
                for(const auto &n: node.nodes)
                    SumBarnesHut(*n,p,depth+1);
                // std::cout<<"++++++++++++++ "<<&node.activeNodes<<"  "<<node.particles.size()<<"  "<<depth<<"  "<<node.depth<<std::endl;
                for(const auto &i: node.activeNodes){
                    if(i>4){
                        // std::cout<<i<<"   "<<&i<<"      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
                        std::cout<<node.type<<" "<<&node.activeNodes<<"  "<<node.particles.size()<<"  "<<node.depth<<std::endl;
                    }
                }
                // std::cout<<std::endl;
                // for(const auto i: node.activeNodes){
                //     std::cout<<depth+1<<"  "<<i<<"  "<<node.type<<std::endl;
                //     SumBarnesHut(*node.nodes[i],p,depth+1);
                // }
                break;
            default:
                for(const auto & tp: node.particles){
                    // std::cout<<&tp<<std::endl;
                    double dx = p.pos[0] - tp->pos[0];
                    double dy = p.pos[1] - tp->pos[1];
                    double r = sqrt(dx*dx + dy*dy);
                    if(r<1e-2)
                        continue;
                    double norm = tp->mass;
                    double acc = norm * forceField(r);
                    p.acc[0] += acc * dx;
                    p.acc[1] += acc * dy;
            }    
        }
    }
}

void ParticleSimulation::BarnesHutSum(double dt){

    QuadTree tree;
    tree.BuildTree(particles);
    // std::map<Particle*,QuadTree::Node*> particleMap;
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


    for(auto &p: particles){
        p.pos[0] += p.vel[0] * dt + 0.5 * p.acc[0] * dt * dt;
        p.pos[1] += p.vel[1] * dt + 0.5 * p.acc[1] * dt * dt;
        p.vel[0] += 0.5 * (p.oldacc[0]+p.acc[0]) * dt;
        p.vel[1] += 0.5 * (p.oldacc[1]+p.acc[1]) * dt;
        
    }
}

void computeAccBarnesHut(QuadTree &tree, Particle &particle){

    particle.oldacc[0] = particle.acc[0];
    particle.oldacc[1] = particle.acc[1];
    particle.acc[0] = 0;
    particle.acc[1] = 0;
    SumBarnesHut(tree.GetRoot(), particle,0);

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
        double r = sqrt( dx * dx + dy * dy);
        double norm = p2.mass;
        double acc1 = norm * forceField(r);            
        localAcc[0] += acc1 * dx;
        localAcc[1] += acc1 * dy;

    }
    auto &p1 = particles[start];
    p1.oldacc[0] = p1c.acc[0];
    p1.oldacc[1] = p1c.acc[1];
    p1.acc[0]=localAcc[0];
    p1.acc[1]=localAcc[1]; 

}

void ParticleSimulation::Step(double dt){
    std::vector<std::thread> threads;
    int nthreads = 4;
    for(int i = 0; i<nthreads-1;i++){
        threads.push_back(std::thread(computeAcc3,&particles,i,nthreads));
        // std::cout<<"Thread "<<i<<" started"<<std::endl;
    }
    // std::cout<<"Thread "<<nthreads-1<<" started"<<std::endl;    
    computeAcc3(&particles, nthreads-1, nthreads);

    for(auto &t:threads)
        t.join();

    for(auto &p: particles){
        // std::cout<<p.acc[0]<<"  "<<p.acc[1]<<std::endl;
        p.acc[1] -=0;
        p.pos[0] += p.vel[0] * dt + 0.5 * p.acc[0] * dt * dt;
        p.pos[1] += p.vel[1] * dt + 0.5 * p.acc[1] * dt * dt;
        p.vel[0] += 0.5 * (p.oldacc[0]+p.acc[0]) * dt;
        p.vel[1] += 0.5 * (p.oldacc[1]+p.acc[1]) * dt;
        
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
                    // std::cout<<"Thread "<<threadIndex<<" processing "<<j<<"   "<<indexQueue.front()<<std::endl;

                       computeAccBarnesHut( *tree, particles[indexQueue.front()]);
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