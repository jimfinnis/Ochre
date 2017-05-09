/**
 * @file game.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAME_H
#define __GAME_H

#include "grid.h"
#include "player.h"

/// a running game level, with data about both players and the world.

struct Game {
    Game(int seed,float waterlevel) : grid(seed,waterlevel){
        grid.cursorx=20;
        grid.cursory=20;
        grid.recentre();
    }
    
    Grid grid;
    
    Player p;
};

#endif /* __GAME_H */
