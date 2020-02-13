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
#include "logger.h"

#include "spiral.h"

//#define BASEPERSONSPEED 3.1f
#define BASEPERSONSPEED 3.1f

// was 8 - this is the amount the opponent field attracts in attack
#define OPPONENT_FIELD_ATTACK 10.0f
// was 8 - this is the amount the opponent field repels in other
#define OPPONENT_FIELD_SETTLE -5.0f
// was 8 - this is the amount the opponent field attracts in attack
#define OPPONENT_FIELD 10.0f
// the amount we are repelled by our own kind in attack mode
#define SELF_FIELD_ATTACK 0.1f
// the amount we are repelled by our own kind otherwise
#define SELF_FIELD_OTHER 1.0f

// how likely we are to make a house in settle mode
// This is an N-sided dice roll, so 10 is one chance in 10.
#define HOUSE_CHANCE_SETTLE 1
// how likely we are to make a house in other modes
// This is an N-sided dice roll, so 10 is one chance in 10.
#define HOUSE_CHANCE_OTHER 20


// this is the amount by which field is artifically skewed in the
// direction of the anchor. It should be very small under normal
// conditions and larger under PLAYER_COLLECT (i.e. move towards anchor)
// TAKE CARE - MULTIPLICATIVE, NOT ADDITIVE like the others.
#define ANCHOR_BUMP_NORMAL 1.0f
#define ANCHOR_BUMP_COLLECT 0.99f

// how big a merged pair can be
#define PERSON_MERGE_LIMIT 10

// search range for pathing to enemy
#define ENEMY_SEARCH_RANGE 10

// search range for pathing to house
#define ENEMY_HOUSE_SEARCH_RANGE 10


static SpiralSearch spiral;

static void genName(char *name,int pl){
    static char cons[] = "tgbnrylmnttssfvc";
    static char vwls[] = "aeiuow";
    char *b = name;
    
    *b++ = cons[rand()%strlen(cons)];
    *b++ = vwls[rand()%strlen(vwls)];
    *b++ = cons[rand()%strlen(cons)];
    *b++ = vwls[rand()%strlen(vwls)];
    *b++ = cons[rand()%strlen(cons)];
    *b++ = vwls[rand()%strlen(vwls)];
    *b=0;
    if(pl){
        while(*name){*name = toupper(*name);name++;}
    }
}


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
    pendDamage=0;
    p=player;
    strength=1;
    drowntime=0;
    walkCycle=0;
    speed = globals::rnd->range(0.9f,1.1f)*BASEPERSONSPEED;
    nextInfrequentUpdate = globals::timeNow+INFREQUENTUPDATEINTERVAL*0.2*(double)(idx%10);
    
    genName(name,player->idx);
}

Person *Person::locateEnemy(){
    int ix = (int)x;
    int iy = (int)y;
    Grid *g = &globals::game->grid;
    
    for(spiral.start();spiral.layer<ENEMY_SEARCH_RANGE;spiral.next()){
        for(Person *pers = g->getPeople(ix+spiral.x,iy+spiral.y);
            pers;pers=pers->next){
            if(pers->p != p){
                return pers;
            }
        }
    }
    return NULL;
}

House *Person::locateEnemyHouse(){
    int ix = (int)x;
    int iy = (int)y;
    Grid *g = &globals::game->grid;
    
    for(spiral.start();spiral.layer<ENEMY_SEARCH_RANGE;spiral.next()){
        GridObj *o = g->getObject(ix+spiral.x,iy+spiral.y);
        if(o && o->type == GO_HOUSE ){
            House *h = (House*)o;
            if(h->p != p)
                return h;
        }
    }
    return NULL;
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
    float opponentRepel = (mode==PLAYER_SETTLE)?
          OPPONENT_FIELD_SETTLE:-OPPONENT_FIELD_ATTACK;
    float selfRepel = (mode==PLAYER_ATTACK)?
          SELF_FIELD_ATTACK:SELF_FIELD_OTHER;
    float anchorBump = (mode==PLAYER_COLLECT)?
          ANCHOR_BUMP_COLLECT:ANCHOR_BUMP_NORMAL;
    
    opponentRepel=0;
    selfRepel=0;
    
    int oxf,oyf;
    for(int ox=-1;ox<=1;ox++){
        for(int oy=-1;oy<=1;oy++){
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
                float st = globals::rnd->range(0.0f,10.5f);
                // apply the potential fields. First, we are repelled by
                // our own kind.
                st += p->potential[xx][yy]*selfRepel;
                // and either repelled or attracted by others, with
                // the effect 5 times stronger on the less-blurred
                // field.
                st += (p->op->potentialClose[xx][yy]*5.0f+
                       p->op->potential[xx][yy])*opponentRepel;
                
                // and a bump if this is in the anchor direction
                if(ox==targetdx || oy==targetdy)
                    st *= anchorBump;
                
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

double Person::getChanceOfWinningAttack(Person *defender){
    if(!defender->strength)return 1; // if he's a corpse we always win!
    double myStr = strength;
    double theirStr = defender->strength;
    
    double diff = myStr-theirStr;
    
    // we use the logistic sigmoid here. Yeah, a lookup would be quicker.
    return 1.0/(1.0+exp(-diff));
}


void Person::updateInfrequent(){
    // various infrequent things - repathing, picking a fight,
    // turning into a house etc.
    
    Grid *g = &globals::game->grid;
    int ix = (int)x;
    int iy = (int)y;
    
    PlayerMode pmode = p->getMode();
    
    if(state==COARSEPATH && pmode!=PLAYER_COLLECT){
        // we might be randomly pathing to the anchor. We might just
        // go off to pick daisies, or decide the Crusades are Not For Us.
        if(!globals::rnd->getInt(100))
            state = WANDER;
    }
    
    
    if(/*state==WANDER && */!globals::rnd->getInt(4)){
        /* if(pmode==PLAYER_ATTACK) */
        {
            // look for a house or a player
            
            if(globals::rnd->getInt(2)){
                if(Person *p = locateEnemy()){
                    // make sure we can get a safe line to the other
                    if(g->isLineSafe(x,y,p->x,p->y)){
                        target = p;
                        state = HUNT;
                    }
                }
            } else {
                if(House *h = locateEnemyHouse()){
                    pathTo(h->x,h->y);
                }
            }
        }
    }
    
    
    
    
    /*
     * Turning into a house
     */
    
    // are we on a flat square? Which is OK?
    if(!g->objects[ix][iy] &&  // no objects in the way
       (*g)(ix,iy) && // safe
       g->get(ix,iy) && // quick flat check
       !globals::rnd->getInt(p->getMode() == PLAYER_SETTLE?HOUSE_CHANCE_SETTLE:HOUSE_CHANCE_OTHER) && // basic chance
       g->isFlatGrass(ix,iy)){ // flat
        // work out a chance we'll actually do it, based on
        // how big the house will be. Big houses are more likely!
        int c = g->countFlatGrass(ix,iy);
        if(globals::rnd->getInt(6)<=c) { // higher chance if bigger result
            // make a new house if we can
            House *h = p->houses.alloc();
            if(h){
                // note - player total population doesn't change
                h->init(ix,iy,p);
                h->pop = strength;
                //            printf("House added at %d,%d  %p\n",ix,iy,p);
                // "kill" the villager (he is now the houseowner and moves
                // into the house)
                state = ZOMBIE; 
                globals::log->p(LOG_POP,"%s is making a house!",name);
            }
        }
    }
    
    /*
     * Picking a fight with a person, OR merging with a person!
     */
    
    for(Person *pp = globals::game->grid.getPeople(ix,iy);pp;pp=pp->next){
        if(pp != this && !pp->state==ZOMBIE){ // can't merge or fight zombies
            if(pp->p == p){ // same player as me
                // we sometimes merge with the other person, if the
                // combined strength would be sane.
                if(pp->strength + strength < PERSON_MERGE_LIMIT && !(rand()%3)){
                    // note - player total population doesn't change
                    pp->state = ZOMBIE;
                    strength += pp->strength;
                    globals::log->p(LOG_POP,"Merge! %s is now part of %s with str=%d",pp->name,name,strength);
                }
            } else { // different player
                // Fight! The chance of winning is based on strength;
                // the loser loses a strength point. Yes, it's heavily
                // weighted to the higher number.
                
                double chance = getChanceOfWinningAttack(pp);
                if(drand48()<chance)
                    pp->damage(1);
                else
                    damage(1);
            }
        }
    }
    
    /*
     * Picking a fight with a house!
     */
    
    GridObj *obj=globals::game->grid.getObject(ix,iy);
    if(obj && obj->type == GO_HOUSE){
        House *h = (House *)obj;
        if(h->p != p && !h->zombie){ // can't attack zombie houses; that's silly.
            // FIGHT HOUSE - similar logic to the above, but houses
            // are tougher.
            
            if(rand()%3){ // more likely attacker will be damaged
                damage(1);
            } else {
                h->damage(1);
            }
        }
    }
    
    // repath - this runs infrequently.
    
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
            if(deltax*deltax+deltay*deltay < 0.1f){
                dx=dy=0;
                state = WANDER;
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
    
    // deal with pending damage
    if(pendDamage>0){
        strength -= pendDamage;
        p->decPop(pendDamage); //  decrease player total pop.
        pendDamage=0;
        globals::log->p(LOG_POP,"Decrement in person damage to %s, person now %d",name,strength);
        if(strength<=0){
            globals::log->p(LOG_POP,"Person %s has ceased to be",name);
            strength=0;
            state = ZOMBIE;
            return; // terminate update here
        }
    }

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
    } else {
        // very rarely, a person in another mode might do this.
        if(p->anchorX>=0 && drand48()<0.0001){
            pathTo(p->anchorX,p->anchorY);
        }
    }
    
    
    if(state == HUNT){
        if(target->state == ZOMBIE)
            state = WANDER;
        else {
            dx = sgn(target->x - x);
            dy = sgn(target->y - y);
        }
    }
    
    // adjustment for diagonal speed slowdown
    float diag = (dx && dy) ? 0.707107f : 1;
    
    x+=speed*t*(float)dx*diag;
    y+=speed*t*(float)dy*diag;
    
    walkCycle += speed*0.01f;
    
    // get grid coords
    int ix = (int)x;
    int iy = (int)y;
    
    if(!(*g)(ix,iy)){ // uses the operator() used by pathing elsewhere
        // something's gone wrong - move us but deal with the bad square
        if(g->getinterp(x,y)<0.5f){ // use the actual height where we are, interpolating between the corners
            // we're in the sea!
            drowntime+=t;
            if(drowntime>DROWNSURVIVALTIME){
                damage(1);
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
    pendDamage+=n;
}


