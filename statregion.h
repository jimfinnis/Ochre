/**
 * @file statregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __STATREGION_H
#define __STATREGION_H

#include "region.h"

class StatRegion : public Region {
public:
    StatRegion() : Region("stat"){};
    virtual ~StatRegion(){};
    
    virtual void onMouseMove(int x,int y);
    virtual void render();
};


#endif /* __STATREGION_H */
