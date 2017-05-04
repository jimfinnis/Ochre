/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#include "region.h"
#include "gameregion.h"

/**
 * The screen is divided into several regions.
 */

class Screen {
    friend class Font;
    
    SDL_Renderer *rdr;
    SDL_Window *wnd;
    SDL_GLContext glc;
    
    static Screen *instance;
    
public:
    int w,h;
    
    static Screen *getInstance(){
        return instance;
    }
    
    /// status bar across the top
    Region stat;
    /// toolbar on the left
    Region tool;
    /// main GL window on the right - with isometric projection
    GameRegion game;
    
    Screen(int w,int h);
    
    void swap();
    
    void resize(int ww,int hh);
};

#endif /* __SCREEN_H */
