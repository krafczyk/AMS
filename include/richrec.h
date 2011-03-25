//  $Id: richrec.h,v 1.71 2011/03/25 14:15:06 mdelgado Exp $

#ifndef __RICHREC__
#define __RICHREC__
#include "richdbc.h"
#include <iostream>
#include "richid.h"
#include "trrec.h"
#include <vector>


PROTOCCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT)
#define SOLVE(A1,A2,A3,A4,A5,A6) CCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFFUN1(FLOAT,PROBKL,probkl,FLOAT)
#define PROBKL(A) CCALLSFFUN1(PROBKL,probkl,FLOAT,A) 

PROTOCCALLSFSUB2(RICHRECLIP,richreclip,INT,INT)
#define RICHRECLIP(I1,I2) CCALLSFSUB2(RICHRECLIP,richreclip,INT,INT,I1,I2)
PROTOCCALLSFSUB1(RICHTOFTRACKINIT,richtoftrackinit,INT)
#define RICHTOFTRACKINIT(I1) CCALLSFSUB1(RICHTOFTRACKINIT,richtoftrackinit,INT,I1)
PROTOCCALLSFSUB3(RICHGETMATRIXHINT,richgetmatrixhint,PFLOAT,PFLOAT,PFLOAT)
#define RICHGETMATRIXHINT(F1,F2,F3) CCALLSFSUB3(RICHGETMATRIXHINT,richgetmatrixhint,PFLOAT,PFLOAT,PFLOAT,F1,F2,F3)


#include<string.h>


/////////////////////////////////////////////
//         Container for hits              //
/////////////////////////////////////////////


class AMSRichRawEvent: public AMSlink{
private:
  integer _channel; // (PMT number-1)*16+window number
  integer _counts;  
  geant   _beta_hit[3];  // Reconstructed betas
	 

  static integer _PMT_Status[RICmaxpmts];

public:
  AMSRichRawEvent(integer channel,integer counts,uinteger status=0):AMSlink(status),
    _channel(channel),_counts(counts){
    if((getchannelstatus()%10)==Status_good_channel) _status|=ok_status;
  };  // counts above the pedestal
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  integer gainx5() const {return (_status&gain_mode)?1:0;}
  inline geant getpos(integer i){
    RichPMTChannel channel(_channel);
    return i<=0?channel.x():channel.y();
  }

  integer getcounts() {return _counts;}
  geant getnpe(){ 
    RichPMTChannel calibration(_channel);
    //PZ FPE bugfix FIXME
    if(calibration.gain[_status&gain_mode?1:0]!=0)
      return _counts/calibration.gain[_status&gain_mode?1:0];
    else
      return 0;
  }

  static void mc_build();
  static void build();
  void reconstruct(AMSPoint,AMSPoint,AMSDir,AMSDir,geant,
		   geant,
                   geant *,geant,int);

  integer reflexo(AMSPoint origin,AMSPoint *ref_point);
  geant inline dist(AMSPoint punto,AMSPoint plane){
    return -(punto[0]*plane[0]+punto[1]*plane[1]+punto[2]*plane[2]);
  }
  AMSPoint inline rpoint(AMSPoint ri,AMSPoint rf,AMSPoint plane){
    geant z1=-dist(ri,plane),z2=-dist(rf,plane);
    AMSPoint output((-2*plane[0]*z1*z2+z2*ri[0]+z1*rf[0])/(z1+z2),
		    (-2*plane[1]*z1*z2+z2*ri[1]+z1*rf[1])/(z1+z2),
		    (-2*plane[2]*z1*z2+z2*ri[2]+z1*rf[2])/(z1+z2));
    return output;		    
  }

  void inline setbit(int bit_number){_status|=1<<bit_number;}
  void inline unsetbit(int bit_number){_status&=~(1<<bit_number);}
  integer inline getbit(int bit_number){return (_status&(1<<bit_number))>>bit_number;}

  // Get the channel information 
  int getchannelstatus(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Status(pmt,channel) && RichPMTsManager::Mask(pmt,channel);}
  float getchannelpedestal(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Pedestal(pmt,channel,getbit(gain_mode_bit));}
  float getchannelsigmapedestal(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::PedestalSigma(pmt,channel,getbit(gain_mode_bit));}
  float getchannelgain(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Gain(pmt,channel,getbit(gain_mode_bit));}
  float getchannelsigagain(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::GainSigma(pmt,channel,getbit(gain_mode_bit));}
  int getchannelgainmode(){return getbit(gain_mode_bit);}

  int photoElectrons(double sigmaOverQ=0.6);

  // Get betas from last call to reconstruct
  inline geant getbeta(integer i){
    if(i<=0) return _beta_hit[0];
    if(i>=2) return _beta_hit[2];
    return _beta_hit[1];
  } 
  
  static int _npart;
#pragma omp threadprivate(_npart)
  static int Npart();
  static double RichRandom();
  static void Select(int howmany,int size,int lista[]);
  static int CrossedPMT(int pmt_id){return _PMT_Status[pmt_id];}
  int getHwAddress();
  
// interface with DAQ :

protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSRichRawEvent: id="<<_channel<<endl;
    stream <<" Adc="<<_counts<<endl;
    stream <<" Status="<<_status<<endl;
  }
  void _writeEl();
  void _copyEl(){};
#ifdef __WRITEROOT__
friend class RichHitR;
#endif
};



// Container for CIEMAT rings.



class AMSRichRing: public AMSlink{

#ifdef __WRITEROOT__
friend class AMSTrTrack;
#endif
private:

static void _Start(){TIMEX(_Time);}
static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
static bool _NoMoreTime(){return _CheckTime()>AMSFFKEY.CpuLimit;}
static geant _Time;
#pragma omp threadprivate(_Time)

  AMSTrTrack* _ptrack;
  integer _used;        // number of hits used
  integer _mused;       // number of mirrored hits used
  number  _beta;        // ring beta
  number  _wbeta;       // weighted beta using the number of p.e. 
  number  _beta_blind;  // Unused: in the future reconstruction without using the track direction
  number  _errorbeta;   // Estimated error in beta
  number  _quality;     // Chi2  
  number  _npexp;       // Number of expected photons for Z=1
  number  _collected_npe;  // Number of collected photoelectrons: the rich charge is estimated as sqrt(npexp/collected_npe)
  number  _collected_npe_lkh;  // Number of collected photoelectrons: the rich charge is estimated as sqrt(npexp/collected_npe)
  number  _probkl;      // Kolgomorov test probability of the reconstructed ring azimuthal distribution being correct
  number  _kdist;       // Leibler-Kullback distance between the reconstructed ring and the hits for the azimuthal marginal distribution
  number _phi_spread;   //(\sum_i (phi_i-\phi_0)^2)/N_{hits} for used hits
  number _unused_dist;   //(\sum_i 1/\dist_i^2) for unused hits which do not belong to PMTs crossed by a charged particle

  number _theta;
  number _errortheta;
  number _radpos[3];
  number _pmtpos[3];
  number _crossingtrack[5];


  number  _npexpg;       // Number of expected photons for Z=1
  number  _npexpr;       // Number of expected photons for Z=1
  number  _npexpb;       // Number of expected photons for Z=1
  
  vector<float> _beta_direct;
  vector<float> _beta_reflected;
  vector<int> _hit_pointer;
  vector<int> _hit_used;

  integer _collected_hits_window[10];
  number  _collected_pe_window[10];


  // All these guys can be obtained asking to the class RichRadiatorTile

  static number _index;  // refractive index used in the recontruction
                  // intended for using with two radiators
  static number _height; // Radiator height
  static AMSPoint _entrance_p;  // Entrance point in the radiator 
  static AMSDir   _entrance_d;  // Entrance direction

  static AMSPoint _emission_p;
  static AMSDir   _emission_d;

  // Those guys necessary to compute the number of 
  // expected photons for Z=1
  static geant   _clarity;
  static geant *_abs_len;
  static geant *_index_tbl;
  static int _kind_of_tile;
  static int _tile_index;
  static geant _distance2border;
#pragma omp threadprivate(_index,_height,_entrance_p,_entrance_d,_emission_p,_emission_d,_clarity,_abs_len,_index_tbl,_kind_of_tile,_tile_index,_distance2border)

  // Tables for light guide response
// Some variables used in AMSRichRing::generated
#define _NRAD_ 400
#define _NFOIL_ 10
#define _NGUIDE_ 14
#define _TILES_ 108 
  static int _generated_initialization;
  static int _first_radiator_call[_TILES_];
  static float _l[RICmaxentries][_TILES_]; 
  static float _r[RICmaxentries][_TILES_]; 
  static float _a[RICmaxentries][_TILES_]; 
  static float _b[RICmaxentries][_TILES_]; 
  static float _g[RICmaxentries][_TILES_]; 
  static float _t[RICmaxentries][_TILES_];
  static float _effg[_NRAD_][_TILES_]; 
  static float _ring[_NRAD_][_TILES_];
  static float _effr[_NRAD_][_NFOIL_][_TILES_]; 
  static float _rinr[_NRAD_][_NFOIL_][_TILES_];
  static float _effb[_NRAD_][_NFOIL_][_TILES_]; 
  static float _rinb[_NRAD_][_NFOIL_][_TILES_];
  static float _effd[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_]; 
  static float _rind[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_];
//#pragma omp threadprivate(_generated_initialization,_first_radiator_call,_l,_r,_a,_b,_g,_t,_effg,_ring,_effr,_rinr,_effb,_rinb,_effd,_rind)

  static float _window;

  // All the routines from Elisa

  geant trace(AMSPoint r, AMSDir u, 
	      geant phi, geant *xb, geant *yb, 
	      geant *lentr, geant *lfoil, 
	      geant *lguide, geant *geff, 
	      geant *reff, geant *beff, 
              integer *tflag,float beta_gen=0.9999);
  static int tile(AMSPoint r);
  float generated(geant length,
		  geant lfoil,
		  geant lguide,
		  geant *fg,
		  geant *fr,
		  geant *fb);
  void refract(geant r1,
	       geant r2, 
	       float *u,
	       geant *v);
  geant lgeff(AMSPoint r, 
	      float u[3],
	      geant *lguide);
  int locsmpl(int id,
	      int *iw, 
	      geant *u, 
	      geant *v);
  


// A parametrisation
  static inline geant Sigma(geant beta,geant A,geant B){
      return beta*(B*beta+A)*1.e-2;
    }

  static inline integer closest(geant beta,
                                geant *betas){

    geant a=fabs(beta-betas[0]);
    geant b=fabs(beta-betas[1]);
    geant c=fabs(beta-betas[2]);
    if(b<c) {if(a<b) return 0; else return 1;}
    else    {if(a<c) return 0; else return 2;}
  }

protected:
  void _printEl(ostream &stream){stream<<" Beta "<<_beta<<" Error "<<_errorbeta<<endl;}
  void _writeEl();
  void _copyEl();
  void CalcBetaError();
  void ReconRingNpexp(geant window_size=3.,int cleanup=0);

public:

  AMSRichRing(AMSTrTrack* track,
	      int used,
	      int mused,
	      geant beta,
	      geant quality,
	      geant wbeta,
	      geant seed_beta,
	      geant recs[RICmaxpmts*RICnwindows/2][3],
	      AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows/2],
	      uinteger size,
	      int ring,
	      uinteger status=0,
	      integer build_charge=0);
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  static AMSRichRing* build(AMSTrTrack *track,int cleanup=1);
  static AMSRichRing* rebuild(AMSTrTrack *ptrack);
  static void SetWindow(float x){_window=x*x;cout<<"AMSRichRing::SetWindow -- Setting reconstruction window to "<<x<<endl;}

  AMSTrTrack* gettrack(){return _ptrack;}
  integer getused(){return _used;}
  bool IsGood(){return !checkstatus(dirty_ring);}
  number getbeta(){return _beta;}
  number geterrorbeta(){return _errorbeta;}
  number getquality(){return _quality;}
  number getnpexp(){return _npexp;}
  number getprob(){return _probkl;}
  number getcollnpe(){return _collected_npe;}

  static geant ring_fraction(AMSTrTrack *ptrack ,geant &direct,
			     geant &reflected,geant &length,geant beta);

#ifdef __WRITEROOT__
friend class RichRingR;
#endif

friend class  RichRadiatorTileManager; 
};



// Unified container for rings

class AMSRichRingNew: public AMSlink{

#ifdef __WRITEROOT__
  friend class AMSTrTrack;
#endif
 private:

  static void _Start(){TIMEX(_Time);}
  static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
  static bool _NoMoreTime(){return _CheckTime()>AMSFFKEY.CpuLimit;}
  static geant _Time;
#pragma omp threadprivate(_Time)

  AMSTrTrack* _ptrack;

  // reconstruction status
  int _Status;            // status of reconstruction
  // velocity reconstruction
  float _Beta;            // reconstructed velocity
  float _AngleRec;        // reconstructed Cerenkov angle
  float _Chi2;            // chi2 for reconstruction
  float _Likelihood;      // likelihood for reconstruction
  int _Used;              // number of hits used in reconstruction
  float _ProbKolm;        // Kolmogorov probability for reconstruction
  float _Flatness[2];     // flatness parameter (sin,cos)
  // charge reconstruction
  float _ChargeRec;       // reconstructed charge (full ring)
  float _ChargeProb[3];   // probabilities for nearest integer charges
  float _ChargeRecDir;	  // reconstructed charge (direct branch)
  float _ChargeRecMir;    // reconstructed charge (reflected branch)
  float _NpeRing;	  // no. photoelectrons in ring (full ring)
  float _NpeRingDir;	  // no. photoelectrons in ring (direct branch)
  float _NpeRingRef;	  // no. photoelectrons in ring (reflected branch)
  float _RingAcc[3];	  // ring geometrical acceptances
  float _RingEff[6];	  // ring efficiencies
  int _NMirSec;           // no. mirror sectors

 protected:

  // acceptance/efficiency data by mirror sector
  float _RingAccMsec1R[3];    // acceptance, 1st reflection
  float _RingAccMsec2R[3];    // acceptance, 2nd reflection
  float _RingEffMsec1R[3];    // efficiency, 1st reflection
  float _RingEffMsec2R[3];    // efficiency, 2nd reflection

  // hit data
  std::vector<float> _HitsResiduals;    // residuals of (ring and non-ring) hits in reconstruction
  std::vector<int>   _HitsStatus;       // status of hits
  std::vector<int>   _HitsAssoc;        // list of hits associated to reconstructed ring
  std::vector<float> _TrackRec;         // track parameters obtained in LIP reconstruction
                                        // _TrackRec[0] = x                _TrackRec[1] = error in x
                                        // _TrackRec[2] = y                _TrackRec[3] = error in y
                                        // _TrackRec[4] = z                _TrackRec[5] = error in z
                                        // _TrackRec[6] = theta (radians)  _TrackRec[7] = error in theta
                                        // _TrackRec[8] = phi (radians)    _TrackRec[9] = error in phi

  void _printEl(ostream &stream){stream<<" Lip Beta "<<_Beta<<" Cerenkov Angle "<<_AngleRec<<endl;}
  void _writeEl();
  void _copyEl();

  // Data from the class RichRadiatorTile

  static number _index;  // refractive index used in the reconstruction
                         // intended for using with two radiators
  static number _height; // Radiator height
  static AMSPoint _entrance_p;  // Entrance point in the radiator 
  static AMSDir   _entrance_d;  // Entrance direction

  static AMSPoint _emission_p;
  static AMSDir   _emission_d;

  static geant   _clarity;
  static geant *_abs_len;
  static geant *_index_tbl;
  static int _kind_of_tile;
#pragma omp threadprivate(_index,_height,_entrance_p,_entrance_d,_emission_p,_emission_d,_clarity,_abs_len,_index_tbl,_kind_of_tile)

 public:

  AMSRichRingNew(AMSTrTrack* , int);
  ~AMSRichRingNew(){};
  //AMSRichRingNew * next(){return (AMSRichRingNew*)_next;}

  int buildlip();
  void fillresult();

  AMSTrTrack* gettrack(){return _ptrack;}

  int getStatus(){return _Status;}
  float getBeta(){return _Beta;}
  float getAngleRec(){return _AngleRec;}
  float getChi2(){return _Chi2;}
  float getLikelihood(){return _Likelihood;}
  float getUsed(){return _Used;}
  float getProbKolm(){return _ProbKolm;}
  float getFlatness(int i){return _Flatness[i];}
  float getChargeRec(){return _ChargeRec;}
  float getChargeProb(int i){return _ChargeProb[i];}
  float getChargeRecDir(){return _ChargeRecDir;}
  float getChargeRecMir(){return _ChargeRecMir;}
  float getNpeRing(){return _NpeRing;}
  float getNpeRingDir(){return _NpeRingDir;}
  float getNpeRingRef(){return _NpeRingRef;}
  float getRingAcc(int i){return _RingAcc[i];}
  float getRingEff(int i){return _RingEff[i];}
  int getNMirSec(){return _NMirSec;}
  float getRingAccMsec1R(int i){return _RingAccMsec1R[i];}
  float getRingAccMsec2R(int i){return _RingAccMsec2R[i];}
  float getRingEffMsec1R(int i){return _RingEffMsec1R[i];}
  float getRingEffMsec2R(int i){return _RingEffMsec2R[i];}
  std::vector<float> getHitsResiduals(){return _HitsResiduals;}
  std::vector<int> getHitsStatus(){return _HitsStatus;}
  std::vector<int> getHitsAssoc(){return _HitsAssoc;}
  std::vector<float> getTrackRec(){return _TrackRec;}
#ifdef __WRITEROOT__
  friend class RichRingBR;
#endif
};


// Container for LIP ring recs

class AMSRichRingNewSet {

 public:
  AMSRichRingNewSet();
  ~AMSRichRingNewSet();
  void reset();
  void build();
  void init();
  AMSRichRingNew* getring(int);
  void AddRing(AMSRichRingNew*);
  int NumberOfRings();
  static double getzphemiloc(int tileno) {if (tileno<0 || tileno>107) return -999.; return zphemiloc[tileno];}
 private:

  vector<AMSRichRingNew*> ringset;

  static void _Start(){TIMEX(_Time);}
  static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
  static bool _NoMoreTime(){return _CheckTime()>AMSFFKEY.CpuLimit;}
  static geant _Time;
  static integer trig;
#pragma omp threadprivate(_Time,trig)

  static const double zphemiloc[108];

};





#endif










