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

struct Person {
    
    float x,y; // note these are in gridspace
    int dx,dy; // direction
    
    bool hasPath;
    int destx,desty; // full destination for path
    int pathidx; // current index of path
    
    Person(){
        hasPath = false;
    }
    
    void init(float xx,float yy,int ddx,int ddy){
        x=xx;y=yy;
        dx=ddx;dy=ddy;
    }
    
    float getrot(){
        return dirToRot[dx+1][dy+1];
    }
    
    void update(float t);
    
    static void initConsts();
    
private:
    static float dirToRot[3][3];
    
    JPS::PathVector path;
};

#endif /* __PERSON_H */
