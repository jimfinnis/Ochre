/**
 * @file grid.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GRID_H
#define __GRID_H

#include "state.h"
#include "effect.h"
#include "material.h"

#include <vector>

/// the grid component of the world from which the heightmap is generated.

#define GRIDSIZE 128
#define MAXVERTS 32767

// the visibility and opaque arrays are bigger than the grid itself
#define VISBORDER 16

// these are the predefined grid materials

#define GMAT_GRASS 0
#define GMAT_FARM 1

class Grid {
    uint8_t grid[GRIDSIZE][GRIDSIZE];     // height map
    uint8_t gridmats[GRIDSIZE][GRIDSIZE]; // material index for square
    
    
    // vertex data goes in here
    UNLITVERTEX verts[MAXVERTS];
    int vertidxs[MAXVERTS][2]; // positions of verts in grid space
    int vertct;
    
    void initGridVerts(){
        vertct=0;
    }
    
    void addtri(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2,int matidx){
        int i0 = v0-verts;
        int i1 = v1-verts;
        int i2 = v2-verts;
        buckets[matidx].push_back(i0);
        buckets[matidx].push_back(i1);
        buckets[matidx].push_back(i2);
    }
    
    UNLITVERTEX *addvert(int gx,int gy,float x,float y,float z,float u,float v){
        if(vertct>=MAXVERTS)
            FATAL("Too many verts in grid!");
        
        vertidxs[vertct][0] = gx;
        vertidxs[vertct][1] = gy;
        
        UNLITVERTEX *vv = verts+(vertct++);
        vv->x = x;
        vv->y = y*heightFactor;
        vv->z = z;
        vv->u = u;
        vv->v = v;
        return vv;
    }
    
    // IN THIS ORDER UNSEPARATED - created in one call.
    GLuint vbo; // vertex buffer object
    GLuint ibo; // index buffer object
    
    int modcount; // modifications since last resetModCount()
    
    // booleans indicating whether node is visible, set in genTriangles.
    uint8_t visible[GRIDSIZE+VISBORDER*2][GRIDSIZE+VISBORDER*2];
    
    // booleans indicating how "opaque" a node is for drawing people.
    // This is an ugly way to do it.
    uint8_t opacity[GRIDSIZE+VISBORDER*2][GRIDSIZE+VISBORDER*2];
    
    // materials
    std::vector<Material> materials;
    // material transition table for rendering
    std::vector<Transition> transitions;
    
    // each material has a vector of ints, which store the triangle indices during
    // mesh generation.
    std::vector<std::vector<GLuint>> buckets;
    
    // we can write the texture to a map, too.
    GLuint maptex;

public:
    int cursorx,cursory; // selected point
    int centrex,centrey;
    
    float heightFactor; // y is also multiplied by this
    
    /// is a given grid square visible (set by genTriangles)
    bool isVisible(int x,int y){
        return visible[x+VISBORDER][y+VISBORDER]!=0;
    }
    
    /// use bilinear interp to get opacity for people from 
    /// opacity array
    float getOpacity(float x, float y);
    
    Grid(int seed,float waterlevel);
    ~Grid();
    
    inline int get(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return grid[x][y];
        else
            return 0;
    }
    
    inline int getmat(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return gridmats[x][y];
        else
            return 0;
    }
    
    /// get the map texture
    GLuint getMapTex(){
        return maptex;
    }
        
    
    // find point nearest ray from p1 in direction p2.
    int intersect(const glm::vec3& origin, const glm::vec3& ray);
    
    // select a point by index
    void select(int idx);
    // recentre to the currently selected point
    void recentre(){
        if(cursorx >=0 && cursory >=0){
            centrex = cursorx;
            centrey = cursory;
        }
    }
    
    // push transform to this location with height offset
    void pushxform(float x,float y,float offset);
    // as pushxform, but does bilinear interp on heights
    void pushxforminterp(float x,float y,float offset);
    
    /// generate triangles centred around centrex,centrey.
    void genTriangles(int range);
    
    void render(glm::mat4 *world);
    void renderCursor();
    
    void resetModCount(){modcount=0;}
    // return mods since last resetModCount()
    int getModCount(){return modcount;}
    
    // raise up at x,y (and neighbours if necessary)
    void up(int x,int y);
    // lower at x,y (and neighbours if necessary)
    void down(int x,int y);
    
    // draw the map (will require people to be added)
    void writeTexture();
    void drawHouses();
};


#endif /* __GRID_H */
