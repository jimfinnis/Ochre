/**
 * \file
 * Brief description. Longer description.
 * 
 * \author $Author$
 * \date $Date$
 */


#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <vector>
#include <string>
#include <map>
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h> 
#include <GL/glext.h> 
#include "exception.h"

/// debugging macro for openGL error checks
#define ERRCHK GLerrorcheck(__FILE__,__LINE__)
void GLerrorcheck(const char *file,int line);



/// This class encapsulates a texture loaded from an image file on disk, and managed by the TextureManager (do not
/// create Texture objects directly).

class Texture {
    /// only this can create or delete us
    friend class TextureManager;
    
protected:
    /// use TextureManager::CreateOrFind().
    Texture(std::string filename);
    
    /// called by texture manager but does nothing right now
    virtual ~Texture();
    
    // pointer to next texture in manager.
    Texture *m_pNext;
    
public:    
    
    /// set up GL to start using this texture on a given unit in a given sampler
    void use(int sampler,int unit);
    /// or just get the ID
    GLuint getID(){
        return id;
    }
    unsigned int mHeight,mWidth;
    const char *name;
    bool valid;
private:
    GLuint id;
    bool mHasAlpha;
};

/// Singleton of this class loads and destroys textures
class TextureManager{
    static class TextureManager *inst;
public:
    // initialises loaders and that
    TextureManager();
    static TextureManager *getInstance(){
        if(!inst)
            inst = new TextureManager();
        return inst;
    }
              
    /// find a texture in the manager or create and init a new one
    Texture *createOrFind(std::string filename);
    /// load an vector of textures from
    /// a directory. Assumes .tga or .png. Yes, I know
    /// I'm horribly mixing C and C++.
    void loadSet(const char *dir,std::vector<Texture *>& vec);
private:
    std::map<std::string,Texture *> textures;
    
};
    



#endif /* __TEXTURE_H */
