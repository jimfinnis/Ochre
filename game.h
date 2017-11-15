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
        p[0].setMode(PLAYER_ATTACK);
        p[1].setMode(PLAYER_ATTACK);
    }

    void update(float t){
        // update grid first, to populate the "people" fields.
        profbar.start("G",0x00ff00ff);
        grid.update(t);
        profbar.end();
        
    
        // do this before updating the player so all the data is there
        
        profbar.start("HT",0x808080ff);
        p[0].updateHouseTerrain();
        p[1].updateHouseTerrain();
        grid.removeNonFlatFarm();
        profbar.end();
        
        profbar.start("P",0xff0000ff);
        p[0].update(t);
        p[1].update(t);
        profbar.end();
        
    }

    Grid grid;

    Player p[2];
};

#endif /* __GAME_H */
