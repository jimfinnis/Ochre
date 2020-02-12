/**
 * @file meshes.cpp
 * @brief  Brief description of file.
 *
 */

#include "obj.h"

namespace meshes {

ObjMesh *house1,*house2,*house3,*house4;
ObjMesh *toppivotcube,*marker,*ico,*cursor;

void load(){
    ico = new ObjMesh("media/meshes/ico","ico.obj");
    cursor = new ObjMesh("media/meshes/cursor","cursor.obj");
    house1 = new ObjMesh("media/meshes/house1","house1.obj");
    house2 = new ObjMesh("media/meshes/house2","house2.obj");
    house3 = new ObjMesh("media/meshes/house3","house3.obj");
    house4 = new ObjMesh("media/meshes/house4","house4.obj");
    marker = new ObjMesh("media/meshes/marker","marker.obj");
    toppivotcube = new ObjMesh("media/meshes/toppivotcube","toppivotcube.obj");
}


}
