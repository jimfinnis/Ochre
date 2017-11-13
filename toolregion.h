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

class ToolRegion : public Region {
    MapTex map;
    DebugMapTex debugmap;
public:
    ToolRegion();
    virtual ~ToolRegion(){};
    
    virtual void onMouseMove(int x,int y);
    virtual void onLeftClick(int x,int y);
    virtual void render();
    
    virtual void onButtonClick(int id);
};

#endif /* __TOOLREGION_H */
