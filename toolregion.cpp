/**
 * @file toolregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "toolregion.h"
#include "state.h"
#include "game.h"
#include "globals.h"
#include "prof.h"

// map width as ratio of tool region
#define MAPWIDTH 0.9
// map top in tool region
#define MAPTOP 20

ToolRegion::ToolRegion() : Region("tool"){
    map.saveVideo("foo.mpg");
}

void ToolRegion::onMouseMove(int x,int y){
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
