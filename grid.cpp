/**
 * @file grid.cpp
 * @brief  Brief description of file.
 *
 */


#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"
#include "font.h"

#include "context.h"
#include "screen.h"
#include "grid.h"
#include "meshes.h"
#include "time.h"
#include "globals.h"
#include "game.h"
#include "player.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/normal.hpp>
#include <glm/gtc/noise.hpp>

static float noise(float x,float y){
    static const int OCTAVES = 4;
    float persist = 0.8;
    
    float p = powf(2,-3);
    float amp = persist;
    float v = 0;
    for(int i=0;i<OCTAVES;i++){
        v += glm::simplex(glm::vec2(x*p,y*p))*amp;
        p*=2;
        amp*=persist;
    }
    return v*0.5;
}

Grid::Grid(int seed,float waterlevel){
    heightFactor = 0.2f;
    vbo=0;ibo=0;
    cursorx=cursory=GRIDSIZE/2;
    centrex=centrey=cursorx;
    float seedf = 1000.0f*seed;
    
    memset(gridterr,0,GRIDSIZE*GRIDSIZE);
    memset(gridowner,0,GRIDSIZE*GRIDSIZE);
    memset(grid,1,GRIDSIZE*GRIDSIZE);
    memset(objects,0,GRIDSIZE*GRIDSIZE*sizeof(GridObj *));
    
    for(int x=0;x<GRIDSIZE;x++){
        for(int y=0;y<GRIDSIZE;y++){
            float v = noise(1093.0f+seedf+x*0.1f,y*0.1f)*5-
                  (waterlevel-12.0f)*0.25f;
            
            if(v<0)v=0;
            if(v>5)v=5;
            grid[x][y] = v;
            gridterr[x][y] = 0;
        }
    }
    recalc(); // calculate initial "safe squares"
    
    
    // same order as GMAT_ constants
    materials.push_back(Material(0,0.7,0.7,0.7)); // grass
    materials.push_back(Material(0,0.9,0.0,0.9)); // farm, ply 0
    materials.push_back(Material(0,0.0,0.9,0.9)); // farm, ply 1
    
    // once we know how many mats we need we can allocate the buckets for the triangles
    // vertices for each material.
    
    for(int i=0;i<materials.size();i++){
        buckets.push_back(std::vector<GLuint>());
    }
    
}

void Grid::select(int idx){
    if(idx>=0){
        cursorx = vertidxs[idx][0];
        cursory = vertidxs[idx][1];
    }
}

Grid::~Grid(){
    if(vbo)glDeleteBuffers(2,&vbo);
}

// two versions of the calcnormal routine, one for when no corners are at the cursor,
// and one for when one of the corners is (so we can brighten it or something)

inline void calcnormal(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2){
    glm::vec3 *vv0 = reinterpret_cast<glm::vec3*>(&v0->x);
    glm::vec3 *vv1 = reinterpret_cast<glm::vec3*>(&v1->x);
    glm::vec3 *vv2 = reinterpret_cast<glm::vec3*>(&v2->x);
    
    glm::vec3 norm = glm::triangleNormal(*vv0,*vv1,*vv2);
    v0->nx = v1->nx = v2->nx = norm.x;
    v0->ny = v1->ny = v2->ny = norm.y;
    v0->nz = v1->nz = v2->nz = norm.z;
}

inline void calcnormal(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2,
                       bool iscur0,bool iscur1,bool iscur2){
    glm::vec3 *vv0 = reinterpret_cast<glm::vec3*>(&v0->x);
    glm::vec3 *vv1 = reinterpret_cast<glm::vec3*>(&v1->x);
    glm::vec3 *vv2 = reinterpret_cast<glm::vec3*>(&v2->x);
    
    glm::vec3 norm = glm::triangleNormal(*vv0,*vv1,*vv2);
    glm::vec3 normb = norm;
    normb *= 1.5;
    vv0 = reinterpret_cast<glm::vec3*>(&v0->nx);
    vv1 = reinterpret_cast<glm::vec3*>(&v1->nx);
    vv2 = reinterpret_cast<glm::vec3*>(&v2->nx);
    
    *vv0 = iscur0 ? normb : norm;
    *vv1 = iscur1 ? normb : norm;
    *vv2 = iscur2 ? normb : norm;
}

void Grid::recalc(){
    // work out grid square safety - a square is only safe if it is entirely land.
    memset(gridsafe,0,GRIDSIZE*GRIDSIZE);
    for(int x=0;x<GRIDSIZE-1;x++){
        for(int y=0;y<GRIDSIZE-1;y++){
            if(grid[x][y] && grid[x+1][y] && grid[x][y+1] && grid[x+1][y+1])
                gridsafe[x][y]=1;
            
            // and work out whether the square is flat
            int h00 = get(x,y);
            int h01 = get(x,y+1);
            int h10 = get(x+1,y);
            int h11 = get(x+1,y+1);
            
            isflat[x][y] = (h00 == h01 && h00 == h10 && h00 == h10 && h00 == h11);
        }
    }
}

void Grid::genTriangles(int range){
    extern bool debugtoggle;
    // First we need to make the vertex data. One useful thing is that we know
    // that each node in the grid cannot be more than 1 different from each of
    // its 8-neighbours. Given that constraint, there are only a few possible
    // basic configurations.
    // Unfortunately this information isn't of much use because we're using
    // flat shading, so we need to create two independent triangles most of the
    // time. We could optimise, but things would get interesting quite quickly
    // and this is a quick and dirty piece of code. Also, the mesh is miniscule
    // so it's not worth it. In fact, we're just going to render all the triangles
    // in order so there's no point even having an index buffer.
    initGridVerts();
    memset(mapvis,0,GRIDSIZE*GRIDSIZE);
    
    for(int i=0;i<materials.size();i++){
        buckets[i].clear();
    }
    
    // line 1 (top left)
    vis.reset();
    vis.add(centrex-range+2,centrey,centrex,centrey-range+2);
    
    // line 2 (far)
    vis.add(centrex,centrey-range+2,centrex+1,centrey-range+2);
    
    // line 3 (top right)
    vis.add(centrex+1,centrey-range+2,centrex+range-1,centrey);
    
    // line 4 (right)
    vis.add(centrex+range-1,centrey,centrex+range-1,centrey+1);
    
    // line 5 (bottom right)
    vis.add(centrex+range-1,centrey+1,centrex+1,centrey+range-1);
    
    // line 6 (near)
    vis.add(centrex+1,centrey+range-1,centrex,centrey+range-1);
    
    // line 7 (bottom left)
    vis.add(centrex,centrey+range-1,centrex-range+2,centrey+1);
    
    // line 8 (left)
    vis.add(centrex-range+2,centrey+1,centrex-range+2,centrey);
    
    viewRange=range;
    for(int ox=-range;ox<range;ox++){
        for(int oy=-range;oy<range;oy++){
            
            if(abs(ox)+abs(oy)>=range)continue;
            
            int x = centrex+ox;
            int y = centrey+oy;
            
            mapvis[x][y]=1;
            
            // coords in the vis and opaque buffers, which are
            // bigger so we can view off the map edge
            int xv = x+VISBORDER;
            int yv = y+VISBORDER;
            
            int mat = getmat(x,y);
            int h00 = get(x,y);
            bool iscur00 = x==cursorx && y==cursory;
            int h10 = get(x+1,y);
            bool iscur10 = x+1==cursorx && y==cursory;
            int h11 = get(x+1,y+1);
            bool iscur11 = x+1==cursorx && y+1==cursory;
            int h01 = get(x,y+1);
            bool iscur01 = x==cursorx && y+1==cursory;
            
            // get the vertex coords
            float x0 = (float)ox;
            float x1 = (float)(ox+1);
            float y0 = (float)oy;
            float y1 = (float)(oy+1);
            
            glm::vec3 norm;
            UNLITVERTEX *v0,*v1,*v2;
            
            // deal with edges
            
#define BASE -10
            if(abs(ox)+abs(oy)==range-1){
                if(ox==0){
                    if(oy>0){
                        // add base poly for front wedge
                        v0 = addvert(x,y,x0,BASE,y0,1,0);
                        v1 = addvert(x+1,y,x1,h10,y0,1,0);
                        v2 = addvert(x,y,x0,h00,y0,0,0);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        v0 = addvert(x,y,x0,BASE,y0,1,0);
                        v1 = addvert(x+1,y,x1,BASE,y0,1,0);
                        v2 = addvert(x+1,y,x1,h10,y0,0,0);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                    }
                }
                else if(ox<0){
                    if(oy==0){}
                    else if(oy<0){
                        v0 = addvert(x,y+1,x0,h01,y1,0,1);
                        v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2,iscur01,iscur11,iscur10);
                        addtri(v0,v1,v2,mat);
                        // no base polys in these cases; they would be hidden
                    } else {
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2,iscur00,iscur11,iscur10);
                        addtri(v0,v1,v2,mat);
                        // base polys
                        v0 = addvert(x+1,y+1,x1,BASE,y1,1,0);
                        v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        v2 = addvert(x,y,x0,h00,y0,0,1);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        v0 = addvert(x+1,y+1,x1,BASE,y1,1,0);
                        v1 = addvert(x,y,x0,h00,y0,0,1);
                        v2 = addvert(x,y,x0,BASE,y0,0,0);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                    }
                } else {
                    if(oy==0){}
                    else if(oy<0){
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x,y+1,x0,h01,y1,0,1);
                        v2 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        calcnormal(v0,v1,v2,iscur00,iscur01,iscur11);
                        addtri(v0,v1,v2,mat);
                        // no base polys in these cases; they would be hidden
                    } else {
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x,y+1,x0,h01,y1,0,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2,iscur00,iscur01,iscur10);
                        addtri(v0,v1,v2,mat);
                        // base polys
                        v0 = addvert(x+1,y,x1,BASE,y0,1,0);
                        v1 = addvert(x+1,y,x1,h10,y0,1,1);
                        v2 = addvert(x,y+1,x0,h01,y1,0,1);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        v0 = addvert(x,y+1,x0,BASE,y1,0,0);
                        v1 = addvert(x+1,y,x1,BASE,y0,1,0);
                        v2 = addvert(x,y+1,x0,h01,y1,0,1);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                    }
                }
            } else {
                // different triangulations for the two kinds of split
                if(h00==h11){
                    // first triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2,iscur00,iscur11,iscur10);
                    addtri(v0,v1,v2,mat);
                    //                v0->dump();v1->dump();v2->dump();
                    // second triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x,y+1,x0,h01,y1,0,1);
                    v2 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    calcnormal(v0,v1,v2,iscur00,iscur01,iscur11);
                    addtri(v0,v1,v2,mat);
                    //                v0->dump();v1->dump();v2->dump();printf("\n");
                } else {
                    // first triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x,y+1,x0,h01,y1,0,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2,iscur00,iscur01,iscur10);
                    addtri(v0,v1,v2,mat);
                    // second triangle
                    v0 = addvert(x,y+1,x0,h01,y1,0,1);
                    v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2,iscur01,iscur11,iscur10);
                    addtri(v0,v1,v2,mat);
                }
            }
        }
    }
    
    // now build the entire index buffer with transitions
    // First clear the data.
    static std::vector<GLuint> idxdata;
    idxdata.clear();
    transitions.clear();
    
    // go over the materials
    for(int i=0;i<materials.size();i++){
        // add the transitions
        if(buckets[i].size()){
            transitions.push_back(Transition(idxdata.size(),buckets[i].size(),i));
            // for each material, add its indices to the buffer.
            for(std::vector<GLuint>::iterator it = buckets[i].begin();it!=buckets[i].end();++it){
                idxdata.push_back(*it);
            }
        }
    }
    
    
    if(vbo)glDeleteBuffers(2,&vbo);
    glGenBuffers(2,&vbo);
    ERRCHK;
    
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,sizeof(UNLITVERTEX)*vertct,verts,GL_STATIC_DRAW);
    ERRCHK;
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint)*idxdata.size(),
                 &idxdata[0],
                 GL_STATIC_DRAW);
    ERRCHK;
}


void Grid::render(glm::mat4 *world){
    State *s = StateManager::getInstance()->get();
    // start the effect
    Effect *eff;
    // use the state's effect if there is one.
    if(s->effect)
        eff = s->effect;
    else
        // otherwise use the standard.
        eff = EffectManager::getInstance()->untex;
    eff->begin();
    // upload the matrices
    eff->setUniforms();
    eff->setWorldMatrix(world);
    
    
    // bind the arrays
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    
    // tell them about offsets
    eff->setArrayOffsetsUnlit();
    
    
    static const float whiteCol[] = {0.8,0.8,0.8,1};
    
    for(std::vector<Transition>::iterator it=transitions.begin();it!=transitions.end();++it){
        Material *m = &materials[it->matidx];
        eff->setMaterial(m->diffuse,m->texture);
        glDrawElements(GL_TRIANGLES,it->count,GL_UNSIGNED_INT,
                       (void *)(it->start*sizeof(GLuint)));
        
    }
    
    eff->setMaterial(whiteCol,0);
    glDrawArrays(GL_TRIANGLES,0,vertct);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    eff->end();
}

int Grid::intersect(const glm::vec3& origin, const glm::vec3& ray){
    
    float mindist;
    int found=-1;
    for(int i=0;i<vertct;i++){
        glm::vec3 *v = reinterpret_cast<glm::vec3*>(&verts[i].x);
        glm::vec3 diff = *v - origin;
        float parm = glm::dot(ray,diff);
        
        if(parm>0){
            // closest point on ray to vec
            glm::vec3 loc = origin + parm*ray;
            diff = *v - loc;
            float dist = sqrt(diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);
            if(!i || dist<mindist){
                found = i;
                mindist = dist;
            }
        }
    }
    return found;
}


void Grid::renderCursor(){
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    State *s = sm->push();
    s->light.col[0] = Colour(1,1,1,1);
    s->light.col[1] = Colour(1,1,1,1);
    s->light.ambient = Colour(0.7,0.7,0.7,1);
    pushxform(cursorx,cursory,-0.5f);
    ms->scale(0.3f);
    ms->rotY(globals::timeNow*2.0f);
    meshes::cursor->render(sm->getx()->top());
    ms->pop();
    sm->pop();
}

void Grid::_up(int x,int y){
    if(in(x,y)){
        grid[x][y]++;
        int h = grid[x][y];
        modcount++;
        for(int xx=x-1;xx<=x+1;xx++){
            for(int yy=y-1;yy<=y+1;yy++){
                if(h-get(xx,yy)>1)_up(xx,yy); // recursion oh god.
            }
        }
    }
}

void Grid::_down(int x,int y){
    if(in(x,y)){
        if(grid[x][y]==0)return;
        
        grid[x][y]--;
        modcount++;
        int h = grid[x][y];
        for(int xx=x-1;xx<=x+1;xx++){
            for(int yy=y-1;yy<=y+1;yy++){
                if(get(xx,yy)-h>1)_down(xx,yy); // recursion oh god.
            }
        }
    }
}

void Grid::pushxform(float x,float y,float offset){
    MatrixStack *ms = StateManager::getInstance()->getx();
    float xx = x-centrex;
    float yy = ((float)get(x,y)+offset)*heightFactor;
    float zz = y-centrey;
    
    ms->push();
    ms->translate(xx,yy,zz);
}

inline float lerp(float s, float e, float t){return s+(e-s)*t;}
inline float blerp(float c00, float c10, float c01, float c11, float tx, float ty){
    return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

float Grid::getinterp(float fx,float fy){
    int x = (int)fx;
    int y = (int)fy;
    
    float h00 = get(x,y);
    float h01 = get(x,y+1);
    float h10 = get(x+1,y);
    float h11 = get(x+1,y+1);
    
    return blerp(h00,h10,h01,h11,fx-x,fy-y);
}

void Grid::pushxforminterp(float fx,float fy,float offset){
    MatrixStack *ms = StateManager::getInstance()->getx();
    
    ms->push();
    float r = getinterp(fx,fy)+offset;
    fx -= centrex;
    fy -= centrey;
    ms->translate(fx,r*heightFactor,fy);
}

bool Grid::isFlatGrass(int x,int y){
    if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
        return isflat[x][y] && gridterr[x][y]==GTERR_GRASS;
    else
        return false;
}

bool Grid::isFlat(int x,int y){
    if(x<GRIDSIZE && x>=0 && y<GRIDSIZE && y>=0)
        return isflat[x][y];
    else
        return false;
}

float VisLines::line::getDistSquared(float x,float y){
    glm::vec2 p(x,y);
    // project (x,y) onto line
    glm::vec2 proj = (a-p) - glm::dot(a-p,n)*n;
    // get dist squared
    float d =  glm::dot(proj,proj);
    // get side of line (CP of line and start-to-point)
    // and return -999 if on the wrong side
    glm::vec2 ap = p-a;
    if(n.x * ap.y - ap.x * n.y < 0)
        return 0;
    return d;
}
float VisLines::getVisibility(float x,float y){
    if(ct==0)return 0;
    float md = lines[0].getDistSquared(x,y);
    for(int i=0;i<ct;i++){
        float d = lines[i].getDistSquared(x,y);
        if(d<md)md=d;
    }
    if(md>1)md=1;
    
    //    md = powf(md,0.2);
    
    
    return md;
}


void Grid::renderObjects(int range){
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    for(int ox=-range;ox<range;ox++){
        for(int oy=-range;oy<range;oy++){
            if(abs(ox)+abs(oy)>=range)continue;
            
            int x = centrex+ox;
            int y = centrey+oy;
            if(x>=0 && y>=0 && x<GRIDSIZE && y<GRIDSIZE && objects[x][y]){
                if(getVisibility(x,y)>0.5f){
                    pushxform(x+0.5f,y+0.5f,0);
                    ms->rotY(glm::radians(45.0f));
                    objects[x][y]->queueRender(ms->top());
                    ms->pop();
                }
            }
        }
    }
}

void Grid::populatePeople(const Player &pl){
    for(Person *p=pl.people.first();p;p=pl.people.next(p)){
        int x = p->x;
        int y = p->y;
        
        p->next = people[x][y];
        people[x][y] = p;
    }
}

void Grid::update(float t){
    // populate the people lists
    memset(people,0,GRIDSIZE*GRIDSIZE*sizeof(GridObj *));
    populatePeople(globals::game->p[0]);
    populatePeople(globals::game->p[1]);
    // clear the terrain if farmland (houses set it later)
    for(int x=0;x<GRIDSIZE;x++){
        for(int y=0;y<GRIDSIZE;y++){
            gridterr[x][y]=GTERR_GRASS;
        }
    }
    // next stage is that the houses add their farms
    // and after that we remove non-flat farm.
    
    
}


void Grid::addHouse(int hx,int hy,House *h){
    objects[hx][hy]=h;
}

void Grid::removeHouse(House *h){
    objects[h->x][h->y]=NULL;
    h->zombie=true;
}

// work out how many squares around me are still flat. We do this
// with lookup tables for positions, working out from the middle.
// Tables are terminated with -999.

static const int lookup0[] = {0,0,-999};
static const int lookup1[] = {
    -1,-1, 0,-1, 1,-1,
    -1,0,   1,0,
    -1,1,  0,1,  1,1,
    -999};
static const int lookup2[] = {
    -2,-2, -2,-1, -2,0, -2,1, -2,2,
    -1,-2,  -1,2,
    0,-2,  0,2,
    1,-2,  1,2,
    2,-2,  2,-1,  2,0,  2,1,  2,2,
    -999};
static const int lookup3[] = {
    -3,-3,-3,-2,-3,-1,-3,0,-3,1,-3,2,-3,3,
    -2,-3,-2,3,
    -1,-3,-1,3,
    0,-3,0,3,
    1,-3,1,3,
    2,-3,2,3,
    3,-3,3,-2,3,-1,3,0,3,1,3,2,3,3,
    -999};
static const int lookup4[] = {
    -4,-4,-4,-3,-4,-2,-4,-1,-4,0,-4,1,-4,2,-4,3,-4,4,
    -4,-3,-4,3,
    -2,-3,-2,3,
    -1,-3,-1,3,
    0,-3,0,3,
    1,-3,1,3,
    2,-3,2,3,
    3,-3,3,3,
    4,-3,4,-2,4,-1,4,0,4,1,4,2,4,3,4,4,
    -999};

int Grid::countFlat(int x,int y){
    if(!isFlatAtAllOffsets(x,y,lookup0) || !get(x,y)){
        // this is a disaster; the bloody thing isn't flat at all - or it's in the sea
        return -1;
    } else if(!isFlatAtAllOffsets(x,y,lookup1)){
        // flat at only offset 0
        return 0;
    } else if(!isFlatAtAllOffsets(x,y,lookup2)){
        // flat at only 1
        return 1;
    } else if(!isFlatAtAllOffsets(x,y,lookup3)){
        return 2;
    } else
        return 3;
}

int Grid::countFlatGrass(int x,int y){
    if(!isFlatGrassAtAllOffsets(x,y,lookup0) || !get(x,y)){
        // this is a disaster; the bloody thing isn't flat at all - or it's in the sea
        return -1;
    } else if(!isFlatGrassAtAllOffsets(x,y,lookup1)){
        // flat at only offset 0
        return 0;
    } else if(!isFlatGrassAtAllOffsets(x,y,lookup2)){
        // flat at only 1
        return 1;
    } else if(!isFlatGrassAtAllOffsets(x,y,lookup3)){
        return 2;
    } else
        return 3;
}
