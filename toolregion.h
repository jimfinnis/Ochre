/**
 * @file toolregion.h
 * @brief  Brief description of file.
 *
 */

#ifndef __TOOLREGION_H
#define __TOOLREGION_H

#include "region.h"

class ToolRegion : public Region {
public:
    ToolRegion() : Region("tool"){};
    virtual ~ToolRegion(){};
    
    virtual void onMouseMove(int x,int y);
    virtual void render();
};

#endif /* __TOOLREGION_H */
