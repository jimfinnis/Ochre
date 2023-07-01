/**
 * @file leveller.cpp
 * @brief  Brief description of file.
 *
 */

#include "spiral.h"

#include "leveller.h"
#include "game.h"
#include "time.h"


static constexpr double AUTOLEVELMININTERVAL = 0.4; // interval between autolevelling events
static constexpr double AUTOLEVELMAXINTERVAL = 1.2; // interval between autolevelling events

void Leveller::run(){
    if(Time::now() > nextAutolevelTime){
        double interval = drand48()*(AUTOLEVELMAXINTERVAL-AUTOLEVELMININTERVAL)+AUTOLEVELMININTERVAL;
        nextAutolevelTime = Time::now() + interval;
    } else
        return;
        
    
    RandomSpiralSearch spiral;
    
    // we assume that the "god" has a view of a particular location, and will always
    // level close to that location. It starts at the average location of all the people.
    double xx=0,yy=0;
    if(levelx<0){ // need to reinit.
        // find avg. location of people
        double n=0;
        for(Person *pp=p->people.first();pp;pp=p->people.next(pp)){
            xx+=pp->x;yy+=pp->y;n++;
        }
        if(n>0.000001){
            xx/=n;yy/=n;
        } else {
            return; // no people
        }
        
        levelx=(int)(xx+0.5);
        levely=(int)(yy+0.5);
        
        // offset a bit, to avoid multiple levellers in the same place
        
        levelx += (rand()%9)-4;
        levely += (rand()%9)-4;
        
        double interval = drand48()*(AUTOLEVELMAXINTERVAL-AUTOLEVELMININTERVAL)+AUTOLEVELMININTERVAL;
        nextAutolevelTime=Time::now()+interval*2.0f;
    }
    
    // now find a candidate to level. What's the simplest thing that can possibly work?
    // We just pull the location towards sea level+1.
    
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    // get the current height and modify it
    int h = g->get(levelx,levely);
    if(h<1){
        g->up(levelx,levely);
    }
    else if(h>1){
        g->down(levelx,levely);
    }
    
    // now get the next location by looking around for something which isn't at sea level
    // but is next to something which is.
    bool found=false;
    for(spiral.start();spiral.layer<50;spiral.next()){
        int gx = levelx+spiral.x;
        int gy = levely+spiral.y;
        if(g->in(gx,gy)){
            h = g->get(gx,gy);
            if(h!=1 && g->nextToDry(gx,gy,3)){
                levelx=gx;
                levely=gy;
                found=true;
                break;
            }
        }
    }
    if(!found){
        printf("Not found\n");
        // damn, everything's level nearby. Delay for random seconds and restart
        nextAutolevelTime=Time::now()+(rand()%2)+2;
        levelx=-1;
    } else if(!(rand()%100)) {
        levelx=-1; // just randomly reset now and then
    }
}    
