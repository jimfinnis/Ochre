#ifndef __MATHS_h
#define __MATHS_h

#include <assert.h>
#include "types.h"
#include "exception.h"

#include <math.h>
#include <string.h>

///\addtogroup maths Maths Functions
///\@{
///

/// a definition of PI is always handy
const float PI	=	3.1415927f;
const float Log2 = 0.6931471805f;


/// this is a namespace for fast maths functions
///\todo rewrite so it's actually fast!

namespace FastMaths
{

inline void sinAndCosPtr( float z, float * sinz, float * cosz )
{
    while( z > 2.0f * PI )
        z -= 2.0f * PI;
    if( z < PI )
    {	if( z < PI * 0.5f )
    {	float z2 = z*z;
        *sinz = z - z2*z*0.16666f+z2*z2*z*0.008333333f;
        *cosz = 1.0f - z2*0.5f + z2*z2*0.04166666f;
    } else
    {	z = PI - z;
        float z2 = z*z;
        *sinz = z - z2*z*0.16666f+z2*z2*z*0.008333333f;
        *cosz = z2*0.5f - 1.0f - z2*z2*0.04166666f;
    }
    } else
    {	if( z < PI * 1.5f )
    {	float z2 = z*z;
        z -= PI;
        *sinz = z*z*z*0.16666f - z - z2*z2*z*0.008333333f;
        *cosz = z2*0.5f - 1.0f - z2*z2*0.04166666f;
    } else
    {	z = ( PI * 2.0f ) - z;
        float z2 = z*z;
        *sinz = z*z*z*0.16666f - z - z2*z2*z*0.008333333f;
        *cosz = 1.0f - z*z*0.5f + z2*z2*0.04166666f;
    }
    }
}

inline float fastSin( float angle )
{
    float sin,cos;
    sinAndCosPtr( angle, &sin, &cos );
    return sin;
}

inline float fastCos( float angle )
{
    float sin,cos;
    sinAndCosPtr( angle, &sin, &cos );
    return cos;
}

/// generate the sin and cosine of a given angle
inline void sinAndCos(float a,float *s,float *c)
{
    *s = sinf(a);
    *c = cosf(a);
}


// fast functions

inline float fmod2PI(float a)
{
    a *= 1.0f / (2*PI); // divide by 2*pi
    int q = (int)a;
    a -= q;
    a*= 2*PI;
    return a;
}


/// handy interpolation function. Returns a value interpolated between a and b with parameter t.
/// Output is a if t=0, b if t=1

inline float interp(float a,float b,float t)
{
    return a*(1.0f-t) + b*t;
}

/// fast, inaccurate sin(). Only works when 2PI<=theta<=2PI
inline float sinQ(float fAngle)
{
    bool neg = false;
    
    if(fAngle<0)
    {
        fAngle *= -1;
        neg=true;
    }
    
    if(fAngle>2*PI)
        fAngle = fmod2PI(fAngle);
    
    if(fAngle>PI*1.5f)
    {
        
        fAngle = 2*PI - fAngle;
        neg=!neg;
    }
    else if(fAngle>PI)
    {
        fAngle -= PI;
        neg = !neg;
    }
    else if(fAngle>0.5f*PI)
    {
        fAngle = PI - fAngle;
    }
    
    float fASqr = fAngle*fAngle;
    float fResult = 7.61e-03f;
    fResult *= fASqr;
    fResult -= 1.6605e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    fResult *= fAngle;
    if(neg)
        return -1.0f * fResult;
    else
        return fResult;
}

/// fast, inaccurate cos(). Only works when 2PI<=theta<=2PI
inline float cosQ(float fAngle)
{
    bool neg = false;
    
    if(fAngle<0)
    {
        fAngle *= -1;
    }
    
    if(fAngle>2*PI)
        fAngle = fmod2PI(fAngle);
    
    if(fAngle>PI*1.5f)
        fAngle = 2*PI - fAngle;
    else if(fAngle>PI)
    {
        fAngle -= PI;
        neg = true;
    }
    else if(fAngle>0.5f*PI)
    {
        fAngle = PI - fAngle;
        neg = true;
    }
    
    float fASqr = fAngle*fAngle;
    float fResult = 3.705e-02f;
    fResult *= fASqr;
    fResult -= 4.967e-01f;
    fResult *= fASqr;
    fResult += 1.0f;
    
    if(neg)
        return -1.0f * fResult;
    else
        return fResult;
}


/// fast, inaccurate tan(). Only works when 2PI<=theta<=2PI
inline float tanQ(float fAngle)
{
    return sinQ(fAngle)/cosQ(fAngle);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

/// very fast inverse sqrt
inline float invSqrt(float a) 
{
    float half = 0.5f * a;
    
    // IEEE hack
    
    uint32_t i = *(uint32_t *)&a;
    i = 0x5f3759df - (i>>1);
    a = *(float *)&i;
    
    // Newton-Raphson iterations
    
    a = a * (1.5f - half*a*a);
    a = a * (1.5f - half*a*a);
    
    return a;
}
/*A much faster but even less accurate version can be written
   by omitting the quadratic refinement. This reduces the time
   from 306 to 72 microseconds, which is more than ten times
   faster than the library function.*/

inline float fastLog2(float i)
{
    const float shift23 = (1<<23);
    const float OOshift23=1.0f/shift23;
    float x;
    x=*(int32_t *)&i;
    x*= OOshift23; 
    return x-127.f;
}
#pragma GCC diagnostic pop


/// generate the inaccurate sin and cosine of a given angle
inline void sinAndCosQ(float a,float *s,float *c)
{
    *s = sinQ(a);
    *c = cosQ(a);
}


}

///\addtogroup rand Random Number Generator
///@{
/// Random number generator. There is a global generator, gRand, but there
/// may also be others. The number is seeded with a uint32_t - the best are odd and 1..2^32-1 (to which this limits the input)


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
namespace Colour
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

///\@}
///


/*****************************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * Vectors
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 ****************************************************************************/

//!\addtogroup linear Linear Algebra Structures
//!\@{

static const float VECTOREPSILON = 0.0001f; //!< very small value for comparisons to avoid div by zero, etc.

/// Vector structure

struct Vector
{
    union {
        struct {
            float x,y,z,w;
        };
        struct {
            float r,g,b,a;
        };
        float v[4];
        uint32_t u[4];
    };
    
    void dump(){
        printf("[ %f %f %f %f ]\n",x,y,z,w);
    }
    
    Vector(){}
    
    
    static const Vector X;		//!< the positive x-axis, vector(1,0,0,1)
    static const Vector Y;		//!< the positive y-axis, vector(0,1,0,1)
    static const Vector Z;		//!< the positive z-axis, vector(0,0,1,1)
    static const Vector ZERO;	//!< the zero vector, vector(0,0,0,1)
    
    
    /// set the vector, set w to 1
    inline void set(float _x,float _y,float _z)
          
    {
        x = _x;
        y = _y;
        z = _z;
        w = 1;
    }
    
    /// set the vector
    void set(float _x,float _y,float _z,float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }
    
    /// set the vector, set w to 1
    Vector(float _x,float _y,float _z){
        set(_x,_y,_z);
    }
    Vector(float _x,float _y,float _z,float _w){
        set(_x,_y,_z,_w);
    }
        
    
    /// set only 3 elements - use this for vec() and norm() from a V3DUnlit vertex
    void set3(float _x,float _y,float _z)
          
    {
        x = _x;
        y = _y;
        z = _z;
    }
    
    /// return a pointer to the first float
    float *ptr(){return &x;}
    
    /// use this vector like an array of floats
    float &operator[]( int id ) { return (&x)[ id ]; }
    
    /// add two vectors. Set the w to 1.
    friend Vector operator + (const Vector& a,const Vector &b)
    {
        Vector r;
        r.x = a.x + b.x;
        r.y = a.y + b.y;
        r.z = a.z + b.z;
        
        r.w = 1;
        
        return r;
        
    }
    
    /// subtract two vectors. Set the w to 1.
    friend Vector operator - (const Vector &a, const Vector &b)
    {
        Vector r;
        r.x = a.x - b.x;
        r.y = a.y - b.y;
        r.z = a.z - b.z;
        
        r.w = 1;
        
        return r;
        
    }
    
    /// add a vector to the current vector. Do nothing with w.
    void operator += (const Vector &a)
    {
        x += a.x;
        y += a.y;
        z += a.z;
    }
    
    /// subtract a vector from the current vector. Do nothing with w.
    void operator -= (const Vector &a)
    {
        x -= a.x;
        y -= a.y;
        z -= a.z;
    }
    
    /// negate a vector, leaving w intact
    friend Vector operator - (const Vector &a)
    {
        Vector r;
        r.x = -a.x;
        r.y = -a.y;
        r.z = -a.z;
        r.w = a.w;
        
        return r;
    }
    
    /// scale a vector in place
    void operator *= (const float f)
    {
        x *= f;
        y *= f;
        z *= f;
    }
    
    /// scale a vector, set w to 1
    friend Vector operator * (const Vector &a, float f)
    {
        Vector r;
        r.x = a.x * f;
        r.y = a.y * f;
        r.z = a.z * f;
        r.w = 1;
        return r;
    }
    
    
    /// dot (scalar) product
    friend float dot(const Vector &a,const Vector &b)
    {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }
    
    /// make this vector the cross (vector) product of two others
    void cross(const Vector &a, const Vector &b)
    {
        assert(this!=&a && this!=&b);
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
        w = 1;
    }
    
    /// return magnitude of vector
    float len() const
    {
        return 1.0f/FastMaths::invSqrt(x*x + y*y + z*z);
    }
    
    /// return length-squared of vector
    float lensq() const
    {
        return x*x + y*y + z*z;
    }
    
    /// normalise in place. Do not change w.
    void normalizeInPlace()
    {
        float r = FastMaths::invSqrt(x*x + y*y + z*z);
        x *= r;
        y *= r;
        z *= r;
    }
    
    /// make this vector a normalisation of another. Does not write w.
    void normalize(const Vector &v)
    {
        *this = v;
        normalizeInPlace();
    }
    
    
    /// Make this vector an interpolation between other two vectors. Hold w to 1.
    void interpolate(const Vector &a,const Vector &b,float t)
    {
        float t2 = (1.0f - t);
        x = t2*a.x + t*b.x;
        y = t2*a.y + t*b.y;
        z = t2*a.z + t*b.z;
        w = 1;
    }
    
    /// make this vector from another vector transformed by a matrix, set w to 1
    inline void transform(const Vector &invec,const struct Matrix &inmatrix);
    
    /// make this vector from another vector transformed by a matrix, and generate w
    inline void transformFull(const Vector &invec,const struct Matrix &inmatrix);
    
    /// make this vector from another vector transformed by a 3x3 rotation matrix
    inline void transformRotationOnly(const Vector &invec,const struct Matrix &inmatrix);
    
    inline void transformInPlace(const Matrix &m){
        Vector v;
        v.transform(*this,m);
        *this = v;
    }
        
    
} ;

/**/

/*****************************************************************************
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * Matrices
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 ****************************************************************************/

/// this is a rotation order, which is used in Matrix::MakeRotate()
typedef int RotateOrder;

/// return the reverse of a given rotation order
inline RotateOrder reverseRotateOrder(RotateOrder o) { return (RotateOrder)((((int)o)+3)%6);}



/// Non-VU0 Matrix type. All the comments for the Vector about apply to this too.

struct Matrix
{
    union{
        struct {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
        float mf[16];
    };
    
    Matrix(){}
    
    Matrix(const Matrix &a){
        for(int i=0;i<16;i++)mf[i]=a.mf[i];
    }
    
    Matrix(float a,float b,float c,float d,
           float e,float f,float g,float h,
           float i,float j,float k,float l,
           float m,float n,float o,float p){
        m00=a;m01=b;m02=c;m03=d;
        m10=e;m11=f;m12=g;m13=h;
        m20=i;m21=j;m22=k;m23=l;
        m30=m;m31=n;m32=o;m33=p;
    }
    
    void dump(){
        printf("[ %2.2f %2.2f %2.2f %2.2f\n",m00,m01,m02,m03);
        printf("  %2.2f %2.2f %2.2f %2.2f\n",m10,m11,m12,m13);
        printf("  %2.2f %2.2f %2.2f %2.2f\n",m20,m21,m22,m23);
        printf("  %2.2f %2.2f %2.2f %2.2f ]\n\n",m30,m31,m32,m33);
    }
    
    
    
    /*****************************************************************************
     * 
     * Constants
     * 
     ****************************************************************************/
    
    static const Matrix IDENTITY; //!< the identity matrix
    static Matrix Projection; //!< a sensible projection matrix set up by setProjection()
    
    static const RotateOrder ROT_XYZ = (const RotateOrder)0;		//!< rotation order for use in MakeRotate()
    static const RotateOrder ROT_XZY = (const RotateOrder)1;		//!< rotation order for use in MakeRotate()
    static const RotateOrder ROT_ZXY = (const RotateOrder)2;		//!< rotation order for use in MakeRotate()
    static const RotateOrder ROT_ZYX = (const RotateOrder)3;		//!< rotation order for use in MakeRotate()
    static const RotateOrder ROT_YZX = (const RotateOrder)4;		//!< rotation order for use in MakeRotate()
    static const RotateOrder ROT_YXZ = (const RotateOrder)5;		//!< rotation order for use in MakeRotate()
    
#define REVORDER(o) (((o)+3)%6)
    
    /// set the projection matrix
    static void setProjection(float fov,float znear,float zfar,float faspect);
    
    
    /// return a pointer to the first float in the matrix
    float *ptr()
    {
        return &m00;
    }
    
    /// return true if the matrices are equal
    friend bool operator == (const Matrix &a,const Matrix &b)
    {
        if(a.m00 == b.m00 &&
           a.m01 == b.m01 &&
           a.m02 == b.m02 &&
           a.m03 == b.m03 &&
           a.m10 == b.m10 &&
           a.m11 == b.m11 &&
           a.m12 == b.m12 &&
           a.m13 == b.m23 &&
           a.m20 == b.m20 &&
           a.m21 == b.m21 &&
           a.m22 == b.m22 &&
           a.m23 == b.m23 &&
           a.m30 == b.m30 &&
           a.m31 == b.m31 &&
           a.m32 == b.m32 &&
           a.m33 == b.m33)
            return true;
        return false;
    }
    
    /// return true if the matrices are not equal
    friend bool operator != (const Matrix &a,const Matrix &b)
    {
        return !(a==b);
    }
    
    /// set to identity
    void identity(){
        *this = Matrix::IDENTITY;
    }
    
    
    /// PARTIAL matrix multiplication. Assumes column 4 is 0,0,0,1. Makes this the product of the two inputs.
    void mul(const Matrix &inmatrix1, const Matrix &inmatrix2)
    {
        assert(this != &inmatrix1 && this!=&inmatrix2);
        m00 = inmatrix1.m00 * inmatrix2.m00 + inmatrix1.m01 * inmatrix2.m10 + inmatrix1.m02 * inmatrix2.m20; // + inmatrix1.m03 * inmatrix2.m30;
        m01 = inmatrix1.m00 * inmatrix2.m01 + inmatrix1.m01 * inmatrix2.m11 + inmatrix1.m02 * inmatrix2.m21; // + inmatrix1.m03 * inmatrix2.m31;
        m02 = inmatrix1.m00 * inmatrix2.m02 + inmatrix1.m01 * inmatrix2.m12 + inmatrix1.m02 * inmatrix2.m22; // + inmatrix1.m03 * inmatrix2.m32;
        m03 = 0.0f; //inmatrix1.m00 * inmatrix2.m03 + inmatrix1.m01 * inmatrix2.m13 + inmatrix1.m02 * inmatrix2.m23 + inmatrix1.m03 * inmatrix2.m33;
        m10 = inmatrix1.m10 * inmatrix2.m00 + inmatrix1.m11 * inmatrix2.m10 + inmatrix1.m12 * inmatrix2.m20; // + inmatrix1.m13 * inmatrix2.m30;
        m11 = inmatrix1.m10 * inmatrix2.m01 + inmatrix1.m11 * inmatrix2.m11 + inmatrix1.m12 * inmatrix2.m21; // + inmatrix1.m13 * inmatrix2.m31;
        m12 = inmatrix1.m10 * inmatrix2.m02 + inmatrix1.m11 * inmatrix2.m12 + inmatrix1.m12 * inmatrix2.m22; // + inmatrix1.m13 * inmatrix2.m32;
        m13 = 0.0f; //inmatrix1.m10 * inmatrix2.m03 + inmatrix1.m11 * inmatrix2.m13 + inmatrix1.m12 * inmatrix2.m23 + inmatrix1.m13 * inmatrix2.m33;
        m20 = inmatrix1.m20 * inmatrix2.m00 + inmatrix1.m21 * inmatrix2.m10 + inmatrix1.m22 * inmatrix2.m20; // + inmatrix1.m23 * inmatrix2.m30;
        m21 = inmatrix1.m20 * inmatrix2.m01 + inmatrix1.m21 * inmatrix2.m11 + inmatrix1.m22 * inmatrix2.m21; // + inmatrix1.m23 * inmatrix2.m31;
        m22 = inmatrix1.m20 * inmatrix2.m02 + inmatrix1.m21 * inmatrix2.m12 + inmatrix1.m22 * inmatrix2.m22; // + inmatrix1.m23 * inmatrix2.m32;
        m23 = 0.0f; //inmatrix1.m20 * inmatrix2.m03 + inmatrix1.m21 * inmatrix2.m13 + inmatrix1.m22 * inmatrix2.m23 + inmatrix1.m23 * inmatrix2.m33;
        m30 = inmatrix1.m30 * inmatrix2.m00 + inmatrix1.m31 * inmatrix2.m10 + inmatrix1.m32 * inmatrix2.m20 + inmatrix2.m30;
        m31 = inmatrix1.m30 * inmatrix2.m01 + inmatrix1.m31 * inmatrix2.m11 + inmatrix1.m32 * inmatrix2.m21 + inmatrix2.m31;
        m32 = inmatrix1.m30 * inmatrix2.m02 + inmatrix1.m31 * inmatrix2.m12 + inmatrix1.m32 * inmatrix2.m22 + inmatrix2.m32;
        m33 = 1.0f; //inmatrix1.m30 * inmatrix2.m03 + inmatrix1.m31 * inmatrix2.m13 + inmatrix1.m32 * inmatrix2.m23 + inmatrix1.m33 * inmatrix2.m33;
    }
    
    /// operator multiply - NOT A FULL MULTIPLY.
    friend Matrix operator * (const Matrix&a, const Matrix &b){
        Matrix m;
        m.mul(a,b);
        return m;
    }
    
    /// FULL matrix multiply - makes this the product of the two inputs.
    void mulF(const Matrix &inmatrix1, const Matrix &inmatrix2)
    {
        assert(this != &inmatrix1 && this!=&inmatrix2);
        m00 = inmatrix1.m00 * inmatrix2.m00 + inmatrix1.m01 * inmatrix2.m10 + inmatrix1.m02 * inmatrix2.m20 + inmatrix1.m03 * inmatrix2.m30;
        m01 = inmatrix1.m00 * inmatrix2.m01 + inmatrix1.m01 * inmatrix2.m11 + inmatrix1.m02 * inmatrix2.m21 + inmatrix1.m03 * inmatrix2.m31;
        m02 = inmatrix1.m00 * inmatrix2.m02 + inmatrix1.m01 * inmatrix2.m12 + inmatrix1.m02 * inmatrix2.m22 + inmatrix1.m03 * inmatrix2.m32;
        m03 = inmatrix1.m00 * inmatrix2.m03 + inmatrix1.m01 * inmatrix2.m13 + inmatrix1.m02 * inmatrix2.m23 + inmatrix1.m03 * inmatrix2.m33;
        m10 = inmatrix1.m10 * inmatrix2.m00 + inmatrix1.m11 * inmatrix2.m10 + inmatrix1.m12 * inmatrix2.m20 + inmatrix1.m13 * inmatrix2.m30;
        m11 = inmatrix1.m10 * inmatrix2.m01 + inmatrix1.m11 * inmatrix2.m11 + inmatrix1.m12 * inmatrix2.m21 + inmatrix1.m13 * inmatrix2.m31;
        m12 = inmatrix1.m10 * inmatrix2.m02 + inmatrix1.m11 * inmatrix2.m12 + inmatrix1.m12 * inmatrix2.m22 + inmatrix1.m13 * inmatrix2.m32;
        m13 = inmatrix1.m10 * inmatrix2.m03 + inmatrix1.m11 * inmatrix2.m13 + inmatrix1.m12 * inmatrix2.m23 + inmatrix1.m13 * inmatrix2.m33;
        m20 = inmatrix1.m20 * inmatrix2.m00 + inmatrix1.m21 * inmatrix2.m10 + inmatrix1.m22 * inmatrix2.m20 + inmatrix1.m23 * inmatrix2.m30;
        m21 = inmatrix1.m20 * inmatrix2.m01 + inmatrix1.m21 * inmatrix2.m11 + inmatrix1.m22 * inmatrix2.m21 + inmatrix1.m23 * inmatrix2.m31;
        m22 = inmatrix1.m20 * inmatrix2.m02 + inmatrix1.m21 * inmatrix2.m12 + inmatrix1.m22 * inmatrix2.m22 + inmatrix1.m23 * inmatrix2.m32;
        m23 = inmatrix1.m20 * inmatrix2.m03 + inmatrix1.m21 * inmatrix2.m13 + inmatrix1.m22 * inmatrix2.m23 + inmatrix1.m23 * inmatrix2.m33;
        m30 = inmatrix1.m30 * inmatrix2.m00 + inmatrix1.m31 * inmatrix2.m10 + inmatrix1.m32 * inmatrix2.m20 + inmatrix1.m33 * inmatrix2.m30;
        m31 = inmatrix1.m30 * inmatrix2.m01 + inmatrix1.m31 * inmatrix2.m11 + inmatrix1.m32 * inmatrix2.m21 + inmatrix1.m33 * inmatrix2.m31;
        m32 = inmatrix1.m30 * inmatrix2.m02 + inmatrix1.m31 * inmatrix2.m12 + inmatrix1.m32 * inmatrix2.m22 + inmatrix1.m33 * inmatrix2.m32;
        m33 = inmatrix1.m30 * inmatrix2.m03 + inmatrix1.m31 * inmatrix2.m13 + inmatrix1.m32 * inmatrix2.m23 + inmatrix1.m33 * inmatrix2.m33;
    }
    
    /// Make the 3x3 rotational submatrix the product of the 3x3 rotational submatrices of the inputs. Do nothing to the remaining elements.
    void mulRot(const Matrix &inmatrix1, const Matrix &inmatrix2)
    {
        assert(this != &inmatrix1 && this!=&inmatrix2);
        m00 = inmatrix1.m00 * inmatrix2.m00 + inmatrix1.m01 * inmatrix2.m10 + inmatrix1.m02 * inmatrix2.m20;
        m01 = inmatrix1.m00 * inmatrix2.m01 + inmatrix1.m01 * inmatrix2.m11 + inmatrix1.m02 * inmatrix2.m21;
        m02 = inmatrix1.m00 * inmatrix2.m02 + inmatrix1.m01 * inmatrix2.m12 + inmatrix1.m02 * inmatrix2.m22;
        
        m10 = inmatrix1.m10 * inmatrix2.m00 + inmatrix1.m11 * inmatrix2.m10 + inmatrix1.m12 * inmatrix2.m20;
        m11 = inmatrix1.m10 * inmatrix2.m01 + inmatrix1.m11 * inmatrix2.m11 + inmatrix1.m12 * inmatrix2.m21;
        m12 = inmatrix1.m10 * inmatrix2.m02 + inmatrix1.m11 * inmatrix2.m12 + inmatrix1.m12 * inmatrix2.m22;
        
        m20 = inmatrix1.m20 * inmatrix2.m00 + inmatrix1.m21 * inmatrix2.m10 + inmatrix1.m22 * inmatrix2.m20;
        m21 = inmatrix1.m20 * inmatrix2.m01 + inmatrix1.m21 * inmatrix2.m11 + inmatrix1.m22 * inmatrix2.m21;
        m22 = inmatrix1.m20 * inmatrix2.m02 + inmatrix1.m21 * inmatrix2.m12 + inmatrix1.m22 * inmatrix2.m22;
        
        m03 = m13 = m23 = 0;
    }
    
    /// copy a rotational submatrix into this one
    void copyRot(const Matrix &a)
    {
        m00 = a.m00;
        m01 = a.m01;
        m02 = a.m02;
        m10 = a.m10;
        m11 = a.m11;
        m12 = a.m12;
        m20 = a.m20;
        m21 = a.m21;
        m22 = a.m22;
    }
    
    /// copy a rotational submatrix into an array of floats
    void copyRotToFloatArray(float *f){
        *f++ = m00;
        *f++ = m01;
        *f++ = m02;
        *f++ = m10;
        *f++ = m11;
        *f++ = m12;
        *f++ = m20;
        *f++ = m21;
        *f =   m22;
    }
    
    void copyRotToFloatArrayTrans(float *f){
        *f++ = m00;
        *f++ = m10;
        *f++ = m20;
        *f++ = m01;
        *f++ = m11;
        *f++ = m21;
        *f++ = m02;
        *f++ = m12;
        *f =   m22;
    }
    
    /// get a pointer to the translation vector
    Vector *getTranslationPtr() const
    {
        return (Vector *)&m30;
    }
    
    /// copy the translation into a vector
    void getTranslation(Vector *v) const
    {
        *v = *getTranslationPtr();
    }
    
    /// copy the translation from a vector. Does not modify rotation.
    void setTranslation(const Vector &v)
    {
        *getTranslationPtr() = v;
    }
    
    /// copy the translation from coordinates. Does not modify rotation.
    void setTranslation(float x,float y,float z)
    {
        m30 = x;
        m31 = y;
        m32 = z;
        m33 = 1;
    }
    
    /// treats the matrix as a 2D AABB, and sees if it contains another AABB. The float can be used to scale the two elements;
    /// for example, it's sqrt(2)/2 if we're using a Poly square
    
    bool AABB2DContainedIn(Matrix *b,float f=1.0f)
    {
        float aex = m00*f;
        float aey = m11*f;
        float bex = b->m00*f;
        float bey = b->m11*f;
        
        float ax = getTranslationPtr()->x;
        float ay = getTranslationPtr()->y;
        
        float bx = b->getTranslationPtr()->x;
        float by = b->getTranslationPtr()->y;
        
        return ((ax-aex > bx-bex) && (ax+aex < bx+bex) && 
                (ay-aey > by-bey) && (ay+aey < by+bey));
    }
    
    
    /// construct the 3x3 submatrix from the parameters. Clear column 4, Leave the translation untouched.
    
    void setRotation(float p00,float p01,float p02,float p10,float p11,float p12,float p20,float p21,float p22)
    {
        m00 = p00;
        m01 = p01;
        m02 = p02;
        m03 = 0;
        
        m10 = p10;
        m11 = p11;
        m12 = p12;
        m13 = 0;
        
        m20 = p20;
        m21 = p21;
        m22 = p22;
        m23 = 0;
        
        m33 = 1;
    }
    
    /// rotate n rads about each axis
    void makeRotate(float rx, float ry, float rz,RotateOrder order)
    {
        float a,b,c,d,e,f;
        
        using namespace FastMaths;
        
        sinAndCos( rx, &b, &a );
        sinAndCos( ry, &d, &c );
        sinAndCos( rz, &f, &e );
        
        switch(order)
        {
        case ROT_YXZ:
            {
                float bf = b*f, be = b*e;
                
                setRotation(
                            c*e - d*bf,  c*f + d*be,  -d*a,
                            -a*f,   a*e,   b,
                            d*e + c*bf,  d*f - c*be,  c*a);
                break;
            }
        case ROT_XYZ:
            {
                float de = d*e, df = d*f;
                setRotation(
                            c*e,   c*f,   -d,
                            b*de-a*f,  b*df+a*e,  b*c,
                            a*de+b*f,  a*df-b*e,  a*c);
                break;
            }
        case ROT_XZY:
            {
                float cf = c*f, df = d*f;
                setRotation(
                            c*e,  f,    -d*e,
                            -a*cf+b*d, a*e,   a*df+b*c,
                            b*cf+a*d, -b*e,   -b*df+a*c);
                break;
            }
        case ROT_YZX:
            {
                float cf = c*f, df = d*f;
                setRotation(
                            c*e,  a*cf+b*d,  b*cf-a*d,
                            -f,   a*e,   b*e,
                            d*e,  a*df-b*c,  b*df+a*c);
                break;
            }
        case ROT_ZXY:
            {
                float bd = b*d, bc = b*c;
                setRotation(
                            c*e+bd*f, a*f,   -d*e+bc*f,
                            -c*f+bd*e, a*e,   d*f+bc*e,
                            a*d,  -b,    a*c);
                break;
            }
        case ROT_ZYX:
            {
                float de = d*e, df = d*f;
                setRotation(
                            c*e,  a*f+b*de,  b*f-a*de,
                            -c*f,  a*e-b*df,  b*e+a*df,
                            d,   -b*c,   a*c);
                break;
            }
        }
    }
    
    
    /// makes a rotation around X of the given angle. Does not modify the translation.
    void setRotX(float x)
    {
        float sn,cs;
        FastMaths::sinAndCos(x,&sn,&cs);
        setRotation(1.0f, 0.0f, 0.0f,
                    0.0f, cs,  sn,
                    0.0f, -sn, cs);
    }
    
    /// makes a rotation around Y of the given angle. Does not modify the translation.
    void setRotY(float y)
    {
        float sn,cs;
        FastMaths::sinAndCos(y,&sn,&cs);
        setRotation(cs,  0.0f, -sn,
                    0.0f, 1.0f, 0.0f,
                    sn,  0.0f, cs);
    }
    
    /// makes a rotation around Z of the given angle. Does not modify the translation.
    void setRotZ(float z)
    {
        float sn,cs;
        FastMaths::sinAndCos(z,&sn,&cs);
        setRotation(cs,  sn,  0.0f,
                    -sn, cs,  0.0f,
                    0.0f, 0.0f, 1.0f);
    }
    
    /// makes a rotation around X of the given angle using the fast, less accurate trig routines. Does not modify the translation.
    void setRotXQuick(float x )
    {
        float sn,cs;
        FastMaths::sinAndCosQ(x,&sn,&cs);
        setRotation(1.0f, 0.0f, 0.0f,
                    0.0f, cs,  sn,
                    0.0f, -sn, cs);
    }
    
    /// makes a rotation around X of the given angle using the fast, less accurate trig routines. Also scales. Does not modify the translation.
    void setRotXQuickAndScale(float x, float sx, float sy, float sz )
    {
        float sn,cs;
        FastMaths::sinAndCosQ(x,&sn,&cs);
        setRotation(sx, 0.0f, 0.0f,
                    0.0f, sy*cs,  sy*sn,
                    0.0f, sz*-sn, sz*cs);
    }
    
    /// makes a rotation around Y of the given angle using the fast, less accurate trig routines. Does not modify the translation.
    void setRotYQuick(float y)
    {
        float sn,cs;
        FastMaths::sinAndCosQ(y,&sn,&cs);
        setRotation(cs,  0.0f, -sn,
                    0.0f, 1.0f, 0.0f,
                    sn,  0.0f, cs);
    }
    
    /// makes a rotation around Y of the given angle using the fast, less accurate trig routines. Also scales. Does not modify the translation.
    void setRotYQuickAndScale(float y, float sx, float sy, float sz )
    {
        float sn,cs;
        FastMaths::sinAndCosQ(y,&sn,&cs);
        setRotation(sx*cs,  0.0f, sx*-sn,
                    0.0f, sy, 0.0f,
                    sz*sn,  0.0f, sz*cs);
    }
    
    /// makes a rotation around Z of the given angle using the fast, less accurate trig routines. Does not modify the translation.
    void setRotZQuick(float z)
    {
        float sn,cs;
        FastMaths::sinAndCosQ(z,&sn,&cs);
        setRotation(cs,  sn,  0.0f,
                    -sn, cs,  0.0f,
                    0.0f, 0.0f, 1.0f);
    }
    
    /// makes a rotation around Z of the given angle using the fast, less accurate trig routines. Also scales. Does not modify the translation.
    void setRotZQuickAndScale(float z, float sx, float sy, float sz )
    {
        float sn,cs;
        FastMaths::sinAndCosQ(z,&sn,&cs);
        setRotation(sx*cs,  sx*sn,  0.0f,
                    sy*-sn, sy*cs,  0.0f,
                    0.0f, 0.0f, sz);
    }
    
    /// Le Kick of De ass Transform functione, owns all known angles. (but gimbals in the wabe)
    void setRotZXYAndScale( float xRot, float yRot, float zRot, float xScale, float yScale, float zScale )
    {
        float x = xScale, y = yScale, z = zScale;
        float sx, cx;
        FastMaths::sinAndCosQ(xRot,&sx,&cx);
        float sy, cy;
        FastMaths::sinAndCosQ(yRot,&sy,&cy);
        float sz, cz;
        FastMaths::sinAndCosQ(zRot,&sz,&cz);
        
        setRotation(
                    x*sx*sy*sz + x*cy*cz, x*cx*sz, x*sx*cy*sz - x*sy*cz, 
                    y*sx*sy*cz - y*cy*sz, y*cx*cz, y*sx*cy*cz + y*sy*sz,
                    z*cx*sy, -z*sx, z*cx*cy );
        
        // I AM KICKING ASS !!!!!!!!!!
    }
    
    /// make the 3x3 submatrix a scaling matrix, scaling each axis with a different scale. Do not modify the translation.
    void setScale(float fx,float fy,float fz)
    {
        m00 = fx;
        m01 = 0;
        m02 = 0;
        m03 = 0;
        
        m10 = 0;
        m11 = fy;
        m12 = 0;
        m13 = 0;
        
        m20 = 0;
        m21 = 0;
        m22 = fz;
        m23 = 0;
    }
    
    /// make the matrix the transpose of another matrix
    void transpose(const Matrix &m)
    {
        assert(&m!=this);
        
        m00=m.m00; m01=m.m10; m02=m.m20; m03=m.m30; 
        m10=m.m01; m11=m.m11; m12=m.m21; m13=m.m31; 
        m20=m.m02; m21=m.m12; m22=m.m22; m23=m.m32; 
        m30=m.m03; m31=m.m13; m32=m.m23; m33=m.m33; 
    }
    
    /// make the 3x3 submatrix the transpose of another 3x3 submatrix. Do not change anything else.
    void transposeRot(const Matrix &m)
    {
        assert(&m!=this);
        
        m00=m.m00; m01=m.m10; m02=m.m20;
        m10=m.m01; m11=m.m11; m12=m.m21;
        m20=m.m02; m21=m.m12; m22=m.m22;
    }
    
    
    /// make the 3x3 submatrix a rotation of "a" radians around the vector "u"
    void setRotateAbout(const Vector& u,float a)
    {
        float sina, cosa;
        
        //float cosa = COS(a);
        //float sina = SIN(a);
        float x,y,z;
        
        a= -a; // turn around from anticlock to clock..
        
        FastMaths::sinAndCos( a, &sina, &cosa );
        float cosa1 = 1-cosa;
        
        Vector qu;
        
        qu.normalize(u);
        x=qu.x; y=qu.y; z=qu.z;
        
        m00=x*x+cosa*(1-x*x);
        m01=x*y*cosa1-z*sina;
        m02=z*x*cosa1+y*sina;
        m03=0;
        
        m10=x*y*cosa1+z*sina;
        m11=y*y+cosa*(1-y*y);
        m12=y*z*cosa1-x*sina;
        m13=0;
        
        m20=z*x*cosa1-y*sina;
        m21=y*z*cosa1+x*sina;
        m22=z*z+cosa*(1-z*z);
        m23=0;
    }
    
    /// make the 3x3 submatrix a rotation of "a" radians around the vector "u" using fast less-accurate maths
    void setRotateAboutQuick(const Vector& u,float a)
    {
        float sina, cosa;
        
        //float cosa = COS(a);
        //float sina = SIN(a);
        float x,y,z;
        
        a= -a; // turn around from anticlock to clock..
        
        FastMaths::sinAndCosQ( a, &sina, &cosa );
        float cosa1 = 1-cosa;
        
        Vector qu;
        
        qu.normalize(u);
        x=qu.x; y=qu.y; z=qu.z;
        
        m00=x*x+cosa*(1-x*x);
        m01=x*y*cosa1-z*sina;
        m02=z*x*cosa1+y*sina;
        m03=0;
        
        m10=x*y*cosa1+z*sina;
        m11=y*y+cosa*(1-y*y);
        m12=y*z*cosa1-x*sina;
        m13=0;
        
        m20=z*x*cosa1-y*sina;
        m21=y*z*cosa1+x*sina;
        m22=z*z+cosa*(1-z*z);
        m23=0;
    }
    
    /// set the rotation matrix from three basis vectors
    void setBasis(const Vector &x,const Vector &y,const Vector &z)
    {
        m00 = x.x;
        m01 = x.y;
        m02 = x.z;
        m10 = y.x;
        m11 = y.y;
        m12 = y.z;
        m20 = z.x;
        m21 = z.y;
        m22 = z.z;
    }
    
    /// return the basis vectors
    void getBasis(Vector *x,Vector *y,Vector *z)
    {
        x->x = m00;
        x->y = m01;
        x->z = m02;
        y->x = m10;
        y->y = m11;
        y->z = m12;
        z->x = m20;
        z->y = m21;
        z->z = m22;
    }
    
    /// return the X axis
    Vector getBasisX()
    {
        Vector v;
        v.x = m00;
        v.y = m01;
        v.z = m02;
        return v;
    }
    /// return the Y axis
    Vector getBasisY()
    {
        Vector v;
        v.x = m10;
        v.y = m11;
        v.z = m12;
        return v;
    }
    /// return the Z axis
    Vector getBasisZ()
    {
        Vector v;
        v.x = m20;
        v.y = m21;
        v.z = m22;
        return v;
    }
    
    
    /// return the determinant of the matrix
    float det() const;
    
    /// make a  matrix inverse, assuming the input is an affine transform
    void invert(const Matrix &in);
    
    /// make a matrix inverse for any 4x4 matrix
    void invertFull(const Matrix &in);
    
    /// makes a rotational submatrix from a given direction and upvector. Leaves translation unaffected.
    void setRotFromDirAndUp(const Vector &dir, const Vector &up );
    
    /// makes a complete (including translation) matrix from position and target
    void setFromPosAndTarget(const Vector &pos, const Vector &target,const Vector &up=Vector::Y);
    
    
    
};




/*****************************************************************************
 * 
 * Inline methods
 * 
 ****************************************************************************/

/// inline definition of vector transform
void inline Vector::transform(const Vector &invec,const struct Matrix &inmatrix)
{
    x = invec.x * inmatrix.m00 + invec.y * inmatrix.m10 + invec.z * inmatrix.m20 + inmatrix.m30;
    y = invec.x * inmatrix.m01 + invec.y * inmatrix.m11 + invec.z * inmatrix.m21 + inmatrix.m31;
    z = invec.x * inmatrix.m02 + invec.y * inmatrix.m12 + invec.z * inmatrix.m22 + inmatrix.m32;
    w = 1.0f;
}


/// inline definition of vector transform
void inline Vector::transformRotationOnly(const Vector &invec,const struct Matrix &inmatrix)
{
    x = invec.x * inmatrix.m00 + invec.y * inmatrix.m10 + invec.z * inmatrix.m20;
    y = invec.x * inmatrix.m01 + invec.y * inmatrix.m11 + invec.z * inmatrix.m21;
    z = invec.x * inmatrix.m02 + invec.y * inmatrix.m12 + invec.z * inmatrix.m22;
    w = 1.0f;
}


/// inline definition of vector transform
void inline Vector::transformFull(const Vector &invec,const struct Matrix &inmatrix)
{
    x = invec.x * inmatrix.m00 + invec.y * inmatrix.m10 + invec.z * inmatrix.m20 + inmatrix.m30;
    y = invec.x * inmatrix.m01 + invec.y * inmatrix.m11 + invec.z * inmatrix.m21 + inmatrix.m31;
    z = invec.x * inmatrix.m02 + invec.y * inmatrix.m12 + invec.z * inmatrix.m22 + inmatrix.m32;
    w = invec.x * inmatrix.m03 + invec.y * inmatrix.m13 + invec.z * inmatrix.m23 + inmatrix.m33;
}

/// Matrix stack

class MatrixStack
{
    Matrix stack[32];
    int ct;
    
public:
    void reset(); // sets stack to hold identity
    Matrix *push(); // pushes stack, copies top and returns ptr
    void pop(); // pops stack and discards
    void mul(Matrix *m); // top = m*top
    void mulBack(Matrix *m); // top = top * m
    Matrix *top(); // get top
    void identity(); // loads into top slot
};




/// class for working with vertices of regular polygons

class RegPolyVertManagerClass
{
public:
    
    /// the regular poly types
    enum Type
    {
        TETRA=0,
              OCTA,
              CUBE,
              ICOS,
              DODEC,
              
              TYPES
          };
    
    /// initialise for use
    void init();
    
    /// get a pointer to a matrix for vertex n in type t.
    /// This matrix is a rotation whose Z axis points towards
    /// the given vertex from the centre.
    
    Matrix *getRotation(Type t,int v)
    {
        return mRotArray[t]+v;
    }
    
    /// as GetRotation() but uses an integer type
    Matrix *getRotation(int t,int v)
    {
        return getRotation((Type)t,v);
    }
    
    /// get the number of vertices for a type
    int getVCt(int t)
    {
        return getVCt((Type)t);
    }
    
    /// get the number of vertices for a type
    int getVCt(Type t)
    {
        return sVertCount[t];
    }
    
    
    
private:
    
    /// array of vertex counts
    static int sVertCount[];
    
    /// pointer to TYPES array of matrices
    Matrix *mRotArray[TYPES];
    
    /// used to generate the matrices
    void setRotation(Matrix *m,float x,float y,float z);    
};

/// single instance
extern RegPolyVertManagerClass gRegPolyVertManager;


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
                            Vector *out, 		//!< output
                            Vector *p0,			//!< control points
                            Vector *p1,
                            Vector *p2,
                            Vector *p3,
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
