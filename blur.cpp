/**
 * @file blur.cpp
 * @brief  Brief description of file.
 *
 */

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "blur.h"

void boxBlurH (float *scl, float *tcl, int w, int h, int r) {
    float iarr = 1.0f / (float)(r+r+1);
    for(int i=0; i<h; i++) {
        int ti = i*w;
        int li = ti;
        int ri = ti+r;
        float fv = scl[ti];
        float lv = scl[ti+w-1];
        float val = ((float)r+1.0f)*fv;
        for(int j=0;j<r;j++) val += scl[ti+j];
        for(int j=0;j<=r;j++){
            val += scl[ri++] - fv;
            tcl[ti++] = (val*iarr);
        }
        for(int j=r+1;j<w-r;j++){
            val += scl[ri++] - scl[li++];
            tcl[ti++] = (val*iarr);
        }
        for(int j=w-r;j<w;j++){
            val += lv - scl[li++];
            tcl[ti++] = (val*iarr);
        }
    }
}
void boxBlurT (float *scl, float *tcl, int w, int h, int r) {
    float iarr = 1.0f / (float)(r+r+1);
    for(int i=0; i<w; i++) {
        int ti = i;
        int li = ti;
        int ri = ti+r*w;
        float fv = scl[ti];
        float lv = scl[ti+w*(h-1)];
        float val = ((float)r+1.0f)*fv;
        for(int j=0; j<r; j++) val += scl[ti+j*w];
        for(int j=0;j<=r;j++) {
            val += scl[ri] - fv;
            tcl[ti] = (val*iarr);
            ri+=w; ti+=w;
        }
        for(int j=r+1;j<h-r;j++){
            val += scl[ri] - scl[li];
            tcl[ti] = (val*iarr);
            li+=w; ri+=w; ti+=w;
        }
        for(int j=h-r;j<h;j++){
            val += lv - scl[li];
            tcl[ti] = (val*iarr);
            li+=w; ti+=w;
        }
    }
}


void boxBlur (float *scl, float *tcl, int w, int h, int r) {
    memcpy(tcl,scl,sizeof(float)*w*h);
    boxBlurH(tcl, scl, w, h, r);
    boxBlurT(scl, tcl, w, h, r);
}

void boxesForGauss(int *boxes,float sigma, int n)  // standard deviation, number of boxes
{
    float nf = (float)n;
    float wIdeal = sqrtf((12.0f*sigma*sigma/nf)+1.0f);  // Ideal averaging filter width 
    int wl = (int)wIdeal;  if(wl%2==0) wl--;
    int wu = wl+2;
				
    float mIdeal = (12.0f*sigma*sigma - nf*wl*wl - 4.0f*nf*wl - 3.0f*nf)/(-4.0f*wl - 4.0f);
    float m = roundf(mIdeal);
    // var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );
    
    for(int i=0; i<n; i++){
        boxes[i] = ((i<m?wl:wu)-1)/2 ;
    }
}


// scl=source, tcl = target, w,h=size, r=radius
void gaussBlur (float *scl, float *tcl, int w, int h, int r) {
    int bxs[3];
    boxesForGauss(bxs,r, 3);
    boxBlur (scl, tcl, w, h, (bxs[0]-1)/2);
    boxBlur (tcl, scl, w, h, (bxs[1]-1)/2);
    boxBlur (scl, tcl, w, h, (bxs[2]-1)/2);
}



MultipassBlur::MultipassBlur(int ww,int hh,int r){
    w=ww;h=hh;
    tmp1 = new float[w*h];
    tmp2 = new float[w*h];
    boxesForGauss(bxs,r, NUMBOXES);
    npass=0;
}
MultipassBlur::~MultipassBlur(){
    delete [] tmp1;
    delete [] tmp2;
}


bool MultipassBlur::pass(int n,float *src,float *res){
    switch(n){
    case 0:boxBlurH(src,tmp1,w,h,bxs[0]);return false;
    case 1:boxBlurT(tmp1,tmp2,w,h,bxs[0]);return false;
    case 2:boxBlurH(tmp2,tmp1,w,h,bxs[1]);return false;
    case 3:boxBlurT(tmp1,tmp2,w,h,bxs[1]);return false;
    case 4:boxBlurH(tmp2,tmp1,w,h,bxs[2]);return false;
    case 5:boxBlurT(tmp1,res,w,h,bxs[2]);return true;
    default:break;
    }
}



/*
int main(int argc,char *argv[]){
    float buf[128][128];
    float res[128][128];
    
//    for(int i=0;i<128*128;i++){
//        ((float *)buf)[i]=rand()%20;
    //    }
    buf[64][64]=1;
    
    gaussBlur((float*)buf,(float*)res,128,128,60);
    for(int y=0;y<128;y++){
        for(int x=0;x<128;x++){
            printf("%f",res[x][y]);
            putchar(x==127 ? '\n' : ',');
        }
    }
}
*/
