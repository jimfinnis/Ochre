/**
 * @file obj.h
 * @brief  Brief description of file.
 *
 */

#ifndef __OBJ_H
#define __OBJ_H

/// meshes which come from OBJ files

#include "gfx.h"
#include "maths.h"
#include <vector>

struct Transition{
    int start,count,matidx;
};

class ObjMesh {
    struct Material *mats;
    
    GLuint buffers[2];
    std::vector<Transition> transitions;
    
public:
    ObjMesh(const char *dir,const char *name);
    virtual ~ObjMesh();
    
    // standard render method
    void render(glm::mat4 *world);
    
    // batch rendering
    void startBatch();
    void renderInBatch(glm::mat4 *world);
    void endBatch();
};

#endif /* __OBJ_H */
