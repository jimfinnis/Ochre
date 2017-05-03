/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#include "region.h"

/**
 * The screen is divided into several regions.
 */

class Screen {
    SDL_Window *wnd;
    SDL_GLContext glc;
    SDL_Renderer *rdr;
    int w,h;
    
public:
    /// status bar across the top
    Region status;
    /// toolbar on the left
    Region tool;
    /// main GL window on the right
    Region game;
    
    Screen(int w,int h);
    
    void swap();
    
    void resize(int ww,int hh);
    
};

#endif /* __SCREEN_H */
