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
    // nopath - haven't found a path or a purpose in life either.
    NOPATH,
          WANDER, // bimble using a negative stigmergic algorithm
          // following a path using JPS
          COARSEPATH,
          // getting to the absolute location within a square
          FINEPATH,
          ZOMBIE, // not really a PFM - but kill me.
          DEBUGSTOP
};

#define UPDATEDIRINTERVAL 0.2

struct Person {

    float x,y; // note these are in gridspace
    float dx,dy; // direction

    float destx,desty; // full destination for path
    int pathidx; // current index of path

    PathFindingMode pmode;
    double nextDirUpdate;

    Person(){
        pmode = WANDER;
    }

    void init(class Player *player, int idx, float xx,float yy);

    // used in wandering - sets the direction to where no-one has been
    void setDirectionToAntiStigmergy();

    float getrot(){
        extern float dirToRot[3][3];
        return dirToRot[sgn(-dx)+1][sgn(-dy)+1];
    }

    bool pathTo(float xx,float yy);

    void update(float t);

private:
    // we've reached our goal in the current mode
    void goalFound();
    void updateDirection();

    class Player *p;
    JPS::PathVector path;
};

#endif /* __PERSON_H */
