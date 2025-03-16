// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  			Copyright 2010 Broadsword Games.
//			      All Rights Reserved
//
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Jim Finnis
//  Created       : Mon May 10 15:57:47 2010
//  Last Modified : <250105.2148>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2010 Broadsword Games.
// 
//  All Rights Reserved.
// 
// This  document  may  not, in  whole  or in  part, be  copied,  photocopied,
// reproduced,  translated,  or  reduced to any  electronic  medium or machine
// readable form without prior written consent from Broadsword Games.
//
//////////////////////////////////////////////////////////////////////////////

#include "effect.h"
#include "state.h"
#include <glm/gtc/matrix_inverse.hpp>

#include <stdio.h>

StateManager *StateManager::instance = NULL;
EffectManager *EffectManager::instance = NULL;


glm::mat4 EffectManager::projection;

/**
 * Effect manager initialisation - shader loading
 */

EffectManager::EffectManager(){
    untex = new Effect("media/untex.shr",
                       EDA_POS|EDA_NORM|
                       EDU_WORLDVIEWPROJ|EDU_NORMMAT|EDU_DIFFUSECOL|
                       EDU_DIFFLIGHTS|EDU_AMBLIGHT|EDU_FOG);
    untex->init();
    
    flattex = new Effect("media/flattex.shr",
                         EDA_POS|EDA_TEXCOORDS|
                         EDU_SAMPLER|EDU_WORLDVIEWPROJ|EDU_DIFFUSECOL);
    flattex->init();
    
    flatuntex = new Effect("media/flatuntex.shr",
                         EDA_POS|EDU_WORLDVIEWPROJ|EDU_DIFFUSECOL);
    flatuntex->init();
}


/*****************************************************************************
 * 
 * 
 * 
 * Effects (i.e. shaders and programs)
 * 
 * 
 * 
 ****************************************************************************/

static int loadShader(GLenum type,const char *src)
{
//        printf("READY TO COMPILE\n%s",src);
    GLuint shader;
    GLint compiled;
    shader = glCreateShader(type);
    ERRCHK;
    if(!shader)
        return 0;
    printf("---------------------\n%s\n------------------\n",src);
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);
    glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
    if(!compiled){
        GLint infolen;
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infolen);
        char *log = (char*)"??";
        if(infolen>1){
            log = (char *)malloc(infolen);
            glGetShaderInfoLog(shader,infolen,NULL,log);
            printf("error compiling shader:\n%s\n",log);
        }
        glDeleteShader(shader);
        throw Exception().set("cannot compile shader: %s",log);
    }
    printf("Shader compiled OK\n");
    return shader;
}

void Effect::compile(){
    printf("Compiling shader %s\n",mName);
    vshader = loadShader(GL_VERTEX_SHADER,mpVShader);
    ERRCHK;
    fshader = loadShader(GL_FRAGMENT_SHADER,mpFShader);
    ERRCHK;
    if(mpGShader){
        printf("%s\n",mpGShader);
        gshader = loadShader(GL_GEOMETRY_SHADER,mpGShader);
        ERRCHK;
    }
    
    program = glCreateProgram();
    if(!program)
        throw Exception().set("cannot create program for shader '%s'",mName);
    glAttachShader(program,vshader);
    ERRCHK;
    if(mpGShader){
        glAttachShader(program,gshader);
        ERRCHK;
    }
    glAttachShader(program,fshader);
    ERRCHK;
    
    
    glLinkProgram(program);
    ERRCHK;
    printf("Get attr for %s\n",mName);
    getAttributes(); // written for each effect
    printf("Get attr done\n");
    GLint linked;
    glGetProgramiv(program,GL_LINK_STATUS,&linked);
    ERRCHK;
    if(!linked){
        GLint infolen;
        glGetProgramiv(program,GL_INFO_LOG_LENGTH,&infolen);
        char *log = (char *)"??";
        if(infolen>1){
            log = (char *)malloc(infolen);
            glGetProgramInfoLog(program,infolen,NULL,log);
            printf("error linking shaders:\n%s\n",log);
        }
        glDeleteProgram(program);
        throw Exception().set("cannot link program %s: %s",mName,log);
    }
    printf("Shader linked OK\n");
    
}

class growbuf
{
    static const int startsize = 256;
    static const int growsize = 256;
    
public:
    
    growbuf()
    {
        buf = (char *)malloc(startsize);
        strlen = 0;
        buflen = startsize;
    }
    
    void add(char c)
    {
        if(strlen==buflen)
        {
            int newbuflen = buflen+growsize;
            char *newbuf = (char *)malloc(newbuflen);
            memcpy(newbuf,buf,buflen);
            free(buf);
            
            buflen = newbuflen;
            buf = newbuf;
        }
        buf[strlen++]=c;
    }
    
    char *endandreturn()
    {
        add(0);
        return buf;
    }
    
    void include(const char *fname)
    {
        FILE *a;
        if(!(a=fopen(fname,"r")))
           throw Exception().set("cannot open file %s",fname);
        fseek(a,0,SEEK_END);
        long size = ftell(a);
        fseek(a,0,SEEK_SET);
        
        char *buf = (char *)malloc(size);
        fread(buf,size,1,a);
        fclose(a);
        
        char *q = buf;
        for(uint32_t i=0;i<size;i++)
            add(*q++);
        free(buf);
    }
    
private:
    char *buf;
    int strlen,buflen;
};

static char *handleInclusion(char *in)
{
    growbuf buf;
    
    for(char *q = in;*q;q++)
    {
        if(*q == '#' && !strncmp(q+1,"include",7))
        {
            q+=8;
            while(*q != '<')q++;
            q++;
            char *fnend = q;
            while(*fnend != '>')fnend++;
            char fname[128];
            memcpy(fname,q,fnend-q);
            fname[fnend-q]=0;
            q=fnend+1;
            buf.include(fname);
        }
        else
            buf.add(*q);
    }
    return buf.endandreturn();
}

void Effect::initFromFile(){
    // there are two shaders, but we're going to put them into a single file
    // here. They are separated by a "##" line. We read the data into a single buffer,
    // then walk through it setting up pointers and adding nulls.    
    
    FILE *a;
    if(!(a=fopen(mName,"r")))
        throw Exception().set("cannot open file %s",mName);
    fseek(a,0,SEEK_END);
    long size = ftell(a);
    fseek(a,0,SEEK_SET);
    
    char *str = (char *)malloc(size+1);
    fread(str,size,1,a);
    str[size]=0;
    fclose(a);
    
    char *vshad,*fshad,*gshad=NULL;
    
    for(char *q=str;*q;q++)
    {
        if(*q=='%' && q[1]=='%')
        {
            // mark end of previous element
            *q = 0;
            // and go forward
            q+=2;
            if(!strncmp(q,"vertex",6))
            {
                q+=6;
                vshad = q;
            }
            else if(!strncmp(q,"fragment",8))
            {
                q+=8;
                fshad = q;
            }
            else if(!strncmp(q,"geometry",8))
            {
                q+=8;
                gshad = q;
            }
        }
    }
    
    if(!fshad)
        throw Exception().set("fragment shader missing in %s",mName);
    if(!vshad)
        throw Exception().set("vertex shader missing in %s",mName);
    
    mpVShader = handleInclusion(vshad);
    mpFShader = handleInclusion(fshad);
    
    if(gshad)
        mpGShader = handleInclusion(gshad);
    else
        mpGShader = NULL;
    
    free(str);
    compile();
}

void Effect::initFromData(const char *vshad,const char *fshad){
    mpVShader = strdup(vshad);
    mpFShader = strdup(fshad);
    compile();
}      


Effect::~Effect(){
    if(program)
        glDeleteProgram(program);
    if(vshader)
        glDeleteShader(vshader);
    if(fshader)
        glDeleteShader(fshader);
    if(gshader)
        glDeleteShader(gshader);
    if(mpVShader)
        free((void *)mpVShader);
    if(mpFShader)
        free((void *)mpFShader);
    if(mpGShader)
        free((void *)mpGShader);
}

Effect *Effect::mCurEffect=NULL;

bool Effect::begin(){
    if(mCurEffect != this)
    {
        glUseProgram(program);
        ERRCHK;
        setUniforms();
        ERRCHK;
        mCurEffect = this;
        return true;
    } else {
        setUniforms();
        ERRCHK;
        
        return false;
    }
}

void Effect::end(){
    glUseProgram(0);
    mCurEffect=NULL;
}

int Effect::getAttribute(const char *name){
    int i = glGetAttribLocation(program,name);
//    if(i<0)
//        throw Exception().set("attribute not found: %s",name);
    printf("attr %s: %d\n",name,i);
    return i;
}

int Effect::getUniform(const char *name)
{
    int i = glGetUniformLocation(program,name);
//    if(i<0)
//        throw Exception().set("uniform not found: %s",name);
    printf("uniform %s: %d\n",name,i);
    return i;
}

/*****************************************************************************
 * 
 * 
 * 
 ****************************************************************************/

void Effect::getAttributes(){
    if(has(EDA_POS))
        mPosIdx = getAttribute("aPosition");
    if(has(EDU_WORLDVIEWPROJ))
        mWorldViewProjIdx = getUniform("matWorldViewProj");
    if(has(EDU_DIFFUSECOL))
        mDiffuseIdx = getUniform("colDiffuse");
    if(has(EDA_TEXCOORDS))
        mTexCoordIdx = getAttribute("aTexCoords");
    if(has(EDU_SAMPLER))
        mSamplerIdx = getUniform("sTex");
    if(has(EDU_NORMMAT))
        mNormalMatIdx = getUniform("matNormal");
    if(has(EDA_NORM))
        mNormIdx = getAttribute("aNormal");
    
    if(has(EDU_DIFFLIGHTS)){
        LightCol1Idx = getUniform("uLight1Col");
        LightCol2Idx = getUniform("uLight2Col");
        LightCol3Idx = getUniform("uLight3Col");
        LightDir1Idx = getUniform("uLight1Dir");
        LightDir2Idx = getUniform("uLight2Dir");
        LightDir3Idx = getUniform("uLight3Dir");
    }
    if(has(EDU_AMBLIGHT))
        AmbientColIdx = getUniform("uAmbient");
    if(has(EDU_FOG)){
        FogColIdx = getUniform("uFogCol");
        FogDistIdx = getUniform("uFogDist");
    }
    if(has(EDU_DIFFUSE2)){
        mDiffuse2Idx = getUniform("colDiffuse2");
    }
    if(has(EDU_SAMPLER2)){
        mSampler2Idx = getUniform("sTex2");
    }
    if(has(EDU_WORLDVIEW))
        mWorldViewIdx = getUniform("matWorldView");
}

void Effect::setWorldMatrix(glm::mat4 *world){
    glm::mat4 modelview,worldviewproj;
    
    State *s = StateManager::getInstance()->get();
    
    // these have to be FULL multiplies!
    modelview = s->view * (*world);
    worldviewproj = EffectManager::projection * modelview;
    
    if(has(EDU_WORLDVIEWPROJ)){
        glUniformMatrix4fv(mWorldViewProjIdx,1,
                           GL_FALSE,&worldviewproj[0][0]);
        ERRCHK;
    }
    if(has(EDU_WORLDVIEW)){
        glUniformMatrix4fv(mWorldViewIdx,1,
                           GL_FALSE,&modelview[0][0]);
        ERRCHK;
    }
    
    // set up the normal matrix
    
    if(has(EDU_NORMMAT)){
        glm::mat3 m = glm::inverseTranspose(glm::mat3(modelview));
        glUniformMatrix3fv(mNormalMatIdx,1,GL_FALSE,&m[0][0]);
        ERRCHK;
    }
    
    if(has(EDU_DIFFLIGHTS)){
        glUniform4fv(LightCol1Idx,1,(float *)&s->light.col[0]);
        glUniform4fv(LightCol2Idx,1,(float *)&s->light.col[1]);
        glUniform4fv(LightCol3Idx,1,(float *)&s->light.col[2]);
    
        // transform light into view space
    
        glm::vec3 v;
        glm::mat3 m = glm::mat3(s->view);
        
        v = m*s->light.dir[0];
        glUniform3fv(LightDir1Idx,1,&v[0]);
        v = m*s->light.dir[1];
        glUniform3fv(LightDir2Idx,1,&v[0]);
        v = m*s->light.dir[2];
        glUniform3fv(LightDir3Idx,1,&v[0]);
    
    }
}

void Effect::setUniforms(){
    State *s = StateManager::getInstance()->get();
    
    if(has(EDU_AMBLIGHT)){
        glUniform4fv(AmbientColIdx,1,(float *)&s->light.ambient);
        ERRCHK;
    }
    
    if(has(EDU_FOG)){
        glUniform4fv(FogColIdx,1,(float *)&s->fog.colour);
        glUniform2fv(FogDistIdx,1,(float *)&s->fog.neardist);
    }
    if(has(EDU_DIFFUSE2)){
        glUniform4fv(mDiffuse2Idx,1,(float*)&s->diffuse2);
        ERRCHK;
    }
    if(has(EDU_SAMPLER2)){
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,s->texID1);
        glUniform1i(mSampler2Idx,1);
        ERRCHK;
    }
}    

void Effect::setMaterial(const float *diffuse,GLuint texture)
{
    State *s = StateManager::getInstance()->get();
    if(has(EDU_DIFFUSECOL)){
        float col[4];
        if(s->overrides & STO_DIFFUSE)
            diffuse = (float *)(&s->diffuse);
        if(s->overrides & STO_ALPHA){
            col[0]=diffuse[2];
            col[1]=diffuse[1];
            col[2]=diffuse[0];
            col[3]=s->alpha;
            diffuse = col;
        }
        
        glUniform4fv(mDiffuseIdx,1,diffuse);
        ERRCHK;
    }
    
    if(has(EDU_SAMPLER) && texture){
        if(s->texID0)texture=s->texID0; // texture override
        glActiveTexture(GL_TEXTURE0);
        ERRCHK;
        glBindTexture(GL_TEXTURE_2D,texture);
        ERRCHK;
        glUniform1i(mSamplerIdx,0);
        ERRCHK;
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    }
    if(s->modes & STM_ADDITIVE)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    else
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        
}


/*****************************************************************************
 * 
 * 
 * 
 ****************************************************************************/

void Effect::setArrayOffsetsPrelit()
{
    if(has(EDA_POS)){
        glVertexAttribPointer(mPosIdx,3,GL_FLOAT,GL_FALSE,
                              sizeof(PRELITVERTEX),(const void *)offsetof(PRELITVERTEX,x));
        glEnableVertexAttribArray(mPosIdx);
    }
    
    if(has(EDA_TEXCOORDS)){
        glVertexAttribPointer(mTexCoordIdx,2,GL_FLOAT,GL_FALSE,
                              sizeof(PRELITVERTEX),(const void *)offsetof(PRELITVERTEX,u));
        glEnableVertexAttribArray(mTexCoordIdx);
    }
}


/*****************************************************************************
 * 
 * 
 * 
 ****************************************************************************/

void Effect::setArrayOffsetsUnlit()
{
    if(has(EDA_POS)){
        glVertexAttribPointer(mPosIdx,3,GL_FLOAT,GL_FALSE,sizeof(UNLITVERTEX),
                              (const void *)offsetof(UNLITVERTEX,x));
        glEnableVertexAttribArray(mPosIdx);
    }
    if(has(EDA_NORM)){
        glVertexAttribPointer(mNormIdx,3,GL_FLOAT,GL_FALSE,sizeof(UNLITVERTEX),
                              (const void *)offsetof(UNLITVERTEX,nx));
        glEnableVertexAttribArray(mNormIdx);
    }
    if(has(EDA_TEXCOORDS)){
        glVertexAttribPointer(mTexCoordIdx,2,GL_FLOAT,GL_FALSE,sizeof(UNLITVERTEX),
                              (const void *)offsetof(UNLITVERTEX,u));
        glEnableVertexAttribArray(mTexCoordIdx);
    }
}

