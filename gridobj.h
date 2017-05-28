/**
 * @file gridobj.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GRIDOBJ_H
#define __GRIDOBJ_H

#include "gfx.h"

/// fixed objects on the grid (house, etc) derive from this

class GridObj {
public:
    virtual void queueRender(glm::mat4 *world) = 0;
};

#endif /* __GRIDOBJ_H */
