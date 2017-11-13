/**
 * @file button.h
 * @brief  Brief description of file.
 *
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#include "gfx.h"

/// buttons belong to regions, and are quads which 
/// can be clicked on and have an image. Buttons use
/// an ID rather than playing silly buggers with subclasses of Button
/// or comparing against pointer.

class Button {
    GLuint tex;
    class Region *r;
    int x,y,w,h; //!< coords within region
    
public:
    int id;
    
    /// normally -1, this can be set to make buttons belong to a mutex set.
    /// One button in this set will be highlighted, others will be disabled.
    int setid;
    
    /// used by mutexes
    bool highlight;
    
    /// giving bw or bh < 0 uses the size of the texture
    Button(int ident,class Region *reg,const char *filename,
           int bx,int by,int bw,int bh);
    
    bool isIn(int xx,int yy){
        return xx>=x && xx<x+w && yy>=y && yy<y+w;
    }
    
    /// set the mutex set within this region - buttons belonging to
    /// a set may be highlighted, and only one can be at a time.
    Button *setMutex(int s){
        setid=s;
        return this;
    }
    
    /// highlight a button, used to set up the initial button for a
    /// mutex.
    Button *setHighlight(){
        highlight=true;
        return this;
    }
    
    /// render the button image
    virtual void render();
};

#endif /* __BUTTON_H */
