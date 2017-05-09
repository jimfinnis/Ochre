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
    int dx,dy; // direction
    
    Person(){
    }
    
    void init(float xx,float yy){
        x=xx;y=yy;
        dx=dy=1;
    }
    
    float getrot(){
        return dirToRot[dx+1][dy+1];
    }
    
    void update(float t);
    
    static void initConsts();
    
private:
    static float dirToRot[3][3];
};

#endif /* __PERSON_H */
