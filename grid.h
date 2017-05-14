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

#define GRIDSIZE 256
#define MAXVERTS 32767

// the visibility and opaque arrays are bigger than the grid itself
#define VISBORDER 16

// these are the predefined grid materials

#define GMAT_GRASS 0
#define GMAT_FARM 1


/// this class manages visibility lines - we tell how visible an entity
/// by looking at the minimum of its distance from edges of the viewable
/// region in world space.
struct VisLines {
    struct line {
        // lines are in vector form
        glm::vec2 a; // start of line
        glm::vec2 n; // line normal
        // return the distance squared from the line
        // (0 if on the wrong side)
        float getDistSquared(float x, float y);
    };
    line lines[8];
    int ct;
    void reset(){ct=0;}
    void add(float x1,float y1,float x2,float y2){
        lines[ct].a=glm::vec2(x1,y1);
        lines[ct++].n=glm::normalize(glm::vec2(x2-x1,y2-y1));
    }

    float getVisibility(float x,float y);

};

/// this is the grid - the heightmap of the world and how to render it.

class Grid {
    uint8_t grid[GRIDSIZE][GRIDSIZE];     // height map
    uint8_t gridmats[GRIDSIZE][GRIDSIZE]; // material index for square (x,y,x+1,y+1)
    uint8_t gridsafe[GRIDSIZE][GRIDSIZE];  // is SQUARE x,y,x+1,y+1 entirely safe (for pathing)
    uint8_t mapvis[GRIDSIZE][GRIDSIZE]; // visibility of node

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

    // materials
    std::vector<Material> materials;
    // material transition table for rendering
    std::vector<Transition> transitions;

    // each material has a vector of ints, which store the triangle indices during
    // mesh generation.
    std::vector<std::vector<GLuint>> buckets;

    // we can write the texture to a map, too.
    GLuint maptex;

    // visibility edges structure
    VisLines vis;

    // raise up at x,y (and neighbours if necessary) (internal, does the work)
    void _up(int x,int y);
    // lower at x,y (and neighbours if necessary) (internal, does the work)
    void _down(int x,int y);

public:
    int cursorx,cursory; // selected point
    int centrex,centrey;

    float heightFactor; // y is also multiplied by this

    // stigmergic trace - incremented when a peep enters square (x,y,x+1,y+1) and decays
    // over time. Idea pinched from Populous, so I've kept the name of the variable.
    float mapsteps[GRIDSIZE][GRIDSIZE];


    /// is a given grid square visible
    bool isVisible(int x,int y){
        return true;
    }

    float getVisibility(float x, float y){
        return vis.getVisibility(x,y);
    }

    Grid(int seed,float waterlevel);
    ~Grid();

    inline int get(int x,int y) const{
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return grid[x][y];
        else
            return 0;
    }

    // called every update tick.
    void update(float t);

    // call this every time the terrain changes to recalculate
    // the safe grid squares for pathing.
    void recalcSafe();

    // this is required for JPS pathing library - uses the gridsafe array
    // to see if a square is safe to walk on
    inline bool operator()(unsigned x, unsigned y) const {
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return gridsafe[x][y];
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
    void up(int x,int y){
        if(grid[x][y]<15){ // have to draw the line somewhere..
            _up(x,y);
            recalcSafe();
        }
    }
    // lower at x,y (and neighbours if necessary)
    void down(int x,int y){
        _down(x,y);
        recalcSafe();
    }

    // draw the map (will require people to be added)
    void writeMapTexture();
    void drawHouses();

    // move cursor and move centre if required
    void moveCursor(int dx,int dy){
        cursorx+=dx;
        cursory+=dy;
        if(getVisibility(cursorx,cursory)<0.5){
            centrex+=dx;centrey+=dy;
        }
    }

};


#endif /* __GRID_H */
