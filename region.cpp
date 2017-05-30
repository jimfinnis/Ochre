/**
 * @file region.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "region.h"
#include "context.h"
#include "effect.h"

Region *Region::current = NULL;

std::vector<Region *> Region::regions;

void Region::notifyMouseMove(int x,int y){
    Context *c = Context::getInstance();
    // first switch to GL device coords (sigh)
    y = c->h - y;
    // then go through the regions
    
    std::vector<Region *>::iterator i;
    for(i=regions.begin();i!=regions.end();++i){
        Region *r = *i;
        if(x >= r->x && x<r->x+r->w && y>=r->y && y<r->y+r->h){
            r->onMouseMove(x-r->x,y-r->y);
            break;
        }
    }
}

void Region::notifyClick(int x,int y,int b){
    Context *c = Context::getInstance();
    // first switch to GL device coords (sigh)
    y = c->h - y;
    // then go through the regions
    
    std::vector<Region *>::iterator i;
    for(i=regions.begin();i!=regions.end();++i){
        Region *r = *i;
        if(x >= r->x && x<r->x+r->w && y>=r->y && y<r->y+r->h){
            x-=r->x;
            y-=r->y;
            switch(b){
            case SDL_BUTTON_LEFT:
                r->onLeftClick(x,y);break;
            case SDL_BUTTON_MIDDLE:
                r->onMiddleClick(x,y);break;
            case SDL_BUTTON_RIGHT:
                r->onRightClick(x,y);break;
            }
        }
    }
}



Region::Region(const char *nm){
    // initially invalid
    name = nm;
    x=-1; y=-1;w=1;h=1;
    regions.push_back(this);
}
Region::~Region(){
    regions.erase(std::remove(regions.begin(),regions.end(),this),regions.end());
}

void Region::setvp(){
    glViewport(x,y,w,h);
    glScissor(x,y,w,h);
    glEnable(GL_SCISSOR_TEST);
    current = this;
}

void Region::set(){
    setvp();
    EffectManager::projection = glm::ortho(0.0f,w,h,0.0f,-1.0f,1.0f);
}

void Region::setAndClear(const Colour& c){
    set();
    
    glClearColor(c.r,c.g,c.b,0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
}

static const float whiteCol[] = {1,1,1,1};
static PRELITVERTEX quad[] = {
    {0,0,0, 0,0},
    {0,1,0, 0,1},
    {1,1,0, 1,1},
    {1,1,0, 1,1},
    {1,0,0, 1,0},
    {0,0,0, 0,0},
};

void Region::renderQuadUntex(float x,float y,float w,float h,float *col){
    EffectManager *em = EffectManager::getInstance();
    em->flatuntex->begin();
    glm::mat4 id;
    em->flatuntex->setWorldMatrix(&id);
    em->flatuntex->setMaterial(col);
    
    PRELITVERTEX *p = quad;
    p->x = x;	p->y = y;  p++;
    p->x = x;	p->y = y+h;p++;
    p->x = x+w;	p->y = y+h;p++;
    p->x = x+w;	p->y = y+h;p++;
    p->x = x+w;	p->y = y  ;p++;
    p->x = x;	p->y = y;
    
    GLuint vbo;
    glGenBuffers(1,&vbo);
    ERRCHK;
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,sizeof(PRELITVERTEX)*6,&quad[0],GL_STATIC_DRAW);
    ERRCHK;
    
    em->flatuntex->setArrayOffsetsPrelit();
    
    glDrawArrays(GL_TRIANGLES,0,6);
    ERRCHK;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&vbo);
    
    em->flatuntex->end();
}
        
void Region::renderQuad(float x,float y,float w,float h,GLuint tex){
    EffectManager *em = EffectManager::getInstance();
    em->flattex->begin();
    glm::mat4 id;
    em->flattex->setWorldMatrix(&id);
    em->flattex->setMaterial(whiteCol,tex);
    
    PRELITVERTEX *p = quad;
    p->x = x;	p->y = y;  p++;
    p->x = x;	p->y = y+h;p++;
    p->x = x+w;	p->y = y+h;p++;
    p->x = x+w;	p->y = y+h;p++;
    p->x = x+w;	p->y = y  ;p++;
    p->x = x;	p->y = y;
    
    GLuint vbo;
    glGenBuffers(1,&vbo);
    ERRCHK;
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,sizeof(PRELITVERTEX)*6,&quad[0],GL_STATIC_DRAW);
    ERRCHK;
    
    em->flattex->setArrayOffsetsPrelit();
    
    glDrawArrays(GL_TRIANGLES,0,6);
    ERRCHK;
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&vbo);
    
    em->flattex->end();
}

void IsoRegion::set(){
    setvp();
    
    float aspect = w/h;
/*    
    static const float pitch = -25;
    static const float yaw = 0;
    
    EffectManager::projection = 
          glm::ortho(-2.0f*aspect,2.0f*aspect,  -2.0f,2.0f,  -10.0f,10.0f) *
          glm::rotate(glm::mat4(),glm::radians(pitch),glm::vec3(1.0f, 0.0f, 0.0f))*
   glm::rotate(glm::mat4(),glm::radians(yaw),glm::vec3(0.0f,1.0f,0.0f));
 */
    
    
    EffectManager::projection = glm::perspective(glm::radians(20.0f), w/h, 1.0f, 100.0f);
}

