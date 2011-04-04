// $Id: TrCalDB.h,v 1.11 2011/04/04 20:30:58 oliva Exp $

#ifndef __TrCalDB__
#define __TrCalDB__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCalDB.h
///\brief Header file of TrCalDB class
///
///\class TrCalDB
///\brief The new tracker caibration data base
///\ingroup tkdbc
///
/// author: P.Zuccon -- INFN Perugia 20/11/2007
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  File name changed, some utils are added
///\date  2008/01/23 SH  Some comments are added
///$Date: 2011/04/04 20:30:58 $
///
///$Revision: 1.11 $
///
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TObject.h"
#include "typedefs.h"
#include "TrLadCal.h"
#include "TFile.h"
#include "TCollection.h"


//! The AMS Tracker Calibration database class

class TrCalDB :public TObject{

public:
  //! map for fast binary search based on Tkhwid
  map<int,TrLadCal*> trcal_hwidmap;

private:


public:
  TrCalDB(){}
  TrCalDB(char * filename);
  ~TrCalDB();
  /// Init the Database
  void init();
  static void Load(char * filename);
  static TrCalDB* Head;
  static float* linear;
  static int  GetLinearSize(){return (1+192*TrLadCal::GetSize())*sizeof(float);}

  //! The run number
  int run;

  //! Merging (needed for hadd) 
  Long64_t Merge(TCollection* list) { return 1; }

  //! Returns the run number;
  int GetRun(){return run;}

  int GetEntries(){return trcal_hwidmap.size();}
  TrLadCal* GetEntry(int ii);
  
  //! write the content of the DB to an ascii file
  int write(char* filename){return 0;}

  //! read the  content of the DB from an ascii file
  int read(char* filename){ return 0;}

  //! Returns the pointer to the ladder Calibration object with the required Assembly id. In case of failure returns a NULL pointer
  TrLadCal* FindCal_TkAssemblyId( int tkassemblyid);
  
  //! Returns the pointer to the ladder Calibration object with the required tkid. In case of failure returns a NULL pointer
  TrLadCal* FindCal_TkId( int tkid);

  //! Returns the pointer to the ladder Calibration object with the required tkid. In case of failure returns a NULL pointer
  TrLadCal* FindCal_JMDC( int JMDCid);
  
  //! Returns the pointer to the ladder Calibration object with the required HwId. In case of failure returns a NULL pointer
  TrLadCal* FindCal_HwId( int hwid);
  
  //! Returns the pointer to the ladder Calibration object with the required ladder name. In case of failure returns a NULL pointer
  TrLadCal* FindCal_LadName(string& name);

  //! Interface to linear storage
  void Lin2CalDB();
  //! Interface to linear storage
  void CalDB2Lin();

  void Print(int verb){
    for(int ii=0;ii<GetEntries();ii++) 
      GetEntry(ii)->PrintInfo(verb);
  }
  //! Setup the linear space for the DB
  static void CreateLinear(){
    linear= new float[GetLinearSize()/4];
  }


  //! Reset to zero all the calibration in the lib
  void Clear(const Option_t*aa=0);

  //! For GBATCH Cal Decoding
  static integer checkdaqidS(unsigned short int  id); 
  //! For GBATCH Cal Decoding
  static void updtrcalib2009S(integer n, int16u* p); 
  static int  DecodeOneCal( int hwid,int16u * rr,int pri=0);

  //! Save cal to TDV DB
  static int  SaveCal2DB();

  void PrintInfo(int verb){for(int ii=0;ii<GetEntries();ii++) GetEntry(ii)->PrintInfo(verb);}
  ClassDef(TrCalDB,2);
};

typedef map<int,TrLadCal*>::const_iterator trcalIT;

void SLin2CalDB();

#endif

