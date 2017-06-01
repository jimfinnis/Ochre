/**
 * @file debugmap.h
 * @brief  Brief description of file.
 *
 */

#ifndef __DEBUGMAP_H
#define __DEBUGMAP_H

#include "debugmap.h"

/// a map, but for debugging. Make of it what you will. Uses alternate methods rather
/// than subclassing to avoid proliferation of "friend" clauses in Grid.

class DebugMapTex : public MapTex {
public:
    void makeStigmergy(Player *pl){
        Grid *g = &globals::game->grid;
        uint32_t *p = &image[0][0];
        for(int y=0;y<GRIDSIZE;y++){
            for(int x=0;x<GRIDSIZE;x++){
                Colour c;
                c.setFromHSV(g->grid[x][y]?0.5:0,0.5,
                             pl->mapsteps[x][y]*0.1f+0.3f);
                *p++ = c.getABGR32();
            }
        }
        copy();
    }
    
    void makePotential(Player *pl){
        uint32_t *p = &image[0][0];
        for(int y=0;y<GRIDSIZE;y++){
            for(int x=0;x<GRIDSIZE;x++){
                float f = pl->potential[x][y];
                Colour c;
                c.set(f,f,f);
                *p++ = c.getABGR32();
            }
        }
        copy();
    }
};

#endif /* __DEBUGMAP_H */