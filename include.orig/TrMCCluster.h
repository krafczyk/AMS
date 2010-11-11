//  $Id: TrMCCluster.h,v 1.11 2010/11/11 12:42:20 pzuccon Exp $
#ifndef __TrMCClusterR__
#define __TrMCClusterR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrMCClusterR
///\brief A class implemnting the Tracker MC Clusters
///\ingroup tksim
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///\date  2008/07/08 PZ  Compatible with new GBATCH and move build to TrSim
///$Date: 2010/11/11 12:42:20 $
///
///$Revision: 1.11 $
///
//////////////////////////////////////////////////////////////////////////
#include "typedefs.h"
#include "point.h"
#include "TrElem.h"

#include "TObject.h"

class TrSimCluster;


class TrMCClusterR: public TrElem {
protected:
  //! abs(tkid)+1000*(lside)+10000*(sensor+1);
  int _idsoft; 
  //! Geant particle code of the particle originatig the cluster.
  short int _itra;
  //!Address of the leftmost strip in the Sim CLuster
  short int _left  [2];
  //!Address of the  strip nearest to the particle in the Sim CLuster
  short int _center[2];
  //!Address of the rightmost strip in the Sim CLuster
  short int _right [2];
  //! array containg the strip signals for p(=1) and n(=0) sides
  float    _ss[2][5];
  //! Calculated impact point on silicon middle
  AMSPoint _xgl;
  //! Momentum vector of the impinging particle
  AMSPoint _Momentum;
  //! Total energy released in silicon (GeV)
  Double32_t   _sum;
  //! Status word
  int Status;

  //! Noise Marker (0: normal cluster, >0 noise generated)
  static int _NoiseMarker;

  //! Pointer to the TrSimClusters generated by TrSim2010 (0: n-side, 1: p-side)
  TrSimCluster* simcl[2]; //!

public:

//####################  CONSTRUCTORS & C. ###########################
  //! Std constructor build a dummy cluster
  TrMCClusterR(){}
  //! Constructor for a digitized hit
  TrMCClusterR(int idsoft, 
	       AMSPoint xgl,AMSPoint mom, float sum,int itra);
  // Constructor for daq
  TrMCClusterR(AMSPoint xgl, integer itra,geant sum=0);

  virtual ~TrMCClusterR();

//####################  ACCESSORS  ###########################

  //! Return the tkid of the hit ladder
  int GetTkId();
  //! returns the sensor number for the hit
  int GetSensor() {return _idsoft/10000-1;}
  //! Returns the address of the first strip od the cluster
  int GetAdd (int side){return (side==0||side==1) ?       _left[side]         :-1; }
  //! Returns the size of the cluster
  int GetSize(int side){return (side==0||side==1) ? (_right[side]-_left[side]+1):-1; }
  //! Return the Signal for the strips of the generated cluster (X/n=0, Y/p=1) 
  float GetSignal(int ii,int side)  {return (side==0||side==1) ? (_ss[side][ii]):-9999.; }
  //! Returns the Totale energy released in silicon (GeV)
  double Sum(){return _sum;}
  //! Checks the cluster type against the IsNoise flag
  int IsNoise(){ return _itra == _NoiseMarker; }
  AMSPoint GetXgl(){return _xgl;}
  //! Returns the Momentum 
  AMSPoint GetMom(){return _Momentum;}
  //! Returns the part type
  int GetPart(){return _itra;}
  unsigned int checkstatus(int c) const { return Status&c; }
  unsigned int getstatus  (void)  const { return Status; }
  void setstatus  (unsigned int s) { Status |=  s; }
  void clearstatus(unsigned int s) { Status &= ~s; }
  //! Return a pointer to the simulated clusters (if TrSim2010 selected)
  TrSimCluster* GetSimCluster(int side) {
    if(side!=0 && side!=1) return 0;
    if (simcl[0]==0||simcl[1]==0) GenSimClusters();
    return simcl[side];
  }
  //! Creation of the simulated clusters objects (TrSimCluster) of TrSim2010
  void GenSimClusters();
  //! Generates the strip distribution (old model)
  void _shower();

protected:
  static float strip2x(int tkid, int side, int strip, int mult);

  // Functions imported from tkmccl.F
  static double fints(double, double);
  static double fint2(double, double, double, double, double);
  static double fintc(double, double, double, double);
  static double fintr(double, double, double, double);
  static double fintl(double, double, double, double);
  static double fdiff(double, int);

  void _PrepareOutput(int full);

public:
//################ PRINTOUT  ########################################

  /// Print MC cluster basic information on a given stream
  std::ostream& putout(std::ostream &ostr = std::cout);

  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr,  TrMCClusterR &cls) { 
    return cls.putout(ostr); 
  }

  /// Print cluster strip variables 
  void Print(int printopt =0);
  /// Return a string with some info (used for event display)
  char* Info(int iRef);
	friend class TrDAQMC;

 /// ROOT definition
  ClassDef(TrMCClusterR,3);

};



#endif
