/**
 * @file gamescreen.cpp
 * @brief  Brief description of file.
 *
 */

#include "gamescreen.h"
#include "globals.h"
#include "game.h"

GameScreen::GameScreen() {
    globals::game = new Game(10,-2);
}

GameScreen::~GameScreen(){
    delete globals::game;
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
    
    Grid *g = &globals::game->grid;
    static int qqq=0;
    switch(k){
//    case 'm': // DEBUGGING KEY TO REGEN WORLD
//        delete globals::game;
//        globals::game = new Game(qqq++,-2);
//        break;
    case SDLK_RIGHT:
        g->cursorx++;break;
    case SDLK_LEFT:
        g->cursorx--;break;
        // these are backwards because right-handed coord system: +ve z is into the screen
    case SDLK_UP:
        g->cursory--;break;
    case SDLK_DOWN:
        g->cursory++;break;
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
    

