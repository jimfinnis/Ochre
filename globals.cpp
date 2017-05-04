/**
 * @file globals.cpp
 * @brief  Brief description of file.
 *
 */

#include "globals.h"
#include "font.h"

Font *globals::font = NULL;
Grid *globals::grid = NULL;
bool globals::running = true;
int globals::cursorx,globals::cursory;

// initialise those globals which need it
void globals::init(){
    font = new Font("media/fonts/Quicksand-Regular.otf",100);
}
