//  $Id: root.h,v 1.82 2003/05/09 16:00:16 choutko Exp $
#ifndef __AMSROOT__
#define __AMSROOT__
//#define __WRITEROOT__
//#define __ROOTSHAREDLIBRARY__

#ifdef __WRITEROOT__
#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TROOT.h>
#include <TBranch.h>
#include <list>
#include <vector>
#include <iostream>
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

  general info

  shuttle/iss parameters

  object counters  (aka arrays dimensions)

  all elements have public accsss
*/

class HeaderR: public TObject {
 public:




// general info block

  unsigned int Run;         ///<run  number
  unsigned int RunType;     ///<runtype(data), RNDM(1) MC
  unsigned int Event;      ///<event number
  unsigned int Status[2];  ///<event status
  int Raw;            ///<raw event length in bytes
  int Version;        ///<program build number
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

  /// A constructor.
  HeaderR(){};  
#ifndef __ROOTSHAREDLIBRARY__
  void   Set(EventNtuple02 *ptr);
#endif
  ClassDef(HeaderR,1)       //HeaderR
};



//!  Ecal Hits Structure
class EcalHitR : public TObject {
public:
  int   Status;   ///< Statusword
  int   Idsoft;   ///< SupLayer/PM/subCell  0:8/0:7/0:3  (?)
  int   Proj;     ///< projection (0-x,1-y)
  int   Plane;    ///< ECAL plane number (0,...)
  int   Cell;     ///< ECAL Cell number (0,...)    
  float Edep;     ///< ECAL measured energy (MeV)
  float EdCorr;   ///< ECAL PMsaturation1-correction(MeV) added to Edep
  float AttCor;   ///<  Attenuation Correction applied (w/r  to center of ecal) (MeV)
  float Coo[3];   ///< ECAL Coo (cm)
  float ADC[3];   ///< ECAL (ADC-Ped) for Hi/Low/Dynode channels
  float Ped[3];   ///< ECAL Pedestals   
  float Gain;     ///<  1/gain (!)

  EcalHitR(AMSEcalHit *ptr);
  EcalHitR(){};
ClassDef(EcalHitR,1)       //EcalHitR
};

/// EcalCluster structure
class EcalClusterR : public TObject {
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
  int NHits;       ///< number of hits in cluster
protected:
  vector <int> fEcalHit;   ///< pointers to EcalHitR collection, protected
public:
  /// access function to EcalHitR collection  
  /// \sa AMSEventR  EcalHitR
  /// \param i index of fEcalHit vector
  /// \return index of EcalHitR collection or -1
  int EcalHit(unsigned int i){return i<fEcalHit.size()?fEcalHit[i]:-1;}
  /// access function to EcalHitR collection   
  /// \param i index of fEcalHit vector
  /// \return pointer to EcalHitR collection or 0
  EcalHitR * pEcalHit(unsigned int i);
  
  EcalClusterR(){};
  EcalClusterR(Ecal1DCluster *ptr);
  friend class Ecal1DCluster;
ClassDef(EcalClusterR,1)       //EcalClusterR
};


//!  Ecal 2d Clusters Structure
class Ecal2DClusterR: public TObject {
public:
  int Status;   ///< statusword
  int Proj;     ///< projection 0-x 1-y
  int Nmemb;    ///< number of 1d clusters
  float Edep;   ///< energy (mev)
  float Coo;    ///< str line fit coo (cm)
  float Tan;    ///< str line fit tangent
  float Chi2;   ///< str line fit chi2

protected:
  vector <int> fEcalCluster;   ///< indexes of EcalClsterR collection
public:
  /// access function to EcalClusterR collection  
  /// \sa AMSEventR  EcalClusterR
  /// \param i index of fEcalCluster vector
  /// \return index of EcalClusterR collection or -1
  int EcalCluster(unsigned int i){return i<fEcalCluster.size()?fEcalCluster[i]:-1;}
  /// access function to EcalClusterR collection   
  /// \param i index of fEcalCluster vector
  /// \return pointer to EcalClusterR collection or 0
  EcalClusterR * pEcalCluster(unsigned int i);

  Ecal2DClusterR(){};
  Ecal2DClusterR(AMSEcal2DCluster *ptr);
ClassDef(Ecal2DClusterR,1)       //Ecal2DClusterR
friend class AMSEcal2DCluster;
};

//!  Ecal Shower Structure
class EcalShowerR : public TObject {
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
  int   N2dCl;            ///< number of Ecal2DClusterR attached

protected:
  vector <int> fEcal2DCluster;  ///< indexes to Ecal2DClusterR collection
public:
  /// access function to Ecal2DClusterR objects
  /// \sa AMSEventR  Ecal2DClusterR
  /// \param i index of fEcal2DCluster vector
  /// \return index of Ecal2DClusterR object in collection or -1
  int Ecal2DCluster(unsigned int i){return i<fEcal2DCluster.size()?fEcal2DCluster[i]:-1;}
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
class RichHitR : public TObject {
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
class RichRingR : public TObject {
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
  /// \sa AMSEventR  TrTrackR
  /// \return index of TrTrackR collection or -1
  int TrTrack()const {return fTrTrack;}
  /// access function to TrTrackR object used
  /// \return pointer to TrTrackR collection or 0
  TrTrackR * pTrTrack();

  RichRingR(){};
  RichRingR(AMSRichRing *ptr);
  friend class AMSRichRing;
  ClassDef(RichRingR,1)           // RichRingR
}; 


/// TofRawClusterR structure
class TofRawClusterR : public TObject {
public:
  int   Status;  ///< statusword (Ask E.Choumilov)
  int   Layer;   ///< Tof plane 1(top)...4
  int   Bar;     ///< Tof Bar number 1...14
  float tovta[2]; ///<anode time over_thres (ns)
  float tovtd[2]; ///<dinode time over_thres (ns)
  float sdtm[2];  ///< A-noncorrected side times
  float edepa;   ///<  Anode Edep (mev)
  float edepd;   ///<  dinode Edep (mev)
  float time;    ///<Time (ns)
  float cool;     ///< Long.coord.(cm)
  TofRawClusterR(){};
  TofRawClusterR(TOF2RawCluster *ptr);

  ClassDef(TofRawClusterR ,1)       //TofRawClusterR
};


/// TofclusterR structure
class TofClusterR : public TObject {
public:
  int Status;   ///< Statusword
                /*!<
                                                 // bit 4 - ambig
                                                 // bit 128 -> problems with history
                                                 // bit 256 -> "1-sided" counter
                                                 // bit 512 -> bad t-measurement on one of the sides
                                                 // bit 2048 -> recovered from 
                                                 // 1-sided (bit256 also set)
   */
  int Layer;    ///<  Tof plane 1(top)...4
  int Bar;      ///< Tof Bar number 1...14
  int Nmemb;   ///< number of TofRawClusters (max 3)
  float Edep;  ///< TOF energy loss (MeV) from anode
  float Edepd; ///< TOF energy loss (MeV) from dinode
  float Time;  ///<TOF time (sec, wrt FastTrig-time)
  float ErrTime;  ///< Error in TOF time
  float Coo[3];   ///< TOF Coo (cm)
  float ErrorCoo[3];   ///< Error TOF Coo (cm)
protected:
  vector<int> fTofRawCluster;   //indexes of TofRawclusterR's current object is made of.
public:
  /// access function to TofRawClusterR's   current object is made of.
  /// \sa AMSEventR  TofRawClusterR
  /// \param i index of fTofRawCluster vector
  /// \return index of TofRawClusterR object in TofRawCluster collection or -1
  int TofRawCluster(unsigned int i){return i<fTofRawCluster.size()?fTofRawCluster[i]:-1;}
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
class AntiClusterR : public TObject {
public:
  int   Status;   ///< Statusword Bit"256"->1sideSector;"1024"->s2 missing if set  s1 missing if not
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
class TrRawClusterR : public TObject {
public:
  int address;   ///<Address \sa TrClusterR Idsoft
  int nelem;     ///< number of strips 
  float s2n;     ///< signal/noise for seed strip

  TrRawClusterR(){};
  TrRawClusterR(AMSTrRawCluster *ptr);
ClassDef(TrRawClusterR,1)       //TrRawClusterR
};


/// TrClusterR structure  
class TrClusterR : public TObject {
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



/// TrClusterR structure   (3D- tracker clusters)
class TrRecHitR : public TObject {
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
  /// \sa AMSEventR  TrClusterR
  /// \param char xy = 'x' for x projection, any other for y projection
  /// \return index of TrClusterR object in TrClusterR collection or -1
  int TrCluster(char xy)const {return xy=='x'?fTrClusterX:fTrClusterY;}
  /// access function to TrClusterR object used
  /// \param char xy = 'x' for x projection, any other for y projection
  /// \return pointer to TrClusterR TrClusterR object in TrClusterR collection or 0
  TrClusterR * pTrCluster(char xy);

  TrRecHitR(){};
  TrRecHitR(AMSTrRecHit *ptr);
  friend class AMSTrRecHit;
ClassDef(TrRecHitR,1)       //TrRecHitR
};



/// TrTrack structure 
class TrTrackR : public TObject {
public:
  int Status;   ///< statusword \sa TrClusterR
  int Pattern;  ///< see datacards.doc
  int Address;  ///< ladders combination code (see trrec.C buildaddress)
  int NHits;     ///< number of hits 
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
  /// access function to TrRecHitR objects
  /// \sa AMSEventR  TrRecHitR
  /// \param i index of fTrRecHit vector
  /// \return index of TrRecHitR object in collection or -1
  int TrRecHit(unsigned int i){return i<fTrRecHit.size()?fTrRecHit[i]:-1;}
  /// access function to TrRecHitR objects   
  /// \param i index of fTrRecHit vector
  /// \return pointer to TrRecHitR object  or 0
  TrRecHitR * pTrRecHit(unsigned int i);
ClassDef(TrTrackR,1)       //TrTrackR
friend class AMSTrTrack;
};

class TrdRawHitR : public TObject {
public:
  int Layer;
  int Ladder;
  int Tube;
  float Amp;

  TrdRawHitR(){};
  TrdRawHitR(AMSTRDRawHit *ptr);

ClassDef(TrdRawHitR,1)       //TrdRawHitR
};

class TrdClusterR : public TObject {
public:
  int   Status;
  float Coo[3];
  int   Layer;
  float CooDir[3];
  int   Multip;
  int   HMultip;
  float EDep;

  int fTrdRawHit;

  TrdClusterR(){};
  TrdClusterR(AMSTRDCluster *ptr);
ClassDef(TrdClusterR,1)       //TrdClusterR
};

class TrdSegmentR : public TObject {
public:
  int   Status;
  int   Orientation;
  float FitPar[2];
  float Chi2;
  int   Pattern;
  int   Nhits;
 
  vector<int> fTrdCluster;
  TrdSegmentR(){};
  TrdSegmentR(AMSTRDSegment *ptr);

ClassDef(TrdSegmentR,1)       //TrdSegmentR
};


class TrdTrackR : public TObject {
public:
  int   Status;
  float Coo[3];
  float ErCoo[3];
  float Phi;
  float Theta;
  float Chi2;
  int   NSeg;
  int   Pattern;
  vector<int> fTrdSegment;
  TrdTrackR(AMSTRDTrack *ptr);
  TrdTrackR(){};
ClassDef(TrdTrackR,1)       //TrdTrackR
};


class Level1R : public TObject {
public:
  int   Mode;
  int   TOFlag;
  int   TOFPatt[4];
  int   TOFPatt1[4];
  int   AntiPatt;
  int   ECALflag;
  float ECALtrsum;

  Level1R(){};
  Level1R(Trigger2LVL1 *ptr);
ClassDef(Level1R,1)       //Level1R
};


class Level3R : public TObject {
public:
  int   TOFTr;
  int   TRDTr;
  int   TrackerTr;
  int   MainTr;
  int   Direction;
  int   NTrHits;
  int   NPatFound;
  int   Pattern[2];
  float Residual[2];
  float Time;
  float ELoss;
  int   TRDHits;
  int   HMult;
  float TRDPar[2];
  int   ECemag;
  int   ECmatc;
  float ECTOFcr[4];

  Level3R(){};
  Level3R(TriggerLVL302 *ptr);
ClassDef(Level3R,1)       //Level3R
};







class BetaR : public TObject {
 public:
  int       Status;
  int       Pattern;
  float     Beta;
  float     BetaC;
  float     Error;
  float     ErrorC;
  float     Chi2;
  float     Chi2S;
  int   fTrTrack;
  vector<int> fTofCluster;
   BetaR(){};
   BetaR(AMSBeta *ptr);
   ClassDef(BetaR,1)         //BetaR
};



class ChargeR : public TObject {
public:
  int Status;
  int ChargeTOF;
  int ChargeTracker;
  int ChargeRich;
  float ProbTOF[4];
  int ChInTOF[4];
  float ProbTracker[4];
  int ChInTracker[4];
  float ProbRich[4];
  int ChInRich[4];
  float ProbAllTracker;
  float TrunTOF;
  float TrunTOFD;
  float TrunTracker;
  int  fBeta;
  int  fRich;
  ChargeR(){};
  ChargeR(AMSCharge *ptr, float probtof[],int chintof[],
               float probtr[], int chintr[], 
               float probrc[], int chinrc[], float proballtr);
ClassDef(ChargeR,1)       //ChargeR

};

class ParticleR : public TObject {
public:
  int Status;
  int   Particle;
  int   ParticleVice;
  float Prob[2];
  float FitMom;
  float Mass;
  float ErrMass;
  float Momentum;
  float ErrMomentum;
  float Beta;
  float ErrBeta;
  float Charge;
  float Theta;
  float Phi;
  float ThetaGl;
  float PhiGl;
  float Coo[3];
  float Cutoff;
  float TOFCoo[4][3];
  float AntiCoo[2][3];
  float EcalCoo[3][3];
  float TrCoo[8][3];
  float TRDCoo[3];
  float RichCoo[2][3];
  float RichPath[2];
  float RichPathBeta[2];
  float RichLength;
  float Local[8];
  float TRDLikelihood;

  int  fBeta;
  int  fCharge;
  int  fTrack;      // pointer to track;
  int  fTrd;        // pointer to trd track
  int  fRich;       // pointer to rich ring
  int  fShower;     // pointer to shower;

  ParticleR(){};
  ParticleR(AMSParticle *ptr, float phi, float phigl);
  ClassDef(ParticleR,1)       //ParticleR
};


class AntiMCClusterR : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;

  AntiMCClusterR(){};
  AntiMCClusterR(AMSAntiMCCluster *ptr);
ClassDef(AntiMCClusterR,1)       //AntiMCClusterR
};

class TrMCClusterR : public TObject {
public:
  int Idsoft;
  int TrackNo;
  int Left[2];
  int Center[2];
  int Right[2];
  float SS[2][5];
  float Xca[3];
  float Xcb[3];
  float Xgl[3];
  float Sum;

  TrMCClusterR(){};
  TrMCClusterR(AMSTrMCCluster *ptr);
ClassDef(TrMCClusterR,1)       //TrMCClusterR
};


class TofMCClusterR : public TObject {
public:
  int   Idsoft;
  float Coo[3];
  float TOF;
  float Edep;
  TofMCClusterR(){};
  TofMCClusterR(AMSTOFMCCluster *ptr);
ClassDef(TofMCClusterR,1)       //TOFMCClusterRoot
};


class TrdMCClusterR : public TObject {
public:
  int   Layer;
  int   Ladder;
  int   Tube;
  int   ParticleNo;
  float Edep;
  float Ekin;
  float Xgl[3];
  float Step;
 
  TrdMCClusterR(){};
  TrdMCClusterR(AMSTRDMCCluster *ptr);
ClassDef(TrdMCClusterR,1)       //TrdMCClusterR
};


class RichMCClusterR : public TObject {
public:
  int   id;            // Particle id.
  float origin[3];     // Particle origin
  float direction[3];  // Original direction
  int   status;        // Status=10*number of reflections+(have it rayleigh?1:0)
  int   numgen;        // Number of generated photons
  int   eventpointer;  // Pointer to detected hit

  RichMCClusterR(){};
  RichMCClusterR(AMSRichMCHit *ptr, int _numgen);
  ClassDef(RichMCClusterR,1)       // RichMCClusterR
};



class MCTrackR : public TObject {
public:
float _radl;
float _absl;
float _pos[3];
char  _vname[4];

 MCTrackR(){};
 MCTrackR(AMSmctrack *ptr);
ClassDef(MCTrackR,1)       //MCTrackR
};


class MCEventgR : public TObject {
public:
  int Nskip;
  int Particle;
  float Coo[3];
  float Dir[3];
  float Momentum;
  float Mass;
  float Charge;
  MCEventgR(){};
  MCEventgR(AMSmceventg *ptr);
ClassDef(MCEventgR,1)       //MCEventgR
};



///AMSEventR Structure (Main AMS Root Class)
class AMSEventR: public  TObject {

protected:

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
static AMSEventR * _Head;
static int         _Count;
static int         _Entry;
public:
 static AMSEventR* Head()  {return _Head;}
 static char      * _Name;
 void SetBranchA(TTree *tree);
 void GetBranchA(TTree *tree);
 void SetCont(); 
 int & Entry(){return _Entry;}
public:


 
  HeaderR  fHeader;
  ///  Reads Header \sa ntupleR.h ntupleR.C
  /// \param Entry - event no
  void ReadHeader(int Entry);
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

  void _RAntiCluster();

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


      ///  \return number of EcalClusterR
      unsigned int   NEcalCluster()  {
        if(fHeader.EcalClusters && fEcalCluster.size()==0)bEcalCluster->GetEntry(_Entry);
        return fEcalCluster.size();
      }
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



      unsigned int   NEcal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return fEcal2DCluster.size();
      }
      vector<Ecal2DClusterR> & Ecal2DCluster()  {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return  fEcal2DCluster;
       }

       Ecal2DClusterR &   Ecal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
         return fEcal2DCluster.at(l);
      }

      Ecal2DClusterR *   pEcal2DCluster(unsigned int l) {
        if(fHeader.Ecal2DClusters && fEcal2DCluster.size()==0)bEcal2DCluster->GetEntry(_Entry);
        return l<fEcal2DCluster.size()?&(fEcal2DCluster[l]):0;
      }



      unsigned int   NEcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
        return fEcalShower.size();
      }
      vector<EcalShowerR> & EcalShower()  {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return  fEcalShower;
       }

       EcalShowerR &   EcalShower(unsigned int l) {
        if(fHeader.EcalShowers && fEcalShower.size()==0)bEcalShower->GetEntry(_Entry);
         return fEcalShower.at(l);
      }

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


      unsigned int   NTrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return fTrdTrack.size();
      }
      vector<TrdTrackR> & TrdTrack()  {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return  fTrdTrack;
       }

       TrdTrackR &   TrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
         return fTrdTrack.at(l);
      }

      TrdTrackR *   pTrdTrack(unsigned int l) {
        if(fHeader.TrdTracks && fTrdTrack.size()==0)bTrdTrack->GetEntry(_Entry);
        return l<fTrdTrack.size()?&(fTrdTrack[l]):0;
      }



      unsigned int   NBeta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return fBeta.size();
      }
      vector<BetaR> & Beta()  {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return  fBeta;
       }

       BetaR &   Beta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
         return fBeta.at(l);
      }

      BetaR *   pBeta(unsigned int l) {
        if(fHeader.Betas && fBeta.size()==0)bBeta->GetEntry(_Entry);
        return l<fBeta.size()?&(fBeta[l]):0;
      }



      unsigned int   NParticle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return fParticle.size();
      }
      vector<ParticleR> & Particle()  {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return  fParticle;
       }

       ParticleR &   Particle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
         return fParticle.at(l);
      }

      ParticleR *   pParticle(unsigned int l) {
        if(fHeader.Particles && fParticle.size()==0)bParticle->GetEntry(_Entry);
        return l<fParticle.size()?&(fParticle[l]):0;
      }




      unsigned int   NMCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return fMCEventg.size();
      }
      vector<MCEventgR> & MCEventg()  {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return  fMCEventg;
       }

       MCEventgR &   MCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
         return fMCEventg.at(l);
      }

      MCEventgR *   pMCEventg(unsigned int l) {
        if(fHeader.MCEventgs && fMCEventg.size()==0)bMCEventg->GetEntry(_Entry);
        return l<fMCEventg.size()?&(fMCEventg[l]):0;
      }










AMSEventR();

virtual ~AMSEventR(){
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
#endif














