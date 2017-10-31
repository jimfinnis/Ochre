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
#include "house.h"
#include "globals.h"

#include <vector>
#include <math.h>

/// the grid component of the world from which the heightmap is generated.

#define MAXVERTS 32767

// the visibility and opaque arrays are bigger than the grid itself
#define VISBORDER 16

// these are the grid square terrains

#define GTERR_GRASS 0
#define GTERR_FARM 1

// these are the grid graphics materials in the same order in which
// they are added to the materials array

#define GMAT_GRASS 0
#define GMAT_FARMPLAYER0 1
#define GMAT_FARMPLAYER1 2


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
    // so the map texture and various debugging classes can access our innards
    friend class MapTex;
    friend class DebugMapTex;
    
    uint8_t grid[GRIDSIZE][GRIDSIZE];     // height map
    uint8_t gridterr[GRIDSIZE][GRIDSIZE]; // terrain (grass, farm etc) for square (x,y,x+1,y+1)
    uint8_t gridowner[GRIDSIZE][GRIDSIZE]; // if owned (typically farm) which player is it?
    uint8_t gridsafe[GRIDSIZE][GRIDSIZE];  // is SQUARE x,y,x+1,y+1 entirely safe (for pathing)
    uint8_t mapvis[GRIDSIZE][GRIDSIZE]; // visibility of node
    uint8_t isflat[GRIDSIZE][GRIDSIZE]; // flatness of SQUARE x,y,x+1,y+1 
    
    
    class Person *people[GRIDSIZE][GRIDSIZE]; // head of linked list of peeps here
    
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

    // visibility edges structure
    VisLines vis;

    // raise up at x,y (and neighbours if necessary) (internal, does the work)
    void _up(int x,int y);
    // lower at x,y (and neighbours if necessary) (internal, does the work)
    void _down(int x,int y);
    
    float viewRange;
    
    // populate the people lists with this player
    void populatePeople(const Player& p);
    
public:
    int cursorx,cursory; // selected point
    int centrex,centrey;

    float heightFactor; // y is also multiplied by this

    /// an array of pointers to fixed objects
    GridObj *objects[GRIDSIZE][GRIDSIZE];
    
    /// return any fixed object in this square
    GridObj *getObject(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return objects[x][y];
        else
            return NULL;
    }
    
    void addHouse(int x,int y,House *h);
    
    void removeHouse(House *h);
        
    /// is a given grid square visible
    bool isVisible(int x,int y){
        return true;
    }
    
    float getVisibility(float x, float y){
        if(fabsf(x-centrex)>viewRange+2 || fabsf(y-centrey)>viewRange+2)
            return false;
        else
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
    
    // get head of linked list of people in this square, updated
    // every grid update.
    class Person *getPeople(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return people[x][y];
        else
            return NULL;
    }
        

    // called every update tick.
    void update(float t);
    // called to clean up after player house update
    void removeNonFlatFarm(){
        for(int x=0;x<GRIDSIZE;x++){
            for(int y=0;y<GRIDSIZE;y++){
                if(gridterr[x][y]==GTERR_FARM && !isflat[x][y])
                    gridterr[x][y]=GTERR_GRASS;
            }
        }
    }

    // call this every time the terrain changes to recalculate
    // the safe grid squares for pathing, and also to calculate which
    // squares are flat.
    void recalc();

    // this is required for JPS pathing library - uses the gridsafe array
    // to see if a square is safe to walk on
    inline bool operator()(unsigned x, unsigned y) const {
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
            return gridsafe[x][y];
        else
            return 0;
    }
    
    // get the material associated with a grid square
    inline int getmat(int x,int y){
        if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0){
            uint8_t t = gridterr[x][y];
            switch(t){
            default:
            case GTERR_GRASS:
                return GMAT_GRASS;
            case GTERR_FARM:
                return GMAT_FARMPLAYER0+gridowner[x][y];
            }
        } else
            return 0;
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
    
    // set terrain around a point IF it is grass (empty). Used
    // for houses. WILL ALSO SET THE OWNER TO ply.
    void setTerrainAroundIfGrass(int x,int y,int size,int mat,int ply){
        int minx=x-size;if(minx<0)minx=0;
        int miny=y-size;if(miny<0)miny=0;
        int maxx=x+size;if(maxx>=GRIDSIZE)maxx=GRIDSIZE-1;
        int maxy=y+size;if(maxy>=GRIDSIZE)maxy=GRIDSIZE-1;
        for(x=minx;x<=maxx;x++){
            for(y=miny;y<=maxy;y++){
                if(gridterr[x][y]==GTERR_GRASS){
                    gridterr[x][y]=mat;
                    gridowner[x][y]=ply;
                }
            }
        }
    }
    
    // given a lookup table of offsets of the form (x1,y1,x2,y2,...,-999)
    // return whether all those squares are flat. Also checks for objects
    // and farmland, which are obstacles too.
    bool isFlatGrassAtAllOffsets(int x,int y,const int *lookup){
        for(int i=0;lookup[i]>-900;i+=2){
            if(!isFlatGrass(x+lookup[i],y+lookup[i+1]))
                return false;
        }
        return true;
    }
    
    // as above but doesn't check for farmland, so a house which is
    // already built doesn't get blocked by its own farm!
    bool isFlatAtAllOffsets(int x,int y,const int *lookup){
        for(int i=0;lookup[i]>-900;i+=2){
            if(!isFlat(x+lookup[i],y+lookup[i+1]))
                return false;
        }
        return true;
    }
    
    // return the number of flat, unoccupied squares around here -
    // farmland is not an obstacle
    int countFlat(int x,int y);
    
    // return the number of flat, unoccupied squares around here -
    // farmland IS an obstacle
    int countFlatGrass(int x,int y);
    
    // does this point lie within a flat square?
    bool isFlat(int x,int y);
    
    // does this point lie within a flat square and is grass?
    bool isFlatGrass(int x,int y);
    
    // get the height at x,y doing bilinear interpolation between the corners
    float getinterp(float x,float y);

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
            recalc();
        }
    }
    // lower at x,y (and neighbours if necessary)
    void down(int x,int y){
        _down(x,y);
        recalc();
    }

    // draw the objects (call after genTriangles())
    void renderObjects(int range);

    // move cursor and move centre if required
    void moveCursor(int dx,int dy){
        if(cursorx+dx>=GRIDSIZE || cursorx+dx<0)dx=0;
        if(cursory+dy>=GRIDSIZE || cursory+dy<0)dy=0;
        cursorx+=dx;
        cursory+=dy;
        if(getVisibility(cursorx,cursory)<0.5){
            centrex+=dx;centrey+=dy;
        }
    }

};


#endif /* __GRID_H */
