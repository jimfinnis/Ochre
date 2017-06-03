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
    globals::game->grid.removeHouse(x,y,this);
    evict(pop);
}


void House::queueRender(glm::mat4 *world){
    meshes::house1->queueRender(world);
}

void House::update(float t){
    Grid *g = &globals::game->grid;
    //    printf("House update at %d,%d\n",x,y);
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
    growcounter+=t*(float)(1+capacity-pop);
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
