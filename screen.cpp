/**
 * @file screen.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "screen.h"
#include "exception.h"

Screen *Screen::instance = NULL;

#define FULLSCREEN 0
Screen::Screen(int ww,int hh) : stat("stat"), tool("tool"), game("game"){
    if(instance)
        FATAL("cannot create multiple screens");
    instance = this;
    
    SDL_Init(SDL_INIT_VIDEO);
#if FULLSCREEN    
    wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, 
                           SDL_WINDOWPOS_UNDEFINED,
                           1024,968,
                           SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
#else
    wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, 
                           SDL_WINDOWPOS_CENTERED,
                           ww,hh,
                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
#endif
    // this doesn't appear to work...
    SDL_SetWindowMinimumSize(wnd,640,480);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
//    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    glc = SDL_GL_CreateContext(wnd);
    
    rdr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
//    glEnable(GL_MULTISAMPLE);
//    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    
    resize(ww,hh);
}

#define TOOLWIDTH 200
#define STATUSHEIGHT 100

void Screen::resize(int ww,int hh){
    w = ww;
    h = hh;
    
    if(ww<640 || hh<480){
        // UGLY HACK because setminimumwindowsize doesn't always work.
        SDL_SetWindowSize(wnd,640,480);
        return;
    }
    
    stat.resize(0,h-STATUSHEIGHT,w,STATUSHEIGHT);
    tool.resize(w-TOOLWIDTH,0,TOOLWIDTH,h);
    game.resize(0,0,w-TOOLWIDTH,h-STATUSHEIGHT);
}
        
void Screen::swap(){
    SDL_GL_SwapWindow(wnd);
}
    
