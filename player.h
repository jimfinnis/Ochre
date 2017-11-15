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
public:
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
    
    PlayerMode mode;
    Player();
    virtual ~Player();
    
    // target for wandering - peeps will gradually drift towards this point;
    // NOT a full-on pathing target but more of a bias for the stigmergic wandering.
    float wanderX,wanderY;
    
    // assumes grid transform is stacked.
    void render(const Colour& col);
    void update(float t);
    
    void updateHouseTerrain();
    
    // spawn some peeps within a given square (at slightly random offsets within it)
    void spawn(int x,int y,int n);
    
    // set the anchor
    void setAnchor(int x,int y){
        if(x>=0 && x<GRIDSIZE && y>=0 && y<GRIDSIZE){
            anchorX = x;
            anchorY = y;
        }
    }
                                
};

#endif /* __PLAYER_H */
