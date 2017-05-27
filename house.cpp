/**
 * @file house.cpp
 * @brief  Brief description of file.
 *
 */
#include "obj.h"
#include "game.h"
#include "globals.h"
#include "house.h"

void House::init(int xx,int yy,Player *pl){
    size = 1;
    pop =1;
    p = pl;
    x=xx;
    y=yy;
    
    // add to grid
    globals::game->grid.houses[x][y]=this;
}

House::~House(){
    // remove me from the game's grid
    globals::game->grid.houses[x][y]=NULL;
}

void House::update(float t){
    Grid *g = &globals::game->grid;
    
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
        -999
    };
    static const int lookup2[] = {
        -2,-2, -2,-1, -2,0, -2,1, -2,2,
        -1,-2,  -1,2,
        0,-2,  0,2,
        1,-2,  1,2,
        2,-2,  2,-1,  2,0,  2,1,  2,2,
        -999
    };
    
    int capacity;
    if(!g->isFlatAtAllOffsets(x,y,lookup0) || !g->get(x,y)){
       // this is a disaster; the bloody thing isn't flat at all - or it's in the sea
        capacity = 0;
        printf("Flat: %s, height %d\n",g->isFlatAtAllOffsets(x,y,lookup0)?"Y":"N",
               g->get(x,y));
               
    } else if(!g->isFlatAtAllOffsets(x,y,lookup1)){
        // flat at only offset 0
        capacity = 2;
    } else if(!g->isFlatAtAllOffsets(x,y,lookup2)){
        // flat at only 1
        capacity = 3;
    } else
        capacity = 5;
    
    // evict excess population (which may destroy the house)
    if(pop>capacity){
        evict(pop-capacity);
    }
}

void House::evict(int n){
    if(n>0){
        pop -= n;
        p->spawn(x,y,n);
    }
}
