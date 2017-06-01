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

static int idxct=0;
Player::Player() : people(MAXPOP), houses(MAXHOUSES){
    idx=idxct++;
    float basex,basey;
    if(idx){
        basex=basey=GRIDSIZE-30;
    } else {
        basex=basey=30;
    }
        
    for(int i=0;i<4;i++){
        float x = drand48()*20+basex;
        float y = drand48()*20+basey;
        Person *p = people.alloc();
        if(!p)break;
        p->init(this,i,y,x);
    }
    
    for(int y=0;y<GRIDSIZE;y++){
        for(int x=0;x<GRIDSIZE;x++){
            mapsteps[x][y]=drand48();
        }
    }
    
    memset(potential,1,GRIDSIZE*GRIDSIZE*sizeof(float));
    blur = new MultipassBlur(GRIDSIZE,GRIDSIZE,10);
    
    mode = PLAYER_SETTLE;
    wanderX = GRIDSIZE/2;
    wanderY = GRIDSIZE/2;
}

Player::~Player(){
    delete blur;
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

    meshes::marker->startBatch();
    for(Person *p=people.first();p;p=people.next(p)){
        float opacity = g->getVisibility(p->x,p->y);
        if(opacity>0.001){
            g->pushxforminterp(p->x,p->y,-0.2f);
            ms->rotY(p->getrot()+glm::radians(90.0f));
            ms->scale(0.2);
            meshes::marker->render(ms->top());
            ms->pop();
        }
    }
    sm->pop();
}

void Player::update(float t){
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    float potentialTmp[GRIDSIZE][GRIDSIZE];
    memset(potentialTmp,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    
    for(int y=0;y<GRIDSIZE;y++){
        for(int x=0;x<GRIDSIZE;x++){
            mapsteps[x][y]*=0.998f;
            // add a little noise to mess things up a bit, and stop the
            // little sods making lawnmower stripes
            //      mapsteps[x][y]+=drand48()*0.001f;
        }
    }
    
    
    // update people and add them to the potential field
    for(Person *q,*p=people.first();p;p=q){
        q=people.next(p);
        p->update(t);
        potentialTmp[(int)p->x][(int)p->y]=1;
        if(p->state == ZOMBIE)people.free(p);
    }
    // update houses and add them to the potential field
    for(House *q,*p=houses.first();p;p=q){
        q=houses.next(p);
        p->update(t);
        potentialTmp[p->x][p->y]=1;
        if(!p->pop){ // houses die when their population hits zero
            houses.free(p);
        }
    }
    
    // blur the potential field
    profbar.start("B",0xff8080ff);
    blur->pass((float*)potentialTmp,(float*)potential);
//    gaussBlur((float*)potentialTmp,(float*)potential,GRIDSIZE,GRIDSIZE,10);
    profbar.end();
    
}

void Player::spawn(int x,int y,int n){
    float fx = ((float)x)+0.5f;
    float fy = ((float)y)+0.5f;
    
    for(int i=0;i<n;i++){
        Person *p = people.alloc();
        if(!p)break; // ran out of slots!
        float px = fx + (drand48()-0.5);
        float py = fy + (drand48()-0.5);
        p->init(this,people.getidx(p),px,py);
    }
}
