//  $Id: root.h,v 1.92 2003/06/04 09:55:37 choutko Exp $

//
//  NB Please increase the version number in corr classdef 
//  for any class modification
//

#ifndef __AMSROOT__
#define __AMSROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TSelector.h>
#include <TROOT.h>
#include <TBranch.h>
#include <list>
#include <vector>
#include <iostream>
#include <TStopwatch.h>
using namespace std;
#ifndef __ROOTSHAREDLIBRARY__
class AMSAntiCluster;
class AMSAntiMCCluster;
class AMSBeta;
class AMSCharge;
class AMSEvent;
class AMSEcalHit;
class AMSmceventg;
class AMSmctrack;
class AMSParticle;
class AMSRichMCHit;
class AMSRichRawEvent;
class AMSRichRing;
class AMSNtuple;
class AMSTOFCluster;
class AMSTOFMCCluster;
class AMSTRDCluster;
class AMSTRDMCCluster;
class AMSTRDRawHit;
class AMSTRDSegment;
class AMSTRDTrack;
class AMSTrCluster;
class AMSTrMCCluster;
class AMSTrRawCluster;
class AMSTrTrack;
class AMSTrTrackGamma;
class AMSTrRecHit;
class Ecal1DCluster;
class AMSEcal2DCluster;
class AMSEcalShower;
class TOF2RawCluster;
class Trigger2LVL1;
class TriggerLVL302;
class EventNtuple02;
#else
class AMSAntiCluster{};
class AMSAntiMCCluster{};
class AMSBeta{};
class AMSCharge{};
class AMSEvent{};
class AMSEcalHit{};
class AMSmceventg{};
class AMSmctrack{};
class AMSParticle{};
class AMSRichMCHit{};
class AMSRichRawEvent{};
class AMSRichRing{};
class AMSNtuple{};
class AMSTOFCluster{};
class AMSTOFMCCluster{};
class AMSTRDCluster{};
class AMSTRDMCCluster{};
class AMSTRDRawHit{};
class AMSTRDSegment{};
class AMSTRDTrack{};
class AMSTrCluster{};
class AMSTrMCCluster{};
class AMSTrRawCluster{};
class AMSTrTrack{};
class AMSTrTrackGamma{};
class AMSTrRecHit{};
class Ecal1DCluster{};
class AMSEcal2DCluster{};
class AMSEcalShower{};
class TOF2RawCluster{};
class Trigger2LVL1{};
class TriggerLVL302{};
class EventNtuple02{};
#endif


//!  Root Header class
/*!
  Contains:

  general info   (public)

  shuttle/iss parameters  public)

  object counters, aka arrays dimensions (protected, access via coo functions)
 \sa AMSEventR 

 \author v.choutko@cern.ch

*/
class HeaderR{
 public:




// general info block

  unsigned int Run;         ///<run  number
  unsigned int RunType;     ///<runtype(data), RNDM(1) MC
  unsigned int Event;      ///<event number
  unsigned int Status[2];  ///<event status
  int Raw;            ///<raw event length in bytes
  int Version;        ///< os number (low 2 bits) program build number (high 10 bits)
  int Time[2];        ///<unix time + usec time(data)/RNDM(2) MC



// shuttle/ISS parameters

   float RadS;    ///<iss orbit altitude cm  (I2000 coo sys)
   float ThetaS;  ///<theta (GTOD rad)  
   float PhiS;    ///<phi   (GTOD rad)
   float Yaw;     ///<ISS yaw (LVLH rad) 
   float Pitch;   ///<ISS pitch (LVLH rad) 
   float Roll;   ///<ISS roll (LVLH rad)  
   float VelocityS;     ///< ISS velocity (rad/sec) 
   float VelTheta;     ///< ISS speed theta (GTOD rad) 
   float VelPhi;       ///< ISS speed phi (GTOD rad)  
   float ThetaM;        ///< magnetic (calculated for an eccentric dipole coo system) theta  rad
   float PhiM;          ///< magnetic (calculated for an eccentric dipole coo system)phi  rad

// counters
protected:
int   EcalHits;              
int   EcalClusters;
int   Ecal2DClusters;
int   EcalShowers;
int   RichHits;
int   RichRings;
int   TofRawClusters;
int   TofClusters;  
int   AntiClusters;
int   TrRawClusters;
int   TrClusters;
int   TrRecHits;
int   TrTracks;
int   TrdRawHits;
int   TrdClusters;
int   TrdSegments;
int   TrdTracks;
int   Level1s;
int   Level3s;
int   Betas; 
int   Vertexs; 
int   Charges;  
int   Particles;  
int   AntiMCClusters;
int   TrMCClusters;
int   TofMCClusters;
int   TrdMCClusters;
int   RichMCClusters;
int   MCTracks;
int   MCEventgs;
public:

  /// A constructor.
  HeaderR(){};  
#ifndef __ROOTSHAREDLIBRARY__
  void   Set(EventNtuple02 *ptr);
#endif
  friend class AMSEventR;
  ClassDef(HeaderR,1)       //HeaderR
};



//!  Ecal Hits Structure
/*!

   \author e.choumilov@cern.ch
*/
class EcalHitR {
public:
  int   Status;   ///< Statusword
  int   Idsoft;   ///< 4digits number SPPC=SuperLayer/PM/subCell  0:8/0:35/0:3  
               /*!<
Idsoft SubCells(pixels) numbering(looking in +Y/+X direction):\n
...............|0|1|................. \n
PM1(at -X/Y)>> ----- >> PM36(at +X/Y) \n
...............|2|3|................. \n
                */
  int   Proj;     ///< projection (0-x,1-y)
  int   Plane;    ///< ECAL plane number (0(top),..,17(bot))
  int   Cell;     ///< ECAL Cell number (0(-x/y),..,71(+x/y))    
  float Edep;     ///< ECAL measured energy (MeV)
  float EdCorr;   ///< ECAL PMsaturation1-correction(MeV) added to Edep
  float AttCor;   ///< Attenuation Correction applied (w/r  to center of ecal) (MeV)
  float Coo[3];   ///< ECAL Coo (cm)
  float ADC[3];   ///< ECAL (ADC-Ped) for Hi/Low/Dynode channels
  float Ped[3];   ///< ECAL Pedestals   
  float Gain;     ///< 1/gain (!)

  EcalHitR(AMSEcalHit *ptr);
  EcalHitR(){};
ClassDef(EcalHitR,1)       //EcalHitR
};

/// EcalCluster structure
/*!

   \author v.choutko@cern.ch
*/
class EcalClusterR {
public:
  int Status;    ///<  Statusword
                 /*!< valid values are:\n
                                                     WIDE=2   \n
                                                     BAD =16  \n
                                                     USED=32  \n
                                                     NEAR=102 \n 
                                                     LEAK=16777216  \n
                                                     CATASTROPHICLEAK=33554432 \n
                                                     JUNK=67108864 \n
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
ClassDef(EcalClusterR,1)       //EcalClusterR
};


//!  Ecal2DClusterR Structure

/*!

   \author v.choutko@cern.ch
*/

class Ecal2DClusterR{
public:
  int Status;   ///< statusword
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
ClassDef(Ecal2DClusterR,1)       //Ecal2DClusterR
friend class AMSEcal2DCluster;
};


/*!
  \class EcalShowerR
  \brief 3d ecal shower description

   \sa ecalrec.h ecalrec.C
   \author v.choutko@cern.ch
*/


class EcalShowerR {
public:
  int   Status;    ///< status word \sa EcalHitR status
  float Dir[3];    ///< direction cos array 
  float EMDir[3];  ///< direction cos array for emag type shower
  float Entry[3];  ///< entry point (cm)
  float Exit[3];   ///< exit point(cm)
  float CofG[3];  ///< center of gravity (cm)
  float ErDir;    ///< 3d angle error
  float Chi2Dir;  ///<  chi2 direction fit
  float FirstLayerEdep; ///< front energy dep (Mev)
  float EnergyC; ///< shower energy (gev)
  float Energy3C[3]; ///< energy(+-2,+-5, +-8 cm)/energy ratios
  float ErEnergyC;   ///< energy error (gev)
  float DifoSum;     ///<  (E_x-E_y)/(E_x+E_y)
  float SideLeak;    ///< rel side leak
  float RearLeak;    ///< rel rear leak
  float DeadLeak;    ///< rel dead leak
  float AttLeak;     ///< rel att length correction
  float NLinLeak;   ///<  rel non-lin correction
  float OrpLeak;   ///<  fraction of shower energy outside core.
  float Orp2DEnergy; ///< orphaned Ecal2DClusterR energy (if any) (geV)
  float Chi2Profile;  ///< chi2 profile fit (by gamma function) 
  float ParProfile[4]; ///< normalization, shower max (cm), rad length, rel rear leak ) for profile fit
  float Chi2Trans;     ///< chi2 transverse fit (sum of two exp)
  float SphericityEV[3]; ///< sphericity tensor eigen values

protected:
  vector <int> fEcal2DCluster;  ///< indexes to Ecal2DClusterR collection
public:
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


  EcalShowerR(){};
  EcalShowerR(AMSEcalShower *ptr);
friend class AMSEcalShower;

ClassDef(EcalShowerR,1)       //EcalShowerR

};




//!  Rich Hits Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/

class RichHitR {
public:
  unsigned int Status;   ///< statusword
                         /*!<

*********) Status bits (counting from 1 to 32)

1- Hit used in a larger (ring number 1) object
2- Hit used in a larger (ring number 2) object 
3- Hit used in a larger (ring number 3) object
.
.
.
10- Hit used in a larger (ring number 10) object 
11- Unused
.
.
.
29- Unused
30- Gain mode chosen for the hit: 0=x1(low)  1=x5(high)
31- Hit belongs to a PMT apparently crossed by a charged particle
          
*/

  int   Channel;    ///<  channel number  
  int   Counts;     ///< ADC counts above the pedestal
  float Npe;        ///< ADC counts above the pedestal/gain of the channel 
  float X;         ///<  X pos. of the channel  
  float Y;         ///<  Y pos. of the channel

  RichHitR(){};
  RichHitR(AMSRichRawEvent *ptr, float x, float y);
ClassDef(RichHitR,1)       // RichHitR
};


  class TrTrackR;
//!  Rich Ring Structure

/*!
 \author Carlos.Jose.Delgado.Mendez@cern.ch
*/

class RichRingR {
public:

  unsigned int Status;     ///< status word
                           /*!<

**********) Ring status bits (counting from 1 to 32)
1- Ring has been rebuild after cleaning PMTs apparently crossed by a charged particle.
   If the rebuilding has been succesful it is stored in the next ring.  
   To confirm that the next ring is a rebuilding of the current one check
   if both rcritrkn are pointing to the same track number.   
	    
2- Rins reconstructed using the NaF radiator in the double radiator configuration

*/

  int   Used;  ///< Nb. of RICH hits in a ring 
  int   UsedM; ///< Nb. of RICH mirrored hits in a ring 
  float Beta;  ///< Reconstructed velocity
  float ErrorBeta;  ///< Error in the velocity
  float Chi2;       ///< chi2/ndof for the beta fit
  float Prob;       ///< probability to be a good ring
  float NpExp;      ///< number of expected photoelectrons for Z=1 charge
  float NpCol;      ///< number of collected photoelectrons 

  protected:
  int fTrTrack;   ///< index of  TrTrackR  in collection
  public:
  /// access function to TrTrackR object used
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR object or 0
  TrTrackR * pTrTrack();

  RichRingR(){};
  RichRingR(AMSRichRing *ptr);
  friend class AMSRichRing;
  ClassDef(RichRingR,1)           // RichRingR
}; 


/// TofRawClusterR structure

/*!
 \author e.choumilov@cern.ch

*/

class TofRawClusterR {
public:
  int   Status;  ///< statusword (as for TofCluster)
  int   Layer;   ///< Tof plane 1(top)...4(bot)
  int   Bar;     ///< Tof paddle number 1...10
  float tovta[2]; ///< 2-sides Anode ADC
  float tovtd[2]; ///< 2-sides Dynode ADC(HiGain channel)
  float sdtm[2];  ///< 2-sides raw(no slewing correction) times
  float edepa;   ///<  Paddle Edep(Anode-made, mev)
  float edepd;   ///<  Paddle Edep(DynodeHiChan-made, mev)
  float time;    ///< Time (ns)
  float cool;     ///< Long.coord.(cm)
  TofRawClusterR(){};
  TofRawClusterR(TOF2RawCluster *ptr);

  ClassDef(TofRawClusterR ,1)       //TofRawClusterR
};


/// TofClusterR structure
/*!
 \author e.choumilov@cern.ch

*/

class TofClusterR {
public:
  int Status;   ///< Statusword
                /*!<
                                                 // bit 4 - ambig
                                                 // bit 128 -> problems with history on any side
                                                 // bit 256 -> Reco: 1side info missing("1-sided" counter)
                                                 // bit 512 -> DBinfo: at least 1 side is bad for t-meas
					         // bit 1024 -> missing side number(=0/1 for 1-sided counter
                                                 // bit 2048 -> recovered from 
                                                 // 1-sided (bit256 also set)
   */
  int Layer;    ///<  Tof plane 1(top)...4
  int Bar;      ///< Tof Bar(paddle) number 1...10
  float Edep;  ///< TOF energy loss (MeV) from anode
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


  TofClusterR(){};
  TofClusterR(AMSTOFCluster *ptr );
  friend class AMSTOFCluster;
ClassDef(TofClusterR,1)       //TofClusterR
};


/// AntiClusterR structure

/*!
 \author e.choumilov@cern.ch

*/
class AntiClusterR {
public:
  int   Status;   ///< Statusword Bit"256"->1sideSector;"1024"->s2 missing if set,  s1 missing if not
  int   Sector;   ///< //Sector number(1-8)
  int   Ntimes;  ///<Number of time-hits(1st come paired ones)
  int   Npairs;   ///<Numb.of time-hits, made of 2 side-times(paired)
  float Times[16];  ///<Time-hits(ns, wrt LEV1trig-time)
  float Timese[16]; ///<Time-hits errors(ns)
  float Edep;    ///<Edep(mev)
  float Coo[3];   ///<R(cm),Phi(degr),Z(cm)-coordinates
  float ErrorCoo[3]; ///<Their errors

  AntiClusterR(){};
  AntiClusterR(AMSAntiCluster *ptr);
ClassDef(AntiClusterR,1)       //AntiClusterR
};


/// TrRawClusterR structure

/*!
 \author v.choutko@cern.ch

*/
class TrRawClusterR {
public:
  int address;   ///<Address \sa TrClusterR Idsoft
  int nelem;     ///< number of strips 
  float s2n;     ///< signal/noise for seed strip

  TrRawClusterR(){};
  TrRawClusterR(AMSTrRawCluster *ptr);
ClassDef(TrRawClusterR,1)       //TrRawClusterR
};


/// TrClusterR structure  
/*!
 \author v.choutko@cern.ch

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
  int Status;  ///< statusword 
               /*!<
 1 - REFITTED object                                     (status&1     !=0) \n
 2 - WIDE in shape (Tracker)                             (status&2     !=0) \n
 3 - AMBIGously associated                               (status&4     !=0) \n
 4 - RELEASED object                                     (status&8     !=0) \n
 5 - BAD                                                 (status&16    !=0) \n
 6 - USED as a component of a larger object              (status&32    !=0) \n
 7 - DELETED object                                      (status&64    !=0) \n
 8 - BADHIStory (TOF)                                    (status&128   !=0) \n
 9 - ONESIDE measurement (TOF)                           (status&256   !=0) \n
10 - BADTIME information (TOF)                           (status&512   !=0) \n
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
  int NelemL;   ///< -Number of strips left to max
  int NelemR;   ///< Number of strips right to max
  float Sum;    ///< Amplitude total
  float Sigma;  ///< Sigma total
  float Mean;   ///< CofG (local)
  float RMS;    ///< RMS cluster
  float ErrorMean; ///< error in CofG
  vector<float> Amplitude;   ///< strips amplitudes (NelemR-NelemL)

  TrClusterR(){};
  TrClusterR(AMSTrCluster *ptr);
ClassDef(TrClusterR,1)       //TrClusterR
};



/// TrRecHitR structure   (3D- tracker clusters)
/*!
 \author v.choutko@cern.ch

*/

class TrRecHitR {
public:
  int   Status;   ///< statusword \sa TrClusterR Status
  int   Layer;    ///<Layer no 1-6 up-down
  float Hit[3];   ///< cluster coordinates
  float EHit[3];  ///< error to above
  float Sum;      ///< Amplitude (x+y)
  float DifoSum;  ///< (A_x-A_y)/(A_x+A_y)
  float CofgX;    ///< local cfg x
  float CofgY;    ///< local cfg y

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

  TrRecHitR(){};
  TrRecHitR(AMSTrRecHit *ptr);
  friend class AMSTrRecHit;
ClassDef(TrRecHitR,1)       //TrRecHitR
};



/// TrTrackR structure 
/*!
 \author v.choutko@cern.ch

*/

class TrTrackR {
public:
  int Status;   ///< statusword \sa TrClusterR
  int Pattern;  ///< see datacards.doc
  int Address;  ///< ladders combination code (see trrec.C buildaddress)
  float LocDBAver;   ///< mean ratio of true momentum to measured one from testbeam
  int   GeaneFitDone; ///<  != 0 if done
  int   AdvancedFitDone;  ///< != 0 if done
  float Chi2StrLine;  ///< chi2 sz fit
  float Chi2Circle;  ///< chi2 circular fit
  float CircleRigidity;  ///< circular rigidity
  float Chi2FastFit;    ///< chi2 fast nonl fit
  float Rigidity;  ///< fast nonl rigidity
  float ErrRigidity;  ///<err to 1/above
  float Theta;      ///< theta (from fast)
  float Phi;        ///< phi (from fast) 
  float P0[3];      ///< coo (from fast)
  float GChi2;     ///< geane chi2
  float GRigidity;  ///< geane rigidity
  float GErrRigidity; ///< geane err(1/rigidity)
  float HChi2[2];     ///< two halves chi2s
  float HRigidity[2];  ///< two halves rigidities
  float HErrRigidity[2];  ///< two halves err(1/rig)
  float HTheta[2];   ///< two halves theta
  float HPhi[2];     ///< two halves phi
  float HP0[2][3];   ///< two halves coo
  float FChi2MS;    ///< fast chi2 mscat off
  float PiErrRig;    ///< PathInt err(1/rig) (<0 means fit was not succesful)
  float RigidityMS;  ///< fast rigidity mscat off
  float PiRigidity;  ///<  PathInt rigidity
  TrTrackR(){};
  TrTrackR(AMSTrTrack *ptr);
protected:
  vector<int> fTrRecHit;
public:
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
ClassDef(TrTrackR,1)       //TrTrackR
friend class AMSTrTrack;
};

/// TRDRawHitR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdRawHitR {
public:
  int Layer;   ///< Layer 0(bottom)...19(top) 
  int Ladder;  ///<  Ladder  number
  int Tube;    ///< tube number
  float Amp;   ///< amplitude (adc counts)

  TrdRawHitR(){};
  TrdRawHitR(AMSTRDRawHit *ptr);

ClassDef(TrdRawHitR,1)       //TrdRawHitR
};

/// TRDClusterR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdClusterR {
public:
  int   Status;    ///< statusword
  float Coo[3];    ///<cluster coo (cm)
  int   Layer;     ///<Layer 0(bottom)...19(top) 
  float CooDir[3]; ///<cluster directional cos (z == along the tube) 
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
  TrdClusterR(){};
  TrdClusterR(AMSTRDCluster *ptr);
  friend class AMSTRDCluster;
ClassDef(TrdClusterR,1)       //TrdClusterR
};

/// TRDSegmentR structure
/*!
 \author v.choutko@cern.ch

*/


class TrdSegmentR {
public:
  int   Status;            ///< status word
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
  TrdSegmentR(){};
  TrdSegmentR(AMSTRDSegment *ptr);
  virtual ~TrdSegmentR(){};
ClassDef(TrdSegmentR,1)       //TrdSegmentR
};


/// TRDTrackR structure
/*!
 \author v.choutko@cern.ch

*/

class TrdTrackR {
public:
  int   Status;  ///< statusword
  float Coo[3];  ///< trac coo (cm)
  float ErCoo[3];  ///< err to coo
  float Phi;     ///<  phi (rads)
  float Theta;   ///< Theta
  float Chi2;   ///<  Chi2
  int   Pattern;  ///< pattern no
  protected:
  vector<int> fTrdSegment;
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
  TrdTrackR(AMSTRDTrack *ptr);
  TrdTrackR(){};
  friend class AMSTRDTrack;
ClassDef(TrdTrackR,1)       //TrdTrackR
};


/// Level1 trigger structure 
/*!
 \author e.choumilov@cern.ch

*/

class Level1R {
public:
  int   Mode;   ///< reserved
  int   TofFlag;   ///< <0->noTOF,=0->4planes,(1-8)->miss.pln.code(at least 1top+1bot required), +10->Z>=2
  int   TofPatt[4]; ///< 4-layers TOF pattern(separately for each side): 

                    /*!<
                                                       1-10  bits  Side-1  \n
                                                       17-26       Side-2  \n
                   */
  int   TofPatt1[4]; ///< same TOF pattern for Z>=2 trigger
  int   AntiPatt;   ///< Antipatt:(1-8)/(9-16)bits->s1(bot)/s2(top), (17-24)->fired sect-ends counters(4) x,z><0   
  int   EcalFlag;   ///< =MN, where 
                    /*!< 
                          M=0/1/2/3->Etot<Mip / Etot>Mip / Etot>LowThr / Etot>HighThr; \n
                          N=2/1/0->ShowerWidthTest=OK/Bad/Unknown
                    */
  float EcalTrSum; ///< EC-energy trig.sum(Gev)                    

  Level1R(){};
  Level1R(Trigger2LVL1 *ptr);
ClassDef(Level1R,1)       //Level1R
};


/// Level3 trigger structure 
/*!
 \author v.choutko@cern.ch

*/

class Level3R {
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
ClassDef(Level3R,1)       //Level3R
};






/// Tof Beta (Velocity/c) structure
/*!
 \author v.choutko@cern.ch

*/

class BetaR {
 public:
  int       Status;   ///< status word  4 - ambig
  int       Pattern;   ///< beta pattern \sa beta.doc
  float     Beta;     ///< velocity
  float     BetaC;    ///< corrected velocity (taking into account true velocity < 1)
  float     Error;   ///< error (1/beta)
  float     ErrorC;  ///< error (1/betac)
  float     Chi2;    ///<chi2 of beta fit(time)
  float     Chi2S;   ///<chi2 of beta fit(space)
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
   BetaR(){};
   BetaR(AMSBeta *ptr);
   friend class AMSBeta;
   virtual ~BetaR(){};
   ClassDef(BetaR,1)         //BetaR
};



/// Voted Charge Structure
/*!
    Particle Charge Determination by Trd, Tof, Tr and Rich detectors \n
    Still Incomplete \n
    Only Tr, Tof aand Rich re used at the moment \n
*/    
class ChargeR {
/*!
  \author Carlos.Jose.Delgado.Mendez@cern.ch

*/

public:
  int Status;     ///<  Status (1-refitted)
  int ChargeTOF;  ///<TOF Charge
  int ChargeTracker;  ///< Tracker Charge
  int ChargeRich;      ///<Rich Charge 
  float ProbTOF[4];    ///< TOF highest Probs
  int ChInTOF[4];      ///< charge indices for highest Probs (see charge.doc)
  float ProbTracker[4]; ///< Tracker highest Probs
  int ChInTracker[4];     ///< charge indices for highest Probs (see charge.doc)
  float ProbRich[4];    ///< rich highest Probs
  int ChInRich[4];          ///< charge indices for highest Probs (see charge.doc)
  float ProbAllTracker;  ///< Tracker highest Prob (all hits)
  float TrunTOF;          ///< Truncated (-1) mean   (Tof Anodes)     
  float TrunTOFD;         ///< Truncated (-1) mean  (Tof Dynodes)     
  float TrunTracker;     ///< Truncated (-1) mean   Tracker  
protected:
  int  fBeta;        ///< index of BetaR used
  int  fRichRing;        ///< index of RichRingR used 
public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();
  /// access function to RichRingR object used
  /// \return index of RichRingR object in collection or -1
  int iRichRing()const {return fRichRing;}
  /// access function to RichRingR object used
  /// \return pointer to RichRingR object or 0
  RichRingR * pRichRing();

   friend class AMSCharge;
  ChargeR(){};
  ChargeR(AMSCharge *ptr, float probtof[],int chintof[],
               float probtr[], int chintr[], 
               float probrc[], int chinrc[], float proballtr);
ClassDef(ChargeR,1)       //ChargeR

};

/*!
  \class VertexR
  \brief converted gamma class

   \sa gamma.h gamma.C
   \author g.lamanna@cern.ch
*/
class VertexR {
 public:
public:
  int   Status;       //
  float Mass;       ///< rec gamma mass
  float Momentum;     ///< Momentum (gev/c)
  float ErrMomentum;  ///< Error to Momentum 
  float Theta;       ///< Theta (rad)
  float Phi;        ///< Phi  (rad)
  float Vertex[3];    ///< reconstructed vertex cm)
  float Distance;   ///< min distance between tracks (cm)
protected:
  int   fTrTrackL;    ///< index to left TrTrack used
  int   fTrTrackR;    ///< index to right TrTrack used
public:
  /// access function to  TrTrackR objects used
  /// \param ptr 0 -> Left Track, 1++ -> RightTrack
  /// \return index of TrTrackR object in collection or -1
  int iTrTrack(unsigned int ptr)const {return ptr==0?fTrTrackL:fTrTrackR;}
  /// access function to  TrTrackR objects used
  /// \param ptr 0 -> Left Track, 1++ -> RightTrack
  /// \return pointer to  TrTrackR object or 0
  TrTrackR * pTrTrack(unsigned int ptr);
   VertexR(){};
   VertexR(AMSTrTrackGamma *ptr);
   friend class AMSTrTrackGamma;
   ClassDef(VertexR,1)         //VertexR
};





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
     - Particle based on VertexR (i.e. converted photon candidate):
      -# Charge set to 0
      -# Velocity may or may not be set depending on fBeta index

\author v.choutko@cern.ch
      
*/     
         
   
   
class ParticleR {
public:
  int Status;   ///< status word (not really used at the moment)
  int   Particle;     ///< voted particle id a-la geant3 (not really reliable)
  int   ParticleVice; ///< nect-to voted particle id a-la geant3 (not really reliable)
  float Prob[2];      ///< Probability to be a particle Particle [0] or ParticleVice [1]
  float FitMom;       ///< Fitted Momentum to particle Particle
  float Mass;         ///< Mass (Gev/c2)
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
  float TOFCoo[4][3]; ///< track extrapol in tof planes
  float AntiCoo[2][3]; ///< track extrapol in anti
  float EcalCoo[3][3]; ///< track extrapol in ecal (enter, cofg, exit)
  float TrCoo[8][3];  ///< track extrapol in tracker planes
  float TRDCoo[3];    ///< track extrapol in trd (center)
  float RichCoo[2][3];  ///< track extrapol in rich (radiator_pos, pmt_pos)
  float RichPath[2];    ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta=1
  float RichPathBeta[2]; ///<  Estimated fraction  of ring photons  within RICH acceptance (direct and reflected ones  respectively) for beta Beta
  float RichLength; ///< Estimated pathlength of particle within rich radiator (cm)
  float Local[8];  ///< contains the minimal distance to sensor edge in sensor length units ;
  float TRDLikelihood; ///< TRD likelihood  (whatever is it)
protected: 
  int  fBeta;          ///<index of  BetaR used
  int  fCharge;        ///<index of  ChargeR used
  int  fTrTrack;      ///<index of  TrTrackR used
  int  fTrdTrack;     ///<index of  TrdTrackR used
  int  fRichRing;     ///<index of  RichringR used
  int  fEcalShower;   ///<index of  EcalShowerR used
  int  fVertex;       ///<index of  VertexR used
public:
  /// access function to BetaR object used
  /// \return index of BetaR object in collection or -1
  int iBeta()const {return fBeta;}
  /// access function to BetaR object used
  /// \return pointer to BetaR object or 0
  BetaR * pBeta();

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

  /// access function to RichRingR object used
  /// \return index of RichRingR object in collection or -1
  int iRichRing()const {return fRichRing;}
  /// access function to RichRingR object used
  /// \return pointer to RichRingR object or 0
  RichRingR * pRichRing();

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

  ParticleR(){};
  ParticleR(AMSParticle *ptr, float phi, float phigl);
  friend class AMSParticle;
  ClassDef(ParticleR,1)       //ParticleR
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
ClassDef(AntiMCClusterR,1)       //AntiMCClusterR
};

/// TrMCCluster structure
/*!

   \author v.choutko@cern.ch
*/
class TrMCClusterR {
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
ClassDef(TrMCClusterR,1)       //TrMCClusterR
};


/// TofMCCluster structure
/*!

   \author e.choumilov@cern.ch
*/
class TofMCClusterR {
public:
  int   Idsoft;  ///< software id  (ask E.Choumilov for details)
  float Coo[3];  ///< coo cm
  float TOF;     ///<  time of flight sec
  float Edep;    ///< energy dep mev
  TofMCClusterR(){};
  TofMCClusterR(AMSTOFMCCluster *ptr);
ClassDef(TofMCClusterR,1)       //TOFMCClusterRoot
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
ClassDef(TrdMCClusterR,1)       //TrdMCClusterR
};


/// RichMCCluster structure
/*!

   \author Carlos.Jose.Delgado.Mendez@cern.ch
*/
class RichMCClusterR {
public:
  int   Id;            ///< Particle id, -666 if noise
  float Origin[3];     ///< Particle origin coo
  float Direction[3];  ///< Particle direction cos
  int   Status;        ///< Status=10*number of reflections+(have it rayleigh?1:0)
                       /*!<
*******) For geant4 this value is 0. For geant 3it has several meanings:

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



NOTE: The information of the mother is only available if RICCONT=1 in
      the datacards
*/

  int   NumGen;        ///< Number of generated photons
  int   fRichHit;      ///< Pointer to detected hit
  RichMCClusterR(){};
  RichMCClusterR(AMSRichMCHit *ptr, int _numgen);
  ClassDef(RichMCClusterR,1)       // RichMCClusterR
};



/// MCTrack structure

/*! 
Contains radiation/absorption length ticknesses. 
 Activated bt SCAN TRUE datacard
\author v.choutko@cern.ch
*/

class MCTrackR {
public:
float RadL;   ///< integrated radiation length 
float AbsL;   ///< integrated nuclear absorption length 
float Pos[3];  ///< x,y,z (cm) 
char  VolName[5];  ///< Volume name

 MCTrackR(){};
 MCTrackR(AMSmctrack *ptr);
ClassDef(MCTrackR,1)       //MCTrackR
};

/// MC particle structure
/*!

   \author v.choutko@cern.ch
*/
class MCEventgR {
public:
  int Nskip;      ///<  reserved
  int Particle;   ///< geant3 particle id
                  /*!<
          (geant3 only) \n
            Particle=g3pid+256 means heavy ion nonelstic
        scattering occured in for pid with dir & momentum at coo \n
        particle=-g3pid means secondary particle produced with
        dir&momentum at coo
*/
  float Coo[3];   ///< coo (cm)
  float Dir[3];   ///< dir cos
  float Momentum;  ///< momentum (gev)
  float Mass;      ///< mass (gev)
  float Charge;    ///< charge (signed)
  MCEventgR(){};
  MCEventgR(AMSmceventg *ptr);
ClassDef(MCEventgR,1)       //MCEventgR
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

    \sa stlv
    \author v.choutko@cern.ch
*/
class AMSEventR: public  TSelector {   
protected:
class Service{
public:
 TFile *            _pOut;
 TStopwatch         _w;
 unsigned int       TotalEv;
 unsigned int       BadEv;
Service():_pOut(0),TotalEv(0),BadEv(0){}
};
Service  fService;
static TBranch*  bHeader;
static TBranch*  bEcalHit;
static TBranch*  bEcalCluster;
static TBranch*  bEcal2DCluster;
static TBranch*  bEcalShower;
static TBranch*  bRichHit;
static TBranch*  bRichRing;
static TBranch*  bTofRawCluster;
static TBranch*  bTofCluster;
static TBranch*  bAntiCluster;
static TBranch*  bTrRawCluster;
static TBranch*  bTrCluster;
static TBranch*  bTrRecHit;
static TBranch*  bTrTrack;
static TBranch*  bTrdRawHit;
static TBranch*  bTrdCluster;
static TBranch*  bTrdSegment;
static TBranch*  bTrdTrack;
static TBranch*  bLevel1;
static TBranch*  bLevel3;
static TBranch*  bBeta;
static TBranch*  bVertex;
static TBranch*  bCharge;
static TBranch*  bParticle;
static TBranch*  bAntiMCCluster;
static TBranch*  bTrMCCluster;
static TBranch*  bTofMCCluster;
static TBranch*  bTrdMCCluster;
static TBranch*  bRichMCCluster;
static TBranch*  bMCTrack;
static TBranch*  bMCEventg;
static TBranch*  bAux;


static void*  vHeader;
static void*  vEcalHit;
static void*  vEcalCluster;
static void*  vEcal2DCluster;
static void*  vEcalShower;
static void*  vRichHit;
static void*  vRichRing;
static void*  vTofRawCluster;
static void*  vTofCluster;
static void*  vAntiCluster;
static void*  vTrRawCluster;
static void*  vTrCluster;
static void*  vTrRecHit;
static void*  vTrTrack;
static void*  vTrdRawHit;
static void*  vTrdCluster;
static void*  vTrdSegment;
static void*  vTrdTrack;
static void*  vLevel1;
static void*  vLevel3;
static void*  vBeta;
static void*  vVertex;
static void*  vCharge;
static void*  vParticle;
static void*  vAntiMCCluster;
static void*  vTrMCCluster;
static void*  vTofMCCluster;
static void*  vTrdMCCluster;
static void*  vRichMCCluster;
static void*  vMCTrack;
static void*  vMCEventg;
static void*  vAux;

static TTree     * _Tree;

static AMSEventR * _Head;
static int         _Count;
static int         _Entry;
static char      * _Name;
public:
 static AMSEventR* & Head()  {return _Head;}
 static char *  BranchName() {return _Name;}
 void SetBranchA(TTree *tree);
 void CreateBranch(TTree *tree, int brs);
 void ReSetBranchA(TTree *tree);
 void GetBranch(TTree *tree);
 void GetBranchA(TTree *tree);
 void SetCont(); 
 int & Entry(){return _Entry;}
protected:
//
//  TSelector functions

   /// System function called before starting the event loop.
   /// Initializes the tree branches.
   /// Optionally opens the output file via chain.Process("xyz.C+","outputfilename");
   /// calls user UBegin()  function
   void    Begin(TTree *tree);

   /// System function called when loading a new file.
   /// Get branch pointers.
   bool  Notify(){GetBranch(_Tree);return true;}
   void    Init(TTree *tree);   ///< InitTree


   void    SetOption(const char *option) { fOption = option; }
   void    SetObject(TObject *obj) { fObject = obj; }
   void    SetInputList(TList *input) {fInput = input;}
   TList  *GetOutputList() const { return fOutput; }
   /// System Function called at the end of a loop on the tree
   ///  optionally stores histos in a file \sa Begin 
   /// Calls UTerminate();
   void    Terminate();



public:




// TSelector user functions
 
   /// User Analysis function called before starting the event loop.
   /// Place to book histos etc
   /// \sa stlv.C
   virtual void UBegin();
   /// User Analysis Function called at the end of a loop on the tree,
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
   Bool_t  ProcessCut(int entry){return ReadHeader(entry);}

  ///  Reads Header 
  /// \param Entry - event no
   /// \return false if error;
  bool ReadHeader(int Entry);

public:
 
  HeaderR  fHeader;  ///<  Event Header \sa HeaderR


unsigned int Run() const {return fHeader.Run;} ///< \return run number
unsigned Event() const {return fHeader.Event;} ///< \return event number
unsigned Time() const {return fHeader.Time[0];} ///< \return event time
int Version() const {return fHeader.Version/4;} ///< \return producer version number
int OS() const {return fHeader.Version%4;}   ///< \return producer Op Sys number  (0 -undef, 1 -dunix, 2 -linux 3 - sun )

int   nEcalHit()const { return fHeader.EcalHits;} ///< \return number of EcalHitR elements (fast)              
int   nEcalCluster()const { return fHeader.EcalClusters;} ///< \return number of EcalClusterR elements (fast)
int   nEcal2DCluster()const { return fHeader.Ecal2DClusters;} ///< \return number of Ecal2DClusterR elements (fast)
int   nEcalShower()const { return fHeader.EcalShowers;} ///< \return number of EcalShowerR elements (fast)
int   nRichHit()const { return fHeader.RichHits;} ///< \return number of RichHitR elements (fast)
int   nRichRing()const { return fHeader.RichRings;} ///< \return number of RichRingR elements (fast)
int   nTofRawCluster()const { return fHeader.TofRawClusters;} ///< \return number of TofRawClusterR elements (fast)
int   nTofCluster()const { return fHeader.TofClusters;} ///< \return number of TofClusterR elements (fast)  
int   nAntiCluster()const { return fHeader.AntiClusters;} ///< \return number of AntiClusterR elements (fast)
int   nTrRawCluster()const { return fHeader.TrRawClusters;} ///< \return number of TrRawClusterR elements (fast)
int   nTrCluster()const { return fHeader.TrClusters;} ///< \return number of TrClusterR elements (fast)
int   nTrRecHit()const { return fHeader.TrRecHits;} ///< \return number of TrRecHitR elements (fast)
int   nTrTrack()const { return fHeader.TrTracks;} ///< \return number of TrTrackR elements (fast)
int   nTrdRawHit()const { return fHeader.TrdRawHits;} ///< \return number of TrdRawHitR elements (fast)
int   nTrdCluster()const { return fHeader.TrdClusters;} ///< \return number of TrdClusterR elements (fast)
int   nTrdSegment()const { return fHeader.TrdSegments;} ///< \return number of TrdSegmentR elements (fast)
int   nTrdTrack()const { return fHeader.TrdTracks;} ///< \return number of TrdTrackR elements (fast)
int   nLevel1()const { return fHeader.Level1s;} ///< \return number of Level1R elements (fast)
int   nLevel3()const { return fHeader.Level3s;} ///< \return number of Level3R elements (fast)
int   nBeta()const { return fHeader.Betas;} ///< \return number of BetaR elements (fast) 
int   nVertex()const { return fHeader.Vertexs;} ///< \return number of VertexR elements (fast) 
int   nCharge()const { return fHeader.Charges;} ///< \return number of ChargeR elements (fast)  
int   nParticle()const { return fHeader.Particles;} ///< \return number of ParticleR elements (fast)  
int   nAntiMCCluster()const { return fHeader.AntiMCClusters;} ///< \return number of AntiMCClusterR elements (fast)
int   nTrMCCluster()const { return fHeader.TrMCClusters;} ///< \return number of TrMCClusterR elements (fast)
int   nTofMCCluster()const { return fHeader.TofMCClusters;} ///< \return number of TofMCClusterR elements (fast)
int   nTrdMCCluster()const { return fHeader.TrdMCClusters;} ///< \return number of TrdMCClusterR elements (fast)
int   nRichMCCluster()const { return fHeader.RichMCClusters;} ///< \return number of RichMCClusterR elements (fast)
int   nMCTrack()const { return fHeader.MCTracks;} ///< \return number of MCTrackR elements (fast)
int   nMCEventg()const { return fHeader.MCEventgs;} ///< \return number of MCEventgR elements (fast)



  protected:
  
  //ECAL 

  vector<EcalHitR> fEcalHit;     
  vector<EcalClusterR> fEcalCluster;
  vector<Ecal2DClusterR> fEcal2DCluster;
  vector<EcalShowerR> fEcalShower;


  //RICH
  vector<RichHitR> fRichHit;
  vector<RichRingR> fRichRing;



  //TOF
  vector<TofRawClusterR> fTofRawCluster;
  vector<TofClusterR> fTofCluster;  


  //Anti

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


  //Triggers

  vector<Level1R> fLevel1;
  vector<Level3R> fLevel3;


  //AxAMS
  vector<BetaR> fBeta; 
  vector<ChargeR> fCharge;  
  vector<VertexR> fVertex;  
  vector<ParticleR> fParticle;  



  //MC SubDet
  vector<AntiMCClusterR> fAntiMCCluster;
  vector<TrMCClusterR>   fTrMCCluster;
  vector<TofMCClusterR>  fTofMCCluster;
  vector<TrdMCClusterR>  fTrdMCCluster;
  vector<RichMCClusterR> fRichMCCluster;


  //MC General

  vector<MCTrackR>       fMCTrack;
  vector<MCEventgR>      fMCEventg;


  //Aux

   vector<float>         fAux;

   public:

      ///  \return number of EcalHitR
      unsigned int   NEcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return fEcalHit.size();
      }
      ///  \return reference of EcalHitR Collection
      vector<EcalHitR> & EcalHit()  {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return  fEcalHit;
       }

       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return reference to corresponding EcalHitR element
       EcalHitR &   EcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
         return fEcalHit.at(l);
      }

       ///  EcalHitR accessor
       /// \param l index of EcalHitR Collection
      ///  \return pointer to corresponding EcalHitR element
      EcalHitR *   pEcalHit(unsigned int l) {
        if(fHeader.EcalHits && fEcalHit.size()==0)bEcalHit->GetEntry(_Entry);
        return l<fEcalHit.size()?&(fEcalHit[l]):0;
      }

      ///  EcalClusterR accessor
      ///  \return number of EcalClusterR
      unsigned int   NEcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return fEcalCluster.size();
      }
      ///  EcalClusterR accessor
      ///  \return reference of EcalClusterR Collection
      vector<EcalClusterR> & EcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return  fEcalCluster;
       }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return reference to corresponding EcalClusterR element
       EcalClusterR &   EcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
         return fEcalCluster.at(l);
      }

       ///  EcalClusterR accessor
       /// \param l index of EcalClusterR Collection
      ///  \return pointer to corresponding EcalClusterR element
      EcalClusterR *   pEcalCluster(unsigned int l) {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return l<fEcalCluster.size()?&(fEcalCluster[l]):0;
      }


      
      ///  Ecal2DClusterR accessor
      ///  \return number of Ecal2DClusterR
      unsigned int   NEcal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return fEcal2DCluster.size();
      }
            ///  Ecal2DClusterR accessor
      ///  \return reference of Ecal2DClusterR Collection
      vector<Ecal2DClusterR> & Ecal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return  fEcal2DCluster;
       }
             ///  Ecal2DClusterR accessor
      /// \param l index of Ecal2DClusterR Collection
     ///  \return reference to corresponding Ecal2DClusterR element
       Ecal2DClusterR &   Ecal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return fEcal2DCluster.at(l);
      }
                 ///  Ecal2DClusterR accessor
    /// \param l index of Ecal2DClusterR Collection
   ///  \return pointer to corresponding Ecal2DClusterR element
      Ecal2DClusterR *   pEcal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return l<fEcal2DCluster.size()?&(fEcal2DCluster[l]):0;
      }


      ///  \return number of EcalShowerR
      unsigned int   NEcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return fEcalShower.size();
      }
       ///  \return reference of EcalShowerR Collection
      vector<EcalShowerR> & EcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return  fEcalShower;
       }
                 ///  EcalShowerR accessor
   /// \param l index of EcalShowerR Collection
  ///  \return reference to corresponding EcalShowerR element
       EcalShowerR &   EcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return fEcalShower.at(l);
      }
                ///  EcalShowerR accessor
  /// \param l index of EcalShowerR Collection
 ///  \return pointer to corresponding EcalShowerR element
      EcalShowerR *   pEcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return l<fEcalShower.size()?&(fEcalShower[l]):0;
      }



      ///  \return number of RichHitR
      unsigned int   NRichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return fRichHit.size();
      }
      ///  \return reference of RichHitR Collection
      vector<RichHitR> & RichHit()  {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return  fRichHit;
       }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return reference to corresponding RichHitR element
       RichHitR &   RichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
         return fRichHit.at(l);
      }

       ///  RichHitR accessor
       /// \param l index of RichHitR Collection
      ///  \return pointer to corresponding RichHitR element
      RichHitR *   pRichHit(unsigned int l) {
        if(fHeader.RichHits && fRichHit.size()==0)bRichHit->GetEntry(_Entry);
        return l<fRichHit.size()?&(fRichHit[l]):0;
      }






      ///  \return number of RichRingR
      unsigned int   NRichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
        return fRichRing.size();
      }
      ///  \return reference of RichRingR Collection
      vector<RichRingR> & RichRing()  {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return  fRichRing;
       }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return reference to corresponding RichRingR element
       RichRingR &   RichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
         return fRichRing.at(l);
      }

       ///  RichRingR accessor
       /// \param l index of RichRingR Collection
      ///  \return pointer to corresponding RichRingR element
      RichRingR *   pRichRing(unsigned int l) {
        if(fHeader.RichRings && fRichRing.size()==0)bRichRing->GetEntry(_Entry);
        return l<fRichRing.size()?&(fRichRing[l]):0;
      }






      ///  \return number of TofRawClusterR
      unsigned int   NTofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return fTofRawCluster.size();
      }
      ///  \return reference of TofRawClusterR Collection
      vector<TofRawClusterR> & TofRawCluster()  {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return  fTofRawCluster;
       }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return reference to corresponding TofRawClusterR element
       TofRawClusterR &   TofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
         return fTofRawCluster.at(l);
      }

       ///  TofRawClusterR accessor
       /// \param l index of TofRawClusterR Collection
      ///  \return pointer to corresponding TofRawClusterR element
      TofRawClusterR *   pTofRawCluster(unsigned int l) {
        if(fHeader.TofRawClusters && fTofRawCluster.size()==0)bTofRawCluster->GetEntry(_Entry);
        return l<fTofRawCluster.size()?&(fTofRawCluster[l]):0;
      }




      ///  \return number of TofClusterR
      unsigned int   NTofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return fTofCluster.size();
      }
      ///  \return reference of TofClusterR Collection
      vector<TofClusterR> & TofCluster()  {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return  fTofCluster;
       }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return reference to corresponding TofClusterR element
       TofClusterR &   TofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
         return fTofCluster.at(l);
      }

       ///  TofClusterR accessor
       /// \param l index of TofClusterR Collection
      ///  \return pointer to corresponding TofClusterR element
      TofClusterR *   pTofCluster(unsigned int l) {
        if(fHeader.TofClusters && fTofCluster.size()==0)bTofCluster->GetEntry(_Entry);
        return l<fTofCluster.size()?&(fTofCluster[l]):0;
      }




      ///  \return number of AntiClusterR
      unsigned int   NAntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return fAntiCluster.size();
      }
      ///  \return reference of AntiClusterR Collection
      vector<AntiClusterR> & AntiCluster()  {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return  fAntiCluster;
       }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return reference to corresponding AntiClusterR element
       AntiClusterR &   AntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
         return fAntiCluster.at(l);
      }

       ///  AntiClusterR accessor
       /// \param l index of AntiClusterR Collection
      ///  \return pointer to corresponding AntiClusterR element
      AntiClusterR *   pAntiCluster(unsigned int l) {
        if(fHeader.AntiClusters && fAntiCluster.size()==0)bAntiCluster->GetEntry(_Entry);
        return l<fAntiCluster.size()?&(fAntiCluster[l]):0;
      }


      ///  \return number of TrRawClusterR
      unsigned int   NTrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return fTrRawCluster.size();
      }
      ///  \return reference of TrRawClusterR Collection
      vector<TrRawClusterR> & TrRawCluster()  {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return  fTrRawCluster;
       }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return reference to corresponding TrRawClusterR element
       TrRawClusterR &   TrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
         return fTrRawCluster.at(l);
      }

       ///  TrRawClusterR accessor
       /// \param l index of TrRawClusterR Collection
      ///  \return pointer to corresponding TrRawClusterR element
      TrRawClusterR *   pTrRawCluster(unsigned int l) {
        if(fHeader.TrRawClusters && fTrRawCluster.size()==0)bTrRawCluster->GetEntry(_Entry);
        return l<fTrRawCluster.size()?&(fTrRawCluster[l]):0;
      }




      ///  \return number of TrClusterR
      unsigned int   NTrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return fTrCluster.size();
      }
      ///  \return reference of TrClusterR Collection
      vector<TrClusterR> & TrCluster()  {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return  fTrCluster;
       }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return reference to corresponding TrClusterR element
       TrClusterR &   TrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
         return fTrCluster.at(l);
      }

       ///  TrClusterR accessor
       /// \param l index of TrClusterR Collection
      ///  \return pointer to corresponding TrClusterR element
      TrClusterR *   pTrCluster(unsigned int l) {
        if(fHeader.TrClusters && fTrCluster.size()==0)bTrCluster->GetEntry(_Entry);
        return l<fTrCluster.size()?&(fTrCluster[l]):0;
      }




       ///  TrRecHitR accessor
      ///  \return number of TrRecHitR
      unsigned int   NTrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return fTrRecHit.size();
      }
      ///  \return reference of TrRecHitR Collection
      vector<TrRecHitR> & TrRecHit()  {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return  fTrRecHit;
       }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return reference to corresponding TrRecHitR element
       TrRecHitR &   TrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
         return fTrRecHit.at(l);
      }

       ///  TrRecHitR accessor
       /// \param l index of TrRecHitR Collection
      ///  \return pointer to corresponding TrRecHitR element
      TrRecHitR *   pTrRecHit(unsigned int l) {
        if(fHeader.TrRecHits && fTrRecHit.size()==0)bTrRecHit->GetEntry(_Entry);
        return l<fTrRecHit.size()?&(fTrRecHit[l]):0;
      }


       ///  TrTrackR accessor
      ///  \return number of TrTrackR
      unsigned int   NTrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return fTrTrack.size();
      }
      ///  \return reference of TrTrackR Collection
      vector<TrTrackR> & TrTrack()  {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return  fTrTrack;
       }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return reference to corresponding TrTrackR element
       TrTrackR &   TrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
         return fTrTrack.at(l);
      }

       ///  TrTrackR accessor
       /// \param l index of TrTrackR Collection
      ///  \return pointer to corresponding TrTrackR element
      TrTrackR *   pTrTrack(unsigned int l) {
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        return l<fTrTrack.size()?&(fTrTrack[l]):0;
      }




       ///  TrdRawHitR accessor
      ///  \return number of TrdRawHitR
      unsigned int   NTrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return fTrdRawHit.size();
      }
      ///  \return reference of TrdRawHitR Collection
      vector<TrdRawHitR> & TrdRawHit()  {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return  fTrdRawHit;
       }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return reference to corresponding TrdRawHitR element
       TrdRawHitR &   TrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
         return fTrdRawHit.at(l);
      }

       ///  TrdRawHitR accessor
       /// \param l index of TrdRawHitR Collection
      ///  \return pointer to corresponding TrdRawHitR element
      TrdRawHitR *   pTrdRawHit(unsigned int l) {
        if(fHeader.TrdRawHits && fTrdRawHit.size()==0)bTrdRawHit->GetEntry(_Entry);
        return l<fTrdRawHit.size()?&(fTrdRawHit[l]):0;
      }


       ///  TrdClusterR accessor
      ///  \return number of TrdClusterR
      unsigned int   NTrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return fTrdCluster.size();
      }
      ///  \return reference of TrdClusterR Collection
      vector<TrdClusterR> & TrdCluster()  {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return  fTrdCluster;
       }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return reference to corresponding TrdClusterR element
       TrdClusterR &   TrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
         return fTrdCluster.at(l);
      }

       ///  TrdClusterR accessor
       /// \param l index of TrdClusterR Collection
      ///  \return pointer to corresponding TrdClusterR element
      TrdClusterR *   pTrdCluster(unsigned int l) {
        if(fHeader.TrdClusters && fTrdCluster.size()==0)bTrdCluster->GetEntry(_Entry);
        return l<fTrdCluster.size()?&(fTrdCluster[l]):0;
      }


       ///  TrdSegmentR accessor
      ///  \return number of TrdSegmentR
      unsigned int   NTrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return fTrdSegment.size();
      }
      ///  \return reference of TrdSegmentR Collection
      vector<TrdSegmentR> & TrdSegment()  {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return  fTrdSegment;
       }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return reference to corresponding TrdSegmentR element
       TrdSegmentR &   TrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
         return fTrdSegment.at(l);
      }

       ///  TrdSegmentR accessor
       /// \param l index of TrdSegmentR Collection
      ///  \return pointer to corresponding TrdSegmentR element
      TrdSegmentR *   pTrdSegment(unsigned int l) {
        if(fHeader.TrdSegments && fTrdSegment.size()==0)bTrdSegment->GetEntry(_Entry);
        return l<fTrdSegment.size()?&(fTrdSegment[l]):0;
      }



       ///  TrdTrackR accessor
      ///  \return number of TrdTrackR
      unsigned int   NTrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return fTrdTrack.size();
      }
      ///  \return reference of TrdTrackR Collection
      vector<TrdTrackR> & TrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return  fTrdTrack;
       }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return reference to corresponding TrdTrackR element
       TrdTrackR &   TrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return fTrdTrack.at(l);
      }

       ///  TrdTrackR accessor
       /// \param l index of TrdTrackR Collection
      ///  \return pointer to corresponding TrdTrackR element
      TrdTrackR *   pTrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return l<fTrdTrack.size()?&(fTrdTrack[l]):0;
      }




       ///  BetaR accessor
      ///  \return number of BetaR
      unsigned int   NBeta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return fBeta.size();
      }
      ///  \return reference of BetaR Collection
      vector<BetaR> & Beta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return  fBeta;
       }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return reference to corresponding BetaR element
       BetaR &   Beta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return fBeta.at(l);
      }

       ///  BetaR accessor
       /// \param l index of BetaR Collection
      ///  \return pointer to corresponding BetaR element
      BetaR *   pBeta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return l<fBeta.size()?&(fBeta[l]):0;
      }


       ///  ChargeR accessor
      ///  \return number of ChargeR
      unsigned int   NCharge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return fCharge.size();
      }
      ///  \return reference of ChargeR Collection
      vector<ChargeR> & Charge()  {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return  fCharge;
       }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return reference to corresponding ChargeR element
       ChargeR &   Charge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
         return fCharge.at(l);
      }

       ///  ChargeR accessor
       /// \param l index of ChargeR Collection
      ///  \return pointer to corresponding ChargeR element
      ChargeR *   pCharge(unsigned int l) {
        if(fHeader.Charges && fCharge.size()==0)bCharge->GetEntry(_Entry);
        return l<fCharge.size()?&(fCharge[l]):0;
      }


       ///  VertexR accessor
      ///  \return number of VertexR
      unsigned int   NVertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return fVertex.size();
      }
      ///  \return reference of VertexR Collection
      vector<VertexR> & Vertex()  {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return  fVertex;
       }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return reference to corresponding VertexR element
       VertexR &   Vertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
         return fVertex.at(l);
      }

       ///  VertexR accessor
       /// \param l index of VertexR Collection
      ///  \return pointer to corresponding VertexR element
      VertexR *   pVertex(unsigned int l) {
        if(fHeader.Vertexs && fVertex.size()==0)bVertex->GetEntry(_Entry);
        return l<fVertex.size()?&(fVertex[l]):0;
      }


       ///  ParticleR accessor
      ///  \return number of ParticleR
      unsigned int   NParticle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return fParticle.size();
      }
      ///  \return reference of ParticleR Collection
      vector<ParticleR> & Particle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return  fParticle;
       }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return reference to corresponding ParticleR element
       ParticleR &   Particle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return fParticle.at(l);
      }

       ///  ParticleR accessor
       /// \param l index of ParticleR Collection
      ///  \return pointer to corresponding ParticleR element
      ParticleR *   pParticle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return l<fParticle.size()?&(fParticle[l]):0;
      }




       ///  AntiMCClusterR accessor
      ///  \return number of AntiMCClusterR
      unsigned int   NAntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return fAntiMCCluster.size();
      }
      ///  \return reference of AntiMCClusterR Collection
      vector<AntiMCClusterR> & AntiMCCluster()  {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return  fAntiMCCluster;
       }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return reference to corresponding AntiMCClusterR element
       AntiMCClusterR &   AntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
         return fAntiMCCluster.at(l);
      }

       ///  AntiMCClusterR accessor
       /// \param l index of AntiMCClusterR Collection
      ///  \return pointer to corresponding AntiMCClusterR element
      AntiMCClusterR *   pAntiMCCluster(unsigned int l) {
        if(fHeader.AntiMCClusters && fAntiMCCluster.size()==0)bAntiMCCluster->GetEntry(_Entry);
        return l<fAntiMCCluster.size()?&(fAntiMCCluster[l]):0;
      }





       ///  TofMCClusterR accessor
      ///  \return number of TofMCClusterR
      unsigned int   NTofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return fTofMCCluster.size();
      }
      ///  \return reference of TofMCClusterR Collection
      vector<TofMCClusterR> & TofMCCluster()  {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return  fTofMCCluster;
       }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return reference to corresponding TofMCClusterR element
       TofMCClusterR &   TofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
         return fTofMCCluster.at(l);
      }

       ///  TofMCClusterR accessor
       /// \param l index of TofMCClusterR Collection
      ///  \return pointer to corresponding TofMCClusterR element
      TofMCClusterR *   pTofMCCluster(unsigned int l) {
        if(fHeader.TofMCClusters && fTofMCCluster.size()==0)bTofMCCluster->GetEntry(_Entry);
        return l<fTofMCCluster.size()?&(fTofMCCluster[l]):0;
      }




       ///  TrMCClusterR accessor
      ///  \return number of TrMCClusterR
      unsigned int   NTrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return fTrMCCluster.size();
      }
      ///  \return reference of TrMCClusterR Collection
      vector<TrMCClusterR> & TrMCCluster()  {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return  fTrMCCluster;
       }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return reference to corresponding TrMCClusterR element
       TrMCClusterR &   TrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
         return fTrMCCluster.at(l);
      }

       ///  TrMCClusterR accessor
       /// \param l index of TrMCClusterR Collection
      ///  \return pointer to corresponding TrMCClusterR element
      TrMCClusterR *   pTrMCCluster(unsigned int l) {
        if(fHeader.TrMCClusters && fTrMCCluster.size()==0)bTrMCCluster->GetEntry(_Entry);
        return l<fTrMCCluster.size()?&(fTrMCCluster[l]):0;
      }





       ///  TrdMCClusterR accessor
      ///  \return number of TrdMCClusterR
      unsigned int   NTrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return fTrdMCCluster.size();
      }
      ///  \return reference of TrdMCClusterR Collection
      vector<TrdMCClusterR> & TrdMCCluster()  {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return  fTrdMCCluster;
       }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return reference to corresponding TrdMCClusterR element
       TrdMCClusterR &   TrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
         return fTrdMCCluster.at(l);
      }

       ///  TrdMCClusterR accessor
       /// \param l index of TrdMCClusterR Collection
      ///  \return pointer to corresponding TrdMCClusterR element
      TrdMCClusterR *   pTrdMCCluster(unsigned int l) {
        if(fHeader.TrdMCClusters && fTrdMCCluster.size()==0)bTrdMCCluster->GetEntry(_Entry);
        return l<fTrdMCCluster.size()?&(fTrdMCCluster[l]):0;
      }





       ///  RichMCClusterR accessor
      ///  \return number of RichMCClusterR
      unsigned int   NRichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return fRichMCCluster.size();
      }
      ///  \return reference of RichMCClusterR Collection
      vector<RichMCClusterR> & RichMCCluster()  {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return  fRichMCCluster;
       }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return reference to corresponding RichMCClusterR element
       RichMCClusterR &   RichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
         return fRichMCCluster.at(l);
      }

       ///  RichMCClusterR accessor
       /// \param l index of RichMCClusterR Collection
      ///  \return pointer to corresponding RichMCClusterR element
      RichMCClusterR *   pRichMCCluster(unsigned int l) {
        if(fHeader.RichMCClusters && fRichMCCluster.size()==0)bRichMCCluster->GetEntry(_Entry);
        return l<fRichMCCluster.size()?&(fRichMCCluster[l]):0;
      }





       ///  MCTrackR accessor
      ///  \return number of MCTrackR
      unsigned int   NMCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return fMCTrack.size();
      }
      ///  \return reference of MCTrackR Collection
      vector<MCTrackR> & MCTrack()  {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return  fMCTrack;
       }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return reference to corresponding MCTrackR element
       MCTrackR &   MCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
         return fMCTrack.at(l);
      }

       ///  MCTrackR accessor
       /// \param l index of MCTrackR Collection
      ///  \return pointer to corresponding MCTrackR element
      MCTrackR *   pMCTrack(unsigned int l) {
        if(fHeader.MCTracks && fMCTrack.size()==0)bMCTrack->GetEntry(_Entry);
        return l<fMCTrack.size()?&(fMCTrack[l]):0;
      }






       ///  MCEventgR accessor
      ///  \return number of MCEventgR
      unsigned int   NMCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return fMCEventg.size();
      }
      ///  \return reference of MCEventgR Collection
      vector<MCEventgR> & MCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return  fMCEventg;
       }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return reference to corresponding MCEventgR element
       MCEventgR &   MCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return fMCEventg.at(l);
      }

       ///  MCEventgR accessor
       /// \param l index of MCEventgR Collection
      ///  \return pointer to corresponding MCEventgR element
      MCEventgR *   pMCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return l<fMCEventg.size()?&(fMCEventg[l]):0;
      }






AMSEventR();

virtual ~AMSEventR(){
cout <<"AMSEventR::dtor-I-Destroying "<<_Count<<endl;;
if(!--_Count){
 cout<<"AMSEventR::dtor-I-ResettingHead "<<_Head<<endl;
 _Head=0;
}
}



void clear();

#ifndef __ROOTSHAREDLIBRARY__
void         AddAMSObject(AMSAntiCluster *ptr);
void         AddAMSObject(AMSAntiMCCluster *ptr);
void         AddAMSObject(AMSBeta *ptr);
void         AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[],
                          float probtr[], int chintr[], float probrc[], 
                          int chinrc[], float proballtr);
void         AddAMSObject(AMSEcalHit *ptr);
void         AddAMSObject(AMSmceventg *ptr);
void         AddAMSObject(AMSmctrack *ptr);
void         AddAMSObject(AMSTrTrackGamma *ptr);
void         AddAMSObject(AMSParticle *ptr, float phi, float phigl);
void         AddAMSObject(AMSRichMCHit *ptr, int numgen);
void         AddAMSObject(AMSRichRing *ptr);
void         AddAMSObject(AMSRichRawEvent *ptr, float x, float y);
void         AddAMSObject(AMSTOFCluster *ptr);
void         AddAMSObject(AMSTOFMCCluster *ptr);
void         AddAMSObject(AMSTrRecHit *ptr);
void         AddAMSObject(AMSTRDCluster *ptr);
void         AddAMSObject(AMSTRDMCCluster *ptr);
void         AddAMSObject(AMSTRDRawHit *ptr);
void         AddAMSObject(AMSTRDSegment *ptr);
void         AddAMSObject(AMSTRDTrack *ptr);
void         AddAMSObject(AMSTrCluster *ptr);
void         AddAMSObject(AMSTrMCCluster *ptr);
void         AddAMSObject(AMSTrRawCluster *ptr);
void         AddAMSObject(AMSTrTrack *ptr);
void         AddAMSObject(Ecal1DCluster *ptr);
void         AddAMSObject(AMSEcal2DCluster  *ptr);
void         AddAMSObject(AMSEcalShower  *ptr);
void         AddAMSObject(TOF2RawCluster *ptr);
void         AddAMSObject(Trigger2LVL1 *ptr);
void         AddAMSObject(TriggerLVL302 *ptr);
#endif

ClassDef(AMSEventR,1)       //AMSEventR
};






#endif














