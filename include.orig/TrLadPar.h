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
private:
  short int _filled;
  
public:  
  
  /// Hardware ID (HwId = iTDR + iCrate*100)
  short int _hwid;
  
 public:

  /// Ladder Gains (XY) (average of VAs:  11, ..., 16 and 1, ..., 10)
  float _gain[2];
  /// Ladder Gain for each VA (mean of 64 strip signals) 
  float _vagain[16];
 public:

  /// Default constructor
  TrLadPar(short int hwid = 0);
  /// Copy constructor
  TrLadPar(const TrLadPar& orig);
  /// Default destructor
  ~TrLadPar(){}
  /// Clear data members
  void Clear(const Option_t* aa=0);

  /// Get the HwId
  short int GetHwId() const   { return _hwid; }
  /// Set Ladder Gain (XY) 
  inline void  SetGain(float* gain)           { for (int ii=0; ii< 2; ii++) _gain[ii] = gain[ii]; }  
  /// Set Ladder Gain (XY) by index
  inline void  SetGain(int icoo, float gain)  { _gain[icoo] = gain; }
  /// Set Gain for each VA (fine correction) 
  inline void  SetVAGain(float* gain)         { for (int ii=0; ii<16; ii++) _vagain[ii] = gain[ii]; }
  /// Set Gain for each VA (fine correction) by index
  inline void  SetVAGain(int iva, float gain) { _vagain[iva] = gain; }

  /// Get Ladder Gain (XY) 
  inline float GetGain(int icoo)  { return _gain[icoo]; }
  /// Get Gain for each iVA (fine correction) 
  inline float GetVAGain(int iva) { return _vagain[iva]; }
  /// The ladder param  are acquired?
  inline bool IsFilled()  { return _filled==1; }
  /// Set the ladder param  acquisition status
  inline void SetFilled() { _filled = 1; } 
  /// Print info 
  void PrintInfo();


  /// It copy the Param data to a linear vector 
  ///  in memory starting at address offset
  int Par2Lin(float* offset);


  /// It fill  the param from o a linear vector 
  ///  in memory starting at address offset
  int Lin2Par(float* offset);


  static int GetSize() {return (3*4+16*4);}


  ClassDef(TrLadPar,1);
};

#endif
