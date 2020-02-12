/**
 * @file meshes.h
 * @brief  Brief description of file.
 *
 */

#ifndef __MESHES_H
#define __MESHES_H

// just a big namespace of meshes and a loader. Keep it simple, eh?

namespace meshes {

void load();

extern class ObjMesh *ico,*cursor,*marker;
extern class ObjMesh *house1,*house2,*house3,*house4;

// 2x2 cube with pivot at centre of top face
extern class ObjMesh *toppivotcube;

}


#endif /* __MESHES_H */
