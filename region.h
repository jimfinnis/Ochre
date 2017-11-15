/**
 * @file region.h
 * @brief  Brief description of file.
 *
 */

#ifndef __REGION_H
#define __REGION_H

#include "types.h"
#include "gfx.h"
#include "button.h"

#include <vector>
#include <algorithm>

/// a region of the screen which can be set to a GL viewport;
/// setting it will set a pixel-space ortho projection
/// flipped so topleft = 0,0

class Region {
    /// a list of all the regions; we iterate over these when
    /// the mouse is clicked or moved.
    static std::vector<Region *> regions;
    
    const char *name;
    
    /// a list of all our buttons
    std::vector<Button *> buttons;
    
    Button *getButtonForCoords(int x,int y);

    void setButtonInMutex(Button *b);
    
protected:
    
    Button *addButton(int id,const char *texname, int bx,int by,int bw,int bh){
        Button *b = new Button(id,this,texname,bx,by,bw,bh);
        buttons.push_back(b);
        return b;
    }
    
public:
    float x,y,w,h;
    
    /// clear button hilights in a given set
    void clearButtonHighlights(int sid){
        for(auto it = buttons.begin();it!=buttons.end();++it){
            Button *bb = *it;
            if(sid == bb->setid)
                bb->highlight = false;
        }
    }
    
    static void notifyMouseMove(int x,int y);
    static void notifyClick(int x,int y,int b); // b is the SDL mouse button ID
    static void notifyResize();
    
    Region(const char *nm);
    virtual ~Region();
    
    static Region *current;
    
    // called both on resize and initialisation
    virtual void resize(int xx,int yy,int ww,int hh){
        x=xx;y=yy;w=ww;h=hh;
    }
    
    void drawButtons();
    virtual void onMouseMove(int x, int y){};
    virtual void onLeftClick(int w,int h){};
    virtual void onRightClick(int w,int h){};
    virtual void onMiddleClick(int w,int h){};
    virtual void onButtonClick(int id){};
    
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
    void renderQuad(float x,float y,float w,float h,GLuint tex);
    void renderQuadUntex(float x,float y,float w,float h,float *col);
};

/// as above, but sets an isometric projection
struct IsoRegion : public Region{
    IsoRegion(const char *nm) : Region(nm){}
    /// set the viewport for OpenGL, and the projection (to isometric)
    virtual void set();
};

#endif /* __REGION_H */
