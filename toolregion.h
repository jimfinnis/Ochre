/**
 * @file toolregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __TOOLREGION_H
#define __TOOLREGION_H

#include "region.h"
#include "maptex.h"
#include "debugmap.h"


#define BUTSET_MODES 0
#define BUTSET_ACTIONS 1

class ToolRegion : public Region {
    friend class GameScreen;
    MapTex map;
    DebugMapTex debugmap;
    class GameScreen *screen;
public:
    
    ToolRegion();
    virtual ~ToolRegion(){};
    
    virtual void onMouseMove(int x,int y);
    virtual void onLeftClick(int x,int y);
    virtual void render();
    
    virtual void onButtonClick(int id);
};

#endif /* __TOOLREGION_H */
