/**
 * @file gridobj.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GRIDOBJ_H
#define __GRIDOBJ_H

#include "gfx.h"

/// different sorts of grid object
enum GridObjType {GO_HOUSE};

/// fixed objects on the grid (house, etc) derive from this
class GridObj {
public:
    GridObjType type;
    virtual void queueRender(glm::mat4 *world) = 0;
};

#endif /* __GRIDOBJ_H */
