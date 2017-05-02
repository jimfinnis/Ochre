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
    
    void renderTex(Matrix *world);
    void renderUntex(Matrix *world);
    
public:
    ObjMesh(const char *dir,const char *name);
    virtual ~ObjMesh();
    
    // standard render method
    virtual void render(Matrix *world);
};

#endif /* __OBJ_H */
