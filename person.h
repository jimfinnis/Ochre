/**
 * @file person.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PERSON_H
#define __PERSON_H

#include "grid.h"
#include "JPS.h"

/// one of the little people in the game - allocated from a pool,
/// ctor and dtor will run.

enum PathFindingMode {
    // nopath - haven't found a path
    NOPATH,
          // following a path using JPS
          COARSEPATH,
          // getting to the absolute location within a square
          FINEPATH,
          ZOMBIE, // not really a PFM - but kill me.
          DEBUGSTOP
};
          

struct Person {
    
    float x,y; // note these are in gridspace
    float dx,dy; // direction
    
    float destx,desty; // full destination for path
    int pathidx; // current index of path
    
    PathFindingMode pmode;
    
    Person(){
        pmode = NOPATH;
    }
    
    void init(class Player *player, float xx,float yy){
        x=xx;y=yy;
        dx=dy=0;
        p=player;
        
    }
    
    float getrot(){
        return dirToRot[sgn(dx)+1][sgn(dy)+1];
    }
    
    bool pathTo(float xx,float yy);
    
    void update(float t);
    
    static void initConsts();
    
private:
    // we've reached our goal in the current mode
    void goalFound();
    
    static float dirToRot[3][3];
    class Player *p;
    JPS::PathVector path;
};

#endif /* __PERSON_H */
