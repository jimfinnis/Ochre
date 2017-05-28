/**
 * @file house.h
 * @brief  Brief description of file.
 *
 */

#ifndef __HOUSE_H
#define __HOUSE_H

#include "gridobj.h"

struct House : public GridObj {
    uint8_t size; // size of house (determines capacity, and is determined by landscape)
    uint8_t pop; // how many people
    uint16_t x,y; // position
    
    struct Player *p; // my owner
    
    void init(int x,int y,Player *pl);
    void update(float t);
    virtual void queueRender(glm::mat4 *world);
    virtual ~House();
    
    // remove a certain number of people, which may destroy the house. Might be
    // -ve, in which case we do nothing.
    void evict(int n);
};

#endif /* __HOUSE_H */
