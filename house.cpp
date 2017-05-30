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
    // add to grid will be done in first update
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
    
    int newsize = (size==255)?
          g->countFlatForBuild(x,y) : g->countFlat(x,y);
    
    // the house has changed size
    if(newsize != size && newsize>=0){
//        printf("Size calculated is %d, old size %d\n",newsize,size);
        // don't do this if we're going to destroy the house,
        // the player will destroy the house and that removes
        // it from the grid.
        globals::game->grid.removeHouse(x,y,this);
        size=newsize;
        globals::game->grid.addHouse(x,y,this);
    }
    
    int capacity = capacities[newsize+1];
    
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
