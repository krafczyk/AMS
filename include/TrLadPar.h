#ifndef __TrLadPar__
#define __TrLadPar__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadPar.h
///\brief Header file of TrLadPar class
///
///\class TrLadPar
///\brief The ladder parameters data base
///\ingroup tkdbc
///
/// author: A. Oliva -- INFN Perugia 19/06/2008
///
///\date  2008/06/19 AO  First version
///
//////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "TObject.h"


//!Class used to store Tracker Ladder parameters
class TrLadPar : public TObject {
  
 public:  

  /// Hardware ID (HwId = iTDR + iCrate*100)
  short int _hwid;
  
 private:

  /// Gain
  float _gain;
  /// Gain for each VA 
  float _VAgain[16];
  /// Landau-Gaussian parameters for muons ("Width","MPV","Area","Sigma")
  float _langaupar_muons[4];
  
  /// 1 if the ladder parameters is acquired, 0 if not
  short int _filled;
  
 public:

  /// Default constructor
  TrLadPar(short int hwid = 0);
  /// Copy constructor
  TrLadPar(const TrLadPar& orig);
  /// Default destructor
  ~TrLadPar(){}
  /// Clear data members
  void Clear();

  /// Print info (long format if long_format>0)
  void PrintInfo(int long_format = 0);

  ClassDef(TrLadPar,1);
};

#endif
