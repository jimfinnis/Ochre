/**
 * @file player.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"
#include "game.h"
#include "globals.h"
#include "meshes.h"
#include "prof.h"

#include "time.h"
#include "logger.h"
#include "spiral.h"

static int idxct=0;
Player::Player() : people(MAXPOP), houses(MAXHOUSES){
    idx=idxct++;
    float basex,basey;
    
    if(idx){
        basex=basey=GRIDSIZE-30;
    } else {
        basex=basey=30;
    }
    
    // set up initial peeps
    
    for(int i=0;i<4;i++){
        int x = basex+rand()%20;
        int y = basey+rand()%20;
        if(!spawn(x,y,1))break;
    }
    
    memset(potential,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    memset(potentialClose,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    
    blur = new MultipassBlur(GRIDSIZE,GRIDSIZE,GRIDSIZE/4);
    blurClose = new MultipassBlur(GRIDSIZE,GRIDSIZE,2);
    
    mode = PLAYER_ATTACK;
    anchorX=anchorY=-1;
    nextAutolevelTime=Time::now()+AUTOLEVELDELAY; // autolevelling doesn't happen for a bit
    levelx=levely=-1;
}

Player::~Player(){
    delete blur;
    delete blurClose;
}

float snark = 3.2;
void Player::renderPerson(Person *p){
    Grid *g = &globals::game->grid;
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    
    meshes::marker->startBatch();
    //    printf("Snark %f\n",snark);
    g->pushxforminterp(p->x,p->y,snark); // grid pos
    ms->scale(0.2);
    ms->rotY(p->getSmoothedRot());
    
    
    
    // render body and head
    ms->push();
    ms->push();
    ms->scale(1,1,0.6); // body
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    ms->scale(0.5);
    ms->translate(0,1.8,0); // head
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    
    //arms
    
    float armswing = p->walkCycle*4;
    float legswing = p->walkCycle*6;
    static const float legswingamount=0.2;
    static const float armswingamount=0.4f;
    static const float armheight=0.1;
    
    
    ms->push(); //  arm
    ms->translate(1.2,-armheight,0);
    ms->rotX(sinf(armswing)*armswingamount);
    ms->scale(0.2,1,0.3);
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    
    ms->push(); //  arm
    ms->translate(-1.2,-armheight,0);
    ms->rotX(-sinf(armswing)*armswingamount);
    ms->scale(0.2,1,0.3);
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    
    // legs
    static const float leglength = 0.6;
    static const float legsep = 0.5;
    ms->push();
    ms->translate(0,-2,0);
    
    ms->push(); // leg
    ms->translate(legsep,0,0);
    ms->rotX(sinf(legswing)*legswingamount);
    ms->scale(0.2,leglength,0.3);
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    
    ms->push(); // leg
    ms->translate(-legsep,0,0);
    ms->rotX(-sinf(legswing)*legswingamount);
    ms->scale(0.2,leglength,0.3);
    meshes::toppivotcube->render(ms->top());
    ms->pop();
    
    ms->pop();
    
    
    
    
    ms->pop();    // grid pos
}

void Player::render(const Colour& col){
    // draw all the little folk.
    
    Game *game = globals::game;
    Grid *g = &game->grid;
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    
    sm->push();
    State *s = sm->get();
    s->diffuse = col;
    s->overrides |= STO_DIFFUSE;
    
    
    // try to render people nicely
    for(Person *p=people.first();p;p=people.next(p)){
        float opacity = g->getVisibility(p->x,p->y);
        if(opacity>0.001){
            s->diffuse.a = opacity;
            renderPerson(p);
        }
    }
    
    
    
    // draw the anchor if set
    if(anchorX>=0){
        float opacity = g->getVisibility(anchorX,anchorY);
        if(opacity>=0.001){
            s->diffuse.a = opacity;
            g->pushxforminterp(anchorX,anchorY,-0.2f);
            ms->scale(1);
            meshes::marker->render(ms->top());
            ms->pop();
        }
    }
    
    sm->pop();
}

void Player::updateHouseTerrain(){
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    for(House *p=houses.first();p;p=houses.next(p)){
        if(p->size<255)
            g->setTerrainAroundIfGrass(p->x,p->y,p->size+1,GTERR_FARM,idx);
    }
    // later code will remove non-flat farms
}    

void Player::setMode(PlayerMode m){
    mode = m;
    resetToWander();
}

void Player::autoLevel(){
    RandomSpiralSearch spiral;
    globals::log->p(LOG_LEVEL,"At start %d,%d",levelx,levely);
    
    // we assume that the "god" has a view of a particular location, and will always
    // level close to that location. It starts at the average location of all the people.
    double xx=0,yy=0;
    if(levelx<0){ // need to reinit.
        double n=0;
        for(Person *p=people.first();p;p=people.next(p)){
            xx+=p->x;yy+=p->y;n++;
        }
        if(n>0.000001){
            xx/=n;yy/=n;
            globals::log->p(LOG_LEVEL,"%f %f - %f",xx,yy,n);
        } else {
            return; // shouldn't happen, there should be some players
        }
        levelx=(int)(xx+0.5);
        levely=(int)(yy+0.5);
    }
    
    // now find a candidate to level. What's the simplest thing that can possibly work?
    // We just pull the location towards sea level+1.
    
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    // get the current height and modify it
    int h = g->get(levelx,levely);
    globals::log->p(LOG_LEVEL,"Levelling at %d,%d: height is %d",levelx,levely,h);
    if(h<1){
        g->up(levelx,levely);
    }
    else if(h>1){
        g->down(levelx,levely);
    }
    
    // now get the next location by looking around for something which isn't at sea level
    // but is next to something which is.
    bool found=false;
    for(spiral.start();spiral.layer<50;spiral.next()){
        int gx = levelx+spiral.x;
        int gy = levely+spiral.y;
        if(g->in(gx,gy)){
            h = g->get(gx,gy);
            if(h!=1 && g->nextToDry(gx,gy,3)){
                globals::log->p(LOG_LEVEL,"Found at %d,%d",gx,gy);
                levelx=gx;
                levely=gy;
                found=true;
                break;
            }
        }
    }
    if(!found){
        globals::log->p(LOG_LEVEL,"Not found");
        // damn, everything's level nearby. Delay for random seconds and restart
        nextAutolevelTime=Time::now()+(rand()%2)+2;
        levelx=-1;
    } else if(!(rand()%100))
        levelx=-1; // just randomly reset now and then
    
    
    globals::log->p(LOG_LEVEL,"At end %d,%d",levelx,levely);
}

// time passed in is interval since last update
void Player::update(double t){
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    float potentialTmp[GRIDSIZE][GRIDSIZE];
    memset(potentialTmp,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    
    int housePop=0;
    int totalPop=0;
    
    globals::log->p(LOG_POP,"Start of update: player %d pop = %d",idx,pop);
    
    // update people and add them to the potential field
    for(Person *q,*p=people.first();p;p=q){
        q=people.next(p);
        p->update(t);
        potentialTmp[(int)p->x][(int)p->y]=1;
        if(p->state == ZOMBIE)
            people.free(p);
        else {
            totalPop+=p->strength;
            globals::log->p(LOG_POP,"Person %d/%d (%s): %d",people.getidx(p),idx,p->getName(),p->strength);
        }
        
    }
    // update houses and add them to the potential field
    for(House *q,*p=houses.first();p;p=q){
        q=houses.next(p);
        p->update(t);
        potentialTmp[p->x][p->y]=2; // houses are more er.. targety.
        if(!p->pop || p->zombie){ // houses die when their population hits zero
            houses.free(p);
        } else {
            housePop+=p->pop;
            globals::log->p(LOG_POP,"House %d/%d: %d",houses.getidx(p),idx,p->pop);
        }
        
    }
    if(globals::log->has(LOG_POP)){
        globals::log->p(LOG_POP,"End of update:");
        if(housePop+totalPop == pop)
            globals::log->p(LOG_POP,"Player %d pop = H%d/P%d = %d",
                   idx,housePop,totalPop,pop);
        else
            globals::log->p(LOG_POP,"Player %d pop = H%d/P%d = %d (WRONG)",
                   idx,housePop,totalPop,pop);
    }
    // perform any auto-levelling
    
    if(Time::now() > nextAutolevelTime){
        autoLevel();
        double interval = drand48()*(AUTOLEVELMAXINTERVAL-AUTOLEVELMININTERVAL)+AUTOLEVELMININTERVAL;
        nextAutolevelTime=Time::now()+interval;
    }
    
    // blur the potential field
    profbar.start("B",0xff8080ff);
    blur->pass((float*)potentialTmp,(float*)potential);
    blurClose->pass((float*)potentialTmp,(float*)potentialClose);
    profbar.end();
    
}

int Player::spawn(int x,int y,int n){
    float fx = ((float)x)+0.5f;
    float fy = ((float)y)+0.5f;
    int ct=0;
    
    for(int i=0;i<n;i++){
        if(canIncPop()){
            Person *p = people.alloc();
            if(!p)break; // ran out of slots!
            float px = fx + (drand48()-0.5);
            float py = fy + (drand48()-0.5);
            p->init(this,people.getidx(p),px,py);
            incPop();
            globals::log->p(LOG_POP,"New person %s, increment in spawn",p->getName());
            ct++;
        } else break;
    }
    return ct;
}
