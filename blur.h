/**
 * @file blur.h
 * @brief  Brief description of file.
 *
 */

#ifndef __BLUR_H
#define __BLUR_H

/// fast blur code nabbed from http://blog.ivank.net/fastest-gaussian-blur.html
/// takes 2D image source and target (as single-dimensional array ptrs). We use
/// this for potential field generation.

/// Just blur. scl=source, tcl = target, w,h=size, r=radius
void gaussBlur (float *scl, float *tcl, int w, int h, int r);


/// multipass blurrer.

class MultipassBlur {
    static const int NUMBOXES=3;
    int bxs[NUMBOXES];
    int w,h;
    int npass;
    float *tmp1,*tmp2;
public:    
    MultipassBlur(int w,int h,int r);
    ~MultipassBlur();
    
    /// perform a pass, given the source and target.
    /// The source is only read when n=0, the target
    /// is written only when the blur is done whereupon
    /// the function returns true.
    
    bool pass(int n,float *src,float *res);
    
    /// auto version
    bool pass(float *src,float *res){
        if(pass(npass,src,res)){
            npass=0;return true;
        } else {
            npass++;return false;
        }
    }
};


#endif /* __BLUR_H */
