/**
 * @file meshes.cpp
 * @brief  Brief description of file.
 *
 */

#include "obj.h"

namespace meshes {

ObjMesh *ico,*cursor,*house1,*marker;

void load(){
    ico = new ObjMesh("media/meshes/ico","ico.obj");
    cursor = new ObjMesh("media/meshes/cursor","cursor.obj");
    house1 = new ObjMesh("media/meshes/house1","house1.obj");
    marker = new ObjMesh("media/meshes/marker","marker.obj");
}


}
