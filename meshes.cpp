/**
 * @file meshes.cpp
 * @brief  Brief description of file.
 *
 */

#include "obj.h"

namespace mesh {

ObjMesh *ico,*cursor;

void load(){
    ico = new ObjMesh("media/meshes/ico","ico.obj");
    cursor = new ObjMesh("media/meshes/cursor","cursor.obj");
}


}
