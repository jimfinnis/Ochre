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
}

GameScreen::~GameScreen(){
}

#define TOOLWIDTH 200
#define STATUSHEIGHT 100


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

