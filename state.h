/**
 * \file
 * Brief description. Longer description.
 * 
 * \author $Author$
 * \date $Date$
 */


#ifndef __STATE_H
#define __STATE_H

#include "gfx.h"
#include "maths.h"

/// the current state

struct State {
    
    void resetLights(){
        light.ambient = Colour(0.1f,0.1f,0.1f,1);
        
        light.dir[0] = glm::vec3(-1,0.3,0);
        light.dir[1] = glm::vec3(-1,0.4,0);
        light.dir[2] = glm::vec3(0,0,0);
        
        light.col[0] = Colour(1,1,1,1);
        light.col[1] = Colour(1,1,1,1);
        light.col[2] = Colour(0.0,0,0,1);
    }
    
    void resetView(){
        view=glm::translate(glm::mat4(),glm::vec3(0,0,0));
    }
        
    
    /// set to the default - not ctor for speed
    void setDefault(){
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
    
    /// LIGHT states
    struct
    {
        Colour ambient;
        Colour col[3];
        glm::vec3 dir[3];
    } light;
    
    
    /// FOG states
    struct
    {
        Colour colour;
        float neardist; // these two are processed as a float[2] array.
        float fardist;  // so don't reorder or separate them
        bool enabled;
    } fog;
    
    /// view matrix
    glm::mat4 view;
    
    // texture if any (texID0 overrides material)
    SDL_Texture *texID0,*texID1;
    class Effect *effect;
    
    // extra stuff
    class Texture *texture2;
    Colour diffuse2;
    
    // other overrides are determined by some flags
    
// diffuse colour overrides material colour    
#define STO_DIFFUSE 1
#define STO_ALPHA 2
    int overrides;
// other booleans
#define STM_ADDITIVE 1
    int modes;
    Colour diffuse; // w is ignored; it gets overwritten by alpha
    float alpha;
    
};




/// state manager singleton

class StateManager {
private:
    static StateManager *instance;
    /// the stack of saved render states
    State statestack[128];
    int stackct;
    
    /// the transform stack
    MatrixStack xformstack;
    
    /// the current state
    State cur;
    
    StateManager(){
        reset();
        xformstack.reset();
    }
    
public:
    static StateManager *getInstance(){
        if(instance==NULL)
            instance = new StateManager();
        return instance;
    }
    
    State *get(){
        return &cur;
    }
    
    MatrixStack *getx(){
        return &xformstack;
    }
    
    /// clear the stacks and set default state.
    void reset(){
        cur.setDefault();
        stackct=0;
    }
    
    /// push the a copy of current state onto the stack, so
    /// that pop will restore it and return the current state
    State *push(){
        if(stackct==128)throw Exception("state stack overflow");
        statestack[stackct++] = cur;
        return &cur;
    }
    
    /// restore the previous state, overwriting the current one
    void pop(){
        if(!stackct)throw Exception("state stack underflow");
        cur = statestack[--stackct];
    }
    
    
};


#endif /* __STATE_H */
