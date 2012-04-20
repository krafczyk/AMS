//
//  TrInnerDzDB.h
//  TrInnerDzDB
//
//  Created by Zuccon Paolo on 19/04/12.
//  Copyright (c) 2012 MIT. All rights reserved.
//

#ifndef TrInnerDzDB_TrInnerDzDB_h
#define TrInnerDzDB_TrInnerDzDB_h
#include <map>
#include <cstring>


typedef unsigned int uint;

class TrInnerDzDB {
 public:
    enum {kLaynum=7};

private:
    class DzElem{
    public:
        float dz[kLaynum];
        DzElem(){memset(dz,0,kLaynum*sizeof(dz[0]));}
        DzElem(float *Dz){memcpy(dz,Dz,kLaynum*sizeof(dz[0]));}

    };

    std::map <uint,DzElem> pos;

    typedef std::map<uint,DzElem>::iterator mapit;
  

    
    /// std constructor (private is a singleton)
    TrInnerDzDB(){}
    static TrInnerDzDB* Head;
    
    int TrInnerDB2Lin2TDV(mapit  it);
  public:
    /// Get the pointer to the class single instance
    static TrInnerDzDB* GetHead(int regen=0){
        if(regen && Head){ delete Head; Head=0;}
        if(!Head)
            Head= new TrInnerDzDB();
        return Head;
    }
    /// std detructor
    ~TrInnerDzDB(){}
    int UpdateTkDBc(uint timeid);
    int GetFromTDV(uint timeid);
    int ReadFromFile(char * filename) {return 0;}
    int WriteToFile(char * filename, int append=0) {return 0;}
    int TDV2DB();
    int DB2TDV();
    void AddEntry(uint Timeid,float dz[]){
        DzElem el(dz);
        pos[Timeid]=el;
        return;
    }
    /*! 
     \brief Returns the best parameter set for the requested time 
     \param Timeid UNIX time for the parameters
     \param Dz pointer to a vector of size kLaynum holding the results
     \param type of interpolation: 
     \li 0 find the closest in time, 
     \li 1 linear iterpolation between the two closest sets
    \return a code to express success of failure of the operation
    \retval >=0   --> Success
    \retval -1    --> The Dz pointer is NULL or invalid
    \retval -2    --> The requested time is before the DB first value
    \retval -3    --> The requested time is after the DB last value
    \retval -4    --> The requested type is not 0 or 1
    \retval -5    --> Internal error, should never happen. Contact the developper if observed.
     */
    int GetEntry(uint Timeid, float* Dz, int type=0);

    static float TDVSwap[2*(1+kLaynum)];
    static int   GetTDVSwapSize(){return 2*(2+kLaynum)*sizeof(float);}


};

void TrInnerLin2DB();
#endif
