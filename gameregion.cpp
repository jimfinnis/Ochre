/**
 * @file gameregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "gameregion.h"
#include "state.h"
#include "grid.h"
#include "globals.h"

GameRegion::GameRegion() : IsoRegion("game") {
    globals::cursorx=20;
    globals::cursory=20;
}

void GameRegion::onMouseMove(int x,int y){
}

void GameRegion::render(){
    setAndClear(Colour(0,0,0.2,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
    MatrixStack *ms = sm->getx();
    ms->push();
    
    ms->mulBack(glm::lookAt(glm::vec3(0.0f,3.0f,10.0f),glm::vec3(),glm::vec3(0.0f,1.0f,0.0f)));
    
    globals::grid->genTriangles(20,20,8);
    globals::grid->render(sm->getx()->top());
    globals::grid->renderCursor(globals::cursorx,globals::cursory);
    
    ms->pop();
    
}
