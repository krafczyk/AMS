/// $Id: TkCoo.h,v 1.5 2013/01/02 19:41:41 oliva Exp $ 
#ifndef  _TKCOO_
#define  _TKCOO_

//////////////////////////////////////////////////////////////////////////
///
///\file  TkCoo.h
///\brief Header file of TkCoo class
///
///\class TkCoo
///\brief A library for the calculation of the local and global coordinate
///\ingroup tkrec
///
///\date  2008/02/21 PZ  First version
///\date  2008/03/19 PZ  Add some features to TkSens
///\date  2008/04/10 AO  GetLocalCoo(float) of interstrip position  
///\date  2008/04/22 AO  Swiching back some methods  
///$Date: 2013/01/02 19:41:41 $
///
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"

class TkSens;

/// Class that hold the routines to get local and global Coordiantes for the tracker strips.
class TkCoo{

 public:

  /// \brief Returns the max Coo multiplicity due to K side ambiguity. 
  /// A tkid and a readout channel number must be provided. The answer for and S-side readout channel is always 1.  
  static int   GetMaxMult(int tkid, int   readchann) { return GetMaxMult(tkid,(float)readchann); }
  static int   GetMaxMult(int tkid, float readchann);
  /// \brief Returns the local coordinate corresponding on the readout channel \param readchann on the ladder indicated by \param tkid.
  /// The third field correspond to the requested multiplicity ( 0 to GetMaxMult(tkid,readchann)-1 ).
  /// If  mult is > GetMaxMult(tkid,readchann)-1   the returned coordinate correspont to mult= GetMaxMult(tkid,readchann)-1
  static float GetLocalCoo(int tkid, int   readchann, int mult);
  static float GetLocalCoo(int tkid, float readchann, int mult);

  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local Coo X and Y
  static AMSPoint GetGlobalN(int tkid,float X, float Y);
  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local readout chann and mult
  static AMSPoint GetGlobalNC(int tkid,float readchanK, float readchanS,int mult=0);
 
  /// \brief Returns the Global coordinate (Nominal) for the point on ladder 
  /// \param tkid with local given by the AMSPoint loc (Z value is ignored)
  static AMSPoint GetGlobalN(int tkid,AMSPoint& loc);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local Coo X and Y
  static AMSPoint GetGlobalA(int tkid,float X, float Y);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local readout chann and mult
  static AMSPoint GetGlobalAC(int tkid,float readchanK, float readchanS,int mult=0);
  /// \brief Returns the Global coordinate (Alignment Corrected) for the point on ladder 
  /// \param tkid with local given by the AMSPoint loc (Z value is ignored)
  static AMSPoint GetGlobalA(int tkid,AMSPoint& loc);

  /// \brief Returns the Global coordinate (MC DisAlignment Corrected) for the point on ladder 
  /// \param tkid with local Coo X and Y
  static AMSPoint GetGlobalT(int tkid,float X, float Y);
  /// \brief Returns the Global coordinate (MC DisAlignment Corrected) for the point on ladder 
  /// \param tkid with local readout chann and mult
  static AMSPoint GetGlobalTC(int tkid,float readchanK, float readchanS,int mult=0);
  /// \brief Returns the Global coordinate (MC DisAlignment Corrected) for the point on ladder 
  /// \param tkid with local given by the AMSPoint loc (Z value is ignored)
  static AMSPoint GetGlobalT(int tkid,AMSPoint& loc);

  /// \brief Calculate the strip address on sensor (S: 0, ..., 639; K5: 0, ..., 191; K7: 0, ..., 223)
  /// \param tkid of the ladder
  /// \param readchann readout channel (0, ..., 1024)
  /// \param cluster multiplicity
  /// \param sensor is the calculated sensor number
  /// \param verbose = 0: do not produce error messages, 1: produce error messages
  static int GetSensorAddress(int tkid, int readchann, int mult, int& sens, int verbose = 1);

 private:

  /// Local coo from the S channel
  static float GetLocalCooS(int readchann);
  /// Local coo from the K5 channel
  static float GetLocalCooK5(int readchann, int mult);
  /// Local coo from the K7 channel
  static float GetLocalCooK7(int readchann, int mult);
  /// Implanted Strip on a sensor from bonded strip index on the sensor
  static int   ImplantedFromReadK7(int readstrip);

  friend class TkSens;

public:
  /// Returns the lenght of ladder
  static double GetLadderLength(int tkid);
  /// Return the central position of the ladder
  static AMSPoint GetLadderCenter(int tkid,int isMC=0); 
  /// Return the X center of a ladder
  static double GetLadderCenterX(int tkid,int isMC=0) { return GetLadderCenter(tkid,isMC).x(); }
  /// Return the Y center of a ladder
  static double GetLadderCenterY(int tkid,int isMC=0) { return GetLadderCenter(tkid,isMC).y(); }
  /// Return the Z at the center of ladder
  static double GetLadderCenterZ(int tkid,int isMC=0) { return GetLadderCenter(tkid,isMC).z(); }
};

#endif
