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

void Person::init(class Player *player, int idx, float xx,float yy){
    x=xx;y=yy;
    dx=dy=0;
    p=player;
    drowntime=0;
    nextInfrequentUpdate = globals::timeNow+INFREQUENTUPDATEINTERVAL*0.2*(double)(idx%10);
}


bool Person::pathTo(float xx,float yy){
    if(JPS::findPath(path,globals::game->grid,x,y,(int)xx,(int)yy)){
        pathidx=0;
        destx = xx;
        desty = yy;
        state = COARSEPATH;
        return true;
    } else {
        state = WANDER;
        return false;
    }
}

// this is how much the player's wander target direction decreases
// the stigmergic bias.
static float stigBias=1.5f;

void Person::setDirectionToAntiStigmergy(){
    Grid *g = &globals::game->grid;
    int cx = (int)x;
    int cy = (int)y;
    
    int targetdx = sgn(p->wanderX-x);
    int targetdy = sgn(p->wanderY-y);
    
    int idx=sgn(dx); // blee.
    int idy=sgn(dy);
    
    float minst = FLT_MAX;
    
    int oxf,oyf;
    for(int ox=-1;ox<=1;ox++){
        for(int oy=-1;oy<=1;oy++){
            float st=1000;
            // do not scan my own area, do not permit us to turn around
            // or go into the sea. The middle rule there is to avoid
            // stuckage.
            if( (ox||oy)  // my own square
                && (ox!=-idx && oy!=-idy) &&  // turning around
                (*g)(cx+ox,cy+oy)) // safe square (uses the operator() JPS uses for pathing)
            {
                // add a bit of random to the stigmergy
                st =g->mapsteps[cx+ox][cy+oy] * globals::rnd->range(1,1.5f);
                if(targetdx==ox && targetdy==oy)st*=stigBias; // and towards target
                if(st<minst){
                    minst=st;oxf=ox;oyf=oy;
                }
            }
        }
    }
    
    
    
    if(minst<FLT_MAX){
        dx = oxf;
        dy = oyf;
    } else {
        dx=dy=0; // might happen if we're in the water
    }
}

void Person::updateInfrequent(){
    // repath - this runs infrequently.
    
    Grid *g = &globals::game->grid;
    int ix = (int)x;
    int iy = (int)y;
    
    // are we on a flat square? Which is OK?
    
    if(!g->objects[ix][iy] &&  // no objects in the way
       (*g)(ix,iy) && // safe
       g->get(ix,iy) && // quick flat check
       !globals::rnd->getInt(5) && // basic chance
       g->isFlatForBuild(ix,iy)){ // flat
        // work out a chance we'll actually do it, based on
        // how big the house will be
        int c = g->countFlatForBuild(ix,iy);
        if(globals::rnd->getInt(6)<=c) { // higher chance if bigger result
            // make a new house if we can
            House *h = p->houses.alloc();
            if(h){
                h->init(ix,iy,p);
                //            printf("House added at %d,%d  %p\n",ix,iy,p);
                // "kill" the villager (he is now the houseowner and moves
                // into the house)
                state = ZOMBIE; 
            }
        }
    }
    
    switch(state){
    case WANDER:
        // if we're wandering stigmergically - the default behaviour - that's in
        // a separate method.
        setDirectionToAntiStigmergy();
        break;
    case COARSEPATH:
        // if we're following a path, and there's some path left, go that way.
        if(path.size()){
            float px = (float)path[pathidx].x, py = (float)path[pathidx].y;
            // "Are we there yet? Are we there yet?"
            // compare current position with path position 
            if((x-px)*(x-px)+(y-py)*(y-py) < 0.25){
                // arrived at next pos, increment path index.
                pathidx++; 
                if(pathidx==path.size()){
                    // if we've completed the path, move onto fine pathing.
                    state = FINEPATH;
                }
            } else {
                // "No, we're not there yet. Shut up, and don't hit your sister."
                dx = sgn(px-x);
                dy = sgn(py-y);
            }
            //  if(abs(g->cursorx - (int)x)<1 && abs(g->cursory-(int)y)<1)
            //  printf("%d/%d: %f %f -> %f %f (%f %f)\n",pathidx,path.size(),x,y,px,py,dx,dy);
        } else
            state=FINEPATH;
        break;
    case FINEPATH:
        {
            float deltax = (destx-x);
            float deltay = (desty-y);
            if(deltax*deltax+deltay*deltay < 0.001f){
                dx=dy=0;
                //                printf("%f %f\n",x,y);
                state = DEBUGSTOP;
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
}    


void Person::update(float t){
    Grid *g = &globals::game->grid;
    
    if(globals::timeNow > nextInfrequentUpdate){
        nextInfrequentUpdate = globals::timeNow + INFREQUENTUPDATEINTERVAL;
        updateInfrequent();
    }
    
    // adjustment for diagonal speed slowdown
    float diag = (dx && dy) ? 0.707107f : 1;
    
    x+=PERSONSPEED*t*(float)dx*diag;
    y+=PERSONSPEED*t*(float)dy*diag;
    
    // get grid coords
    int ix = (int)x;
    int iy = (int)y;
    
    if(!(*g)(ix,iy)){ // uses the operator() used by pathing elsewhere
        // something's gone wrong - move us but deal with the bad square
        if(g->getinterp(x,y)<0.5f){ // use the actual height where we are, interpolating between the corners
            // we're in the sea!
            drowntime+=t;
            if(drowntime>DROWNSURVIVALTIME){
                state = ZOMBIE; // "Like tears in the rain. Time to die."
            } else {
                path.clear(); // our path is useless, it goes into the sea
                state = WANDER;
            }
        }
    }
     
    g->mapsteps[ix][iy]++;
    
    if(x<0)x=0;
    if(x>=GRIDSIZE-1)x=GRIDSIZE-2;
    if(y<0)y=0;
    if(y>=GRIDSIZE-1)y=GRIDSIZE-2;
}
