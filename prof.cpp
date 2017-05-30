/**
 * @file prof.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "context.h"

#include "prof.h"
#include "time.h"

ProfBar profbar;

ProfBar::ProfBar(){
    ct=0;
}

void ProfBar::start(){
    tstart = Time::now();
    ct=0;
}

void ProfBar::mark(uint32_t col){
    if(ct<MAXSLOTS){
        cols[ct].setFromRGBA32(col);
        t[ct++]=Time::now();
    }
}


void ProfBar::render(Region *r,float x,float y,float w,float h){
    double end = Time::now();
    double range = end-tstart;
    double cur=0;
    for(int i=0;i<ct;i++){
        double mark = (t[i]-tstart)/range;
        double pos = mark*w;
        r->renderQuadUntex(x+cur,y,pos-cur,h,cols[i].getFloatPtr());
        cur=pos;
    }
    static float blackCol[]={0,0,0,1};
    r->renderQuadUntex(x+cur,y,w-(x+cur),h,blackCol);
}
