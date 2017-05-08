/**
 * @file person.h
 * @brief  Brief description of file.
 *
 */

#ifndef __PERSON_H
#define __PERSON_H

/// one of the little people in the game - allocated from a pool,
/// ctor and dtor will run.

struct Person {
    float x,y; // note these are in gridspace
    float rot;       // facing direction so we can render and move
    
    Person(){
        rot=0;
    }
    
    void init(float xx,float yy){
        x=xx;y=yy;
    }
    
};

#endif /* __PERSON_H */
