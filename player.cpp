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

Player::Player() : people(256){
    for(;;){
        Person *p = people.alloc();
        if(!p)break;
        p->init(drand48()*20+20,drand48()*20+20,
                (rand()%3)-1,(rand()%3)-1);
    }
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
        float opacity = g->getOpacity(p->x,p->y);
        if(opacity>0.001){
            g->pushxforminterp(p->x,p->y,-0.2f);
            ms->rotY(p->getrot());
            ms->scale(0.2);
            s->diffuse = Colour(opacity,opacity,opacity,opacity);
            meshes::marker->render(ms->top());
            // RENDER HERE
            ms->pop();
        }
    }
}

void Player::update(float t){
    for(Person *p=people.first();p;p=people.next(p)){
        p->update(t);
    }
}
