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
  // temporary: calibration sigma (used for DSP)
  double _sigma;
  // charge parametrization
  static double chargepars[2][2][6];

 public:

  //! Constructor
  TrSimCluster(vector<double> signal, int address, int seedind, double sigma = 0.);
  //! Destructor
  ~TrSimCluster() { Clear(); }
  //! Clear data members
  void   Clear();
  //! Infos
  void   Info(int verbose=0);
  //! Set noise 
  void   SetSigma(double sigma) { _sigma = sigma; }
  //! Get signal
  double GetSignal(int i);
  //! Get noise 
  double GetSigma()   { return _sigma; }
  //! Get signal/noise
  double GetSN(int i) { return (GetSigma()>0.) ? GetSignal(i)/GetSigma() : 0.; } 
  //! Get signal at address (0 if invalid address, no error!!!)
  double GetSignalAtAddress(int i);
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
  //! Get eta
  double GetEta();
  //! Get eta computed with the third strip 
  double GetOtherEta() { return 1. - GetEta(); } 
  //! Get CofG with 3 strips 
  double GetCofG3();
  //! Get total signal
  double GetTotSignal();
  //! Get X with eta (used for residual calculation) 
  double GetXEta()      { double eta = GetEta();      return (eta>0.5) ? GetAddress() + FindSeedIndex() - 1 + eta : GetAddress() + FindSeedIndex() + eta; }   
  double GetXOtherEta() { double eta = GetOtherEta(); return (eta>0.5) ? GetAddress() + FindSeedIndex() - 1 + eta : GetAddress() + FindSeedIndex() + eta; }
  //! Get X with 3 strips (LCR)
  double GetXCofG3()    { return GetAddress() + FindSeedIndex() + GetCofG3(); } 

  // Temporary implementation

  //! Multiply a cluster by a number (signal rescaling)
  void   Multiply(double signal);
  //! LanGauExp static functions
  static double LanGauExpPdf(double* x, double *par);
  static double LanGauExpFun(double* x, double *par);
  static double LanGauExpIntInv(double integral, double *par);
  
  //! Reclusterize with a pseudo-DSP code
  void   DSPClusterize(double seed, double neig);
  //! It returns a Cluster summed with another 
  void   AddCluster(TrSimCluster* cluster);
};

#endif

