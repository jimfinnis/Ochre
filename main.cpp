/**
 * @file main.cpp
 * @brief  Brief description of file.
 *
 */

#include <exception>

#include "gfx.h"
#include "effect.h"
#include "state.h"
#include "obj.h"

// Shader sources
const GLchar* vertexSource =
    "attribute vec4 position;    \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = vec4(position.xyz, 1.0);  \n"
    "}                            \n";
const GLchar* fragmentSource =
//    "precision mediump float;\n"
    "void main()                                  \n"
    "{                                            \n"
    "  gl_FragColor = vec4 (1.0, 1.0, 1.0, 1.0 );\n"
    "}                                            \n";



int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    auto wnd(
        SDL_CreateWindow("test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    auto glc = SDL_GL_CreateContext(wnd);

    auto rdr = SDL_CreateRenderer(
        wnd, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    
    
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // initialise GL state and projection
    Matrix::setProjection(90,1,2000,640.0/480.0);
    Matrix::setProjectionOrtho(0,5,
                               5,5,
                               0,10);
   
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    // load the effects by starting the effect manager
    EffectManager::getInstance();
    
    // load meshes
    ObjMesh *test = new ObjMesh("media/meshes/ico","ico.obj");
    
    while(1)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) std::terminate();
        }


        /* Clear the color and depth buffers. */
        glClearColor(0,0,1,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // reset the state manager
        StateManager *sm = StateManager::getInstance();
        sm->reset();
        
        // draw here!
        MatrixStack *ms = StateManager::getInstance()->getx();
        ms->push();
        
        Matrix m=Matrix::IDENTITY;
        m.setTranslation(0,0,-2);
        ms->mul(&m);
        {
            test->render(sm->getx()->top());
        }
        ms->pop();
        
        
        
        SDL_GL_SwapWindow(wnd);
    };

    return 0;
}
