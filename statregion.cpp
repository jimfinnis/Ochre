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
#include "prof.h"

void StatRegion::onMouseMove(int x,int y){
}

void StatRegion::render(){
    setAndClear(Colour(0,0.5,0,1));

    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();

//    globals::font->render(10,20,20,"Ochre 0.0 (%s)",__DATE__);

    static double lastTime=0;
    double t = Time::now();
    double ft = t-lastTime;
    lastTime = t;

    Grid *g = &globals::game->grid;

    globals::font->render(10,50,20,"curs[%d %d] peeps %d+%d pop %d+%d winf %.2f/%.2f fps %d tick %08u",
                          g->cursorx,g->cursory,
                          globals::game->p[0].people.size(),
                          globals::game->p[1].people.size(),
                          globals::game->p[0].getPop(),
                          globals::game->p[1].getPop(),
                          globals::game->p[0].winfactor,
                          globals::game->p[1].winfactor,
                          
                          
                          (int)(1.0/ft),
                          Time::ticks()
                          );
}

void StatRegion::renderprof(){
    profbar.render(this,0,0,w,40);
}    
