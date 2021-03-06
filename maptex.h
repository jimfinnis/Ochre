/**
 * @file maptex.h
 * @brief  Brief description of file.
 *
 */

#ifndef __MAPTEX_H
#define __MAPTEX_H

#include "grid.h"
#include "region.h"
#include "game.h"
#include "videofile.h"

/// a map texture. Used for the main game map, but also for debugging.

class MapTex {
protected:
    struct coltable {
        uint32_t cols[16]; // normal map colour
        uint32_t colsvis[16]; // visible map colour
        coltable(){
            Colour c;
            c.setFromHSV(0.5,0.8,0.5f);
            cols[0] = c.getABGR32();
            c.setFromHSV(0.5,0.8,1);
            colsvis[0] = c.getABGR32();
            
            for(int i=1;i<16;i++){
                Colour c;
                float base = powf(((float)i)/16.0f,0.4f)*0.5f;
                c.setFromHSV(0,0,base);
                cols[i] = c.getABGR32();
                c.setFromHSV(0,0,base*2.0f);
                colsvis[i] = c.getABGR32();
            }
        }
        
    };
    static coltable cols;
    
    GLuint tex;
    uint32_t image[GRIDSIZE][GRIDSIZE];

    // puts the image data into the texture, call at the end of writeTex().
    void copy();
    
    // writes a player's peeps to image
    void writePlayer(Player *p,uint32_t col);
    
    Video *vid;
public:
    
    MapTex();
    virtual ~MapTex();
    
    // if CV is linked in, write this map to a video. Call once only.
    void saveVideo(const char *fn){
        vid = new Video(fn,GRIDSIZE,GRIDSIZE);
    }
    
    // draw the thing into a screen region - will reset all transforms so this is screen coords
    void render(Region *r,int x,int y,int w,int h);
    
    // override to actually do something different - this will perform the standard
    // behaviour - but remember to call copy() at the end.
    // If writerame is true AND saveVideo was called,
    // write frame to video too.
    virtual void make(Grid *g,bool writeframe=false);
};

#endif /* __MAPTEX_H */
