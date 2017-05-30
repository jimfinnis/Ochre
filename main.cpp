/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */

#include <exception>
#include <unistd.h>

#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"
#include "font.h"

#include "context.h"
#include "screen.h"
#include "meshes.h"
#include "time.h"
#include "game.h"
#include "globals.h"

#include "gamescreen.h"
#include "prof.h"

bool debugtoggle=false;

Screen *curscreen;

// this creates all the graphics and does initialisation, etc.
Context context(800,600);


void render(){
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
}

void handleInput(){
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
        case SDL_MOUSEBUTTONDOWN:
            // put the mouse button to all regions
            Region::notifyClick(e.motion.x,e.motion.y,e.button.button);
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

}

void update(float t){
    globals::timeNow = Time::now();
    globals::game->update(t);
}

#include "pool.h"

// pool test functions.
void tfl(){
    Pool<int> pool(10);
    int *foo[10];
    for(int i=0;i<30;i++){
        foo[i] = pool.alloc();
        if(!foo[i])break;
        *foo[i] = i*10;
        printf("%d - %d\n",i,*foo[i]);
    }


    int chk=0;
    for(int *p = pool.first(); p ; p=pool.next(p)){
        if(chk++ == 100)break;
        printf("%d\n",*p);
   }

    for(int i=0;i<200;i++){
        int qq = rand()%10;
        int pp = rand()%10;
        if(pp==qq)continue;
        if(*foo[qq]!=qq*10){printf("FAIL0\n");};
        pool.free(foo[qq]);
        if(*foo[pp]!=pp*10){printf("FAIL0\n");};
        pool.free(foo[pp]);
        foo[qq] = pool.alloc();
        *foo[qq]=qq*10;
        foo[pp] = pool.alloc();
        *foo[pp]=pp*10;
        pool.dump();
    }

    for(int *p = pool.first(); p ; p=pool.next(p)){
        printf("%d\n",*p);
   }
    std::terminate();

}


int main(int argc, char** argv)
{
    Time::init();
    Font::init();

    globals::init();

    curscreen = new GameScreen();

    meshes::load();

    double lastt = Time::now();
    while(globals::running)
    {
        Time::tick();
        double t = Time::now();
        profbar.start();
        update(t-lastt);
        lastt=t;
        handleInput();
        render();
 //       Time::sleep(0.02); // yeah, frame rate cap.
    };

    return 0;
}
