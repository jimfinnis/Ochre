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
}

MapTex::~MapTex(){
    if(tex)glDeleteTextures(1,&tex);
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
    for(Person *p=pl->people.first();p;p=pl->people.next(p)){
        image[(int)p->y][(int)p->x] = col;
    }
    
}

void MapTex::make(Grid *g){
    uint32_t *p = &image[0][0];
    for(int y=0;y<GRIDSIZE;y++){
        for(int x=0;x<GRIDSIZE;x++){
            uint8_t h = g->grid[x][y];
            uint32_t col = g->mapvis[x][y] ? cols.colsvis[h] : cols.cols[h];
            // add objects
            if(g->objects[x][y])
                col = 0xff0000ff;
            *p++ = col;
        }
    }
    
    // add peeps
    
    if(1 && globals::game){
        writePlayer(&globals::game->p[0],0xffff0000);
        writePlayer(&globals::game->p[1],0xff00ff00);
    }
    copy();
}