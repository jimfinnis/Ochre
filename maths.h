#ifndef __MATHS_h
#define __MATHS_h

#include <assert.h>
#include "types.h"
#include "exception.h"

#include <math.h>
#include <string.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

///\addtogroup maths Maths Functions
///\@{
///

/// a definition of PI is always handy
const float PI	=	3.1415927f;
const float Log2 = 0.6931471805f;



/// handy interpolation function. Returns a value interpolated between a and b with parameter t.
/// Output is a if t=0, b if t=1

inline float interp(float a,float b,float t)
{
    return a*(1.0f-t) + b*t;
}

class RandomNumberGenerator
{
public:
    /// initialise the generator and seed with a given value, or a default.
    void init(uint32_t seed = 4357)
    {
        mLeft = -1;
        setSeed(seed);
    }
    
    /// reseed the generator with its original seed
    void reset()
    {
        setSeed(mOriginalSeed);
    }
    
    /// return the last value
    uint32_t repeat() const
    {
        return mLast;
    }
    
    /// get a random number 0..2^32-1
    uint32_t getRandom()
    {
        uint32_t y;
        
        if(--mLeft<0)
            mLast = rejuvenate();
        else
        {
            y = *mNext++;
            y ^= y>>11;
            y ^= (y<<7) & 0x9d2c5680U;
            y ^= (y<<15) & 0xefc60000U;
            mLast = y ^ (y>>18);
        }
        return mLast;
    }
    
    
    /// return -1 or 1
    
    float getRandomSign()
    {
        return (getRandom() & 0x100) ? -1 : 1;
    }
    
    /// get a random integer from 0-n
    uint32_t getInt(uint32_t n=0xffffffffU)
    {
        return getRandom() % n;
    }
    
    /// return an integer between a and b
    int32_t rangeInt(int32_t a,int32_t b)
    {
        if(a==b)return a;
        int32_t q = (int32_t)getInt((uint32_t)(b-a));
        return a+q;
    }
    
    /// return a float in the range 0-1
    
    float getFloat(float q = -1)
    {
        float f = getInt(0xffffffU);
        f /= (float)0xffffffU;
        if(q>=0)f *= q;
        return f;
    }
    
    /// return a float in the range a..b
    
    float range(float a,float b)
    {
        float q = getFloat();
        return q * (b-a) + a;
    }
    
    /// produce an array of values 0 - n-1 in random order.
    void juggle(
                int *array,		//!< array to juggle
                int n,			//!< number of elements
                bool fill=true,	//!< if true, fill the array with 0 - n-1 before juggling
                int swaps=50	//!< number of swaps
                )
    {
        int i;
        
        if(fill)
        {
            for(i=0;i<n;i++)
                array[i] = i;
        }
        
        for(i=0;i<swaps;i++)
        {
            int a = i%n;
            int b = getInt(n);
            
            int t = array[a];
            array[a] = array[b];
            array[b] = t;
        }
    }
    
    
    
    /// seed the generator with a given value
    void setSeed(uint32_t seed)
    {
        uint32_t x = (seed | 1U) & 0xFFFFFFFFU, *s = mState;
        int j;
        
        for(mLeft=0, *s++=x, j=STATELENGTH; --j;
            *s++ = (x*=69069U) & 0xFFFFFFFFU)	{	}
    }
    
private:
    
    static const int STATELENGTH = 624;	//!< length of state vector
    static const uint32_t M = 397;		//!< used in Rejuvenate()
    static const uint32_t K = 0x9908b0dfu;		//!< used in Rejuvenate()
    
    /// used in Rejuvenate()
    static uint32_t hiBit(uint32_t u)
    {
        return u & 0x8000000U;
    }
    
    /// used in Rejuvenate()
    static uint32_t loBit(uint32_t u)
    {
        return u & 1;
    }
    
    /// used in Rejuvenate()
    static uint32_t loBits(uint32_t u)
    {
        return u & 0x7fffffffU;
    }
    
    /// used in Rejuvenate()
    static uint32_t mixBits(uint32_t u,uint32_t v)
    {
        return hiBit(u)|loBits(v);
    }
    
    
    /// regenerate the state vector
    
    uint32_t rejuvenate(void)
    {
        uint32_t *p0=mState, *p2=mState+2, *pM=mState+M, s0, s1;
        int j;
        
        if(mLeft < -1)
            setSeed(mOriginalSeed);
        
        mLeft=STATELENGTH-1, mNext=mState+1;
        
        for(s0=mState[0], s1=mState[1], j=STATELENGTH-M+1; --j; s0=s1, s1=*p2++)
            *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
        
        for(pM=mState, j=M; --j; s0=s1, s1=*p2++)
            *p0++ = *pM++ ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
        
        s1=mState[0], *p0 = *pM ^ (mixBits(s0, s1) >> 1) ^ (loBit(s1) ? K : 0U);
        s1 ^= (s1 >> 11);
        s1 ^= (s1 <<  7) & 0x9D2C5680U;
        s1 ^= (s1 << 15) & 0xEFC60000U;
        return(s1 ^ (s1 >> 18));
    }
    
    
    
    int32_t mLeft;					//!< how much state vector left?
    uint32_t mState[STATELENGTH+1];	//!< state vector with spare room "to not violate ansi c" according to Rich's code!
    uint32_t mOriginalSeed;			//!< original seed
    uint32_t mLast;					//!< last generated value
    uint32_t *mNext;					//!< next ptr
};

/// instance of generator, initialised in MainApplicationClass::InitPostShell()
extern RandomNumberGenerator gRand;



///\@}
///

///\addtogroup colour Colour Manipulation
///\@{

///
/// this namespace is for colour manipulation functions. See \ref colchart "here" for an HSV colour chart.
///
namespace Col
{
enum FadeType {
    COLOURFADE_DIRECT,	//!< fades between the given hues without affecting s or v
          COLOURFADE_VIABLACK //!< fades lightness down between hue changes
};
/// convert a (0-1) HSV triplet into a (0-1) RGB triplet
void HSVtoRGB(float *r,float *g,float *b,float h,float s,float v);
///	convert a (0-1) H value assuming S=V=1 into a (0-1) RGB triplet
void H11toRGB(float *r,float *g,float *b,float h);

/// convert a (0-1) RGB triplet into a (0-1) HSV triplet
void RGBtoHSV(float *h,float *s,float *v,float r,float g,float b);
}

/// Matrix stack

class MatrixStack
{
    glm::mat4 stack[32];
    int ct;
    
public:
    void reset(); // sets stack to hold identity
    glm::mat4 *push(); // pushes stack, copies top and returns ptr
    void pop(); // pops stack and discards
    void mul(const glm::mat4 &m); // top = m*top
    void mulBack(const glm::mat4 &m); // top = top * m
    glm::mat4 *top(); // get top
    void identity(); // loads into top slot
};

/// perlin noise generator, see http://freespace.virgin.net/hugo.elias/models/m_perlin.htm

class PerlinNoise
{
public:
    void init(
              float persistence, //!< persistence value - 0.25 is calm, 1 is spiky!
              int octaves   //!< number of octaves
              )
    {
        mPersistence = persistence;
        mOctaves = octaves;
    }
    
    /// get a noise value for a given x
    
    
    
    float get(float x)
    {
        float total = 0;
        
        float freq = 1;
        float amp = 1;
        for(int i=0;i<mOctaves;i++)
        {
            total += interpolatedNoise(i,x*freq) * amp;
            
            freq *= 2;
            amp *= mPersistence;
        }
        
        return total;
    }
    
private:
    
    
    /// noise functions
    
    inline float noise(int n, //!< function number
                       int x //!< argument
                       )
    {
        x = (x<<13)^x;
        x += n*31;
        return (1.0f - ((x*(x*x*15731+789221)+1376312589)&0x7fffffff)/1073741824.0f);
    }
    
    
    inline float smoothedNoise(int n,float x)
    {
        return noise(n,(int)x)/2 + noise(n,(int)x-1)/4 + noise(n,(int)x+1)/4;
    }
    
    inline float interpolatedNoise(int n,float x)
    {
        int inx = (int)x;
        float fracx = x - inx;
        float v1 = smoothedNoise(n,inx);
        float v2 = smoothedNoise(n,inx+1);
        
        return interpolate(v1,v2,fracx);
    }
    
    inline float interpolate(float a,float b,float x)
    {
        float ft = x * 3.1415927f;
        float f = (1-cosf(ft))*0.5f;
        return a*(1-f)+b*f;
    }
    
    int mOctaves;
    float mPersistence;
    
};


/// 2D perlin noise generator, just a single call. Works rather differently from the one above!

namespace PerlinNoise2D
{
/// noise functions

// float Noise(int n, //!< function number
//             int x, //!< argument
//             int y //!< argument
//             )
// {
//     x = x+y*57;
//     x = (x<<13)^x;
//     x += n*31;
//     return (1.0 - ((x*(x*x*15731+789221)+1376312589)&0x7fffffff)/1073741824.0f);
// }
// 
// 
// float SmoothedNoise(int n,float x,float y)
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
// float interpolate(float a,float b,float x)
// {
//     float ft = x * 3.1415927;
//     float f = (1-cosf(ft))*0.5;
//     return a*(1-f)+b*f;
// }

// 
// float interpolatedNoise(int n,float x,float y)
// {
//     int inx = (int)x;
//     int iny = (int)y;
//     
//     float fracx = x - inx;
//     float fracy = y - iny;
//     
//     float v1 = SmoothedNoise(n,inx,iny);
//     float v2 = SmoothedNoise(n,inx+1,iny);
//     float v3 = SmoothedNoise(n,inx,iny+1);
//     float v4 = SmoothedNoise(n,inx+1,iny+1);
//     
//     float i1 = interpolate(v1,v2,fracx);
//     float i2 = interpolate(v3,v4,fracx);
//     
//     return interpolate(i1,i2,fracy);
// }
// 
// 

// /// get a noise value for a given x
// 
float get(float x,float y,int octaves,float persist);
//{
//    float total = 0;
//    
//    float freq = 1;
//    float amp = 1;
//    
//    for(int i=0;i<octaves;i++)
//    {
//        total += PerlinNoise2D::interpolatedNoise(i,x*freq,y*freq) * amp;
//        
//        freq *= 2;
//        amp *= persist;
//    }
//    
//    return total;
//}
//

}


/// hash function, used to generate type IDs from component names
/// and packed text parameter IDs from text names. Also used for checksums.

inline uint32_t hashfunction(const char *s,int length=0)
{
    uint32_t total = 0;
    uint32_t mult = 1;
    
    if(!length)length = strlen(s);
    
    for(int i=0;i<length;i++)
    {
        uint32_t q = *s++;
        q -= 32;
        q *= mult;
        mult *= 37;
        total += q;
    }
    return total;
}



/// 2D point

struct Point2D
{
    float x,y;
};


/// Kochanek-Bartels Splines

class KBSpline 
{
private:    
    // matrix elements
    float a,b,c,d,e,f,g,h,i,j,k,l;
public:
    /// initialisation. 0,0,0 gives Catmull-Rom, 1,0,0 = simple cubic, 0,0,1 = linear interpolation
    void init(
              float tension,	//!< +1 = tight, -1 = round
              float bias,		//!< +1 = post shoot, -1 = pre shoot
              float continuity //!< +1 = inverted corners, -1 = box corners
              )
    {
        float FT = tension;
        float FB = bias;
        float FC = continuity;
        
        float FFA=(1-FT)*(1+FC)*(1+FB);
        float FFB=(1-FT)*(1-FC)*(1-FB);
        float FFC=(1-FT)*(1-FC)*(1+FB);
        float FFD=(1-FT)*(1+FC)*(1-FB);
        
        // now get the matrix coefficients
        
        
        a=-FFA;
        b=(4+FFA-FFB-FFC);
        c=(-4+FFB+FFC-FFD);
        d=FFD;
        
        e=2*FFA;
        f=(-6-2*FFA+2*FFB+FFC);
        g=(6-2*FFB-FFC+FFD);
        h=-FFD;
        
        i=-FFA;
        j=(FFA-FFB);
        k=FFB;
        l=0;
        
    }
    
    
    /// 1D spline function
    
    inline float getSpline1D(
                             float p,			//!< control points
                             float q,
                             float r,
                             float s,
                             float t				//!< position of desired point between points[1] and points[2] - i.e. between 2nd and 3rd points
                             )
    {
        float t2 = t*t;
        float t3 = t2*t;
        
        return 0.5f * (
                       (a*p + b*q + c*r + d*s) * t3 + 
                       (e*p + f*q + g*r + h*s) * t2 +
                       (i*p + j*q + k*r + l*s) * t +
                       2*q);
        
        
    }
    
    
    /// calculate a point on a 2D spline
    inline void getSpline2D(
                            Point2D *out, 		//!< output
                            Point2D *p0,			//!< control points
                            Point2D *p1,
                            Point2D *p2,
                            Point2D *p3,
                            float t				//!< position of desired point between points[1] and points[2] - i.e. between 2nd and 3rd points
                            )
    {
        out->x = getSpline1D(p0->x,p1->x,p2->x,p3->x,t);
        out->y = getSpline1D(p0->y,p1->y,p2->y,p3->y,t);
    }
    
    /// calculate a point on a 3D spline
    inline void getSpline3D(
                            glm::vec4 *out, 		//!< output
                            glm::vec4 *p0,			//!< control points
                            glm::vec4 *p1,
                            glm::vec4 *p2,
                            glm::vec4 *p3,
                            float t				//!< position of desired point between points[1] and points[2] - i.e. between 2nd and 3rd points
                            )
    {
        out->x = getSpline1D(p0->x,p1->x,p2->x,p3->x,t);
        out->y = getSpline1D(p0->y,p1->y,p2->y,p3->y,t);
        out->z = getSpline1D(p0->z,p1->z,p2->z,p3->z,t);
        out->w = 1.0f;
    }
};



#endif
