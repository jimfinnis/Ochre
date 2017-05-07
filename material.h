/**
 * @file material.h
 * @brief  Brief description of file.
 *
 */

#ifndef __MATERIAL_H
#define __MATERIAL_H

struct Material {
    SDL_Texture *t; // will probably always be null
    float diffuse[4];
    
    Material() : Material(NULL,1,1,1){}
        
    Material(SDL_Texture *tt,float r,float g,float b,float a=1){
        t = tt;
        diffuse[0] = r;
        diffuse[1] = g;
        diffuse[2] = b;
        diffuse[3] = a;
    }
        
};

// used for material transitions inside meshes
struct Transition{
    Transition(){
        start=count=matidx=0;
    }
    Transition(int s,int c,int m){
        start = s;
        count = c;
        matidx = m;
    }
    int start,count,matidx;
};



#endif /* __MATERIAL_H */
