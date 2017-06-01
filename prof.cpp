/**
 * @file prof.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "context.h"

#include "prof.h"
#include "time.h"
#include "font.h"
#include "globals.h"

ProfBar profbar;

ProfBar::ProfBar(){
}

void ProfBar::startbar(){
    tstart = Time::now();
    regs.clear();
    while(stack.size())stack.pop();
}

void ProfBar::start(const char *name,uint32_t col){
    double t = Time::now();
    // if we are within a region already (stack has items),
    // then terminate that region.
    if(stack.size())
        regs.back().t2 = t;
    // push the entry onto the stack.
    stack.push(Ent(name,0,col));
    // start a new region
    regs.push_back(Reg(name,t,Colour(col)));
}

void ProfBar::end(){
    double t = Time::now();
    
    // terminate the current region
    regs.back().t2=t;
    
    Ent e = stack.top();
    stack.pop();
    
    // start a new region, either black for dead space or returning to a previous colour
    Colour col;
    if(stack.size())
        col = stack.top().col;
    else
        col = Colour(0,0,0);
    
    regs.push_back(Reg(NULL,t,col));
}


void ProfBar::render(Region *rg,float x,float y,float w,float h){
    double end = Time::now();
    double range = end-tstart;
    h*=0.5f;
    // terminate the current region
    regs.back().t2=end;
    
    rg->set();
    glDepthMask(GL_FALSE);
    for(int i=0;i<regs.size();i++){
        Reg& r = regs[i];
        double t1 = (r.t1-tstart)/range;
        double t2 = (r.t2-tstart)/range;
        double spos = t1*w;
        double epos = t2*w;
        rg->renderQuadUntex(x+spos,y,epos-spos,h,r.col.getFloatPtr());
        if(r.name)
            globals::font->render(x+spos,y+h,h,r.name);
    }
    glDepthMask(GL_TRUE);
    
}
