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
    // one we wander about - away from the enemy.
    PLAYER_SETTLE,
};


/// a player in the game, computer or human. Contains information about
/// their people and the player themselves. House information will probably
/// be here too.

class Player {
    MultipassBlur *blur; // potential field blurrer - takes several ticks
public:
    Pool<Person> people;
    Pool<House> houses;
    
    // potential field produced by my people
    float potential[GRIDSIZE][GRIDSIZE];
    // stigmergic trace - incremented when a peep enters square (x,y,x+1,y+1) and decays
    // over time. Idea pinched from Populous, so I've kept the name of the variable.
    float mapsteps[GRIDSIZE][GRIDSIZE];
    
    
    
    PlayerMode mode;
    Player();
    virtual ~Player();
    
    // target for wandering - peeps will gradually drift towards this point;
    // NOT a full-on pathing target but more of a bias for the stigmergic wandering.
    float wanderX,wanderY;
    
    // assumes grid transform is stacked.
    void render();
    void update(float t);
    
    // spawn some peeps within a given square (at slightly random offsets within it)
    void spawn(int x,int y,int n);
};

#endif /* __PLAYER_H */
