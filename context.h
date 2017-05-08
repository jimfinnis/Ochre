/**
 * @file context.h
 * @brief  Brief description of file.
 *
 */

#ifndef __CONTEXT_H
#define __CONTEXT_H

#include "region.h"
#include "gameregion.h"

/**
 * Graphical context
 */

class Context {
    SDL_Window *wnd;
    SDL_GLContext glc;
    
    GLuint fb;
    GLuint rbcolor,rbdepth; // don't separate these
    GLuint fbwidth,fbheight;
    
    static Context *instance;
    
    void rebuildFSAA();
    
public:
    int w,h;
    class Screen *curscreen;
    SDL_Renderer *rdr;
    
    static Context *getInstance(){
        return instance;
    }
    
    /// status bar across the top
    Region stat;
    /// toolbar on the left
    Region tool;
    /// main GL window on the right - with isometric projection
    GameRegion game;
    
    Context(int w,int h);
    
    void swap();
    
    void resize(int ww,int hh);
};

#endif /* __CONTEXT_H */
