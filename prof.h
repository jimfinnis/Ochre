/**
 * @file prof.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PROF_H
#define __PROF_H

#include <stdint.h>
#include "types.h"
#include <stack>
#include <vector>

class Region;

/// crude profiling bar.

class ProfBar {
    struct Ent {
        Ent(const char *n,double tt,uint32_t cc){
            name =n;
            t = tt;
            col.setFromRGBA32(cc);
        }
        Ent(const char *n,double tt,const Colour& cc){
            name =n;
            t = tt;
            col = cc;
        }
        const char *name;
        double t;
        Colour col;
    };
    
    struct Reg {
        Reg(const char *n,double tt,const Colour &cc){
            t1=tt; // t2 will be filled in later.
            col=cc;
            name=n;
        }
        double t1,t2;
        Colour col;
        const char *name;
    };
    
    std::vector<Reg> regs;
    std::stack<Ent> stack;
    
    double tstart;
public:
    ProfBar();
    void startbar(); // start profiling bar
    
    // start of a region
    void start(const char *name,uint32_t col);
    // end of a region
    void end();
    
    // end and render - remaining time is in black
    void render(Region *r,float x,float y,float w,float h);
};

extern ProfBar profbar;

#endif /* __PROF_H */
