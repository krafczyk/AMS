//  $Id: root.h,v 1.551 2013/04/17 13:40:09 incaglim Exp $
//
//  NB
//  Only stl vectors ,scalars and fixed size arrays
//    are allowed as root file entries
//

//
//  NB Please increase the version number in corr classdef
//  for any class modification
//


#ifndef __AMSROOT__
#define __AMSROOT__
#ifdef _OPENMP
#include <omp.h>
#endif
#include <map>
#include <list>
#include <vector>
#include <iostream>
#include <math.h>
#include "TObject.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TSelector.h"
#include "TROOT.h"
#include "TMVA/Reader.h"
#include <TRandom.h>
#include "TBranch.h"
#include "TMatrixD.h"
#include "TStopwatch.h"
#include <time.h>
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "id.h"
#ifdef _PGTRACK_
#include "trrec.h"
#else
#include "trfit.h"
#endif
#include "trdhrec.h"
#include "TrdHCharge.h"
#include "TrdHCalib.h"
#include "TrdTFit.h"
#include "root_setup.h"
#include "SlowControlDB.h"
#include "Tofdbc.h"
#include "Tofcharge_ihep.h"

#ifdef __AMSVMC__
#include "amsvmc_MCApplication.h"
#include "amsvmc_DetectorConstruction.h"
#include "amsvmc_PrimaryGenerator.h"
#include "amsvmc_MCStack.h"
#include "amsvmc_RootManager.h"
#include "amsvmc_MagField.h"
#include "amsvmc_RunConfiguration.h"
#endif


using namespace std;
#ifdef __SLC3__
char* operator+( std::streampos&, char* );
#endif
#ifndef __ROOTSHAREDLIBRARY__


#ifdef __AMSVMC__
class amsvmc_MCApplication;
class amsvmc_DetectorConstruction;
class amsvmc_PrimaryGenerator;
class amsvmc_MCStack;
class amsvmc_MagField;
class amsvmc_RootManager;
class amsvmc_RunConfiguration;
#endif



#ifndef _PGTRACK_
class AMSTrCluster;
class AMSTrMCCluster;
class AMSTrRawCluster;
class AMSTrTrack;
class AMSTrRecHit;
#endif

class AMSAntiCluster;
class AMSAntiMCCluster;
class AMSBeta;
class TofBetaPar;
class AMSBetaH;
class AMSChargeSubD;
class AMSCharge;
class AMSEvent;
class AMSmceventg;
class AMSmctrack;
class AMSParticle;
class AMSRichMCHit;
class AMSRichRawEvent;
class AMSRichRing;
class AMSRichRingNew;
class AMSNtuple;
class AMSTOFCluster;
class AMSTOFClusterH;
class AMSTOFMCCluster;
class AMSTOFMCPmtHit;
class AMSEcalHit;
class AMSEcalMCHit;
class AMSTRDCluster;
class AMSTRDMCCluster;
class AMSTRDRawHit;
class AMSTRDSegment;
class AMSTRDTrack;
class AMSVtx;
class Ecal1DCluster;
class AMSEcal2DCluster;
class AMSEcalShower;
class TOF2RawCluster;
class TOF2RawSide;
class Anti2RawEvent;
class Trigger2LVL1;
class TriggerLVL302;
class EventNtuple02;
class DAQEvent;
#else
class AMSAntiCluster{};
class AMSAntiMCCluster{};
class AMSBeta{};
class AMSBetaH{};
class AMSCharge{};
class AMSChargeSubD{};
class AMSEvent{};
class AMSEcalHit{};
class AMSEcalMCHit{};
class AMSmceventg{};
class AMSmctrack{};
class AMSParticle{};
class AMSRichMCHit{};
class AMSRichRawEvent{};
class AMSRichRing{};
class AMSRichRingNew{};
class AMSNtuple{};
class AMSTOFCluster{};
class AMSTOFClusterH{};
class AMSTOFMCCluster{};
class AMSTOFMCPmtHit{};
class AMSTRDCluster{};
class AMSTRDMCCluster{};
//class AMSTRDRawHit{};
class AMSTRDSegment{};
class AMSTRDTrack{};



#ifdef __AMSVMC__
class amsvmc_MCApplication{};
class amsvmc_DetectorConstruction{};
class amsvmc_PrimaryGenerator{};
class amsvmc_MCStack{};
class amsvmc_RootManager{};
class amsvmc_RunConfiguration{};
class amsvmc_MagField{};
#endif



#ifndef _PGTRACK_
class AMSTrCluster{};
class AMSTrMCCluster{};
class AMSTrRecHit{};
class AMSTrRawCluster{};
class AMSTrTrack{};
class AMSVtx{};
#endif
class Ecal1DCluster{};
class AMSEcal2DCluster{};
class AMSEcalShower{};
class TOF2RawCluster{};
class TOF2RawSide{};
class Anti2RawEvent{};
class Trigger2LVL1{};
class TriggerLVL302{};
class EventNtuple02{};
class DAQEvent{};
#endif
class AMSEventR;

//!  Root Header class
/*!
  Contains:

  general info   (public)

  shuttle/iss parameters  public)
  (this includes:
   ISS -z axis (LVLH) pointing direction in external reference frame
   AMS z axis (according to 12 degree tilt in roll) in external reference frame
  The reference frames are the equatorial and galactic frames, which are fixed to the sky and not to the Earth. More info in upcoming AMS note)

  object counters, aka arrays dimensions (protected, access via coo functions)
 \sa AMSEventR

 \author vitali.choutko@cern.ch

*/
class HeaderR{
static char _Info[1024];
 public:




// general info block

  unsigned int Run;         ///<run  number
  unsigned int RunType;     ///<runtype(data)
  unsigned int Event;      ///<event number
  int Raw;            ///<raw event length in bytes
  int Version;        ///< os number (low 2 bits) program build number (high 10 bits)
  unsigned int Time[2];        ///<unix time + usec time(data) 
  int RNDMSeed[2];  ///< MC Only RNDM(1)/RNDM(2) seeds


// shuttle/ISS parameters

   float RadS;    ///<iss orbit altitude cm  (GTOD coo sys, prediction from NORAD web site)
   float ThetaS;  ///<theta (GTOD rad,NORAD, accuracy declared 1km)
   float PhiS;    ///<phi   (GTOD rad,NORAD, accuracy declared 1km )
   float Yaw;     ///<ISS yaw (LVLH rad)
   float Pitch;   ///<ISS pitch (LVLH rad)
   float Roll;   ///<ISS roll (LVLH rad)
   float VelocityS;     ///< ISS velocity ( rad/sec)
   float VelTheta;     ///< ISS speed theta (GTOD rad)
   float VelPhi;       ///< ISS speed phi (GTOD rad)
   float ThetaM;        ///< magnetic (calculated for an eccentric dipole coo system) theta  rad
   float PhiM;          ///< magnetic (calculated for an eccentric dipole coo system)phi  rad


 public:
    double Zenith();///< \return angle betwen ams z axis and zenith in degrees
   //--------DSP Errors-----------------------
   /// Access function to DSPError object
   /// \return pointer to DSPError object (if current event time is affected by a DSP error) or 0
   AMSSetupR::DSPError* pDSPError();
   //-----------------------------------------

   vector<unsigned int> GPSTime; ///< see https://twiki.cern.ch/twiki/bin/view/AMS/AMSGPSTimeFormat;

   /*!
    \return 0  if succcess ; 1 if no gps time; 2 wrong time format; 3 int logic error; 4 gps time not valid
    output parameters gps_time_sec, gps_time_nsec of epoche
  */
   int  GetGPSEpoche( unsigned int &gps_time_sec, unsigned int &gps_time_nsec);


   // pointing direction in equatorial and galactic systems
   // removed for the moment

   int   TrStat;     ///< SetofFlags for Problems during Tracking
   //

// counters
protected:
int   EcalHits;
int   EcalClusters;
int   Ecal2DClusters;
int   EcalShowers;
int   RichHits;
int   RichRings;
int   RichRingBs;
int   TofRawClusters;
int   TofRawSides;
int   TofClusters;
int   TofClusterHs;
int   AntiRawSides;
int   AntiClusters;
int   TrRawClusters;
int   TrClusters;
int   TrRecHits;
int   TrTracks;
int   TrdRawHits;
int   TrdClusters;
int   TrdSegments;
int   TrdTracks;
int   TrdHSegments;
int   TrdHTracks;
int   Level1s;
int   Level3s;
int   Betas;
int   BetaBs;
int   BetaHs;
int   Vertexs;
int   Charges;
int   Particles;
int   AntiMCClusters;
int   TrMCClusters;
int   TofMCClusters;
int   TofMCPmtHits;
int   EcalMCHits;
int   TrdMCClusters;
int   RichMCClusters;
int   MCTracks;
int   MCEventgs;
int   DaqEvents;
public:

  /// A constructor.
  HeaderR(){};
#ifndef __ROOTSHAREDLIBRARY__
  void   Set(EventNtuple02 *ptr);
#endif
  friend class AMSEventR;

int getISSSA(float & alpha,float & b1a, float &b3a, float &b1b, float &b3b, float dt=0); ///<get AMSSetupR::ISSSA values for the current event time;

int getISSCTRS(float & r,float & theta, float &phi, float &v, float &vtheta, float &vphi,float dt=0); ///<get AMSSetupR::ISSCTRS values for the current event time;
int getISSGTOD(float & r,float & theta, float &phi, float &v, float &vtheta, float &vphi,float dt=0); ///<get AMSSetupR::ISSGTOD values for the current event time;
int getGPSWGS84(float & r,float & theta, float &phi, float &v, float &vtheta, float &vphi,float dt=0); ///<get AMSSetupR::GPSWGS84 values for the current event time;

int getISSTLE(double dt=0.5);///<update TLE values for the current event time and replace in fHeader;
double TimeCorr(double& error, unsigned int time=0) const;///< Correction to be added to JMDC time to get correct gps time (sec) ; error : error estimation (sec) 
double UTCTime(int mode=0)const ;///< Returns Best Known UTCTime for  curren tevent 
int getISSAtt(float & roll,float & pitch, float &yaw); ///<get AMSSetupR::ISSAtt values for the current event time
int getISSAtt(); ///<get AMSSetupR::ISSAtt values for the current event time and replace roll,pitch,yaw in the fHeader
double getBetaSun();///<get solar beta angle via geometrical calculation
int getSunAMS(double & azimut, double & elevation ); ///<get sun position in AMS coordinate, ret= -1:error, 0:night, 1:day

//--------DSP Errors-----------------------
 int getDSPError(AMSSetupR::DSPError& dsperr); ///< Get AMSSetupR::DSPError (if any) for the current event time. \retval return values from AMSSetupR::getDSPError() \retval 2 in case of not AMSSetupR object in HeaderR
//-----------------------------------------

/// ISS Geomagnetic Latitude [rad]
//
/*! To keep the same convention as HeaderR::ThetaM
 *  the North Magnetic Pole points toward North Earth Pole.
 *  This latutude is calculated with IGRF coefficients   
 */ 
double GetThetaM();

/// ISS Geomagnetic Longitude [rad]
//
/*! To keep the same convention as HeaderR::PhiM
 *  the North Magnetic Pole points toward North Earth Pole.
 *  This longitude is calculated with IGRF coefficients 
 */
double GetPhiM();



//-----------Coordinates -------------------
  int get_gal_coo(double & gal_long, double & gal_lat, double AMSTheta, double AMSPhi, double RPT[3] ,double VelPT[3], double YPR[3], double  time, bool gtod=true, bool gal_coo=true);///< Get galactic coordinates using ISS position, velocity and LVLH attitude
  int get_gal_coo(double & gal_long, double & gal_lat, double AMSTheta, double AMSPhi, int CamID, double CAM_RA, double CAM_DEC, double CAM_Orient, bool gal_coo=true);///< Get galactic coordinates using Star Tracker
  int get_gal_coo(double & gal_long, double & gal_lat, double AMSTheta, double AMSPhi, double YPR[3], double time, bool gal_coo=true);///< Get galactic coordinates using YPR attitude with respect to J2000
int get_gal_coo(double & gal_long, double & gal_lat,  double ams_ra, double ams_dec);///< convert celestial coordinates into galactic coordinates

//-----------Backtracing -------------------
//! Do Backtracing
/*!
 Get galactic coordinates with backtracing using ISS position, velocity and LVLH attitude
\param
   
input   
          AMSTheta(rad)  in ams coo system
          AMSPhi  (rad)  in ams coo system
	  momentum  (not signed gev/c)
          velocity  (not signed v/speed_of_light)
          charge    (signed, integer)
	  RPT[3]    ISS coordinates (Rs,PhiS,ThetaS)
          VelPT[3]  ISS velocity (Vel rad/sec,VelPhi rad,VelTheta rad)
	  YPR[3]    ISS attitude (Yaw,Pitch,Roll)
	  xtime     time
	  att       1:GTOD, 2:CTRS, 3:INTL
	  galactic  Galactic or Celestial coordinates
output
          gal_long   Galctic longigude (deg)
          gal_lat    Galctic latitude  (deg)
	  time_trace time of flight in sec
          RPTO[3]    Particle final GTOD coordinate (R,Phi,Theta)
	  GPT[2]     Particle final GTOD direction  (Phi,Theta)
 \return 0 unercutoff i.e. particle is likely of atmoepheric origin ;
         1 overcutoff i.e. particle is likely coming from space ;
	 2 trapped ;
        -1 error
*/
int do_backtracing(double & gal_long, double & gal_lat, double & time_trace, double RPTO[3], double GPT[2], double AMSTheta, double AMSPhi, double momentum, double velocity, int charge, double RPT[3], double VelPT[3], double YPR[3], double  xtime, int att=1, bool galactic=true);

//------------------------------------------------------
//! Function used in AMSEventR::DoBacktracing
/*! Performes particle backtracing with external field models of Tsyganenko (T96 and T05)
 *  (see:  bt_method flag) 
 *
input   
          AMSTheta(rad)  in ams coo system
          AMSPhi  (rad)  in ams coo system
          momentum  (not signed gev/c)
          charge    (signed, integer)
          RPT[3]    ISS coordinates (Rs,PhiS,ThetaS)
          VPT[3]    ISS velocity (Vel rad/sec,VelPhi rad,VelTheta rad)
          YPR[3]    ISS attitude (Yaw,Pitch,Roll)
          xtime     particle time used according to use_time of AMSEventR::DoBacktracing 
          att       1:GTOD, 2:CTRS, 3:INTL
          gal_coo   if true gal_long and gal_lat will be in Galactic reference frame, lese in Equatorial one
          bt_method flag for external magnetic field (1 just internal, 2 Tsyganenko 96, 3 Tsyganenko 05)
          Amass     atomic mass number  (0: for electrons and positrons  :1 for p anti-p  ...)
output
          gal_long   Galctic longigude (deg)
          gal_lat    Galctic latitude  (deg)
          time_trace backtracing time of flight in sec
          RPTO[3]    Particle final GTOD coordinate (R [cm] ,Phi [rad], Theta [rad])
          GPT[2]     Particle final GTOD direction  (Phi[rad],Theta[rad])

\return  0 secondary particle (unercutoff i.e. particle is likely of atmospheric origin) ;
         1 primary particle   (overcutoff i.e. particle is likely coming from space) ;
         2 time limit (set at 10 seconds) ;
         3 velocity greater than c
         4 External field model parameters out of validity range
         5 External field model parameters do not exist
         6 all other cases where the program was exiting (max number of steps reached..)
        -1 failure (Charge==0 or if bt_method!=0  prameters file not found) 
        -2 error   (if bt_method==2  particle UTime out of time limits for parameter file T96)
        -3 error   (if bt_method==3  particle UTime out of time limits for parameter file T05)
        -4 error   bt_method must be 1 || 2 || 3
         
*/
int backtracing_mib(double &gal_long, double &gal_lat, double &time_trace, double RPTO[3],double GPT[2] ,double AMSTheta, double AMSPhi, double momentum,int charge, double RPT[3],double VPT[3], double YPR[3],double xtime ,int att=1, bool gal_coo=true, int bt_method=1,int Amass=0);


//-----------------------------------------
int get_gtod_coo(double & gtod_long, double & gtod_lat, double AMSTheta, double AMSPhi, double RPT[3] ,double VelPT[3], double YPR[3], double  time, bool gtod=true);///< Get gtod coordinates using ISS position, velocity and LVLH attitude



  //#ifdef _PGTRACK_
  friend class VCon_root;
  //#endif
   /*!
    \return human readable info about HeaderR
  */
  char * Info(unsigned long long status);

  virtual ~HeaderR(){};
  ClassDef(HeaderR,22)       //HeaderR
//#pragma omp threadprivate(fgIsA)
};



//!  DAQ Parameters Structure
/*!

   \author vitali.choutko@cern.ch
*/
class DaqEventR {
static char _Info[255];
public:
  unsigned int Length;    ///<Event length in bytes
  unsigned int Tdr;  ///< Tracker  length in bytes
  unsigned int Udr;  ///< TRD  length in bytes
  unsigned int Sdr;  ///< TOF+Anti  length in bytes
  unsigned int Rdr;  ///< RICH  length in bytes
  unsigned int Edr;  ///< Ecal length in bytes
  unsigned int L1dr;  ///< Lvl1  length in bytes
  unsigned int L3dr;  ///< Lvl3  info two short integers (lvl3[1]<<16 | lvl3[0])
/*!
  bit 63 proc error
  bit 62 desync error
  bit 61 event error
  byte 6  number of TDR  with fatal errors
  byte 5  number of UDR  with fatal errors
  byte 4  number of RDR  with fatal errors
  byte 3  number of EDR  with fatal errors
  byte 2  number of SDR  with fatal errors
  byte 1  number of LV1  with fatal errors
  byte 0  number of ETRG  with fatal errors
*/
  unsigned long long  L3Error;  ///<Lvl3 event error
  unsigned int  L3VEvent; ///<Lvl3 event counter + Version
  unsigned int  L3TimeD; ///< lvl3 event time diffence in 0.64 usec counts;
  unsigned short int JINJStatus[4];///< JINJ-[0,1,2,3] status word 
  unsigned char JError[24]; ///< higher 8 bit of corresponding slave in jinj block
  unsigned int L3Version() const {return (L3VEvent>>24)&255;};
  unsigned int L3Event()const {return (L3VEvent&16777215);}
  unsigned int L3NodeError(int i); ///< Return number of nodes with errors for (ETRG,LV1,SDR,EDR,RDR,UDR,TDR 0...6
  bool L3ProcError(){return (L3Error & (1ULL<<63))?true:false;}
  bool L3RunError(){return (L3Error & (1ULL<<62))?true:false;}
  bool L3EventError(){return (L3Error & (1ULL<<61))?true:false;}
  DaqEventR(DAQEvent *ptr);
  DaqEventR(){};
  virtual ~DaqEventR(){};

  /// Checks the error bits (0x7F00)c on the four JINJStatus word and return True if any of these bit is up
  bool HasHWError(){
   return ( (JINJStatus[0]&0x7F00)>0 ||
            (JINJStatus[1]&0x7F00)>0 ||
            (JINJStatus[2]&0x7F00)>0 ||
            (JINJStatus[3]&0x7F00)>0
	  );
  }
  /// \return human readable info about DaqEventR
  char * Info(int number=-1);
ClassDef(DaqEventR,8)       //DaqEventR
#pragma omp threadprivate(fgIsA)
};



//!  Ecal Hits Structure
/*!

   \author e.choumilov@cern.ch
*/
class EcalHitR {
public:
  unsigned int   Status;   ///< Statusword
  short   Idsoft;   ///< 4digits number SPPC=SuperLayer/PM/subCell  1:9/1:36/1:4
               /*!<
Idsoft SubCells(pixels) numbering(+Z is top):\n
---------------|0|1|------------------\n
pm1(at -x/y)>> ------>> pm36(at +x/y) \n
---------------|2|3|------------------\n
                */
  short   Proj;     ///< projection (0-x,1-y)
  short   Plane;    ///< ECAL plane number (0,...17)
  short   Cell;     ///< ECAL Cell number (0,...71)
  float Edep;     ///< ECAL measured energy (MeV, includes all corrections)
  float EdCorr;   ///< ECAL PMsaturation1-correction(MeV) added to Edep
  float AttCor;   ///<  Attenuation Correction applied (w/r  to center of ecal) (MeV)
  float Coo[3];   ///< ECAL Coo (cm)
  float ADC[3];   ///< ECAL (ADC-Ped) for Hi/Low/Dynode channels
  float Gain;     ///<  1/gain (!)

  /// ECAL Ped for current run and given layer, cell, channel (Hi/Low/Dynode)
  /// \author basara@lapp.in2p3.fr
  /// \param layer between 0 and 17 (including for dynode)
  /// \param cell between 0 and 71 (including for dynode)
  /// \param channel between 0 and 2 for Hi/Low/Dynode
  /// \return -1 if bad parameters, -2 if SlowControl error, value for Pedestal otherwise
  static float GetECALPed(int layer, int cell, int channel=0);

  /// ECAL RMS for current run and given layer, cell, channel (Hi/Low/Dynode)
  /// \author basara@lapp.in2p3.fr
  /// \param layer between 0 and 17 (including for dynode)
  /// \param cell between 0 and 71 (including for dynode)
  /// \param channel between 0 and 2 for Hi/Low/Dynode
  /// \return -1 if bad parameters, -2 if SlowControl error, value for RMS otherwise
 static float GetECALRms(int layer, int cell, int channel=0);

  EcalHitR(AMSEcalHit *ptr);
  EcalHitR(){};
  virtual ~EcalHitR(){};
ClassDef(EcalHitR,5)       //EcalHitR
#pragma omp threadprivate(fgIsA)
};

/// EcalCluster structure
/*!

   \author vitali.choutko@cern.ch
*/
class EcalClusterR {
static char _Info[255];
public:
  unsigned int Status;    ///<  Statusword
                 /*!< valid values are:\n
           widecluster >4 hits                       WIDE=2   \n
           no signal or noise                        BAD =16  \n
           used in shower                            USED=32  \n
           next cluster just nearby                  NEAR=102 \n
           side leak detected and corrected          LEAK=16777216  \n
           side leak detected and not corrected      CATASTROPHICLEAK=33554432 \n
            isolated clusters below threshold - will not be used for shower                                          JUNK=67108864 \n
                */

  int Proj;   ///< projection (0x, 1y)
  int Plane;  ///< layer no [0:17]  0-top
  int Left;   ///< cluster leftmost hit pos [0:72]
  int Center; ///< Cluster Max Energy Hit pos
  int Right;  ///< cluster rightmost hit pos [0:72]
  float Edep; ///< energy deposition (mev)
  float SideLeak;  ///< side leak estimation (mev)
  float DeadLeak;  ///<  dead hit leak est (mev)
  float Coo[3];    ///< cluster coo (cm)
protected:
  vector <int> fEcalHit;   ///< pointers to EcalHitR collection, protected
public:
  /// access function to EcalHitR objects used
  /// \return number of EcalHitR used
  int NEcalHit()const {return fEcalHit.size();}
  /// access function to EcalHitR objects used
  /// \param i index of fEcalHit vector
  /// \return index of EcalHitR object in collection or -1
  int iEcalHit(unsigned int i){return i<fEcalHit.size()?fEcalHit[i]:-1;}
  /// access function to EcalHitR collection
  /// \param i index of fEcalHit vector
  /// \return pointer to EcalHitR object or 0
  EcalHitR * pEcalHit(unsigned int i);

  EcalClusterR(){};
  EcalClusterR(Ecal1DCluster *ptr);
  friend class Ecal1DCluster;
  friend class AMSEventR;
  /// \param number index in container
  /// \return human readable info about EcalClusterR
  char * Info(int number=-1){
    sprintf(_Info,"EcalCluster No %d Layer=%d Proj=%d Coo=(%5.2f,%5.2f,%5.2f) E_{Dep}(MeV)=%5.2f Multip=%d",number,Plane,Proj,Coo[0],Coo[1],Coo[2],Edep,NEcalHit());
  return _Info;
  }
  virtual ~EcalClusterR(){};
ClassDef(EcalClusterR,1)       //EcalClusterR
#pragma omp threadprivate(fgIsA)
};


//!  Ecal2DClusterR Structure

/*!

   \author vitali.choutko@cern.ch
*/

class Ecal2DClusterR{
public:
  unsigned int Status;   ///< statusword 
                 /*!< valid values are:\n
           used in shower                            USED=32  \n
                */
  int Proj;     ///< projection 0-x 1-y
  float Edep;   ///< energy (mev)
  float Coo;    ///< str line fit coo (cm)
  float Tan;    ///< str line fit tangent
  float Chi2;   ///< str line fit chi2

protected:
  vector <int> fEcalCluster;   ///< indexes of EcalClsterR collection
public:
  /// access function to EcalClusterR objects used
  /// \return number of EcalClusterR used
  int NEcalCluster()const {return fEcalCluster.size();}
  /// access function to EcalClusterR collection
  /// \param i index of fEcalCluster vector
  /// \return index of EcalClusterR collection or -1
  int iEcalCluster(unsigned int i){return i<fEcalCluster.size()?fEcalCluster[i]:-1;}
  /// access function to EcalClusterR collection
  /// \param i index of fEcalCluster vector
  /// \return pointer to EcalClusterR collection or 0
  EcalClusterR * pEcalCluster(unsigned int i);

  Ecal2DClusterR(){};
  Ecal2DClusterR(AMSEcal2DCluster *ptr);
  virtual ~Ecal2DClusterR(){};
ClassDef(Ecal2DClusterR,1)       //Ecal2DClusterR
#pragma omp threadprivate(fgIsA)
friend class AMSEcal2DCluster;
friend class AMSEventR;
};


/*!
  \class EcalShowerR
  \brief 3d ecal shower description

   \sa ecalrec.h ecalrec.C
   \author vitali.choutko@cern.ch
*/


class EcalShowerR {
static char _Info[255];
public:
  unsigned int   Status;    ///< status word \sa EcalHitR status
                 /*!< valid values are:\n
           used in particle                            USED=32  \n
           energy estimatation not reliable      CATASTROPHICLEAK=33554432 \n
                */
  ///
  float Dir[3];    ///< direction cos array
  float EMDir[3];  ///< direction cos array for emag type shower
  float Entry[3];  ///< entry point (cm)
  float Exit[3];   ///< exit point(cm)
  float CofG[3];  ///< center of gravity (cm)
  float ErDir;    ///< 3d angle error
  float ErDirE;   ///< 3d angle error - Pisa version
  float Chi2Dir;  ///<  chi2 direction fit
  float FirstLayerEdep; ///< front energy dep (Mev)
  float EnergyD; ///< energy deposit (Mev)
  float EnergyC; ///< shower energy (gev) - old version
  float EnergyE; ///< shower reconstructed energy (GeV)
  float Energy3C[3]; ///< energy(+-3,+-5, +-1 cm)/energy ratios
  float  S13R;        ///< S1/S3 Ratio
  float  S13Rpi[2];  ///< S1/S3 Ratio for X and Y view
  float ErEnergyC;   ///< energy error (gev) - old version
  float ErEnergyE;   ///< energy error (gev) - Pisa version
  float DifoSum;     ///<  (E_x-E_y)/(E_x+E_y)
  float SideLeak;    ///< rel side leak
  float RearLeak;    ///< rel rear leak
  float DeadLeak;    ///< rel dead leak
  float AttLeak;     ///< rel att length correction
  float SideLeakPI;    ///< rel side leak - Pisa version
  float RearLeakPI;    ///< rel rear leak - Pisa version
  float DeadLeakPI;    ///< rel dead leak - Pisa version
  float AttLeakPI;     ///< rel att length correction - Pisa version

  float NLinLeak;   ///<  rel non-lin correction - Pisa version
  float NLinLeakPI;  ///<  rel non-lin correction
  float S13Leak;   ///<  s1/s3 ratio correction;
  float S13LeakXPI;  ///< s1/s3 ratio correction for X view
  float S13LeakYPI;  ///< s1/s3 ratio correction for Y view
  float OrpLeak;   ///<  fraction of shower energy outside core.
  float OrpLeakPI;  ///<  fraction of shower energy outside core - Pisa version.

//LAPP (MP) begin
  float Energy0A[2]; ///< LAPP shower energy x, y only impact point correction applied, no fiber cross-talk (Mev)
  float EnergyA; ///< LAPP shower energy (Gev)
  float ErEnergyA;   ///< LAPP energy error (Gev)
  float SideLeakA;    ///< LAPP rel side leak
  float RearLeakA;    ///< LAPP rel rear leak
  float DeadLeakA;    ///< LAPP rel dead leak
  float AttLeakA;     ///< LAPP rel att length correction
  float NLinLeakA;   ///<  LAPP rel non-lin correction
  float S13LeakXA0;  ///< LAPP  impact-point correction  applied to energy deposited in X view
  float S13LeakYA0;  ///< LAPP  impact-point correction applied to energy deposited in Y view
  float S13LeakXA;  ///< LAPP impact-point x hit multiplicity correction  applied to energy deposited in X view
  float S13LeakYA;  ///< LAPP  impact-point x hit multiplicity correction applied to energy deposited in Y view
  float OrpLeakA;   ///<  LAPP fraction of shower energy outside core.
  float S35LeakXA;  ///< LAPP impact-point correction applied to S3/S5 Ratio X view 
  float S35LeakYA;  ///< LAPP  impact-point correction applied to S3/S5 Ratio Y view
  float VarLeakXA;  ///< LAPP impact-point correction applied to ShowerLatDisp X view 
  float VarLeakYA;  ///< LAPP  impact-point correction applied to ShowerLatDisp Y view
  float S13Ra[2];  ///< LAPP S1/S3 Ratio for X and Y view
  float S35Ra[2];  ///< LAPP S3/S5 Ratio for X and Y view
  float S1tot[3];  ///< Energy CofG cell / energy ratio: 0=X; 1=Y ; 2=X+Y 
  float S3tot[3];  ///< Energy CofG+-1 cells / energy ratio: 0=X ; 1=Y ; 2=X+Y 
  float S5tot[3];  ///< Energy CofG+-2 cells / energy ratio: 0=X ; 1=Y ; 2=X+Y
// LAPP (MP) end

  float Orp2DEnergy; ///< orphaned Ecal2DClusterR energy (if any) (geV)
  float Chi2Profile;  ///< chi2 profile fit (by gamma function)
  float Chi2ProfileBack; ///< chi2 profile fit in back direction
  float ParProfile[4]; ///< normalization, shower max (cm), rad length, rel rear leak ) for profile fit
  float Chi2Trans;     ///< chi2 transverse fit (sum of two exp)
  float SphericityEV[3]; ///< sphericity tensor eigen values
  int Nhits;

protected:
  vector <int> fEcal2DCluster;  ///< indexes to Ecal2DClusterR collection
public:

// LAPP Variables
  int NbLayerX;
  int NbLayerY;
  float ShowerLatDisp[3];  ///< Shower Lateral Dispersion: 0=X - 1=Y - 2=X+Y
  float ShowerLongDisp;  ///< Shower Longitudinal Dispersion
  float ShowerDepth;  ///< Shower Depth
  float ShowerFootprint[3]; ///< Shower Footprint - 0=X - 1=Y - 2=X+Y
  float ZprofileChi2; ///< Chi2 of Zprofile
  float Zprofile[4]; ///< Parameter of the Shower Z profile - 0=Normalised integral divided by energy 1=Z at fit max value 2=Reference Zmax 3=ratio b/Xo
  float ZprofileChi2v2; ///< Chi2 of Zprofile version 2
  float Zprofilev2[4]; ///< Parameter of the Shower Z profile version 2 - 0=Normalised integral divided by energy 1=Z at fit max value 2=Reference Zmax 3=Primary interaction shift
  float EnergyF; ///< Energy obtained by the Gamma function fit 
  float EnergyFractionLayer[18];  ///< Energy fraction per layer

// LAPP Variables Normalised (Filled only if NormaliseVariableLAPP() or EcalStandaloneEstimator() functions are called)
  float NS1tot[3]; ///< Normalised Energy max fired cell / energy ratio - 0=X - 1=Y - 2=X+Y
  float NS3tot[3]; ///< Normalised Energy 3 max fired cells / energy ratio - 0=X - 1=Y - 2=X+Y
  float NS5tot[3]; ///< Normalised Energy 5 max fired cells / energy ratio - 0=X - 1=Y - 2=X+Y
  float NShowerLatDisp[3]; ///< Normalised Shower Lateral Dispersion - 0=X - 1=Y - 2=X+Y
  float NShowerLongDisp; ///< Normalised Shower Longitudinal Dispersion
  float NShowerFootprint[3]; ///< Normalised Shower Footprint - 0=X - 1=Y - 2=X+Y
  float NZprofile[4];  ///< Normalised Parameter of the Shower Z profile - 0=Normalised integral divided by energy 1=Z at fit max value 2=Reference Zmax 3=
  float NZprofileChi2; ///< Normalised Chi2 of Zprofile
  float NZProfileMaxRatio; ///< Ratio between profile fitted and expected Zmax
  float NEnergyFractionLayer[18];  ///< Normalised energy fraction per layer
  float NS1S3[3];
  float NS3S5[3];

// Other normalised variables
  float NS13R;
  float NEnergy3C2;
  float NEnergy3C3;

  /// Function to obtain the best reconstructed energy for the shower according to a particle hyptothesis: partid=1(photon),2=(electron/positron). Using different methods to recover anode efficiency, rear leakage, lateral leakage and temperature effects. It's till dummy for the moment: just retrieves EnergyC (method=0), EnergyA (method=1) or EnergyE (method=2)
  float GetCorrectedEnergy(int partid=2,int method=2);

  void NormaliseVariableLAPP();
  ///< LAPP function to normalise several variables
  ///< Remove the energy dependence (Mean and RMS deduced from electron selection May19->August31)
  ///< Fill variables Nxxx
  ///< Is automatically called if the LAPP estimator function is called ( EcalStandaloneEstimator() )

  float EcalStandaloneEstimator();
  ///< LAPP Ecal Estimator V2 -
  ///< Updated December 2011 -
  ///< 90% efficiency cut (based on electron-like flight events) is -1.49.
  ///< Relies on normalised variables (Nxxx) -
  float EcalStandaloneEstimatorV2();
  ///< LAPP Ecal Estimator -
  ///< Updated June 2012 -
  ///< 90% efficiency cut (based on electron-like flight events) is -1.23
  ///< Relies on normalised variables (Nxxx) -
  ///< Due to a bug in the version of TMVA used in the current AMS software, it is not possible to 
  ///< call in the same program EcalShowerR::GetEcalBDT() and EcalShowerR::EcalStandaloneEstimator() or EcalShowerR::EcalStandaloneEstimatorV2()

 float EcalStandaloneEstimatorV3();
  ///< LAPP Ecal Estimator V3
  ///< Updated Jan 2013
  ///< Efficiency bin-dependant ;matching efficiency / rejection retrieved via GetESEv3Efficiency() / GetESEv3Rejection()
  ///< for an Energy-Bin independant cut, you should use EcalShowerR::EcalStandaloneEstimatorV2()
  
 float GetESEv3Efficiency();
  ///< Get the lepton efficiency for the current shower using LAPP Ecal Estimator V3 value and energy bin
  
 float GetESEv3Rejection();
  ///< Get the proton rejection for the current shower using LAPP Ecal Estimator V3 value and energy bin
  
  
 
  /// Pisa function to obtain the Boosted Decision Tree Classifier for shower.
  /// It has three arguments: a pointer to the event (AMSEventR *pev), a version number and a Classifier number.
  /// Version number:
  /// iBDTVERSION=3           trained with B552 \ *pev is not required
  /// iBDTVERSION=4           trained with B610 /      for these versions
  /// iBDTVERSION=5 (default) trained with B620 -> NEEDS *pev as input
  /// TMVA Classifier:
  /// TMVAClassifier=0 (default) standard BDTG
  /// TMVAClassifier=1           BDT with smoothing
  float GetEcalBDT();
  float GetEcalBDT(unsigned int iBDTVERSION);
  float GetEcalBDT(unsigned int iBDTVERSION, int TMVAClassifier);
  float GetEcalBDT(AMSEventR *pev, unsigned int iBDTVERSION);
  float GetEcalBDT(AMSEventR *pev, unsigned int iBDTVERSION, int TMVAClassifier=0);
  /// Function that combines GetEcalBDT and GetChi2 .
  /// It has three arguments: a pointer to the event (AMSEventR *pev), a version number and a Classifier number.
  /// Version number:
  /// iBDTCHI2VERSION = 2           trained with B620 ecalaxis->process(pev,2)
  /// iBDTCHI2VERSION = 3 (default) trained with B620 ecalaxis->process(pev,8) (cell center found with Neighbour Cell algorithm; much faster than process(pev,2)); 
  /// TMVA Classifier:
  /// TMVAClassifier=0 (default) standard BDTG
  /// TMVAClassifier=1           BDT with smoothing
  float GetEcalBDTCHI2();
  float GetEcalBDTCHI2(unsigned int iBDTCHI2VERSION);
  float GetEcalBDTCHI2(unsigned int iBDTCHI2VERSION, int TMVAClassifier);
  float GetEcalBDTCHI2(AMSEventR *pev, unsigned int iBDTCHI2VERSION);
  float GetEcalBDTCHI2(AMSEventR *pev, unsigned int iBDTCHI2VERSION, int TMVAClassifier=0);
  /// access function to Ecal2DClusterR objects used
  /// \return number of Ecal2DClusterR used
  int NEcal2DCluster()const {return fEcal2DCluster.size();}
  /// access function to Ecal2DClusterR objects
  /// \param i index of fEcal2DCluster vector
  /// \return index of Ecal2DClusterR object in collection or -1
  int iEcal2DCluster(unsigned int i){return i<fEcal2DCluster.size()?fEcal2DCluster[i]:-1;}
  /// access function to Ecal2DClusterR objects
  /// \param i index of fEcal2DCluster vector
  /// \return pointer to Ecal2DClusterR object  or 0
  Ecal2DClusterR * pEcal2DCluster(unsigned int i);
  /// \param number index in container
  /// \return human readable info about EcalShowerR
  char * Info(int number=-1){
    sprintf(_Info,"EcalShower No %d NHits %d Energy=%7.3g#pm%5.2g  #theta=%4.2f #phi=%4.2f Coo=(%5.2f,%5.2f,%5.2f) #chi^{2}=%7.3g Asymm=%4.2f Leak_{Side,Rear,Dead,Att,NonLin,S13,Orp}=(%4.2f,%4.2f,%4.2f,%4.2f,%4.2f,%4.2f,%4.2f) Max=%4.2f",number,Nhits, EnergyC,ErEnergyC,acos(Dir[2]),atan2(Dir[1],Dir[0]),CofG[0],CofG[1],CofG[2],Chi2Dir,DifoSum,SideLeak,RearLeak,DeadLeak,AttLeak,NLinLeak,S13Leak,Orp2DEnergy/(EnergyC+1e-10),ParProfile[1]);
  return _Info;
  }


  EcalShowerR(){};
  EcalShowerR(AMSEcalShower *ptr);
  friend class AMSEcalShower;
  friend class AMSEventR;

  virtual ~EcalShowerR(){};
  ClassDef(EcalShowerR,15)       //EcalShowerR
#pragma omp threadprivate(fgIsA)

};


/// TofRawSideR structure

/*!
 \author e.choumilov@cern.ch

*/

class TofRawSideR {
public:
  int swid;///<short softw-id(LBBS)
  int hwidt;///<time hardw-id CSIIII(Crate|SSlot|each I(1-9) is TDC InpChNumb for LT,FT,SumHT,SumSHT measurements
  int hwidq[4];///<array of CSII, where II is ADC InpChNumb for Anode and Dynodes charge-measurements(II=1-10)
  int stat;///< 0/1->ok/bad
  int nftdc;///<numb.of FTtime-hits
  vector<int> fftdc;///<FTtime-hits(FastTrigger-channel time, TDC-ch)
  int nstdc;///<numb.of LTtime-hits
  vector<int> fstdc;///<LTtime-hits(LowThreshold-channel time, TDC-ch)
  int nsumh;///<numb.of SumHTtime-hits
  vector<int> fsumht;///<SumHTtime-hits(half_plane_sum of HighThreshold-channel time history, TDC-ch)
  int nsumsh;///<numb.of SumSHTtime-hits
  vector<int> fsumsht;///<SumSHTtime-hits(half_plane_sum of SuperHighThreshold-channel time history, TDC-ch)
  float adca;///<Anode signal(ADC-counts, ped-subtracted if not PedCal-run)
  int nadcd;///<number of Dynode nonzero(!) signals
  float adcd[3];///<Dynode signals(ADC-counts, positional(keep "0"s), ped-subtracted if not PedCal-run)
  float temp;///<Anode-readout temperature (given by probe in SFET(A) slots)
  float tempC;///<Dynode-readout temperature (given by probe in SFEC card)
  float tempP;///<PM-temperature (averaged over PMTs of 1side, based on probes in TOP-plane envelop)

  TofRawSideR(){};
  TofRawSideR(TOF2RawSide *ptr);
  friend class TOF2RawSide;
  friend class AMSEventR;
  virtual ~TofRawSideR(){};
  int getftdc(int i){return i<fftdc.size()?fftdc[i]:0;}
  int getstdc(int i){return i<fstdc.size()?fstdc[i]:0;}
  int getsumht(int i){return i<fsumht.size()?fsumht[i]:0;}
  int getsumsht(int i){return i<fsumsht.size()?fsumsht[i]:0;}

  ClassDef(TofRawSideR ,6)       //TofRawSideR
#pragma omp threadprivate(fgIsA)
};





/// TofRawClusterR structure

/*!
 \author e.choumilov@cern.ch

*/

class TofRawClusterR {
public:
  unsigned int   Status;  ///< statusword
                /*!<
                                              // bit8(128)  -> bad time-history on any side
                                              // bit9(256)  -> "1-sided" counter(1-side meas. is missing)
                                              // bit10(512) -> ignore time-measurement(as known from DB),not used
					      // bit11(1024)-> missing side number(0->s1,1->s2)
                                              // bit12(2048)-> recovered from 1-sided (bit256 also set)
                                              // bit13(4096)-> no bestLT/sumHT-hit matching(when requested) on any side
					      // bit6(32)   -> used for TofCluster
   */
  int   Layer;   ///< Tof plane 1(top)...4
  int   Bar;     ///< Tof Bar number 1...10
  float adca[2]; ///< Anode raw signal(adc), side=1-2
  float adcd[2]; ///< Dynode(equilized sum of pmts on each side) raw signal(adc)
  float adcdr[2][3]; ///< Dynode(pm=1-3) raw signals(adc)
  float sdtm[2];  ///< A-noncorrected side times
  float edepa;   ///< Anode Edep (mev)
  float edepd;   ///< Dynode Edep (mev, in Anode scale)
  float time;    ///< Time (ns)
  float cool;     ///< Long.coord.(cm)
protected:
  vector<int> fTofRawSide;   //indexes of TofRawSideR's current object is made of.
public:
  /// access function to TofRawSideR objects used
  /// \return number of TofRawSideR used
  int NTofRawSide()const {return fTofRawSide.size();}
  /// access function to TofRawSideR    used
  /// \param i index of fTofRawSide vector
  /// \return index of TofRawSideR object in TofRawSide collection or -1
  int iTofRawSide(unsigned int i){return i<fTofRawSide.size()?fTofRawSide[i]:-1;}
  /// access function to TofRawSideR's   current object is made of.
  /// \param i index of fTofRawSide vector
  /// \return pointer to TofRawSideR object or 0
  TofRawSideR * pTofRawSide(unsigned int i);

  TofRawClusterR(){};
  TofRawClusterR(TOF2RawCluster *ptr);

  friend class TOF2RawCluster;
  friend class AMSEventR;

  virtual ~TofRawClusterR(){};
  ClassDef(TofRawClusterR ,5)       //TofRawClusterR
#pragma omp threadprivate(fgIsA)
};



/// AntiRawSideR structure

/*!
 \author e.choumilov@cern.ch

*/

class AntiRawSideR {
public:
  int swid;///< BBS (Bar/Side)
  int stat;///< status (0/1/... -> alive/dead/...)
  float temp;///< board temperature (SFEA-sensor reading)
  float adca;///< anode pulse-charge hit(ADC-counts, ped-subtracted if not PedCalibJob)
  int nftdc;///< number of FastTrig(FT) hits (normally =1)
  int ftdc[8];///< FT-hits(tdc-chan),1/4-plane common, but stored for each sector
  int ntdct;///< number of Time(LT-chan) hits
  int tdct[16];///< Time hits

  AntiRawSideR(){};
  AntiRawSideR(Anti2RawEvent *ptr);
  friend class Anti2RawEvent;
  friend class AMSEventR;
  virtual ~AntiRawSideR(){};

  ClassDef(AntiRawSideR ,1)       //AntiRawSideR
#pragma omp threadprivate(fgIsA)
};




/// TofClusterR structure
/*!
 \author e.choumilov@cern.ch

*/

class TofClusterR {
protected:
static char _Info[255];
public:
  unsigned int Status;   ///< Statusword(as for RawCluster + ones below:
                /*!<
                                                 // bit3(4)  -> ambig
						 // bit5(16) -> BAD((bit9 | bit10) & !bit12))
						 // bit8 is ORed over cluster-members
   */
  int Layer;    ///<  Tof plane 1(top)...4
  int Bar;      ///< Tof Bar number 1...10
  float Edep;  ///< TOF energy loss (MeV, anode + dynode combined)
  float Edepd; ///< TOF energy loss (MeV) from dinode
  float Time;  ///<TOF time (sec, wrt FastTrig-time)
  float ErrTime;  ///< Error in TOF time
  float Coo[3];   ///< TOF Coo (cm)
  float ErrorCoo[3];   ///< Error TOF Coo (cm)
protected:
  vector<int> fTofRawCluster;   //indexes of TofRawclusterR's current object is made of.
public:
  /// access function to TofRawClusterR objects used
  /// \return number of TofRawClusterR used
  int NTofRawCluster()const {return fTofRawCluster.size();}
  /// access function to TofRawClusterR    used
  /// \param i index of fTofRawCluster vector
  /// \return index of TofRawClusterR object in TofRawCluster collection or -1
  int iTofRawCluster(unsigned int i){return i<fTofRawCluster.size()?fTofRawCluster[i]:-1;}
  /// access function to TofRawClusterR's   current object is made of.
  /// \param i index of fTofRawCluster vector
  /// \return pointer to TofRawClusterR object or 0
  TofRawClusterR * pTofRawCluster(unsigned int i);

  /// \param number index in container
  /// \return human readable info about TofClusterR
  char * Info(int number=-1){
    sprintf(_Info,"ToF Cluster No %d S%dB%d: time=%3.1f#pm%3.1f, E_{Dep}(MeV)=%4.1f, at (%5.1f,%5.1f,%5.1f)#pm(%5.1f,%5.1f,%5.1f)",number,Layer,Bar,Time*1.e9,ErrTime*1.e9,Edep,Coo[0],Coo[1],Coo[2],ErrorCoo[0],ErrorCoo[1],ErrorCoo[2]);
  return _Info;
  }
  TofClusterR(){};
  TofClusterR(AMSTOFCluster *ptr );
  friend class AMSTOFCluster;
  friend class AMSEventR;
  virtual ~TofClusterR(){};
ClassDef(TofClusterR,1)       //TofClusterR
#pragma omp threadprivate(fgIsA)
};

/// TofClusterHR structure
/*! 
 *  IHEP TOF Recontruction: TofClusterH is rectructed from TofRawSide, which contains new independent calibration of each fired TOF Counter.
 *  IHEP New TOF Beta Measument BetaH: TofRawSide->TofClusterH(only 1 Fired TOF Counter)->BetaH(New Calibration+Software)
 *  \author qyan@cern.ch
 */
#include "TrElem.h"
class TofClusterHR :public TrElem {
   static char _Info[255];
 public:
   /// TOF Counter Bad tag  
   unsigned int Status;
   ///  TOF Counter 2 RawSdie Bit Pattern
   unsigned int SideBitPat[2];
   /// TOF Counter Pattern
   int   Pattern;
   /// TOF Counter Layer Number 0-3
   int   Layer;
   /// TOF Counter Bar   Number 0-9
   int   Bar;
   /// Raw Anode ADC Side  0-1
   float Aadc[2];
   /// Raw Dynode ADC Side 0-1
   float Dadc[2][3];
   /// Non Correct Time(ns)(FT-LT) Side 0-1
   float Rtime[2];
   /// Correct Time(ns) (FT-LT) Side 0-1 (LT Real Time -Stime)
   float Stime[2];
   /// Counter Hit Time(ns)(After Caliberation LT-FT+Const)
   float Time;
   /// Counter Hit Time Error(ns)
   float ETime;
   /// Counter Fired Time Estimate Coo(cm)
   float Coo[3];
   /// Counter Fired Time Estimate Coo Error(cm) 
   float ECoo[3];
   /// Anode Estimate Energy dep(MeV)
   float AEdep;
   /// Dynode Estimate Energy dep(MeV)
   float DEdep;
   /// Anode Q^2 Estimate for 2Sides PMTs (NonL+AttCor No-Birk-Cor) 
   float AQ2[2];
   /// Dynode Q^2 Estimate for 2Sides 2~3PMTs(NonL+AttCor No-Birk-Cor)
   float DQ2[2][3];
   /// Q Par
   map<int,float> QPar;

 protected:
    vector<int> fTofRawSide;

 public:
     static int DefaultQOpt;
     static int DefaultQ2Opt;

 public:
/** @name Constructors and Index Accessors
 * @{
 */
   /// access function to TofRawSideR objects used
   /// \return number of TofRawSideR used(Fired Side Number)
   int  NTofRawSide(){int sums=0;for(int is=0;is<2;is++)if(fTofRawSide[is]>=0)sums++;return sums;}
   /// access function to TofRawSideR objects used
   /// \return index of TofRawSideR object in collection or -1
   int iTofRawSide(unsigned int i);
   /// access function to TofRawSideR objects used
   /// \return pointer to TofRawSideR object or 0
   TofRawSideR*  pTofRawSide(unsigned int i);

   /// access function to TofRawSie// is=0 NSide is=1 PSide return 0 if not exist
   TofRawSideR*  GetRawSideHS(int is);
   /// if  TofClusterH TofRawSide iside(0-1) exists Return true
   bool          TestExistHS(int is){return (is>=0&&is<2)&&(fTofRawSide[is]>=0);}

  TofClusterHR(){};
  TofClusterHR(int ilay,int ibar):Layer(ilay),Bar(ibar){};
  TofClusterHR(unsigned int sstatus[2],unsigned int status,int pattern,int idsoft,double adca[2],double adcd[2][3],
               double sdtm[2],double times[2],double timer,double etimer,AMSPoint coo,AMSPoint ecoo,double q2pa[2],double q2pd[2][3],double edepa,double edepd,TofRawSideR *tfraws[2]);
  TofClusterHR(AMSTOFClusterH *ptr);
  virtual ~TofClusterHR(){};
/**@}*/


/** @name User Function
 * @{
 */
  /// check Counter Side is good or not //require A+T measure
  bool IsGoodSide(int is){return (is==0)?((Status&TOFDBcN::BADN)==0):((Status&TOFDBcN::BADP)==0);}//Side 0 or 1
  /// check Side whether only 1 LT in FT windows
  bool IsOneLT(int is)  {return (SideBitPat[is]&TOFDBcN::LTMANY)==0;}
  /// check Side whether HT in FT windows
  bool IsExistHT(int is){return (SideBitPat[is]&TOFDBcN::NOWINDOWHT)==0;}
  /// Counter Good Time(TCoo) or not //due to one side bad
  bool IsGoodTime()     {return (Status&TOFDBcN::BADTIME)==0;}
  /// Recover Time from Hit Position+ one Side Time // Bad Time or Side Lost
  /*!
   * @param[in]  tklcoo  longitide hit position to recover 
   * @param[in]  useside Side1 bad(lost)=>useside=0   Side0 bad(lost)=>useside=1
   * @param[out] tm  Recover Time
   * @param[out] etm Recover Time Error
   */
  int TRecover(float  tklcoo,int useside,float &tm,float &etm);
  /// Get Counter Edep From One PMT Estimate (MeV)
  /*! 
    * @param[in] pmtype  1-Anode 0-Dynode
    * @param[in] is PMT Side: 0-NSide 1-PSide
    * @param[in] pm PMTid For Each Side: Dyndoe 0~2  Anode=0
    * @return   =0  No Response or Bad PMT(mask)  <0 PMT Saturation
  */
  float GetEdepPM  (int pmtype, int is,int pm=0);
  /// Q Or Q^2 Estimate From One PMT
   /*!
    * @param[in] pmtype 1-Anode 0-Dynode
    * @param[in] is  PMT Side:  0-NSide 1-PSide
    * @param[in] pm  PMTid For Each Side: Dyndoe 0~2  Anode=0
    * @param[in] opt DefaultQOpt Q Estimate, DefaultQ2Opt Q^2 Estimate
    * @param[in] cosz PathLength Corr
    * @param[in] beta Beta Corr
    * @return  =0  No Response or Bad PMT(mask)  <0 PMT Saturation
  */
  float GetQSignalPM(int pmtype,int is,int pm=0,int opt=DefaultQOpt,float cosz=1,float beta=1);
  /// Edep Measurement Combine From All Good PMTs (MeV)
   /*!
    * @param[in] pmtype 2-Default(Best Between Anode and Dynode) 1-Anode 0-Dynode
    * @param[in] pattern 111(P-PMT)111(N-PMT): Use All Good PMTs; 100(P-PMT)-110(N-PMT): PSide-PMT-No0~1 and NSide-PMT-No0 will not used for Calculation
    * @param[in] optw 1-Different weight for different PMTs. 0-Same weight for All PMTs
  */
  float GetEdep(int pmtype=2,int pattern=111111,int optw=1);
   /// Q Or Q^2 Estimate  From All Good PMTs
   /*!
    * @param[in] pmtype  2-Default(Best Between Anode and Dynode) 1-Anode 0-Dynode -1-For Reconstruction
    * @param[in] opt  DefaultQOpt Q Estimate, DefaultQ2Opt Q^2 Estimate
    * @param[in] cosz PathLength Corr
    * @param[in] beta Beta Corr
    * @param[in] lcoo Longitudinal Coo-Attenuation Corr, Default -1000: Use Counter TCoo
    * @param[in] pattern 111(P-PMT)111(N-PMT): Use All Good PMTs; 100(P-PMT)-110(N-PMT): PSide-PMT-No0~1 and NSide-PMT-No0 will not used for Calculation
    * @param[in] optw 1-Different weight for different PMTs. 0-Same weight for All PMTs
   */
  float GetQSignal(int pmtype=2,int opt=DefaultQOpt,float cosz=1,float beta=1,float lcoo=-1000,int pattern=111111,int optw=1);
  /// Number of Good PMTs Used For Measurement
  /*!
    * @param[in] pmtype 1-Anode 0-Dynode
  */
  int   GetNPMTQ(int pmtype);
  /// Number of BetaHs use this Counter to build 
  /*! 
    * @return 0--this counter not belong to any BetaH  (Matched by zero track)
    * @return 1--this counter belong to 1 BetaH        (Matched by one track)//indicate clear events
    * @return >=2(N)...--this counter belong to N BetaH(Matched by N track)  //indicate many Tracker many Counter Fired
  */
  int  NBetaHUsed( )              {return (Pattern/1000);}
/**@}*/

/** @name TOF Counter Geometry Information
 * @{
 */
  /// Counter Shape is Trapezoid(1) or Rectangle(0)
  bool IsTrapezoid()              {return TOFGeom::IsTrapezoid(Layer,Bar);}
  /// Counter Along Direction ->0(Along x)-1(Along y)
  int  GetDirection()             {return TOFGeom::Proj[Layer];}
  /// Counter PMT Number per Side
  int  GetPMTNumber()             {return TOFGeom::Npmt[Layer][Bar];}
  /// Counter Length
  float GetBarLength()            {return TOFGeom::Sci_l[Layer][Bar];}
  /// Counter Width 
  float GetBarWidth()             {return TOFGeom::Sci_w[Layer][Bar];} 
  /// Counter Overlap Width
  float GetBarWidthO()            {return TOFGeom::Overlapw;}
  /// Counter Edge ///(3)xyz (2)low high edge 
  void  GetBarEdge(float x[3][2]) {return TOFGeom::GetBarEdge(Layer,Bar,x);}
  /// Counter Central Position
  AMSPoint  GetBarPos()           {return TOFGeom::GetBarCoo(Layer,Bar);}
  /// Convert Global Coo to Counter Local Coo
  AMSPoint  GToLCoo(AMSPoint gpos){return TOFGeom::GToLCoo(Layer,Bar,gpos);}
  /// Convert Counter Local Coo to Global Coo
  AMSPoint  LToGCoo(AMSPoint lpos){return TOFGeom::LToGCoo(Layer,Bar,lpos);}
  /// Global Coo Inside Counter or Not 
  /// z=0 don't use z information,  z!=0 include z judgment.
  bool IsInSideBar(float x, float y, float z=0){
       float bdis;
       return  TOFGeom::IsInSideBar(Layer,Bar,x,y,bdis,z);
    }
/**@}*/

  bool operator<(const TofClusterHR &right){
    return Layer*1000+Bar*100<(right.Layer*1000+right.Bar*100);
  }
  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("TofClusterHR Info");
  };

  void Print(int opt=0){
    _PrepareOutput();
    cout<<sout<<endl;
  }

  const char * Info(int number=-1){
    sprintf(_Info,"TofClusterHR L%dB%d  Time(ns)=%3.1f#pm%3.1f, E_{Dep}(MeV)=%4.1f, Coo(%5.1f,%5.1f,%5.1f)#pm(%5.1f,%5.1f,%5.1f)",Layer,Bar,Time,ETime,AEdep,Coo[0],Coo[1],Coo[2],ECoo[0],ECoo[1],ECoo[2]);
    return _Info;
  }
  
   std::ostream& putout(std::ostream &ostr = std::cout){
    _PrepareOutput(1);
    return ostr << sout  << std::endl;
  };

//-------
  friend class AMSTOFClusterH;
  friend class AMSEventR;
  ClassDef(TofClusterHR,7)       //TofRawClusterR
#pragma omp threadprivate(fgIsA)
};


/// AntiClusterR structure

/*!
 \author e.choumilov@cern.ch

*/
class AntiClusterR {
static char _Info[255];
public:
  unsigned int   Status;   ///< Bit"128"->No FT-coinc. on 2 sides;"256"->1sideSector;"1024"->miss.side#
  int   Sector;   ///< //Sector number(1-8)
  int   Ntimes;  ///<Number of time-hits(1st come paired ones)
  int   Npairs;   ///<Numb.of time-hits, made of 2 side-times(paired)
  float Times[16];  ///<Time-hits(ns, wrt FT-time, "+" means younger hit)
  float Timese[16]; ///<Time-hits errors(ns)

  float Edep;    ///<Edep(mev)
  float Coo[3];   ///<R(cm),Phi(degr),Z(cm)-coordinates
  float ErrorCoo[3]; ///<Their errors

  AntiClusterR(){};
  AntiClusterR(AMSAntiCluster *ptr);
  /// \param number index in container
  /// \return human readable info about AnticlusterR
  char * Info(int number=-1){
    float xm=1.e9;
//    for(int i=0;i<Time.size();i++){
//      if(fabs(Time[i])<fabs(xm))xm=Time[i];
//    }
    for(int i=0;i<Ntimes;i++){
      if(fabs(Times[i])<fabs(xm))xm=Times[i];
    }
    sprintf(_Info,"Anticluster No %d Sector=%d R=%5.2f#pm%5.2f #Phi=%5.2f#pm%5.2f Z=%5.2f#pm%5.2f E_{Dep}(MeV)=%7.3g CTime(nsec)=%7.2f",number,Sector,Coo[0],ErrorCoo[0],Coo[1],ErrorCoo[1],Coo[2],ErrorCoo[2],Edep,xm);
  return _Info;
  }
  virtual ~AntiClusterR(){};
ClassDef(AntiClusterR,2)       //AntiClusterR
#pragma omp threadprivate(fgIsA)
};

#ifndef _PGTRACK_
/// TrRawClusterR structure

/*!
 \author vitali.choutko@cern.ch

*/
class TrRawClusterR {
public:
  int address;   ///<Address of leftmost strip \sa TrClusterR Idsoft
  float s2n;     ///< signal to noise for the seed strip
  vector<float> amp; ///<amplitudes

  TrRawClusterR(){};
  TrRawClusterR(AMSTrRawCluster *ptr);
  virtual ~TrRawClusterR(){};
ClassDef(TrRawClusterR,2)       //TrRawClusterR
#pragma omp threadprivate(fgIsA)
};


/// TrClusterR structure
/*!
 \author vitali.choutko@cern.ch

*/
class TrClusterR {
public:
  int Idsoft;  ///< software id
               /*!<
                                                   \n mod(id,10) layer
                                                   \n mod(id/10,100) ladder
                                                   \n i=mod(id/1000,10)
                                                   \n i==0 x 1st half
                                                   \n i==1 x 2nd half
                                                   \n i==2 y 1st half
                                                   \n i==3 y 2nd half
                                                   \n id/10000 strip
            */
  unsigned int Status;  ///< statusword
               /*!<
 1 - REFITTED object                                     (status&1     !=0) \n
 2 - WIDE in shape (Tracker)                             (status&2     !=0) \n
 3 - AMBIGously associated                               (status&4     !=0) \n
 4 - RELEASED object                                     (status&8     !=0) \n
 5 - BAD                                                 (status&16    !=0) \n
 6 - USED as a component of a larger object              (status&32    !=0) \n
 7 - DELETED object                                      (status&64    !=0) \n
 8 - BADTimeHistory (TOF)                                (status&128   !=0) \n
 9 - ONESIDE measurement (TOF)                           (status&256   !=0) \n
10 - BADTIME information (TOF), not used now             (status&512   !=0) \n
11 - NEAR, close to another object (Trck)                (status&1024  !=0) \n
12 - WEAK, defined with looser criteria (Trck)           (status&2046  !=0) \n
13 - AwayTOF, away from TOF predictions (Trck)           (status&4096  !=0) \n
14 - FalseX, x-coordinate built but not measured (Trck)  (status&8192  !=0) \n
15 - FalseTOFX, x-coordinates from TOF (Trck)            (status&16384 !=0) \n
16 - 4th tof plane was recovered using tracker           (status&32768 !=0) \n
17 - LocalDB was used to align track                     (status&65536 !=0) \n
18 - GlobalDB was used to align the track                (status&(65536*2)!=0) \n
19 - Cluster was used to get the charge                  (status&(65536*4)!=0) \n
20 - TrRecHit was good enough to be used in track find   (status&(65536*8)!=0) \n
21 - Track->Trladder interpol was done on plane level    (status&(65536*16)!=0) \n
22 - Track was created using TOF only                    (status&(65536*32)!=0) \n
23 - Object Overflow                                     (status&(65536*64)!=0) \n
26 - CATLEAK  (Ecal only)                               (status&(65536*256)!=0) \n
                 */
  int NelemL;   ///< Number of strips left to max
  int StripM;   ///< Strip at Max
  int NelemR;   ///< Number of strips right to max
  float Sum;    ///< Amplitude total
  float Sigma;  ///< Sigma total
  float Mean;   ///< CofG (local)
  float RMS;    ///< RMS cluster
  float ErrorMean; ///< error in CofG
  vector<float> Amplitude;   ///< strips amplitudes (NelemR-NelemL)

  TrClusterR(){};
  TrClusterR(AMSTrCluster *ptr);
  virtual ~TrClusterR(){};
ClassDef(TrClusterR,3)       //TrClusterR
#pragma omp threadprivate(fgIsA)
};



/// TrRecHitR structure   (3D- tracker clusters)
/*!
 \author vitali.choutko@cern.ch

*/

class TrRecHitR {
static char _Info[255];
public:
  unsigned int   Status;   ///< statusword \sa TrClusterR Status
  ///
  int  Id;             ///<ssddhl ss sensor(1:xx) dd ladder(1:15) h half(0:1) l layer(1u:9d)
  float Hit[3];            ///< cluster coordinates
  float Hitf(int k){return Hit[k];}
   AMSPoint GetCoord(){return AMSPoint(Hit);}  ///< cluster coordinates
  float EHit[3];           ///< error to above
  float Sum;               ///< Amplitude (x+y)
  float DifoSum;           ///< (A_x-A_y)/(A_x+A_y)
  float CofgX;             ///< local cfg x
  float CofgY;             ///< local cfg y
  float Bfield[3];         ///< magnetic field vector (kG)
  float HitDist(const TrRecHitR &o,int icoo){return o.Hit[icoo]-Hit[icoo];}
protected:
  int  fTrClusterX;   ///<x proj TrClusterR pointer
  int  fTrClusterY;   ///<y proj TrClusteR pointer
public:
  /// access function to TrClusterR object used
  /// \param  xy = 'x' for x projection, any other for y projection
  /// \return index of TrClusterR object in TrClusterR collection or -1
  int iTrCluster(char xy)const {return xy=='x'?fTrClusterX:fTrClusterY;}
  /// access function to TrClusterR object used
  /// \param  xy = 'x' for x projection, any other for y projection
  /// \return pointer to TrClusterR TrClusterR object in TrClusterR collection or 0
  TrClusterR * pTrCluster(char xy);
  ///
 ///  \return layer(1....),ladder(1...), stripx(0...) stripy (0...)
 void Geom(int &lay, int &lad, int &half, int &stripx, int &stripy){
    TrClusterR  *x=pTrCluster('x');
    TrClusterR *y=pTrCluster('y');
     lay=-1;
     lad=-1;
     half=-1;
     stripx=-1;
     stripy=-1;
   if(x)stripx=x->StripM;
   if(y){
    int id=y->Idsoft;
    lay=id%10;
    lad=(id/10)%100;
    half=(id/1000)%10-2;
    stripy=y->StripM;
   }
}

   ///
  /// \return layer
      int lay()const{return Id%10;}
  /// \return ladder
      int lad()const{return (Id/100)%100;}
  /// \return half
      int half()const{return (Id/10)%10;}
  /// \return sensor
      int sen()const{return (Id/10000)%100;}
  ///
 ///  \return stripx
 int  stripx(){
    TrClusterR  *x=pTrCluster('x');
   if(x)return x->StripM;
   else return -1;
}
  ///
 ///  \return stripy
 int  stripy(){
    TrClusterR *y=pTrCluster('y');
   if(y)return y->StripM;
   else return -1;
}
   ///
  /// \return human readable info about TrRecHitR
  char * Info(int number=-1){
   sprintf(_Info,"TrRecHit no %d Id %d Ampl=%4.1f, at (%9.4f,%9.4f,%9.4f)#pm(%5.3f,%5.3f,%5.3f, asym=%4.1f, status=%x,llhsss=%d %d %d %d %d %d)",number,Id,Sum,Hit[0],Hit[1],Hit[2],EHit[0],EHit[1],EHit[2],DifoSum,Status,lay(),lad(),half(),sen(),stripx(),stripy());
     return _Info;
}

  TrRecHitR(){};
  TrRecHitR(AMSTrRecHit *ptr);
  friend class AMSTrRecHit;
  friend class AMSEventR;
  virtual ~TrRecHitR(){};
ClassDef(TrRecHitR,4)       //TrRecHitR
#pragma omp threadprivate(fgIsA)
};



/// /TrTrackR structure
/*!
 \author j.alcaraz@cern.ch vitali.choutko@cern.ch

*/

class TrTrackR {
static char _Info[255];
public:
  unsigned int Status;   ///< statusword \sa TrClusterR
  ///
  int Pattern;  ///< see datacards.doc
  int Patternf(){return Pattern;}
  unsigned long long Addressl;  ///< ladders combination code (see trrec.C buildaddress)
///<  != 0 if done
  int   GeaneFitDone; //!
/// != 0 if done
  int   AdvancedFitDone;  //!
  float Chi2StrLine;  ///< chi2 sz fit
  float Chi2StrLinef(){return Chi2StrLine;}
  float Chi2WithoutMS;  ///< chi2 circular fit OR pathint chi2  without ms
  float Chi2WithoutMSf(){return Chi2WithoutMS;}
  float RigidityWithoutMS;  ///< circular rigidity OR pathint rigidity without ms
/// chi2 fast nonl fit
  float Chi2FastFit;    //!
  float Chi2FastFitf(){return Chi2FastFit;}
/// fast nonl rigidity
  float Rigidity;  //!
  float Rigidityf(){return Rigidity;}
///err to 1/above
  float ErrRigidity;  //!
/// theta (from fast)
  float Theta;      //!
/// phi (from fast)
  float Phi;        //!
  AMSDir GetDir(){return AMSDir(Theta,Phi);} ///< dir cos
/// coo (from fast)
  float P0[3];      //!
/// geane chi2
  float GChi2;     //!
/// geane rigidity
  float GRigidity;  //!
/// geane err(1/rigidity)
  float GErrRigidity; //!
/// two halves chi2s
  float HChi2[2];     //!
  float HChi2f(int k){return HChi2[k];}
/// two halves rigidities
  float HRigidity[2];  //!
   float HRigidityf(int k){return HRigidity[k];}
/// two halves err(1/rig)
  float HErrRigidity[2];  //!
/// two halves theta
  float HTheta[2];   //!
/// two halves phi
  float HPhi[2];     //!
/// two halves coo
    float HP0[2][3];   //!
///< fast chi2 mscat off
  float FChi2MS;    //!
  float FChi2MSf(){return FChi2MS;}
 /// PathInt err(1/rig) (<0 means fit was not succesful)
  float PiErrRig;    //!
/// fast rigidity mscat off
    float RigidityMS;  //!
///  PathInt rigidity
  float PiRigidity;  //!
///Fast,PI,RK Rigidities with only internal or only external points
  float RigidityIE[3][2];  //!
  float Hit[9][3];   ///< Track Coordinate (after alignment)
  float EHit[9][3];   ///< Track Error Coordinate (after alignment)
  AMSPoint GetCoord(unsigned int i){return i<9?AMSPoint(Hit[i]):AMSPoint(0,0,0);}
  float DBase[2];    ///< Rigidity & chi2 without alignment
  TrTrackR(AMSTrTrack *ptr); ///< GBatchConstructor
  bool Compat(int refiti=0); ///< Compatibility function to be called once to ensure old functionality
  vector<TrTrackFitR> fTrTrackFit; ///< !!Vector of Fits;
  protected:
vector<int> fTrRecHit;  ///< Vector of trrechit indexes
  public:
   int PatternL();///< \return   1....9  123459 means hits in layers are present; 1 top 9 bottom)
   int Layer(unsigned int i); ///< return layer number (0...8) for the Hit[i]
  bool isSubSet(int pattern); ///< return true if pattern is subset of track Bitpattern;
   int BitPattern; ///<  bits xxxxxxxx where x =0 1 and layer 1 corresponds to the leaset significant bit
   int ToBitPattern( int patternL);///< return bit pattern for given patternL or -1 if error
   unsigned int NHits()const ;  ///< retutn NHits;

  TrTrackR(){};
/// Get and optionally refit  track using pattern bit pattern (inclusive)
/// \return index of the fTrTrackFit vector or -1 if no such element or -2 if alig or alg or ms  not defined or -3 if pattern is not subset of BitPattern or -4 if refit failed or not yet implemented
/// \param refit 0: do not refit, 1 refit if not present, 2 refit and replace
/// \param fit  TrTrackFitR params  (pattern,alg,alig,ms) also returned
int iTrTrackFit(TrTrackFitR &fit, int refit=1);   ///< !!accessor

bool setFitPattern(TrTrackFitR::kAtt k, int & pattern); ///< set fit pattern for the given attribute  return false if not

 /// access function to TrTrackFitR
 /// select good tracks (i.e. tracks having x & y hits from tracker)
  /// \return true if good false otherwise
 ///
   bool IsGood()const {return !(Status & 16384) && AdvancedFitDone!=0;}
  /// access function to TrRecHitR objects used
  /// \return number of TrRecHitR used
  int NTrRecHit()const {return fTrRecHit.size();}
  /// access function to TrRecHitR objects used
  /// \param i index of fTrRecHit vector
  /// \return index of TrRecHitR object in collection or -1
  int iTrRecHit(unsigned int i){return i<fTrRecHit.size()?fTrRecHit[i]:-1;}
  /// access function to TrRecHitR objects
  /// \param i index of fTrRecHit vector
  /// \return pointer to TrRecHitR object  or 0
  TrRecHitR * pTrRecHit(unsigned int i);
  /// \param number index in container
  /// \return human readable info about TrTrackR
  char * Info(int number=-1);
  virtual ~TrTrackR(){};
ClassDef(TrTrackR,15)       //TrTrackR
#pragma omp threadprivate(fgIsA)
friend class AMSTrTrack;
friend class AMSEventR;
};

#endif

//!  Rich Hits Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/

class RichHitR {
static char _Info[255];
public:
   unsigned int Status;   ///< statusword
                         /*!<

*********) Status bits (counting from 1 to 32)

1- Hit used in the ring number 1
2- Hit used in the ring number 2
3- Hit used in the ring number 3
.
.
.
10- Hit used in the ring number 10 (no more than 10 rings currently flagged)
11- Unused
.
.
.
29- Channel taggeg as good in calibration
30- Gain mode chosen for the hit: 0=x1(low)  1=x5(high)
31- Hit belongs to a PMT apparently crossed by a charged particle


*/

  int   Channel;    ///<  channel number (16*PMT+pixel)
  int   Counts;     ///< ADC counts above the pedestal
  float Npe;        ///< ADC counts above the pedestal/gain of the channel
  float Coo[3];     ///<  Hit coordinates
  int   HwAddress;  ///< Hardware address
  int SoftId   ;    ///< Online software identification (PMT*16+pixel)

  RichHitR(){};
  RichHitR(AMSRichRawEvent *ptr, float x, float y, float z);
    /// \param number index in container
  /// \return human readable info about RichHitR
 ///
  char * Info(int number=-1){
    //    sprintf(_Info,"RichHit No %d Channel=%d Ampl=%d No_{PhotoEl}=%5.2f, Coo=(%5.2f,%5.2f,%5.2f) Status=%u Gain=%d HotSpot=%d ",number,Channel,Counts,Npe,Coo[0],Coo[1],Coo[2],Status%2048,(Status>>29)%2?5:1,(Status>>30));
    sprintf(_Info,"RichHit No %d Channel=%d HwAddress=%d Ampl=%d No_{PhotoEl}=%5.2f, Coo=(%5.2f,%5.2f,%5.2f) Status=%u Gain=%d HotSpot=%d OK=%s",
	    number,
	    Channel,
	    HwAddress,
	    Counts,
	    Npe,
	    Coo[0],Coo[1],Coo[2],
	    Status%2048,
	    (Status>>29)%2?5:1,             // Check richconst to be wure about this
	    (Status>>30),
	    (Status>>28)&1?"true":"false");
  return _Info;
  }

  virtual ~RichHitR(){};

  bool IsCrossed(){return (Status&(1<<30))!=0;}
  bool UsedInRingNumber(int number){return (Status&(1<<number))!=0;}
  bool IsHighGain(int){return (Status&(1<<29))!=0;}
  /// Number of photoelectrons
  int  PhotoElectrons(double sigmaOverQ=0.6);
  /// Return the number of photoelectrons in the event, discarding those PMTs crossed by charged particles
  static float getCollectedPhotoElectrons();
  /// Return the number of PMTs. This is useful because the algorithm indetifiying crossed PMTS relies in a large enough statistics of PMTs in the event
  static int  getPMTs(bool countCrossed=true);

ClassDef(RichHitR,6)       // RichHitR
#pragma omp threadprivate(fgIsA)
};


#include "TH1F.h"



//!  Rich Ring Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/


class RichRingR {
public:
enum{ready=-1,noCorrection=0,tileCorrection,fullUniformityCorrection};

static int shouldLoadCorrection;           ///< Holds the current state of the corrections loader
static bool loadChargeUniformityCorrection; ///< Allows to enable or disable using the uniformity correction for charge

/// Rich Charge Corrections Settings & Flags
static bool loadPmtCorrections; ///< Allows to enable or disable PMT corrections for charge
static int pmtCorrectionsFailed;///< Corrections fail flag (-1/0/1 : Not/Done/Failed)
static TString correctionsDir;  ///< Directory containing corrections
static bool useRichRunTag;      ///< Define corrections only for good runs
static bool usePmtStat;         ///< Define correnctions only for good PMT
static bool useSignalMean;      ///< Equalize PMT Gains to signal mean(median)
static bool useGainCorrections; ///< Activate PMT Gain equalization
static bool useEfficiencyCorrections;  ///< Activate PMT Efficiency equalization
static bool useBiasCorrections; ///< Activate PMT Efficiency bias corrections
static bool useTemperatureCorrections; ///< Activate PMT Temperature corrections
static bool useExternalFiles; ///< Use external files instead of info stored in the root file
static bool reloadTemperatures; ///< Force load of Temperatures from external files
static bool reloadRunTag; ///< Force load of Config & Status info from external files
static bool reloadPMTs; ///< Force load of PMT info info from external files
/** @name Sets the initial correction to reconstrcuted beta.There are three possibilities foreseen, and only two implementes:

 *   RichRingR::noCorrection - No initial information is loaded. The dynamic calibration can be used to update the refractive index on the tiles on the flight

 *   RichRingR::tileCorrection - A default initial correction to the refractive indexes are loaded. These values can be subsequently corrected using the dynamic calibration, although it is not necessary. This is loaded by default.

 *   RichRingR::fullUniformityCorrection -  A fine grained correction to the refractive index, which also takes into account the direction of the incident particle, is loaded. Further corrections using the dynamic calibration are not possible. 

 */
  ///@{
  /// Sets the initial correction
  ///\ param Modes:  RichRingR::noCorrection, RichRingR::tileCorrection, RichRing::fullUniformityCorrection
static void setBetaCorrection(int mode){shouldLoadCorrection=mode;}

static bool _updateDynamicCalibration;
static double _pThreshold;
static int    _tileCalEvents;
static int    _tileCalWindow;
static double _tileLearningFactor;
static char _Info[255];
static bool _isCalibrationEvent;
/// Selection of events for the dynamic calibration
static bool calSelect(AMSEventR &event);
/// Returns an unique tile id for the tile crossed by the particle.
/// This is is unrelated to the one used internally in the reconstruction
int getTileIndex();
static int getTileIndex(float x,float y);
static void updateCalibration( AMSEventR &event);
public:
static TH1F indexHistos[122];
static double _sumIndex[122];
static int _totalIndex[122];
static double indexCorrection[122];
static int _lastUpdate[122];
static int _numberUpdates[122];
public:
/** @name Dynamic calibration utilities
 *  These methods provide switchs to control the RICH dynamic calibration
 */
  ///@{
  /// Activate (or deactivate) the dynamic calibration
 static void switchDynCalibration();
  /// Get the status of the dynamic calibration
  static bool isCalibrating(){return RichRingR::_updateDynamicCalibration;}
  /// Return if current event has been used for calibration
  static bool isCalibrationEvent(){return _isCalibrationEvent;}
  /// Set the momentum threshold to consider a Z=1 particle as a beta=1 one
  static void setPThreshold(double th){_pThreshold=th;}
  /// Get the momentum threshold to consider a Z=1 particle as a beta=1 one
  static double getPThreshold(){return _pThreshold;}
  /// Set the number of event required to update the tile calibration
  static void setTileCalEvents(int n){_tileCalEvents=n;};
  /// Get the number of event required to update the tile calibration
  static int getTileCalEvents(){return _tileCalEvents;};
  /// Set the window width for tile calibration
  static void setTileCalWindow(int n){_tileCalWindow=n;};
  /// Get the window width for tile calibration
  static int getTileCalWindow(){return _tileCalWindow;};
  /// Set the tile calibration learning factor
  static void setTileLearning(double f){_tileLearningFactor=f;};
  /// Get the tile calibration learning factor
  static double getTileLearning(){return _tileLearningFactor;};
  /// Get used multiplicative correction due to the dynamic calibration
  double betaCorrection();
  /// Get multiplicative correction for a crossing point in the radiator
  static double betaCorrection(float index,float x,float y);
  /// Get number of time the calibration for the used tile has been updated
  int updates();
  /// Get number of time the calibration for the tile at the given crossing point has been updated
  static int updates(float x,float y);
  /// Push a value into the syn calibration manually
  static void calPush(double beta,double index,float x,float y);
  ///@}

/** @name Class variables
 *  These variables contain reconstructed quantities. However access to  them should
 *  rely of the accessor methods instead the variables themselves, because
 *  some corrections taken into account uniformity and time depencence is
 *  applied within the accessor methods
 */
  ///@{

 unsigned int Status;     ///< status word
                           /*!<

**********) Ring status bits (counting from 1 to 32)
1- Ring has been rebuild after cleaning PMTs apparently crossed by a charged particle.
   If the rebuilding has been succesful it is stored in the next ring in the rings container.
   However to confirm that next ring is indeed a rebuilt one, both tracks, the one from the
   current and the one from the next, should be the same. Otherwise the crebuilding was unsuccesful.

2- Ring reconstructed using the NaF radiator in the double radiator configuration

14 - Associated to a particle

16-30 reserved

31 - Associated to a track used in a gamma reconstruction.

*/

  int   Used;       ///< Nb. of RICH hits in the ring cluster
  int   UsedM;      ///< Nb. of RICH reflected hits in the ring cluster
  float Beta;       ///< Reconstructed velocity using only hits (no charge)
  float ErrorBeta;  ///< Estimate error in the velocity
  float BetaRefit;  ///< Beta estimate taking into account the number of photoelectrons for each hit
  float Prob;       ///< Kolmogorov test probability to be a good ring based on azimuthal distribution
  float UDist;      ///< (\sum_i 1/\dist_i^2) for unused hits which do not belong to PMTs crossed by a charged particle
  float NpExp;      ///< Number of expected photoelectrons for Z=1 charge
  float NpCol;      ///< Number of collected photoelectrons. The rich charge reconstruction is estimated as sqrt(NpCol/NPExp)
  float NpColLkh;   ///< Number of collected photoelectrons computed using a weighted mean (experts only)
  float Theta;      ///< Recontructed emission angle
  float ErrorTheta; ///< Error of the reconstructed emission angle
  float TrRadPos[3];///< Mean emission point of the Cerenkov photons
  float TrPMTPos[3];///< Intersection point of the track with the PMT plane
  float AMSTrPars[5];///< Radiator crossing track parameters (in AMS frame): x y z theta phi
  map<unsigned short,float>NpColPMT;     /// Collected photoelectrons in the ring per PMT
  map<unsigned short,float>NpExpPMT;     /// Expected photoelectrons in the ring per PMT

 protected:
  int fTrTrack;   ///< index of  TrTrackR  in collection
   vector<int> fRichHit; ///< indexes of RichHitR in collection
   vector<float> fBetaHit;  ///<Beta Residues for each event hit. A negative value means it is reflected
   void FillRichHits(int m);
 public:
   int   UsedWindow[10];      ///< Hits asociated to the ring for different windows sizes (1,2,3...10)
   float NpColWindow[10];       ///< Photoelectrons asociated to the ring for different windows sizes (1,2,3...10)

  // Time dependent charge corrections
  map<unsigned short,float> NpColCorr;   //!  Efficiency correction to NpExpPMT
  map<unsigned short,float> NpExpCorr;   //!  Temperature correction to npcol

  ///@}



  // Add new variables here.


  public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to RichHitR objects used
  /// \return index of RichHitR object in collection or -1
  int iRichHit(unsigned int i){return i<fRichHit.size()?fRichHit[i]:-1;}
  /// access function to the reconstructed beta per hit
  /// \param i index of the hit.
  /// \return Reconstructed beta for the hit number i or 0 if no more reconstructed hits are available
  ///         A negative beta is returned for reflected hits
  float BetaHit(unsigned int i){return (i<fBetaHit.size())?fBetaHit[i]:0;}

  /// Class empty constructor (as required by root)
  RichRingR(){};

  /// Class constructor
  RichRingR(AMSRichRing *ptr, int nhits);
  friend class AMSRichRing;
  friend class AMSEventR;


  /////////////////////////
  /// Accesor utilities ///
  /////////////////////////
/** @name User accessor methods
 *  These methods provide user level access to the necessary RICH reconstructed quantities.
 * These are preferred to directly accessing the public attributes in the class and in some cases they provided further functionality.
 */
  ///@{
  /// Check if the ring has been rebuilt
  /// \return true if the ring has been rebuilt
  bool Rebuild(){return (Status&1)!=0;}
  /// Check if the ring is contaminated by charge particles crossing the RICH detection plane
  /// \return true if the ring is good
  bool IsGood(){return IsClean() && getExpectedPhotoelectrons(false)>1e-6;}
  bool IsClean(){return (Status&1)==0;}
  /// Bool set to true if the radiator used is NaF
  bool IsNaF(){return (Status&2)!=0;}
  /// Distance of the track impacto point in the radiator to the border of the radiator tile
  double DistanceTileBorder(){double value=double((Status>>15)&0x3ff)/100.;return value>8?0:value;}
  /// Compute the width of the reconstructed ring measured in the expected width.
  /// \param usedInsteadNpCol ignore the signal collected in the hits if set to true
  double RingWidth(bool usedInsteadNpCol=false);

  //
  // Brand new user interface
  //
  /// Beta of the event
  /// \return Beta
  float getBeta()          {return BetaRefit*betaCorrection();}
  /// Build charge corrections
  bool buildChargeCorrections();
  /// Charge corrections fail flag (-1/0/1 : Not/Done/Failed)
  int PmtCorrectionsFailed(){return pmtCorrectionsFailed;}
  /// Total number of used hits in the ring
  int getUsedHits(bool corr=true);
  int getUsedHits(int pmt, bool corr);
  /// Total number of photoelectrons in the ring.
  float getPhotoelectrons(bool corr=true){return getPhotoElectrons(corr);}
  float getPhotoElectrons(bool corr=true);
  float getPhotoElectrons(int pmt, bool corr);
  /// Number of expected photoelectrons for a Z=1 ring with the reconstruction input parameters of the current event.
  float getExpectedPhotoelectrons(bool corr=true){return getExpectedPhotoElectrons(corr);}
  float getExpectedPhotoElectrons(bool corr=true);
  float getExpectedPhotoElectrons(int pmt, bool corr);
  /// Continuous Z^2 estimate for this ring
  float getCharge2Estimate(bool corr=true) {return getExpectedPhotoelectrons(corr)>0?getPhotoElectrons(corr)/getExpectedPhotoelectrons(corr):0;}
  /// Estimation of the error of the reconstructed beta
  /// \return Estimate of the error of the reconstructed beta
  float getBetaError()     {return sqrt(2.5e-3*2.5e-3*(IsNaF()?9.0:1.0)/getPhotoElectrons()+1e-4*1e-4);}
  /// Quality parameter, providing the probability result of applying a Kolmogorov test to the distribution of charge along the ring.
  /// This quantity is almost uniformly distributed between 0 and 1 for rings correctly reconstructed, and peaks at 0 for incorrectly reconstructed ones.
  /// \return Kolmogorov test probability.
  float getProb()          {return Prob;}
  /// Quality parameter providing the width of the distribution of charge around the ring over the expected one.
  /// This quantity should be close to one from above for correctly reconstructed rings, and large otherwise.
  /// \param usedInsteadNpCol Is the same parameter used in RingWidth
  /// \return Width of the ring
  float getWidth(bool usedInsteadNpCol=false){return RingWidth(usedInsteadNpCol);}

  /// \return Uncorrected index associated to the track crossing point.
  float getRawIndexUsed()     {return 1.0/Beta/cos(Theta);}
  /// \return Index associated to the track crossing point.
  float getIndexUsed()     {return getRawIndexUsed()/betaCorrection();}
  /// The track parameters extrapolated to the radiator as used in the reconstruction.
  /// \return A pointer to an array of 5 floats, corresponding to x,y,z theta and phi of the track used in the reconstruction
  const float *getTrackEmissionPoint(){return AMSTrPars;}
  float getTrackTheta()               {return AMSTrPars[3];}
  float getTrackPhi()                 {return AMSTrPars[4];}
  /// Compute the absolute value of the difference of the recontructed beta between the algorithm used in RichRingR class and the one used in RichRingBR
  /// \return Difference in the reconstructed beta between the two RICH reconstruction algorithms
  float getBetaConsistency();
  /// Number of hits in the ring
  int   getHits()          {return Used;}
  /// Number of pmts in the ring
  int   getPMTs();
  /// Number of hits which are consistent with reflected photons
  int   getReflectedHits() {return UsedM;}
  /// Simple statistical test to check if the hit by hit charge is consistent with PMT by PMT one
  float getPMTChargeConsistency();
  ///@}

/** @name Interface to retrieve quality estimates of the uniformity corrections
 * This is only availble if the uniformity corrections are used. Otherwise the
 * functions return -1.
 */
  ///@{
  // Expected resolution for Z=1 particles
  float getBetaExpectedResolution();
  // Expected Rms of the distribution of beta for Z=1 beta=1 particles.
  // This gives an estimate of how likely is the event to have tails
  float getBetaExpectedRms();
  float getChargeExpectedResolution();
  float getChargeExpectedRms();
  ///@}


  /// \param number index in container
  /// \return human readable info about RichRingR
  char * Info(int number=-1){
    sprintf(_Info,"RichRing No %d Track=%d %s%s%s N_{Hits}=%d N_{MirrHits}=%d  #beta=%7.3g#pm%6.2g Prob_{Kl.}=%7.3g Width=%7.3g Expected_{PhotoEl}=%5.2f Collected_{PhotoEl}=%5.2f",number,fTrTrack,Status&2?"NaF":"",Status&1?"Refit":"",Status&(16384*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2U)?"Gamma":"",Used,UsedM,Beta,ErrorBeta,Prob,RingWidth(),NpExp,NpCol);
    return _Info;
  }

  virtual ~RichRingR(){};
  ClassDef(RichRingR,28)           // RichRingR
#pragma omp threadprivate(fgIsA)
};

//!  A class to dump and retrieve dynamic calibration values

class RichRingTables:public TObject{
 public:
  TH1F indexHistos[122];
  double _sumIndex[122];
  int _totalIndex[122];
  double indexCorrection[122];
  int _lastUpdate[122];
  int _numberUpdates[122];

  RichRingTables();
  static void Save(TString fileName);
  static bool Load(TString fileName);
  ClassDef(RichRingTables,2)
#pragma omp threadprivate(fgIsA)
};


//!  LIP Rich Ring Segment
//!  (part of reconstructed ring touching a given RICH PMT or pixel)

/*!
 \authors barao@lip.pt, pereira@lip.pt
*/

class RichRingBSegmentR {
 public:
  short int PMT;        ///< RICH PMT where segment falls
  short int Pixel;      ///< Pixel (between 0 and 15) where segment falls,
                        ///< or -1 if segment corresponds to a whole PMT
  short int RefStatus;  ///< segment reflectivity status flag:
                        ///< 0 = direct branch,
                        ///< 10*S+N = reflected branch with N reflections
                        ///< from mirror sector S
  float Theta;          ///< photon angle at light guide (with respect to RICH z axis)
  float Phi;            ///< photon angle at light guide (azimuthal)
  float Acceptance;     ///< integrated geometrical acceptance
  float EffRad;         ///< integrated efficiency (radiator only)
  float EffFull;        ///< integrated efficiency (full, i.e. radiator*LG)
 public:
  virtual ~RichRingBSegmentR(){};
  ClassDef(RichRingBSegmentR,1)         // RichRingBSegmentR
#pragma omp threadprivate(fgIsA)
};


//!  LIP Rich Ring Structure

/*!
 \authors barao@lip.pt, pereira@lip.pt
*/

class RichRingBR {
static char _Info[255];
 public:

  int Status;                          ///< Reconstruction status
                                       ///< Status = i+10*j,
                                       ///< Kind of velocity reconstruction performed:
                                       ///<          i = 2    (track: standard track)
                                       ///<          i = 3    (track: TOF upper+lower || weakly constrained tracker with larger errors)
                                       ///<          i = 4    (track: from RICH standalone)
                                       ///<          i = 5    (track: TOF upper+lower && RICH)
                                       ///<          i = 6    (track: TOF upper && RICH)
                                       ///<          i = 7    (track: TOF lower && RICH)
                                       ///< Charge reconstruction:
                                       ///<          j = 0    (off)
                                       ///<          j = 1    (on)
  float Beta;                          ///< reconstructed velocity
  float AngleRec;                      ///< reconstructed Cerenkov angle (rad)
  float AngleRecErr;                   ///< error in reconstructed Cerenkov angle (rad)
  float Likelihood;                    ///< likelihood prob for reconstruction
  int Used;                            ///< number of hits used in reconstruction
  float ProbKolm;                      ///< Kolmogorov prob for ring hits uniformity
  float Flatness[2];                   ///< flatness parameter (sin,cos)
  float ChargeRec;                     ///< reconstructed charge (full ring)
  float ChargeProb[3];                 ///< probabilities for nearest integer charges
  float ChargeRecDir;	               ///< reconstructed charge (from direct branch)
  float ChargeRecMir;                  ///< reconstructed charge (from reflected branch)
  float NpeRing;	               ///< n. of photoelectrons in ring (full ring)
  float NpeRingDir;	               ///< n. of photoelectrons in ring (direct branch)
  float NpeRingRef;	               ///< n. of photoelectrons in ring (reflected branch)
  float RingAcc[3];	               ///< geometrical ring acceptances
                                       ///< [0] = visible acceptance
                                       ///< [1] = direct acceptance
                                       ///< [2] = mirror acceptance, 1 reflection
  float RingEff[6];                    ///< ring efficiencies
                                       ///< [0] = total efficiency
                                       ///< [1] = total efficiency, direct
                                       ///< [2] = total efficiency, 1 reflection
                                       ///< [3] = total efficiency, 2 reflection
                                       ///< [4] = radiator efficiency * geometrical acceptance
                                       ///< [5] = light guide efficiency
  float RingAccRW[2];                  ///< radiator wall ring acceptance fraction
                                       ///< [0] = direct branch
                                       ///< [1] = reflected branch
  int NMirSec;                         ///< number of mirror sectors
  float RingAccMsec1R[3];              ///< ring acceptance for mirror sectors, 1 reflection
  float RingAccMsec2R[3];              ///< ring acceptance for mirror sectors, 2 reflection
  float RingEffMsec1R[3];              ///< ring efficiency for mirror sectors, 1 reflection
  float RingEffMsec2R[3];              ///< ring efficiency for mirror sectors, 2 reflection
  float RingAccRWMsec[3];              ///< radiator wall ring acceptance fraction
                                       ///< for reflected branches by mirror sector
  map<unsigned short,vector<RichRingBSegmentR> > Segments;  ///< Ring segment data by PMT
  std::vector<float> HitsResiduals;    ///< hit residuals (ring and non-ring hits)
  std::vector<int> HitsStatus;         ///< hit status:
                                       ///<         -2 = not considered for reconstruction,
                                       ///<         -1 = not associated to ring,
                                       ///<          0 = direct,
                                       ///<          1,2,etc reflections.
  std::vector<float> TrackRec;         ///< reconstructed track parameters (in LIP coord system)
                                       ///< TrackRec[0] = x                TrackRec[1] = error in x
                                       ///< TrackRec[2] = y                TrackRec[3] = error in y
                                       ///< TrackRec[4] = z                TrackRec[5] = error in z
                                       ///< TrackRec[6] = theta (radians)  TrackRec[7] = error in theta
                                       ///< TrackRec[8] = phi (radians)    TrackRec[9] = error in phi
 protected:
  int fTrTrack;   ///< index of  TrTrackR in collection or null if rec. i=3,4,5,6,7
  vector<int> fRichHit; ///< indexes of RichHitR in collection
  //void FillRichHits(int ringnew);
 public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to RichHitR objects used
  /// \return index of RichHitR object in collection or -1
  int iRichHit(unsigned int i){return i<fRichHit.size()?fRichHit[i]:-1;}

  RichRingBR(){};
  RichRingBR(AMSRichRingNew *ptr);
  friend class AMSRichRingNew;
  friend class AMSEventR;

  //////////////////////////
  /// Accessor utilities ///
  //////////////////////////

  /// Bool set to true if the radiator used is NaF
  bool IsNaF() {return 1./(Beta*cos(AngleRec))>1.1;}
  /// Refractive index used in the reconstruction
  /// \return Index associated to the track crossing point.
  float getIndexUsed() {return 1./(Beta*cos(AngleRec));}
  /// Compute the absolute value of the difference of the recontructed beta between the algorithm used in RichRingR class and the one used in RichRingBR
  /// \return Difference in the reconstructed beta between the two RICH reconstruction algorithms
  float getBetaConsistency();
  /// Compute the Difference of the recontructed beta between the algorithm used in RichRingR class and the one used in RichRingBR
  /// \return Signed difference in the reconstructed beta between the two RICH reconstruction algorithms (positive means RichRingR has larger value than RichRingBR)
  float getBetaConsistencySigned();

  /// \param number index in container
  /// \return human readable info about RichRingBR
  char * Info(int number=-1){
    int tkStatus;
    if(Status>10)
      tkStatus=Status-10;
    else
      tkStatus=Status;

    sprintf(_Info,"RichRingB No %d Track=%d %s %s%s%s%s%s%s  N_{Hits}=%d #beta=%7.3g #theta_{c}=%6.3g ",
	    number,fTrTrack,
	    Status>10?"velocity+charge":"velocity only",
	    tkStatus==2?"standard rec.":"",
	    tkStatus==3?"flexible rec., track from uTOF+lTOF":"",
	    tkStatus==4?"standalone rec.":"",
	    tkStatus==5?"flexible rec., track from uTOF+lTOF+RICH":"",
	    tkStatus==6?"flexible rec., track from uTOF+RICH":"",
	    tkStatus==7?"flexible rec., track from lTOF+RICH":"",
	    Used,
	    Beta,
	    AngleRec*180./3.14159265);
    return _Info;
  }
  virtual ~RichRingBR(){};
  ClassDef(RichRingBR,5)           // RichRingBR
#pragma omp threadprivate(fgIsA)
};


/// TRDRawHitR structure
/*!
 \author vitali.choutko@cern.ch

*/


/*
class TrdRawHitR {
protected:
public:
  int Layer;   ///< Layer 0(bottom)...19(top)
  int Ladder;  ///<  Ladder  number
  int Tube;    ///< tube number
  float Amp;   ///< amplitude (adc counts)
  int Haddr;   ///< Hardware Address cufhh  c crate[0-1],u udr[0-6] f ufe [0-7] hh channel[0-63]
   unsigned int getid();  ///< return channel number in TRDPedestals, TRDSigmas,TRDGains structures
  float getped(int &error);   ///< return ped
  float getsig(int &error);   ///< return sigma
  float getgain(int &error);  ///< return gain from TRDGains
  float getgain2(int &error);  ///< return gain from TRDGains2
  TrdRawHitR(){};
  TrdRawHitR(AMSTRDRawHit *ptr);

  virtual ~TrdRawHitR(){};
ClassDef(TrdRawHitR,4)       //TrdRawHitR
#pragma omp threadprivate(fgIsA)
};*/

/// TRDClusterR structure
/*!
 \author vitali.choutko@cern.ch

*/

class TrdClusterR {
static char _Info[255];
public:
  unsigned int   Status;    ///< statusword
  float Coo[3];    ///<cluster coo (cm)
  int   Layer;     ///<Layer 0(bottom)...19(top)
  int   Direction; ///<  0 == along x  1 == along y
  float ClSizeR;    ///< Tube Radius
  float ClSizeZ;    ///< Tube 1/2 Length
  int   Multip;    ///< multiplicity
  int   HMultip;   ///< multiplicity above threshold (5.9 kev)
  float EDep;      ///< energy dposition (kev)
  protected:
  int fTrdRawHit;   ///< pointer to trdrawhit with max ampl
  public:
  /// access function to TrdRawHitR object used
  /// \return index of TrdRawHitR object in collection or -1
  int iTrdRawHit()const {return fTrdRawHit;}
  /// access function to TrdRawHitR object used
  /// \return pointer to TrdRawHitR object or 0
  TrdRawHitR * pTrdRawHit();
  ///
  /// \return path length of the stright line in the trd tube
  double Range(float coo[], float Theta, float Phi);
  /// \return path length of the stright line in the trd tube
  static double RangeCorr(double range,double norm);
  TrdClusterR(){};
  TrdClusterR(AMSTRDCluster *ptr);
  friend class AMSTRDCluster;
  friend class AMSEventR;
  /// \param number index in container
  /// \return human readable info about TrdClusterR
  char * Info(int number=-1){
   sprintf(_Info,"TRD Cluster No %d Layer %d TubeDir %s Coo %5.1f,%5.1f,%5.1f  Mult  %d HMult %d E_{Dep}(Kev) %5.1f Amp %5.1f Haddr %d Status %x",number,Layer,(Direction==0?"x":"y"),Coo[0], Coo[1], Coo[2],Multip,HMultip,EDep,pTrdRawHit()->Amp,pTrdRawHit()->Haddr,Status);
   return _Info;
  }
  virtual ~TrdClusterR(){};
ClassDef(TrdClusterR,3)       //TrdClusterR
#pragma omp threadprivate(fgIsA)
};

/// TRDSegmentR structure
/*!
 \author vitali.choutko@cern.ch

*/


class TrdSegmentR {
public:
  unsigned int   Status;            ///< status word
  int   Orientation;       ///< segment orientation (0-x 1-y)
  float FitPar[2];         ///< str line fit pars
  float Chi2;              ///< str line chi2
  int   Pattern;           ///< segment pattern no

  protected:
  vector<int> fTrdCluster;   // used TrdClusterR's indexes
  public:
  /// access function to TrdClusterR objects used
  /// \return number of TrdClusterR used
  int NTrdCluster()const {return fTrdCluster.size();}
  /// access function to TrdClusterR objects used
  /// \param i index of fTrdCluster vector
  /// \return index of TrdClusterR object in collection or -1
  int iTrdCluster(unsigned int i){return i<fTrdCluster.size()?fTrdCluster[i]:-1;}
  /// access function to TrdClusterR collection
  /// \param i index of fTrdCluster vector
  /// \return pointer to TrdClusterR object or 0
  TrdClusterR * pTrdCluster(unsigned int i);
  friend class AMSTRDSegment;
  friend class AMSEventR;
  TrdSegmentR(){};
  TrdSegmentR(AMSTRDSegment *ptr);
  virtual ~TrdSegmentR(){};
ClassDef(TrdSegmentR,1)       //TrdSegmentR
#pragma omp threadprivate(fgIsA)
};



/// TRDTrackR structure
/*!
 \author vitali.choutko@cern.ch

*/

class TrdTrackR {
static char _Info[255];
public:
  unsigned int   Status;  ///< statusword
  float Coo[3];  ///< trac coo (cm)
  float ErCoo[3];  ///< err to coo
  float Phi;     ///<  phi (rads)
  float Theta;   ///< Theta
  float Chi2;   ///<  Chi2
  int   Pattern;  ///< pattern no
  float Q;   ///< Charge Magnitude (0 if not calculated)
  short int Charge[10];  ///<  Ordered Charge guess from pdf functions (0 == electrons, 1,2,3,4,5,6,7,8,9 are protons to F)
  float ChargeP[10];   ///<  log(likelihood) from pdf functions
  protected:
  vector<int> fTrdSegment;
  static float ChargePDF[10030];
#ifdef __ROOTSHAREDLIBRARY__
//#pragma omp threadprivate (ChargePDF)
#endif
  public:
  /// access function to TrdSegmentR objects used
  /// \return number of TrdSegmentR used
  int NTrdSegment()const {return fTrdSegment.size();}
  /// access function to TrdSegmentR objects used
  /// \param i index of fTrdSegment vector
  /// \return index of TrdSegmentR object in collection or -1
  int iTrdSegment(unsigned int i){return i<fTrdSegment.size()?fTrdSegment[i]:-1;}
  /// access function to TrdSegmentR collection
  /// \param i index of fTrdSegment vector
  /// \return pointer to TrdSegmentR object or 0
  TrdSegmentR * pTrdSegment(unsigned int i);
  /// create pdf function from file and/or TDV
      static bool CreatePDF(const char *fnam);
  /// compute charge & likelihoods
  void ComputeCharge(double betacorr);
  TrdTrackR(AMSTRDTrack *ptr);
  TrdTrackR(const TrdTrackR & o);
  TrdTrackR(){};
  /// \param number index in container
  /// \return human readable info about TrdTrackR
  char * Info(int number=-1){
    int np=0;
    for(int i=0;i<NTrdSegment();i++)np+=pTrdSegment(i)->NTrdCluster();
    int nph=0;
    for(int i=0;i<NTrdSegment();i++){
     for(int k=0;k<pTrdSegment(i)->NTrdCluster();k++){
       if(pTrdSegment(i)->pTrdCluster(k)->HMultip)nph++;
     }
    }
    sprintf(_Info,"TrdTrack No %d Coo=(%5.2f,%5.2f,%5.2f)#pm((%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g N_{Hits,HHits}=%d,%d Q=%7.2g QC %d/%d %7.2g/%7.2g ",number,Coo[0],Coo[1],Coo[2],ErCoo[0],ErCoo[1],ErCoo[2],Theta,Phi,Chi2,np,nph,Q, Charge[0], Charge[1],exp(-ChargeP[0]),exp(-ChargeP[1]));
  return _Info;
  }
  friend class AMSTRDTrack;
  friend class AMSEventR;
  virtual ~TrdTrackR(){};
ClassDef(TrdTrackR,4)       //TrdTrackR
#pragma omp threadprivate(fgIsA)
};



/// Level1 trigger structure
/*!
 \author e.choumilov@cern.ch

*/

class Level1R {

static char _Info[255];
public:
 bool IsEcalFtrigOK()const {return (EcalFlag/10)>=2;}
 bool IsEcalLev1OK()const {return (EcalFlag%10)>1;}
public:
  int   PhysBPatt;   ///< 8 lsbits-> pattern of LVL1 sub-triggers ("predefined physics" branches)
                    /*!<        LVL1 is (masked) OR of above branches.
		           The list of predefined branches(preliminary):                                         \n                    \n
		        bit1: unbiased TOF-trig(i.e. FTC= z>=1)  \n
			bit2: Z>=1(FTC+anti) "protons"           \n
			bit3: Z>=2(FTC & BZ) ions                \n
			bit4: SlowZ>=2 (FTZ) slow ions           \n
			bit5: electrons(FTC & FTE)               \n
			bit6: gammas(FTE & ECLVL1("shower angle")\n
		        bit7: unbECAL(FTE)                       \n
			bit8: External                           \n
		    */
  int   JMembPatt; ///< 16 lsbits -> pattern of trig.system members checked by particular physics branch decision logic.
                    /*!<        The bits assignment :                              \n
		        bit0:  FTC (TOF FT z>=1)                                   \n
			bit1:  LUT-1 decision (used by FTC as OR with LUT-2)       \n
			bit2:  LUT-2 .......  (used by FTC as OR with LUT-1)       \n
			bit3:  same as  LUT-1, but for subset of central counters  \n
			bit4:  same as  LUT-2, but for subset of central counters  \n
			bit5:  FTZ (TOF SlowFT z>=2)                               \n
			bit6:  FTE (ECAL FT, OR or AND of desisions in 2 proj.)    \n
			bit7:  ACC0 (bit set if Nacc < Nthr_1)                     \n
			bit8:  ACC1 (bit set if Nacc < Nthr_2)                     \n
			bit9:  BZ-flag (LUT-3 decision on z>=2 4-layers config.)   \n
			bit10: ECAL FT using AND of 2 projections decisions        \n
			bit11: ............. OR ...........................        \n
			bit12: ECAL LVL1("shower angle") using AND ........        \n
			bit13: ECAL LVL1...................... OR .........        \n
			bit14: EXT-trigger_1                                       \n
			bit15: EXT-trigger_2                                       \n
		    */
  int   AuxTrigPatt;///< 5 lsbits-> pattern of Aux.trig.members(LA-0/LA-1/Reserv/DSP/InternTrigger)
  int   TofFlag1;   ///< FTC(z>=1) LayersPatternCode, <0:noFTC,=0:4L,(1-4):1missLnumb,5:1+3,6:1+4,7:2+3,8:2+4,9:1+2,10:3+4,(11-14):1,..4
  int   TofFlag2;   ///< BZ(z>=2) LayersPatternCode, <0:noBZ,=0:4L,(1-4):1missLnumb,..... as above
  int   TofPatt1[4]; ///< 4-layers TOF paddles pattern for FTC(z>=1)(separately for each side)
  int   TofPatt2[4]; ///< the same for BZ(z>=2)(separately for each side):

                    /*!<
                                                       1-10 bits  Side-1  \n
                                                       17-26      Side-2  \n
                   */
  int   AntiPatt;   ///< Antipatt:(1-8)bits->sectors in coincidence with FastTrigger
  int   EcalFlag;   ///< =MN, where
                    /*!<
		          M=0/1/2/3->FTE(energ=multipl_High):no/no_in1prj@2prj-requir/FTE&in1proj/FTE&in2proj) \n
                          N=0/1/2/3->LVL1(ShowerAngleSmall):Unknown/in0proj_whenFTEok/in1proj/in2proj       \n
                    */
  unsigned short int EcalPatt[6][3];///< EC DynodesPattern for 6 "trigger"-SupLayers(36dynodes use 36of(3x16)lsbits)
  float EcalTrSum; ///< EC-energy trig.sum(Gev, MC only)
  float LiveTime;  ///< Fraction of "nonBusy" time;  May be 0 at the bef of run using  AMSEventR::LiveTime() isrecommended instead
  float TrigRates[19]; ///< TrigCompRates(Hz):FT,FTC,FTZ,FTE,NonPH,LVL1,L1M1-M8,CPmx,BZmx,ACmx,EFTmx,EANmx
  unsigned int TrigTime[5];///< [0]-Tcalib.counter,[1]-Treset.counter,[2]-[3]-0.64mks Tcounter(32lsb+8msb), [4]-time_diff in mksec

  int GetGPSTime(unsigned int & gps_sec, unsigned int & gps_nsec) ; // return 0 if success

  Level1R(){};
/// \param level -1 any tof level
///  \return true if any tof related level1 was set
 bool isTOFTrigger(int level=-1);
 void RestorePhysBPat();
 
 int RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt);///< Rebuid JMembPatt/PhysBPatt according to Flight tr.setup(mainly for old MC root-files)
/// \return 1/0 (was physical/unbiased trigger)
 
  Level1R(Trigger2LVL1 *ptr);
  /// \param number index in container
  /// \return human readable info about Level1R
  char * Info(int number=-1);



  virtual ~Level1R(){};
ClassDef(Level1R,9)       //Level1R
#pragma omp threadprivate(fgIsA)
};


/// Level3 trigger structure
/*!
 \author vitali.choutko@cern.ch

*/

class Level3R {
static char _Info[512];
public:
  int   TOFTr;  ///< TOF Trigger
                /*!<
                                                      \n -1 if rejected by matrix trigger,
                                                      \n   0 Too Many Hits
                                                      \n   1  tof 0  1 cluster or 2 cluster
                                                      \n   2  tof 1  -----
                                                      \n   4  tof 2  -----
                                                      \n   8  tof 3  -----
                                                      \n   16  tof 0  2 cluster
                                                      \n   32  tof 1  -----
                                                      \n   64  tof 2  -----
                                                      \n   128  tof 3  -----
                           */
  int   TRDTr;   ///<  TRD Trigger
                   /*!<
                                                     \n   0  Nothing found
                                                     \n   bit 0:  Segment x found
                                                     \n   bit 1:  segment y found
                                                     \n   bit 2: too many hits found
                                                     \n   bit 3: high gamma event found
                   */

  int   TrackerTr;   ///< Tracker Trigger
                   /*!<
                            \n 0  - initial state
                            \n 1  - No Tr Tracks found
                            \n 2  - Too Many Hits in Tracker
                            \n 3  - Positive Rigidity found
                            \n 4  - Ambigious Comb (A) found
                            \n 5  - Ambigious Comb (B) found
                            \n 6  - Negative Rigidity(Momentum) found
                            \n + 8   // Heavy Ion (Tracker)
                   */
  int   MainTr;   ///< Main Trigger
                   /*!<
                            \n bit  0 No Tr Tracks found
                            \n bit  1 Too Many Hits in Tracker
                            \n bit  2 Too Many Hits in TRD
                            \n bit  3 Too Many Hits in TOF
                            \n bit  4 No TRD Tracks found
                            \n bit  5 Upgoing event found
                            \n bit  6 No TOF Time Information found
                            \n bit  7 Positive Rigidity(Momentum) found
                            \n bit  8 Ambigious Comb (A) found
                            \n bit  9 Ambigious Comb (B) found
                            \n bit  10  Negative Rigidity(Momentum) found
                            \n bit  11  High Gamma (TRD)
                            \n bit  12   Heavy Ion (Tracker)
                            \n bit  13 Prescaled event
			    \n bit  14 No ECAL activity(>=MIP) found
			    \n bit  15 ECAL EM-object found(or Eectot>20gev)
			    \n bit  16 ECAL track found(line fit chi2x,chi2y ok)
			    \n bit  17 ECAL track match to TOF/TRD path
                  */
  int   Direction;  ///< Particle Direction -1 Up 0 Uknown 1 Down
  int   NTrHits;    ///< Number Tracker Hits
  int   NPatFound;   ///< Number "Tracks" found
  int   Pattern[2];  ///<   Pattern no
  float Residual[2]; ///< Aver Residual (cm)
  float Time;       ///< Algorithm Time (sec)
  float ELoss;      ///< Tracker Aver energy loss
  int   TRDHits;    ///< trd hits no
  int   HMult;      ///< trd high energy hits (>6 kev) no
  float TRDPar[2];  ///< trd track tan x,y if any
  int   ECemag;   ///< ECelectromagn(-1/0/1=nonEM/noECactivity)/EM)
  int   ECmatc;   ///< EC-TOF/TRD match(-1/0/1=noMatch/noECtrack/Match)
  float ECTOFcr[4];  ///<Xcr/Ycr/TangX/TangY of ECtrack with bottom  plane of Tracker(when ECtrack was found)

  Level3R(){};
  Level3R(TriggerLVL302 *ptr);
  /// \param number index in container
  /// \return human readable info about Level3R
  char * Info(int number=-1){
    char infol[500];
    strcpy(infol,"");
    for(int k=0;k<18;k++){
     if( MainTr & (1<<k)){
      switch (k) {
       case 0:
        strcat(infol,"NoTrTracks ");
        break;
       case 1:
        strcat(infol,"TManyTrHits ");
        break;
       case 2:
        strcat(infol,"TManyTrdHits ");
        break;
       case 3:
        strcat(infol,"TManyTofHits ");
        break;
       case 4:
        strcat(infol,"NoTrdTracks ");
        break;
       case 5:
        strcat(infol,"Upgoing ");
        break;
       case 6:
        strcat(infol,"NoTofTime ");
        break;
       case 7:
        strcat(infol,"Rig+ ");
        break;
       case 8:
        strcat(infol,"AmbA ");
        break;
       case 9:
        strcat(infol,"AmbB ");
        break;
       case 10:
        strcat(infol,"Rig- ");
        break;
       case 11:
        strcat(infol,"High#gamma ");
        break;
       case 12:
        strcat(infol,"HeavyIon ");
        break;
       case 13:
        strcat(infol,"Prescaled ");
        break;
       case 14:
        strcat(infol,"NoEcal ");
        break;
       case 15:
        strcat(infol,"EcalEmag ");
        break;
       case 16:
        strcat(infol,"EcalTrack ");
        break;
       case 17:
        strcat(infol,"EcalTrackMatch ");
        break;
      }
     }
    }
    sprintf(_Info,"TrLevel3: %s",infol);
  return _Info;
  }
  virtual ~Level3R(){};
ClassDef(Level3R,1)       //Level3R
#pragma omp threadprivate(fgIsA)
};






/// Tof Beta (Velocity/c) structure
/*!
 \author vitali.choutko@cern.ch

*/

class BetaR {
 public:
  unsigned int       Status;   ///< status word  4 - ambig
  int       Pattern;   ///< beta pattern \sa beta.doc
 ///
  float     Beta;     ///< velocity
  float     BetaC;    ///< corrected velocity (taking into account true velocity < 1)
  float     Error;   ///< error (1/beta)
  float     ErrorC;  ///< error (1/betac)
  float     Chi2;    ///<chi2 of beta fit(time)
  float     Chi2S;   ///<chi2 of beta fit(space)
  float     Len[4];  ///<the signed distance along the track from plane Z=0 to the tof cluster
protected:
  int   fTrTrack;    ///< index to TrTrack used
  vector<int> fTofCluster; ///< indexes of TofClusterR's used
public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to TofClusterR objects used
  /// \return number of TofClusterR used
  int NTofCluster()const {return fTofCluster.size();}
  /// access function to TofClusterR objects used
  /// \param i index of fTofCluster vector < NTofCluster()
  /// \return index of TofClusterR object in collection or -1
  int iTofCluster(unsigned int i){return i<fTofCluster.size()?fTofCluster[i]:-1;}
  /// access function to TofClusterR objects
  /// \param i index of fTofCluster vector  < NTofCluster()
  /// \return pointer to TofClusterR object  or 0
  TofClusterR * pTofCluster(unsigned int i);
  /// MC derived correction  function to TofClusterR objects
  /// \param datamc 0- MC 1-Data
  /// \return dedx amplification factor for the give benta
       double GetTRDBetaCorr(int datamc=0);

   BetaR(){};
   BetaR(AMSBeta *ptr);
   friend class AMSBeta;
   friend class AMSEventR;
   virtual ~BetaR(){};
   ClassDef(BetaR,3)         //BetaR
#pragma omp threadprivate(fgIsA)
};

/// TofBetaPar structure
/*!
 * IHEP Recontruction New Beta(BetaH) information is recorded by TofBataPar
 * \author qyan@cern.ch
*/
//////////////////////////////////////////////////////////////////////////
class  TofBetaPar: public TObject{
public:

  /// Status of BetaH.....
  int    Status;
  /// Number of ClusterHs  (Pos Matched by Tracker or TRDTracker or TofTrack-Self) 
  int    SumHit;
  /// Number of ClusterHs for Beta Fit
  int    UseHit;
  /// ClusterH Pattern for iLayer(lay missing, only sideN, only sideP...)
  int    Pattern[4];
  /// Signed Distance along the Track from plane Z=0 to the Tof iLayer
  float  Len[4];
  /// Time Measument for iLayer(return 0 means Layer missing or Time bad)/ns
  float  Time[4];
  /// Time Measument Eror for iLayer
  float  ETime[4];

//--Fit Result
  /// Fit Beta T0 //Time[il]=Len[il]/(Beta*Cvel)+T0
  float  T0;
  /// BetaH Fit Beta Value
  float  Beta;
  /// Convert Beta Value(BetaC Convert fabs(Beta)>1 to <1 for Mass Calculation)(Vitaly alogrithm) 
  float  BetaC;
 /// Fitting Error on 1/Beta
  float  InvErrBeta;
  /// Fitting Error on 1/BetaC
  float  InvErrBetaC;
  /// Fitting Time Chis on Beta
  float  Chi2T;
  /// Fiting Time Residual for iLayer
  float  TResidual[4];
  /// Fiting Coo Chis on Beta (TOF Counter Coo compare to Tracker in XY)
  float  Chi2C;
  /// Fiting Coo Residual
  float  CResidual[4][2];

//--Other Par
  /// Fiting Mass with Particle Charge and Rigidity
  float  Mass;
  /// Fit Mass Error
  float  EMass;

  /// TOF iLayer CosZ
  float CosZ[4];
  /// TOF iLayer One Anode Q2 for 2Sides PMTs(TkCoo Attenuation Cor,No-Birk-Cor)//iLayer iSide
  float AQ2L[4][2];
  /// TOF iLayer One Dynode Q2 for 2Side 3PMTs(TkCoo Attenuation Cor,No-Birk-Cor) //iLayer iSide iPM
  float DQ2L[4][2][3];
  /// TOF iLayer Anode Estimate Energy dep(MeV) //TkCoo Attenuation Cor
  float AEdepL[4];
  /// TOF iLayer Dynode Estimate Energy dep(MeV) //TkCoo Attenuation Cor
  float DEdepL[4];
  /// TOF iLayer Q+SumQ for Different Pattern 
  map<int,float>QL[4];
  /// TOF QNLayer+Q+QRMS  
  map<int,pair<int, pair<float,float> > > QPar;
  
 
public:
  TofBetaPar(){Init();}
  const TofBetaPar& operator=(const TofBetaPar &right);
  /// Set Beta
  void SetFitPar(float _Beta,float _BetaC,float _InvErrBeta,float _InvErrBetaC,
                 float _Chi2T, float _T0);
  void CalFitRes();//calculate residual
  void Init();
  ClassDef(TofBetaPar,4);//
};

/// Tof BetaH structure
/*! 
 * IHEP New TOF Beta+Charge Measument BetaH TofRawSide->TofClusterH(only 1 TOF Counter)->BetaH+TofChargeH(New Calibration+Software)
 * Root Mode ReFit User Need One function:  TofRecH::ReBuild() 
 * Example of How to Use:  vdev/example/Tof_BetaH.C (AMSsoft)
 * include file:  #include "Tofrec02_ihep.h"
 * \author qyan@cern.ch
*/
class BetaHR: public TrElem{

 protected:
/// BetaH Data
  TofBetaPar       BetaPar;
 protected:
/// index to Matched TrTrack used
  int   fTrTrack;
/// index to Matched TrdTrack
  int   fTrdTrack;
/// index to Matched EcalShower
  int   fEcalShower;
/// indexes of TofClusterHR's used
  vector<int> fTofClusterH;
/// indexes of 4Layer TofClusterHR's used
  int   fLayer[4];
/// indexes of TofChargeHR
  int  fTofChargeH;

 public:

/** @name Constructors and Index Accessors
 * @{
 */
  BetaHR(){};
  BetaHR(AMSBetaH *ptr);
  BetaHR(TofClusterHR *phith[4],TrTrackR* ptrack,TrdTrackR *trdtrack,EcalShowerR *show, TofBetaPar betapar);
  virtual ~BetaHR(){};

 public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
 /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();
  /// access function to TrdTrackR object Matched
  /// \return index of TrdTrackR object in collection or -1
  int iTrdTrack()const {return fTrdTrack;}
  /// access function to TrdTrackR object Matched
  /// \return pointer to TrdTrackR object or 0
  TrdTrackR * pTrdTrack();
  /// access function to EcalShowerR object Matched
  /// \return index of EcalShowerR object in collection or -1
  int iEcalShower()const {return fEcalShower;}
  /// access function to EcalShowerR object Matched
  /// \return pointer to EcalShowerR object or 0
  EcalShowerR * pEcalShower();
   
 
  /// access function to TofClusterHR objects used
  /// \return number of TofClusterHR used
  int NTofClusterH()const {return fTofClusterH.size();}
  /// access function to TofClusterHR objects used
  /// \return index of TofClusterHR object in collection or -1
  int iTofClusterH(unsigned int i){return i<fTofClusterH.size()?fTofClusterH[i]:-1;}
  /// access function to TofClusterHR objects used
  /// \return pointer to TofClusterHR object or 0
  TofClusterHR * pTofClusterH(unsigned int i);

  /// Get pointer to TOF BetaH iLayer(0-3) ClusterH /*return 0 if not exist*/
  TofClusterHR * GetClusterHL (int ilay);
  /// if TOF BetaH iLayer(0-3) ClusterH exists Return true
  bool           TestExistHL  (int ilay){return (ilay>=0&&ilay<4)&&(fLayer[ilay]>=0);}

  /// access function to TofChargeHR object
  int iTofChargeH();
  ///access function to TofChargeHR objects(this suggest to replace gTofCharge)
  TofChargeHR * pTofChargeH(); 
  /// Set TofChargeH index
  void setChargeHI(int iChargeH) {fTofChargeH=iChargeH;}
/**@}*/


/** @name General Information of TOF
 * @{
 */
  /// Number of All Fired TOF Counters in iLayer
  int            GetAllFireHL (int ilay){return BetaPar.Pattern[ilay]/1000;}
  /// Retrun True if Beta is Good Meaurement Candidate/// require match with track and 4Layer TOF Measument
  bool           IsGoodBeta()           {return ((fTrTrack>=0)&&(GetUseHit()==4));}
  /// Retrun True if TkTrack And TOF Geometry Match
  bool           IsTkTofMatch()         {return fTrTrack>=0;}
  /// Return Beta TOF-Layer Pattern
  /// \return 4144: TOF-Layer0,2,3 Used For Beta-Fit, not Use Layer1. 4: Two-Side Time Good && Used, 1: Side-N Good, 2: Side-P Good, 3: Two-Side-Good(but Bad Time) 0: BAD
  int            GetBetaPattern();
  /// Return BetaH Build Type
  /// \return 1:Tracker-BetaH 2:Trd-BetaH 3:Ecal-BetaH 4:TOF-StandAlone(4Layer)-BetaH 5:TOF-StandAlone(3Layer)-BetaH
  int            GetBuildType();
  /// Return True if TOF ilay is Used For Beta-Fit
  bool           IsBetaUseHL(int ilay)  {return (BetaPar.Pattern[ilay]%10==4);}
  /// Return True if BetaH Cluster is Isolation Fire Counter
  /// \return 0 if neighbor Counter Fire, idis distance to  Central Counter
  /*!
   * @param[in] idis  distance to BetaH used TOF Counter, idis=3 => Judge if there is any of Left 3Counter or right 3Counter fired
   */
  bool           IsIsolationHL (int ilay,int idis=1){
        bool left=0;
        if((BetaPar.Pattern[ilay]/100%10)==0)left=1;
        else if((BetaPar.Pattern[ilay]/100%10)>idis)left=1;
        bool right=0;
        if((BetaPar.Pattern[ilay]/10%10)==0)right=1;
        else if((BetaPar.Pattern[ilay]/10%10)>idis)right=1;
        return (left&&right);
     }
  ///  Return BetaH ClusterH nearest Fired Counter Bar Number(exclude the BetaH Counter Used)
  ///  \return -1 if Not Found any other Cluster is lr direction //lr=-1 left direction search, lr=1 right direction search
  int           GetNearbyHLNo (int ilay,int lr)   {
      if(TestExistHL(ilay)==0)return -1;
      if(lr<0)return ((BetaPar.Pattern[ilay]/100%10)==0)?-1:GetClusterHL(ilay)->Bar-BetaPar.Pattern[ilay]/100%10;
      else    return ((BetaPar.Pattern[ilay]/10%10)==0)? -1:GetClusterHL(ilay)->Bar+BetaPar.Pattern[ilay]/10%10;
  }

 public:
   /// Access BetaH All Data
  const TofBetaPar&  gTofBetaPar()      {return BetaPar;}
  /// Set BetaH All Data
  void  SetTofBetaPar(TofBetaPar tofpar){BetaPar=tofpar;}
  ///  Access PDF-Charge All Data
   TofChargeHR  gTofCharge()            {return TofChargeHR(this);}
/**@}*/   


/** @name BetaH  Data  Accessors
 * @{
 */
 public:
  /// Beta Value
  float GetBeta  () {return BetaPar.Beta; }
  /// Fitting Error on 1/Beta
  float GetEBetaV () {return BetaPar.InvErrBeta; }
  /// Good-Time Selected Layer Fitting Beta Value //return -1 if no Good-Time Counter (Serious BackSplash-Recommend to Use This Beta)
  float GetBetaS ();  
  /// Convert Beta Value(BetaC Convert fabs(Beta)>1 to <1 for Mass Calculation)
  float GetBetaC () {return BetaPar.BetaC;}
  /// Fitting Error on 1/BetaC
  float GetEBetaCV() {return BetaPar.InvErrBetaC; }
  /// Fitting Time Chis on Beta
  float GetChi2T()  {return BetaPar.Chi2T; }
  /// Fitting Coo Chis On Beta(Match with Tracker)
  float GetChi2C()  {return BetaPar.Chi2C; }
  /// Fitting Time Normlized Chis on Beta(Normalized)
  float GetNormChi2T();
  /// Fitting Coo  Normlized Chis On Beta(Match with Tracker Normalized)
  float GetNormChi2C();
  /// Number of ClusterHs  (Positon Matched by Tracker) 
  int   GetSumHit() {return BetaPar.SumHit;}
  /// Number of ClusterHs for Beta Fit  (more strict require counter 2side Ok)
  int   GetUseHit() {return BetaPar.UseHit;}
  /// Cluster Status
  int   GetStatus() {return BetaPar.Status;}
  /// ClusterH Pattern for iLayer (TOF Layer missing, only sideN, only has sideP,2side OK)
  int   GetPattern(int ilay){return BetaPar.Pattern[ilay];}
  /// Time Measument for iLayer(return 0 means Layer missing or Time bad)/ns
  float GetTime   (int ilay){return BetaPar.Time[ilay];}
  /// Time Measument Eror for iLayer
  float GetETime  (int ilay){return BetaPar.ETime[ilay];}
  /// Time Residual for iLayer
  float GetTResidual (int ilay){return BetaPar.TResidual[ilay];}
  /// Fit Beta T0 //Time[il]=Len[il]/(Beta*Cvel)+T0
  float GetT0()  {return BetaPar.T0;}
   /// Signed Distance along the Track from plane Z=0 to the Tof iLayer
  float GetTkTFLen(int ilay){return BetaPar.Len[ilay];}
  /// Coo Residula for iLayer ixy 0-X 1-Y (Distance with Tracker)
  float GetCResidual (int ilay,int ixy){return BetaPar.CResidual[ilay][ixy];}

 public:
  /// Mass Measument for Beta
  float GetMass()      {return BetaPar.Mass;}
  /// Err Mass
  float GetEMass()     {return BetaPar.EMass;}
/**@}*/


/** @name ReFit and Interpolation Function
 * @{
 */
  /// ReFit Mass using Rigidity+Charge
  /*!
   * @param[out] mass Fit Mass
   * @param[out] emass Fit Mass Error
   * @param[in] rig Rigidity
   * @param[in] charge Charge
   * @param[in] erigv   Error of 1/Rigidity
   * @param[in] isbetac (1)Using BetaC(ask Vitaly)  (0)Using Beta //For Fit Mass
   * @param[in] update 1:Copy Mass ReFit Result to BetaH; 0:No Copy
  */
  int  MassReFit(double &mass, double &emass,double rig=0,double charge=0,double erigv=0,int isbetac=0,int update=0);
   /// Beta Measurement ReFit Using different-TOF Layers
   /*!
    * @param[out] beta Fit Beta
    * @param[out] ebetav Fit Error 1/Beta
    * @param[in] pattern  mmmm: m=1 or 0 or 2,  1011 using Tof Lay0 2 3 for Beta Fit,exclude Lay1. 2022 force to use Time recover hit to Fit. -2: ReSelect Good-Layer To Fit(Good Coo+Time)  -1: Remove Max-dT(T deviation) Layer to Fit.
    * @param[in] mode =1(using same time weight)  =0(different time weight) for Fit
    * @param[in] update 1:Copy Beta ReFit Result to BetaH; 0:No Copy
    * @return    -1: Bad Time  0: Normal Measument
   */
  int  BetaReFit(double &beta,double &ebetav,int pattern=1111,int mode=1,int update=0);
   /// Beta Measurement ReFit Using different-TOF Layers
   /*
    * @param[out]  Fit betapar(Include New Fit-ChisT, Fit-ChisC, Fit-UseHit, Beta...)
    * @param[in]   pattern  mmmm: m=1 or 0 or 2, 1011 using TOF Lay0 2 3 for Beta Fit. 2022 include Time recover hit For Fit. -2: ReSelect Good-Layer To Fit(Good Coo+Time). -1: Remove Max-dT(T deviation) Layer to Fit
    * @param[in]   mode =1(using same time weight)  =0(different time weight) for Fit
    * @param[in] update 1:Copy Beta ReFit Result to BetaH; 0:No Copy
    * @return    -1: Bad Time  0: Normal Measument
   */
  int  BetaReFit(TofBetaPar &betapar,int pattern=-2,int mode=1,int update=0); 
  /// TOF BetaH Measurement Time Interpolation to posZ (Using Matched TrTrack if exist, otherwise Using Tof Self Track)
  /*!
   * @param[in] zpl Interpolate to Z position (Z=zpl)
   * @param[out] pnt BetaH's Track position at Z=zpl
   * @param[out] dir BetaH's Track direction at Z=zpl
   * @param[out] time TOF Time at Z=zpl(ns)
   * @return BetaH's Track Path length at Z=zpl
   */
  double  TInterpolate(double zpl,AMSPoint &pnt,AMSDir &dir,double &time, bool usetrtrack=true);
/**@}*/


/** @name TOF Charge Estimation And TOF-Dedx-Measure-Beta
 * @{
 */
///  iLay TOF Edep(MeV) /*Proton-Mip 1.67MeV */ From One PMT Estimate ///Attenuation ReCorr
  /*! 
   * @param[in] ilay TOF layer(0-3)
   * @param[in] pmtype 1-Anode 0-Dynode
   * @param[in] is  PMT Side: 0-NSide 1-PSide
   * @param[in] pm  PMTid For Each Side: Dyndoe 0~2  Anode=0
   * @return   =0  No Response or Bad PMT(mask)  <0 PMT Saturation
  */
   float GetEdepLPM  (int ilay,int pmtype, int is,int pm=0);
/// iLay TOF Q Or Q^2 Measurement from One PMT Estimate ///Adding BetaH PathLength+Birk+Beta+Attenuation ReCorr
   /*!
    * @param[in] ilay TOF layer(0-3)
    * @param[in] pmtype 1-Anode 0-Dynode
    * @param[in] is  PMT Side: 0-NSide 1-PSide
    * @param[in] pm  PMTid For Each Side: Dyndoe 0~2  Anode=0
    * @param[in] opt  DefaultQOpt Q Estimate, DefaultQ2Opt Q^2 Estimate
    * @return   =0  No Response or Bad PMT(mask)  <0 PMT Saturation
  */
   float GetQLPM(int ilay,int pmtype,int is,int pm=0,int opt=TofClusterHR::DefaultQOpt);
/// iLay TOF Edep(MeV)/*Proton-Mip 1.67MeV */ with Combinition of All Good PMTs //Attnuation ReCorr
   /*!
    * @param[in] ilay TOF layer(0-3)
    * @param[in] pmtype  2-Default(Best Between Anode and Dynode) 1-Anode  0-Dynode 
    * @param[in] pattern 111(P-PMT)111(N-PMT): Use All Good PMTs; 100(P-PMT)-110(N-PMT): PSide-PMT-No0~1 and NSide-PMT-No0 will not used for Calculation
    * @param[in] optw 1-Different weight for different PMTs. 0-Same weight for All PMTs
  */
  float GetEdepL(int ilay,int pmtype=2,int pattern=111111,int optw=1);
/// iLay TOF Q Or Q^2 Estimator with Combinition of All Good PMTs /// Adding BetaH PathLength+Birk+Beta+Attnuation ReCorr
   /*!
    * @param[in] ilay TOF layer(0-3)
    * @param[in] pmtype 2-Default(Best Between Anode and Dynode)  1-Anode  0-Dynode
    * @param[in] opt  DefaultQOpt Q Estimate, DefaultQ2Opt Q^2 Estimate
    * @param[in] pattern 111(P-PMT)111(N-PMT): Use All Good PMTs; 100(P-PMT)-110(N-PMT): PSide-PMT-No0~1 and NSide-PMT-No0 will not used for Calculation
    * @param[in] fbeta: 0-use BetaH beta Correction 1-no beta Correction !=0-use Fix beta Correction
    * @param[in] optw 1-Different weight for different PMTs. 0-Same weight for All PMTs
  */
  float GetQL(int ilay,int pmtype=2,int opt=TofClusterHR::DefaultQOpt,int pattern=111111,float fbeta=0,int optw=1);
/// TOF Q Mean
   /*!
    * @param[out] nlay Number of TOF Layers Used For Q-Measument
    * @param[out] qrms Q-RMS of TOF Layers Used
    * @param[in] pmtype 2-Default(Best Between Anode and Dynode)  1-Anode  0-Dynode
    * @param[in] opt  DefaultQOpt Q Estimate, DefaultQ2Opt Q^2 Estimate
    * @param[in] pattern -1: Remove Max-dQ(Q deviation)+BadPath-Length Layer; -2: Remove Max-Q+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; -12: Remove Max-Q Layer;  1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    * @param[in] fbeta: 0-use BetaH beta Correction 1-no beta Correction !=0-use Fix beta Correction
    * @return =0 No Good TOF Layer for measurement  >0 Q(or Q^2) value
    */
  float GetQ(int &nlay,float &qrms,int pmtype=2,int opt=TofClusterHR::DefaultQOpt,int pattern=-2,float fbeta=0);
/// TOF Beta Measument of 4Layers from TOF Dedx /*validate form beta~0.3~0.94*/
   /*!
    * @param[in] ilay TOF layer(0-3)
    * @param[in] charge Particle charge
    * @param[in] pmtype 2-Default(Best Between Anode and Dynode)  1-Anode  0-Dynode
    * @return =1 or =0.3 Edep Reach Charge unvalidate Boundary Region,=0 BetaH don't has This Layer, (0.3,0.94) normal measuremtnt
    */
  float GetQBetaL(int ilay,int charge,int pmtype=2);
  /// Return True if TOF-ilay Q PathLength is Good ///First Require Track-Match-TOF
  bool  IsGoodQPathL(int ilay);
  /// PDF-LikeLihood Integer Z Cover All Charge Z=1=>Z>26(PDF-Part Recommend To Use pTofChargeH() To Access All Data)
  /*!
    * @param[out] nlay Number of TOF Layers Used For Charge Z-Measument
    * @param[out] Z Likelihood-Prob
    * @param[in]  IZ  =0 Max-Prob Z, =1 Second-Max-Prob Z...
    * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
    * @return Charge Z (<0 Faild)
  */
   int GetZ(int &nlay,float &Prob,int IZ=0,int pattern=-10){return pTofChargeH()->GetZ(nlay,Prob,IZ,pattern);}
  /// Likehood Q-Estimator
 /*!
    * @param[out] nlay Number of TOF Layers Used For Q-Measument
    * @param[in]  pattern -1: Remove Big-dQ(From PDF)+BadPath-Length Layer; -10: Remove BadPath-Length Layer; -11: Remove Max-dQ(Q deviation) Layer; 1111: Using all 4Layers(if exist);1011: Using Lay0,2,3 exclude Layer; 1100: Using Up-TOF; 11 Using Down-TOF...
  */
   float GetLikeQ(int &nlay,int pattern=-10)     {return pTofChargeH()->GetLikeQ(nlay,pattern);}  
/**@}*/


/** @name TOF Geometry Information
 * @{
 */
  /// TOF ilay Edge(All Counter Dimension) x[3](xyz)[2](low high edge)
  void GetTOFLayEdge(int ilay,float x[3][2])             {return TOFGeom::GetLayEdge(ilay,x);}
  /// Global Coo Inside ilay TOF Region or Not
  /// z=0 don't use z information,  z!=0 include z judgment.
  bool IsInSideTOF(int ilay, float x, float y, float z=0){return TOFGeom::IsInSideTOF(ilay,x,y,z);}
  /// Global Coo Inside ilay TOF Overlap Region or Not
  /// nexcl=1 overlap=(Bar Overlap), nexcl=2 overlap=2*(Bar Overlap)
  bool IsInOverlap(int ilay,float x, float y,int nexcl=1){return TOFGeom::IsInOverlap(ilay,x,y,nexcl);}
  /// Find Global Coo nearest ilay TOF Counter
  /*!
   * @param[in] xyz Global Coo // z=0 don't use z information,  z!=0 include z judgment
   * @param[out] dis Distance from Global Coo to it's Nearest TOF Counter Central
   * @param[out] isinbar 1)if Global Coo is inside nearest Counter (0)Not
   * @return    Nearest Counter Bar Number
  */
   int FindNearBar(int ilay,float x, float y,float &dis,bool &isinbar,float z=0){return TOFGeom::FindNearBar(ilay,x,y,dis,isinbar,z);}
/**@}*/

  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("BetaHR Info");
  };

  void Print(int opt=0){
    _PrepareOutput();
    cout<<sout<<endl;
  }

  const char * Info(int number=-1){
    sprintf(_Info,"BetaH #beta=%6.3f TOFHitUse=%d Chi2C=%7.3g Chi2T=%7.3g EdepL(MeV)=(%5.2f,%5.2f,%5.2f,%5.2f)",GetBeta(),GetUseHit(),GetChi2C(),GetChi2T(),GetEdepL(0,1),GetEdepL(1,1),GetEdepL(2,1),GetEdepL(3,1) );
    return _Info;
  }

  std::ostream& putout(std::ostream &ostr = std::cout){
    _PrepareOutput(1);
    return ostr << sout  << std::endl;
  };

//---- 
  friend class AMSBetaH;
  friend class AMSEventR;
  ClassDef(BetaHR,14)
#pragma omp threadprivate(fgIsA)   
};
                                                       

#include <map>
#include <vector>

/// Subdetector Charge structure
/*!
 \author jorge.casaus@ciemat.es

*/
class ChargeSubDR{
 public:
  TString ID;                      ///< (Sub)Subdetector ID (AMSChargeTOF, AMSChargeTracker, AMSChargeTrackerInner....)
  unsigned int Status;             ///< Status word
  vector<unsigned short> ChargeI;  ///< Charge Indexes sorted in descending likelihood (0:e, 1:H, 2:He ...)
  vector<float> Lkhd;              ///< LogLikelihood for ordered charges
  vector<float> Prob;              ///< Probabilities for ordered charges
  float Q;                         ///< Charge value estimator (Truncated Mean, Median ...)

  /* Subdetector Specific Information */
  map <TString,float> Attr;  //< map with additional subdetector specific information */

 protected:
  int fParent;  //< Index in its corresponding container of the object used to reconstruct this charge

 public:
  /// access function to the reconstructed object used
  /// \return index of object in collection or -1
  int iParent()const {return fParent;}

  /// number of charge hypothesis stored in charge vectors
  int    getSize(){return ChargeI.size();}
  /// charge index (0:e, 1:H, 2:He ...) for the i'th most likely hypothesis
  int getChargeI(int i=0){return i<ChargeI.size()?ChargeI.at(max(i,0)):-1;}
  /// loglikelihood value for the i'th most likely hypothesis
  float getLkhd(int i=0){return i<Lkhd.size()?Lkhd.at(max(i,0)):0;}
  /// estimated probablility for the i'th most likely hypothesis
  float getProb(int i=0){return i<Prob.size()?Prob.at(max(i,0)):0;}
  /// print subdetector specific information (attributes and values)
  void dumpAttr(){for(map<TString,float>::iterator i=Attr.begin();i!=Attr.end();i++) cout<<i->first<<" = "<<i->second<<endl;}
  /// return value corresponding to a subdetector specific attribute
  float getAttr(TString attr);

  /// set function of the index of reconstructed object used
  void setParent(int iParent) {fParent=iParent;}

  ChargeSubDR(AMSChargeSubD* ptr);
  ChargeSubDR(){};
  friend class AMSEventR;
  friend class ChargeR;
  ClassDef(ChargeSubDR,1)
#pragma omp threadprivate(fgIsA)
};

/// AMS Charge structure
/*!
 \author jorge.casaus@ciemat.es

*/
class ChargeR{
 public:
  map<TString,ChargeSubDR> Charges; ///< map of the subdetector reconstructed charges
  unsigned int Status;              ///< Status word
  vector<unsigned short> ChargeI;   ///< Charge Indexes sorted in descending probability (0:e, 1:H, 2:He ...)
  vector<float> Lkhd;               ///< LogLikelihood for ordered charges
  vector<float> Prob;               ///< Probabilities for ordered charges

 protected:
  int fBeta;                        ///< index of BetaR used
  int fBetaH;                       ///< index of BetaRH 
 public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();
  /// access function to BetaHR object used
  /// \return index of BetaHR object in collection or -1
  int iBetaH()const {return fBetaH;}
  /// access function to BetaHR object used
  /// \return pointer to BetaHR object or 0
  BetaHR *pBetaH();

  /// most probable integer charge
  int Charge(){return max(1,int(ChargeI[0]));}
  /// number of ChargeSubD objects stored
  int getNCharges(){return Charges.size();}
  /// number of ChargeSubD objects used
  int getNUsed(){int Nused=0;for(map<TString,ChargeSubDR>::iterator i=Charges.begin();i!=Charges.end();i++)if(i->second.Status&32)Nused++;return Nused;}
  /// print the IDs of the ChargeSubD objects stored
  void dumpCharges(){for(map<TString,ChargeSubDR>::iterator i=Charges.begin();i!=Charges.end();i++) cout<<i->first<<endl;}
  /// number of charge hypothesis stored in charge vectors
  int    getSize(){return ChargeI.size();}
  /// charge index (0:e, 1:H, 2:He ...) for the i'th most probable hypothesis
  int getChargeI(int i=0){return i<ChargeI.size()?ChargeI.at(max(i,0)):-1;}
  /// loglikelihood value for the i'th most probable hypothesis
  float getLkhd(int i=0){return i<Lkhd.size()?Lkhd.at(max(i,0)):0;}
  /// estimated probablility for the i'th most probable hypothesis
  float getProb(int i=0){return i<Prob.size()?Prob.at(max(i,0)):0;}
  /// return a pointer to the ChargeSubDR object using its ID or 0 if not reconstructed
  ChargeSubDR *getSubD(TString ID);

  /// set function of the index of BetaR object used
  void setBeta(int iBeta) {fBeta=iBeta;}
  void setBetaH(int iBetaH){fBetaH=iBetaH;}
  ChargeR(AMSCharge* ptr);
  ChargeR(){};
  friend class AMSEventR;
  friend class AMSCharge;

  ClassDef(ChargeR,3)
};

#ifndef _PGTRACK_
/*!
  \class VertexR
  \brief Vertex class

   \sa vtx.h vtx.C
   \author juan.alcaraz@cern.ch
*/
class VertexR {
public:
  unsigned int   Status;      ///< Status
  float Mass;        ///< Rec mass at vertex (GeV)
  float Momentum;    ///< Momentum sum (GeV)
  float ErrMomentum; ///< Error in 1 / Momentum Sum (1/GeV)
  float Theta;       ///< Theta (rad)
  float Phi;         ///< Phi  (rad)
  float Vertex[3];   ///< reconstructed vertex (cm)
  int   Charge;      ///< Charge at vertex
  float Chi2;        ///< Chi2
  int   Ndof;        ///< Number of degrees of freedom
protected:
  vector<int> fTrTrack; ///< indexes of associated tracks
public:
  /// access function to TrTrackR objects used
  /// \return number of TrTrackR used
  int NTrTrack()const {return fTrTrack.size();}
  /// access function to TrTrackR objects used
  /// \param i index of fTrTrackR vector
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack(unsigned int i)const {return i<fTrTrack.size()? fTrTrack[i]:-1;}
  /// access function to TrTrackR objects
  /// \param i index of fTrTrackR vector
  /// \return pointer to TrTrackR object  or 0
  TrTrackR * pTrTrack(unsigned int i);
  VertexR(){};
  VertexR(AMSVtx *ptr);
  friend class AMSVtx;
  friend class AMSEventR;
  virtual ~VertexR(){};
  ClassDef(VertexR,3)         //VertexR
#pragma omp threadprivate(fgIsA)
};
#endif



/// AMS Particle structure
/*!
  AMS Particle types:
     - "Normal" Particle:
      -# Derived from ChargeR, BetaR and TrTrackR  objects
      -# Has   Charge, Rigidity, Velocity and DirCos properties set up
      -# Has fBeta,fCharge,fTrTrack set up
      -# Optionally has fTrdTrack set up in case TrdTrackR was found
      -# Optionally has fEcalShower set up in case EcalShowerR was found
      -# Optionally has fRichRing sett up in case Rich was used in velocity determination
     - Particle without TrTrackR:
      -# Derived from ChargeR, BetaR and optionally TrdTrack  objects
      -# Has rigidity set up to 100000000 gev
      -# Has fBeta,fCharge set up
      -# fTrTrack set to -1
      -# Optionally has fTrdTrack set up in case TrdTrackR was found
      -# Optionally has fRichRing setted up in case Rich was used in velocity determination
      -# Optionally has fEcalShower set up in case EcalShowerR was found
     - Particle based on EcalShower object:
      -# Derived from EcalShowerR (Momentum,DirCos);
      -# fBeta, fCharge, fTrTrack, fTrdTrack and fRichRing set to -1
      -# Velocity set to +-1 depend on shower ditection
      -# Two particles are in fact created with charg set to +-1
     - Particle based on VertexR (i.e. converted photon candidate or electron/positron ):
      -# fTrTrack set to -1
      -# fVertex set up
      -# Charge set to 0 or +-1
      -# Velocity may or may not be set depending on fBeta index

\author vitali.choutko@cern.ch

*/



class ParticleR {
private:
static char _Info[255];
public:
  unsigned int Status;   ///< status word (not really used at the moment)
  int   Particle;     ///< voted particle id a-la geant3 (not really reliable)
  int   ParticleVice; ///< nect-to voted particle id a-la geant3 (not really reliable)
  float Prob[2];      ///< Probability to be a particle Particle [0] or ParticleVice [1]
  float FitMom;       ///< Fitted Momentum to particle Particle
  float Mass;         ///< Mass (Gev/c2),signed means particles with abs(beta)>1 will have negative mass
                      /*!<
pmass definition \n
if(abs(beta)<1)mass = abs(momentum) * sqrt( 1/beta^2-1) \n
else mass=  -abs(momentum)*sqrt(beta^2-1) \n
*/
  float ErrMass;      ///< Error to Mass
  float Momentum;     ///< Momentum (Gev/c) (signed), means particle with negative charge will have negative momentum
                      /*!<
pmom definition  for fortran gurus

if( pbetap(i) > 0)then

  if(ptrackp(i)>0)then \n
     pmom(i)=ridgidity(ptrackp(i))*pcharge(i)*sign(beta(pbetap(i))) \n
  else \n
     pmom(i)=1000000*sign(beta(pbetap(i))) \n
  endif \n
else if(pecalp(i)>0)then \n
     pmom(i)=ecshen(pecalp(i)) \n
endif
*/
  float ErrMomentum;  ///< Error in momentum
  float Beta;         ///< Velocity (Tof + Rich if available)
  float ErrBeta;      ///< Error to Velocity
  float Charge;       ///< Abs(particle charge)
  float Theta;        ///< theta (1st[last] tracker plane) rad
  float Phi;          ///< phi (1st[last] tracker plane)   rad
  float ThetaGl;       ///< Theta in GTOD coo system (rad)
  float PhiGl;          ///< Phi in GTOD coo system (rad)
  float Coo[3];        ///< coo (1st[last] tracker plane) cm
  float Cutoff;       ///<  geomag cutoff in GeV/c, dipole model
  float TOFCoo[4][3]; ///< track extrapol point(x,y,z) in tof planes
  float TOFTLength[4]; ///< track length till tof planes crossing
  float AntiCoo[2][5]; ///< track extrapol in anti [dir][x/y/z/theta/phi]
  float EcalCoo[3][3]; ///< track extrapol in ecal (enter, cofg, exit)
  float TrCoo[9][3];  ///< track extrapol in tracker planes
  float TRDCoo[2][3];    ///< track extrapol in trd (center,top)
  float RichCoo[2][3];  ///< track extrapol in rich (radiator_pos, pmt_pos)
  float RichPath[2];    ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta=1
  float RichPathBeta[2]; ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta Beta
  float RichLength; ///< Estimated pathlength of particle within rich radiator (cm)
  int   RichParticles; ///< Estimated number of particles crossing the RICH radiator
  float Local[9];  ///< contains the minimal distance to sensor edge in sensor length units ;
  float TRDHLikelihood; ///< TRDH likelihood  (-log(elik/elik+plik))
  float TRDHElik; ///< normalized product of single layer electron probability
  float TRDHPlik; ///< normalized product of single layer proton probability
  float TRDCCnhit; ///< CC number of layers with edep above cut
  float TrdSH_E2P_Likelihood;  ///< TrdSCalib  e/p likelihood using TrdHTrack
  float TrdSH_He2P_Likelihood; ///< TrdSCalib He/p likelihood using TrdHTrack
  float TrdSH_E2He_Likelihood; ///< TrdSCalib e/He likelihood using TrdHTrack

  float CutoffS;    ///<  geomag cutoff in GeV/c, Stoermer model

  // Outputs of DoBacktracing
  int   BT_result;  ///< Result bits; see DoBacktracing
  int   BT_status;  ///< Status 1:Over cutoff, 2:Under cutoff, 3:Trapped
  float BT_glong;   ///< Galactic longitude(deg)
  float BT_glat;    ///< Galactic latitude (deg)
  float BT_RPTO[3]; ///< GTOD coordinates Rad(cm), Phi(rad), Theta(rad)
  float BT_time;    ///< Time of flight (sec) for the back tracing

protected:
  int  fBeta;          ///<index of  BetaR used
  int  fBetaH;          ///<index of  BetaHR used
  int  fCharge;        ///<index of  ChargeR used
  int  fTrTrack;      ///<index of  TrTrackR used
  int  fTrdTrack;     ///<index of  TrdTrackR used
  int  fTrdHTrack;     ///<index of TrdHTrackR used
  int  fRichRing;     ///<index of  RichringR used
  int  fEcalShower;   ///<index of  EcalShowerR used
  int  fVertex;       ///<index of  VertexR used
  int  fRichRingB;     ///<index of  RichringBR used
  static void _calcmass(float momentum,float emom, float beta, float ebeta, float &mass, float &emass);
  static void _build(double rid, double err, float charge,float beta, float ebeta,float &mass, float &emass, float &mom, float &emom);


public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();

  /// access function to BetaHR object used
  /// \return index of BetaHR object in collection or -1
  int iBetaH()const {return fBetaH;}
  /// access function to BetaHR object used
  /// \return pointer to BetaHR object or 0
  BetaHR * pBetaH();
   // Set BetaH index
  void setBetaH(int iBetaH){fBetaH=iBetaH;}

  /// access function to ChargeR object used
  /// \return index of ChargeR object in collection or -1
  int iCharge()const {return fCharge;}
  /// access function to ChargeR object used
  /// \return pointer to ChargeR object or 0
  ChargeR * pCharge();

  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();

  /// access function to TrdTrackR object used
  /// \return index of TrdTrackR object in collection or -1
  int iTrdTrack()const {return fTrdTrack;}
  /// access function to TrdTrackR object used
  /// \return pointer to TrdTrackR object or 0
  TrdTrackR * pTrdTrack();

  /// access function to TrdHTrackR object used
  /// \return index of TrdHTrackR object in collection or -1
  int iTrdHTrack()const {return fTrdHTrack;}
  /// access function to TrdHTrackR object used
  /// \return pointer to TrdHTrackR object or 0
  TrdHTrackR * pTrdHTrack();

  /// access function to RichRingR object used
  /// \return index of RichRingR object in collection or -1
  int iRichRing()const {return fRichRing;}
  /// access function to RichRingR object used
  /// \return pointer to RichRingR object or 0
  RichRingR * pRichRing();

  /// access function to RichRingBR object used
  /// \return index of RichRingBR object in collection or -1
  int iRichRingB()const {return fRichRingB;}
  /// access function to RichRingBR object used
  /// \return pointer to RichRingBR object or 0
  RichRingBR * pRichRingB();

  /// access function to EcalShowerR object used
  /// \return index of EcalShowerR object in collection or -1
  int iEcalShower()const {return fEcalShower;}
  /// access function to EcalShowerR object used
  /// \return pointer to EcalShowerR object or 0
  EcalShowerR * pEcalShower();

  /// access function to VertexR object used
  /// \return index of VertexR object in collection or -1
  int iVertex()const {return fVertex;}
  /// access function to VertexR object used
  /// \return pointer to VertexR object or 0
  VertexR * pVertex();

///
int ReBuildTrdEcal(float DisMax=20, float DirMax=10, float DistX=1,float DistY=2,bool force=false); ///<  Rebuild particle if both Trd and EcalShower present
int ReBuildTrdTOF(float DisMax=20, float DirMax=10, float DistX=3.5,float DistY=3.5,bool force=false); ///<  Rebuild particle if both Trd and TOF present

  int Loc2Gl(AMSEventR* pev); ///< recompute ThetaGl,PhiGl;

  int DoBacktracing();  ///< Fill BT_glong,BT_glat etc.

 char * pType(){
   static char type[63];
   if(iTrTrack()>=0){
     strcpy(type,"Tr");
   }
   else strcpy(type,"");
      if(iBeta()>=0)strcat(type,"Tof");
      if(iTrdTrack()>=0)strcat(type,"Trd");
      if(iTrdHTrack()>=0)strcat(type,"TrdH");
      if(iRichRing()>=0)strcat(type,"Rich");
      if(iRichRingB()>=0)strcat(type,"RichB");
      if(iEcalShower()>=0)strcat(type,"Ecal");
      if(iVertex()>=0)strcat(type,"Vertex");
   return type;
  }
#ifdef _PGTRACK_
   /// Returns the extrapolation on the TrackerLayer J-scheme (1-9)
  float GetTrCooJ(int lay,int coo) const {
    return GetTrCoo(TkDBc::Head->GetJFromLayer(lay),coo);
  }
  /// Returns the extrapolation on the TrackerLayer OLD scheme (1-9)
  float GetTrCoo(int jlay,int coo) const {
    return TrCoo[jlay-1][coo];
  }
#endif
  /// \param number index in container
  /// \return human readable info about ParticleR
  char * Info(int number,AMSEventR *pev);
  ParticleR(){};
  ParticleR(AMSParticle *ptr, float phi, float phigl);
  friend class AMSParticle;
  friend class AMSEventR;
  virtual ~ParticleR(){};

  /// \return the average of the two RICH beta reconstruction, or 0 if there is only one or zero reconstructions returning a value
  double RichBetasAverage();
  /// \return the difference in the reconstructed beta between the two RICH reconstrution algorithm, of inf if one reconstruction is missing. The value returned is RichRingR::Beta-RichRingBR::Beta.
  double RichBetasDiscrepancy();
  /// Check Whether Particle Pass-Through iLayer TOF-Geometry
   /*!
   * @param[in]  ilay    TOF ilay(0-3)
   * @param[in]  pnt     reference point[cm](can be tk-or-shower position )
   * @param[in]  dir     reference dir      (can be tk-or-shower direction)
   * @param[out] tofpnt  interpolate to iLay TOF position
   * @param[out] disedge distance to the nearest TOF-geometry-edge, -1:outside TOF-geometry-edge
   * @return  Pass-Through Nearest TOF-BarId(0-7(9)), -1  Outside TOF Geometry Region
   */  
  static int  IsPassTOF(int ilay, const AMSPoint &pnt, const AMSDir &dir, AMSPoint &tofpnt, float &disedge);

  /// \return true if position at z=(center & top of the TRD) is inside the geometrical acceptance of the TRD, otherwise false.
  bool IsInsideTRD();

//----------------------------------------------------------------------------------------------------------
/// returns  Stoermer Rigidity cutoff multiply by ParticleR.Charge [GeV/c]:
 /// The method performs a linear interpolation of Gauss coefficients,
 /// finds location of Geomegnetic Poles and Dipole Center at event Utime ,
 /// Calculates Geomagnetic Coordinates in simple shifted tilted dipole model.
 double GetGeoCutoff(AMSEventR* pev);

//----------------------------------------------------------------------------------------------------------
 /// Stoermer Geomagnetic Cutoff
 /*!
 * The method performs a linear interpolation of Gauss coefficients,
 * finds location of Geomegnetic Poles and Dipole Center at event Utime ,
 * Calculates Geomagnetic Coordinates in simple shifted tilted dipole model.
 * 
 * @param[in]   Scut is the Stoermer Cutoff value (in GeV/c or GV )
 * @param[in]   if momOrR==0 returns ---> Stoermer Rigidity cutoff multiplied by ParticleR.Charge [GeV/c]:
 * @param[in]   if momOrR==1 returns ---> Stoermer Rigidity cutoff [GV] 
 * @param[in]   if sign ==  0 Particle sign given by ParticleR.Momentum   
 * @param[in]   if sign == +1 force the Particle sign to be positive
 * @param[in]   if sign == -1 force the Particle sign to be negative
 * @param[in]   amsdir   particle direction in AMS reference frame (theta-->pi==out-going; theta-->0==up-going)

 * @param[out]  -4 Error:  momOrR must be 0 or 1
 * @param[out]  -3 Error:  parameter "sign" must be 0 , +1 or -1 
 * @param[out]  -2 Error:  No  AMSEventR::Head()
 * @param[out]  -1 Error:  AMSEventR::GetGalCoo failure
 * @param[out]   0 Success
 * @param[out]   from 1 to 4 equivalent to AMSEventR::GetGalCoo output

 */
    int GetStoermerCutoff( double &Scut ,int momOrR, int sign, AMSDir amsdir );



//-----------------------------------------------------------------------------------------------------------

  ClassDef(ParticleR,16)       //ParticleR
#pragma omp threadprivate(fgIsA)
};

/// AntiMCCluster structure
/*!

   \author e.choumilov@cern.ch
*/
class AntiMCClusterR {
public:
  int   Idsoft;   ///< software id  (ask E.Choumilov for details)
  float Coo[3];   ///< coo (cm)
  float TOF;      ///< time of flight (sec)
  float Edep;     ///< energy dep (GeV)

  AntiMCClusterR(){};
  AntiMCClusterR(AMSAntiMCCluster *ptr);
  virtual ~AntiMCClusterR(){};
ClassDef(AntiMCClusterR,1)       //AntiMCClusterR
#pragma omp threadprivate(fgIsA)
};

#ifndef _PGTRACK_
/// TrMCCluster structure
/*!

   \author vitali.choutko@cern.ch
*/
class TrMCClusterR {
static char _Info[255];
public:
  int Idsoft;   ///< idsoft
               /*!<
                  Idsoft%10 layer
                  Idsoft/10)%100 ladder
                  Ifsoft/1000    sensor
               */
  int TrackNo;   ///< geant3 particle id or 555 if noise
  int Left[2];   ///< left strip no
  int Center[2];   ///< center strip no
  int Right[2]; ///< right strip no
  float SS[2][5];  ///< Strip amplitudes  (x,y)
  float Xca[3];    ///< local enter coo  cm
  float Xcb[3];    ///< local exit coo  cm
  float Xgl[3];    ///< global coo cm,center
  float Sum;       ///< total amplitude gev

  TrMCClusterR(){};
  TrMCClusterR(AMSTrMCCluster *ptr);
  /// \param number index in container
  /// \return human readable info about TrMCClusterR
  char * Info(int number=-1){
    sprintf(_Info,"TrMCCluster No %d PId=%d Coo=(%5.2f,%5.2f,%5.2f), Ampl(Mev)=%5.2f",number,TrackNo,Xgl[0],Xgl[1],Xgl[2],Sum*1000);
  return _Info;
  }
  virtual ~TrMCClusterR(){};
ClassDef(TrMCClusterR,1)       //TrMCClusterR
#pragma omp threadprivate(fgIsA)
};
#endif

/// TofMCCluster structure
/*!

   \author e.choumilov@cern.ch
*/
class TofMCClusterR {
public:
  int   Idsoft;  ///< software id  (ask E.Choumilov for details)
  int   ParentNo;///< parent no
  int   Particle;///< particle id
  float Coo[3];  ///< coo cm
  float TOF;     ///< time of flight of incident particle sec
  float Beta;    ///< velocity of incident particle
  float Edep;    ///< energy dep for photon convention(=EdepR*birksat) mev
  float EdepR;   ///< energy dep mev
  float Step;    ///< step length cm
  int   GetLayer(){return Idsoft/100-1;} ///< TOF Counter Layer Number 0-3
  int   GetBar()  {return Idsoft%100-1;} ///< TOF Counter Bar   Number 0-9
  TofMCClusterR(){};
  TofMCClusterR(AMSTOFMCCluster *ptr);
  virtual ~TofMCClusterR(){};
ClassDef(TofMCClusterR,3)       //TOFMCClusterRoot
#pragma omp threadprivate(fgIsA)
};

/// TofMCPmtHit structure
/*!

   \author qyan@cern.ch
*/
class TofMCPmtHitR {
public:
   int    Idsoft; ///< Idsoft  LBSP---L=Layer(0-3) B=Bar(0-9) S=Side(0-1) P=PMT(0-2)
   int    ParId;   ///< photon parent g4id 
   float  TimeG;   ///< time of photon in pmt (nsec)
   float  TimeT;   ///< time of photon delay in SC+LG (nsec)
   float  Ekin;    ///< photon energy (eV)
   float  Length;  ///< photon transmit length in SC+LG
   float  Pos[3];  ///< photon gen vetex pos
   float  Dir[3];  ///< photon gen vetex dir
   float  TimeP;   ///< transmit time in PMT 
   float  Amp;     ///< SE amp

   TofMCPmtHitR(){};
   TofMCPmtHitR(AMSTOFMCPmtHit *ptr);
   virtual ~TofMCPmtHitR(){};
ClassDef(TofMCPmtHitR,1)       //TofMCPmtHitR
#pragma omp threadprivate(fgIsA)
};


/// EcalMCHit structure
/*!

   \author e.choumilov@cern.ch
*/
class EcalMCHitR {
public:
  int   Idsoft;  ///< software id  (SSLLFFF,SS=SupLayer,LL=FiberLayer,FFF=FiberNumber)
  float Coo[3];  ///< coo cm
  float TOF;     ///<  time of flight sec
  float Edep;    ///< energy dep mev
  EcalMCHitR(){};
  EcalMCHitR(AMSEcalMCHit *ptr);
  virtual ~EcalMCHitR(){};
ClassDef(EcalMCHitR,1)       //EcalMCHitRoot
#pragma omp threadprivate(fgIsA)
};



/// TrdMCCluster structure
/*!

   \author k.scholberg@cern.ch
*/
class TrdMCClusterR {
public:
  int   Layer;             ///< layer 0(top) 19 (bottom) no
  int   Ladder;     ///< ladder  no
  int   Tube;   ///< tube no
  int   ParticleNo;   ///< particle id ala g3, (-) if secondary partilces
  float Edep;        ///<  energy dep gev
  float Ekin;        ///< part kin energy (gev)
  float Xgl[3];     ///< hit global coo(cm)
  float Step;        ///< step size (cm)

  TrdMCClusterR(){};
  TrdMCClusterR(AMSTRDMCCluster *ptr);
  virtual ~TrdMCClusterR(){};
ClassDef(TrdMCClusterR,1)       //TrdMCClusterR
#pragma omp threadprivate(fgIsA)
};


/// RichMCCluster structure
/*!

   \author Carlos.Jose.Delgado.Mendez@cern.ch
*/
class RichMCClusterR {
public:
  int   Id;            ///< Particle id, -666 if noise
  float Origin[3];     ///< Particle origin coo
  float Direction[3];  ///< Particle direction coo
  int   Status;        ///< *******)

                       /*!<
*******) For geant4 this value is 0. For geant 3 it has several meanings:

Cerenkov photon generated in radiator:

    ricstatus = 100*(mother of Cerenkov if secondary?1:0)+10*(number of
                reflections in mirror) + (photon suffered rayleigh
                scattering?1:0)

PMT noise:

    ricstatus = -1

Cerenkov photon generated in PMT window:

    ricstatus = -(2+100*(mother of Cerenkov if secondary?1:0))


Cerenkov photon generated in light guide:

    ricstatus = -(5+100*(mother of Cerenkov if secondary?1:0))


No Cerenkov photon:

    ricstatus = -(3+100*(mother of Cerenkov if secondary?1:0))


Downward charged particle crossing the radiator

    ricstatus = -7

NOTE: The information of the mother is only available if RICCONT=1 in
      the datacards
*/

  int   NumGen;        ///< Number of generated photons
  int   fRichHit;      ///< Pointer to detected hit
  RichMCClusterR(){};
  RichMCClusterR(AMSRichMCHit *ptr, int _numgen);
  virtual ~RichMCClusterR(){};
  ClassDef(RichMCClusterR,2)       // RichMCClusterR
#pragma omp threadprivate(fgIsA)
};



/// MCTrack structure

/*!
Contains radiation/absorption length ticknesses.
 Activated bt SCAN TRUE datacard
\author vitali.choutko@cern.ch
*/

class MCTrackR {
public:
float RadL;   ///< integrated radiation length
float AbsL;   ///< integrated nuclear absorption length
float Pos[3];  ///< x,y,z (cm)
char  VolName[5];  ///< Volume name

 MCTrackR(){};
 MCTrackR(AMSmctrack *ptr);
  virtual ~MCTrackR(){};
ClassDef(MCTrackR,1)       //MCTrackR
#pragma omp threadprivate(fgIsA)
};

/// MC particle structure
/*!

   \author vitali.choutko@cern.ch
*/
class MCEventgR {
static char _Info[255];
public:
static bool Rebuild;

  int Nskip;      ///< geant4 partcle create process: bit-1 IonInelastic(He+Deturon+Triton+Ion) bit-2 ProtonInelastic bit-3 ConvertPhoton
  int Particle;   ///< geant3 particle id
                  /*!<
          (geant3 only) \n
            Particle=g3pid+256 means heavy ion nonelstic
        scattering occured in for pid with dir & momentum at coo \n
        particle=-g3pid means secondary particle produced with
        dir&momentum at coo
*/
  int   trkID;    ///< G4 trk id. Not implemented for G3 (-2) is dummy value
  int   parentID; ///< parent G4 trk id. Not implemented for G3 (-2) is dummy value
  float Coo[3];   ///< coo (cm)
  float Dir[3];   ///< dir cos
  float Momentum;  ///< momentum (gev)
  float Mass;      ///< mass (gev)
  float Charge;    ///< charge (signed)
  int tbline;      ///< For TB generation mode postion id (line number of the file of positions)
  MCEventgR(){};
  MCEventgR(AMSmceventg *ptr);
  /// \param number index in container
  /// \return human readable info about McEventgR
  char * Info(int number=-1){
    sprintf(_Info,"McParticle No %d Pid=%d, TrkId=%i, ParentId=%i, Coo=(%5.2f,%5.2f,%5.2f) #theta=%4.2f #phi=%4.2f Momentum(Gev)=%7.2g Mass=%7.2g Q=%4.0f",number,Particle, trkID, parentID, Coo[0],Coo[1],Coo[2],acos(Dir[2]),atan2(Dir[1],Dir[0]),Momentum,Mass,Charge);
  return _Info;
  }
  virtual ~MCEventgR(){};
ClassDef(MCEventgR,3)       //MCEventgR
#pragma omp threadprivate(fgIsA)
};



///   AMS Event Root Class
/*!   Contains instance of HeaderR class and containers
    (stl vectors) of all other AMS related classes.
    Access to most HeaderR elements are public, while
     access to any vectors are via access function only. \n
     For any class XYZR  5 (five) functions are provided: \n
      - int nXYZ() - returns number of XYZ objects (via HeaderR counters, fast).  \n
      - int NXYZ() -  same as nXYZ but do actual read of vectors from file.
                  Not really recommended. Use nXYZ instead.\n
      - vector<XYZR> & XYZ() - returns reference to container. For advanced users only. \n
      - XYZ & XYZ(unsigned int i) - returns reference on ith element of vector of class XYZR. Recommended way of access. \n
      - XYZ * pXYZ(unsigned int i) - same as XYZ but returns pointer instead of reference. For advanced users only.  \n

    For some classes having references to other XYZR classes the following functions are (optionally) provided
      - int iXYZ(int i=0) const  returns index of i-th used XYZR element in
        corresponding  collection or -1. This element may be later accessed by functions described above.
      - XYZ* pXYZ(int i=0) return pointer to i-th used XYZR element or 0.
        For advanced users only.
      -  (optionally)  int NXYZ() const returns number of XYZR objects used

    Contains set of routines for (old fashioned) hbook like histograms manipulation: \n
- hbook(1,2,p)
- hlist
- hfit1
- hftetch
- hreset
- hdelete
- hfill
- hf(1,2,p)
- hcopy
- hdivide
- hsub
- hscale

    \sa ad.C \sa stlv.C \sa daqe.C
    \author vitali.choutko@cern.ch
*/
#include <map>
class AMSEventR: public  TSelector {
public:
static int gpsdiff(unsigned int time);///< Return gps-utc difference for the time time
static vector<unsigned int>RunType;
static vector<unsigned int>BadRunList;
static bool LoadFromTDV;
static unsigned int MinRun;
static unsigned int MaxRun;
static int ProcessSetup;
static bool isBadRun(unsigned int run);
static bool RunTypeSelected(unsigned int runtype);
protected:
void InitDB(TFile *file); ///< Read db-like objects from file
bool InitSetup(TFile* file,char *name, uinteger time); ///< Load AMSRootSetup Tree
bool UpdateSetup(uinteger run);  ///< Update RootSetup for the new Run
class Service{
public:
 TFile *            _pOut;
TDirectory *_pDir;
vector<string> fProcessed; //<  Files processed during ops
vector<string> fNotProcessed; //<  Files not processed during ops

typedef map<AMSID,TH1D*> hb1_d;
typedef map<AMSID,TH1D*>::iterator hb1i;
typedef map<AMSID,TH2D*> hb2_d;
typedef map<AMSID,TH2D*>::iterator hb2i;
typedef map<AMSID,TProfile*> hbp_d;
typedef map<AMSID,TProfile*>::iterator hbpi;
static  hb1_d hb1;
static  hb2_d hb2;
static  hbp_d hbp;
//#pragma omp threadprivate(hb1,hb2,hbp)
 TStopwatch         _w;
 unsigned long long       TotalEv;
 unsigned long long       BadEv;
 unsigned long long        TotalTrig;

Service():_pOut(0),_pDir(0),TotalEv(0),BadEv(0),TotalTrig(0){fNotProcessed.clear();fProcessed.clear();}
~Service(){
}
};
public:
static bool fgThickMemory;
static int fgThreads;
 static TFile* fgOutSep[32];
 static TDirectory* fgOutSepDir[32];
static int fgSeparateOutputFile;  //< 0 one file ; 1 one file different directories ;2 diff files
static TString gWDir;
long long Size();
     union if_t{
     float f;
     unsigned int u;
     int i;
     };
protected:
static TString Dir;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate(Dir)
#pragma omp threadprivate(gWDir)
#endif
Service  fService;
static Service*  pService;
static TBranch*  bStatus;
static TBranch*  bAll;
static TBranch*  bHeader;
static TBranch*  bEcalHit;
static TBranch*  bEcalCluster;
static TBranch*  bEcal2DCluster;
static TBranch*  bEcalShower;
static TBranch*  bRichHit;
static TBranch*  bRichRing;
static TBranch*  bRichRingB;
static TBranch*  bTofRawCluster;
static TBranch*  bTofRawSide;
static TBranch*  bTofCluster;
static TBranch*  bTofClusterH;
static TBranch*  bAntiRawSide;
static TBranch*  bAntiCluster;
static TBranch*  bTrRawCluster;
static TBranch*  bTrCluster;
static TBranch*  bTrRecHit;
static TBranch*  bTrTrack;
static TBranch*  bTrdRawHit;
static TBranch*  bTrdCluster;
static TBranch*  bTrdSegment;
static TBranch*  bTrdTrack;
static TBranch*  bTrdHSegment;
static TBranch*  bTrdHTrack;
static TBranch*  bLevel1;
static TBranch*  bLevel3;
static TBranch*  bBeta;
static TBranch*  bBetaB;
static TBranch*  bBetaH;
static TBranch*  bVertex;
static TBranch*  bCharge;
static TBranch*  bParticle;
static TBranch*  bAntiMCCluster;
static TBranch*  bTrMCCluster;
static TBranch*  bTofMCCluster;
static TBranch*  bTofMCPmtHit;
static TBranch*  bEcalMCHit;
static TBranch*  bTrdMCCluster;
static TBranch*  bRichMCCluster;
static TBranch*  bMCTrack;
static TBranch*  bMCEventg;
static TBranch*  bDaqEvent;
static TBranch*  bAux;
#ifdef __ROOTSHAREDLIBRARY__

#pragma omp threadprivate (bStatus,bHeader,bEcalHit,bEcalCluster,bEcal2DCluster,bEcalShower,bRichHit,bRichRing,bRichRingB,bTofRawCluster,bTofRawSide,bTofCluster,bTofClusterH,bAntiRawSide,bAntiCluster,bTrRawCluster,bTrCluster,bTrRecHit,bTrTrack,bTrdRawHit,bTrdCluster,bTrdSegment,bTrdTrack,bTrdHSegment,bTrdHTrack,bLevel1,bLevel3,bBeta,bBetaB,bBetaH,bVertex,bCharge,bParticle,bAntiMCCluster,bTrMCCluster,bTofMCCluster,bTofMCPmtHit,bEcalMCHit,bTrdMCCluster,bRichMCCluster,bMCTrack,bMCEventg,bDaqEvent,bAux)

#endif


static void*  vStatus;
static void*  vHeader;
static void*  vEcalHit;
static void*  vEcalCluster;
static void*  vEcal2DCluster;
static void*  vEcalShower;
static void*  vRichHit;
static void*  vRichRing;
static void*  vRichRingB;
static void*  vTofRawCluster;
static void*  vTofRawSide;
static void*  vTofCluster;
static void*  vTofClusterH;
static void*  vAntiRawSide;
static void*  vAntiCluster;
static void*  vTrRawCluster;
static void*  vTrCluster;
static void*  vTrRecHit;
static void*  vTrTrack;
static void*  vTrdRawHit;
static void*  vTrdCluster;
static void*  vTrdSegment;
static void*  vTrdTrack;
static void*  vTrdHSegment;
static void*  vTrdHTrack;
static void*  vLevel1;
static void*  vLevel3;
static void*  vBeta;
static void*  vBetaB;
static void*  vBetaH;
static void*  vVertex;
static void*  vCharge;
static void*  vParticle;
static void*  vAntiMCCluster;
static void*  vTrMCCluster;
static void*  vTofMCCluster;
static void*  vTofMCPmtHit;
static void*  vEcalMCHit;
static void*  vTrdMCCluster;
static void*  vRichMCCluster;
static void*  vMCTrack;
static void*  vMCEventg;
static void*  vDaqEvent;
static void*  vAux;

static TTree     * _Tree;
static TTree     * _TreeSetup;
static TTree     * _ClonedTree[32];   //max 32 threads
static TTree     * _ClonedTreeSetup[32]; //max 32 threads
static unsigned long long  _Lock;
static AMSEventR * _Head;
static int         _Count;
static int _NFiles;
static TTree* _pFiles;
static char filename[1024];
static vector<string> fRequested; //< Files Requested
static int         _Entry;
static int _EntrySetup;

struct MY_TLS_ITEM{
  std::map<unsigned int,int> theMap;
};

static MY_TLS_ITEM _RunSetup;
static char      * _Name;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate(_Tree,_Entry,_Head)
#pragma omp threadprivate(_TreeSetup,_EntrySetup,_RunSetup)
#endif
public:
static unsigned long long & Lock(){return
_Lock;}
 static AMSEventR* & Head()  {return _Head;}
 static char *  BranchName() {return _Name;}
// void SetBranchA(TTree *tree);   // don;t use it anymore use Init



public:
   /// hbook like 1d histgoram booking by int id \n parameres like in classical hbook1
static void hbook1(int id,const char title[], int ncha, float  a, float b);
   ///  few identical 1d histos booking in one call \n parameter howmany  number of histograms to be booked \n parameter shift    shift in id in subs hiistos
static void hbook1s(int id,const char title[], int ncha, float  a, float bi, int howmany=6,int shift=100000);
   ///  hbook like 2d histgoram booking by int id \n parameters like in classical hbook2
static void hbook2(int id,const char title[], int ncha, float  a, float b,int nchaa, float  aa, float ba);
   ///  few identical 2d histos booking in one call \n  parameter howmany  number of histograms to be booked \n parameter shift    shift in id in subs histos
static void hbook2s(int id,const char title[], int ncha, float  a, float b,int nchaa, float  aa, float ba,int howmany=5,int shift=100000);
   ///  hbook like profile histgoram booking by int id \n  parameters like in classical maphbook1
static void hbookp(int id,const char title[], int ncha, float  a, float b);
 /// expert only TH1D* accessor
 /// returns pointer to TH1D* by id
static  TH1D *h1(int id);
 /// expert only TH2D* accessor
 /// returns pointer to TH2D* by id
static  TH2D *h2(int id);
 /// expert only TProfile* accessor
 /// returns pointer to TH1D* by id
static  TProfile *hp(int id);
/// print histogram (eg from root session)
/// AMSEventR::hprint(id,"same");
static  void hprint(int id, char opt[]="");
/// list histos with title contains ptit or all if ptit==""
/// AMSEventR::hlist("xyz");
static  void hlist(char ptit[]="");
/// reset histogram by  id or all if id==0
static  void hreset(int id);
/// scale histogram id content by fac
static  void hscale(int id, double fac,bool sumw2=true);
/// fit 1d histogram by   func = "g" "e" "pN"
static  void hfit1(int id,char func[],double xmin=0,double xmax=0);
///  change dir to dir
static void chdir(const char dir[]="");
/// list current dir
static void hcdir(const char dir[]="");
/// list current dir
static void hjoin();
/// joins histos with the same id
static int g_t(){
#ifdef _OPENMP
return omp_get_thread_num( );
#else
return 0;
#endif
}
static void ldir(){cout<<" Current Dir: "<<Dir<<endl;}
/// fetch histos from TFile (to the dir dir)
static  void hfetch(TFile & f, const char dir[],int id,const char pat[]);
/// fetch histos from TFile file (to the dir file)
static  void hfetch(const char file[],int id=0);
static  void hfetch(const char file[],const char pat[]);
/// delete histogram by  id or all if id==0
static  void hdelete(int id);
/// copy hist id1 to id2
static void hcopy(int id1,int id2);
/// copy hist id1 for the dir dir []to id2 to current dir
static void hcopy(char dir[],int id1, int id2);
/// divide hist id1 by id2 and put the result into id3
static void hdivide(int id1,int id2,int id3);
/// sub hist id1 by id2 and put the result into id3
static void hsub(int id1,int id2,int id3);
/// add hist id1 by id2 and put the result into id3
static void hadd(int id1,int id2,int id3);
/// general fill for 1d,2d or profile
static  void hfill(int id, float a,float b, float w);
/// fast fill for 1d histos
static  void hf1(int id,float a, float w=1);
///  fill acc to cuts for 1d histos
static  void hf1s(int id,float a, bool cuts[], int ncuts,int icut, int shift=100000,float w=1);
static  void hf2s(int id,float a, float b,bool cuts[], int ncuts,int icut, int shift=100000,float w=1);
/// fast fill for profile histos
static  void hfp(int id,float a, float w);
/// fast fill for 2d histos
static  void hf2(int id,float a, float b,float w);
//
//----> user's functions for root files analysis:
bool GetEcalTriggerFlags(float Thr_factor[],float angle_factor[],int fastalgo,bool MCflag,bool debug,int flagtype);
bool GetTofTrigFlags(float HT_factor, float SHT_factor,string TOF_type, int TOF_max, int ACC_max);

 void    Init(TTree *tree);   ///< InitTree
 void CreateBranch(TTree *tree, int brs);
 void GetBranch(TTree *tree);
 void GetBranchA(TTree *tree);
 void SetCont();
 int & Entry(){return _Entry;}
 static TTree* & Tree()  {return _Tree;}
// static TTree* & ClonedTree()  {return _ClonedTree;}
 Int_t Fill();
 TFile* & OutputFile(){return (*pService)._pOut;};
 static TDirectory *OFD(){ return pService?(*pService)._pDir:0;}
#ifdef  __CINT__
public:
#elif  defined WIN32
public:
#else
protected:
#endif
//
//  TSelector functions

   /// System function called before starting the event loop.
   /// Initializes the tree branches.
   /// Optionally opens the output file via chain.Process("xyz.C+","outputfilename");
   /// calls user UBegin()  function
   void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);

   /// System function called when loading a new file.
   /// Get branch pointers.
   bool  Notify(){GetBranch(_Tree);return true;}


   void    SetOption(const char *option) { fOption = option; }
   void    SetObject(TObject *obj) { fObject = obj; }
   void    SetInputList(TList *input) {fInput = input;}
   TList  *GetOutputList() const { return fOutput; }
   /// System Function called at the end of a loop on the tree
   ///  optionally stores histos in a file \sa Begin
   /// Calls UTerminate();
   void    Terminate();
   virtual void    SlaveTerminate();

   /// Analysis kernel function.
   /// Entry is the entry number in the current tree.
   /// Should Not be modified by (Non)Advanced User\n
   /// \sa stlv.C
   /// \param entry - event no
   void ProcessFill(Long64_t entry){ProcessFill((int)entry);};
   void ProcessFill(int entry);
   bool Process(Long64_t entry){return Process((int)entry);};
   bool Process(int entry){
   try{
     if(ProcessCut(entry))ProcessFill(entry);
     return true;
   }
    catch (...)
    {
     cout <<"  exception caught "<<endl;
    return false;
    }
 }
public:




// TSelector user functions

   /// User Analysis function called before starting the event loop.
   /// Place to book histos etc
   /// \sa stlv.C
   virtual void UBegin();
   /// Fast User Selection function called event by event
   /// \sa stlv.C
   inline virtual bool UProcessStatus(unsigned long long status){return true;}
   /// User Selection function called event by event
   /// May return false as soon as a bad event is detected.\n
   /// \sa stlv.C
   /// \return false if error;
   inline virtual bool UProcessCut(){return true;}
   /// User Analysis function called event by event
   /// Place to fill in histograms etc
   /// \sa stlv.C
   virtual void UProcessFill();
   /// User Termination Function called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   /// \sa stlv.C
   virtual void UTerminate();


   /// Analysis Selection function.
   /// Entry is the entry number in the current tree.
   /// Read only the header to select entries.
   /// May return kFALSE as soon as a bad entry is detected.\n
   /// Should Not be modified by (Non)Advanced User\n
   /// \sa stlv.C
   /// \param entry - event no
   /// \return false if error;
   Bool_t ProcessCut(Long64_t entry){return ProcessCut((int)entry);}
   Bool_t ProcessCut(int entry){try{ return ReadHeader(entry);}catch(...){return false;}}
   ///  Reads Header
   /// \param Entry - event no
   /// \return false if error;
   bool ReadHeader(int Entry);

public:

//!  Status Word
/*!
    Contains bits:


 0-1  nParticle()

 2   ParticleR::iTrdTrack() !=-1

 3   ParticleR::iBeta() !=-1

 4   ParticleR::iTrTrack() !=-1

 5  ParticleR::iRichRing() !=-1

 6  ParticleR::iEcalShower() !=-1

 7  ParticleR::iVertex() !=-1

 8-9 nTrdTrack()

 10-12 nTofCluster()

 13-14  nTrTrack()

 15-16  nRichRing()

 17-18  nEcalShower()

 19-20  nVertex()

 21-22 nAntiCluster()

 23-25    ParticleR::Charge

 26-27 z=1 lvl1 number_of_tof_planes-1

 28-29  z>1 lvl1 number_of_tof_planes-1

 30   event has errors

 31   status not found



0
 0   npart
 1

1
 2   trd in part

2
 3   tof in part

3
 4   tr in part

4
 5   rich in part

5
 6   ecal in part

6
 7   vtx in part

7
 8  ntrd
 9

8
 10  ntof
 11
 12

9
 13  ntr
 14

10
 15  nrich
 16

11
 17  necal
 18

12
 19  nvtx
 20

13
 21  nanti
 22

14
 23  charge
 24
 25

15
 26 "z>=1" flag(Lvl1-info): [NtofLayers(max=4)-1] with signal above the low(z>=1) discr.threshold
 27

16
 28 "z>=2"  .............. : .................... ..................the high(z>=2) discr.threshold **)
 29

17
 30   event has errors if 1

18
 31   status not found if 1

19
 32  not prescaled event, prescaled event

20
 33  velocity <0; >0

21
 34  momentum <0; >0

22
 35 rig<8 ; <32 ; <128 ; >128 GV
 36

23
 37 ecalenergy <2; <8; <32; >32 gev
 38

24
 39 magnet current 0 ; <250 ; <400 ; >400  A
 40

24 (Perm Magnet)
 39 external planes :no; 1;9; 1&9
 40




25  trigger rate <500; <1000 ; <2000; >2000 Hz
 41
 42

26 trd hmult <1 <2 <4 >4
 43
 44


27
 geomagnetic latitude  (not yet defined,only reserved)
 0 < 10 degres  cos <0.1736
 1 < 30 degrees cos <0.5
 2 < 50 degrees cos < 0.766
 3 > 50 degrees cos > 0.766
 45
 46

28
 47  rich charge *4
 48
 49
 50
 51

29
 52   0   rich  agl 1 rich naf

30
 53  beta pattern    <2 tof hits
 54                  <3 tof hits
                     <4 tof hits
                      4 tof hits

31   mass/charge  <0.5 gev
55
56           <1.5
           <3.
           >3.
32
         track hit
57      <4
58      <5
59       <6
      <7
     <8
     <9
      9



33
      trd charge *2
60
61
62
63

correspondance between bits & status datacards words:

values 0123...etc --> 1 10 100 1000    (in fact base ESTA 36)
                      0 in datacards means select everything




N.B.

Due to lack of bits there is no "undefined state" bits,
so to ensure proper functionality one has to require
particle (datacards: ESTA 1=1110) for most other status bits

**) "z>=2" threshold is chosen to share He4 signal between 2 branches("z>=1" and "z>=2") !!!





 \author vitali.choutko@cern.ch

*/

  unsigned long long  fStatus;  ///<  Event Status
  HeaderR  fHeader;  ///<  Event Header \sa HeaderR
   ///
// Some functions for inter root
//
#ifdef __ROOTSHAREDLIBRARY__
#ifdef _PGTRACK_
  #include "root_methodsPG.h"
#else
  #include "root_methods.h"
#endif
#endif


bool Status(unsigned int bit);                  ///< \return true if corresponding bit (0-63) is set
bool Status(unsigned int group, unsigned int bitgroup);                  ///< \return true if corresponding bitgroup set for the group
int Version() const {return fabs(fHeader.Version/16)>465?(fHeader.Version>0?fHeader.Version/16:1023+fHeader.Version/16):fHeader.Version/4;} ///< \return producer version number
///
static AMSSetupR *  getsetup(){return AMSSetupR::gethead();} ///< \return RootSetup Tree Singleton
int OS() const {return fHeader.Version/16>465?fHeader.Version%16:fHeader.Version%4;}   ///< \return producer Op Sys number  (0 -undef, 1 -dunix, 2 -linux 3 - sun  12 linux 64bit )
///
unsigned int Run() const {return fHeader.Run;} ///< \return Run number
///
unsigned int Event() const {return fHeader.Event;} ///< \return Event number
///

   void UpdateGPS();  ///< Update GPSTime from getsetup(AMSSetupR *head);


   /*!
    \return 0  if succcess ; 1 if no gps time; 2 wrong time format; 3 int logic error; 4 gps time not valid; 5 no lvl1 block; 6 no coarse reg in lvl1 block
    output parameters gps_time_sec, gps_time_nsec
  */
   int  GetGPSTime( unsigned int &gps_time_sec, unsigned int &gps_time_nsec);

//--------------------------------------------------------------------------------------------------
///
        //!   Says if particle pass through the ISS Solar Array;
/*!
       input parameter: ipart= ParticleR index (default==0);
       output: AMSPoint ic== Coordinates of the particle crossing Solar Arrays in Station-Analysis-Coo.System [cm];
       returns: 0 if not in shadow ;  
                1 if in shadow; 
               -1 if no particle; 
               -2 if upgoing particle; 
               -3 if no Solar Array data; 
*/


        int isInShadow(AMSPoint & ic,int ipart=0); 
        int isInShadow(AMSPoint & ic, ParticleR &part);


//--------------------------------------------------------------------------------------------------
///
  //! Calculates the AMS solid Angle in Shadow and returns its value [sr];

  /*!
         input parameter: AMSfov ==AMS field of view in [deg];
  */
        double SolidAngleInShadow(double ANSfov0);


///
   //! Transforms xzy [cm] ISS coordinates into AMS reference system
/*!
       input parameter: iss[3] ==ISS coo. [cm] ; 
       output         : ams[3] ==AMS coo. [cm] (filled value);
*/
        void FromISStoAMS(double iss[3],double ams[3] );

//--------------------------------------------------------------------------------------------------
///
        //! Returns Maximum Rcutoff [GV] for Charge (+/-)1 particles in a fixed AMS field of view. 
        /*! return  0: if success;
	    retrun -1: if failure;	
            input parameters : AMSfov = AMS field of view in deg. ;
		             : degbin = binning precision (theta x phi) deg >> Example degbin=5 -> (5 x 5 )deg
            output           : cutoff[0] = Max. Rcut [GV] for negative particles (Z==-1); 
                               cutoff[1] = Max. Rcut [GV] for positive particles (Z== 1).
        */
        int GetMaxGeoCutoff( double AMSfov,double degbin ,double  cutoff[2]);



//--------------------------------------------------------------------------------------------------
///
       //! Return status of AMS Exposure-Time for each second
       /*! return 0: if sucess
           !=0: RIT second has problem(please not use this second for Flux Cal)
       */  
       int GetRTIStat();
       //! get AMS Exposure-Time RTI information for each second
       /*! return 0: if sucess
           !=0: RIT second has problem 
       */      
       int GetRTI(AMSSetupR::RTI & a);
       //!  AMS Exposure-Time RTI for each second off-rootfile mode
       /*! return 0: if sucess
           !=0: RIT second has problem 
           \param[in] xtime JMDC Time
       */
       static int GetRTI(AMSSetupR::RTI & a, unsigned int  xtime);

//--------------------------------------------------------------------------------------------------



///

//! Fast TDVR Element Accessor
/*!
   input  tdvname

          index of element to by accessed

    output value

    return 0 if success

           1 no such tdv found

           2 no valid tdv record found for the given event

           3 index outside of bounds

           4 tdv body is empty

           5 no setup tree found

*/
int GetTDVEl(const string & tdvname,unsigned int index, if_t &value);


         //! Yet Another SlowControlElement Accessor
        /*!
            based on AMSSetupR::SlowControlR::GetData

            int GetData(const char * elementname,unsigned int time, float frac, vector<float> &value , int imethod=1, const char *nodename="", int dt=-1, int st=-1);

         take time & frac from the current event; all others as defaults

          return -2  if no instance of AMSSetupR found

        */
int GetSlowControlData(char *ElementName, vector<float>&value,int method=1); ///<  SlowControlElement Accessor

float LiveTime(unsigned int time=0); ///< trying to get livetime from scalers map return -1 if error



void GTOD2CTRS(double RPT[3],double v, double VelPT[2]);
void CTRS2GTOD(double RPT[3],double v, double VelPT[2]);
void CTRS2GTOD(double theta, double phi, double v, double &vtheta, double &vphi);
void CTRS2GTOD(double theta, double phi, double v, float &vtheta, float &vphi){
  double vth = vtheta, vph = vphi;
  CTRS2GTOD(theta, phi, v, vth, vph);
  vtheta = vth; vphi = vph;
}

char * Time() const {time_t ut=fHeader.Time[0];return ctime(&ut);} ///< \return  Time

  //! Calculate distance between two RPT coordinates used in GetGalCoo
  static double get_coo_diff(double RPT[3], double r, double phi, double theta);

  /*!
  \brief Convert direction in AMS coordinates into Galactic coordinates

  \param[out] result bits (1<<0 =  1): STK used, (1<<1 = 2):INTL used,
			  (1<<2 =  4): TLE used, (1<<3 = 8):GTOD used,
			  (1<<4 = 16):CTRS used, (1<<5 =32):AMS-GPS coo.used,
			  (1<<6 = 64):GPS time used,
			  (1<<7 =128):GPS time corrected used
  \param[out] glong       Galactic longitude (degree)
  \param[out] glat        Galactic latitude  (degree)

  \param[in]  theta (rad) in ams coo system (pi: down-going 0: up-going)
  \param[in]  phi   (rad) in ams coo system
  \param[in]  use_att     1:Use LVLH,  2:Use INTL, 3: Use STK
  \param[in]  use_coo     1:Use TLE,   2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS
  \param[in]  use_time    1:UTCTime(), 2:AMSGPS time, 3:AMSGPS Time Corrected
  \param[in]  dt          time jitter (sec) for coordinates input
  \param[in]  out_type    1:Galactic coord. 2:Equatorial coord.(R.A. and Dec.)
                          3:GTOD coord.(use_att forced to 1)

  \retval     0 success
  \retval    -1 failure
  \retval     1 specified use_att  data not available; instead used TLE+LVLH
  \retval     2 specified use_coo  data not available; instead used TLE
  \retval     3 specified use_coo  data not reliable;  instead used TLE
  \retval     4 specified use_time data not available; instead used UTCTime()
  */
  int GetGalCoo(int &result, double &glong, double &glat, 
		double theta = 3.1415926,   double phi = 0,
		int use_att= 1, int use_coo = 4, int use_time= 3,
		double   dt= 0, int out_type= 1);


  /*!
  \brief Back trace charged particle into Galactic coordinates

  \param[out] result bits (1<<0 =  1): STK used, (1<<1 = 2):INTL used,
			  (1<<2 =  4): TLE used, (1<<3 = 8):GTOD used,
			  (1<<4 = 16):CTRS used, (1<<5 =32):AMS-GPS coo.used,
			  (1<<6 = 64):GPS time used,
			  (1<<7 =128):GPS time corrected used
  \param[out] status      0: errors (see retuned value)
                          1: Over cutoff, 
                          2: Under cutoff, 
                          3: Trapped 
                          4: Calculated velocity greater than c
                          5: External field model parameters out of validity range
                          6: External field model Parameters do not exist
                          7: All other cases where the program was exiting (max number of steps reached..)

  \param[out] glong       Galactic longitude (degree)
  \param[out] glat        Galactic latitude  (degree)
  \param[out] RPTO        GTOD coordinates Rad(cm), Phi(rad), Theta(rad)
  \param[out] TraceTime   Time of flight (sec) for the back tracing

  \param[in]  theta (rad) in ams coo system (pi: down-going 0: up-going)
  \param[in]  phi   (rad) in ams coo system
  \param[in]  Momentum    (GeV/c) Not signed
  \param[in]  Velocity    (=beta) Not signed
  \param[in]  Charge      Signed charge
  \param[in]  bt_method   0: Haino's  
                          1:MIB No ext.Field  
                          2:MIB ext.Field Tsyganenko 1996 (T96)  
                          3:MIB ext.Field Tsyganenko 2005 (T05) 
  \param[in]  Amass       atomic mass number(0:  e- , e+ ; 1:  p, anti-p ; 2: d , anti-d ; 3: He3...) 

  \param[in]  use_att     1:Use LVLH,  2:Use INTL, 3: Use STK
  \param[in]  use_coo     1:Use TLE,   2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS
  \param[in]  use_time    1:UTCTime(), 2:AMSGPS time, 3:AMSGPS Time corrected
  \param[in]  dt          time jitter (sec) for coordinates input
  \param[in]  out_type    1:Galactic coord. 2:Equatorial coord.(R.A. and Dec.)
                          3:GTOD coord.(use_att forced to 1)

  \retval     0 success
  \retval    -1 failure
  \retval    -2 error (if bt_method==2  particle UTime out of time limits for parameter file T96)
  \retval    -3 error (if bt_method==3  particle UTime out of time limits for parameter file T05)
  \retval    -4 error (if bt_method!=0) bt_method must be 1 || 2 || 3
  \retval     1 specified use_att  data not available; instead used TLE+LVLH
  \retval     2 specified use_coo  data not available; instead used TLE
  \retval     3 specified use_coo  data not reliable;  instead used TLE
  \retval     4 specified use_time data not available; instead used UTCTime()
  */
  int DoBacktracing(int &result, int &status, double &glong, double &glat,
		    double RPTO[3], double &TraceTime,
		    double theta, double phi,
		    double Momentum, double Velocity, int Charge, 
                    int bt_method= 0, int Amass=0 ,
		    int use_att= 1, int use_coo = 4, int use_time= 3,
		    double   dt= 0, int out_type= 1);

time_t UTime() const {return fHeader.Time[0];} ///< \return Unix GPS Time
int  UTCTime(double &time, double &err ) ;///< Best Known UTCTime for  curren tevent; return 0 if gps time was used, 2 if no gps time was found, 1 if gpstime is too far from jmdc time; err approximative error  
double UTCTime() const {return fHeader.UTCTime();}///< return best known event UTCTime (without gps)
      //! RunTagChecker
        /*!


         \param unsigned int bit to be checked
                bit 0  -> Bad MD ext alignment accuracy
                bit 32++ -> any bit
           \return
               0   bit not set
               1   bit  set
               2   unable to access root_setup
      */

int IsBadRun(const char * reason); ///< Call AMSSetupR::IsBadRun(string & reason, unsignet in time for the current event time
int  CheckRunTag(unsigned int bit);///<  Check run tag  

///
float Frac() const {return fHeader.Time[1]/1000000.;} ///< \return sec fraction
///
int   nEcalHit()const { return fHeader.EcalHits;} ///< \return number of EcalHitR elements (fast)
///
int   nEcalCluster()const { return fHeader.EcalClusters;} ///< \return number of EcalClusterR elements (fast)
///
int   nEcal2DCluster()const { return fHeader.Ecal2DClusters;} ///< \return number of Ecal2DClusterR elements (fast)
///
int   nEcalShower()const { return fHeader.EcalShowers;} ///< \return number of EcalShowerR elements (fast)
///
int   nRichHit()const { return fHeader.RichHits;} ///< \return number of RichHitR elements (fast)
///
int   nRichRing()const { return fHeader.RichRings;} ///< \return number of RichRingR elements (fast)
///
int   nRichRingB()const { return fHeader.RichRingBs;} ///< \return number of RichRingBR elements (fast)
///
int   nTofRawCluster()const { return fHeader.TofRawClusters;} ///< \return number of TofRawClusterR elements (fast)
///
int   nTofRawSide()const { return fHeader.TofRawSides;} ///< \return number of TofRawSideR elements (fast)
///
int   nTofCluster()const { return fHeader.TofClusters;} ///< \return number of TofClusterR elements (fast)
///
int   nTofClusterH()const { return fHeader.TofClusterHs;} ///< \return number of TofClusterHR elements (fast)
///
int   nAntiRawSide()const { return fHeader.AntiRawSides;} ///< \return number of AntiRawSideR elements (fast)
///
int   nAntiCluster()const { return fHeader.AntiClusters;} ///< \return number of AntiClusterR elements (fast)
///
int   nTrRawCluster()const { return fHeader.TrRawClusters;} ///< \return number of TrRawClusterR elements (fast)
///
int   nTrCluster()const { return fHeader.TrClusters;} ///< \return number of TrClusterR elements (fast)
///
int   nTrRecHit()const { return fHeader.TrRecHits;} ///< \return number of TrRecHitR elements (fast)
///
int   nTrTrack()const { return fHeader.TrTracks;} ///< \return number of TrTrackR elements (fast)
///
int   nTrdRawHit()const { return fHeader.TrdRawHits;} ///< \return number of TrdRawHitR elements (fast)
///
int   nTrdCluster()const { return fHeader.TrdClusters;} ///< \return number of TrdClusterR elements (fast)
///
int   nTrdSegment()const { return fHeader.TrdSegments;} ///< \return number of TrdSegmentR elements (fast)
///
int   nTrdTrack()const { return fHeader.TrdTracks;} ///< \return number of TrdTrackR elements (fast)
 int   nTrdHSegment()const { return fHeader.TrdHSegments;} ///< \return number of TrdHSegmentR elements (fast)
///
   int   nTrdHTrack()const { return fHeader.TrdHTracks;} ///< \return number of TrdHTrackR elements (fast)
///
///
int   nLevel1()const { return fHeader.Level1s;} ///< \return number of Level1R elements (fast)
///
int   nLevel3()const { return fHeader.Level3s;} ///< \return number of Level3R elements (fast)
///
int   nBeta()const { return fHeader.Betas;} ///< \return number of BetaR elements (fast)
///
int   nBetaB()const { return fHeader.BetaBs;} ///< \return number of BetaR elements alternative rec (fast)
///
int   nBetaH()const { return fHeader.BetaHs;} ///< \return number of BetaHR elements alternative rec (fast)
///
int   nVertex()const { return fHeader.Vertexs;} ///< \return number of VertexR elements (fast)
///
int   nCharge()const { return fHeader.Charges;} ///< \return number of ChargeR elements (fast)
///
int   nParticle()const { return fHeader.Particles;} ///< \return number of ParticleR elements (fast)
///
int   nAntiMCCluster()const { return fHeader.AntiMCClusters;} ///< \return number of AntiMCClusterR elements (fast)
///
int   nTrMCCluster()const { return fHeader.TrMCClusters;} ///< \return number of TrMCClusterR elements (fast)
///
int   nTofMCCluster()const { return fHeader.TofMCClusters;} ///< \return number of TofMCClusterR elements (fast)
///
int   nTofMCPmtHit()const { return fHeader.TofMCPmtHits;} ///< \return number of TofMCPmtHitR elements (fast)
///
int   nEcalMCHit()const { return fHeader.EcalMCHits;} ///< \return number of EcalMCHitR elements (fast)
///
int   nTrdMCCluster()const { return fHeader.TrdMCClusters;} ///< \return number of TrdMCClusterR elements (fast)
///
int   nRichMCCluster()const { return fHeader.RichMCClusters;} ///< \return number of RichMCClusterR elements (fast)
///
int   nMCTrack()const { return fHeader.MCTracks;} ///< \return number of MCTrackR elements (fast)
///
int   nMCEventg()const { return fHeader.MCEventgs;} ///< \return number of MCEventgR elements (fast)
///
int   nDaqEvent()const { return fHeader.DaqEvents;} ///< \return number of MCEventgR elements (fast)
///


  protected:


  //ECAL

  vector<EcalHitR> fEcalHit;
  vector<EcalClusterR> fEcalCluster;
  vector<Ecal2DClusterR> fEcal2DCluster;
  vector<EcalShowerR> fEcalShower;


  //RICH
  vector<RichHitR> fRichHit;
  vector<RichRingR> fRichRing;
  vector<RichRingBR> fRichRingB;



  //TOF
  vector<TofRawClusterR> fTofRawCluster;
  vector<TofRawSideR> fTofRawSide;
  vector<TofClusterR> fTofCluster;
  vector<TofClusterHR>fTofClusterH;


  //Anti
  vector<AntiRawSideR> fAntiRawSide;
  vector<AntiClusterR> fAntiCluster;


  //Tracker

  vector<TrRawClusterR> fTrRawCluster;
  vector<TrClusterR> fTrCluster;
  vector<TrRecHitR> fTrRecHit;
  vector<TrTrackR> fTrTrack;

  //TRD

  vector<TrdRawHitR> fTrdRawHit;
  vector<TrdClusterR> fTrdCluster;
  vector<TrdSegmentR> fTrdSegment;
  vector<TrdTrackR> fTrdTrack;
  vector<TrdHSegmentR> fTrdHSegment;
  vector<TrdHTrackR> fTrdHTrack;


  //Triggers

  vector<Level1R> fLevel1;
  vector<Level3R> fLevel3;


  //AxAMS
  vector<BetaR> fBeta;
  vector<BetaR> fBetaB;
  vector<BetaHR> fBetaH;
  vector<TofChargeHR> fTofChargeH;
  vector<ChargeR> fCharge;
  vector<VertexR> fVertex;
  vector<ParticleR> fParticle;



  //MC SubDet
  vector<AntiMCClusterR> fAntiMCCluster;
  vector<TrMCClusterR>   fTrMCCluster;
  vector<TofMCClusterR>  fTofMCCluster;
  vector<TofMCPmtHitR>   fTofMCPmtHit;
  vector<EcalMCHitR>     fEcalMCHit;
  vector<TrdMCClusterR>  fTrdMCCluster;
  vector<RichMCClusterR> fRichMCCluster;


  //MC General

  vector<MCTrackR>       fMCTrack;
  vector<MCEventgR>      fMCEventg;

  //DAQ
  vector<DaqEventR>      fDaqEvent;

  //Aux

   vector<float>         fAux;

   public:

     /// Get into memory contents for all branches
     void GetAllContents();

      ///  \return number of EcalHitR
      ///
      unsigned int   NEcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return fEcalHit.size();
      }
      ///  \return reference of EcalHitR Collection
      ///
      vector<EcalHitR> & EcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return  fEcalHit;
       }

       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return reference to corresponding EcalHitR element
      ///
       EcalHitR &   EcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return fEcalHit.at(l);
      }



       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return pointer to corresponding EcalHitR element
      ///
      EcalHitR *   pEcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return l<fEcalHit.size()?&(fEcalHit[l]):0;
      }

      ///  EcalClusterR accessor
      ///  \return number of EcalClusterR
      ///
      unsigned int   NEcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return fEcalCluster.size();
      }
      ///  EcalClusterR accessor
      ///  \return reference of EcalClusterR Collection
      ///
      vector<EcalClusterR> & EcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return  fEcalCluster;
       }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return reference to corresponding EcalClusterR element
      ///
       EcalClusterR &   EcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return fEcalCluster.at(l);
      }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return pointer to corresponding EcalClusterR element
      ///
      EcalClusterR *   pEcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return l<fEcalCluster.size()?&(fEcalCluster[l]):0;
      }



      ///  Ecal2DClusterR accessor
      ///  \return number of Ecal2DClusterR
      ///
      unsigned int   NEcal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return fEcal2DCluster.size();
      }
            ///  Ecal2DClusterR accessor
      ///  \return reference of Ecal2DClusterR Collection
      ///
      vector<Ecal2DClusterR> & Ecal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return  fEcal2DCluster;
       }
             ///  Ecal2DClusterR accessor
      /// \param l index of Ecal2DClusterR Collection
     ///  \return reference to corresponding Ecal2DClusterR element
      ///
       Ecal2DClusterR &   Ecal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return fEcal2DCluster.at(l);
      }
                 ///  Ecal2DClusterR accessor
    /// \param l index of Ecal2DClusterR Collection
   ///  \return pointer to corresponding Ecal2DClusterR element
      ///
      Ecal2DClusterR *   pEcal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return l<fEcal2DCluster.size()?&(fEcal2DCluster[l]):0;
      }


      ///  \return number of EcalShowerR
      ///
      unsigned int   NEcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return fEcalShower.size();
      }
       ///  \return reference of EcalShowerR Collection
      ///
      vector<EcalShowerR> & EcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return  fEcalShower;
       }
                 ///  EcalShowerR accessor
   /// \param l index of EcalShowerR Collection
  ///  \return reference to corresponding EcalShowerR element
      ///
       EcalShowerR &   EcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return fEcalShower.at(l);
      }
                ///  EcalShowerR accessor
  /// \param l index of EcalShowerR Collection
 ///  \return pointer to corresponding EcalShowerR element
      ///
      EcalShowerR *   pEcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return l<fEcalShower.size()?&(fEcalShower[l]):0;
      }



      ///  \return number of RichHitR
      ///
      unsigned int   NRichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return fRichHit.size();
      }
      ///  \return reference of RichHitR Collection
      ///
      vector<RichHitR> & RichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return  fRichHit;
       }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return reference to corresponding RichHitR element
      ///
       RichHitR &   RichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return fRichHit.at(l);
      }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return pointer to corresponding RichHitR element
      ///
      RichHitR *   pRichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return l<fRichHit.size()?&(fRichHit[l]):0;
      }






      ///  \return number of RichRingR
      ///
      unsigned int   NRichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0){
           bRichRing->GetEntry(_Entry);
        }
        return fRichRing.size();
      }
      ///  \return reference of RichRingR Collection
      ///
      vector<RichRingR> & RichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return  fRichRing;
       }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return reference to corresponding RichRingR element
      ///
       RichRingR &   RichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return fRichRing.at(l);
      }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return pointer to corresponding RichRingR element
      ///
      RichRingR *   pRichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
        return l<fRichRing.size()?&(fRichRing[l]):0;
      }






      ///  \return number of RichRingBR
      ///
      unsigned int   NRichRingB()  {
        if(fHeader.RichRingBs && fRichRingB.size()==0){
           bRichRingB->GetEntry(_Entry);
        }
        return fRichRingB.size();
      }
      ///  \return reference of RichRingBR Collection
      ///
      vector<RichRingBR> & RichRingB()  {
        if(fHeader.RichRingBs && fRichRingB.size()==0)bRichRingB->GetEntry(_Entry);
         return  fRichRingB;
       }

       ///  RichRingBR accessor
       /// \param l index of RichRingBR Collection
      ///  \return reference to corresponding RichRingBR element
      ///
       RichRingBR &   RichRingB(unsigned int l) {
        if(fHeader.RichRingBs && fRichRingB.size()==0)bRichRingB->GetEntry(_Entry);
         return fRichRingB.at(l);
      }

       ///  RichRingBR accessor
       /// \param l index of RichRingBR Collection
      ///  \return pointer to corresponding RichRingBR element
      ///
      RichRingBR *   pRichRingB(unsigned int l) {
        if(fHeader.RichRingBs && fRichRingB.size()==0)bRichRingB->GetEntry(_Entry);
        return l<fRichRingB.size()?&(fRichRingB[l]):0;
      }






      ///  \return number of TofRawClusterR
      ///
      unsigned int   NTofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return fTofRawCluster.size();
      }
      ///  \return reference of TofRawClusterR Collection
      ///
      vector<TofRawClusterR> & TofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return  fTofRawCluster;
       }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return reference to corresponding TofRawClusterR element
      ///
       TofRawClusterR &   TofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return fTofRawCluster.at(l);
      }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return pointer to corresponding TofRawClusterR element
      ///
      TofRawClusterR *   pTofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return l<fTofRawCluster.size()?&(fTofRawCluster[l]):0;
      }






      ///  \return number of TofRawSideR
      ///
      unsigned int   NTofRawSide()  {
        if(fHeader.TofRawSides  && fTofRawSide.size()==0){
          if(bTofRawSide)bTofRawSide->GetEntry(_Entry);
        }
          return fTofRawSide.size();
      }
      ///  \return reference of TofRawSideR Collection
      ///
      vector<TofRawSideR> & TofRawSide()  {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
         return  fTofRawSide;
       }

       ///  TofRawSideR accessor
       /// \param l index of TofRawSideR Collection
      ///  \return reference to corresponding TofRawSideR element
      ///
       TofRawSideR &   TofRawSide(unsigned int l) {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
         return fTofRawSide.at(l);
      }

       ///  TofRawSideR accessor
       /// \param l index of TofRawSideR Collection
      ///  \return pointer to corresponding TofRawSideR element
      ///
      TofRawSideR *   pTofRawSide(unsigned int l) {
        if(fHeader.TofRawSides && fTofRawSide.size()==0)bTofRawSide->GetEntry(_Entry);
        return l<fTofRawSide.size()?&(fTofRawSide[l]):0;
      }




      ///  \return number of TofClusterR
      ///
      unsigned int   NTofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return fTofCluster.size();
      }
      ///  \return reference of TofClusterR Collection
      ///
      vector<TofClusterR> & TofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return  fTofCluster;
       }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return reference to corresponding TofClusterR element
      ///
       TofClusterR &   TofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return fTofCluster.at(l);
      }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return pointer to corresponding TofClusterR element
      ///
      TofClusterR *   pTofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return l<fTofCluster.size()?&(fTofCluster[l]):0;
      }


        ///  \return number of TofClusterHR
      ///
      unsigned int   NTofClusterH()  {
        if(fHeader.TofClusterHs && fTofClusterH.size()==0)bTofClusterH->GetEntry(_Entry);
        return fTofClusterH.size();
      }
      ///  \return reference of TofClusterR Collection
      ///
      vector<TofClusterHR> & TofClusterH()  {
        if(fHeader.TofClusterHs && fTofClusterH.size()==0)bTofClusterH->GetEntry(_Entry);
         return  fTofClusterH;
       }

       ///  TofClusterHR accessor
       /// \param l index of TofClusterHR Collection
      ///  \return reference to corresponding TofClusterHR element
      ///
       TofClusterHR &   TofClusterH(unsigned int l) {
        if(fHeader.TofClusterHs && fTofClusterH.size()==0)bTofClusterH->GetEntry(_Entry);
         return fTofClusterH.at(l);
      }

       ///  TofClusterHR accessor
       /// \param l index of TofClusterHR Collection
      ///  \return pointer to corresponding TofClusterHR element
      ///
      TofClusterHR *   pTofClusterH(unsigned int l) {
        if(fHeader.TofClusterHs && fTofClusterH.size()==0)bTofClusterH->GetEntry(_Entry);
        return l<fTofClusterH.size()?&(fTofClusterH[l]):0;
      }



      ///  \return number of AntiRawSideR
      ///
      unsigned int   NAntiRawSide()  {
        if(fHeader.AntiRawSides  && fAntiRawSide.size()==0){
          if(bAntiRawSide)bAntiRawSide->GetEntry(_Entry);
        }
          return fAntiRawSide.size();
      }
      ///  \return reference of AntiRawSideR Collection
      ///
      vector<AntiRawSideR> & AntiRawSide()  {
        if(fHeader.AntiRawSides && fAntiRawSide.size()==0)bAntiRawSide->GetEntry(_Entry);
         return  fAntiRawSide;
       }

       ///  AntiRawSideR accessor
       /// \param l index of AntiRawSideR Collection
      ///  \return reference to corresponding AntiRawSideR element
      ///
       AntiRawSideR &   AntiRawSide(unsigned int l) {
        if(fHeader.AntiRawSides && fAntiRawSide.size()==0)bAntiRawSide->GetEntry(_Entry);
         return fAntiRawSide.at(l);
      }

       ///  AntiRawSideR accessor
       /// \param l index of AntiRawSideR Collection
      ///  \return pointer to corresponding AntiRawSideR element
      ///
      AntiRawSideR *   pAntiRawSide(unsigned int l) {
        if(fHeader.AntiRawSides && fAntiRawSide.size()==0)bAntiRawSide->GetEntry(_Entry);
        return l<fAntiRawSide.size()?&(fAntiRawSide[l]):0;
      }




      ///  \return number of AntiClusterR
      ///
      unsigned int   NAntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return fAntiCluster.size();
      }
      ///  \return reference of AntiClusterR Collection
      ///
      vector<AntiClusterR> & AntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return  fAntiCluster;
       }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return reference to corresponding AntiClusterR element
      ///
       AntiClusterR &   AntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return fAntiCluster.at(l);
      }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return pointer to corresponding AntiClusterR element
      ///
      AntiClusterR *   pAntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return l<fAntiCluster.size()?&(fAntiCluster[l]):0;
      }


      ///  \return number of TrRawClusterR
      ///
      unsigned int   NTrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return fTrRawCluster.size();
      }
      ///  \return reference of TrRawClusterR Collection
      ///
      vector<TrRawClusterR> & TrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return  fTrRawCluster;
       }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return reference to corresponding TrRawClusterR element
      ///
       TrRawClusterR &   TrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return fTrRawCluster.at(l);
      }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return pointer to corresponding TrRawClusterR element
      ///
      TrRawClusterR *   pTrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return l<fTrRawCluster.size()?&(fTrRawCluster[l]):0;
      }




      ///  \return number of TrClusterR
      ///
      unsigned int   NTrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return fTrCluster.size();
      }
      ///  \return reference of TrClusterR Collection
      ///
      vector<TrClusterR> & TrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return  fTrCluster;
       }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return reference to corresponding TrClusterR element
      ///
       TrClusterR &   TrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return fTrCluster.at(l);
      }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return pointer to corresponding TrClusterR element
      ///
      TrClusterR *   pTrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return l<fTrCluster.size()?&(fTrCluster[l]):0;
      }




       ///  TrRecHitR accessor
      ///  \return number of TrRecHitR
      ///
      unsigned int   NTrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return fTrRecHit.size();
      }
      ///  \return reference of TrRecHitR Collection
      ///
      vector<TrRecHitR> & TrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return  fTrRecHit;
       }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return reference to corresponding TrRecHitR element
      ///
       TrRecHitR &   TrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return fTrRecHit.at(l);
      }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return pointer to corresponding TrRecHitR element
      ///
      TrRecHitR *   pTrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return l<fTrRecHit.size()?&(fTrRecHit[l]):0;
      }


       ///  TrTrackR accessor
      ///  \return number of TrTrackR
      ///
      unsigned int   NTrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
#ifndef _PGTRACK_
for(int k=0;k<fTrTrack.size();k++)fTrTrack[k].Compat();
#endif
        return fTrTrack.size();
      }
      ///  \return reference of TrTrackR Collection
      ///
      vector<TrTrackR> & TrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
#ifndef _PGTRACK_
for(int k=0;k<fTrTrack.size();k++)fTrTrack[k].Compat();
#endif
         return  fTrTrack;
       }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return reference to corresponding TrTrackR element
      ///
       TrTrackR &   TrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
#ifndef _PGTRACK_
for(int k=0;k<fTrTrack.size();k++)fTrTrack[k].Compat();
#endif
         return fTrTrack.at(l);
      }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return pointer to corresponding TrTrackR element
      ///
      TrTrackR *   pTrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
#ifndef _PGTRACK_
for(int k=0;k<fTrTrack.size();k++)fTrTrack[k].Compat();
#endif
        return l<fTrTrack.size()?&(fTrTrack[l]):0;
      }




       ///  TrdRawHitR accessor
      ///  \return number of TrdRawHitR
      ///
      unsigned int   NTrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return fTrdRawHit.size();
      }
      ///  \return reference of TrdRawHitR Collection
      ///
      vector<TrdRawHitR> & TrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return  fTrdRawHit;
       }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return reference to corresponding TrdRawHitR element
      ///
       TrdRawHitR &   TrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return fTrdRawHit.at(l);
      }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return pointer to corresponding TrdRawHitR element
      ///
      TrdRawHitR *   pTrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return l<fTrdRawHit.size()?&(fTrdRawHit[l]):0;
      }


       ///  TrdClusterR accessor
      ///  \return number of TrdClusterR
      ///
      unsigned int   NTrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return fTrdCluster.size();
      }
      ///  \return reference of TrdClusterR Collection
      ///
      vector<TrdClusterR> & TrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return  fTrdCluster;
       }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return reference to corresponding TrdClusterR element
      ///
       TrdClusterR &   TrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return fTrdCluster.at(l);
      }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return pointer to corresponding TrdClusterR element
      ///
      TrdClusterR *   pTrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return l<fTrdCluster.size()?&(fTrdCluster[l]):0;
      }


       ///  TrdSegmentR accessor
      ///  \return number of TrdSegmentR
      ///
      unsigned int   NTrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return fTrdSegment.size();
      }
      ///  \return reference of TrdSegmentR Collection
      ///
      vector<TrdSegmentR> & TrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return  fTrdSegment;
       }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return reference to corresponding TrdSegmentR element
      ///
       TrdSegmentR &   TrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return fTrdSegment.at(l);
      }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return pointer to corresponding TrdSegmentR element
      ///
      TrdSegmentR *   pTrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return l<fTrdSegment.size()?&(fTrdSegment[l]):0;
      }



       ///  TrdTrackR accessor
      ///  \return number of TrdTrackR
      ///
      unsigned int   NTrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return fTrdTrack.size();
      }
      ///  \return reference of TrdTrackR Collection
      ///
      vector<TrdTrackR> & TrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return  fTrdTrack;
       }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return reference to corresponding TrdTrackR element
      ///
       TrdTrackR &   TrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return fTrdTrack.at(l);
      }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return pointer to corresponding TrdTrackR element
      ///
      TrdTrackR *   pTrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return l<fTrdTrack.size()?&(fTrdTrack[l]):0;
      }


      ///  TrdHSegmentR accessor
      ///  \return number of TrdHSegmentR
      ///
      unsigned int   NTrdHSegment() {
        if(fHeader.TrdHSegments && fTrdHSegment.size()==0)
	  bTrdHSegment->GetEntry(_Entry);
        return fTrdHSegment.size();
      }
      ///  \return reference of TrdHSegmentR Collection
      ///
      vector<TrdHSegmentR> & TrdHSegment() {
        if(fHeader.TrdHSegments && fTrdHSegment.size()==0)
	  bTrdHSegment->GetEntry(_Entry);
        return  fTrdHSegment;
      }
      ///  TrdHSegmentR accessor
      /// \param l index of TrdHSegmentR Collection
      ///  \return reference to corresponding TrdHSegmentR element
      ///
      TrdHSegmentR &   TrdHSegment(unsigned int l) {
        if(fHeader.TrdHSegments && fTrdHSegment.size()==0)bTrdHSegment->GetEntry\
							    (_Entry);
        return fTrdHSegment.at(l);
      }

      ///  TrdHSegmentR accessor
      /// \param l index of TrdHSegmentR Collection
      ///  \return pointer to corresponding TrdHSegmentR element
      ///
      TrdHSegmentR *   pTrdHSegment(unsigned int l) {
        if(fHeader.TrdHSegments && fTrdHSegment.size()==0
	   )bTrdHSegment->GetEntry(_Entry);
        return l<fTrdHSegment.size()?&(fTrdHSegment[l]):0;
      }

      ///  TrdHTrackR accessor
      ///  \return number of TrdHTrackR
      ///
      unsigned int   NTrdHTrack()  {
        if(fHeader.TrdHTracks && fTrdHTrack.size()==0)bTrdHTrack->GetEntry(_Entry);
        return fTrdHTrack.size();
      }
      ///  \return reference of TrdHTrackR Collection
      ///
      vector<TrdHTrackR> & TrdHTrack() {
        if(fHeader.TrdHTracks && fTrdHTrack.size()==0)
	  bTrdHTrack->GetEntry(_Entry);
	return  fTrdHTrack;
      }

      ///  TrdHTrackR accessor
      /// \param l index of TrdHTrackR Collection
      ///  \return reference to corresponding TrdHTrackR element
      ///
      TrdHTrackR &   TrdHTrack(unsigned int l) {
        if(fHeader.TrdHTracks && fTrdHTrack.size()==0)
	  bTrdHTrack->GetEntry(_Entry);
	return fTrdHTrack.at(l);
      }

      ///  TrdHTrackR accessor
      /// \param l index of TrdHTrackR Collection
      ///  \return pointer to corresponding TrdHTrackR element
      ///
      TrdHTrackR *   pTrdHTrack(unsigned int l) {
        if(fHeader.TrdHTracks && fTrdHTrack.size()==0)
	  bTrdHTrack->GetEntry(_Entry);
        return l<fTrdHTrack.size()?&(fTrdHTrack[l]):0;
      }



       ///  Level1R accessor
      ///  \return number of Level1R
      ///
      unsigned int   NLevel1()  {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
        return fLevel1.size();
      }
      ///  \return reference of Level1R Collection
      ///
      vector<Level1R> & Level1()  {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
         return  fLevel1;
       }

       ///  Level1R accessor
       /// \param l index of Level1R Collection
      ///  \return reference to corresponding Level1R element
      ///
       Level1R &   Level1(unsigned int l) {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
         return fLevel1.at(l);
      }

       ///  Level1R accessor
       /// \param l index of Level1R Collection
      ///  \return pointer to corresponding Level1R element
      ///
      Level1R *   pLevel1(unsigned int l=0) {
        if(fHeader.Level1s && fLevel1.size()==0)bLevel1->GetEntry(_Entry);
        return l<fLevel1.size()?&(fLevel1[l]):0;
      }






       ///  Level3R accessor
      ///  \return number of Level3R
      ///
      unsigned int   NLevel3()  {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
        return fLevel3.size();
      }
      ///  \return reference of Level3R Collection
      ///
      vector<Level3R> & Level3()  {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
         return  fLevel3;
       }

       ///  Level3R accessor
       /// \param l index of Level3R Collection
      ///  \return reference to corresponding Level3R element
      ///
       Level3R &   Level3(unsigned int l) {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
         return fLevel3.at(l);
      }

       ///  Level3R accessor
       /// \param l index of Level3R Collection
      ///  \return pointer to corresponding Level3R element
      ///
      Level3R *   pLevel3(unsigned int l) {
        if(fHeader.Level3s && fLevel3.size()==0)bLevel3->GetEntry(_Entry);
        return l<fLevel3.size()?&(fLevel3[l]):0;
      }










       ///  BetaR accessor
      ///  \return number of BetaR
      ///
      unsigned int   NBeta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return fBeta.size();
      }
      ///  \return reference of BetaR Collection
      ///
      vector<BetaR> & Beta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return  fBeta;
       }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return reference to corresponding BetaR element
      ///
       BetaR &   Beta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return fBeta.at(l);
      }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return pointer to corresponding BetaR element
      ///
      BetaR *   pBeta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return l<fBeta.size()?&(fBeta[l]):0;
      }




       ///  BetaR accessor to BetaB container (alt. reconstruction)
      ///  \return number of BetaR to BetaB container (alt. reconstruction)
      ///
      unsigned int   NBetaB()  {
        if(fHeader.BetaBs && fBetaB.size()==0)bBetaB->GetEntry(_Entry);
        return fBetaB.size();
      }
      ///  \return reference of BetaR Collection to BetaB container (alt. reconstruction)
      ///
      vector<BetaR> & BetaB()  {
        if(fHeader.BetaBs && fBetaB.size()==0)bBetaB->GetEntry(_Entry);
         return  fBetaB;
       }

       ///  BetaR accessor to BetaB container (alt. reconstruction)
       /// \param l index of BetaR Collection
      ///  \return reference to corresponding BetaR element to BetaB container (alt. reconstruction)
      ///
       BetaR &   BetaB(unsigned int l) {
        if(fHeader.BetaBs && fBetaB.size()==0)bBetaB->GetEntry(_Entry);
         return fBetaB.at(l);
      }

       ///  BetaR accessor to BetaB container (alt. reconstruction)
       /// \param l index of BetaR Collection
      ///  \return pointer to corresponding BetaR element to BetaB container (alt. reconstruction)
      ///
      BetaR *   pBetaB(unsigned int l) {
        if(fHeader.BetaBs && fBetaB.size()==0)bBetaB->GetEntry(_Entry);
        return l<fBetaB.size()?&(fBetaB[l]):0;
      }


       ///  BetaHR accessor
      ///  \return number of BetaHR
      ///
      unsigned int   NBetaH()  {
        if(fHeader.BetaHs && fBetaH.size()==0)bBetaH->GetEntry(_Entry);
        return fBetaH.size();
      }
      ///  \return reference of BetaHR Collection
      ///
      vector<BetaHR> & BetaH()  {
        if(fHeader.BetaHs && fBetaH.size()==0)bBetaH->GetEntry(_Entry);
         return  fBetaH;
       }

       ///  BetaHR accessor
       /// \param l index of BetaHR Collection
      ///  \return reference to corresponding BetaHR element
      ///
       BetaHR &   BetaH(unsigned int l) {
        if(fHeader.BetaHs && fBetaH.size()==0)bBetaH->GetEntry(_Entry);
         return fBetaH.at(l);
      }

       ///  BetaHR accessor
       /// \param l index of BetaHR Collection
      ///  \return pointer to corresponding BetaHR element
      ///
      BetaHR *   pBetaH(unsigned int l) {
        if(fHeader.BetaHs && fBetaH.size()==0)bBetaH->GetEntry(_Entry);
        return l<fBetaH.size()?&(fBetaH[l]):0;
      }

       ///  TofChargeHR accessor
      ///  \return number of TofChargeHR
      vector<TofChargeHR> & TofChargeH()  {
         return  fTofChargeH;
       }

       ///  TofChargeHR accessor
       /// \param l index of TofChargeHR Collection (l should from BetaH::iTofChargeH())
      ///  \return reference to corresponding TofChargeHR element
      ///
       TofChargeHR &   TofChargeH(unsigned int l) {
         return fTofChargeH.at(l);
      }

       ///  TofChargeHR accessor
       /// \param l index of TofChargeHR Collection (l should from BetaH::iTofChargeH())
      ///  \return pointer to corresponding TofChargeHR element
      ///
      TofChargeHR *   pTofChargeH(unsigned int l) {
        return l<fTofChargeH.size()?&(fTofChargeH[l]):0;
      }



       ///  ChargeR accessor
      ///  \return number of ChargeR
      ///
      unsigned int   NCharge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return fCharge.size();
      }
      ///  \return reference of ChargeR Collection
      ///
      vector<ChargeR> & Charge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return  fCharge;
       }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return reference to corresponding ChargeR element
      ///
       ChargeR &   Charge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return fCharge.at(l);
      }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return pointer to corresponding ChargeR element
      ///
      ChargeR *   pCharge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return l<fCharge.size()?&(fCharge[l]):0;
      }



       ///  ChargeSubDR accessor
      ///  \return number of ChargeSubDR
      ///
      unsigned int   NChargeSubD()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return 0;
      }
      ///  \return reference of ChargeSubDR Collection
      ///
      /*
      vector<ChargeSubDR> & ChargeSubD()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return NULL;
       }
*/

       ///  ChargeSubDR accessor
       /// \param l index of ChargeSubDR Collection
      ///  \return reference to corresponding ChargeSubDR element
      ///
      /*
       ChargeSubDR &   ChargeSubD(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return NULL;
      }
      */

       ///  ChargeSubDR accessor
       /// \param l index of ChargeSubDR Collection
      ///  \return pointer to corresponding ChargeSubDR element
      ///
      ChargeSubDR *   pChargeSubD(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return NULL;
      }





       ///  VertexR accessor
      ///  \return number of VertexR
      ///
      unsigned int   NVertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return fVertex.size();
      }
      ///  \return reference of VertexR Collection
      ///
      vector<VertexR> & Vertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return  fVertex;
       }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return reference to corresponding VertexR element
      ///
       VertexR &   Vertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return fVertex.at(l);
      }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return pointer to corresponding VertexR element
      ///
      VertexR *   pVertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return l<fVertex.size()?&(fVertex[l]):0;
      }

       ///  ParticleR accessor
      ///  \return number of ParticleR
      ///
      unsigned int   NParticle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return fParticle.size();
      }
      ///  \return reference of ParticleR Collection
      ///
      vector<ParticleR> & Particle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return  fParticle;
       }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return reference to corresponding ParticleR element
      ///
       ParticleR &   Particle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return fParticle.at(l);
      }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return pointer to corresponding ParticleR element
      ///
      ParticleR *   pParticle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return l<fParticle.size()?&(fParticle[l]):0;
      }




       ///  AntiMCClusterR accessor
      ///  \return number of AntiMCClusterR
      ///
      unsigned int   NAntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return fAntiMCCluster.size();
      }
      ///  \return reference of AntiMCClusterR Collection
      ///
      vector<AntiMCClusterR> & AntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return  fAntiMCCluster;
       }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return reference to corresponding AntiMCClusterR element
      ///
       AntiMCClusterR &   AntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return fAntiMCCluster.at(l);
      }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return pointer to corresponding AntiMCClusterR element
      ///
      AntiMCClusterR *   pAntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return l<fAntiMCCluster.size()?&(fAntiMCCluster[l]):0;
      }





       ///  TofMCClusterR accessor
      ///  \return number of TofMCClusterR
      ///
      unsigned int   NTofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return fTofMCCluster.size();
      }
      ///  \return reference of TofMCClusterR Collection
      ///
      vector<TofMCClusterR> & TofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return  fTofMCCluster;
       }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return reference to corresponding TofMCClusterR element
      ///
       TofMCClusterR &   TofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return fTofMCCluster.at(l);
      }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return pointer to corresponding TofMCClusterR element
      ///
      TofMCClusterR *   pTofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return l<fTofMCCluster.size()?&(fTofMCCluster[l]):0;
      }


      ///  TofMCPmtHitR accessor  //Qi Yan
      ///  \return number of TofMCPmtHitR
      ///
      unsigned int   NTofMCPmtHit()  {
        if(fHeader.TofMCPmtHits && fTofMCPmtHit.size()==0)bTofMCPmtHit->GetEntry(_Entry);
        return fTofMCPmtHit.size();
      }
      ///  \return reference of TofMCPmtHitR> Collection
      ///
      vector<TofMCPmtHitR> & TofMCPmtHit()  {
        if(fHeader.TofMCPmtHits && fTofMCPmtHit.size()==0)bTofMCPmtHit->GetEntry(_Entry);
         return  fTofMCPmtHit;
       }

       ///  TofMCPmtHitR accessor
       /// \param l index of TofMCPmtHitR Collection
      ///  \return reference to corresponding TofMCPmtHitR element
      ///
     TofMCPmtHitR &   TofMCPmtHit(unsigned int l) {//problem
        if(fHeader.TofMCPmtHits && fTofMCPmtHit.size()==0)bTofMCPmtHit->GetEntry(_Entry);
         return fTofMCPmtHit.at(l);
      }

       ///  TofMCPmtHitR accessor
       /// \param l index of TofMCPmtHitR Collection
      ///  \return pointer to corresponding TofMCPmtHitR element
      ///
      TofMCPmtHitR *   pTofMCPmtHit(unsigned int l) {//problem
        if(fHeader.TofMCPmtHits && fTofMCPmtHit.size()==0)bTofMCPmtHit->GetEntry(_Entry);
        return l<fTofMCPmtHit.size()?&(fTofMCPmtHit[l]):0;
      }



       ///  EcalMCHitR accessor
      ///  \return number of EcalMCHitR
      ///
      unsigned int   NEcalMCHit()  {
        if(fHeader.EcalMCHits && fEcalMCHit.size()==0)bEcalMCHit->GetEntry(_Entry);
        return fEcalMCHit.size();
      }
      ///  \return reference of EcalMCHitR Collection
      ///
      vector<EcalMCHitR> & EcalMCHit()  {
        if(fHeader.EcalMCHits && fEcalMCHit.size()==0)bEcalMCHit->GetEntry(_Entry);
         return  fEcalMCHit;
       }

       ///  EcalMCHitR accessor
       /// \param l index of EcalMCHitR Collection
      ///  \return reference to corresponding EcalMCHitR element
      ///
       EcalMCHitR &   EcalMCHit(unsigned int l) {
        if(fHeader.EcalMCHits && fEcalMCHit.size()==0)bEcalMCHit->GetEntry(_Entry);
         return fEcalMCHit.at(l);
      }

       ///  EcalMCHitR accessor
       /// \param l index of EcalMCHitR Collection
      ///  \return pointer to corresponding EcalMCHitR element
      ///
      EcalMCHitR *   pEcalMCHit(unsigned int l) {
        if(fHeader.EcalMCHits && fEcalMCHit.size()==0)bEcalMCHit->GetEntry(_Entry);
        return l<fEcalMCHit.size()?&(fEcalMCHit[l]):0;
      }





       ///  TrMCClusterR accessor
      ///  \return number of TrMCClusterR
      ///
      unsigned int   NTrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return fTrMCCluster.size();
      }
      ///  \return reference of TrMCClusterR Collection
      ///
      vector<TrMCClusterR> & TrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return  fTrMCCluster;
       }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return reference to corresponding TrMCClusterR element
      ///
       TrMCClusterR &   TrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return fTrMCCluster.at(l);
      }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return pointer to corresponding TrMCClusterR element
      ///
      TrMCClusterR *   pTrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return l<fTrMCCluster.size()?&(fTrMCCluster[l]):0;
      }





       ///  TrdMCOAClusterR accessor
      ///  \return number of TrdMCClusterR
      ///
      unsigned int   NTrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return fTrdMCCluster.size();
      }
      ///  \return reference of TrdMCClusterR Collection
      ///
      vector<TrdMCClusterR> & TrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return  fTrdMCCluster;
       }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return reference to corresponding TrdMCClusterR element
      ///
       TrdMCClusterR &   TrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return fTrdMCCluster.at(l);
      }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return pointer to corresponding TrdMCClusterR element
      ///
      TrdMCClusterR *   pTrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return l<fTrdMCCluster.size()?&(fTrdMCCluster[l]):0;
      }





       ///  RichMCClusterR accessor
      ///  \return number of RichMCClusterR
      ///
      unsigned int   NRichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return fRichMCCluster.size();
      }
      ///  \return reference of RichMCClusterR Collection
      ///
      vector<RichMCClusterR> & RichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return  fRichMCCluster;
       }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return reference to corresponding RichMCClusterR element
      ///
       RichMCClusterR &   RichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return fRichMCCluster.at(l);
      }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return pointer to corresponding RichMCClusterR element
      ///
      RichMCClusterR *   pRichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return l<fRichMCCluster.size()?&(fRichMCCluster[l]):0;
      }





       ///  MCTrackR accessor
      ///  \return number of MCTrackR
      ///
      unsigned int   NMCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return fMCTrack.size();
      }
      ///  \return reference of MCTrackR Collection
      ///
      vector<MCTrackR> & MCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return  fMCTrack;
       }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return reference to corresponding MCTrackR element
      ///
       MCTrackR &   MCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return fMCTrack.at(l);
      }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return pointer to corresponding MCTrackR element
      ///
      MCTrackR *   pMCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return l<fMCTrack.size()?&(fMCTrack[l]):0;
      }




       ///  DaqEventR accessor
      ///  \return number of DaqEventR
      ///
      unsigned int   NDaqEvent()  {
        if(fHeader.DaqEvents && fDaqEvent.size()==0)bDaqEvent->GetEntry(_Entry);
        return fDaqEvent.size();
      }

      ///  \return reference of DaqEventR Collection
      ///
      vector<DaqEventR> & DaqEvent()  {
        if(fHeader.DaqEvents && fDaqEvent.size()==0)bDaqEvent->GetEntry(_Entry);
         return  fDaqEvent;
       }

       ///  DaqEventR accessor
       /// \param l index of DaqEventR Collection
      ///  \return reference to corresponding DaqEventR element
      ///
       DaqEventR &   DaqEvent(unsigned int l) {
        if(fHeader.DaqEvents && fDaqEvent.size()==0)bDaqEvent->GetEntry(_Entry);
         return fDaqEvent.at(l);
      }

       ///  DaqEventR accessor
       /// \param l index of DaqEventR Collection
      ///  \return pointer to corresponding DaqEventR element
      ///
      DaqEventR *   pDaqEvent(unsigned int l) {
        if(fHeader.DaqEvents && fDaqEvent.size()==0)bDaqEvent->GetEntry(_Entry);
        return l<fDaqEvent.size()?&(fDaqEvent[l]):0;
      }


       ///  MCEventgR accessor
      ///  \return number of MCEventgR
      ///
      unsigned int   NMCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return fMCEventg.size();
      }
      ///  \return reference of MCEventgR Collection
      ///
      vector<MCEventgR> & MCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return  fMCEventg;
       }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return reference to corresponding MCEventgR element
      ///
       MCEventgR &   MCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return fMCEventg.at(l);
      }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return pointer to corresponding MCEventgR element
      ///
      MCEventgR *   pMCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return l<fMCEventg.size()?&(fMCEventg[l]):0;
      }






AMSEventR();
AMSEventR(const AMSEventR &o);
friend class ParticleR;

#ifdef __ROOTSHAREDLIBRARY__
virtual ~AMSEventR(){
--_Count;
////cout <<"AMSEventR::dtor-I-Destroying "<<_Count<<endl;;
//if(!--_Count){
 //cout<<"AMSEventR::dtor-I-ResettingHead "<<_Head<<endl;
//// _Head=0;
//}
//}
}
#endif


void clear();

#ifndef __ROOTSHAREDLIBRARY__
void         AddAMSObject(DAQEvent *ptr);
void         AddAMSObject(Anti2RawEvent *ptr);
void         AddAMSObject(AMSAntiCluster *ptr);
void         AddAMSObject(AMSAntiMCCluster *ptr);
void         AddAMSObject(AMSBeta *ptr);
void         AddAMSObjectB(AMSBeta *ptr);
void         AddAMSObject(AMSBetaH *ptr);
void         AddAMSObject(AMSCharge *ptr);
void         AddAMSObject(AMSEcalHit *ptr);
void         AddAMSObject(AMSEcalMCHit *ptr);
void         AddAMSObject(AMSmceventg *ptr);
void         AddAMSObject(AMSmctrack *ptr);
void         AddAMSObject(AMSVtx *ptr);
void         AddAMSObject(AMSParticle *ptr, float phi, float phigl);
void         AddAMSObject(AMSRichMCHit *ptr, int numgen);
void         AddAMSObject(AMSRichRing *ptr);
void         AddAMSObject(AMSRichRingNew *ptr);
void         AddAMSObject(AMSRichRawEvent *ptr, float x, float y, float z);
void         AddAMSObject(AMSTOFCluster *ptr);
void         AddAMSObject(AMSTOFClusterH *ptr);
void         AddAMSObject(AMSTOFMCCluster *ptr);
void         AddAMSObject(AMSTOFMCPmtHit *ptr);
void         AddAMSObject(AMSTrRecHit *ptr);
void         AddAMSObject(AMSTRDCluster *ptr);
void         AddAMSObject(AMSTRDMCCluster *ptr);
void         AddAMSObject(AMSTRDRawHit *ptr);
void         AddAMSObject(AMSTRDSegment *ptr);
void         AddAMSObject(AMSTRDTrack *ptr);
void         AddAMSObject(AMSTRDHTrack *ptr);
void         AddAMSObject(AMSTRDHSegment *ptr);
void         AddAMSObject(AMSTrCluster *ptr);
void         AddAMSObject(AMSTrMCCluster *ptr);
void         AddAMSObject(AMSTrRawCluster *ptr);
void         AddAMSObject(AMSTrTrack *ptr);
void         AddAMSObject(Ecal1DCluster *ptr);
void         AddAMSObject(AMSEcal2DCluster  *ptr);
void         AddAMSObject(AMSEcalShower  *ptr);
void         AddAMSObject(TOF2RawCluster *ptr);
void         AddAMSObject(TOF2RawSide *ptr);
void         AddAMSObject(Trigger2LVL1 *ptr);
void         AddAMSObject(TriggerLVL302 *ptr);
#endif
void RebuildBetaH();
void RebuildMCEventg();
friend class AMSChain;
ClassDef(AMSEventR,21)       //AMSEventR
#pragma omp threadprivate(fgIsA)
};



typedef AMSEventR  ams;
#endif

