/**
 * @file gameregion.cpp
 * @brief  Brief description of file.
 *
 */
#include "gameregion.h"
#include "state.h"
#include "globals.h"
#include "game.h"
#include "obj.h"
#include "prof.h"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

GameRegion::GameRegion() : IsoRegion("game") {
    rotAngle=0;
}

void GameRegion::onMouseMove(int sx,int sy){
    // get norm. dev. coords
    sx=sx-x;
    sy=sy-y;
//    printf("POS %d %d - ",sx,sy);
    float ndx=(2.0f * sx)/w-1.0f;
    float ndy=(2.0f * sy)/h-1.0f; // this is already reversed (see region::notifymousemove)
//    printf("%f %f\n",ndx,ndy);
    
    set(); // to make sure the effect manager has the correct projection
    glm::vec4 ray_clip(ndx,ndy,-1.0f,1.0f);
    glm::vec4 ray_eye = glm::inverse(EffectManager::projection)*ray_clip;
    ray_eye.z = -1;ray_eye.w = 0;
//    printf("EYE %f %f %f\n",ray_eye.x,ray_eye.y,ray_eye.z);
    
    glm::vec3 ray_world = glm::vec3(glm::inverse(view)*ray_eye);
    ray_world = glm::normalize(ray_world);
//    std::cout << glm::to_string(view) << std::endl;
//    printf("WOR %f %f %f\n",ray_world.x,ray_world.y,ray_world.z);
    
    int pt = globals::game->grid.intersect(glm::vec3(0,10,35),ray_world);
//    printf("%d\n",pt);
    globals::game->grid.select(pt);
}

void GameRegion::onLeftClick(int x,int y){
    Grid *g = &globals::game->grid;
    g->up(g->cursorx,g->cursory);
}

void GameRegion::onRightClick(int x,int y){
    Grid *g = &globals::game->grid;
    g->down(g->cursorx,g->cursory);
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
    
    Grid *g = &globals::game->grid;

    ms->push();
    ms->translate(0,0.5f*g->heightFactor,0);
    // the 0.6 is because we're not rendering the whole thing, just a diamondy shape
    ms->scale(visibleGridSize*0.62f);
    ms->rotY(glm::radians(45.0f));
    em->untex->begin();
    em->untex->setWorldMatrix(sm->getx()->top());
    static const float seaCol[] = {0.5,2,2,1}; // note the overunity colours for mult. rendering
    em->untex->setMaterial(seaCol,0);
    em->untex->setArrayOffsetsUnlit();
    // trick for multiplicative rendering
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_ZERO,GL_SRC_COLOR);
    glDrawArrays(GL_TRIANGLES,0,6);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    glDeleteBuffers(1,&vbo);
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
    
    ms->mul(glm::lookAt(glm::vec3(0.0f,10.0f,35.0f),glm::vec3(),glm::vec3(0.0f,1.0f,0.0f)));
    ms->rotY(rotAngle);
    ms->translate(-0.5,0,0);
    
    // copy the worldview matrix so we can access it for mouse clickage.
    view = *(ms->top());
    
    Game *game = globals::game;
    
    Grid *g = &game->grid;
    g->genTriangles(visibleGridSize);
    g->render(ms->top());
    
    profbar.start("RO",0xffff00ff);
    g->renderObjects(visibleGridSize);
    ObjMesh::renderAll();
    profbar.end();
    
    g->renderCursor();
    
    renderWater();
    
    profbar.start("RP",0xff8000ff);
    game->p[0].render(Colour(1,0.2f,0.2f));
    game->p[1].render(Colour(0.2f,1,0.2f));
    profbar.end();
    
    ms->pop();
    
}
