/**
 * @file player.cpp
 * @brief  Brief description of file.
 *
 */

#include "player.h"
#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"
#include "grid.h"

Player::Player() : people(256){
    // we'll just test with one person
    Person *p = people.alloc();
    p->init(24,24);
}

void Player::render(){
    // draw all the little folk.
    
    for(Person *p=people.first();p;p=people.next(p)){
        
    }
    
}
