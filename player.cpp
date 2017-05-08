/**
 * @file player.cpp
 * @brief  Brief description of file.
 *
 */

#include "player.h"

Player::Player() : people(256){
    // we'll just test with one person
    Person *p = people.alloc();
    p->init(24,24);
}

