/**
 * @file player.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include "pool.h"
#include "person.h"
#include "house.h"
#include "blur.h"
#include "globals.h"
#include "logger.h"

/// these are the modes available to the player
enum PlayerMode {
    // In settle mode, we look nearby for a flat piece of land. If we can't find
    // one we wander about - away from the enemy: we are repelled
    // by the opponent's potential field; but also away from ourselves.
    PLAYER_SETTLE,
          // In attack mode, we are attracted by the opponent's PF, but still
          // repelled by our own.
          PLAYER_ATTACK,
          // In this mode, we are attracted by a fixed anchor. If not
          // present, works like "settle".
          PLAYER_COLLECT,
          
};


/// a player in the game, computer or human. Contains information about
/// their people and the player themselves. House information will probably
/// be here too.

class Player {
    MultipassBlur *blur; // potential field blurrer - takes several ticks
    MultipassBlur *blurClose; // potential field blurrer - takes several ticks
    PlayerMode mode;
    static constexpr double AUTOLEVELDELAY = 1; // delay before autolevelling starts
    static constexpr double AUTOLEVELMININTERVAL = 0.1; // interval between autolevelling events
    static constexpr double AUTOLEVELMAXINTERVAL = 0.2; // interval between autolevelling events
    static constexpr int POPLIMIT = 10000;
    
    double nextAutolevelTime;
    int pop; // total population
    
public:
    int levelx,levely; // location near which we attempt to level
    int idx; // which player am I?
    Player *op; // opposing player
    Pool<Person> people;
    Pool<House> houses;
    
    // the "collect" anchor, -1 means not set.
    int anchorX,anchorY;
    
    // potential field produced by my people
    float potential[GRIDSIZE][GRIDSIZE];
    // potential field produced by my people, this one blurred less - 
    // it's for close range avoidance/attraction.
    float potentialClose[GRIDSIZE][GRIDSIZE];
    
    Player();
    virtual ~Player();
    
    void setMode(PlayerMode m);
    PlayerMode getMode(){
        return mode;
    }
    
    // remove the anchor completely
    void removeAnchor(){
        anchorX = -1;
        anchorY = -1;
        resetToWander();
    }
    
    void resetToWander(){
        // reset all peeps to wander; used when anchor or mode changed
        // also reset all peeps to wandering
        for(Person *p=people.first();p;p=people.next(p)){
            p->resetToWander();
        }
    }
    
    // assumes grid transform is stacked.
    void render(const Colour& col);
    // time passed in is interval since last update
    void update(double t);
    
    // performs automatic levelling
    void autoLevel();
    
    void updateHouseTerrain();
    
    // spawn some peeps within a given square (at slightly random offsets within it)
    // returns number actually spawned
    int spawn(int x,int y,int n);
    
    // set the anchor
    void setAnchor(int x,int y){
        if(x>=0 && x<GRIDSIZE && y>=0 && y<GRIDSIZE){
            anchorX = x;
            anchorY = y;
        }
        resetToWander();
    }
                                
    static void renderPerson(Person *p);
    
    int getPop(){
        return pop;
    }
    bool canIncPop(int n=1){
        return pop+n < POPLIMIT;
    }
    
    void incPop(int n=1){
        pop+=n;
        globals::log->p(LOG_POP,"Pop inc in player %d by %d to %d\n",idx,n,pop);
    }
    
    void decPop(int n=1){
        pop-=n;
        if(pop<0)pop=0;
        globals::log->p(LOG_POP,"Pop dec in player %d by %d to %d\n",idx,n,pop);
    }
};

#endif /* __PLAYER_H */
