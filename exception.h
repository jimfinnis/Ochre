/**
 * @file exception.h
 * @brief  Brief description of file.
 *
 */

#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include <exception>

class Exception : public std::exception {
public:
    /// default ctor
    Exception() : Exception(NULL){}
    /// more usual ctor
    Exception(const char *e){
        if(e)
            strncpy(error,e,1024);
        else
            strcpy(error,"???");
    }
    
    /// a variadic fluent modifier to set a better string with sprintf
    Exception& set(const char *s,...){
        va_list args;
        va_start(args,s);
        
        vsnprintf(error,1024,s,args);
        va_end(args);
        return *this;
    }
    
    
    /// return the error string
    virtual const char *what () const throw (){
        return error;
    }
    
    /// a copy of the error string
    char error[1024];
};

class FatalException : public Exception {
public:
    FatalException():Exception(){}
    FatalException(const char *e,const char *fn,int l) : Exception()
    {
        strcpy(brief,e?e:"unknown");
        if(fn)
            strncpy(fileName,fn,1024);
        else
            strcpy(fileName,"<unknown>");
        line = l;
        
        snprintf(error,1024,"%s(%d):  %s",fileName,line,brief);
    }
    int line;
    char fileName[1024];
    char brief[1024]; // error without leading filename and line
};

#define FATAL(x) throw FatalException(x,__FILE__,__LINE__)

// generic GL error check (use ERRCHK macro)
inline void GLerrorcheck(const char *file,int line){
    int code = glGetError();
    if(code != GL_NO_ERROR){
        printf("GL error at %s:%d - 0x%x",file,line,code);
        std::terminate();
    }
}

// generic GL error check (use ERRCHK macro)
#define ERRCHK GLerrorcheck(__FILE__,__LINE__)
void GLerrorcheck(const char *file,int line);


#endif /* __EXCEPTION_H */
