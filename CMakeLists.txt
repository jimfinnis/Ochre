cmake_minimum_required(VERSION 2.6)
project(ochre)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SD2_TTF REQUIRED sdl2)

find_package(OpenGL)
find_package(SDL2TTF)

add_executable(${PROJECT_NAME} main.cpp state.cpp screen.cpp region.cpp font.cpp
    effect.cpp obj.cpp maths.cpp grid.cpp)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS}
     ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS}
     )

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} 
    ${SDL2TTF_LIBRARY}
    ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

