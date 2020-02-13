/**
 * @file house.cpp
 * @brief  Brief description of file.
 *
 */
#include "obj.h"
#include "game.h"
#include "globals.h"
#include "meshes.h"
#include "house.h"
#include "font.h"
#include "logger.h"

#define POP_GROW_RATE 0.1

void House::init(int xx,int yy,Player *pl){
    // total player pop. unchanged since spawning player destroyed
    pop = 1;
    pendDamage=0;
    p = pl;
    x=xx;
    y=yy;
    size=255;
    growcounter=0;
    zombie=false;
    type = GO_HOUSE;
    globals::game->grid.addHouse(x,y,this);
}

House::~House(){
    // remove me from the game's grid
    globals::game->grid.removeHouse(this);
    evict(pop);
}

void House::queueRender(glm::mat4 *world){
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    
    ObjMesh *m;
    switch(size){
    default:
    case 0:m=meshes::house1;break;
    case 1:m=meshes::house2;break;
    case 2:m=meshes::house3;break;
    case 3:m=meshes::house4;break;
    case 4:m=meshes::house4;break;
    }
    
    m->queueRender(world);
    ms->push();
    ms->rotY(globals::timeNow);
    
    State *s = sm->push();
    s->overrides |= STO_DIFFUSE;
    s->diffuse = Colour(0.1,0.1,0.5);
    ms->push();
    ms->translate(0,pop+0.4,0);
    ms->scale(0.2);
    meshes::ico->queueRender(ms->top());
    ms->pop();
    
    
    s->diffuse = Colour(0.5,0.1,0.1);
    ms->push();
    ms->translate(0,size+0.6,0);
    ms->scale(0.2);
    meshes::ico->queueRender(ms->top());
    ms->pop();
    
    ms->pop();
    sm->pop();
    
}

void House::update(float t){
    Grid *g = &globals::game->grid;
    static const int capacities[]={
        0,
        1,2,4,5
    };
    
    // pending damage?
    if(pendDamage){
        if(pendDamage>pop)pendDamage=pop;
        pop -= pendDamage;
        p->decPop(pendDamage);
        pendDamage=0;
        globals::log->p(LOG_POP,"Decrement in house damage");
        if(pop<=0){
            zombie=true;
            return;
        }
    }
    
    size = (size==255)?
          g->countFlatGrass(x,y) : g->countFlat(x,y);
    
    if(size<0){
        globals::log->p(LOG_POP,"House destroyed due to size<0");
        zombie=true; // no room! 
        return;
    }
    
    int capacity = capacities[size+1];
    
    //    printf("cap %d, pop %d, gc %f\n",capacity,pop,growcounter);
    growcounter+=t*(float)capacity*POP_GROW_RATE;
    if(growcounter>1){
        growcounter=0;
        if(p->canIncPop()){
            pop++;
            p->incPop();
            globals::log->p(LOG_POP,"Increment in grow");
        }
    }
    
    // evict excess population, which may destroy the house
    // when it is updated.
    if(pop>capacity){
        evict(pop-capacity);
    }
}

void House::evict(int n){
    if(n>pop)n=pop;
    if(pop>=n){
        int spawned = p->spawn(x,y,n);
        globals::log->p(LOG_POP,"Decrement in evict");
        p->decPop(spawned); // do this first to make room
        pop -= spawned;
    }
}

void House::damage(int n){
    pendDamage+=n;
}
    
