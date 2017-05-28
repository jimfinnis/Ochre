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

void House::init(int xx,int yy,Player *pl){
    pop = 1;
    p = pl;
    x=xx;
    y=yy;
    size=255;
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
    // work out how many squares around me are still flat. We do this
    // with lookup tables for positions, working out from the middle.
    // Tables are terminated with -999.
    
    // worst case - the centre isn't flat!
    static const int lookup0[] = {
        0,0,-999
    };
    static const int lookup1[] = {
        -1,-1, 0,-1, 1,-1,
        -1,0,        1,0,
        -1,1,  0,1,  1,1,
        -999};
    static const int lookup2[] = {
        -2,-2, -2,-1, -2,0, -2,1, -2,2,
        -1,-2,  -1,2,
        0,-2,  0,2,
        1,-2,  1,2,
        2,-2,  2,-1,  2,0,  2,1,  2,2,
        -999};
    static const int lookup3[] = {
        -3,-3,-3,-2,-3,-1,-3,0,-3,1,-3,2,-3,3,
        -2,-3,-2,3,
        -1,-3,-1,3,
        0,-3,0,3,
        1,-3,1,3,
        2,-3,2,3,
        3,-3,3,-2,3,-1,3,0,3,1,3,2,3,3,
        -999};
    static const int capacities[]={
        0,
        1,2,4,5
    };
    
    int newsize;
    if(size==255){
        if(!g->isFlatAtAllOffsetsForBuild(x,y,lookup0) || !g->get(x,y)){
            // this is a disaster; the bloody thing isn't flat at all - or it's in the sea
            newsize = -1;
        } else if(!g->isFlatAtAllOffsetsForBuild(x,y,lookup1)){
            // flat at only offset 0
            newsize = 0;
        } else if(!g->isFlatAtAllOffsetsForBuild(x,y,lookup2)){
            // flat at only 1
            newsize = 1;
        } else if(!g->isFlatAtAllOffsetsForBuild(x,y,lookup3)){
            newsize = 2;
        } else
            newsize = 3;
    } else {
        if(!g->isFlatAtAllOffsets(x,y,lookup0) || !g->get(x,y)){
            // this is a disaster; the bloody thing isn't flat at all - or it's in the sea
            newsize = -1;
        } else if(!g->isFlatAtAllOffsets(x,y,lookup1)){
            // flat at only offset 0
            newsize = 0;
        } else if(!g->isFlatAtAllOffsets(x,y,lookup2)){
            // flat at only 1
            newsize = 1;
        } else if(!g->isFlatAtAllOffsets(x,y,lookup3)){
            newsize = 2;
        } else
            newsize = 3;
    }
    
    
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
