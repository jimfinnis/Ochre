/**
 * @file region.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "region.h"
#include "effect.h"

void Region::setvp(){
    glViewport(x,y,w,h);
    glScissor(x,y,w,h);
    glEnable(GL_SCISSOR_TEST);
}

void Region::set(){
    setvp();
    EffectManager::projection = glm::ortho(0,w,0,h,-1,1);
}

void Region::setAndClear(const Colour& c){
    set();
    
    glClearColor(c.r,c.g,c.b,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
}

void IsoRegion::set(){
    setvp();
    
    EffectManager::projection = 
          glm::ortho(-2.0f,2.0f,  -2.0f,2.0f,  -2.0f,2.0f) *
          glm::rotate(glm::mat4(),glm::radians(-35.264f),glm::vec3(1.0f, 0.0f, 0.0f))*
          glm::rotate(glm::mat4(),glm::radians(-45.0f),glm::vec3(0.0f,1.0f,0.0f));
}

