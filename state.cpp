/**
 * @file state.cpp
 * @brief  Brief description of file.
 *
 */

#include "state.h"

void State::resetLights(){
    light.ambient = Colour(0.1f,0.1f,0.2f,1);
    
    light.dir[0] = glm::normalize(glm::vec3(1,1,2));
    light.dir[1] = glm::normalize(glm::vec3(2,3,2));
    light.dir[2] = glm::vec3(0,0,0);
    
    light.col[0] = Colour(0,0,0,1);
    light.col[1] = Colour(0,0,0,1);
    light.col[2] = Colour(0,0,0,1);
    
    light.col[0] = Colour(0.7,0.7,  0.5,1);
    light.col[1] = Colour(0.6,0.4,  0.3,1);
}

void State::resetView(){
    view=glm::translate(glm::mat4(1.0f),glm::vec3(0,0,0));
}

void State::setDefault(){
    resetLights();
    resetView();
    fog.enabled = false;
    fog.colour = Colour(0,0,0,1);
    fog.neardist = 10000;
    fog.fardist = 20000;
    diffuse2 = Colour(1,1,1,1);
    texID0 = texID1 = 0;
    overrides = 0;
    modes = 0;
    effect = NULL;
}
