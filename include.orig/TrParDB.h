// $Id: TrParDB.h,v 1.1 2009/04/03 08:39:24 pzuccon Exp $

#ifndef __TrParDB__
#define __TrParDB__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrParDB.h
///\brief Header file of TrParDB class
///
///\class TrParDB
///\brief The tracker parameters data base
///\ingroup tkdbc
///
/// author: A. Oliva -- INFN Perugia 19/06/2008 
///
///\date  2008/06/19 AO  First version
///$Date: 2009/04/03 08:39:24 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////




#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "TkDBc.h"
#include "TrLadPar.h"

#include "TObject.h"
#include "typedefs.h"
#define PARDBOFF 3


//! The AMS Tracker parameters database class
class TrParDB :public TObject{
  static float* linear;
 public:
  
  //! map for fast binary search based on tk-hwid
  map<int,TrLadPar*> trpar_hwidmap;

  //! PN gain (normalization to the N side)
  float _pngain;
  //! Generic correction: Cluster Asymmetry Correction (XY)
  float _asymmetry[2];
    
 public:
  
  /// Default contructor
  TrParDB(){}
  /// Constructor with a root file
  TrParDB(char * filename);
  /// Default destructor
  ~TrParDB();
  /// Initialization
  void init();
  /// Load a root file 
  static void Load(char* filename);
  static void Read(char* filename) { Load(filename); } 
  /// TrLadPar singleton pointer
  static TrParDB* Head;

  /// Entries in the TrLadPar map
  int GetEntries() {return trpar_hwidmap.size();}
  /// Get a TrLadPar by map index
  TrLadPar* GetEntry(int ii);
  
  /// Write the content of the parameters DB to an ascii file
  // int write(char* filename) { return 0; }
  /// Read the  content of the parameters DB from an ascii file
  // int read(char* filename) { return 0; }

  /// Set PN Gain
  inline void  SetPNGain(float gain)              { _pngain = gain; }
  /// Set PN Gain
  inline float GetPNGain()                        { return _pngain; }
  /// Set Asimmetry Correction (XY) 
  inline void  SetAsymmetry(float* corr)          { for (int ii=0; ii< 2; ii++) _asymmetry[ii] = corr[ii]; }
  /// Set Asimmetry Correction (XY) by index
  inline void  SetAsymmetry(int icoo, float corr) { _asymmetry[icoo] = corr; }
  /// Get AsimmetryCorrection (XY) 
  inline float GetAsymmetry(int icoo)             { return _asymmetry[icoo]; }

  //! Returns the pointer to the TrLadPar obj with the required Assembly id. In case of failure returns a NULL pointer
  TrLadPar* FindPar_TkAssemblyId(int tkassemblyid);
  //! Returns the pointer to the TrLadPar obj with the required tkid. In case of failure returns a NULL pointer
  TrLadPar* FindPar_TkId(int tkid);
  //! Returns the pointer to the TrLadPar obj with the required tkid. In case of failure returns a NULL pointer
  TrLadPar* FindPar_JMDC(int JMDCid);
  //! Returns the pointer to the TrLadPar obj with the required HwId. In case of failure returns a NULL pointer
  TrLadPar* FindPar_HwId(int hwid);
  //! Returns the pointer to the TrLadPar obj with the required ladder name. In case of failure returns a NULL pointer
  TrLadPar* FindPar_LadName(string& name);

  /// Print info 
  void PrintInfo();


  //! Interface to linear storage
  void Lin2ParDB();
  //! Interface to linear storage
  void ParDB2Lin();

  static int  GetLinearSize(){return (PARDBOFF+192*TrLadPar::GetSize())*sizeof(float);}

  //! Setup the linear space for the DB
  static void CreateLinear(){
    linear= new float[GetLinearSize()/4];
  } 

  //! Reset to zero all the param in the lib
  void Clear();

  ClassDef(TrParDB,2);
};

typedef map<int,TrLadPar*>::const_iterator trparIT;

void SLin2ParDB();

#endif

