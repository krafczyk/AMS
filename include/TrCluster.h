// $Id: TrCluster.h,v 1.1 2008/12/18 11:19:24 pzuccon Exp $ 
#ifndef __AMSTrCluster__
#define __AMSTrCluster__

#include "TrRawCluster.h"
#include "TkDBc.h"
#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TkCoo.h"
#include "link.h"
#include <cmath>
#include <vector>
#include <string>
/*!
\class AMSTrCluster
\brief A class to manage reconstructed cluster in AMS Tracker
\ingroup tkrec

 AMSTrCluster is the a core of the Tracker reconstruction. 
 New ladder geometry (TKDBc) and calibration databases (TrCalDB) 
 and strip database (TkStrip) are used instead of 
 the original TKDBc, TrIdSoft, and TrIdGeom. 

\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
\date  2008/01/14 SH  First stable vertion after a refinement
\date  2008/01/20 SH  Imported to tkdev (test version)
\date  2008/02/13 SH  _signal array is changed as vector
\date  2008/02/16 AO  New data format 
\date  2008/02/19 AO  Signal corrections
\date  2008/02/26 AO  Eta based local coordinate (see TkCoo.h)
\date  2008/03/01 AO  Added member _seedind
\date  2008/03/06 AO  Changing some data members and methods
\date  2008/03/31 AO  Eta and CofG methods changing
\date  2008/04/11 AO  XEta and XCofG coordinate based on TkCoo
\date  2008/06/19 AO  Using TrCalDB instead of data members 

 $Date: 2008/12/18 11:19:24 $

 $Revision: 1.1 $

*/

//class AMSTrCluster : public TObject, public AMSlink {
class AMSTrCluster : public AMSlink{

private:
  /// Local coordinate by multiplicity index (for now is XCofG(3))
  vector<float> _coord; //!
  /// Global coordinate by multiplicity index
  vector<float> _gcoord; //!
 
 public:
  /// TkLadder ID (layer *100 + slot)*side 
  short int    _tkid;
  /// First strip address (0-639 for p-side, 640-1023 for n-side)
  short int    _address;
  /// Number of strips
  short int    _nelem;
  /// Seed index in the cluster (0<=_seedind<_nelem)
  short int    _seedind;
  /// ADC data array
  std::vector<float> _signal;
  /// Cluster status
  unsigned int       _clstatus;
  /// Multiplicity;
  short int _mult;

 protected:

  /// This part has to be putted in the TrParDB
  /// Asimmetry correction parameters 
  static float AsimmetryCorr[2];
  /// Gain correction parameters (VA by VA!!!)
  static float GainCorr[2];
  /// Charge loss corrections for protons (in 3 eta regions) 
  static float ChargeLossCorr[2][3];
 
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
 
 public:
  /// Default constructor
  AMSTrCluster(void);
  /// Constructor with data
  AMSTrCluster(int tkid, int side, int add, int nelem, int seedind, float* adc, unsigned int clstatus);
  /// Constructor divided is several instructions 
  AMSTrCluster(int tkid, int side, int add, int seedind, unsigned int clstatus);
  /// Insert a strip in the cluster
  void push_back(float adc);
  /// Build the coordinates (with multiplicity)
  void BuildCoordinates();
  /// Copy constructor
  AMSTrCluster(const AMSTrCluster& orig);
  /// Destructor
  ~AMSTrCluster();
  /// Clear
  void Clear();

  /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }

  /// Get ladder TkId identifier 
  int   GetTkId()          const { return _tkid; }
  /// Get ladder layer
  int   GetLayer()         const { return abs(_tkid/100); }
  /// Get ladder slot
  int   GetSlot()          const { return abs(_tkid%100); }
  /// Get cluster side (0: n-side, 1: p-side)
  int   GetSide()          const { return (_address<640)?1:0; }
  /// Get the cluster first strip number (0-640 for p-side, 0-384 for n-side)  
  int   GetAddress()       const { return _address; }
  /// Get i-th strip address
  int   GetAddress(int ii);
  /// Get the cluster strip multiplicity
  int   GetNelem()         const { return _nelem; }
  /// Get i-th strip signal (A: apply asimmetry correction, G: apply gain correction)
  float GetSignal(int ii, const char* options = "AG");
  /// Get i-th strip sigma (from calibration)
  float GetSigma(int ii) { return GetNoise(ii);  }
  /// Get i-th strip sigma (from calibration)
  // float GetNoise(int ii)   const { return _sigma.at(ii);  }
  float GetNoise(int ii);   
  /// Get i-th signal to noise ratio 
  float GetSN(int ii, const char* options = "AG") { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii,options)/GetNoise(ii); }
  /// Get i-th strip status (the same of TrRawCluster)
  // short GetStatus(int ii)  const { return _status.at(ii); }
  short GetStatus(int ii);
//   /// Get gain 
//   float GetGain()          const { return _gain; }
  /// Get cluster status
  int   GetClusterStatus() const { return _clstatus; } 
  /// Get multiplicity (1 for p-side, >1 for n-side)
  int   GetMultiplicity()   const { return _mult; }
  //int GetMultiplicity() { return TkCoo::GetMaxMult(GetTkId(),GetAddress())+1; }
  /// Get local coordinate by multiplicity index
  float GetCoord(int imult); 
  /// Get global coordinate by multiplicity index
  float GetGCoord(int imult);
  /// Get the seed index 
  int   GetSeedIndex()   { return _seedind; }
  /// Get the seed address 
  int   GetSeedAddress() { return GetAddress(GetSeedIndex()); }
  /// Get seed signal 
  float GetSeedSignal(const char* options = "AG")  { return GetSignal(GetSeedIndex(),options); }
  /// Get seed noise 
  float GetSeedNoise()   { return GetNoise(GetSeedIndex()); }
  /// Get seed signal to noise ratio 
  float GetSeedSN()      { return GetSN(GetSeedIndex()); }
  /// Get the numeber of strips on the left of the seed strip
  int   GetLeftLength()  { return GetSeedIndex(); }
  /// Get the number of strips on the right of the seed strip
  int   GetRightLength() { return GetNelem() - GetSeedIndex() - 1; } 
  /// Get cluster length
  int   GetLength()      { return GetNelem(); }
  /// Get cluster amplitude
  /*
    options:
    A: apply asimmetry correction (signal)
    I: apply inclination correction (???)
    L: apply charge loss correction
    G: apply gain correction 
    N: apply the p/n normalization
  */
  float GetTotSignal(const char* options = "ALG");
  /// Get cluster eta (center of gravity with the two higher strips) 
  /// (A: apply asimmetry correction, G: apply gain correction)
  float GetEta(const char* options = "AG");
  /// Get cluster eta index (0: readout, 1: middle, 2: central) 
  /// (A: apply asimmetry correction, G: apply gain correction)
  int   GetEtaIndex(const char* options = "AG");
  /// Get the cluster amplitude Center of Gravity with the n higher consecutive strips
  float GetCofG(int nstrips=2, const char* options = "AG");

  /// Get the local coordinate for i-th strip (0-1024)
  float GetX(int ii, int imult = 0) { return TkCoo::GetLocalCoo(GetTkId(),GetAddress(ii),imult); }
  /// Get the local coordinate for the given interstrip position (0.-1.)
  float GetX(float interpos, int imult = 0);
  /// Get local coordinate with eta 
  float GetXEta(int imult = 0, const char* options = "AG");
  /// Get local coordinate with center of gravity on nstrips
  float GetXCofG(int nstrips, int imult = 0, const char* options = "AG");

  /// Also global coordinate (with only translations)


  // - useful for eta analysis - //
  /// Get the second strip index (<0 if not)
  int   GetSecondIndex();
  /// Get the second strip address 
  int   GetSecondAddress() { return (GetSecondIndex()==0) ? GetAddress()+GetSecondIndex() : -1; }
  /// Get the second strip signal 
  float GetSecondSignal(const char* options = "AG")  { return (GetSecondIndex()==0) ? GetSignal(GetSecondIndex(),options) : 0.; }
  /// Get the second strip noise signal 
  float GetSecondNoise()   { return (GetSecondIndex()==0) ? GetNoise(GetSecondIndex()) : -1.; }
  /// Get the second trip signal to noise ratio 
  float GetSecondSN()      { return GetSecondSignal()/GetSecondNoise(); } 


  /// Print cluster basic information
  std::ostream& putout(std::ostream &ostr = std::cout) const;
  friend std::ostream &operator << (std::ostream &ostr, const AMSTrCluster &cls) { return cls.putout(ostr); }
  /// Print cluster strip variables (A: apply asimmetry correction, G: apply gain correction)
  void Print(const char* options = "AG");
  void Info(const char* options = "AG");

  static string sout;

  void _copyEl(){}
  void _printEl(std::ostream& ostr){  putout(ostr); }
  void _writeEl(){}

   void * operator new(size_t t) {return TObject::operator new(t); }
   void operator delete(void *p)   {TObject::operator delete(p);p=0;}

  AMSTrCluster* next(){ return (AMSTrCluster*) _next;}
  /// ROOT definition
  ClassDef(AMSTrCluster, 1)
};
typedef AMSTrCluster TrClusterR;
#endif
