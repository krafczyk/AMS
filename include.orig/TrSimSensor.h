#ifndef __TrSimSensor__
#define __TrSimSensor__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSimSensor
///\brief TrSim2010: class for the Tracker Ladder Electronics Simulation  
///
//////////////////////////////////////////////////////////////////////////

#include "TrSimCluster.h"

#include "TF1.h"
#include "TMath.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>

#define VERBOSE 0
#define WARNING 0

#define MAXDIFFPARS 2
#define MAXCLUSTERWIDTH 5
#define SUBSTRATEWIDTH 300
#define DEGTORAD 0.0174533
#define TOLERANCE 0.00001
#define MAXADC 2000.

using namespace std;

class TrSimSensor {

 public:

  enum ESensorType {
    /// S Type Sensor (640 strips)
    kS  = 0,
    /// K5 Type Sensor (only one sensor, 192 strips)
    kK5 = 1,
    /// K7 Type Sensor (only one sensor, 224 strips)
    kK7 = 2
  };

  enum EDiffusionType {
    /// Uniform distribution (par[0] = box width) 
    kUniform  = 0,
    /// Gauss distribution (par[0] = sigma)
    kGauss = 1,
    /// Gauss*Box distribution (par[0] = box width, par[1] = gaussian smearing)
    kGaussBox = 2,
    /// Triangle distribution (par[0] = triangle base)
    kTriangle = 3
  };

 public:

  // Sensor bonding type (0:S, 1:K5, 2:K7)
  int    _sensor_type;
  // Number of implanted strips 
  int    _nimplants;
  // Number of readout strips
  int    _nreadout;
  // Implant pitch
  double _implant_pitch;

  // Equivalent capacitance (DOWN) 
  vector<double> _CD; 
  // Equivalent capacitance (UP)
  vector<double> _CU; 
  // Equivalent capacitance (LEFT)
  vector<double> _CL; 
  // Equivalent capacitance (RIGHT)
  vector<double> _CR; 

  // Diffusion shape type (0:Uniform, 1:Gauss, 2:Box*Gauss, 3:Triangular, ...)
  int    _diff_type;
  // Diffusion parameters for the selected diffusion model
  double _diff_pars[MAXDIFFPARS];

  // Interstrip capacitance [pF]
  double _Cint;    
  // Backplane-guardring capacitance [pF]
  double _Cbk;    
  // Decoupling capacitance [pF]
  double _Cdec;    

  // EDep in Geant3 for vertical tracks (Landau distribution): ONLY PROTONS
  TF1*   _mcfun;
  // EDep in real data (TB2003) for vertical tracks (LanGauExp): ONLY PROTONS
  TF1*   _refun;
  // Interpolationg function for a fast conversion between MC and data: ONLY PROTONS 
  // TGraph* _mctorefun; 

 public:
   
  //! Default constructor
  TrSimSensor();
  //! Constructor with sensor type
  TrSimSensor(int type); 
  //! Destructor
  ~TrSimSensor() { Clear(); }
  //! Clear model
  void Clear();

  ////////////////
  // Model Parameters 
  ////////////////

  //! Set sensor type (0: S, 1: K5, 2: K7)
  void   SetSensorType(int type);
  //! Set the default parameters for the selected sensor type 
  void   SetDefaults(); 
  //! Set decoupling capacitance [pF]
  void   SetCdec(double cdec)        { _Cdec = cdec; }
  //! Set backplane-guardring capacitance [pF]
  void   SetCbk(double cbk)          { _Cbk = cbk; }
  //! Set interstrip capacitance [pF]
  void   SetCint(double cint)        { _Cint = cint; }
  //! Set diffusion type (0:Uniform, 1:Gauss, 2:Box*Gauss, 3:Triangular)
  void   SetDiffusionType(int type)  { _diff_type = type; }
  //! Set diffusion parameters
  /* Diffusion parameters:
     - type 0: par[0] = box width (at 0 deg)
     - type 1: par[0] = gaussian sigma (at 0 deg)
     - type 2: par[0] = box width (at 0 deg), par[1] = gaussian smearing
     - type 3: par[0] = triangle base (at 0 deg)
  */
  //! Set diffusion radius [um]
  void   SetDiffusionPars(double* pars) { for (int i=0; i<MAXDIFFPARS; i++) _diff_pars[i] = pars[i]; }
  //! Get sensor type (0:S, 1:K5, 2:K7)
  int    GetSensorType()        { return _sensor_type; }
  //! Get number of implantation strips
  int    GetNImplantStrips()    { return _nimplants; }
  //! Get number of readout strips 
  int    GetNReadoutStrips()    { return _nreadout; }
  //! Get implantation pitch [um]
  double GetImplantPitch()      { return _implant_pitch; }
  //! Get decoupling capacitance [pF]
  double GetCdec()              { return _Cdec; }
  //! Get backplane-guardring capacitance [pF]
  double GetCbk()               { return _Cbk; }
  //! Get interstrip capacitance [pF]
  double GetCint()              { return _Cint; }
  //! Get diffusion type (0:Uniform, 1:Gauss, 2:Box*Gauss, 3:Triangular)
  int    GetDiffusionType()     { return _diff_type; }
  //! Get diffusion parameter 
  double GetDiffusionPar(int i) { return _diff_pars[i]; }

  ////////////////
  // Readout/Implant Conversions
  ////////////////

  //! Get implant address from readout strip
  int    GetImplantAddressFromReadoutAddress(int readoutadd);
  //! Get implant address from readout strip (for a floating value)
  double GetImplantAddressFromReadoutAddress(double readoutadd);
  //! Returns true if the i-est implant is read 
  bool   IsReadoutStrip(int impl); 
  //! Get readout address from implant strip (the sensor number is needed for K5 and K7 cases)
  int    GetReadoutAddressFromImplantAddress(int implantadd, int nsens = 0);

  ////////////////
  // Capacitive Net Calculation 
  ////////////////

  //! Calculate the equivalent capacitances for all the implanted strips
  void   CalculateEquivalentCapacitances();
  //! See the equivalent capacitances net
  void   SeeEquivalentCapacitances();
 
  ////////////////
  // Cluster Construction Methods 
  ////////////////

  //! Calculate implant-strip cluster from coordinate [cm] and angle [rad]
  TrSimCluster* MakeImplantCluster(double senscoo, double sensangle);
  //! Calculate readout-strip cluster from a charge injection Q on the i-nth implant (the sensor number is needed for K5 and K7 case)
  TrSimCluster* MakeClusterFromAChargeInjectionOnAnImplant(double Q, int impladd, int nsens = 0);
  //! Calculate readout-strip cluster from a implant-strip cluster (injects the modelized cluster in the capacitive net)
  TrSimCluster MakeClusterFromImplantCluster(TrSimCluster* implclus, int nsens = 0);
  //! Calculate readout-strip cluster from coordinate [cm] and angle [rad] (the sensor number is needed for K5 and K7 case)
  TrSimCluster* MakeCluster(double senscoo, double sensangle, int nsens = 0); 

  ////////////////
  // Cluster Sharing Pdf Library
  ////////////////

  //! Calculate weight for the implanted-strip model (uniform distribution)  
  static double GetWeightUniform(double x, double width, double x1, double x2);
  //! Calculate weight for the implanted-strip model (using a gaussian distribution)
  static double GetWeightGauss(double x, double sigma, double x1, double x2);
  //! Calculate weight for the implanted-strip model (using a gaussian*box distribution)
  static double GetWeightGaussBox(double x, double width, double sigma, double x1, double x2);
  //! Calculate weight for the implanted-strip model (using a triangular pdf distribution)
  static double GetWeightTriangular(double x, double width, double x1, double x2);
  //! Gauss*Box distribution cumulative function (analytical)
  static double CumulativeGaussBox(double x, double t, double sigma);
  //! Triangular distribution cumulative function (analytical)
  static double CumulativeTriangular(double x, double t);

  ////////////////
  // dE/dx Normalization 
  ////////////////

  //! EDep description of real data (ADC units)
  static double LanGauExpFun(double *x, double *par);
  //! EDep description in Geant3 (mean,width,horizontal scaling,vertical scaling) 
  static double LanGauFun(double *x, double *par);
  //! EDep description in Geant3 (mean,width,sigma,horizontal scaling,vertical scaling)
  static double LandauFun(double *x, double *par);
  //| EDep from PDG Bethe-Block (300 um of Si) 
  static double BetheBlock(double z, double betagamma);
  //! Return the pointer to the Geant3 Landau parametrization
  TF1* GetMonteCarloFun() { return _mcfun; }
  //! Return the pointer to the LanGauExp Real Data function 
  TF1* GetRealDataFun()   { return _refun; }
  //! keV to MPV factor
  double GetkeVtoADC()    { return _mcfun->GetParameter(2); }
  //! EDep MC-Data ADC normalization (from ADC to ADC)
  double fromMCtoRealData(double adc);
  
};

#endif

