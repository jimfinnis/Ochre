/**
 * @file game.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAME_H
#define __GAME_H

#include "grid.h"
#include "player.h"
#include "prof.h"

/// a running game level, with data about both players and the world.

struct Game {
    Game(int seed,float waterlevel) : grid(seed,waterlevel){
        grid.cursorx=20;
        grid.cursory=20;
        grid.recentre();
        p[0].op = &p[1];
        p[1].op = &p[0];
    }

    void update(float t){
        profbar.start("P",0xff0000ff);
        p[0].update(t);
        p[1].update(t);
        profbar.end();
        
        profbar.start("G",0x00ff00ff);
        grid.update(t);
        profbar.end();
    }

    Grid grid;

    Player p[2];
};

#endif /* __GAME_H */
