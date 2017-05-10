/**
 * @file person.cpp
 * @brief  Brief description of file.
 *
 */

#include "person.h"
#include "maths.h"
#include "grid.h"

#define PERSONSPEED 1.1f

// table mapping direction onto rotation (in degrees, but gets
// switched to radians)

float Person::dirToRot[3][3];

void Person::initConsts(){
    for(int x=-1;x<=1;x++){
        for(int y=-1;y<=1;y++){
            float fx = x-1;
            float fy = y-1;
            if(x&&y)
                dirToRot[x][y] = atan2f(fy,fx);
        }
    }
    dirToRot[1][1]=0;
}


void Person::update(float t){
    // adjustment for diagonal speed slowdown
    float diag = (dx && dy) ? 0.707107f : 1;
    
    x += PERSONSPEED*t*(float)dx*diag;
    y += PERSONSPEED*t*(float)dy*diag;
    
    if(x<0)x=0;
    if(x>=GRIDSIZE-1)x=GRIDSIZE-2;
    if(y<0)y=0;
    if(y>=GRIDSIZE-1)y=GRIDSIZE-2;
}
