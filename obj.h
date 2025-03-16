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
#include "state.h"
#include <vector>
#include <iostream>

using namespace std;

class ObjMesh {
    Material *mats;
    
    struct QueueEntry {
        glm::mat4 world;
        State state; // state when added to queue
        QueueEntry(glm::mat4 *w,State *s){
            world = *w;
            state = *s;
        }
    };
    
    GLuint buffers[2];
    std::vector<Transition> transitions; // material transitions
    std::vector<QueueEntry> queue; // queued renders
    const char *name;
public:
    ObjMesh(const char *dir,const char *name);
    virtual ~ObjMesh();
    
    const char *getName();
    
    // standard render method
    void render(glm::mat4 *world);
    
    // queue a render, and then render the queue using batching.
    // we can override the colour here, but not much else.
    
    void queueRender(glm::mat4 *world){
        State *s = StateManager::getInstance()->get();
        queue.push_back(QueueEntry(world,s));
    }
    
    // render this mesh's queued objects
    void renderQueue();
    
    
    // render all object mesh queues
    static void renderAll();
    
    // batch rendering, generally used internally.
    void startBatch();
    void endBatch();
    void renderInBatch(glm::mat4 *world);
};

#endif /* __OBJ_H */
