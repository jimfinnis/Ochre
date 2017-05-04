/**
 * @file meshes.cpp
 * @brief  Brief description of file.
 *
 */

#include "obj.h"

namespace mesh {

ObjMesh *ico;

void load(){
    ico = new ObjMesh("media/meshes/ico","ico.obj");
}


}
