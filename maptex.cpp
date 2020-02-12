/**
 * @file maptex.cpp
 * @brief  Brief description of file.
 *
 */

#include "maptex.h"

MapTex::MapTex(){
    tex=0;
    glGenTextures(1,&tex);
    ERRCHK;
    glBindTexture(GL_TEXTURE_2D,tex);
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
    
    if(!tex){
        throw Exception().set("could not create grid map texture: %s",
                              SDL_GetError());
    }
    vid=NULL;
}

MapTex::~MapTex(){
    if(tex)glDeleteTextures(1,&tex);
    if(vid)delete vid;
}

void MapTex::copy(){
    glBindTexture(GL_TEXTURE_2D,tex);
    ERRCHK;
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    ERRCHK;
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,GRIDSIZE,GRIDSIZE,
                    GL_RGBA,GL_UNSIGNED_BYTE,&image[0][0]);
    ERRCHK;
    glBindTexture(GL_TEXTURE_2D,0);
    ERRCHK;
}

void MapTex::render(Region *r,int x,int y,int w,int h){
    StateManager *sm = StateManager::getInstance();
    sm->push();
    State *s = sm->get();
    s->setDefault();
    MatrixStack *ms = sm->getx();
    ms->push();
    ms->identity();
    
    r->renderQuad(x,y,w,h,tex);
    ms->pop();
    sm->pop();
}

MapTex::coltable MapTex::cols;

void MapTex::writePlayer(Player *pl,uint32_t col){
    uint32_t c;
    for(Person *p=pl->people.first();p;p=pl->people.next(p)){
        switch(p->state){
        case WANDER:c=col;break;
        case COARSEPATH:  c=0xff00ff00;break;
        case FINEPATH:    c=0xff0000ff;break;
        case HUNT:        c=0xffff00ff;break;
        default:          c=0xffffffff;break;
        }
        image[(int)p->y][(int)p->x] = c;
/*        
        if(p->state == COARSEPATH){
            for(int i=0;i<p->path.size();i++){
                image[p->path[i].y][p->path[i].x]=0xffffffff;
            }
        }
*/    }
    
    if(pl->anchorX>=0){
        int ax = (int)pl->anchorX;
        int ay = (int)pl->anchorY;
        for(int x=ax-2;x<=ax+2;x++){
            for(int y=ay-2;y<=ay+2;y++){
                if(x>=0 && x<GRIDSIZE && y>=0 && y<GRIDSIZE){
                    image[y][x] = col;
                }
            }
        }
    }
    
    // for debugging, write the levelling points
    if(pl->levelx>0)
        image[pl->levely][pl->levelx]= 0xffffffff;

}

void MapTex::make(Grid *g,bool writeframe){
    uint32_t *p = &image[0][0];
    for(int y=0;y<GRIDSIZE;y++){
        for(int x=0;x<GRIDSIZE;x++){
            uint8_t h = g->grid[x][y];
            uint32_t col = g->mapvis[x][y] ? cols.colsvis[h] : cols.cols[h];
            // add objects
            GridObj *o = g->objects[x][y];
            if(o){
                if(o->type == GO_HOUSE){
                    House *h = (House *)o;
                    col = h->p->idx ? 0xff008080 : 0xffff8000;
                }
            }
            *p++ = col;
        }
    }
    
    // add peeps
    
    if(1 && globals::game){
        writePlayer(&globals::game->p[0],0xffff0000);
        writePlayer(&globals::game->p[1],0xff00ffff);
    }
    copy();
    if(vid && writeframe)vid->write((uint32_t*)&image[0][0]);
}
