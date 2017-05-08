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
#include <glm/gtx/normal.hpp>
#include <glm/gtc/noise.hpp>

static float noise(float x,float y){
    static const int OCTAVES = 4;
    float persist = 0.75;
    
    float v = glm::simplex(glm::tvec2<float>(x,y));
    float p = 0.5f;
    float amp = persist;
    for(int i=0;i<OCTAVES;i++){
        v += glm::simplex(glm::tvec2<float>(x*p,y*p))*amp;
        p*=0.5f;
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
    
    memset(grid,1,GRIDSIZE*GRIDSIZE);
    grid[20][20]=0;
    grid[20][21]=0;
    grid[21][20]=0;
    grid[21][21]=0;
    //    return;
    
    for(int x=0;x<GRIDSIZE;x++){
        for(int y=0;y<GRIDSIZE;y++){
            float v = noise(1093.0f+seedf+x*0.1f,y*0.1f)*5-waterlevel;
            if(v<0)v=0;
            if(v>5)v=5;
            grid[x][y] = v;
            gridmats[x][y] = 0;
        }
    }
    
    // same order as GMAT_ constants
    materials.push_back(Material(NULL,1,1,1)); // grass
    materials.push_back(Material(NULL,0.9,0.0,0.9)); // farm
    
    // once we know how many mats we need we can allocate the buckets for the triangles
    // vertices for each material.
    
    for(int i=0;i<materials.size();i++){
        buckets.push_back(std::vector<GLuint>());
    }
    
    maptex = SDL_CreateTexture(Context::getInstance()->rdr,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               GRIDSIZE,GRIDSIZE);
    if(!maptex){
        throw Exception().set("could not create grid map texture: %s",
                              SDL_GetError());
    }
    writeTexture();
}

void Grid::select(int idx){
    if(idx>=0){
        cursorx = vertidxs[idx][0];
        cursory = vertidxs[idx][1];
    }
}

Grid::~Grid(){
    if(vbo)glDeleteBuffers(2,&vbo);
    if(maptex)SDL_DestroyTexture(maptex);
}


static void calcnormal(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2){
    glm::tvec3<float> *vv0 = reinterpret_cast<glm::tvec3<float>*>(&v0->x);
    glm::tvec3<float> *vv1 = reinterpret_cast<glm::tvec3<float>*>(&v1->x);
    glm::tvec3<float> *vv2 = reinterpret_cast<glm::tvec3<float>*>(&v2->x);
    
    glm::vec3 norm = glm::triangleNormal(*vv0,*vv1,*vv2);
    v0->nx = v1->nx = v2->nx = norm.x;
    v0->ny = v1->ny = v2->ny = norm.y;
    v0->nz = v1->nz = v2->nz = norm.z;
    
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
    
    for(int i=0;i<materials.size();i++){
        buckets[i].clear();
    }
    
    memset(visible,0,GRIDSIZE*GRIDSIZE);
    
    for(int ox=-range;ox<range;ox++){
        for(int oy=-range;oy<range;oy++){
            
            if(abs(ox)+abs(oy)>=range)continue;
            
            int x = centrex+ox;
            int y = centrey+oy;
            
            int mat = getmat(x,y);
            int h00 = get(x,y);
            int h10 = get(x+1,y);
            int h11 = get(x+1,y+1);
            int h01 = get(x,y+1);
            
            // get the vertex coords (adding correction)
            float x0 = (float)ox-0.5;
            float x1 = (float)(ox+0.5);
            float y0 = (float)oy;
            float y1 = (float)(oy+1);
            
            glm::vec3 norm;
            UNLITVERTEX *v0,*v1,*v2;
            
            // deal with edges
            
#define BASE -10
            if(abs(ox)+abs(oy)==range-1){
                if(ox==0){
                    if(oy>0){
                        visible[x][y]=1;
                        visible[x+1][y]=1;
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
                        visible[x+1][y]=1;
                        visible[x][y+1]=1;
                        visible[x+1][y+1]=1;
                        v0 = addvert(x,y+1,x0,h01,y1,0,1);
                        v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        // no base polys in these cases; they would be hidden
                    } else {
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2);
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
                        visible[x][y]=1;
                        visible[x][y+1]=1;
                        visible[x+1][y+1]=1;
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x,y+1,x0,h01,y1,0,1);
                        v2 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        // no base polys in these cases; they would be hidden
                    } else {
                        visible[x][y]=1;
                        visible[x][y+1]=1;
                        visible[x+1][y]=1;
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x,y+1,x0,h01,y1,0,1);
                        v2 = addvert(x+1,y,x1,h10,y0,1,0);
                        calcnormal(v0,v1,v2);
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
                visible[x][y]=1;
                visible[x][y+1]=1;
                visible[x+1][y]=1;
                visible[x+1][y+1]=1;
                // different triangulations for the two kinds of split
                if(h00==h11){
                    // first triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2);
                    addtri(v0,v1,v2,mat);
                    //                v0->dump();v1->dump();v2->dump();
                    // second triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x,y+1,x0,h01,y1,0,1);
                    v2 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    calcnormal(v0,v1,v2);
                    addtri(v0,v1,v2,mat);
                    //                v0->dump();v1->dump();v2->dump();printf("\n");
                } else {
                    // first triangle
                    v0 = addvert(x,y,x0,h00,y0,0,0);
                    v1 = addvert(x,y+1,x0,h01,y1,0,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2);
                    addtri(v0,v1,v2,mat);
                    // second triangle
                    v0 = addvert(x,y+1,x0,h01,y1,0,1);
                    v1 = addvert(x+1,y+1,x1,h11,y1,1,1);
                    v2 = addvert(x+1,y,x1,h10,y0,1,0);
                    calcnormal(v0,v1,v2);
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
        eff->setMaterial(m->diffuse,m->t);
        glDrawElements(GL_TRIANGLES,it->count,GL_UNSIGNED_INT,
                       (void *)(it->start*sizeof(GLuint)));
        
    }
    
    eff->setMaterial(whiteCol,NULL);
    glDrawArrays(GL_TRIANGLES,0,vertct);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    eff->end();
    
    drawHouses();
    writeTexture();
}

int Grid::intersect(const glm::vec3& origin, const glm::vec3& ray){
    
    float mindist;
    int found=-1;
    for(int i=0;i<vertct;i++){
        glm::tvec3<float> *v = reinterpret_cast<glm::tvec3<float>*>(&verts[i].x);
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
    ms->rotY(Time::now()*2.0f);
    meshes::cursor->render(sm->getx()->top());
    ms->pop();
    sm->pop();
}

void Grid::up(int x,int y){
    if(x<0 || x>=GRIDSIZE || y<0 || y>=GRIDSIZE)return;
    
    grid[x][y]++;
    int h = grid[x][y];
    modcount++;
    for(int xx=x-1;xx<=x+1;xx++){
        for(int yy=y-1;yy<=y+1;yy++){
            if(h-get(xx,yy)>1)up(xx,yy); // recursion oh god.
        }
    }
}

void Grid::down(int x,int y){
    if(x<0 || x>=GRIDSIZE || y<0 || y>=GRIDSIZE)return;
    if(grid[x][y]==0)return;
    
    grid[x][y]--;
    modcount++;
    int h = grid[x][y];
    for(int xx=x-1;xx<=x+1;xx++){
        for(int yy=y-1;yy<=y+1;yy++){
            if(get(xx,yy)-h>1)down(xx,yy); // recursion oh god.
        }
    }
}

void Grid::pushxform(int x,int y,float offset){
    MatrixStack *ms = StateManager::getInstance()->getx();
    float xx = ((float)(x-centrex))-0.5;
    float yy = ((float)get(x,y)+offset)*heightFactor;
    float zz = (float)(y-centrey);
    
    ms->push();
    ms->translate(xx,yy,zz);
}

void Grid::drawHouses(){
    StateManager *sm = StateManager::getInstance();
    MatrixStack *ms = sm->getx();
    int locs[] = {
        20,20,
        24,23,
        18,12,
        20,22,
        19,18,
        -1,-1
    };
    
    meshes::house1->startBatch();
    
    for(int *p = locs;*p>=0;p+=2){
        if(visible[p[0]][p[1]]){
            pushxform(p[0],p[1],0);
            ms->rotY(glm::radians(45.0f));
            meshes::house1->render(ms->top());
            ms->pop();
        }
    }
    
    
    meshes::house1->endBatch();
}
    
void Grid::writeTexture(){
    SDL_Rect r;
    uint8_t *pixels;
    int pitch;
    
    if(SDL_LockTexture(maptex,NULL,(void **)&pixels,&pitch)<0)
        throw Exception().set("cannot lock grid map texture: %s",
                              SDL_GetError());
    uint8_t *row = pixels;
    for(int y=0;y<GRIDSIZE;y++){
        uint32_t *r = (uint32_t *)row;
        for(int x=0;x<GRIDSIZE;x++){
            uint32_t col;
            switch(grid[x][y]){
            case 0:
                col = 0x0080ff;break;
            case 1:
                col = 0x808080;break;
            case 2:
                col = 0xa0a0a0;break;
            case 3:
                col = 0xb0b0b0;break;
            case 4:
                col = 0xc0c0c0;break;
            case 5:
                col = 0xffffff;break;
            default:
                col = 0xff0000;break;
            }
            col |= ((visible[x][y])?0xff:0xa0)<<24;
            *r++=col;
        }
        row += pitch;
    }
    SDL_UnlockTexture(maptex);
}
