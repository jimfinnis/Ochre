/**
 * @file prof.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PROF_H
#define __PROF_H

#include <stdint.h>
#include "types.h"

class Region;

/// crude profiling bar.

class ProfBar {
    static const int MAXSLOTS=32;
    double t[MAXSLOTS];
    Colour cols[MAXSLOTS];
    double tstart;
    int ct;
public:
    ProfBar();
    void start();
    // mark up to this time - bar from previous time (or start) to
    // this point is in col.
    void mark(uint32_t col);
    // end and render - remaining time is in black
    void render(Region *r,float x,float y,float w,float h);
};

extern ProfBar profbar;

#endif /* __PROF_H */
