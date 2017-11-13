/**
 * \file
 * Brief description. Longer description.
 *
 * 
 * \author $Author$
 * \date $Date$
 */

#include "texture.h"

#include <strings.h>
#include <dirent.h>
#include <unistd.h>

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>


TextureManager *TextureManager::inst=NULL;

TextureManager::TextureManager(){
    ilInit();
    iluInit();
    ilEnable(IL_CONV_PAL);
    ilutRenderer(ILUT_OPENGL);
    ilutEnable(ILUT_OPENGL_CONV);
    
}

static void handleDevilErrors(const char *fname){
    char wd[PATH_MAX];
    
    ILenum error = ilGetError ();
    if (error != IL_NO_ERROR) {
        getcwd(wd,PATH_MAX);
        printf("Devil errors, CWD: %s\n",wd);
        do {
            printf ("- %s\n", iluErrorString (error));	
        } while ((error = ilGetError ()));
        printf("cannot load image: %s\n",fname);
    }
}

Texture::Texture(std::string n) {
    valid=false;
    name=NULL;
    ERRCHK;
    const char *fname = n.c_str();
    
    if(strlen(fname)<3)
        throw Exception().set("silly texture file name '%s'",fname);
    
    ILuint img;
    ilGenImages(1,&img);
    ilBindImage(img);
    
    if(!ilLoadImage(fname)){
        handleDevilErrors(fname);
        return;
    }
    
    mWidth = ilGetInteger(IL_IMAGE_WIDTH);
    mHeight = ilGetInteger(IL_IMAGE_HEIGHT);
    
    id = ilutGLBindTexImage();
    handleDevilErrors(fname);
    ilDeleteImages(1,&img);
    
    glBindTexture(GL_TEXTURE_2D,id);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    name = strdup(fname);
    valid=true;
}

Texture::~Texture(){
    if(name)free((void *)name);
}

void Texture::use(int sampler, int unit){
    ERRCHK;
    glActiveTexture(GL_TEXTURE0+unit); // we are modifying texture unit 'unit'
    ERRCHK;
    glBindTexture(GL_TEXTURE_2D,id); // bind the given texture into the current unit
    ERRCHK;
    glUniform1i(sampler,unit); // tell the given sampler to use that unit
    ERRCHK;
    
    
}

Texture *TextureManager::createOrFind(std::string name){
    auto s = textures.find(name);
    if(s!=textures.end()){
        return s->second;
    } else {
        Texture *t = new Texture(name);
        if(t->valid){
            textures[name]=t;
            return t;
        } else {
            delete t;
            throw Exception().set("Bad or nonexistent texture '%s'",name.c_str());
        }
    }
}


void TextureManager::loadSet(const char *dirname,
                             std::vector<Texture *>&vec){
    DIR *dir = opendir(dirname);
    if(!dir)
        throw Exception().set("cannot open directory '%s'",dirname);
    
    vec.clear();
    while(dirent *ent = readdir(dir)){
        const char *name = ent->d_name;
        if(strlen(name)>3){
            const char *ext = name+(strlen(name)-3);
            if(!strcasecmp(ext,"png") || !strcasecmp(ext,"tga")){
                // throws on failure
                std::string qname(dirname);
                qname+="/"+std::string(name);
                Texture *t = createOrFind(qname);
                vec.push_back(t);
            }
        }
    }
    if(!vec.size())
        throw Exception().set("no textures found in '%s'",dirname);
}
