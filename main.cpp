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

void setViewport(int x,int y,int w,int h){
    glViewport(x,y,w,h);
    glScissor(x,y,w,h);
    glEnable(GL_SCISSOR_TEST);
}
    


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
    
    // set the viewport
    setViewport(0,0,320,240);
    
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    
    EffectManager::projection = glm::perspective(glm::radians(45.0f),
                                                 640.0f/480.0f,
                                                 0.1f,100.0f);
    
    EffectManager::projection = 
          glm::ortho(-2.0f,2.0f,  -2.0f,2.0f,  -2.0f,2.0f) *
          glm::rotate(glm::mat4(),glm::radians(-35.264f),glm::vec3(1.0f, 0.0f, 0.0f))*
          glm::rotate(glm::mat4(),glm::radians(-45.0f),glm::vec3(0.0f,1.0f,0.0f));
    
//    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    // load the effects by starting the effect manager
    EffectManager::getInstance();
    
    // load meshes
    ObjMesh *test = new ObjMesh("media/meshes/plane","plane.obj");
    

    while(1)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) std::terminate();
        }
        
        printf("foo\n");

        /* Clear the color and depth buffers. */
        glClearColor(0,0,1,0);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // reset the state manager
        StateManager *sm = StateManager::getInstance();
        sm->reset();
        
        MatrixStack *ms = sm->getx();
        ms->push();
        
        ms->mul(glm::translate(glm::mat4(),glm::vec3(0.0f,-0.0f,-0.0f)));
        
        // draw here!
        test->render(sm->getx()->top());
        ms->pop();
        
        
        SDL_GL_SwapWindow(wnd);
    };

    return 0;
}
