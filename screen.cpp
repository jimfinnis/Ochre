/**
 * @file screen.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "screen.h"

Screen::Screen(int ww,int hh){
    SDL_Init(SDL_INIT_VIDEO);
    
    wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, 
                           SDL_WINDOWPOS_CENTERED,
                           ww,hh,
                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    glc = SDL_GL_CreateContext(wnd);
    rdr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    resize(ww,hh);
}

#define TOOLWIDTH 200
#define STATUSHEIGHT 200

void Screen::resize(int ww,int hh){
    w = ww;
    h = hh;
    
    status.resize(0,h-STATUSHEIGHT,w,STATUSHEIGHT);
    tool.resize(w-TOOLWIDTH,0,TOOLWIDTH,h);
    game.resize(0,0,w-TOOLWIDTH,h-STATUSHEIGHT);
}
        
void Screen::swap(){
    SDL_GL_SwapWindow(wnd);
}
    
