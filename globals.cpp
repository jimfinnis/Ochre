/**
 * @file globals.cpp
 * @brief  Brief description of file.
 *
 */

#include "globals.h"
#include "font.h"

namespace globals {
Font *font = NULL;
Game *game = NULL;
bool running = true;
double timeNow = 0; 

// initialise those globals which need it
void init(){
    font = new Font("media/fonts/Quicksand-Regular.otf",20);
}

}
