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

#include "screen.h"
#include "grid.h"
#include "meshes.h"
#include "time.h"

#define VERSION "Ochre 0.0 pre-alpha0"
bool debugtoggle=false;
int main(int argc, char** argv)
{
    Screen scr(800,600);
    
//    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    // load the effects by starting the effect manager
    EffectManager::getInstance();
    Time::init();
    Font::init();
    
    Font *font = new Font("media/fonts/Quicksand-Regular.otf",100);
    Grid *grid = new Grid();
    
    mesh::load();
    
    bool running=true;
    while(running)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type){
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                if(e.key.keysym.sym=='q')running = false;
                if(e.key.keysym.sym=='t')grid = new Grid();
                if(e.key.keysym.sym=='d')debugtoggle=!debugtoggle;
                break;
            case SDL_WINDOWEVENT:
                switch(e.window.event){
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    scr.resize(e.window.data1,
                               e.window.data2);
                    break;
                }
                break;
            }
        }
        
        /* Clear the color and depth buffers. */
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0,0,1,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // draw regions
        scr.stat.setAndClear(Colour(0,0.5,0,1));
        scr.tool.setAndClear(Colour(0,0,0.5,1));
        scr.game.setAndClear(Colour(0,0,0.2,1));
        
        
        // reset the state manager
        StateManager *sm = StateManager::getInstance();
        sm->reset();
        
        scr.stat.set();
        font->render(10,20,30,VERSION);
        
        scr.game.set();
        
        MatrixStack *ms = sm->getx();
        ms->push();
        
        ms->mul(glm::translate(glm::mat4(),glm::vec3(0.0f,0.0f,-0.0f)));
        
        grid->genTriangles(20,20,8);
        grid->render(sm->getx()->top());
        grid->renderHighlight(20,20);
        ms->pop();
        
        
        scr.swap();
        
    };

    return 0;
}
