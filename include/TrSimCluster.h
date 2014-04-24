#ifndef __TrSimCluster__
#define __TrSimCluster__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSimClusterr
///\brief TrSim2010: Class for the Tracker Cluster simulation   
///       
/// Used to represent 2 type of clusters
/// a) implant pitch clusters 
/// b) readout pitch clusters 
///
//////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TkCoo.h"
#include "TrLadPar.h"
#include "TrParDB.h"
#include "TrGainDB.h"
#include "TrCluster.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>

#define VERBOSE 0
#define WARNING 0

using namespace std;

class TrSimCluster {

 public:

  // signals vector
  vector<double> _signal;
  // address of the first strip
  int _address;
  // seed index
  int _seedind;

 public:

  //! Constructor
  TrSimCluster() { Clear(); }
  //! Copy
  TrSimCluster(const TrSimCluster& orig); 
  //! Constructor
  TrSimCluster(vector<double> signal, int address, int seedind = -1);
  //! Assignmnent operator  
  TrSimCluster& operator=(const TrSimCluster& that); 
  //! Destructor
  virtual ~TrSimCluster() { Clear(); }
  //! Clear data members
  void   Clear();
  //! Infos
  void   Info(int verbose=0);

  //! Set signal
  void   SetSignal(int i, double s);
  //! Get signal (0 if out of bounds, no error produced!)
  double GetSignal(int i);
  //! Get total signal
  double GetTotSignal();
  //! Get the cluster lenght
  int    GetWidth()          { return int(_signal.size()); }
  //! Get the address of the first strip
  int    GetAddress()        { return _address; }
  //! Get the address of a strip
  int    GetAddress(int i)   { return _address + i; }
  //! Get the address of a strip with cyclicity 
  int    GetAddressCycl(int i, int nstrips=384) { return (_address + i)%nstrips; }
  //! Get the seed as setted by the constructor
  int    GetSeedIndex()    { return _seedind; } 
  //! Find a seed 
  int    FindSeedIndex(double seed = 0., int force = 0);
  //! Get seed signal
  double GetSeedSignal()   { return _signal.at(FindSeedIndex()); }
  //! Get eta
  double GetEta();
  //! Get 3 strips center of gravity
  double GetCofG(int nstrips); 
  //! Get coordinate
  double GetX(int iside, int tkid, int nstrips, int imult = 0);
  //! Multiply a cluster by a number (signal rescaling)
  void   Multiply(double signal);
  //! It returns a Cluster summed with another 
  void   AddCluster(TrSimCluster& cluster, int nstrips, bool cycl = false);
  //! Gaussianize a fraction of signal of each strip 
  void   GaussianizeFraction(int iside, int hcharge,double fraction,float IP);
  //! Add gaussian noise to each strip 
  void   AddNoise(double noise);
  //! Apply a saturation value to the cluster (from datacard)
  void   ApplySaturation(double maxvalue);
  //! Apply the gain (from TrParDB)
  void   ApplyGain(int iside, int tkid);
  //! Apply asymmetry factor (from TrParDB)
  void   ApplyAsymmetry(int iside);
  //! Apply the non-linearity stretching function (p-side)
  void   ApplyStripNonLinearity();
};

#endif

