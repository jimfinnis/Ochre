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
#include "material.h"
#include <vector>
#include <iostream>

using namespace std;

class ObjMesh {
    Material *mats;
    
    struct QueueEntry {
        glm::mat4 world;
        QueueEntry(glm::mat4 *w){
            world = *w;
        }
    };
    
    GLuint buffers[2];
    std::vector<Transition> transitions; // material transitions
    std::vector<QueueEntry> queue; // queued renders
    
public:
    ObjMesh(const char *dir,const char *name);
    virtual ~ObjMesh();
    
    // standard render method
    void render(glm::mat4 *world);
    
    // queue a render, and then render the queue using batching
    void queueRender(glm::mat4 *world){
        queue.push_back(QueueEntry(world));
    }
    
    void renderQueue(){
        startBatch();
        for(auto it = queue.begin();it!=queue.end();++it){
            glm::mat4 *w = &(*it).world;
            renderInBatch(w);
        }
        endBatch();
        queue.clear();
    }
    // render all object mesh queues
    static void renderAll();
    
    // batch rendering, generally used internally.
    void startBatch();
    void endBatch();
    void renderInBatch(glm::mat4 *world);
};

#endif /* __OBJ_H */
