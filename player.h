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


/// these are the modes available to the player
enum PlayerMode {
    // In settle mode, we look nearby for a flat piece of land. If we can't find
    // one we wander about - away from the enemy.
    PLAYER_SETTLE,
};


/// a player in the game, computer or human. Contains information about
/// their people and the player themselves. House information will probably
/// be here too.

struct Player {
    Pool<Person> people;
    Pool<House> houses;
    
    PlayerMode mode;
    Player();
    
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
