/**
 * @file videofile.cpp
 * @brief  Brief description of file.
 *
 */

#include "videofile.h"

// DOES NOTHING at the mo because there's no such thing as a
// lightweight video encoder library and life's too short.

#if(VIDEO)


struct VidData {
    int w,h;
    
    VidData(const char *fn,int ww,int hh){
        w=ww;h=hh;
    }
    ~VidData(){}
    
    void write(uint32_t *rgba){
    }
};


#endif

Video::Video(const char *fn,int w,int h){
#if VIDEO
    data = new VidData(fn,w,h);
#endif
}

Video::~Video(){
#if VIDEO
    delete (VidData *)data;
#endif
}


void Video::write(uint32_t *rgba){
#if VIDEO
    VidData *d = (VidData *)data;
    d->write(rgba);
#endif
}

                            
