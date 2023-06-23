/**
 * @file context.cpp
 * @brief  Brief description of file.
 *
 */

#include "gfx.h"
#include "context.h"
#include "except.h"
#include "screen.h"
#include "effect.h"

Context *Context::instance = NULL;
#define GLDEBUG 0

#define FULLSCREEN 0

#if GLDEBUG
void debugcallback(GLenum sourceb, GLenum typeb, GLuint idb,
                   GLenum severityb, GLsizei lengthb, const GLchar* messageb,
                   const void* userParamb){
    printf("%s\n",messageb);
}
#endif

void Context::rebuildFSAA(){
    // build off-screen buf.

    int msaa = 4;
    fbwidth=w;
    fbheight=h;

    if(fb){
        glDeleteRenderbuffers(2,&rbcolor);
        glDeleteFramebuffers(1,&fb);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    glGenRenderbuffers(1, &rbcolor);
    ERRCHK;
    glBindRenderbuffer(GL_RENDERBUFFER, rbcolor);
    ERRCHK;
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_RGB, fbwidth,fbheight);
    ERRCHK;

    glGenRenderbuffers(1, &rbdepth);
    ERRCHK;
    glBindRenderbuffer(GL_RENDERBUFFER, rbdepth);
    ERRCHK;
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa, GL_DEPTH_COMPONENT, fbwidth,fbheight);
    ERRCHK;

    glGenFramebuffers(1,&fb);
    ERRCHK;
    glBindFramebuffer(GL_FRAMEBUFFER,fb);
    ERRCHK;

    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbcolor);
    ERRCHK;
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbdepth);
    ERRCHK;

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        FATAL("bad fb");
    ERRCHK;

}


Context::Context(int ww,int hh) : stat("stat"), tool("tool") {
    fb = 0;
    if(instance)
        FATAL("cannot create multiple screens");
    instance = this;
    curscreen = NULL;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#if GLDEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

#if FULLSCREEN
    wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           1024,968,
                           SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
#else
    wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED,
                           ww,hh,
                           SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
#endif
    SDL_GL_SetSwapInterval(0);
    // this doesn't appear to work...
    SDL_SetWindowMinimumSize(wnd,640,480);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetSwapInterval(0);

    glc = SDL_GL_CreateContext(wnd);

    printf("Blart1\n");fflush(stdout);
//    rdr = SDL_CreateRenderer(wnd, -1,  SDL_RENDERER_TARGETTEXTURE);
    rdr = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    printf("Blart2\n");fflush(stdout);

#if GLDEBUG
    // call to enable debug
    glDebugMessageCallback(debugcallback,NULL);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    GLuint unusedIds = 0;
    glDebugMessageControl(GL_DONT_CARE,
                          GL_DONT_CARE,
                          GL_DONT_CARE,
                          0,
                          &unusedIds,
                          true);
#endif
    // load the effects by starting the effect manager
    EffectManager::getInstance();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

}

void Context::resize(int ww,int hh){
    w = ww;
    h = hh;

    if(ww<640 || hh<480){
        // UGLY HACK because setminimumwindowsize doesn't always work.
        SDL_SetWindowSize(wnd,640,480);
        return;
    }
    rebuildFSAA();
    curscreen->resize(ww,hh);
}

extern bool debugtoggle;
void Context::swap(){
#if CAPTURING
    debugtoggle=1;
#endif

    // if debug toggle is on, we render direct (for testing FSAA)
    if(debugtoggle){
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    } else {
        // bind the FSAA buffer's color 0 to the read buffer and the
        // screen to the draw buffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER,fb);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        // and blit the FSAA buffer to the screen
        glDisable(GL_SCISSOR_TEST);
        glBlitFramebuffer(0,0,fbwidth,fbheight,
                          0,0,fbwidth,fbheight,
                          GL_COLOR_BUFFER_BIT,
                          GL_LINEAR);
        // rebind to the FSAA buffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fb);
    }
    SDL_GL_SwapWindow(wnd);
}
