/**
 * \file
 * Brief description. Longer description.
 * 
 * \author $Author$
 * \date $Date$
 */


#ifndef __VERTEXDATA_H
#define __VERTEXDATA_H

#include "maths.h"

/// vertex structure for PrelitEffect(s).
///

struct PRELITVERTEX
{
    float x,y,z;
    float u,v;
};

/// vertex for meshes

struct UNLITVERTEX
{
    float x,y,z;
    float nx,ny,nz;
    float u,v;
    
    static const float VERTEXEPSILON;
    static const float NORMALEPSILON;
    static const float UVEPSILON;
    
    inline bool compare(UNLITVERTEX *p){
        float d,t;
        d = (x-p->x);        t = d*d;
        d = (y-p->y);        t += d*d;
        d = (z-p->z);        t += d*d;
        if(t>VERTEXEPSILON)
            return false;
        d = (nx-p->nx);        t = d*d;
        d = (ny-p->ny);        t += d*d;
        d = (nz-p->nz);        t += d*d;
        if(t>NORMALEPSILON)
            return false;
        d = (u-p->u);        t = d*d;
        d = (v-p->v);        t += d*d;
        if(t>UVEPSILON)
            return false;
        return true;
    }
    
};


#endif /* __VERTEXDATA_H */
