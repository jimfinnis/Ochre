/**
 * @file logger.h
 * @brief  Brief description of file.
 *
 */

#ifndef __LOGGER_H
#define __LOGGER_H

#include <stdarg.h>
#include <stdio.h>


#define LOG_POP 1
#define LOG_LEVEL 2

class Logger {
    unsigned flags;
public:
    Logger(unsigned f){
        set(f);
    }
    
    void set(unsigned f){
        flags = f;
    }
    
    bool has(unsigned f){
        return (flags & f) != 0;
    }
    
    void p(unsigned f,const char *s,...){
        if(f & flags){
            char buf[256];
        
            va_list args;
            va_start(args,s);
        
            vsprintf(buf,s,args);
            printf("LOG: %s\n",buf);
        }
    }
};
    

#endif /* __LOGGER_H */
