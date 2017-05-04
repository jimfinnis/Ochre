/**
 * @file gameregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAMEREGION_H
#define __GAMEREGION_H

#include "region.h"

class GameRegion : public IsoRegion {
public:
    GameRegion() : IsoRegion("game") {
    }
    
    virtual void onMouseMove(int x,int y);
    virtual void render();
};

#endif /* __GAMEREGION_H */
