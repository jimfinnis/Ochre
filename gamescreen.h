/**
 * @file gamescreen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GAMESCREEN_H
#define __GAMESCREEN_H

#include "screen.h"
#include "gameregion.h"
#include "statregion.h"
#include "toolregion.h"

/// The main game screen

class GameScreen : public Screen {
    GameRegion game;
    ToolRegion tool;
    StatRegion stat;
public:
    GameScreen();
    virtual ~GameScreen();
    
    virtual void resize(int w,int h);
    virtual void render();
};

#endif /* __GAMESCREEN_H */
