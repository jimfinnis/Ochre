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

Player::Player() : people(256){
    // we'll just test with one person
    Person *p = people.alloc();
    p->init(24,24);
}

void Player::render(){
    // draw all the little folk.
    
    Game *game = globals::game;
    Grid *g = &game->grid;
    MatrixStack *ms = StateManager::getInstance()->getx();
    
    for(Person *p=people.first();p;p=people.next(p)){
        // this might go wrong at edges due to rounding, see how it looks
        if(g->isVisible(p->x,p->y)){
            g->pushxforminterp(p->x,p->y,0.0f);
            ms->rotY(p->rot);
            // RENDER HERE
            ms->pop();
        }
    }
    
}
