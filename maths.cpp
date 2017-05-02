// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  			Copyright 2004 Broadsword Interactive.
//			      All Rights Reserved
//
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : <unknown>
//  Created       : Wed Jul 14 14:22:46 2004
//  Last Modified : <:40714.1447>
//
//  Description	
//
//  Notes
//
//  History
//	
//  $Log$
//
/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004 Broadsword Interactive.
// 
//  All Rights Reserved.
// 
// This  document  may  not, in  whole  or in  part, be  copied,  photocopied,
// reproduced,  translated,  or  reduced to any  electronic  medium or machine
// readable form without prior written consent from Broadsword Interactive.
//
//////////////////////////////////////////////////////////////////////////////

#include "maths.h"



/// convert a (0-1) HSV triplet into a (0-1) RGB triplet
void Col::HSVtoRGB(float *r,float *g,float *b,float h,float s,float v)
{
    h = fmodf(h,1);
    if(h<0)h=1+h;
    if(h<0.0001f)h=0.0001f;
    if(h>0.9999f)h=0.9999f;
    if(s>1)s=1;
    if(v>1)v=1;
    
    h = 6*h;
    
    float i = floorf(h);
    float f = h-i;
    
    float m = v*(1-s);
    float n = v*(1-(s*f));
    float k = v*(1-(s*(1-f)));
    
    switch((int)i)
    {
    case 0:
        *r=v;*g=k;*b=m;break;
    case 1:
        *r=n;*g=v;*b=m;break;
    case 2:
        *r=m;*g=v;*b=k;break;
    case 3:
        *r=m;*g=n;*b=v;break;
    case 4:
        *r=k;*g=m;*b=v;break;
    case 5:
        *r=v;*g=m;*b=n;break;
    }
}


/// convert a (0-1) H value assuming S=V=1 into a (0-1) RGB triplet
void Col::H11toRGB(float *r,float *g,float *b,float h)
{
    if(h<0.0001f)h=0.0001f;
    if(h>0.9999f)h=0.9999f;
    
    h = 6*h;
    
    float i = floorf(h);
    float f = h-i;
    float n = 1-f;
    
    switch((int)i)
    {
    case 0:
        *r=1;*g=f;*b=0;break;
    case 1:
        *r=n;*g=1;*b=0;break;
    case 2:
        *r=0;*g=1;*b=f;break;
    case 3:
        *r=0;*g=n;*b=1;break;
    case 4:
        *r=f;*g=0;*b=1;break;
    case 5:
        *r=1;*g=0;*b=n;break;
    }
}



/// convert a (0-1) RGB triplet into a (0-1) HSV triplet
void Col::RGBtoHSV(float *h,float *s,float *v,float r,float g,float b)
{
    float max = r;
    if(g>max)max=g;
    if(b>max)max=b;
    
    *v = max;
    
    if(!max)
    {
        *h = *s = *v = 0;
        return;
    }
    
    float min = r;
    if(g<min)min=g;
    if(b<min)min=b;
    
    float delta = max-min;
    
    *s = delta / max;
    
    if(!*s)
    {
        *h = 0;
        return;
    }
    
    //	r = (max-r)/delta;
    //	g = (max-g)/delta;
    //	b = (max-b)/delta;
    
    float hue;
    if(r==max)
        hue = (g-b)/delta;
    else if(g==max)
        hue = 2.0f+(b-r)/delta;
    else
        hue = 4.0f+(r-g)/delta;
    
    hue *= 60.0f;
    if(hue<0)hue+=360.0f;
    *h = hue/360.0f;
}


void Colour::setFromHSV(float h,float s,float v,float _a)
{
    Col::HSVtoRGB(&r,&g,&b,h,s,v);
    a=_a;
}

void Colour::setFromH11(float h)
{
    Col::H11toRGB(&r,&g,&b,h);
    a=1;
}

void Colour::interpolate(Colour &p,Colour &q,float t)
{
    r = interp(p.r,q.r,t);
    g = interp(p.g,q.g,t);
    b = interp(p.b,q.b,t);
    a = interp(p.a,q.a,t);
}

void Colour::setToComplement(Colour &c)
{
    float h,s,v;
    Col::RGBtoHSV(&h,&s,&v,c.r,c.g,c.b);
    setFromHSV(1-h,s,v,c.a);
}





namespace PerlinNoise2D
{
/// noise functions

inline float noise(int n, //!< function number
                   int x, //!< argument
                   int y //!< argument
                   )
{
    x = x+y*57;
    x = (x<<13)^x;
    x += n*31;
    return ( 1.0f - ((x*(x*x*15731+789221)+1376312589)&0x7fffffff)/1073741824.0f);
}

// inline float SmoothedNoise(int n,float x,float y)
// {
//     float corners = (Noise(n,(int)x-1,(int)y-1) 
//+ Noise(n,(int)x+1,(int)y-1)
//+ Noise(n,(int)x-1,(int)y+1)
//+ Noise(n,(int)x+1,(int)y+1)) * 0.0625;
//     float sides = (Noise(n,(int)x-1,(int)y)
//+ Noise(n,(int)x+1,(int)y)
//+ Noise(n,(int)x,(int)y-1) 
//+ Noise(n,(int)x,(int)y+1)) * 0.125;
//     return corners + sides + Noise(n,(int)x,(int)y);
// }

inline float interpolate(float a,float b,float x)
{
    float ft = x * 3.1415927f;
    float f = (1-cosf(ft))*0.5f;
    return a*(1-f)+b*f;
}


inline float interpolatedNoise(int n,float x,float y)
{
    // n is both the level changer, and the 2^n multiplier for size of the map at this level        
    {
        int size = 1 << n;
        
        x /= size;
        y /= size;
    }
    
    int inx = (int)x;
    int iny = (int)y;
    
    float fracx = x - inx;
    float fracy = y - iny;
    
    float v1 = noise(n,inx,iny);
    float v2 = noise(n,inx+1,iny) * fracx;
    float v3 = noise(n,inx,iny+1) * fracy;
    float v4 = noise(n,inx+1,iny+1) * fracx * fracy;
    
    float c2 = ( 1.0f - fracy );
    float c3 = ( 1.0f - fracx );
    float c1 = c2*c3;//( 1.0f - fracx ) * ( 1.0f - fracy );
    //float c4 = fracx * fracy;
    
    //float v1 = noise(n,inx,iny);
    //      float v2 = noise(n,inx+1,iny);
    //      float v3 = noise(n,inx,iny+1);
    //      float v4 = noise(n,inx+1,iny+1);
    //      
    //float c1 = ( 1.0f - fracx ) * fracy;
    //float c2 = fracx * fracy;
    //float c3 = ( 1.0f - fracx ) * ( 1.0f - fracy );
    //float c4 = fracx * ( 1.0f - fracy );
    
    //float i1 = interpolate(v1,v2,fracx);
    //float i2 = interpolate(v3,v4,fracx);
    
    return v1*c1 + v2*c2 + v3*c3 + v4;//interpolate(i1,i2,fracy);
}



/// get a noise value for a given x

float get(float x,float y,int octaves,float persist)
{
    const float xUpScale = 4.0f;
    const float yUpScale = 14.0f;
    x *= xUpScale; x += 421.752f;
    y *= yUpScale; x += 15689.45263f;
    float total = 0;
    
    //float freq = 1;
    //float amp = 1;
    
    for(int i=0;i<octaves;i++)
    {
        //total += PerlinNoise2D::interpolatedNoise(i,x*freq,y*freq) * amp;
        total += PerlinNoise2D::interpolatedNoise(i,x,y);// * amp;
        total *= persist;
        
        //freq *= 2;
        //amp *= persist;
    }
    
    return total;
}


}

RandomNumberGenerator gRand;

void MatrixStack::reset()
{
    ct=0;
    identity();
}

glm::mat4 *MatrixStack::push()
{
    ct++;
    if(ct==32)
        FATAL("stack overflow");
    stack[ct]=stack[ct-1];
    return stack+ct;
}

void MatrixStack::pop()
{
    ct--;
    if(ct<0)
        FATAL("stack underflow");
}
void MatrixStack::mul(const glm::mat4& m)
{
    stack[ct] = m * stack[ct];
}

void MatrixStack::mulBack(const glm::mat4& m)
{
    stack[ct] = stack[ct] * m;
}

glm::mat4 *MatrixStack::top()
{
    return stack+ct;
}

void MatrixStack::identity()
{
    stack[ct] = glm::mat4();
}
