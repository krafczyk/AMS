//  $Id: richrec.h,v 1.48 2008/08/20 10:21:22 mdelgado Exp $

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
PROTOCCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)
#define RICHTOFTRACKINIT() CCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)


#include<string.h>


/////////////////////////////////////////////
//         Container for hits              //
/////////////////////////////////////////////


class AMSRichRawEvent: public AMSlink{
private:
  integer _channel; // (PMT number-1)*16+window number
  integer _counts;  // 0 means true, 1 means noise
  geant   _beta_hit[3];  // Reconstructed betas
	 
public:
  AMSRichRawEvent(integer channel,integer counts,uinteger status=0):AMSlink(status),
    _channel(channel),_counts(counts){
    if((getchannelstatus()%10)==Status_good_channel) _status|=ok_status;
  };  // counts above the pedestal
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  inline geant getpos(integer i){
    RichPMTChannel channel(_channel);
    return i<=0?channel.x():channel.y();
  }

  integer getcounts() {return _counts;}
  geant getnpe(){ 
    RichPMTChannel calibration(_channel);
    return _counts/calibration.gain[_status&gain_mode?1:0];
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
  int getchannelstatus(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Status(pmt,channel);}
  float getchannelpedestal(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Pedestal(pmt,channel,getbit(gain_mode_bit));}
  float getchannelsigmapedestal(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::PedestalSigma(pmt,channel,getbit(gain_mode_bit));}
  float getchannelgain(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::Gain(pmt,channel,getbit(gain_mode_bit));}
  float getchannelsigagain(){int pmt,channel;RichPMTsManager::UnpackGeom(_channel,pmt,channel);return RichPMTsManager::GainSigma(pmt,channel,getbit(gain_mode_bit));}
  int getchannelgainmode(){return getbit(gain_mode_bit);}

  // Get betas from last call to reconstruct
  inline geant getbeta(integer i){
    if(i<=0) return _beta_hit[0];
    if(i>=2) return _beta_hit[2];
    return _beta_hit[1];
  } 
  
  static int _npart;
  static int Npart();
  static double RichRandom();
  static void Select(int howmany,int size,int lista[]);
  


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
  number  _probkl;      // Kolgomorov test probability of the reconstructed ring azimuthal distribution being correct
  number  _kdist;       // Leibler-Kullback distance between the reconstructed ring and the hits for the azimuthal marginal distribution
  number _phi_spread;   //(\sum_i (phi_i-\phi_0)^2)/N_{hits} for used hits
  number _unused_dist;   //(\sum_i 1/\dist_i^2) for unused hits which do not belong to PMTs crossed by a charged particle

  number _theta;
  number _errortheta;
  number _radpos[3];
  number _pmtpos[3];


  number  _npexpg;       // Number of expected photons for Z=1
  number  _npexpr;       // Number of expected photons for Z=1
  number  _npexpb;       // Number of expected photons for Z=1
  
  vector<float> _beta_direct;
  vector<float> _beta_reflected;
  vector<int> _hit_pointer;
  vector<int> _hit_used;

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
  

  // All the routines from Elisa

  geant trace(AMSPoint r, AMSDir u, 
	      geant phi, geant *xb, geant *yb, 
	      geant *lentr, geant *lfoil, 
	      geant *lguide, geant *geff, 
	      geant *reff, geant *beff, 
              integer *tflag,float beta_gen=0.9999);
  int tile(AMSPoint r);
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
  AMSRichRing(AMSTrTrack* track,int used,int mused,geant beta,geant quality,geant wbeta,
	      geant recs[RICmaxpmts*RICnwindows/2][3],AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows/2],uinteger size,int ring,uinteger status=0,integer build_charge=0);
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  static AMSRichRing* build(AMSTrTrack *track,int cleanup=1);
  static AMSRichRing* rebuild(AMSTrTrack *ptrack);

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

 protected:

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
  std::vector<float> getHitsResiduals(){return _HitsResiduals;}
  std::vector<int> getHitsStatus(){return _HitsStatus;}
  std::vector<int> getHitsAssoc(){return _HitsAssoc;}
  std::vector<float> getTrackRec(){return _TrackRec;}
#ifdef __WRITEROOT__
  friend class RichRingBR;
#endif
};

// LIP functions prototype
void richiniteventlip();
void richinittracklip(AMSPoint, AMSDir);
void coordams2lip(float pxams, float pyams, float pzams, float &pxlip, float &pylip, float &pzlip);
void coordlip2ams(float pxlip, float pylip, float pzlip, float &pxams, float &pyams, float &pzams);
void angleams2lip(float theams, float phiams, float &thelip, float &philip);
void anglelip2ams(float thelip, float philip, float &theams, float &phiams);
int goodLIPREC();


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

 private:

  vector<AMSRichRingNew*> ringset;

  static void _Start(){TIMEX(_Time);}
  static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
  static bool _NoMoreTime(){return _CheckTime()>AMSFFKEY.CpuLimit;}
  static geant _Time;

};





#endif










