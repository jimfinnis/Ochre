/**
 * @file grid.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GRID_H
#define __GRID_H

#include "state.h"
#include "effect.h"

/// the grid component of the world from which the heightmap is generated.

#define GRIDSIZE 256
#define MAXVERTS 16384

class Grid {
    uint8_t grid[GRIDSIZE][GRIDSIZE];
    // vertex data goes in here
    UNLITVERTEX verts[MAXVERTS];
    int vertct;
    void initGridVerts(){
        vertct=0;
    }
    
    UNLITVERTEX *addvert(float x,float y,float z,float u,float v){
        if(vertct==MAXVERTS)
            FATAL("Too many verts in grid!");
        
        UNLITVERTEX *vv = verts+(vertct++);
        vv->x = x*scaleFactor;
        vv->y = y*heightFactor*scaleFactor;
        vv->z = z*scaleFactor;
        vv->u = u;
        vv->v = v;
        return vv;
    }
    
    GLuint vbo; // vertex buffer object
    
    int centrex,centrey; // current centre point
    
public:
    
    float scaleFactor;  // all coords are multiplied by this
    float heightFactor; // y is also multiplied by this
    
    Grid();
    ~Grid();
    
    inline int get(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return grid[x][y];
        else
            return 0;
    }
    
    /// generate triangles centred around cx,cy.
    void genTriangles(int cx,int cy,int range);
    
    void render(glm::mat4 *world);
    void renderHighlight(int x,int y);
};


#endif /* __GRID_H */
