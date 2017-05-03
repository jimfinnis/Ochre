/**
 * @file region.h
 * @brief  Brief description of file.
 *
 */

#ifndef __REGION_H
#define __REGION_H

#include "types.h"


/// a region of the screen which can be set to a GL viewport;
/// setting it will set a pixel-space ortho projection
struct Region {
    int x,y,w,h;
    
    Region(){
        // initially invalid
        x=-1; y=-1;w=1;h=1;
    }
    
    virtual void resize(int xx,int yy,int ww,int hh){
        x=xx;y=yy;w=ww;h=hh;
    }
    
    /// convert a 0-1 into region coords - only used where appropriate
    int getx(float xx){
        return (xx*w)+x;
    }
    
    /// convert a 0-1 into region coords - only used where appropriate
    int gety(float yy){
        return (yy*h)+y;
    }
    
    /// set the viewport for OpenGL
    virtual void setvp();
    
    /// set the viewport for OpenGL, and the projection (to pixels)
    virtual void set();
    
    /// set and clear the region to a colour (also the depth buffer).
    void setAndClear(const Colour& c);
    
};

/// as above, but sets an isometric projection
struct IsoRegion : public Region{
    /// set the viewport for OpenGL, and the projection (to isometric)
    virtual void set();
};

#endif /* __REGION_H */
