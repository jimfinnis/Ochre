/**
 * @file toolregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "toolregion.h"
#include "state.h"

void ToolRegion::onMouseMove(int x,int y){
}

void ToolRegion::render(){
    setAndClear(Colour(0,0,0.5,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
}
