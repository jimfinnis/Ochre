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

/*****************************************************************************
 * 
 * Constants declared in maths.h
 * 
 ****************************************************************************/

using namespace FastMaths;

const Vector Vector::X  = Vector(1,0,0,1);
const Vector Vector::Y  = Vector(0,1,0,1);
const Vector Vector::Z  = Vector(0,0,1,1);
const Vector Vector::ZERO  = Vector(0,0,0,1);

const Matrix Matrix::IDENTITY = Matrix(1,0,0,0,	0,1,0,0,	0,0,1,0,	0,0,0,1);
Matrix Matrix::Projection;

void Matrix::setProjection(float fov,float znear,float zfar,float aspect){
    /*
    fov *= 3.1415927f/180.0f;
    
    float f = 1.0f/tanf(fov/2.0f);
    
    Projection = Matrix(
                        f/aspect,	0,		0,				0,
                        0,			f,		0,				0,
                        0,			0,		zfar/(znear-zfar),	-1,
                        0,			0,		(znear*zfar)/(znear-zfar),0);	
    */
    
    float xymax = znear * tanf(fov*(3.1415927f/360.0f));
    float ymin = -xymax;
    float xmin = -xymax;
    
    float width = xymax - xmin;
    float height = xymax - ymin;
    
    float depth = zfar - znear;
    float q = -(zfar + znear) / depth;
    float qn = -2 * (zfar * znear) / depth;
    
    float w = 2 * znear / width;
    w = w / aspect;
    float h = 2 * znear / height;
    
    
    
    Projection.mf[0]  = w;
    Projection.mf[1]  = 0;
    Projection.mf[2]  = 0;
    Projection.mf[3]  = 0;
    
    Projection.mf[4]  = 0;
    Projection.mf[5]  = h;
    Projection.mf[6]  = 0;
    Projection.mf[7]  = 0;
    
    Projection.mf[8]  = 0;
    Projection.mf[9]  = 0;
    Projection.mf[10] = q;
    Projection.mf[11] = -1;
    
    Projection.mf[12] = 0;
    Projection.mf[13] = 0;
    Projection.mf[14] = qn;
    Projection.mf[15] = 0;
}


float Matrix::det() const
{
    return 0
          + m00 *
          ( 0 + m11 * ( m22 * m33 - m32 * m23 )
            - m12 * ( m21 * m33 - m31 * m23 )
            + m13 * ( m21 * m32 - m31 * m22 )
            )
          - m01 *
          ( 0 + m10 * ( m22 * m33 - m32 * m23 )
            - m12 * ( m20 * m33 - m30 * m23 )
            + m13 * ( m20 * m32 - m30 * m22 )
            )
          + m02 *
          ( 0 + m10 * ( m21 * m33 - m31 * m23 )
            - m11 * ( m20 * m33 - m30 * m23 )
            + m13 * ( m20 * m31 - m30 * m21 )
            )
          - m03 *
          ( 0 + m10 * ( m21 * m32 - m31 * m22 )
            - m11 * ( m20 * m32 - m30 * m22 )
            + m12 * ( m20 * m31 - m30 * m21 )
            );
}

/** Turns this matrix into the inverse of the input. Must be an affine transform. For 
 * inverting an arbitrary matrix see Matrix::FullInvert. */

void Matrix::invert(const Matrix &in)
{
    assert( this != &in );
    
    float det = 1.0f / in.det();
    
    m00 = det * ( in.m11 * in.m22 - in.m12 * in.m21 );
    m01 = det * ( in.m02 * in.m21 - in.m01 * in.m22 );
    m02 = det * ( in.m01 * in.m12 - in.m02 * in.m11 );
    m03 = 0.0f;
    
    m10 = det * ( in.m12 * in.m20 - in.m10 * in.m22 );
    m11 = det * ( in.m00 * in.m22 - in.m02 * in.m20 );
    m12 = det * ( in.m02 * in.m10 - in.m00 * in.m12 );
    m13 = 0.0f;
    
    m20 = det * ( in.m10 * in.m21 - in.m11 * in.m20 );
    m21 = det * ( in.m01 * in.m20 - in.m00 * in.m21 );
    m22 = det * ( in.m00 * in.m11 - in.m01 * in.m10 );
    m23 = 0.0f; 
    
    m30 = - ( in.m30 * m00 + in.m31 * m10 + in.m32 * m20 );
    m31 = - ( in.m30 * m01 + in.m31 * m11 + in.m32 * m21 );
    m32 = - ( in.m30 * m02 + in.m31 * m12 + in.m32 * m22 );
    m33 = 1.0f;
}



/** Make the current matrix into the inverse of an arbitarary matrix. Slow - use
 * Matrix::Invert() instead if you're using an affine transform.
 */

void Matrix::invertFull(const Matrix &in)
{
    assert( this != &in );
    
    // BASICALLY, we get each value from the transform of the determinants of the sub matrices transposed over the overall determinant.
    
    float m2031 = in.m20 * in.m31;
    float m2032 = in.m20 * in.m32;
    float m2033 = in.m20 * in.m33;
    float m2130 = in.m21 * in.m30;
    float m2132 = in.m21 * in.m32;
    float m2133 = in.m21 * in.m33;
    float m2230 = in.m22 * in.m30;
    float m2231 = in.m22 * in.m31;
    float m2233 = in.m22 * in.m33;
    float m2330 = in.m23 * in.m30;
    float m2331 = in.m23 * in.m31;
    float m2332 = in.m23 * in.m32;
    
    float m0011 = in.m00 * in.m11;
    float m0012 = in.m00 * in.m12;
    float m0013 = in.m00 * in.m13;
    float m0110 = in.m01 * in.m10;
    float m0112 = in.m01 * in.m12;
    float m0113 = in.m01 * in.m13;
    float m0210 = in.m02 * in.m10;
    float m0211 = in.m02 * in.m11;
    float m0213 = in.m02 * in.m13;
    float m0310 = in.m03 * in.m10;
    float m0311 = in.m03 * in.m11;
    float m0312 = in.m03 * in.m12;
    
    float detVal = 1.0f / (
                           ( m0011 * ( m2233 - m2332 ) - m0012 * ( m2133 - m2331 ) + m0013 * ( m2132 - m2231 ) )
                           - ( m0110 * ( m2233 - m2332 ) - m0112 * ( m2033 - m2330 ) + m0113 * ( m2032 - m2230 ) )
                           + ( m0210 * ( m2133 - m2331 ) - m0211 * ( m2033 - m2330 ) + m0213 * ( m2031 - m2130 ) )
                           - ( m0310 * ( m2132 - m2231 ) - m0311 * ( m2032 - m2230 ) + m0312 * ( m2031 - m2130 ) ));
    
    m00 = ( in.m11 * ( m2233 - m2332 ) + in.m12 * ( m2331 - m2133 ) + in.m13 * ( m2132 - m2231 ) ) * detVal;
    m01 = ( in.m10 * ( m2332 - m2233 ) + in.m12 * ( m2033 - m2330 ) + in.m13 * ( m2230 - m2032 ) ) * detVal;
    m02 = ( in.m10 * ( m2133 - m2331 ) + in.m11 * ( m2330 - m2033 ) + in.m13 * ( m2031 - m2130 ) ) * detVal;
    m03 = ( in.m10 * ( m2231 - m2132 ) + in.m11 * ( m2032 - m2230 ) + in.m12 * ( m2130 - m2031 ) ) * detVal;
    
    m10 = ( in.m01 * ( m2332 - m2233 ) + in.m02 * ( m2133 - m2331 ) + in.m03 * ( m2231 - m2132 ) ) * detVal;
    m11 = ( in.m00 * ( m2233 - m2332 ) + in.m02 * ( m2330 - m2033 ) + in.m03 * ( m2032 - m2230 ) ) * detVal;
    m12 = ( in.m00 * ( m2331 - m2133 ) + in.m01 * ( m2033 - m2330 ) + in.m03 * ( m2130 - m2031 ) ) * detVal;
    m13 = ( in.m00 * ( m2132 - m2231 ) + in.m01 * ( m2230 - m2032 ) + in.m02 * ( m2031 - m2130 ) ) * detVal;
    
    m20 = ( in.m31 * ( m0213 - m0312 ) + in.m32 * ( m0311 - m0113 ) + in.m33 * ( m0112 - m0211 ) ) * detVal;
    m21 = ( in.m30 * ( m0312 - m0213 ) + in.m32 * ( m0013 - m0310 ) + in.m33 * ( m0210 - m0012 ) ) * detVal;
    m22 = ( in.m30 * ( m0113 - m0311 ) + in.m31 * ( m0310 - m0013 ) + in.m33 * ( m0011 - m0110 ) ) * detVal;
    m23 = ( in.m30 * ( m0211 - m0112 ) + in.m31 * ( m0012 - m0210 ) + in.m32 * ( m0110 - m0011 ) ) * detVal;
    
    m30 = ( in.m21 * ( m0312 - m0213 ) + in.m22 * ( m0113 - m0311 ) + in.m23 * ( m0211 - m0112 ) ) * detVal;
    m31 = ( in.m20 * ( m0213 - m0312 ) + in.m22 * ( m0310 - m0013 ) + in.m23 * ( m0012 - m0210 ) ) * detVal;
    m32 = ( in.m20 * ( m0311 - m0113 ) + in.m21 * ( m0013 - m0310 ) + in.m23 * ( m0110 - m0011 ) ) * detVal;
    m33 = ( in.m20 * ( m0112 - m0211 ) + in.m21 * ( m0210 - m0012 ) + in.m22 * ( m0011 - m0110 ) ) * detVal;
    
}


void Matrix::setRotFromDirAndUp(const Vector &dir, const Vector &up )
{
    Vector X,Y,Z;
    Z.normalize(dir);
    X.cross(up,Z);
    
    if(X.lensq()<VECTOREPSILON)
    {
        X.set(1,0,0);
    }
    else
    {
        X.normalizeInPlace();
    }
    
    Y.cross(Z,X);
    
    m00 = X.x;
    m10 = Y.x;
    m20 = Z.x;
    
    m01 = X.y;
    m11 = Y.y;
    m21 = Z.y;
    
    m02 = X.z;
    m12 = Y.z;
    m22 = Z.z;
}

/** The up vector is Y unless set otherwise.
 */

void Matrix::setFromPosAndTarget(const Vector &pos, const Vector &target,const Vector &up)
{
    Vector dir;
    
    dir = target - pos;
    setRotFromDirAndUp(dir,up);
    setTranslation(pos);
}






/// convert a (0-1) HSV triplet into a (0-1) RGB triplet
void Colour::HSVtoRGB(float *r,float *g,float *b,float h,float s,float v)
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
void Colour::H11toRGB(float *r,float *g,float *b,float h)
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
void Colour::RGBtoHSV(float *h,float *s,float *v,float r,float g,float b)
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


void FLOATRGBA::setFromHSV(float h,float s,float v,float _a)
{
    Colour::HSVtoRGB(&r,&g,&b,h,s,v);
    a=_a;
}

void FLOATRGBA::setFromH11(float h)
{
    Colour::H11toRGB(&r,&g,&b,h);
    a=1;
}

void FLOATRGBA::interpolate(FLOATRGBA &p,FLOATRGBA &q,float t)
{
    r = FastMaths::interp(p.r,q.r,t);
    g = FastMaths::interp(p.g,q.g,t);
    b = FastMaths::interp(p.b,q.b,t);
    a = FastMaths::interp(p.a,q.a,t);
}

void FLOATRGBA::setToComplement(FLOATRGBA &c)
{
    float h,s,v;
    Colour::RGBtoHSV(&h,&s,&v,c.r,c.g,c.b);
    setFromHSV(1-h,s,v,c.a);
}




/*****************************************************************************
 * 
 * Reg poly
 * 
 ****************************************************************************/

RegPolyVertManagerClass gRegPolyVertManager;

int RegPolyVertManagerClass::sVertCount[] = {4,6,8,12,20};

void RegPolyVertManagerClass::setRotation(Matrix *m,float x,float y,float z)
{
    Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.normalizeInPlace();
    
    m->setRotFromDirAndUp(v,Vector::Y);
}

void RegPolyVertManagerClass::init()
{
    for(int i=0;i<TYPES;i++)
        mRotArray[i] = new Matrix [sVertCount[i]];
    
    
    
    Matrix *m;
    
    // tetrahedron
    
    m = mRotArray[TETRA];
    
    setRotation(m+0,		-1,-1,-1);
    setRotation(m+1,		-1,1,1);
    setRotation(m+2,		1,-1,1);
    setRotation(m+3,		1,1,-1);
    
    // cube
    
    m = mRotArray[CUBE];
    
    setRotation(m+0,		-1,-1,-1);
    setRotation(m+1,		-1,-1,1);
    setRotation(m+2,		-1,1,-1);
    setRotation(m+3,		-1,1,1);
    setRotation(m+4,		1,-1,-1);
    setRotation(m+5,		1,-1,1);
    setRotation(m+6,		1,1,-1);
    setRotation(m+7,		1,1,1);
    
    
    // octahedron
    
    m = mRotArray[OCTA];
    
    setRotation(m+0,		-1,0,0);
    setRotation(m+1,		1,0,0);
    setRotation(m+2,		0,-1,0);
    setRotation(m+3,		0,1,0);
    setRotation(m+4,		0,0,-1);
    setRotation(m+5,		0,0,1);
    
    
    // icosahedron
    
    m = mRotArray[ICOS];
    float G = 1.6180339887f;		// the golden ratio!
    
    setRotation(m+0,		0,G,1);	
    setRotation(m+1,		0,-G,1);
    setRotation(m+2,		0,G,-1);
    setRotation(m+3,		0,-G,-1);
    setRotation(m+4,		1,0,G);
    setRotation(m+5,		-1,0,G);
    setRotation(m+6,		1,0,-G);
    setRotation(m+7,		-1,0,-G);
    setRotation(m+8,		G,1,0);
    setRotation(m+9,		-G,1,0);
    setRotation(m+10,	G,-1,0);
    setRotation(m+11,	-G,-1,0);
    
    
    // dodecahedron
    
    m = mRotArray[DODEC];
    float RG = 1.0f/G;				// 1/the golden ratio
    
    setRotation(m+0,	-1,-1,-1);
    setRotation(m+1,	-1,-1,1);
    setRotation(m+2,	-1,1,-1);
    setRotation(m+3,	-1,1,1);
    setRotation(m+4,	1,-1,-1);
    setRotation(m+5,	1,-1,1);
    setRotation(m+6,	1,1,-1);
    setRotation(m+7,	1,1,1);
    
    setRotation(m+8,	0,RG,G);
    setRotation(m+9,	0,-RG,G);
    setRotation(m+10,	0,RG,-G);
    setRotation(m+11,	0,-RG,-G);
    
    setRotation(m+12,	G,0,RG);
    setRotation(m+13,	-G,0,RG);
    setRotation(m+14,	G,0,-RG);
    setRotation(m+15,	-G,0,-RG);
    
    setRotation(m+16,	RG,G,0);
    setRotation(m+17,	-RG,G,0);
    setRotation(m+18,	RG,-G,0);
    setRotation(m+19,	-RG,-G,0);
    
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

Matrix *MatrixStack::push()
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
void MatrixStack::mul(Matrix *m)
{
    stack[ct] = (*m) * stack[ct];
}

void MatrixStack::mulBack(Matrix *m)
{
    stack[ct] = stack[ct] * (*m);
}

Matrix *MatrixStack::top()
{
    return stack+ct;
}

void MatrixStack::identity()
{
    stack[ct] = Matrix::IDENTITY;
}
