/**
 * @file gamescreen.cpp
 * @brief  Brief description of file.
 *
 */

#include "gamescreen.h"
#include "globals.h"
#include "grid.h"

GameScreen::GameScreen() {
    globals::grid = new Grid();
    hx=20;
    hy=20;
}

GameScreen::~GameScreen(){
}

#define TOOLWIDTH 200
#define STATUSHEIGHT 80


void GameScreen::resize(int w,int h){
    stat.resize(0,h-STATUSHEIGHT,w,STATUSHEIGHT);
    tool.resize(w-TOOLWIDTH,0,TOOLWIDTH,h);
    game.resize(0,0,w-TOOLWIDTH,h-STATUSHEIGHT);
}    

void GameScreen::render(){
    stat.render();
    tool.render();
    game.render();
    
}

void GameScreen::onKeyDown(int k){
    switch(k){
    case 't':
        delete globals::grid;
        globals::grid = new Grid();
        break;
    case SDLK_RIGHT:
        globals::cursorx++;break;
    case SDLK_LEFT:
        globals::cursorx--;break;
        // these are backwards because right-handed coord system: +ve z is into the screen
    case SDLK_UP:
        globals::cursory--;break;
    case SDLK_DOWN:
        globals::cursory++;break;
    default:
        printf("%d\n",k);
    }
}
    

