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
    MatrixStack *ms = StateManager::getInstance()->getx();
    
    meshes::marker->startBatch();
    
    for(Person *p=people.first();p;p=people.next(p)){
        // this might go wrong at edges due to rounding, see how it looks
        if(g->isVisible(p->x,p->y) && g->isVisible(p->x,p->y+1)){
            g->pushxforminterp(p->x,p->y,-0.2f);
            ms->rotY(p->getrot());
            ms->scale(0.2);
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
