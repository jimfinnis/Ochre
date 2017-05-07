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
    delete globals::grid;
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
        globals::grid->cursorx++;break;
    case SDLK_LEFT:
        globals::grid->cursorx--;break;
        // these are backwards because right-handed coord system: +ve z is into the screen
    case SDLK_UP:
        globals::grid->cursory--;break;
    case SDLK_DOWN:
        globals::grid->cursory++;break;
    case 'p':
        globals::grid->up(globals::grid->cursorx,globals::grid->cursory);
        break;
    case 'l':
        globals::grid->down(globals::grid->cursorx,globals::grid->cursory);
        break;
    case 'x':
        globals::grid->recentre();
        break;
    default:
        printf("%d\n",k);
    }
}
    

