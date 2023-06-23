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
    int cx = g->cursorx;
    int cy = g->cursory;
    
    Player *p0 = &globals::game->p[0];
    Player *p1 = &globals::game->p[1];
    
    
    globals::font->render(10,40,20,"curs[%d %d] pop %d+%d realpop %d+%d inf %.2f+%.2f fps %d tick %08u",
                          cx,cy,
                          p0->people.size(), p1->people.size(),
                          p0->pop,p1->pop,
                          p0->influence,p1->influence,
                          (int)(1.0/ft),
                          Time::ticks()
                          );
    
    float a0 = globals::game->getAdvantage(p0,cx,cy);
    float a1 = globals::game->getAdvantage(p1,cx,cy);
    
    float pot0 = p0->potentialClose[cx][cy];
    float pot1 = p1->potentialClose[cx][cy];
    
    globals::font->render(10,60,20,"adv0 %.2f adv1 %.2f  cpots %.2f+%.2f",
                          a0,a1,
                          pot0,pot1);
}

void StatRegion::renderprof(){
    profbar.render(this,0,0,w,40);
}    
