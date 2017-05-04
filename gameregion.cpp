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

void GameRegion::renderWater(){
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
   // could usefully prebind this..
    static UNLITVERTEX seaquad[] = {
        {-1,0,-1, 0,1,0, 0,0},
        {-1,0,1, 0,1,0, 0,0},
        {1,0,1, 0,1,0, 1,1},
        {1,0,1, 0,1,0, 1,1},
        {1,0,-1, 0,1,0, 1,0},
        {-1,0,-1, 0,1,0, 0,0}};
    
    GLuint vbo;
    glGenBuffers(1,&vbo);
    ERRCHK;
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,sizeof(UNLITVERTEX)*6,&seaquad[0],GL_STREAM_DRAW);
    ERRCHK;
    
    EffectManager *em = EffectManager::getInstance();
    
    ms->push();
    ms->translate(0,0.5f*globals::grid->heightFactor,0);
    // the 0.6 is because we're not rendering the whole thing, just a diamondy shape
    ms->scale(visibleGridSize*0.62f);
    ms->rotY(glm::radians(45.0f));
    em->untex->begin();
    em->untex->setWorldMatrix(sm->getx()->top());
    static const float seaCol[] = {0.5,2,2,1}; // note the overunity colours for mult. rendering
    em->untex->setMaterial(seaCol,NULL);
    em->untex->setArrayOffsetsUnlit();
    // trick for multiplicative rendering
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_ZERO,GL_SRC_COLOR);
    glDrawArrays(GL_TRIANGLES,0,6);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    em->untex->end();
    ms->pop();
 }

void GameRegion::render(){
    setAndClear(Colour(0,0,0.2,1));
    
    // reset the state manager
    StateManager *sm = StateManager::getInstance();
    sm->reset();
    
    MatrixStack *ms = sm->getx();
    ms->push();
    
    ms->mulBack(glm::lookAt(glm::vec3(0.0f,10.0f,35.0f),glm::vec3(),glm::vec3(0.0f,1.0f,0.0f)));
    
    
    globals::grid->genTriangles(globals::cursorx,globals::cursory,visibleGridSize);
    globals::grid->render(sm->getx()->top());
    globals::grid->renderCursor(globals::cursorx,globals::cursory);
    
    renderWater();
    
    
    ms->pop();
    
}
