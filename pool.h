/**
 * @file pool.h
 * @brief  Brief description of file.
 *
 */

#ifndef __POOL_H
#define __POOL_H

#include <new>
#include <stdlib.h>
#include <stdio.h>

/// allocation pool with iterator. Runs ctors and dtors.

template<class T> class Pool {
    int cap;
    T *data;
    int *freenext;
    int freehead;
    int capacity;
    int *allocprev,*allocnext,allochead;
    bool *snark;
public:
    Pool(int cap){
        capacity = cap;
        data = new T[cap];
        freenext = new int[cap];
        allocnext = new int[cap];
        allocprev = new int[cap];
        snark = new bool[cap];
        
        
        for(int i=0;i<cap;i++){
            if(i<cap-1)
                freenext[i]=i+1;
            allocnext[i]=allocprev[i]=-1;
            snark[i]=false;
        }
        freenext[cap-1]=-1;
        freehead=0;
        allochead=-1;
    }
    ~Pool(){
        delete [] freenext;
        delete [] data;
    }
    
    T *alloc(){
        if(freehead==-1){
            return NULL;
        }
        
        // add freehead to the alloc list
        
        if(allochead>=-1)
            allocprev[allochead]=freehead;
        
        allocnext[freehead]=allochead;
        allocprev[freehead]=-1;
        allochead=freehead;

        T *o = data+freehead;
        snark[freehead]=true;;
        freehead = freenext[freehead];
        //        dump("alloc",o-data);
        
        // run constructor
        new(o) T;
        return o;
    }
    
    /// handy for doing some kind of hashing
    int getidx(T *o){
        return o-data;
    }
    
    void dump(const char *deb="???",int n=-1){
        printf("%s %d: Freehead %d, allochead %d\n",deb,n,freehead,allochead);
        for(int i=0;i<capacity;i++){
            printf("%4d   Freenext: %4d  Allocprev: %4d Allocnext: %4d Reallyfree: %s\n",
                   i,freenext[i],allocprev[i],allocnext[i],
                   snark[i]?"":"F");
        }
    }
    
    void free(T *o){
        int idx = o-data;
        snark[idx]=false;
        freenext[idx]=freehead;
        freehead=idx;
        
        // and remove from the alloc list
        if(idx==allochead){
            allochead=allocnext[idx];
        } else {
            int n = allocnext[idx];
            int p = allocprev[idx];
            if(p>=0){
                allocnext[p]=n;
            } if(n>=0) {
                allocprev[n]=p;
            }
        }
        allocprev[idx]=-1;
        allocnext[idx]=-1;
        //        dump("free",idx);
        
        // run destructor
        o->~T();
    }
    
    T *first(){
        return allochead>=0 ? data+allochead : NULL;
    }
    
    T *next(T *o){
        int idx = o-data;
        return (allocnext[idx]>=0) ? data+allocnext[idx] : NULL;
    }
};

#endif /* __POOL_H */
