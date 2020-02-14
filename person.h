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
          
          // looking for a particular enemy bod!
          HUNT,
          
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
    Person *next; // used to link the "people" lists in the grid
    float x,y; // note these are in gridspace
    float dx,dy; // direction
    float drowntime; // "and how long have you been in the sea, Sir?"
    float destx,desty; // full destination for path
    float speed; // my personal speed
    int pathidx; // current index of path
    // how many actual "people" this is - they merge and sometimes split!
    int strength; 
    // pending damage
    int pendDamage;
    float walkCycle; // where we are in the walk cycle. Just keeps increasing.
    float smoothRot;
    int debug;
    
    PersonState state;
    double nextInfrequentUpdate;
    
    Person *target; // if HUNT, who we're after stabbing

    Person(){
        state = WANDER;
    }

    void init(class Player *player, int idx, float xx,float yy);
    
    const char *getName(){return name;}

    // used in wandering - sets the direction to where no-one has been
    // and also to either attack or move away from the other player.
    // Will also take into account the anchor.
    void setDirectionFromPotentialField();
    
    float getrot(){
        extern float dirToRot[3][3];
        return dirToRot[sgn(-dx)+1][sgn(-dy)+1];
    }
    
    float getSmoothedRot(){
        return getrot();
        float r = getrot();
        smoothRot = 0.9f*smoothRot+0.1f*r;
        return smoothRot;
    }
    
    /// find a possible enemy nearby. Within sight. It's a bit
    /// expensive on a dense grid. Careful now.
    Person *locateEnemy();
    //// less expensive, locates enemy houses
    House *locateEnemyHouse();

    bool pathTo(float xx,float yy);
    
    /// return true if we're not following a path
    bool hasNoValidPath(){
        return state!=COARSEPATH && state!=FINEPATH;
    }
    
    /// this gets called when the player mode gets changed
    void resetToWander(){
        state = WANDER;
    }
    
    /// get the chance of winning a fight against another
    /// person or house
    
    double getChanceOfWinningAttack(Person *defender);
    double getChanceOfWinningAttack(House *defender);

    void update(float t);
    
    JPS::PathVector path;
private:
    char name[32]; // just for fun
    
    /// an update which doesn't happen very often, but at least once per traversed
    /// map square.
    void updateInfrequent();
    
    /// damage me by n points of strength; zombie me if <=0.
    /// Damage is added to pending damage and applied in the next update by
    /// the person itself.
    void damage(int n);

    class Player *p;
};

#endif /* __PERSON_H */
