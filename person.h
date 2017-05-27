/**
 * @file person.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PERSON_H
#define __PERSON_H

#include "grid.h"
#include "JPS.h"

/// State, mainly to do with how I'm moving.

enum PersonState {
    WANDER, // bimble using a negative stigmergic algorithm
          // following a path using JPS
          COARSEPATH,
          
          // having completed COARSEPATH,
          // getting to the absolute location within a square
          FINEPATH,
          
          // not a path finding mode as such, more a state
          ZOMBIE,
          DEBUGSTOP
};

// how many seconds can we survive the cold, cold sea?
#define DROWNSURVIVALTIME 2.0f

#define INFREQUENTUPDATEINTERVAL 0.2

/// one of the little people in the game - allocated from a pool,
/// ctor and dtor will run.

struct Person {

    float x,y; // note these are in gridspace
    float dx,dy; // direction
    float drowntime; // "and how long have you been in the sea, Sir?"
    float destx,desty; // full destination for path
    int pathidx; // current index of path

    PersonState state;
    double nextInfrequentUpdate;

    Person(){
        state = WANDER;
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
    /// an update which doesn't happen very often, but at least once per traversed
    /// map square.
    void updateInfrequent();

    class Player *p;
    JPS::PathVector path;
};

#endif /* __PERSON_H */
