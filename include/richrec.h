//  $Id: richrec.h,v 1.43 2008/07/25 18:26:32 barao Exp $

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
//+LIP
//PROTOCCALLSFSUB1(RICHRECLIP,richreclip,INT)
//#define RICHRECLIP(I1) CCALLSFSUB1(RICHRECLIP,richreclip,INT,I1)
PROTOCCALLSFSUB2(RICHRECLIP,richreclip,INT,INT)
#define RICHRECLIP(I1,I2) CCALLSFSUB2(RICHRECLIP,richreclip,INT,INT,I1,I2)
PROTOCCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)
#define RICHTOFTRACKINIT() CCALLSFSUB0(RICHTOFTRACKINIT,richtoftrackinit)
//END

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



// Container for rings. Each ring is a reconstructed track

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

  
//#ifdef __LIPDATASAVE__
  // NEW VARIABLES
// parameters used in array sizes below
// (should match those in other files)
#define LIP_NHITMAX 1000
#define LIP_NMAXLIPREC 10
  integer _resb_iflag[LIP_NMAXLIPREC];
  integer _resb_itype[LIP_NMAXLIPREC];
  integer _resb_itrk[LIP_NMAXLIPREC];
  number  _resb_beta[LIP_NMAXLIPREC];
  number  _resb_thc[LIP_NMAXLIPREC];
  number  _resb_chi2[LIP_NMAXLIPREC];
  integer _resb_nhit[LIP_NMAXLIPREC];
  integer _resb_phit[LIP_NHITMAX][LIP_NMAXLIPREC];
  integer _resb_used[LIP_NHITMAX][LIP_NMAXLIPREC];
  number  _resb_hres[LIP_NHITMAX][LIP_NMAXLIPREC];
  number  _resb_invchi2[LIP_NMAXLIPREC];
  number  _resb_flatsin[LIP_NMAXLIPREC];
  number  _resb_flatcos[LIP_NMAXLIPREC];
  number  _resb_probkl[LIP_NMAXLIPREC];
  integer _resc_iflag[LIP_NMAXLIPREC];
  number  _resc_cnpe[LIP_NMAXLIPREC];
  number  _resc_cnpedir[LIP_NMAXLIPREC];
  number  _resc_cnperef[LIP_NMAXLIPREC];
  number  _resc_chg[LIP_NMAXLIPREC];
  number  _resc_chgdir[LIP_NMAXLIPREC];
  number  _resc_chgmir[LIP_NMAXLIPREC];
  number  _resc_accgeom[3][LIP_NMAXLIPREC];
  number  _resc_eff[6][LIP_NMAXLIPREC];
  number  _resc_chgprob[3][LIP_NMAXLIPREC];
  number  _resb_pimp[3][LIP_NMAXLIPREC];
  number  _resb_epimp[3][LIP_NMAXLIPREC];
  number  _resb_pthe[LIP_NMAXLIPREC];
  number  _resb_epthe[LIP_NMAXLIPREC];
  number  _resb_pphi[LIP_NMAXLIPREC];
  number  _resb_epphi[LIP_NMAXLIPREC];
  number  _rstd_creclike[50][LIP_NMAXLIPREC];
  number  _rstd_crecx0[50][LIP_NMAXLIPREC];
  number  _rstd_crecy0[50][LIP_NMAXLIPREC];
  number  _rstd_crectheta[50][LIP_NMAXLIPREC];
  number  _rstd_crecphi[50][LIP_NMAXLIPREC];
  number  _rstd_crecbeta[50][LIP_NMAXLIPREC];
  integer _rstd_crecuhits[50][LIP_NMAXLIPREC];
  number  _rstd_crecpkol[50][LIP_NMAXLIPREC];
//#else
/*   // OBSOLETE VARIABLES */
/*   integer _liphused;     //nr of hits used=10000 + nr hits mirror */
/*   number  _lipthc;       // rec. Cerenkov angle */
/*   number  _lipbeta;      // rec. beta */
/*   number  _lipebeta;     // error on rec. beta */
/*   number  _liplikep;     // likelihood prob. */
/*   number  _lipchi2;      // chi2 of the fit */
/*   number  _liprprob;     // ring prob. */
//#endif
  

  static integer _lipdummy;
  //ENDofLIP
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
  //+LIP
  AMSRichRing(AMSTrTrack* track,int used,int mused,geant beta,geant quality,geant wbeta,
//#ifdef __LIPDATASAVE__
	      int resb_iflag[LIP_NMAXLIPREC],int resb_itype[LIP_NMAXLIPREC],int resb_itrk[LIP_NMAXLIPREC],geant resb_beta[LIP_NMAXLIPREC],geant resb_thc[LIP_NMAXLIPREC],geant resb_chi2[LIP_NMAXLIPREC],int resb_nhit[LIP_NMAXLIPREC],int resb_phit[LIP_NHITMAX][LIP_NMAXLIPREC],int resb_used[LIP_NHITMAX][LIP_NMAXLIPREC],geant resb_hres[LIP_NHITMAX][LIP_NMAXLIPREC],geant resb_invchi2[LIP_NMAXLIPREC],geant resb_flatsin[LIP_NMAXLIPREC],geant resb_flatcos[LIP_NMAXLIPREC],geant resb_probkl[LIP_NMAXLIPREC],int resc_iflag[LIP_NMAXLIPREC],geant resc_cnpe[LIP_NMAXLIPREC],geant resc_cnpedir[LIP_NMAXLIPREC],geant resc_cnperef[LIP_NMAXLIPREC],geant resc_chg[LIP_NMAXLIPREC],geant resc_chgdir[LIP_NMAXLIPREC],geant resc_chgmir[LIP_NMAXLIPREC],geant resc_accgeom[3][LIP_NMAXLIPREC],geant resc_eff[6][LIP_NMAXLIPREC],geant resc_chgprob[3][LIP_NMAXLIPREC],geant resb_pimp[3][LIP_NMAXLIPREC],geant resb_epimp[3][LIP_NMAXLIPREC],geant resb_pthe[LIP_NMAXLIPREC],geant resb_epthe[LIP_NMAXLIPREC],geant resb_pphi[LIP_NMAXLIPREC],geant resb_epphi[LIP_NMAXLIPREC],geant rstd_creclike[50][LIP_NMAXLIPREC],geant rstd_crecx0[50][LIP_NMAXLIPREC],geant rstd_crecy0[50][LIP_NMAXLIPREC],geant rstd_crectheta[50][LIP_NMAXLIPREC],geant rstd_crecphi[50][LIP_NMAXLIPREC],geant rstd_crecbeta[50][LIP_NMAXLIPREC],int rstd_crecuhits[50][LIP_NMAXLIPREC],geant rstd_crecpkol[50][LIP_NMAXLIPREC],
//#else
/* 	      int liphused, geant lipthc, geant lipbeta,geant lipebeta, geant liplikep,geant lipchi2, geant liprprob, */
//#endif
	      geant recs[RICmaxpmts*RICnwindows/2][3],AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows/2],uinteger size,int ring,uinteger status=0,integer build_charge=0);
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  static AMSRichRing* build(AMSTrTrack *track,int cleanup=1);
  static AMSRichRing* rebuild(AMSTrTrack *ptrack);
  //+LIP
  static void buildlip(AMSTrTrack *track);
  // new LIP routines, April/May 2008
  static void richiniteventlip();
  static void richinittracklip();
  static void coordams2lip(float pxams, float pyams, float pzams, float &pxlip, float &pylip, float &pzlip);
  static void coordlip2ams(float pxlip, float pylip, float pzlip, float &pxams, float &pyams, float &pzams);
  static void angleams2lip(float theams, float phiams, float &thelip, float &philip);
  static void anglelip2ams(float thelip, float philip, float &theams, float &phiams);
  //ENDofLIP

  AMSTrTrack* gettrack(){return _ptrack;}
  integer getused(){return _used;}
  bool IsGood(){return !checkstatus(dirty_ring);}
  number getbeta(){return _beta;}
  number geterrorbeta(){return _errorbeta;}
  number getquality(){return _quality;}
  number getnpexp(){return _npexp;}
  number getprob(){return _probkl;}
  number getcollnpe(){return _collected_npe;}


  // lipaccesors
  integer getlipused(){
    int lhused = -1;
    for(int k=0;k<LIP_NMAXLIPREC;k++) {
      if(_resb_itype[k]==2) {
	lhused = _resb_nhit[k];
      }
      return lhused;
    }
  }
  number getlipprob(){
    number lrprob = -1.0;
    for(int k=0;k<LIP_NMAXLIPREC;k++) {
      if(_resb_itype[k]==2) {
	lrprob = _resb_probkl[k];
      }
      return lrprob;
    }
  }
//#else
/*   // USING OBSOLETE VARIABLES */
/*   integer getlipused(){return _liphused;} */
/*   number getlipprob(){return _liprprob;} */
//#endif

  static geant ring_fraction(AMSTrTrack *ptrack ,geant &direct,
			     geant &reflected,geant &length,geant beta);

#ifdef __WRITEROOT__
friend class RichRingR;
#endif
};





#endif










