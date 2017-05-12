/**
 * @file gameregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAMEREGION_H
#define __GAMEREGION_H

#include "maths.h"
#include "region.h"

class GameRegion : public IsoRegion {
    void renderWater();
    static const int visibleGridSize=6;
    glm::mat4 view;
public:
    GameRegion();
    
    virtual void onMouseMove(int x,int y);
    virtual void onLeftClick(int x,int y);
    virtual void onRightClick(int x,int y);
    virtual void render();
    
    float rotAngle;
};

#endif /* __GAMEREGION_H */
