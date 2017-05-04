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

#include "screen.h"
#include "grid.h"

#include <glm/gtx/normal.hpp>

Grid::Grid(){
    scaleFactor = 0.5f;
    heightFactor = 0.25f;
    memset(grid,1,GRIDSIZE*GRIDSIZE);
    for(int x=0;x<GRIDSIZE;x++)
        for(int y=0;y<GRIDSIZE;y++)
            grid[x][y] = rand()%2;
    vbo=0;
}

Grid::~Grid(){
    if(vbo)glDeleteBuffers(1,&vbo);
}


static glm::vec3 calcnormal(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2){
    glm::tvec3<float> *vv0 = reinterpret_cast<glm::tvec3<float>*>(&v0->x);
    glm::tvec3<float> *vv1 = reinterpret_cast<glm::tvec3<float>*>(&v1->x);
    glm::tvec3<float> *vv2 = reinterpret_cast<glm::tvec3<float>*>(&v2->x);
    
    return glm::triangleNormal(*vv0,*vv1,*vv2);
    
}

void Grid::genTriangles(int cx,int cy,int range){
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
    for(int x=cx-range;x<cx+range;x++){
        for(int y=cy-range;y<cy+range;y++){
            int h00 = get(x,y);
            int h10 = get(x+1,y);
            int h11 = get(x+1,y+1);
            int h01 = get(x,y+1);
            
            float x0 = (float)(x-cx);
            float x1 = (float)(x-cx+1);
            float y0 = (float)(y-cy);
            float y1 = (float)(y-cy+1);
            
            glm::vec3 norm;
            UNLITVERTEX *v0,*v1,*v2;
            
//            printf("%d %d %d %d\n",h00,h01,h10,h11);
            // different triangulations for the two kinds of split
            if(h00==h11){
                // first triangle
                v0 = addvert(x0,h00,y0,0,0);
                v1 = addvert(x1,h11,y1,1,1);
                v2 = addvert(x1,h10,y0,1,0);
                norm = calcnormal(v0,v1,v2);
                v0->nx = v1->nx = v2->nx = norm.x;
                v0->ny = v1->ny = v2->ny = norm.y;
                v0->nz = v1->nz = v2->nz = norm.z;
//                v0->dump();v1->dump();v2->dump();
                // second triangle
                v0 = addvert(x0,h00,y0,0,0);
                v1 = addvert(x0,h01,y1,0,1);
                v2 = addvert(x1,h11,y1,1,1);
                norm = calcnormal(v0,v1,v2);
                v0->nx = v1->nx = v2->nx = norm.x;
                v0->ny = v1->ny = v2->ny = norm.y;
                v0->nz = v1->nz = v2->nz = norm.z;
//                v0->dump();v1->dump();v2->dump();printf("\n");
            } else {
                // first triangle
                v0 = addvert(x0,h00,y0,0,0);
                v1 = addvert(x0,h01,y1,0,1);
                v2 = addvert(x1,h10,y0,1,0);
                norm = calcnormal(v0,v1,v2);
                v0->nx = v1->nx = v2->nx = norm.x;
                v0->ny = v1->ny = v2->ny = norm.y;
                v0->nz = v1->nz = v2->nz = norm.z;
                // second triangle
                v0 = addvert(x0,h01,y1,0,1);
                v1 = addvert(x1,h11,y1,1,1);
                v2 = addvert(x1,h10,y0,1,0);
                norm = calcnormal(v0,v1,v2);
                v0->nx = v1->nx = v2->nx = norm.x;
                v0->ny = v1->ny = v2->ny = norm.y;
                v0->nz = v1->nz = v2->nz = norm.z;
            }
        }
    }
    if(vbo)glDeleteBuffers(1,&vbo);
    glGenBuffers(1,&vbo);
    ERRCHK;
    
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,sizeof(UNLITVERTEX)*vertct,verts,GL_STATIC_DRAW);
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
    
    static const float grayCol[] = {0.5,0.5,0.5,1};
    static const float whiteCol[] = {1,1,1,1};
    
    eff->setMaterial(whiteCol,NULL);
    glDrawArrays(GL_TRIANGLES,0,vertct);
    glClear( GL_DEPTH_BUFFER_BIT );
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    eff->end();
}
