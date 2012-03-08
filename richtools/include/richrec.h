//  $Id: richrec.h,v 1.9 2012/03/08 10:33:40 jorgec Exp $

#ifndef __RICHREC__
#define __RICHREC__
#include "cfortran.h"
#include "richdbc.h"
#include <iostream>
#include "richid.h"
#include "trrec.h"
#include <vector>

// Disable private parts
#define private public  

//#define __AMSDEBUG__


PROTOCCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT)
#define SOLVE(A1,A2,A3,A4,A5,A6) CCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFFUN1(FLOAT,PROBKL,probkl,FLOAT)
#define PROBKL(A) CCALLSFFUN1(PROBKL,probkl,FLOAT,A) 

PROTOCCALLSFSUB2(RICHRECLIP,richreclip,INT,INT)
#define RICHRECLIP(I1,I2) CCALLSFSUB2(RICHRECLIP,richreclip,INT,INT,I1,I2)
PROTOCCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)
#define RICHTOFTRACKINIT() CCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)


#include<string.h>

// Override private members (we wnat full inspection capabilities)
#define private public


/////////////////////////////////////////////
//         Container for hits              //
/////////////////////////////////////////////

#include "root.h" // the reading classes

class RichRing;  // Forward declaration

class RichRawEvent{
 private:
  AMSEventR *_event;
  vector<RichHitR*> _mc_hits; 
  int _current; // current hit
  static integer _PMT_Status[RICmaxpmts];
  static geant _beta_hit[RICmaxpmts*RICnwindows][3];

public:
  RichRawEvent(AMSEventR *event=0):_event(event),_current(0){}
  void Add(int pmt_channel);
  ~RichRawEvent(){for(int i=0;i<_mc_hits.size();delete _mc_hits[i++]);};
  void Rewind(){_current=0;}
  RichRawEvent *next(){_current++;
    if(_mc_hits.size()){
      if(_current>=_mc_hits.size()) return 0;  // Only simulate once
      return this;
    }
    if(_current>=_event->nRichHit()) {delete this;return 0;} 
    return this;}

  RichHitR *getpointer(){if(_mc_hits.size())return _mc_hits[_current];return _event->pRichHit(_current);}
  integer getchannel() const {if(_mc_hits.size())return _mc_hits[_current]->Channel;return _event->pRichHit(_current)->Channel;}
  integer gainx5() const {if(_mc_hits.size())return 1;return (_event->pRichHit(_current)->Status&gain_mode)?1:0;}
  //  inline geant getpos(integer i){return _event->pRichHit(_current)->Coo[i];}

  integer getcounts() {if(_mc_hits.size())return 0;return _event->pRichHit(_current)->Counts;}
  geant getnpe(){ if(_mc_hits.size())return _mc_hits[_current]->Npe;return _event->pRichHit(_current)->Npe;}

  static void build(AMSEventR *event);
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

  void inline setbit(int bit_number){if(_mc_hits.size()) _mc_hits[_current]->Status|=1<<bit_number;else _event->pRichHit(_current)->Status|=1<<bit_number;}
  void inline unsetbit(int bit_number){if(_mc_hits.size()) _mc_hits[_current]->Status|=~(1<<bit_number);else _event->pRichHit(_current)->Status&=~(1<<bit_number);}
  integer inline getbit(int bit_number){if(_mc_hits.size()) return _mc_hits[_current]->Status&(1<<bit_number)>>bit_number;return (_event->pRichHit(_current)->Status&(1<<bit_number))>>bit_number;}

  // Get the channel information 
  int getchannelstatus(){return 0;}
  float getchannelpedestal(){return 0;}
  float getchannelsigmapedestal(){return 0;}
  float getchannelgain(){return 0;}
  float getchannelsigagain(){return 0;}
  int getchannelgainmode(){return 0;}

  int photoElectrons(double sigmaOverQ=0.6);

  // Get betas from last call to reconstruct
  inline geant getbeta(integer i){
    if(i<=0) return _beta_hit[_current][0];
    if(i>=2) return _beta_hit[_current][2];
    return _beta_hit[_current][1];
  } 
  
  static int CrossedPMT(int pmt_id){return _PMT_Status[pmt_id];}

  friend class RichRing;

};



// Container for CIEMAT rings.



class RichRing{
public:
  // Flags for further control not implemented in gbatch
  static bool ComputeNpExp;
  static bool UseDirect;
  static bool UseReflected;
  static double DeltaHeight;

  double photons_per_channel[680*16];  // Photons for each channel
private:
  static AMSEventR *_event;
  integer _status;
  TrTrack* _ptrack;
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

  float NpColPMT[680];     /// Collected photoelectrons in the ring per PMT hits
  float NpExpPMT[680];     /// Expected photoelectrons in the ring per PMT 

  number  _beta_window[10]; 

  number _npexplg[16];
  number _usedlg[16];

  int _emitted_rays;    // number of rays emitted for npexp computation
  int _reflected_rays;  // number of rays emitted and reflected


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
  static float _window;

  // All the routines from Elisa

  geant trace(AMSPoint r, AMSDir u, 
	      geant phi, geant *xb, geant *yb, 
	      geant *lentr, geant *lfoil, 
	      geant *lguide, geant *geff, 
	      geant *reff, geant *beff, 
              integer *tflag,int &_channel,int &_pmt);
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

 static void getSobol(float &x,float &y,bool reset=false);

protected:
  void CalcBetaError();
  void ReconRingNpexp(geant window_size=3.,int cleanup=0);


public:
  RichRing* FastMC(TrTrackR *tr,double beta=-1); // Returns the reconstructed beta value  
  RichRing(TrTrack* track,
	      int used,
	      int mused,
	      geant beta,
	      geant quality,
	      geant wbeta,
	      geant seed_beta,
	      geant recs[RICmaxpmts*RICnwindows/2][3],
	      RichHitR *hitp[RICmaxpmts*RICnwindows/2],
	      uinteger size,
	      int ring,
	      uinteger status=0,
	      integer build_charge=0);
  ~RichRing(){};

  static RichRing* build(AMSEventR *event,AMSPoint p,AMSDir d);
  static RichRing* build(AMSEventR *event,TrTrackR *tr=0);
  static RichRing* build(TrTrack *track,int cleanup=1);
  static void SetWindow(float x){_window=x*x;cout<<"AMSRichRing::SetWindow -- Setting reconstruction window to "<<x<<endl;}

  TrTrack* gettrack(){return _ptrack;}
  integer getused(){return _used;}
  integer checkstatus(uinteger bit){return _status&(1<<bit);}
  bool IsGood(){return !checkstatus(dirty_ring);}
  number getbeta(){return _beta;}
  number geterrorbeta(){return _errorbeta;}
  number getquality(){return _quality;}
  number getnpexp(){return _npexp;}
  number getprob(){return _probkl;}
  number getcollnpe(){return _collected_npe;}

};


#undef private


#endif










