/**
 * @file person.cpp
 * @brief  Brief description of file.
 *
 */

#include "person.h"
#include "maths.h"
#include "globals.h"
#include "game.h"
#include "player.h"
#include "time.h"

#define PERSONSPEED 3.1f

// table mapping direction onto rotation (in degrees, but gets
// switched to radians)

float dirToRot[3][3] = {
    {glm::radians(45.0f),glm::radians(90.f),glm::radians(135.0f)},
    {glm::radians(0.0f), 0,                 glm::radians(180.0f)},
    {glm::radians(315.0f),glm::radians(270.0f),glm::radians(225.0f)},
};


void Person::goalFound(){
}

bool Person::pathTo(float xx,float yy){
    if(JPS::findPath(path,globals::game->grid,x,y,(int)xx,(int)yy)){
        pathidx=0;
        destx = xx;
        desty = yy;
        pmode = COARSEPATH;
        return true;
    } else {
        pmode = NOPATH;
        return false;
    }
}

float targetX = 0,targetY=128,stigBias=0.8f;

void Person::setDirectionToAntiStigmergy(){
    Grid *g = &globals::game->grid;
    int cx = (int)x;
    int cy = (int)y;
    
    int targetdx = sgn(targetX-x);
    int targetdy = sgn(targetY-y);
    
    static Person *stuckChap=NULL;
    bool show;
    
    if(0 && stuckHormone>0.1f && (*g)(cx,cy) && (!stuckChap || stuckChap==this)){
        stuckChap=this;
        show=true;
    } else show=false;
    if(show)printf("%p %f: %2.2f,%2.2f [%d,%d] (%2.2f,%2.2f) : ",this,
                   stuckHormone,
                   x,y,cx,cy,dx,dy);
    
    int idx=sgn(dx); // blee.
    int idy=sgn(dy);
    
    float minst = FLT_MAX;
    float effectiveStigBias = interp(stigBias,1.0f,stuckHormone);
    //  printf("SB: %f H: %f\n",effectiveStigBias,stuckHormone);
    int oxf,oyf;
    for(int ox=-1;ox<=1;ox++){
        if(show)putchar('(');
        for(int oy=-1;oy<=1;oy++){
            float st=1000;
            // do not scan my own area, do not permit us to turn around
            // or go into the sea. The middle rule there is to avoid
            // stuckage, when combined with the stuck hormone (which decreases
            // bias in stigmergy).
            if( (ox||oy) && (ox!=-idx || oy!=-idy) && (*g)(cx+ox,cy+oy)){
                st =g->mapsteps[cx+ox][cy+oy];
                if(targetdx==ox && targetdy==oy)st*=effectiveStigBias; // and towards target
                if(st<minst){
                    minst=st;oxf=ox;oyf=oy;
                }
            }
            if(show)printf("%2.2f,",st);
        }
        if(show)putchar(')');
    }
            
    
    
    if(minst<FLT_MAX){
        dx = oxf;
        dy = oyf;
        if(show)printf(" --> %2.2f,%2.2f",dx,dy);
    } else {
        dx=dy=0; // might happen if we're in the water
    }
    if(show)putchar('\n');
}



void Person::update(float t){
    Grid *g = &globals::game->grid;
    
    // pathing
    switch(pmode){
    case WANDER:
        setDirectionToAntiStigmergy();
        break;
    case NOPATH:
        pathTo(rand()%GRIDSIZE,rand()%GRIDSIZE);
        break;
    case COARSEPATH:
        if(path.size()){
            float px = (float)path[pathidx].x, py = (float)path[pathidx].y;
            // compare current position with path position
            if((x-px)*(x-px)+(y-py)*(y-py) < 0.25){
                pathidx++; // arrived at next pos, increment path
                if(pathidx==path.size()){
                    pmode = FINEPATH;
                }
            } else {
                dx = sgn(px-x);
                dy = sgn(py-y);
            }
            //  if(abs(g->cursorx - (int)x)<1 && abs(g->cursory-(int)y)<1)
            //  printf("%d/%d: %f %f -> %f %f (%f %f)\n",pathidx,path.size(),x,y,px,py,dx,dy);
        } else
            pmode=FINEPATH;
        break;
    case FINEPATH:
        {
            float deltax = (destx-x);
            float deltay = (desty-y);
            if(deltax*deltax+deltay*deltay < 0.001f){
                dx=dy=0;
                //                printf("%f %f\n",x,y);
                pmode = DEBUGSTOP;
            } else {
                // Zeno's person.
                dx = deltax*0.5f;
                dy = deltay*0.5f;
                //                printf("F %f %f -> %f %f (%f %f)\n",x,y,destx,desty,dx,dy);
            }
        }
        break;
    default:break;
    }
    
    
    if(!(Time::ticks() % 20)){
        oldx=x;oldy=y;
    }
    
    // adjustment for diagonal speed slowdown
    float diag = (dx && dy) ? 0.707107f : 1;
    
    x += PERSONSPEED*t*(float)dx*diag;
    y += PERSONSPEED*t*(float)dy*diag;
    
    // release the stuck hormone
    float ddx = x-oldx;
    float ddy = y-oldy;
    if(ddx*ddx + ddy*ddy < 0.01f){
        // just set it high if we haven't moved.
        stuckHormone=1;
    }
    //float release = (0.99f-stuckHormone)/(1.0f+ddx*ddx+ddy*ddy);
    //stuckHormone += release*0.001f;
    //printf("%f %f\n",release,stuckHormone);
    // and decay it
    stuckHormone *= 0.9f;
    
    g->mapsteps[(int)x][(int)y]++;
    
    if(x<0)x=0;
    if(x>=GRIDSIZE-1)x=GRIDSIZE-2;
    if(y<0)y=0;
    if(y>=GRIDSIZE-1)y=GRIDSIZE-2;
}
