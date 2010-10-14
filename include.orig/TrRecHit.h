//  $Id: TrRecHit.h,v 1.20 2010/10/14 09:17:29 shaino Exp $
#ifndef __TrRecHitR__
#define __TrRecHitR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRecHitR
///\brief A class to manage hit reconstruction in AMS Tracker
///\ingroup tkrec
///
/// TrRecHitR is a core of the hit reconstruction.
/// New ladder geometry (TkDBc) and calibration databases (TrCalDB) 
/// are used instead of the original TKDBc, TrIdSoft, and TrIdGeom. 
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\date  2008/03/06 AO  Changing some data members and methods
///\date  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///\date  2008/11/29 SH  _dummyX added, _residual moved to TrTrack
///\date  2009/08/16 PZ  General revison -- new inheritance scheme - std printout
///
//////////////////////////////////////////////////////////////////////////
#include <string>
#include "TrCluster.h"
#include "point.h"
#include "TkCoo.h"
#include "TrElem.h"

#include "amsdbc.h"


class TrRecHitR : public TrElem {

public:
  enum { XONLY = 0x100, YONLY = 0x200, TASHIT = 0x400 };

protected:
  /// Pointer to the X (n-side) TrClusterR in the fTrClusterR collection
  TrClusterR*  _clusterX; //!
  /// Pointer to the Y (p-side) TrClusterR in the fTrClusterR collection
  TrClusterR*  _clusterY; //!
  /// TkLadder ID
  short int   _tkid;
  /// Correlation between X and Y
  float _corr;
  /// Probability of correlation between the X and Y clusters 
  float _prob;
  /// Hit multiplicity 
  short int   _mult;
  /// Multiplicity index (-1 means not yet resolved, >-1 resolved by tracking algorithm)
  short int _imult;
  /// Dummy X-strip position for YONLY hit
  float _dummyX;
  /// Hit global coordinate (multiplicity vector) 
  vector<AMSPoint> _coord;
  /// Hit global coordinate (multiplicity vector) 
  vector<AMSPoint> _bfield;

  /// X Cluster index
  int _iclusterX;
  /// Y Cluster index
  int _iclusterY;

  /// Hit status (...)
  int   Status;


  /// load the std::string sout with the info for a future output
  void _PrepareOutput(int full=0);

 public:

//################# CONSTRUCTOR & C ########################
  /// Default constructor
  TrRecHitR(void);
  /// Copy constructor
  TrRecHitR(const TrRecHitR& orig);
  /// Constructor with clusters
  TrRecHitR(int tkid, TrClusterR* clX, TrClusterR* clY, float corr, float prob, int imult = -1, int status = 0);
  /// Destructor
  virtual ~TrRecHitR();
  /// Clear data members
  void Clear();
  /// Build coordinates
  void BuildCoordinates();

//####################  ACCESSORS ##############################
  /// Access function to TrClusterR Object used; 
  /// \param xy 'x' for x projection; any other for y projection;
  /// Returns index in TrClusterR collection or -1 
  int iTrCluster(char xy) const { return (xy=='x')?_iclusterX:_iclusterY; }

  /// Access function to TrClusterR Object used; 
  /// \param xy 'x' for x projection; any other for y projection;
  /// Returns index in TrClusterR collection or -1 
  TrClusterR* pTrCluster(char xy)  { return (xy=='x')?GetXCluster():GetYCluster(); }

  /// Return a string with hit infos (used for the event display)
  char *Info(int iRef=0);
  
  /// Get ladder TkId identifier 
  int   GetTkId()        const { return _tkid; }
  /// Get ladder layer
  int   GetLayer()       const { return abs(_tkid/100); }
  /// Get ladder slot
  int   GetSlot()        const { return abs(_tkid%100); }
  /// Get ladder slot Side (0 == negative X, 1== positive X)
  int   GetSlotSide()        const { return (_tkid>=0)?1:0; }
  /// Get the pointer to X cluster
  TrClusterR* GetXCluster();
  /// Get the pointer to Y cluster
  TrClusterR* GetYCluster();
  /// Get the index of X cluster
  int GetXClusterIndex() const { return _iclusterX; }
  /// Get the index of Y cluster
  int GetYClusterIndex() const { return _iclusterY; }
  /// Get the hit multiplicity 
  int   GetMultiplicity()      { return _mult; }
  /// Returns the computed global coordinate (if resolved)
  AMSPoint GetCoord() { return ( (0<=_imult) && (_imult<_mult) ) 
			  ? GetCoord(_imult) : _coord.at(0); }
  /// Get the computed global coordinate by multiplicity index
  AMSPoint GetCoord(int imult) { if(_coord.empty()) BuildCoordinates();
    return (0<=imult && imult<_mult) ? _coord.at(imult) : AMSPoint(0,0,0); }

  /// Returns the computed global coordinate (if resolved)
  AMSPoint GetBField() { return ( (0<=_imult) && (_imult<_mult) ) 
			   ? GetBField(_imult) : AMSPoint(0, 0, 0); }
  /// Get the computed global coordinate by multiplicity index
  AMSPoint GetBField(int imult) { if(_coord.empty()) BuildCoordinates();
    return (0<=imult && imult<_mult) ? _bfield.at(imult) : AMSPoint(0,0,0); }


  /// Returns the errors on the computed global coordinate (if resolved)
  AMSPoint GetECoord() {return AMSPoint(0.002,0.003,0.015);}
  /// Get correlation between the X and Y clusters
  float GetCorrelation() const { return _corr;   }
  /// Get probability of correlation between the X and Y clusters 
  float GetProb()        const { return _prob;   }
  /// Get the resolved multiplicity index (-1 if not resolved)
  int   GetResolvedMultiplicity() { return _imult; }
  /// Set the resolved multiplicity index (-1 if not resolved)
  void  SetResolvedMultiplicity(int im) { 
    if (im < 0) im = 0;
    if (im >= _mult) im = _mult-1;
    _imult = im; 
  }


  /// Get dummy strip position
  float GetDummyX() { return _dummyX; }
  /// Set dummy strip position
  void SetDummyX(float dumx) { _dummyX = dumx; }
  /// Returns the signal of the Y cluster 
  float Sum(){return (GetYCluster())? GetYCluster()->GetTotSignal():0;}
  /// Returns the signal sum of the X and Y clusters
  float GetTotSignal() { 
    return ((GetXCluster())? GetXCluster()->GetTotSignal():0)+
           ((GetYCluster())? GetYCluster()->GetTotSignal():0); }
  /// Get X local coordinate (ladder reference frame)
  float GetXloc(int imult = 0, int nstrips = TrClusterR::DefaultUsedStrips);
  /// Get Y local coordinate (ladder reference frame)
  float GetYloc(int nstrips = TrClusterR::DefaultUsedStrips);
  /// Get local coordinate (ladder reference frame, Z is zero by definition)
  AMSPoint GetLocalCoordinate(int imult = 0, 
			      int nstripsx = TrClusterR::DefaultUsedStrips,
			      int nstripsy = TrClusterR::DefaultUsedStrips) { 
    return AMSPoint(GetXloc(imult, nstripsx), GetYloc(nstripsy),0.); }
  /// Get global coordinate (AMS reference system) 
  /// default: nominal position, A: with alignement correction
  AMSPoint GetGlobalCoordinate(int imult = 0, const char* options = "A",
			       int nstripsx = TrClusterR::DefaultUsedStrips,
			       int nstripsy = TrClusterR::DefaultUsedStrips);

  bool OnlyX () { return checkstatus(XONLY); }
  bool OnlyY () { return checkstatus(YONLY); }
  bool TasHit() { return checkstatus(TASHIT); }
  // AMSDBc::USED = 32; (0x0020)
  bool Used  () { return checkstatus(AMSDBc::USED); }
  // AMSDBc::FalseX = 8192; (0x2000)
  bool FalseX() { return checkstatus(AMSDBc::FalseX); }

  /// chek some bits into cluster status
  uinteger checkstatus(integer checker) const{return Status & checker;}
  /// Get cluster status
  uinteger getstatus() const{return Status;}
  /// Set cluster status
  void     setstatus(uinteger status){Status=Status | status;}
  /// Clear cluster status
  void     clearstatus(uinteger status){Status=Status & ~status;}

  /// Compatibility with default Gbatch
  AMSPoint getHit(bool = true) { return GetCoord(); }
  number   getsum()            { return GetTotSignal(); }

  /// Print clusterRec hit  basic information  on a given stream 
  std::ostream& putout(std::ostream &ostr = std::cout);
  friend std::ostream &operator << (std::ostream &ostr,  TrRecHitR &hit){
    return hit.putout(ostr);}
  /// Print hit info (verbose if opt !=0 )
  void  Print(int opt=0);


  ///STD GBATCH compatibility layer
  int lay() const { return abs(_tkid/100); }
  int lad() const {return GetSlot();}
  int half() const{ int l = lay()==8?-1:1;return _tkid*l>0?1:0; }
  // Returns the (minimal) distance between two hits on the selcted coo (x 0, y 1, z 2)
  float HitDist(TrRecHitR & B,int coo);
  /// ROOT definition
  ClassDef(TrRecHitR,1)
};

#endif
