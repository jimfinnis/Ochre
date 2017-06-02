/**
 * @file gamescreen.cpp
 * @brief  Brief description of file.
 *
 */

#include "gamescreen.h"
#include "globals.h"
#include "game.h"
#include "prof.h"

GameScreen::GameScreen() {
    globals::game = new Game(100,-3);
}

GameScreen::~GameScreen(){
    delete globals::game;
}

#define TOOLWIDTH 300
#define STATUSHEIGHT 80


void GameScreen::resize(int w,int h){
    stat.resize(0,h-STATUSHEIGHT,w-TOOLWIDTH,STATUSHEIGHT);
    tool.resize(w-TOOLWIDTH,0,TOOLWIDTH,h);
    game.resize(0,0,w-TOOLWIDTH,h-STATUSHEIGHT);
}    

void GameScreen::render(){
    game.render();
    tool.render();
    stat.render(); // render last if profiling!
    
}

void GameScreen::onKeyDown(int k){
    
    Grid *g = &globals::game->grid;
    static int qqq=0;
    switch(k){
//    case 'm': // DEBUGGING KEY TO REGEN WORLD
//        delete globals::game;
//        globals::game = new Game(qqq++,-2);
//        break;
    case SDLK_RIGHT:
        g->moveCursor(1,0);
        break;
    case SDLK_LEFT:
        g->moveCursor(-1,0);
        break;
        // these are backwards because right-handed coord system: +ve z is into the screen
    case SDLK_UP:
        g->moveCursor(0,-1);
        break;
    case SDLK_DOWN:
        g->moveCursor(0,1);
        break;
    case 'p':
        g->up(g->cursorx,g->cursory);
        break;
    case 'l':
        g->down(g->cursorx,g->cursory);
        break;
    case 'x':
        g->recentre();
        break;
    case SDLK_PERIOD:
        game.rotAngle+=0.1f;break;
    case SDLK_COMMA:
        game.rotAngle-=0.1f;break;
    default:
        printf("%d\n",k);
    }
}
    

