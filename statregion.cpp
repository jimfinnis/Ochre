/**
 * @file statregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "statregion.h"
#include "state.h"
#include "globals.h"
#include "font.h"
#include "time.h"
#include "game.h"

void StatRegion::onMouseMove(int x,int y){
}

void StatRegion::render(){
    setAndClear(Colour(0,0.5,0,1));

    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();

    globals::font->render(10,20,20,"Ochre 0.0 (%s)",__DATE__);

    static double lastTime=0;
    double t = Time::now();
    double ft = t-lastTime;
    lastTime = t;

    Grid *g = &globals::game->grid;

    extern float snark;
    globals::font->render(10,50,20,"curs[%d %d] pop %d fps %d tick %08u snark %.3f",
                          g->cursorx,g->cursory,
                          globals::game->p.people.size(),
                          (int)(1.0/ft),
                          Time::ticks(),
                          snark);


}
