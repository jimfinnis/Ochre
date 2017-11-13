/**
 * @file button.cpp
 * @brief  Brief description of file.
 *
 */

#include "button.h"
#include "texture.h"
#include "state.h"
#include "region.h"


Button::Button(int i,Region *reg,const char *texname,int bx,int by,int bw,int bh){
    Texture *t = TextureManager::getInstance()->createOrFind(texname);
    tex = t->getID();
    id = i;
    r = reg;
    x = bx; y = by; 
    w = bw>0?bw:t->mWidth; 
    h = bh>0?bh:t->mHeight;
    setid = -1;
    highlight = false;
}

void Button::render(){
    StateManager *sm = StateManager::getInstance();
    sm->push();
    State *s = sm->get();
    s->setDefault();
    MatrixStack *ms = sm->getx();
    ms->push();
    ms->identity();
    int xx=x,yy=y,ww=w,hh=h;
    if(highlight){
        static const int HLSIZE=10;
        xx-=HLSIZE;yy-=HLSIZE;ww+=HLSIZE*2;hh+=HLSIZE*2;
    }
    
    // render the buttons "upside-down" because of the weird coord space
    r->renderQuad(xx,yy+hh,ww,-hh,tex);
    ms->pop();
    sm->pop();
}
