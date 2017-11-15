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
// was 8
#define OPPONENT_FIELD 10.0f

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
    strength=1;
    drowntime=0;
    nextInfrequentUpdate = globals::timeNow+INFREQUENTUPDATEINTERVAL*0.2*(double)(idx%10);
}


bool Person::pathTo(float xx,float yy){
    path.clear();
    if(JPS::findPath(path,globals::game->grid,(int)x,(int)y,(int)xx,(int)yy)){
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

void Person::setDirectionFromPotentialField(){
    PlayerMode mode = p->getMode();
    Grid *g = &globals::game->grid;
    int cx = (int)x;
    int cy = (int)y;
    
    int tx,ty;
    
    if(p->anchorX<0){
        tx = ty = GRIDSIZE/2;
    } else {
        tx = p->anchorX; ty = p->anchorY;
    }
    
    int targetdx = sgn(tx-x);
    int targetdy = sgn(ty-y);
    
    int idx=sgn(dx); // blee.
    int idy=sgn(dy);
    
    float minst = FLT_MAX;
    float opponentRepel = (mode==PLAYER_SETTLE)?OPPONENT_FIELD:-OPPONENT_FIELD;
    float anchorBump = (mode==PLAYER_COLLECT)?0.1:0.7;
    
    int oxf,oyf;
    for(int ox=-1;ox<=1;ox++){
        for(int oy=-1;oy<=1;oy++){
            float st=1000;
            int xx=cx+ox;
            int yy=cy+oy;
            
            // do not scan my own area, do not permit us to turn around
            // or go into the sea. The middle rule there is to avoid
            // stuckage.
            if( (ox||oy)  // my own square
                && (ox!=-idx && oy!=-idy) &&  // turning around
                (*g)(xx,yy)) // safe square (uses the operator() JPS uses for pathing)
            {
                // add a bit of random to the field
                st = globals::rnd->range(0.0f,0.05f);
                
                // apply the potential fields. First, we are repelled by
                // our own kind.
                st += p->potential[xx][yy];
                // and either repelled or attracted by others, with
                // the effect 5 times stronger on the less-blurred
                // field.
                st += (p->op->potentialClose[xx][yy]*5.0f+
                       p->op->potential[xx][yy])*opponentRepel;
                
                // and a bump if this is in the anchor direction
//           if(ox==targetdx || oy==targetdy)
//                    st *= anchorBump;
                
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
       g->isFlatGrass(ix,iy)){ // flat
        // work out a chance we'll actually do it, based on
        // how big the house will be
        int c = g->countFlatGrass(ix,iy);
        if(globals::rnd->getInt(6)<=c) { // higher chance if bigger result
            // make a new house if we can
            House *h = p->houses.alloc();
            if(h){
                h->init(ix,iy,p);
                h->pop = strength;
                //            printf("House added at %d,%d  %p\n",ix,iy,p);
                // "kill" the villager (he is now the houseowner and moves
                // into the house)
                state = ZOMBIE; 
            }
        }
    }
    
    // PICK A FIGHT!
    for(Person *pp = globals::game->grid.getPeople(ix,iy);pp;pp=pp->next){
        if(pp != this){
            if(pp->p == p){ // same player as me
                // we sometimes merge with the other person, if the
                // combined strength would be sane.
                if(pp->strength + strength < 10 && !(rand()%3)){
                    pp->state = ZOMBIE;
                    strength += pp->strength;
                }
            } else { // different player
                // Fight! There's a 50% chance of either player losing
                // a strength point; and if you get to zero you die.
                if(rand()%2)
                    damage(1);
                else
                    pp->damage(1);
            }
        }
    }
    GridObj *obj=globals::game->grid.getObject(ix,iy);
    if(obj && obj->type == GO_HOUSE){
        House *h = (House *)obj;
        if(h->p != p){
            // FIGHT HOUSE - similar logic to the above, but houses
            // are tougher.
            
            if(rand()%3){ // more likely attacker will be damaged
                damage(1);
            } else {
                h->damage(1);
            }
        }
    }
    
    switch(state){
    case WANDER:
        setDirectionFromPotentialField();
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
    PlayerMode mode = p->getMode();
    if(globals::timeNow > nextInfrequentUpdate){
        nextInfrequentUpdate = globals::timeNow + INFREQUENTUPDATEINTERVAL;
        updateInfrequent();
    }
    
    // every now and then, if we are heading to an anchor, try to
    // path to it. Otherwise wandering will draw us there. Note that
    // this will cause all villagers to repath.
    if(mode == PLAYER_COLLECT){
        if(drand48()<0.01){
            pathTo(p->anchorX,p->anchorY);
        }
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
    
    if(x<0)x=0;
    if(x>=GRIDSIZE-1)x=GRIDSIZE-2;
    if(y<0)y=0;
    if(y>=GRIDSIZE-1)y=GRIDSIZE-2;
}

void Person::damage(int n){
    strength -= n;
    if(strength<0){
        strength=0;
        state = ZOMBIE;
    }
}
        
