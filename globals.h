/**
 * @file globals.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

/// max number of people for each player
#define MAXPOP 2048
/// max houses for each player
#define MAXHOUSES 65536
/// map size
#define GRIDSIZE 512


class Font;
class Game;
class RandomNumberGenerator;

namespace globals {
extern Font *font;
extern bool running;
extern Game *game;
extern double timeNow; // to save time calling now()
void init();
extern class RandomNumberGenerator *rnd; // use only for dumb things.
}

#endif /* __GLOBALS_H */
