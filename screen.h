/**
 * @file screen.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SCREEN_H
#define __SCREEN_H

/// A collection of regions associated with a game mode

class Screen {
public:
    Screen();
        
    virtual void render()=0;
    virtual void resize(int w,int h)=0;
};

#endif /* __SCREEN_H */
