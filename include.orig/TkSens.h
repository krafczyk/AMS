/// $Id: TkSens.h,v 1.5 2011/03/29 15:48:45 pzuccon Exp $ 
#ifndef _TKSENS_
#define _TKSENS_

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkSens
///\brief A library for the calculation from the global to local coordinate
///\ingroup tkrec
///
///\date  2008/03/18 PZ  First version
///\date  2008/04/02 SH  Some bugs are fixed
///\date  2008/04/18 SH  Updated for alignment study
///\date  2008/04/21 AO  Ladder local coordinate
///$Date: 2011/03/29 15:48:45 $
///
/// $Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include "TkLadder.h"

//! Tracker Utility class for the calculation from the global to local coordinate

class TkSens {

 private:

  // Input data
  
  //! MC flag
  bool _isMC;
  //! The Global Coordinate input
  AMSPoint GlobalCoo;
  //! The optional Global Dirction input
  AMSDir   GlobalDir;

  // These data members are calculated

  //! The ladder identifier ( -1 if none)
  int      lad_tkid;
  //! sensor on the ladder
  int      sens;
  //! Multiplicity number
  int      mult;
  //! Local coordinate in the sensor frame
  AMSPoint SensCoo;
  //! Local direction in the sensor frame (actually by def. == LaddDir)
  AMSDir   SensDir;
  //! Local coordinate in the ladder frame
  AMSPoint LaddCoo;
  //! Local direction in the sensor frame
  AMSDir   LaddDir;
  //! nearest K side readout channel
  int      ReadChanX;
  //! nearest S side readout channel
  int      ReadChanY;
  //! The X coo. Impact Point in strip pitch unit wrt the nearest strip
  number   ImpactPointX;
  //! The Y coo. Impact Point in strip pitch unit wrt the nearest strip
  number   ImpactPointY;
  //! The XZ plane Impact Angle 
  number   ImpactAngleXZ;
  //! The YZ plane Impact Angle 
  number   ImpactAngleYZ;

  // Private methods

  //! checks if the point is on a layer
  int  GetLayer();
  //! checks if the point is on a layer J scheme
  int  GetLayerJ();
  //! checks if it is inside the ladder pointed by lad
  bool IsInsideLadder(int tkid);
  //! checks if it is inside the ladder pointed by lad
  bool IsInsideLadder(TkLadder* lad);
  //! Determines on which ladder (if any) is the point
  int  FindLadder();
  //! find the sensor number on the ladder(0-n) and the local coo on it
  int  GetSens();
  //! find the nearest readout channel from sensor S local coo
  int  GetStripFromLocalCooS(number Y);
  //! find the nearest readout channel from sensor K5 local coo
  int  GetStripFromLocalCooK5(number X,int sens);
  //! find the nearest readout channel from sensor K7 local coo
  int  GetStripFromLocalCooK7(number X,int sens);

 public:

  //! Default Constructor
  TkSens(bool MC);
  //! Constructor calculates all the quantities from a point in global coo
  TkSens(AMSPoint& GCoo, bool MC);
  //! Constructor calculates all the quantities from a point/direction in global coo
  TkSens(AMSPoint& GCoo, AMSDir& GDir,bool MC); 
  //! Constructor calculates all the quantities from the tkid and a point in global coo 
  TkSens(int tkid, AMSPoint& GCoo,bool MC);
  //! Constructor calculates all the quantities from the tkid and a point/direction in global coo
  TkSens(int tkid, AMSPoint& GCoo, AMSDir& GDir, bool MC);

  //! (re)set the global coo.
  void SetGlobalCoo(AMSPoint& pp) { Clear(); GlobalCoo=pp; Recalc(); }
  //! (re)set the global coo. specifying the selected ladder
  void SetGlobalCoo(int tkid, AMSPoint& pp) { Clear(); lad_tkid=tkid; GlobalCoo=pp; Recalc(); }
  //! (re)set the global coo. and dir. 
  void SetGlobal(AMSPoint& pp, AMSDir& aa) { Clear(); GlobalCoo=pp; GlobalDir=aa; Recalc(); }
  //! (re)set the global coo. and dir specifying the selected ladder
  void SetGlobal(int tkid, AMSPoint& pp, AMSDir& aa) { Clear(); lad_tkid=tkid; GlobalCoo=pp; GlobalDir=aa; Recalc(); }
  //! Recalc everything from the global coo
  void Recalc();  

  //! True if the point is on a ladder
  bool     LadFound()     { return lad_tkid!=0; }
  //! Returns the ladder id ( 0 if not on ladder)
  int      GetLadTkID()   { return lad_tkid; }
  //! Returns the sensor number
  int      GetSensor()    { return sens; }
  //! Returns the multiplicity index
  int      GetMultIndex() { return mult; }
  //! Returns the global coordinate
  AMSPoint GetGlobalCoo() { return GlobalCoo; }
  //! Returns the global direction
  AMSDir   GetGlobalDir() { return GlobalDir; }
  //! Returns the local coordinate in the sensor frame (for now == LaddDir) 
  AMSPoint GetSensCoo()   { return SensCoo; }
  //! Returns the local direction in the sensor frame
  AMSDir   GetSensDir()   { return SensDir; }
  //! Returns the local coordinate in the ladder frame 
  AMSPoint GetLaddCoo()   { return LaddCoo; }
  //! Returns the local direction in the ladder frame 
  AMSPoint GetLaddDir()   { return LaddDir; }
  //! Returns the nearest readout channel for K side
  int      GetStripX()    { return ReadChanX; }
  //! Returns the nearest readout channel for S side
  int      GetStripY()    { return ReadChanY; }
  //! Returns the X coo. Impact Point in strip pitch unit wrt the nearest strip (i.e. GetStripX())
  number   GetImpactPointX() { return ImpactPointX; }
  //! Returns the Y coo. Impact Point in strip pitch unit wrt the nearest strip (i.e. GetStripY())
  number   GetImpactPointY() { return ImpactPointY; }
  //! Returns the XZ plane Impact Angle
  number   GetImpactAngleXZ() { return ImpactAngleXZ; }
  //! Returns the YZ plane Impact Angle
  number   GetImpactAngleYZ() { return ImpactAngleYZ; }
  //! Returns if is MC TkSens
  bool IsMC() {return _isMC;}

  //! Clear the calculated content
  void Clear();
};

#endif
