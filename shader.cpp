/**
 * @file shader.cpp
 * @brief  Brief description of file.
 *
 */

#include "shader.h"

GLuint loadShaderFromText(GLenum type,const char *src)
{
//        printf("READY TO COMPILE\n%s",src);
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    ERRCHK;
    if(!shader)
        return 0;
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
    if(!compiled){
        GLint infolen;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infolen);
        char *log = (char*)"??";
        if(infolen>1){
            log = (char *)malloc(infolen);
            glGetShaderInfoLog(shader,infolen,NULL,log);
            printf("error compiling shader:\n%s\n",log);
            FATAL("error compiling shader");
        }
        glDeleteShader(shader);
        std::terminate();
    }
    return shader;
}
