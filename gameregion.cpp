/**
 * @file gameregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "gameregion.h"
#include "state.h"
#include "grid.h"

void GameRegion::onMouseMove(int x,int y){
}

void GameRegion::render(){
    extern Grid *grid;
    setAndClear(Colour(0,0,0.2,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
    MatrixStack *ms = sm->getx();
    ms->push();
    
    ms->mul(glm::translate(glm::mat4(),glm::vec3(0.0f,0.0f,-0.0f)));
    
    grid->genTriangles(20,20,8);
    grid->render(sm->getx()->top());
    grid->renderHighlight(20,20);
    
    ms->pop();
    
}
