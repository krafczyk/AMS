//  $Id: TrMCCluster.h,v 1.3 2009/08/17 12:53:47 pzuccon Exp $
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
///$Date: 2009/08/17 12:53:47 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////
#include "typedefs.h"
#include "point.h"
#include "TrElem.h"

#include "TObject.h"



class TrMCClusterR: public TrElem {
protected:
  //! Status word
  int Status;


public:
  unsigned int _status;
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

  float    _ss[2][5];
  //! impact point on silicon top
  AMSPoint _xca;
  //! impact point on silicon bottom
  AMSPoint _xcb;
  AMSPoint _xgl;
  //! Momentum vector of the impinging particle
  AMSPoint _Momentum;
  //! Totale energy released in silicon (GeV)
  Double32_t   _sum;

  //! Generates the strip distribution 
  void _shower();

  static int _NoiseMarker;



public:
  //! Returns the address of the first strip od the cluster
  int GetAdd (int side){return (side==0||side==1) ?       _left[side]         :-1; }
  //! Returns the size of the cluster
  int GetSize(int side){return (side==0||side==1) ? (_right[side]-_left[side]+1):-1; }
  //! Signal 
  float GetSignal(int ii,int side)  {return (side==0||side==1) ? (_ss[side][ii]):-9999.; }
  //! Return the tkid
  int GetTkId();
  //! Returns the Totale energy released in silicon (GeV)
  double Sum(){return _sum;}
  //! returns the sensor number for the hit
  int GetSensor() {return _idsoft/10000-1;}

  TrMCClusterR(){}
  //! Constructor for float track
  TrMCClusterR(int idsoft, AMSPoint xca, 
		 AMSPoint xcb, AMSPoint xgl,AMSPoint mom, float sum,int itra)
    : _idsoft(idsoft), _itra(itra), _xca(xca), _xcb(xcb), 
      _xgl(xgl), _Momentum(mom), _sum(sum) {
    _shower();
  }
  virtual ~TrMCClusterR(){}

  unsigned int checkstatus(int c) const { return Status&c; }
  unsigned int getstatus  (void)  const { return Status; }
  void setstatus  (unsigned int s) { Status |=  s; }
  void clearstatus(unsigned int s) { Status &= ~s; }
  int IsNoise(){ return _itra == _NoiseMarker; }

 
  static float strip2x(int tkid, int side, int strip, int mult);

  // Functions imported from tkmccl.F
  static double fints(double, double);
  static double fint2(double, double, double, double, double);
  static double fintc(double, double, double, double);
  static double fintr(double, double, double, double);
  static double fintl(double, double, double, double);
  static double fdiff(double, int);


  void print(std::ostream & stream);


  /// ROOT definition
  ClassDef(TrMCClusterR,1);

};



#endif
