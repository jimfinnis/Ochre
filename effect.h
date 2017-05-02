/**
 * \file
 * Brief description. Longer description.
 * 
 * \author $Author$
 * \date $Date$
 */


#ifndef __EFFECT_H
#define __EFFECT_H

#include "vertexdata.h"

class Effect {
    friend class EffectManager;
public:
    bool begin();
    
    void end();
    
    /// set general uniforms ready for running
    void setUniforms();
    /// set the transformation uniforms
    void setWorldMatrix(glm::mat4 *world);
    /// set material uniforms
    void setMaterial(float *diffuse,class Texture *texture);
    /// set envmap colour and alpha
    void setMapCol(float *col);
    
    /// set up the GL array pointers for prelit data
    /// NOT USED - might never be, and would be changed
    /// to offsets anyway.
    void setArrayPointersPrelit(PRELITVERTEX *v);
    
    /// set up the array offsets for unlit vertices
    void setArrayOffsetsUnlit();
    
protected:
    /// 1st stage of initialisation
    Effect(const char *name,unsigned int f){
        dataflags=f;
        mpVShader = NULL;
        mpFShader = NULL;
        mpGShader = NULL;
        vshader=0;
        fshader=0;
        gshader=0;
        program=0xffffffff;
        mName = name;
        additive=false;
    }
    
    bool additive;
    
    /// 2nd stage - separated to avoid calling overridable methods from constructor
    Effect *init(){
        if(mName)
            initFromFile();
        return this;
    }
        
    virtual ~Effect();
    
    const char *getFileName(){
        return mName;
    }
    
    void initFromFile(); // name must be set up by ctor
    void initFromData(const char *vertSrc,const char *fragSrc);
    
    const char *mName;
    
    /// effect currently in use; checked in Begin()
    static Effect *mCurEffect;
    
    /// pointer to vertex shader text
    const char *mpVShader;
    
    /// pointer to geometry shader text
    const char *mpGShader;
    
    /// pointer to fragment shader text
    const char *mpFShader;
    
    GLuint vshader,gshader,fshader,program;
    
    /// used to get shader indices
    int getAttribute(const char *name);
    /// used to get shader indices
    int getUniform(const char *name);
    
    /// actually does the compile
    void compile();
    
    /// get the attribute indices ready for loading, done at
    /// compile
    void getAttributes();
    
    
    // these flags indicate which uniforms and attributes 
    // and present and need to be loaded
    
    
    unsigned int dataflags;
    
    bool inline has(int f){
        return (dataflags & f)!=0;
    }
    
    // these are the flags, and the attrs/uniforms for each
    // flag

#define EDA_POS 1
    int mPosIdx;
#define EDA_TEXCOORDS 2
    int mTexCoordIdx;
#define EDA_NORM 4
    
    
    
    int mNormIdx;
#define EDU_WORLDVIEWPROJ 8
    int mWorldViewProjIdx;
#define EDU_WORLDVIEW 16
    int mWorldViewIdx;
#define EDU_SAMPLER 32
    int mSamplerIdx;
#define EDU_DIFFUSECOL 64
    int mDiffuseIdx;
#define EDU_DIFFLIGHTS 128
    int LightCol1Idx,LightCol2Idx,LightCol3Idx;
    int LightDir1Idx,LightDir2Idx,LightDir3Idx;
#define EDU_AMBLIGHT 256
    int AmbientColIdx;
#define EDU_FOG 512
    int FogColIdx;
    int FogDistIdx;
#define EDU_NORMMAT 1024
    int mNormalMatIdx;
#define EDU_DIFFUSE2 2048
    int mDiffuse2Idx;
#define EDU_SAMPLER2 4096
    int mSampler2Idx;
};



class EffectManager {
    EffectManager();
    
    static EffectManager *instance;
    
public:
    static glm::mat4 projection;
    
    static EffectManager *getInstance(){
        if(instance==NULL)
            instance = new EffectManager();
        return instance;
    }
    
    Effect *untex; // untextured mesh, to be lit
};


#endif /* __EFFECT_H */
