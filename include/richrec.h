//  $Id: richrec.h,v 1.28 2003/12/18 16:12:50 mdelgado Exp $

#ifndef __RICHREC__
#define __RICHREC__
#include <richdbc.h>
#include <iostream>
#include <richid.h>


PROTOCCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT)
#define SOLVE(A1,A2,A3,A4,A5,A6) CCALLSFSUB6(SOLVE,solve,DOUBLE,DOUBLE,DOUBLE,DOUBLE,DOUBLEV,INT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFFUN1(FLOAT,PROBKL,probkl,FLOAT)
#define PROBKL(A) CCALLSFFUN1(PROBKL,probkl,FLOAT,A) 
//+LIP
PROTOCCALLSFSUB1(RICHRECLIP,richreclip,INT)
#define RICHRECLIP(I1) CCALLSFSUB1(RICHRECLIP,richreclip,INT,I1)
//END

#include<string.h>

// First a simple template to use safe arrays. The value of should
// be always greater than 0

template<class T,int S>
class safe_array{
private:
  T *_origin;
  T _org[S];
  int _size;
public:
  T error;
  safe_array(int size=0){
    if(size<=0) {_size=-S;_origin=_org;} else {
    _origin=new T[size];
    _size=size;}
  }
  ~safe_array(){
    if(_origin!=_org && _size>0){
      delete []_origin;
    }
  };

  T& operator [](int pos){
    if(pos>=abs(_size) || pos<0) {
      cout << "RICH Safe array overflow. Request: "<<pos<<" of "<<abs(_size)<<endl;
      return error;
    }
    return *(_origin+pos);
  }
  int size(){return abs(_size);}
};

typedef safe_array<geant,3> geant_small_array;
typedef safe_array<integer,3> integer_small_array;


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
    _channel(channel),_counts(counts){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  inline geant getpos(integer i){
    AMSRICHIdGeom channel(_channel);
    return i<=0?channel.x():channel.y();
  }

  integer getcounts() {return _counts;}
  geant getnpe(){ 
    AMSRICHIdSoft calibration(_channel);
    return _counts/calibration.getgain(_status&gain_mode?1:0);
  }

  static void mc_build();
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


  // Get betas from last call to reconstruct
  inline geant getbeta(integer i){
    if(i<=0) return _beta_hit[0];
    if(i>=2) return _beta_hit[2];
    return _beta_hit[1];
  } 
  



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
  number  _collected_npe;  // Number of collected photoelectrons
  number  _probkl;      // kolgomorov probability of the ring

  number _theta;
  number _errortheta;
  number _radpos[3];
  number _pmtpos[3];


  number  _npexpg;       // Number of expected photons for Z=1
  number  _npexpr;       // Number of expected photons for Z=1
  number  _npexpb;       // Number of expected photons for Z=1

  //+LIP
  // variables
  integer _liphused;     //nr of hits used=10000 + nr hits mirror
  number  _lipthc;       // rec. Cerenkov angle
  number  _lipbeta;      // rec. beta
  number  _lipebeta;     // error on rec. beta
  number  _liplikep;     // likelihood prob.
  number  _lipchi2;      // chi2 of the fit
  number  _liprprob;     // ring prob.

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
  

  // ALl the routines from Elisa

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
  void ReconRingNpexp(geant window_size=3.);
public:
  //+LIP
  AMSRichRing(AMSTrTrack* track,int used,int mused,geant beta,geant quality,geant wbeta,int liphused, geant lipthc, geant lipbeta,geant lipebeta, geant liplikep,geant lipchi2, geant liprprob,uinteger status=0,integer build_charge=0):AMSlink(status),
    _ptrack(track),_used(used),_mused(mused),_beta(beta),_quality(quality),_wbeta(wbeta),_liphused(liphused),_lipthc(lipthc),_lipbeta(lipbeta),_lipebeta(lipebeta),_liplikep(liplikep),_lipchi2(lipchi2),_liprprob(liprprob){
  //  AMSRichRing(AMSTrTrack* track,int used,int mused,geant beta,geant quality,geant wbeta,uinteger status=0,integer build_charge=0):AMSlink(status),
  //   _ptrack(track),_used(used),_mused(mused),_beta(beta),_quality(quality),_wbeta(wbeta){

    CalcBetaError();

    _npexp=0;
    _collected_npe=0;
    _probkl=0;


    if(build_charge){
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHZ");
      ReconRingNpexp();
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHZ");
    }

    AMSPoint pnt;
    number theta,phi,length;
    
    //    track->interpolate(AMSPoint(0.,0.,RICradpos-RICHDB::rad_height+_mean_height[_kind-1][0]),
    //		       AMSDir(0.,0.,-1.),pnt,theta,phi,length);

    _radpos[0]=_emission_p[0];
    _radpos[1]=_emission_p[1];
    _radpos[2]=_emission_p[2];
    
    _theta=acos(1/_beta/_index);
    _errortheta=_errorbeta/_beta/tan(_theta);

    track->interpolate(AMSPoint(0,0,RICradpos-RICHDB::pmt_pos()),AMSDir(0.,0.,-1.),pnt,theta,phi,length);
    
    _pmtpos[0]=pnt[0];
    _pmtpos[1]=pnt[1];
    _pmtpos[2]=pnt[2];


  };
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  //  static void build(AMSTrTrack *track,int cleanup=1);
  //  static void rebuild(AMSTrTrack *ptrack);
  static AMSRichRing* build(AMSTrTrack *track,int cleanup=1);
  static AMSRichRing* rebuild(AMSTrTrack *ptrack);
  //+LIP
  static AMSRichRing* buildlip(AMSTrTrack *track);
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
#ifdef __WRITEROOT__
friend class RichRingR;
#endif
};



///////////////////////////////////////////
// This class contains all the information used in the reconstruction
// about the radiator
//////////////////////////////////////////////

class RichRadiatorTile{
 private:

  number _index;          // Refractive index of current tile
  number _height;         // Height of current tile
  integer _kind;
  AMSPoint _p_direct;
  AMSPoint _p_reflected;
  AMSDir _d_direct;
  AMSDir _d_reflected;
  AMSPoint _p_entrance;
  AMSDir   _d_entrance;


  // ALl the necessary numbers
  static integer _number_of_rad_tiles;       // Number of radiator tiles
  static integer *_kind_of_tile;         // Kind of tile 0-> no tile; 1 agl; 2 NaF
  static geant _eff_indexes[radiator_kinds];
  static geant _rad_heights[radiator_kinds];     
  static geant _mean_height[radiator_kinds][2]; // For reflected and direct cases
  static geant *_abs_length[radiator_kinds];
  static geant *_index_tables[radiator_kinds];
  static geant _clarities[radiator_kinds];


  static void _compute_mean_height(geant *index,
				    geant clarity,
				    geant *abs_len,
				    geant rheight,
				    geant &eff_index,
				    geant &height);

 public:
  static void Init();                  // Init geometry and kinds and so on
  static number getpos(int id,int pos);  // get pos (x,y) of a given tile
  //  RichRadiatorTile(geant x,geant y){};   //Constructor given the position at the bottom: use the static member instead 
  RichRadiatorTile(AMSTrTrack *track); // Constructor given a track 
  RichRadiatorTile(){};
  ~RichRadiatorTile(){};
  


  //////////////////////////////////////////////
  // Some code to access to some global values /
  //////////////////////////////////////////////
  static inline number get_number_of_tiles(){return _number_of_rad_tiles;};
  static inline number get_tile_x(integer tile){
    return int(tile/_number_of_rad_tiles)*RICHDB::rad_length-
      (_number_of_rad_tiles-1)*RICHDB::rad_length/2.;
  };
  static inline number get_tile_y(integer tile){
    return int(tile%_number_of_rad_tiles)*RICHDB::rad_length-
      (_number_of_rad_tiles-1)*RICHDB::rad_length/2.;
  };
  static inline integer get_tile_number(geant x,geant y){
    x/=RICHDB::rad_length;
    y/=RICHDB::rad_length;
    x+=_number_of_rad_tiles/2.;
    y+=_number_of_rad_tiles/2.;
    if(x<0 || y<0 || x>=_number_of_rad_tiles || y>=_number_of_rad_tiles) return -1;
    return int(x)*_number_of_rad_tiles+int(y);
  };
  static inline integer get_tile_kind(integer tile){
    if(tile<0 || tile>=_number_of_rad_tiles*_number_of_rad_tiles) return empty_kind;
    else return _kind_of_tile[tile];
  };




  ////////////////////////////////////////////////////////////////////////
  // Given a tile object constructed with a track return a lot od stuff //  
  ////////////////////////////////////////////////////////////////////////


  inline number getindex(){return _index;};
  inline number getheight(){return _height;};
  inline integer getkind(){return _kind;};
  inline AMSPoint getemissionpoint(int reflected=0){
    return !reflected?_p_direct:_p_reflected;};   // return the intersection point with a given track
  inline AMSDir getemissiondir(int reflected=0)
    {return reflected?_d_direct:_d_reflected;};
  inline AMSPoint getentrancepoint(){return _p_entrance;};
  inline AMSDir getentrancedir(){return _d_entrance;};


  inline geant getclarity(){
    return _kind!=empty_kind?_clarities[_kind-1]:0.;}
  
  inline geant *getabstable(){
    return _kind!=empty_kind?_abs_length[_kind-1]:0;}
  
  inline geant *getindextable(){
    return _kind!=empty_kind?_index_tables[_kind-1]:0;}



};





#endif










