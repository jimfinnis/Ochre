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

static int idxct=0;
Player::Player() : people(MAXPOP), houses(MAXHOUSES){
    idx=idxct++;
    float basex,basey;
    
    if(idx){
        basex=basey=GRIDSIZE-30;
    } else {
        basex=basey=30;
    }
        
    for(int i=0;i<400;i++){
        float x = drand48()*20+basex;
        float y = drand48()*20+basey;
        Person *p = people.alloc();
        if(!p)break;
        p->init(this,i,y,x);
    }
    
    memset(potential,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    memset(potentialClose,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    
    blur = new MultipassBlur(GRIDSIZE,GRIDSIZE,GRIDSIZE/4);
    blurClose = new MultipassBlur(GRIDSIZE,GRIDSIZE,2);
    
    mode = PLAYER_SETTLE;
    anchorX=anchorY=-1;
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
    

void Player::update(float t){
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    float potentialTmp[GRIDSIZE][GRIDSIZE];
    memset(potentialTmp,0,GRIDSIZE*GRIDSIZE*sizeof(float));
    
    
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
        potentialTmp[p->x][p->y]=2; // houses are more er.. targety.
        if(!p->pop || p->zombie){ // houses die when their population hits zero
            houses.free(p);
        }
    }
    
    // blur the potential field
    profbar.start("B",0xff8080ff);
    blur->pass((float*)potentialTmp,(float*)potential);
    blurClose->pass((float*)potentialTmp,(float*)potentialClose);
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
