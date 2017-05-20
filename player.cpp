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

Player::Player() : people(1024){
//    for(int i=0;i<3;i++){
    for(;;){
        Person *p = people.alloc();
        if(!p)break;
        p->init(this,drand48()*20+20,drand48()*20+20);
    }
    mode = PLAYER_SETTLE;
}

void Player::render(){
    // draw all the little folk.

    Game *game = globals::game;
    Grid *g = &game->grid;
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();

    meshes::marker->startBatch();
    State *s = sm->get();
    s->overrides |= STO_DIFFUSE;
    for(Person *p=people.first();p;p=people.next(p)){
        float opacity = g->getVisibility(p->x,p->y);
        if(opacity>0.001){
            g->pushxforminterp(p->x,p->y,-0.2f);
            ms->rotY(p->getrot()+glm::radians(90.0f));
            ms->scale(0.2);
            s->diffuse = Colour(1,1-p->stuckHormone,1-p->stuckHormone,opacity);
            meshes::marker->render(ms->top());
            ms->pop();
        }
    }
}

void Player::update(float t){
    for(Person *q,*p=people.first();p;p=q){
        q=people.next(p);
        p->update(t);
        if(p->pmode == ZOMBIE)people.free(p);
    }
}
