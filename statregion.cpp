/**
 * @file statregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "statregion.h"
#include "state.h"
#include "globals.h"
#include "font.h"

void StatRegion::onMouseMove(int x,int y){
}

void StatRegion::render(){
    setAndClear(Colour(0,0.5,0,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
    globals::font->render(10,20,30,"Ochre 0.0 pre-alpha0");
}
