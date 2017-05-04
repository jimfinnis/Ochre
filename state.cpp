/**
 * @file state.cpp
 * @brief  Brief description of file.
 *
 */

#include "state.h"

void State::resetLights(){
    light.ambient = Colour(0.1f,0.1f,0.1f,1);
    
    light.dir[0] = glm::normalize(glm::vec3(-1,1,1));
    light.dir[1] = glm::normalize(glm::vec3(1,0.4,1));
    light.dir[2] = glm::vec3(0,0,0);
    
    light.col[0] = Colour(0.7,1,1,1);
    light.col[1] = Colour(0.2,0.2,0.7,1);
    light.col[2] = Colour(0,0,0,1);
}

void State::resetView(){
    view=glm::translate(glm::mat4(),glm::vec3(0,0,0));
}

void State::setDefault(){
    resetLights();
    resetView();
    fog.enabled = false;
    fog.colour = Colour(0,0,0,1);
    fog.neardist = 10000;
    fog.fardist = 20000;
    diffuse2 = Colour(1,1,1,1);
    texID0 = texID1 = NULL;
    overrides = 0;
    modes = 0;
    effect = NULL;
}
