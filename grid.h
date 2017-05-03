/**
 * @file grid.h
 * @brief  Brief description of file.
 *
 */

#ifndef __GRID_H
#define __GRID_H

/// the grid component of the world from which the heightmap is generated.

#define GRIDSIZE 256

class Grid {
    uint8_t grid[GRIDSIZE][GRIDSIZE];
    
public:
    Grid(){
        memset(grid,0,GRIDSIZE*GRIDSIZE);
    }
    
    
    /// generate triangles centred around cx,cy. Areas not in the grid
    /// are at 0,0.
    void genTriangles(int cx,int cy);
        
};


#endif /* __GRID_H */
