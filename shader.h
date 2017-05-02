/**
 * @file shader.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SHADER_H
#define __SHADER_H

#include "gfx.h"
#include "exception.h"

/// load a shader of the appropriate type
GLuint loadShaderFromText(GLenum type,const char *src);


#endif /* __SHADER_H */
