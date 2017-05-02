/**
 * @file obj.cpp
 * @brief  Brief description of file.
 *
 */

#include <malloc.h>
#include <unistd.h>
#include <limits.h>

#include "obj.h"
#include "state.h"
#include "effect.h"
#include "vertexdata.h"
#include "exception.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

const float UNLITVERTEX::VERTEXEPSILON=0.001f;
const float UNLITVERTEX::NORMALEPSILON=0.00f;
const float UNLITVERTEX::UVEPSILON=0.01f;

struct Material {
    struct Texture *t; // will probably always be null
    float diffuse[4];
};

static const int VERTEXBUFFER=0;
static const int INDEXBUFFER=1;

static int findOrCreateVert(std::vector<UNLITVERTEX>& verts,UNLITVERTEX &v){
//    for(size_t i=0;i<verts.size();i++){
//        if(verts[i].compare(&v))
//            return i;
//    }
    verts.push_back(v);
    return verts.size()-1;
}

ObjMesh::ObjMesh(const char *dir,const char *name){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    printf("Loading OBJ %s/%s\n",dir,name);
    
    char wd[PATH_MAX];
    getcwd(wd,PATH_MAX);
    if(chdir(dir))
        throw Exception().set("cannot change to directory '%s'",dir);
    
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib,&shapes,&materials,
                                &err,name);
    
    if(!ret)
        throw Exception().set("loader failure, %s",err.c_str());
    
    // first, run through the materials cutting them down into
    // our sort of material
    
    mats = new Material[materials.size()];
    for(size_t i=0;i<materials.size();i++){
        tinyobj::material_t& tm = materials[i]; 
        Material *m = mats+i;
        std::string texname;
        // work out what the texture name is.
        if(!tm.diffuse_texname.empty())
            texname = tm.diffuse_texname;
        else {
            if(!tm.ambient_texname.empty())
                texname = tm.ambient_texname;
            else if(!tm.emissive_texname.empty())
                texname = tm.emissive_texname;
        }
        printf("Mat %ld : tex %s\n",i,texname.c_str());
        // load the texture if any
        if(!texname.empty()){
/*            m->t = TextureManager::getInstance()->createOrFind(texname.c_str());
            if(!m->t){
                printf("cannot load %s\n",texname.c_str());
   }
 */
            m->t = NULL;
        }else
              m->t = NULL;
        
        // now set the diffuse (the only attrib we support)
        m->diffuse[0] = tm.diffuse[0];
        m->diffuse[1] = tm.diffuse[1];
        m->diffuse[2] = tm.diffuse[2];
        m->diffuse[3] = 1;
        printf("  Diffuse: %f %f %f\n",m->diffuse[0],m->diffuse[1],m->diffuse[2]);
    }
    
    // combined vertices (pos+norm+uv)
    std::vector<UNLITVERTEX> verts;
    // indices into the above
    std::vector<int> indx;
    // and materials (which will have indices / 3, since triangles.
    std::vector<int> matidx;
    
    // find centroid
    float cx=0,cy=0,cz=0;
    int ct=0;
    for(size_t s=0;s<shapes.size();s++){
        // for each face..
        int indexoffset=0;
        printf("Processing shape of %d polys\n",(int)
               shapes[s].mesh.num_face_vertices.size());
               
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            if(shapes[s].mesh.num_face_vertices[f]!=3)
                throw Exception("ooh, non-triangular face!");
            for(int i=0;i<3;i++){
                // build a vertex and add it, or get the index of the old
                // one if it was used before. This combines all the elements
                // into one.
                tinyobj::index_t idx = shapes[s].mesh.indices[indexoffset+i];
                cx += attrib.vertices[3*idx.vertex_index+0];
                cy += attrib.vertices[3*idx.vertex_index+1];
                cz += attrib.vertices[3*idx.vertex_index+2];
                ct++;
            }
            indexoffset+=3;
        }
    }
    
    cx/=(float)ct;
    cy/=(float)ct;
    cz/=(float)ct;
    
    
    // next step - build out of this mess a unified set of vertices
    // and indices into them (as triples), and a material index list.
    
//    for(size_t s=0;s<10;s++){
    for(size_t s=0;s<shapes.size();s++){
        // for each face..
        int indexoffset=0;
        printf("Processing shape of %d polys\n",(int)
               shapes[s].mesh.num_face_vertices.size());
        
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            UNLITVERTEX v;
            for(int i=0;i<3;i++){
                // build a vertex and add it, or get the index of the old
                // one if it was used before. This combines all the elements
                // into one.
                tinyobj::index_t idx = shapes[s].mesh.indices[indexoffset+i];
                v.x = attrib.vertices[3*idx.vertex_index+0]-cx;
                v.y = attrib.vertices[3*idx.vertex_index+1]-cy;
                v.z = attrib.vertices[3*idx.vertex_index+2]-cz;
                if(idx.normal_index<0){
                    throw Exception("Shape has no normals, re-export with normals.");
                }
                v.nx = attrib.normals[3*idx.normal_index+0];
                v.ny = attrib.normals[3*idx.normal_index+1];
                v.nz = attrib.normals[3*idx.normal_index+2];
                if(idx.texcoord_index>=0){
                    v.u = attrib.texcoords[2*idx.texcoord_index+0];
                    v.v = attrib.texcoords[2*idx.texcoord_index+1];
                } else {
                    v.u = v.v = 0;
                }
                int vertidx = findOrCreateVert(verts,v);
                // now add the index of the combined vertex
                indx.push_back(vertidx);
//                printf("%f,%f,%f %f,%f,%f %f,%f\n",
//                       v.x,v.y,v.z,v.nx,v.ny,v.nz);

            }
            indexoffset+=3;
            matidx.push_back(shapes[s].mesh.material_ids[f]);
        }
    }
    
    // blimey, that took ages. Now we need to create a material
    // transition list.
    
    int curmat=-1000;
    int transct=0;
    for(size_t i=0;i<matidx.size();i++)
    {
        if(matidx[i]!=curmat)
        {
            Transition t;
            curmat=matidx[i];
            t.matidx=curmat;
            t.start=i*3;
            if(transct)
                transitions[transct-1].count=i*3-
                  transitions[transct-1].start;
            transct++;
            transitions.push_back(t);
        }
    }
    transitions[transct-1].count=matidx.size()*3-
          transitions[transct-1].start;
    
    // we now have a list of material transitions we can use
    // in the above list. What remains is to make things more
    // permanent: create vbo and ib from the verts and idxs.
    
    // create index and vertex buffers
    glGenBuffers(2,buffers);
    ERRCHK;
    
    // bind the array and element array buffer to our buffers
    glBindBuffer(GL_ARRAY_BUFFER,buffers[VERTEXBUFFER]);
    ERRCHK;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[INDEXBUFFER]);
    ERRCHK;
    
    // create the vertex and index buffer and fill them with data
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint)*indx.size(),
                 &indx[0],
                 GL_STATIC_DRAW);
    ERRCHK;
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(UNLITVERTEX)*verts.size(),
                 &verts[0],
                 GL_STATIC_DRAW);
    ERRCHK;
    
    
    chdir(wd);
}

ObjMesh::~ObjMesh(){
    glDeleteBuffers(2,buffers);
    delete [] mats;
}

static Material defaultMat = {
    NULL,{0,0,0,1}};


void ObjMesh::render(Matrix *world){
    State *s = StateManager::getInstance()->get();
    
    // start the effect
    Effect *eff;
    
    // use the state's effect if there is one.
    if(s->effect)
        eff = s->effect;
    else
        // otherwise use the standard.
        eff = EffectManager::getInstance()->untex;
    eff->begin();
    // upload the matrices
    eff->setUniforms();
    eff->setWorldMatrix(world);
    
    // bind the arrays
    glBindBuffer(GL_ARRAY_BUFFER,buffers[VERTEXBUFFER]);
    ERRCHK;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffers[INDEXBUFFER]);
    ERRCHK;
    
    // tell them about offsets
    eff->setArrayOffsetsUnlit();
       
    // and iterate over the transitions
    
    for(std::vector<Transition>::iterator
        it=transitions.begin();it!=transitions.end();it++){
        Material *m = it->matidx < 0 ? &defaultMat : mats+it->matidx;
        
        float *col = (s->overrides & STO_DIFFUSE)?
              ((float *)&s->diffuse):m->diffuse;
        if(s->overrides & STO_ALPHA)
            col[3] = s->alpha;
        else
            col[3] = 1.0;
        eff->setMaterial(col,NULL);
        glDrawElements(GL_TRIANGLES,it->count,GL_UNSIGNED_INT,
                       (void *)(it->start*sizeof(GLuint)));
        ERRCHK;
        
    }
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}


