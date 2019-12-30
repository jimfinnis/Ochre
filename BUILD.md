# Requirements

- OpenGL 2
- SDL 2
- SDL 2 TTF
- OpenGL GLM
- DevIL crossplatform image library (libdevil-dev)

# Build steps on Linux

```
mkdir build
cd build
cmake ..
make

ln -s ../media .
```
The last step is to get a link to the media directory in the build
directory, so you can run with ```./ochre``` from inside the build
directory (better for debugging).

