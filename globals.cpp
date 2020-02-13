/**
 * @file globals.cpp
 * @brief  Brief description of file.
 *
 */

#include "globals.h"
#include "font.h"
#include "maths.h"
#include "logger.h"

namespace globals {
Font *font = NULL;
Game *game = NULL;
Logger *log = NULL;
bool running = true;
double timeNow = 0; 
RandomNumberGenerator *rnd;

// initialise those globals which need it
void init(){
    font = new Font("media/fonts/Quicksand-Regular.otf",20);
    rnd = new RandomNumberGenerator();
    log = new Logger(0);
}

}
