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
    int tk;
public:
    DebugMapTex() : tk(0) {
        
    }
    void makePotential(Player *pl){
        float maxpot=0, maxpotcl=0;
        uint32_t *p = &image[0][0];
        for(int y=0;y<GRIDSIZE;y++){
            for(int x=0;x<GRIDSIZE;x++){
                float pot = pl->potential[x][y];
                float potcl = pl->potentialClose[x][y];
                float f = powf(pot+potcl*5.0f,0.5f);
                
                if(pot>maxpot)maxpot=pot;
                if(potcl>maxpotcl)maxpotcl=potcl;
                Colour c;
                c.set(f,f,f);
                *p++ = c.getABGR32();
                
            }
        }
        copy();
        /*
        if(tk++==10){
            tk=0;
            Game *g = globals::game;
            printf("Influence : %f / %f\n",g->p[0].influence, g->p[1].influence);
           }
         */
    }
};

#endif /* __DEBUGMAP_H */
