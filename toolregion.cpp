/**
 * @file toolregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "toolregion.h"
#include "state.h"
#include "game.h"
#include "globals.h"

void ToolRegion::onMouseMove(int x,int y){
}

void ToolRegion::render(){
    setAndClear(Colour(0,0,0.5,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
    Grid *g = &globals::game->grid;
    
    float mapw = w*0.75f;
    float mapx = (w-mapw)/2.0f;
    float mapy = 20;
    
    renderQuad(mapx,mapy,mapw,mapw,g->getMapTex());
}


void ToolRegion::onLeftClick(int x,int y){
    y=h - y; // flip coords back again, since we are really working from the top.
    float mapw = w*0.75f;
    float mapx = (w-mapw)/2.0f;
    float mapy = 20;
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
