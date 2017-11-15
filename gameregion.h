/**
 * @file gameregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAMEREGION_H
#define __GAMEREGION_H

#include "maths.h"
#include "region.h"

enum GameRegionMode {
    GRM_TERRAIN, // clicking modifies terrain (default)
          GRM_SET_ANCHOR // clicking sets anchor
};

class GameRegion : public IsoRegion {
    friend class GameScreen;
    
    void renderWater();
    static const int visibleGridSize=8;
    glm::mat4 view;
    GameRegionMode mode;
    class GameScreen *screen;
public:
    GameRegion();
    
    void setMode(GameRegionMode m){
        mode  = m;
    }
                        
    virtual void onMouseMove(int x,int y);
    virtual void onLeftClick(int x,int y);
    virtual void onRightClick(int x,int y);
    virtual void render();
    
    float rotAngle;
};

#endif /* __GAMEREGION_H */
