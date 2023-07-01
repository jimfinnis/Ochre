/**
 * @file leveller.h
 * @brief  Brief description of file.
 *
 */

#ifndef __LEVELLER_H
#define __LEVELLER_H

class Leveller {
    class Player *p;
    double nextAutolevelTime;
    
public:
    int levelx,levely;
    Leveller(class Player *_p){
        levelx=-1;
        levely=-1;
        p = _p;
    }
    
    void run();
};

#endif /* __LEVELLER_H */
