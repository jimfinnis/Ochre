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

// scl=source, tcl = target, w,h=size, r=radius
void gaussBlur (float *scl, float *tcl, int w, int h, int r);


#endif /* __BLUR_H */
