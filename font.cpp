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
    
    SDL_Surface *tmp = TTF_RenderUTF8_Blended(font,buf,textcol);
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(Context::getInstance()->rdr,tmp);
    if(SDL_SetTextureBlendMode(tex,SDL_BLENDMODE_BLEND))
        FATAL("texture blend mode not supported");
    SDL_FreeSurface(tmp);
    
    int ww,hh;
    if(TTF_SizeUTF8(font,buf,&ww,&hh))
        FATAL("cannot get string size");
    float texw=ww,texh=hh;
        
    ERRCHK;
    
    float r = texw/texh;
    
    Region::current->renderQuad(x,y,h*r,h,tex);
    
    SDL_DestroyTexture(tex);
    
}
