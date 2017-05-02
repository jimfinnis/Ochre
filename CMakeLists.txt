cmake_minimum_required(VERSION 2.6)
project(ochre)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

find_package(OpenGL)

add_executable(${PROJECT_NAME} main.cpp shader.cpp)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS}
     ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS}
     )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES}
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

