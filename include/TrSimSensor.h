#ifndef __TrSimSensor__
#define __TrSimSensor__

//////////////////////////////////////////////////////////////////////////
///
///\class TrSimSensor
///\brief TrSim2010: class for the Tracker Sensor simulation  
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

#define MAXCLUSTERWIDTH 20
#define SUBSTRATEWIDTH 300
#define DEGTORAD 0.0174533
#define TOLERANCE 0.00001
#define MAXADC 5000
#define NSTEPSGAUSSSUM 100

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
    /// Uniform distribution 
    kUniform  = 0,
    /// Gauss distribution 
    kGauss = 1,
    /// Gauss*Box distribution 
    kGaussBox = 2,
    /// Triangle distribution 
    kTriangle = 3,
    /// Sum of Gauss distributions 
    kGaussSum = 4
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

  // Diffusion shape type (0:Uniform, 1:Gauss, 2:Box*Gauss, 3:Triangular, 4:GaussSum, ...)
  int    _diff_type;
  // Diffusion radius [um] 
  double _diff_radius;

  // Interstrip capacitance [pF]
  double _Cint;    
  // Backplane-guardring capacitance [pF]
  double _Cbk;    
  // Decoupling capacitance [pF]
  double _Cdec;    

 public:
 
  //! Default constructor
  TrSimSensor() { Init(); }
  //! Constructor with sensor type
  TrSimSensor(int type) { Init(); SetSensorType(type); }
  //! Copy constructor
  TrSimSensor(const TrSimSensor& that) { Copy(that); }
  //! Assignment operator
  TrSimSensor& operator=(const TrSimSensor& that);
  //! Destructor
  virtual ~TrSimSensor() { Clear(); }
  //! Initializer
  void Init();
  //! Clear 
  void Clear();
  //! Copy  
  void Copy(const TrSimSensor& that);

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
  //! Set diffusion radius
  /* Diffusion radius meaning:
     - type 0: box width (at 0 deg)
     - type 1: gaussian sigma (at 0 deg)
     - type 2: gaussian smearing (at 0 deg)
     - type 3: triangle base (at 0 deg)
     - type 4: maxima sigma value (at 0 deg)
  */
  //! Set diffusion radius [um]
  void   SetDiffusionRadius(double radius) { _diff_radius = radius; }

  //! Sensor type is K7 
  bool   IsK7()                 { return (GetSensorType()==kK7); }
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
  //! Get diffusion type (0:Uniform, 1:Gauss, 2:Box*Gauss, 3:Triangular, 4:GaussSum)
  int    GetDiffusionType()     { return _diff_type; }
  //! Get diffusion parameter 
  double GetDiffusionRadius()   { return _diff_radius; }
  //! Get number of readout channels 
  int    GetNChannels();

  ////////////////
  // Readout/Implant Conversions
  ////////////////

  //! Get implant address from readout strip
  int    GetImplantAddressFromReadoutAddress(int readoutadd);
  //! Get implant address from readout strip (for a floating value)
  double GetImplantAddressFromReadoutAddress(double readoutadd);
  //! Returns true if the i-est implant is read 
  bool   IsReadoutStrip(int impl); 
  //! Returns true if the i-est implant is metalized  
  bool   IsMetalizedStrip(int impl); 
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

  //! Calculate implant-strip cluster from coordinate [cm] and angle [rad] and DZ of the step (<=300 um)
  TrSimCluster MakeImplantCluster(double senscoo, double sensangle, double DZStep=300);
  //! Calculate readout-strip cluster from a charge injection Q on the i-nth implant (the sensor number is needed for K5 and K7 case)
  TrSimCluster MakeClusterFromAChargeInjectionOnAnImplant(double Q, int impladd, int nsens = 0);
  //! Calculate readout-strip cluster from a implant-strip cluster (injects the modelized cluster in the capacitive net)
  TrSimCluster MakeClusterFromImplantCluster(TrSimCluster& implclus, int nsens = 0);
  //! Calculate readout-strip cluster from coordinate [cm] and angle [rad] (the sensor number is needed for K5 and K7 case)
  TrSimCluster MakeCluster(double senscoo, double sensangle, int nsens = 0, double DZStep=300); 

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
  // Calculare weight for the implanted-strip model (using the best approximation, very slow) 
  static double GetWeightGaussSum(double x, double width, double sigma, double x1, double x2);

  //! Gauss*Box distribution cumulative function (analytical)
  static double CumulativeGaussBox(double x, double t, double sigma);
  //! Triangular distribution cumulative function (analytical)
  static double CumulativeTriangular(double x, double t);

  ////////////////
  // dE/dx Normalization 
  ////////////////

  //! keV to ADC conversion
  double GetkeVtoADC(float keV);
  //! TB2003: n-side ADC vs Z^2 with no saturation (separately treated)
  static double nside_nosat_charge_dependence_function(double *x, double *par);
  //! TB2003: p-side ADC vs Z^2 with no p-strip correction
  static double pside_uncorr_charge_dependence_function(double *x, double *par);
  //! TB2003: p-side ADC vs Z^2 with p-strip correction
  static double pside_corr_charge_dependence_function(double *x, double *par);

  //! EDep from PDG Bethe-Block (300 um of Si) 
  static double BetheBlock(double z, double betagamma);

};

#endif

