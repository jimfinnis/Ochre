/**
 * @file globals.cpp
 * @brief  Brief description of file.
 *
 */

#include "globals.h"
#include "font.h"

namespace globals {
Font *font = NULL;
Grid *grid = NULL;
bool running = true;
int cursorx,cursory;

// initialise those globals which need it
void init(){
    font = new Font("media/fonts/Quicksand-Regular.otf",20);
}

}
