/**
 * @file house.h
 * @brief  Brief description of file.
 *
 */

#ifndef __HOUSE_H
#define __HOUSE_H

#include "gridobj.h"

struct House : public GridObj {
    /// size of house (determines capacity, and is determined by landscape)
    /// it's the number of clear, flat spaces around me in all directions,
    /// and is typically 0,1,2 or 3. -1 means it shouldn't be there.
    
    int size;
    uint8_t pop; // how many people
    uint16_t x,y; // position
    // when this hits a certain number we increase pop, when pop>=cap
    // we spawn.
    float growcounter; 
    struct Player *p; // my owner
    bool zombie; // kill me with fire
    
    void init(int x,int y,Player *pl);
    void update(float t);
    virtual void queueRender(glm::mat4 *world);
    virtual ~House();
    
    // remove a certain number of people, which may destroy the house. Might be
    // -ve, in which case we do nothing.
    void evict(int n);
};

#endif /* __HOUSE_H */
