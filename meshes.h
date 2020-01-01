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

extern class ObjMesh *ico,*cursor,*house1,*marker;

// 2x2 cube with pivot at centre of top face
extern class ObjMesh *toppivotcube;

}


#endif /* __MESHES_H */
