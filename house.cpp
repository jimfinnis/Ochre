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

#define POP_GROW_RATE 0.1

void House::init(int xx,int yy,Player *pl){
    pop = 1;
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
    
    meshes::house1->queueRender(world);
    ms->push();
    ms->rotY(globals::timeNow);
    
    // NOTE: STO_DIFFUSE and other overrides have no effect in batched
    // rendering, which is a pain. So this doesn't work.
    
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
    
    size = (size==255)?
          g->countFlatGrass(x,y) : g->countFlat(x,y);
    
    if(size<0){
        zombie=true; // no room! 
    }
    
    int capacity = capacities[size+1];
    
//    printf("cap %d, pop %d, gc %f\n",capacity,pop,growcounter);
    growcounter+=t*(float)capacity*POP_GROW_RATE;
    if(growcounter>1){
        growcounter=0;
        pop++;
    }
    
    // evict excess population, which may destroy the house
    // when it is updated.
    if(pop>capacity){
        evict(pop-capacity);
    }
}

void House::evict(int n){
    if(pop>=n){
        pop -= n;
        p->spawn(x,y,n);
    }
}

void House::damage(int n){
    pop -= n;
    if(pop<=0)
        globals::game->grid.removeHouse(this);
        
}
