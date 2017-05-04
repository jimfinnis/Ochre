/**
 * @file region.h
 * @brief  Brief description of file.
 *
 */

#ifndef __REGION_H
#define __REGION_H

#include "types.h"
#include "gfx.h"
#include <vector>
#include <algorithm>

/// a region of the screen which can be set to a GL viewport;
/// setting it will set a pixel-space ortho projection
/// flipped so topleft = 0,0

class Region {
    static std::vector<Region *> regions;
    const char *name;
public:
    float x,y,w,h;
    
    static void notifyMouseMove(int x,int y);
    
    Region(const char *nm);
    virtual ~Region();
    
    static Region *current;
    
    virtual void resize(int xx,int yy,int ww,int hh){
        x=xx;y=yy;w=ww;h=hh;
    }
    
    virtual void onMouseMove(int x, int y);
    
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
    
    /// render the things for this region
    virtual void render(){};
    
    /// set and clear the region to a colour (also the depth buffer).
    void setAndClear(const Colour& c);
    
    /// render a quad into the region using its coordinates (not for iso!)
    void renderQuad(float x,float y,float w,float h,SDL_Texture *tex);
};

/// as above, but sets an isometric projection
struct IsoRegion : public Region{
    IsoRegion(const char *nm) : Region(nm){}
    /// set the viewport for OpenGL, and the projection (to isometric)
    virtual void set();
};

#endif /* __REGION_H */
