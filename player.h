/**
 * @file player.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include "pool.h"
#include "person.h"

/// max number of people for each player
#define MAXPOP 256

/// a player in the game, computer or human. Contains information about
/// their people and the player themselves. House information will probably
/// be here too.


/// these are the modes available to the player
enum PlayerMode {
    // In settle mode, we look nearby for a flat piece of land. If we can't find
    // one we wander about - away from the enemy.
    PLAYER_SETTLE,
};


struct Player {
    Pool<Person> people;
    PlayerMode mode;
    Player();
    
    // assumes grid transform is stacked.
    void render();
    void update(float t);
};

#endif /* __PLAYER_H */
