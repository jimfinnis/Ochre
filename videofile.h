/**
 * @file videofile.h
 * @brief  Brief description of file.
 * 
 */

#ifndef __VIDEOFILE_H
#define __VIDEOFILE_H

#include <stdint.h>

// this is a wrapper around debugging code to write the debug
// map to video files.

// DOES NOTHING at the mo because there's no such thing as a
// lightweight video encoder library and life's too short.

class Video {
public:
    // open file
    Video(const char *fn,int w,int h);
    // close file
    ~Video();
    
    // write a frame: data is 32-bit rgba
    void write(uint32_t *rgba);
private:    
    void *data;
};


#endif /* __VIDEOFILE_H */
