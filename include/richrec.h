//  $Id: richrec.h,v 1.10 2001/09/11 08:07:21 choutko Exp $

#ifndef __RICHREC__
#define __RICHREC__
#include <richdbc.h>
#include <iostream>
// Define Safe if you want to use safe arrays (under construction)
//#define __SAFE__


PROTOCCALLSFSUB6(SOLVE,solve,FLOAT,FLOAT,FLOAT,FLOAT,FLOATV,INT)
#define SOLVE(A1,A2,A3,A4,A5,A6) CCALLSFSUB6(SOLVE,solve,FLOAT,FLOAT,FLOAT,FLOAT,FLOATV,INT,A1,A2,A3,A4,A5,A6)

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

// Container for hits

class AMSRichRawEvent: public AMSlink{
private:
  integer _channel; // (PMT number-1)*16+window number
  integer _counts;  // 0 means true, 1 means noise
	 
public:
  AMSRichRawEvent(integer channel,integer counts):AMSlink(),
    _channel(channel),_counts(counts){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  integer getcounts() {return _counts;}

  static void mc_build();
  void reconstruct(AMSPoint,AMSPoint,AMSDir,AMSDir,geant,
		   geant,
#ifdef __SAFE__
                   geant_small_array &);
#else
                   geant *);
#endif

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
// interface with DAQ :

protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSRichRawEvent: id="<<_channel<<endl;
    stream <<" Adc="<<_counts<<endl;
  }
  void _writeEl();
  void _copyEl(){};
};



// Container for rings. Each ring is a reconstructed track

class AMSRichRing: public AMSlink{

#ifdef __WRITEROOT__
friend class AMSTrTrack;
#endif
private:

  AMSTrTrack* _ptrack;
  integer _used;
  integer _charge;
  number  _beta;
  number  _errorbeta;
  number  _quality; //probability per hit for the cluster

// A parametrisation
  static inline geant Sigma(geant beta,geant A,geant B){
      return beta*(B*beta+A)*1.e-2;
    }

  static inline integer closest(geant beta,
#ifdef __SAFE__
                                geant_small_array betas){
#else
                                geant *betas){
#endif

    geant a=fabs(beta-betas[0]);
    geant b=fabs(beta-betas[1]);
    geant c=fabs(beta-betas[2]);
    if(b<c) {if(a<b) return 0; else return 1;}
    else    {if(a<c) return 0; else return 2;}
  }

protected:
  void _printEl(ostream &stream){stream<<" Beta "<<_beta<<" Error "<<_errorbeta<<endl;}
  void _writeEl();
  void _copyEl(){};
  void CalcBetaError();
public:
  AMSRichRing(AMSTrTrack* track,int used,geant beta,geant quality,int charge):AMSlink(),
   _ptrack(track),_used(used),_beta(beta),_quality(quality),_charge(charge){CalcBetaError();};
  ~AMSRichRing(){};
  AMSRichRing * next(){return (AMSRichRing*)_next;}

  static void build();
  static void rebuild(AMSTrTrack *ptrack);
  AMSTrTrack* gettrack(){return _ptrack;}
  integer getused(){return _used;}
  integer getcharge(){return _charge;}
  number getbeta(){return _beta;}
  number geterrorbeta(){return _errorbeta;}
  number getquality(){return _quality;}
};


#endif





