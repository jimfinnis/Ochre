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
    heightFactor = 0.5f;
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
            mapsteps[x][y] = 0.0f;
        }
    }
    recalcSafe(); // calculate initial "safe squares"


    // same order as GMAT_ constants
    materials.push_back(Material(0,1,1,1)); // grass
    materials.push_back(Material(0,0.9,0.0,0.9)); // farm

    // once we know how many mats we need we can allocate the buckets for the triangles
    // vertices for each material.

    for(int i=0;i<materials.size();i++){
        buckets.push_back(std::vector<GLuint>());
    }

    glGenTextures(1,&maptex);
    ERRCHK;
    glBindTexture(GL_TEXTURE_2D,maptex);
    ERRCHK;

    // yes, I'd rather use glTexStorage2D, but my laptop is running
    // Ubuntu 14.04 (and I can't distupgrade yet, I don't want to break
    // ROS [I'm a roboticist] and also it's *really old*)
    // (My normal devbox is up-to-date but I *like* terence [the laptop])
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GRIDSIZE,GRIDSIZE,
                 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
//    glTexStorage2D(GL_TEXTURE_2D,1,GL_RGBA8,);
    ERRCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    ERRCHK;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ERRCHK;

    if(!maptex){
        throw Exception().set("could not create grid map texture: %s",
                              SDL_GetError());
    }
    writeMapTexture();
}

void Grid::select(int idx){
    if(idx>=0){
        cursorx = vertidxs[idx][0];
        cursory = vertidxs[idx][1];
    }
}

Grid::~Grid(){
    if(vbo)glDeleteBuffers(2,&vbo);
    if(maptex)glDeleteTextures(1,&maptex);
}

static void calcnormal(UNLITVERTEX *v0,UNLITVERTEX *v1,UNLITVERTEX *v2){
    glm::vec3 *vv0 = reinterpret_cast<glm::vec3*>(&v0->x);
    glm::vec3 *vv1 = reinterpret_cast<glm::vec3*>(&v1->x);
    glm::vec3 *vv2 = reinterpret_cast<glm::vec3*>(&v2->x);

    glm::vec3 norm = glm::triangleNormal(*vv0,*vv1,*vv2);
    v0->nx = v1->nx = v2->nx = norm.x;
    v0->ny = v1->ny = v2->ny = norm.y;
    v0->nz = v1->nz = v2->nz = norm.z;

}

void Grid::recalcSafe(){
    // work out grid square safety - a square is only safe if it is entirely land.
    memset(gridsafe,0,GRIDSIZE*GRIDSIZE);
    for(int x=0;x<GRIDSIZE-1;x++){
        for(int y=0;y<GRIDSIZE-1;y++){
            if(grid[x][y] && grid[x+1][y] && grid[x][y+1] && grid[x+1][y+1])
                gridsafe[x][y]=1;
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
            int h10 = get(x+1,y);
            int h11 = get(x+1,y+1);
            int h01 = get(x,y+1);

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
                        v0 = addvert(x,y,x0,h00,y0,0,0);
                        v1 = addvert(x,y+1,x0,h01,y1,0,1);
                        v2 = addvert(x+1,y+1,x1,h11,y1,1,1);
                        calcnormal(v0,v1,v2);
                        addtri(v0,v1,v2,mat);
                        // no base polys in these cases; they would be hidden
                    } else {
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
        eff->setMaterial(m->diffuse,m->texture);
        glDrawElements(GL_TRIANGLES,it->count,GL_UNSIGNED_INT,
                       (void *)(it->start*sizeof(GLuint)));

    }

    eff->setMaterial(whiteCol,0);
    glDrawArrays(GL_TRIANGLES,0,vertct);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    eff->end();

    drawHouses();
    writeMapTexture();
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


float snark=0;
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

    snark = vis.getVisibility(cursorx,cursory);
}

void Grid::_up(int x,int y){
    if(x<0 || x>=GRIDSIZE || y<0 || y>=GRIDSIZE)return;

    grid[x][y]++;
    int h = grid[x][y];
    modcount++;
    for(int xx=x-1;xx<=x+1;xx++){
        for(int yy=y-1;yy<=y+1;yy++){
            if(h-get(xx,yy)>1)_up(xx,yy); // recursion oh god.
        }
    }
}

void Grid::_down(int x,int y){
    if(x<0 || x>=GRIDSIZE || y<0 || y>=GRIDSIZE)return;
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

void Grid::pushxforminterp(float fx,float fy,float offset){
    MatrixStack *ms = StateManager::getInstance()->getx();

    int x = (int)fx;
    int y = (int)fy;

    float h00 = get(x,y);
    float h01 = get(x,y+1);
    float h10 = get(x+1,y);
    float h11 = get(x+1,y+1);

    float r = blerp(h00,h10,h01,h11,fx-x,fy-y)+offset;

    ms->push();
    fx -= centrex;
    fy -= centrey;
    ms->translate(fx,r*heightFactor,fy);
}

float VisLines::line::getDistSquared(float x,float y){
    glm::vec2 p(x,y); // get point
    // project (x,y) onto line
    glm::vec2 proj = (a-p) - glm::dot(a-p,n)*n;
    // get dist squared
    float d =  glm::dot(proj,proj);
    // get side of line (CP of line and start-to-point)
    // and return 0 if on the wrong side
    glm::vec2 ap = p-a;
    if(n.x * ap.y - ap.x * n.y < 0)
        return 0;
    return d;
}
float VisLines::getVisibility(float x,float y){
    // calculate minimum distance from each line, bombing out
    // immediately if we're on the wrong side

    if(ct==0)return 0;
    float md = lines[0].getDistSquared(x,y);
    for(int i=0;i<ct;i++){
        float d = lines[i].getDistSquared(x,y);
        if(d<md)md=d;
    }
    if(md>1)md=1;

    md = powf(md,0.2);


    return md;
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
        if(getVisibility(p[0],p[1])>0.5f){
            pushxform(p[0],p[1],0);
            ms->rotY(glm::radians(45.0f));
            meshes::house1->render(ms->top());
            ms->pop();
        }
    }
    meshes::house1->endBatch();

}

struct coltable {
    uint32_t cols[16]; // normal map colour
    uint32_t colsvis[16]; // visible map colour
    coltable(){
        Colour c;
        c.setFromHSV(0.5,0.8,0.5f);
        cols[0] = c.getABGR32();
        c.setFromHSV(0.5,0.8,1);
        colsvis[0] = c.getABGR32();

        for(int i=1;i<16;i++){
            Colour c;
            float base = powf(((float)i)/16.0f,0.4f)*0.5f;
            c.setFromHSV(0,0,base);
            cols[i] = c.getABGR32();
            c.setFromHSV(0,0,base*2.0f);
            colsvis[i] = c.getABGR32();
        }
    }

} cols;

void Grid::writeMapTexture(){
    uint32_t image[GRIDSIZE][GRIDSIZE];

    uint32_t *p = &image[0][0];
    for(int y=0;y<GRIDSIZE;y++){
        for(int x=0;x<GRIDSIZE;x++){
            uint8_t h = grid[x][y];
            uint32_t col = mapvis[x][y] ? cols.colsvis[h] : cols.cols[h];

            // for debugging, replace that with stigmergy
            Colour c;
            c.setFromHSV(grid[x][y]?0.5:0,0.5,mapsteps[x][y]*0.1f+0.3f);
            col = c.getABGR32();

            *p++ = col;
        }
    }


    if(0 && globals::game){
        Player *player = &globals::game->p;
        for(Person *p=player->people.first();p;p=player->people.next(p)){
            uint32_t col;
            switch(p->pmode){
            case NOPATH:
                col = 0xffffffff;break;
            case COARSEPATH:
                col = 0xff00ffff;break;
            case FINEPATH:
                col = 0xffff0000;break;
            default:
                col = 0xff0000ff;break;
            }
            image[(int)p->y][(int)p->x] = col;

        }
    }

    glBindTexture(GL_TEXTURE_2D,maptex);
    ERRCHK;
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    ERRCHK;
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,GRIDSIZE,GRIDSIZE,
                    GL_RGBA,GL_UNSIGNED_BYTE,&image[0][0]);
    ERRCHK;
    glBindTexture(GL_TEXTURE_2D,0);
    ERRCHK;
}

void Grid::update(float t){
  for(int y=0;y<GRIDSIZE;y++){
    for(int x=0;x<GRIDSIZE;x++){
      mapsteps[x][y]*=0.996f;
      // add a little noise to mess things up a bit, and stop the
      // little sods making lawnmower stripes
      mapsteps[x][y]+=drand48()*0.001f;
    }
  }
}
