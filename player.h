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



struct Player {
    Pool<Person> people;
    
    Player();
    
    // assumes grid transform is stacked.
    void render();
};

#endif /* __PLAYER_H */
