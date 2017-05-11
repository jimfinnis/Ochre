/**
 * @file font.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include <stdarg.h>

#include "context.h"
#include "font.h"
#include "exception.h"

Font::Font(const char *file,int size){
    font = TTF_OpenFont(file,size);
    if(!font)
        throw Exception().set("Cannot open %s at size %d",file,size);
}

void Font::init(){
    TTF_Init();
}    

static const SDL_Color textcol = {255,255,255,255};

void Font::render(float x,float y,float h,const char *s,...){
    char buf[1024];
    va_list args;
    va_start(args,s);
        
    vsnprintf(buf,1024,s,args);
    va_end(args);
    
    // yes, this is vile. Render, creating a new surface, create a
    // new gl texture from that surface, render that, delete both.
    // SDL_GL_CreateTexture probably does this underneath, but it
    // creates rectangle textures which my laptop (see elsewhere)
    // doesn't like.
    
    // blended creates 32-bit RGBA
    
    SDL_Surface *tmp = TTF_RenderUTF8_Blended(font,buf,textcol);
    GLuint tex;
    glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D,tex);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,tmp->w,tmp->h,0,GL_RGBA,GL_UNSIGNED_BYTE,
                 tmp->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);    
    
    SDL_FreeSurface(tmp);
    
    int ww,hh;
    if(TTF_SizeUTF8(font,buf,&ww,&hh))
        FATAL("cannot get string size");
    float texw=ww,texh=hh;
        
    ERRCHK;
    
    float r = texw/texh;
    
    Region::current->renderQuad(x,y,h*r,h,tex);
    
    glDeleteTextures(1,&tex);
}
