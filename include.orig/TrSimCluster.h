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
  // seed index (used in mdodelized clusters)
  int _seedind;

 public:

  //! Constructor
  TrSimCluster(vector<double> signal, int address, int seedind, double sigma = 0.);
  //! Destructor
  ~TrSimCluster() { Clear(); }
  //! Clear data members
  void   Clear();
  //! Infos
  void   Info(int verbose=0);

  //! Get signal
  double GetSignal(int i);
  //! Get signal at address (0 if invalid address, no error!!!)
  double GetSignalAtAddress(int i);
  //! Get total signal
  double GetTotSignal();
  //! Get the cluster lenght
  int    GetWidth()      { return int(_signal.size()); }
  //! Get the address of the first strip
  int    GetAddress()    { return _address; }
  //! Get the seed as setted by the constructor
  int    GetSeedIndex()  { return _seedind; } 
  //! Find a seed (...equal strips?)
  int    FindSeedIndex(double seed = 0.);
  //! Get seed signal
  double GetSeedSignal() { return (GetSeedIndex()>=0) ? _signal.at(GetSeedIndex()) : _signal.at(FindSeedIndex()); }
  //! Multiply a cluster by a number (signal rescaling)
  void   Multiply(double signal);
  //! It returns a Cluster summed with another 
  void   AddCluster(TrSimCluster* cluster);

};

#endif

