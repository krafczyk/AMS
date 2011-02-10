#ifndef __TrSimCluster__
#define __TrSimCluster__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSimClusterr
///\brief TrSim2010: Class for the Tracker Cluster Simulation   
///       
/// Used to represent 2 type of clusters
/// a) implant pitch clusters 
/// b) readout pitch clusters 
///
//////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TrParDB.h"
#include "TrLadPar.h"

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
  // seed index (used in modelized clusters)
  int _seedind;

 public:

  //! Constructor
  TrSimCluster() { Clear(); }
  //! Copy
  TrSimCluster(const TrSimCluster& orig); 
  //! Constructor
  TrSimCluster(vector<double> signal, int address, int seedind = -1);
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
  //! Get the address of a strip with cyclicity (K7 cluster could extend over 384)
  int    GetAddressK7(int i) { return (_address + i)%384; }
  //! Get the seed as setted by the constructor
  int    GetSeedIndex()      { return _seedind; } 
  //! Find a seed (...equal strips?)
  int    FindSeedIndex(double seed = 0.);
  //! Get seed signal
  double GetSeedSignal()   { return (GetSeedIndex()>=0) ? _signal.at(GetSeedIndex()) : _signal.at(FindSeedIndex()); }
  //! Multiply a cluster by a number (signal rescaling)
  void   Multiply(double signal);
  //! It returns a Cluster summed with another 
  void   AddCluster(TrSimCluster& cluster);
  //! Gaussianize a fraction of signal of each strip 
  void   GaussianizeFraction(double fraction);
  //! Add gaussian noise to each strip 
  void   AddNoise(double noise);
  //! Apply a saturation value to the cluster
  void   ApplySaturation(double maxvalue);
  //! Apply the gain
  void   ApplyGain(int side, int tkid);
};

#endif

