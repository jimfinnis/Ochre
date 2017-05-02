#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
typedef uint32_t RGBA32;			//!< colour as a 32-bit integer
typedef uint32_t ABGR32;			//!< colour as a 32-bit integer PS2 texture format
typedef uint32_t ARGB32;			//!< colour as a 32-bit int with the alpha at the high end

#define MKRGB(r,g,b)		(0xff000000|((r)<<16)|((g)<<8)|(b))
#define MKRGBA(r,g,b,a)		(((a)<<24)|((r)<<16)|((g)<<8)|(b))

/*****************************************************************************
 * 
 * callback types
 * 
 ****************************************************************************/


/// simple void(void) callback
typedef void (*FUNCVOIDRETSVOID)(void);

/// simple void(int) callback
typedef void (*FUNCINTRETSVOID)(int);

/// simple int(void) callback
typedef int (*FUNCVOIDRETSINT)(void);

/// simple int(int) callback
typedef int (*FUNCINTRETSINT)(int);

/// simple bool(void) callback
typedef bool (*FUNCVOIDRETSBOOL)(void);

/// simple bool(char *) callback
typedef bool (*FUNCSTRINGRETSBOOL)(char *);

/// simple bool(int) callback
typedef bool (*FUNCINTRETSBOOL)(int);

/// simple bool(uchar) callback
typedef bool (*FUNCBYTERETSBOOL)(unsigned char);



/// colour structure, all values 0-1

struct FLOATRGBA
{
    float r;	//!< red
    float g;	//!< green
    float b;	//!< blue
    float a;	//!< alpha
    
    friend bool operator == (FLOATRGBA &a, FLOATRGBA &b)
    {
        return a.r == b.r && a.g==b.g && a.b == b.b && a.a == b.a;
    }
    
    friend bool operator != (FLOATRGBA &a, FLOATRGBA &b)
    {
        return a.r != b.r || a.g!=b.g || a.b != b.b || a.a != b.a;
    }
    
    /// brightness (crude, used for estimating black)
    float brightness()
    {
        return r+g+b;
    }
    
    /// set
    
    void set(float _r,float _g,float _b,float _a=1)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }
    
    /// set from HSV values
    void setFromHSV(float h,float s,float v,float _a=1);
    
    /// set from H11 values (same as hsv, assumes s=v=1)
    void setFromH11(float h);
    
    /// interpolate between two colours
    void interpolate(FLOATRGBA &a, FLOATRGBA &b,float t);
    
    /// find the complementary colour
    void setToComplement(FLOATRGBA &in);
    
    
    /// returns a pointer to the first float
    
    float *getFloatPtr()
    {
        return &r;
    }
    
    /// convert to an rgba32
    RGBA32 getRGBA32()
    {
        int i;
        RGBA32 c;
        
        i = (int)(r*255.0f);
        c = i<<24;
        
        i = (int)(g*255.0f);
        c |= i<<16;
        
        i = (int)(b*255.0f);
        c |= i<<8;
        
        i = (int)(a*255.0f);
        c |= i;
        
        return c;
    }
    
    /// convert from an rgba32
    
    void setFromRGBA32(RGBA32 c)
    {
        int i;
        
        i = c>>24;
        r = ((float )i) * (1.0f/255.0f);
        
        i = (c>>16)&0xff;
        g = ((float )i) * (1.0f/255.0f);
        
        i = (c>>8)&0xff;
        b = ((float )i) * (1.0f/255.0f);
        
        i = c&0xff;
        a = ((float )i) * (1.0f/255.0f);
    }
    
    /// convert from discrete RGB
    void setFromRgb(uint8_t ri,uint8_t gi,uint8_t bi)
    {
        r = ((float )ri) * (1.0f/255.0f);
        g = ((float )gi) * (1.0f/255.0f);
        b = ((float )bi) * (1.0f/255.0f);
        a = 1.0f;
    }
    
    /// convert from discrete RGB
    void setFromRgba(uint8_t ri,uint8_t gi,uint8_t bi,uint8_t ai)
    {
        r = ((float )ri) * (1.0f/255.0f);
        g = ((float )gi) * (1.0f/255.0f);
        b = ((float )bi) * (1.0f/255.0f);
        a = ((float )ai) * (1.0f/255.0f);
    }
    
    
    /// convert to an abgr32
    ABGR32 getABGR32()
    {
        int i;
        RGBA32 c;
        
        i = (int)(a*255.0f);
        c = i<<24;
        
        i = (int)(b*255.0f);
        c |= i<<16;
        
        i = (int)(g*255.0f);
        c |= i<<8;
        
        i = (int)(r*255.0f);
        c |= i;
        
        return c;
    }
    
    /// convert to an argb32
    ARGB32 getARGB32()
    {
        int i;
        ARGB32 c;
        
        i = (int)(a*255.0f);
        c = i<<24;
        
        i = (int)(r*255.0f);
        c |= i<<16;
        
        i = (int)(g*255.0f);
        c |= i<<8;
        
        i = (int)(b*255.0f);
        c |= i;
        
        return c;
    }
    
    /// convert from an abgr32
    
    void setFromABGR32(ABGR32 c)
    {
        int i;
        
        i = c>>24;
        a = ((float )i) * (1.0f/255.0f);
        
        i = (c>>16)&0xff;
        b = ((float )i) * (1.0f/255.0f);
        
        i = (c>>8)&0xff;
        g = ((float )i) * (1.0f/255.0f);
        
        i = c&0xff;
        r = ((float )i) * (1.0f/255.0f);
    }
    
    /// convert from an ARGB32
    
    void setFromARGB32(ARGB32 c)
    {
        int i;
        
        i = c>>24;
        a = ((float )i) * (1.0f/255.0f);
        
        i = (c>>16)&0xff;
        r = ((float )i) * (1.0f/255.0f);
        
        i = (c>>8)&0xff;
        g = ((float )i) * (1.0f/255.0f);
        
        i = c&0xff;
        b = ((float )i) * (1.0f/255.0f);
    }
    
};

const static FLOATRGBA gFLOATRGBA_White = {1.0f, 1.0f, 1.0f, 1.0f};

inline ARGB32 makeARGB32(float a,ARGB32 rgb)
{
    a *= 255;
    int q = (int)a;
    rgb = (rgb & 0xffffff) | (q<<24);
    return rgb;
}

inline ARGB32 getInterpARGB32(ARGB32 a,ARGB32 b,float t)
{
    FLOATRGBA p,q,r;
    p.setFromARGB32(a);
    q.setFromARGB32(b);
    r.interpolate(p,q,t);
    return r.getARGB32();
}


#endif /* __TYPES_h */
