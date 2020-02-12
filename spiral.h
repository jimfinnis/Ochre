/**
 * @file spiral.h
 * @brief  Brief description of file.
 *
 */

#ifndef __SPIRAL_H
#define __SPIRAL_H

#include <vector>
#include <algorithm>

// a way of searching a spiral around a point. This spirals out
// from 0,0 as next() is called. Layer is how far from the centre
// we are.

class SpiralSearch {
    int leg;
public:
    int x,y,layer,mode;
    void start(){
        x=y=0;
        layer=1;leg=0;
        mode=rand()%4;
    }
    void next(){
        switch(mode){
        case 0:
            switch(leg){
            case 0:if(++x==layer)++leg;break;
            case 1:if(++y==layer)++leg;break;
            case 2:if(-(--x)==layer)++leg;break;
            case 3:if(-(--y)==layer){leg=0;++layer;}break;
            }
            break;
        case 1:
            switch(leg){
            case 0:if(++y==layer)++leg;break;
            case 1:if(++x==layer)++leg;break;
            case 2:if(-(--y)==layer)++leg;break;
            case 3:if(-(--x)==layer){leg=0;++layer;}break;
            }
            break;
        case 2:
            switch(leg){
            case 0:if(-(--y)==layer)++leg;break;
            case 1:if(-(--x)==layer)++leg;break;
            case 2:if(++y==layer)++leg;break;
            case 3:if(++x==layer){leg=0;++layer;}break;
            }
            break;
        case 3:
            switch(leg){
            case 0:if(-(--x)==layer)++leg;break;
            case 1:if(-(--y)==layer)++leg;break;
            case 2:if(++x==layer)++leg;break;
            case 3:if(++y==layer){leg=0;++layer;}break;
            }
            break;
        }
    }
};

struct SRSPoint{
    int x,y;
};

class RandomSpiralSearch {
    SpiralSearch s;
    std::vector<SRSPoint> pointsInLayer;
    int pointIdx;
    
    // use the spiral search to build a single entire layer, collecting
    // the points. Then shuffle them.
    void buildNextLayer(){
        pointsInLayer.clear();
        int curlayer=s.layer; // remember the layer we're on
        while(s.layer==curlayer){
            // and keep adding points from the spiral until the layer changes.
            SRSPoint p;
            p.x = s.x;
            p.y = s.y;
            pointsInLayer.push_back(p);
            s.next();
        }
        curlayer=s.layer;
        // now shuffle.
        std::random_shuffle(pointsInLayer.begin(),pointsInLayer.end());
        pointIdx=0;
    }
    
    void getPoint(){
        if(pointIdx>=pointsInLayer.size()){
            buildNextLayer();
        }
        x = pointsInLayer[pointIdx].x;
        y = pointsInLayer[pointIdx].y;
        pointIdx++;
    }
    
public:
    int x,y,layer;
    void start(){
        pointIdx=0;
        s.start();
        layer = 0;
        getPoint();
    }
    void next(){
        layer = s.layer;
        getPoint();
    }
    
    
    
};

#endif /* __SPIRAL_H */
