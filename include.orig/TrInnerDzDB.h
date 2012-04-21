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

/*!
  Class TrInnerDzDB, it holds the InnerTracker Time dependent Dz Layer corrections
  It provides Update of the TkDBc for a given time, with fall back to TDV if the requested time is not in the DB
  The provided parameter can be equal to the closest timeset int he DB or they can be interpolated with the two closest timesets

  It can store the DB into the TDV.
  Each TDV entry is composed by two following sets on the DB and has time validity from T0 to T1.
  Utility functions are provided to copy the DB to the TDV and viceversa and to read/write from/to a text file.

  During the GBATCH running, as the TDV is updated be new events being reconstructed  the DB grows 
  the way that at the end of the run the DB contains all the Dz Alignment data for that run and can be optionnaly saved on output.

*/

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

  /// map implementig the DB
  std::map <uint,DzElem> pos;
  
  typedef std::map<uint,DzElem>::iterator mapit;
    
  /// std constructor (private is a singleton)
  TrInnerDzDB(){}
  static TrInnerDzDB* Head;
#pragma omp threadprivate (Head)
   
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
  /// Clear the DB content
  void Clear(){pos.clear();return;}
  /// look up on the DB and update the TkDBC _layer_deltaZA values.
  /// In the case the time is not available it tries to seach fot it in the TDV 
  int UpdateTkDBc(uint timeid);

  /// search on the TDV the data good for the requested time and update the DB
  int GetFromTDV(uint timeid);
 
  /// Read the Dz values from a file (Format %u %e %e %e %e %e %e %e)
  int ReadFromFile(char * filename);
  /// Write the DB content to a file one record per line
  int WriteToFile(char * filename, int append=0);
  /// Copy the all the TDV content to the DB
  int TDV2DB();
  /// Writes on the TDV the current content of the DB
  int DB2TDV();
  /// Adds an entry in the TDV
  void AddEntry(uint Timeid,float* dz){
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

  /// TDV Swap Space
  static float TDVSwap[2*(1+kLaynum)];
#pragma omp threadprivate (TDVSwap)
  /// TDV Swap Space Size
  static int   GetTDVSwapSize(){return 2*(2+kLaynum)*sizeof(float);}


};

void TrInnerLin2DB();
#endif
