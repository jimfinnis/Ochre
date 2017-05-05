/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */

#include <exception>

#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"
#include "font.h"

#include "context.h"
#include "screen.h"
#include "grid.h"
#include "meshes.h"
#include "time.h"
#include "globals.h"

#include "gamescreen.h"

bool debugtoggle=false;

Screen *curscreen;

int main(int argc, char** argv)
{
    Context context(800,600);
    
    
    // load the effects by starting the effect manager
    EffectManager::getInstance();
    Time::init();
    Font::init();
    
    globals::init();
    
    curscreen = new GameScreen();
    
    meshes::load();
    
    while(globals::running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type){
            case SDL_QUIT:
                globals::running = false;
                break;
            case SDL_KEYDOWN:
                // Emergency quick quit!
                switch(e.key.keysym.sym){
                case 'q':globals::running = false;break;
                case 'd':debugtoggle=!debugtoggle;break;
                default:
                    curscreen->onKeyDown(e.key.keysym.sym);
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                // put the mouse move to all regions
                Region::notifyMouseMove(e.motion.x,e.motion.y);
                break;
            case SDL_WINDOWEVENT:
                switch(e.window.event){
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    context.resize(e.window.data1,
                                   e.window.data2);
                    break;
                }
                break;
            }
        }
        
        if(debugtoggle)
            glDisable(GL_MULTISAMPLE);
        else
            glEnable(GL_MULTISAMPLE);
            
        
        /* Clear the color and depth buffers. */
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0,0,1,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // reset the state manager
        StateManager *sm = StateManager::getInstance();
        sm->reset();
        
        // draw regions
//        scr.stat.setAndClear(Colour(0,0.5,0,1));
//        scr.tool.setAndClear(Colour(0,0,0.5,1));
        
        
//        scr.stat.set();
        
        curscreen->render();
        
        context.swap();
        
    };

    return 0;
}
