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

/// max number of people for each player
#define MAXPOP 256

/// max houses for each player
#define MAXHOUSES 256


Player::Player() : people(MAXPOP), houses(MAXHOUSES){
    for(int i=0;i<MAXPOP;i++){
        float x = drand48()*20+20;
        float y = drand48()*20+20;
        Person *p = people.alloc();
        if(!p)break;
        p->init(this,i,y,x);
    }
    
    mode = PLAYER_SETTLE;
    wanderX = GRIDSIZE/2;
    wanderY = GRIDSIZE/2;
}

void Player::render(){
    // draw all the little folk.

    Game *game = globals::game;
    Grid *g = &game->grid;
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();

    meshes::marker->startBatch();
    State *s = sm->get();
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
}

void Player::update(float t){
    Game *game = globals::game;
    Grid *g = &game->grid;
    
    for(Person *q,*p=people.first();p;p=q){
        q=people.next(p);
        p->update(t);
        if(p->state == ZOMBIE)people.free(p);
    }
    for(House *q,*p=houses.first();p;p=q){
        q=houses.next(p);
        p->update(t);
        if(!p->pop){ // houses die when their population hits zero
            houses.free(p);
        }
    }
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
