/**
 * @file person.cpp
 * @brief  Brief description of file.
 *
 */

#include "person.h"
#include "maths.h"
#include "globals.h"
#include "game.h"

#define PERSONSPEED 11.1f

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
    Grid *g = &globals::game->grid;
    
    // randomly change dest
    if(!(rand()%100)){
        if((*g)(x,y))        path.clear();
    }

    
    // pathing test
    if(!path.size()){
        pathidx=0;
        destx = rand()%GRIDSIZE;
        desty = rand()%GRIDSIZE;
        dx=dy=0;
        if(g->get(destx,desty)>0){
            bool found=JPS::findPath(path,*g,
                                     x,y,
                                     destx,desty,
                                     0);
        }
    }        
    
    // pathing
    if(path.size()){
        int px = path[pathidx].x, py = path[pathidx].y;
        // compare current position with path position
        if((int)x == px && (int)y == py){
            pathidx++; // arrived at next pos, increment path
            if(pathidx==path.size()){
                path.clear();dx=dy=0; // arrived at final point
            }
        } else {
            dx = sgn(px-x);
            dy = sgn(py-y);
        }
//        printf("%d/%d: %f %f -> %d %d (%d %d)\n",pathidx,path.size(),x,y,px,py,dx,dy);
    }
    
    
    // adjustment for diagonal speed slowdown
    float diag = (dx && dy) ? 0.707107f : 1;
    
    x += PERSONSPEED*t*(float)dx*diag;
    y += PERSONSPEED*t*(float)dy*diag;
    
    if(x<0)x=0;
    if(x>=GRIDSIZE-1)x=GRIDSIZE-2;
    if(y<0)y=0;
    if(y>=GRIDSIZE-1)y=GRIDSIZE-2;
}
