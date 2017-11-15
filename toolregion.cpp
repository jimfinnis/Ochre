/**
 * @file toolregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "toolregion.h"
#include "state.h"
#include "game.h"
#include "gamescreen.h"
#include "globals.h"
#include "prof.h"

// map width as ratio of tool region
#define MAPWIDTH 0.9
// map top in tool region
#define MAPTOP 20

// top of buttons row
#define BUTTON_TOP 500
// left edge of buttons row
#define BUTTON_LEFT 10
// button separation
#define BUTTON_SEP 55

// mode button IDs
#define BUT_ATTACK 1
#define BUT_SETTLE 2
#define BUT_COLLECT 3

// modal action button ID
#define BUT_MODE_ANCHOR 4

ToolRegion::ToolRegion() : Region("tool"){
    int x = BUTTON_LEFT;
    int y = BUTTON_TOP;
    
    // player modes
    addButton(BUT_SETTLE,"media/tex/shield.png",x,y,-1,-1)->setMutex(BUTSET_MODES)->setHighlight();
    x+=BUTTON_SEP;
    addButton(BUT_ATTACK,"media/tex/sword.png",x,y,-1,-1)->setMutex(BUTSET_MODES);
    x+=BUTTON_SEP;
    addButton(BUT_COLLECT,"media/tex/collect.png",x,y,-1,-1)->setMutex(BUTSET_MODES);
    
    // player actions
    x = BUTTON_LEFT; y += BUTTON_SEP;
    addButton(BUT_MODE_ANCHOR,"media/tex/collect.png",x,y,-1,-1)->setMutex(BUTSET_ACTIONS);
}

void ToolRegion::onMouseMove(int x,int y){
}

void ToolRegion::onButtonClick(int id){
    Game *game = globals::game;
    GameRegion *gr = &screen->game;
    switch(id){
    // player modes
    case BUT_SETTLE:
        game->p[0].mode = PLAYER_SETTLE;break;
    case BUT_ATTACK:
        game->p[0].mode = PLAYER_ATTACK;break;
    case BUT_COLLECT:
        game->p[0].mode = PLAYER_COLLECT;break;
        
    // modal player actions
    case BUT_MODE_ANCHOR:
        gr->setMode(GRM_SET_ANCHOR);
        break;
    }
              
}

void ToolRegion::render(){
    setAndClear(Colour(0,0,0.5,1));
    
    float mapw = w*MAPWIDTH;
    float mapx = (w-mapw)/2.0f;
    float mapy = MAPTOP;
    
    profbar.start("M",0x008000ff);
    Grid *g = &globals::game->grid;
    map.make(g,true);
    map.render(this,mapx,mapy,mapw,mapw);
    profbar.end();
    
    profbar.start("D1",0x008080ff);
    debugmap.makePotential(&globals::game->p[0]);
    debugmap.render(this,mapx,mapy+20+mapw,mapw/2,mapw/2);
    profbar.end();
    
    profbar.start("D2",0x0080a0ff);
    debugmap.makePotential(&globals::game->p[1]);
    debugmap.render(this,mapx+mapw/2,mapy+20+mapw,mapw/2,mapw/2);
    profbar.end();
    
    profbar.start("B",0x0080a0a0);
    drawButtons();
    profbar.end();
}


void ToolRegion::onLeftClick(int x,int y){
    y=h - y; // flip coords back again, since we are really working from the top.
    float mapw = w*MAPWIDTH;
    float mapx = (w-mapw)/2.0f;
    float mapy = MAPTOP;
    float xx=x,yy=y;
    
    if(x>=mapx && y>=mapy && x<mapx+mapw && y<mapy+mapw){
        xx-=mapx;
        yy-=mapy;
        xx/=mapw;
        yy/=mapw;
        xx*=GRIDSIZE;yy*=GRIDSIZE;
        printf("%f %f\n",xx,yy);
        Grid *g = &globals::game->grid;
        g->cursorx=xx;
        g->cursory=yy;
        g->recentre();
        
    }
}
