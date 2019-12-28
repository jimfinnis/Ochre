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
    
    void resetLights();
    void resetView();
    
    /// set to the default - not ctor for speed
    void setDefault();
    
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
    glm::mat4 view = glm::mat4(1.0f);
    
    // texture if any (texID0 overrides material)
    GLuint texID0,texID1;
    class Effect *effect;
    
    // extra stuff
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
