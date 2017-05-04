/**
 * @file screen.cpp
 * @brief  Brief description of file.
 *
 */

#include "screen.h"
#include "context.h"

Screen::Screen(){
    Context::getInstance()->curscreen = this;
}

void Screen::onKeyDown(int k){
    printf("Keycode: %d\n",k);
}

