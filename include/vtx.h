// Author J. Alcaraz June-2003
//
//
#ifndef __AMSVtx__
#define __AMSVtx__

#include <upool.h>
#include <apool.h>
#include <gsen.h>
#include <trid.h>
#include <link.h>
#include <commons.h>
#include <cont.h>
#include <event.h>
#include <vector>
#include <trrec.h>
#include <trdrec.h>
#include <point.h>

// Global namespace Vtxconst
namespace Vtxconst{
	  const integer maxtr=2;
}

// Begin AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class AMSVtx: public AMSlink{

protected:

// AMSVtx Protected Data
  number _Momentum;
  number _ErrMomentum;
  number _Theta;
  number _Phi;
  number _Mass;
  integer _Charge;
  number _Chi2;
  integer _Ndof;
  integer _Ntracks;
  AMSTrTrack * _Ptrack[Vtxconst::maxtr];
  AMSTRDTrack * _Ptrd;
  AMSPoint _Vertex;

// AMSVtx Protected Methods
  void _printEl(ostream & stream){ 
                     stream <<
                     " Vtx >>> Run " << AMSEvent::gethead()->getrun() <<
                     " Event " << AMSEvent::gethead()->getEvent() << endl; 
                     
                     stream << 
                     " Vtx >>> #tracks " << _Ntracks << 
                     " Momentum " << _Momentum << 
                     " ErrMomentum " << _ErrMomentum << 
                     " Theta " << _Theta <<
                     " Phi " << _Phi <<
                     " Chi2 " << _Chi2 <<
                     " Ndof " << _Ndof << endl;

                     stream << 
                     " Vtx >>> Vextex Point: " << _Vertex[0] <<
                     "        " << _Vertex[1] <<
                     "        " << _Vertex[2] << endl; 

  }

  void _copyEl();
  void _writeEl();
  static integer Out(integer);
  static void _StartTiming(){ TIMEX(_TimeInit); }
  static bool _NoMoreTime(){
	    float timeNow;
	    TIMEX(timeNow);
	    return timeNow-_TimeInit > AMSFFKEY.CpuLimit;
  }

// AMSVtx Protected Parameters
  static number chi2cut;
  static integer minimum_number_of_layers;
  static integer min_layers_with_different_hits;
  static float _TimeInit;
  static bool InTofPath;

public:

// AMSVtx Public Creators, Destructor, Operators
  AMSVtx(): AMSlink(){};
  AMSVtx(int ntracks, AMSTrTrack *ptrack[]): AMSlink(), _Ntracks(ntracks) {
    for (int i=0; i<ntracks; i++) { _Ptrack[i]=ptrack[i]; }
  }
  ~AMSVtx(){};

// AMSVtx Public Methods
  AMSVtx * next(){return (AMSVtx*)_next;}
  static integer build(integer refit=0);
  static AMSTrTrack* copy_track(AMSTrTrack* ptr);
  static AMSTrTrack* remove_track(AMSTrTrack* ptrack);
  static number Test(AMSTrTrack* ptrack);
  static AMSTrTrack * next_track();
  static AMSTrRecHit * firstgood(integer pattern, integer index);
  static AMSTrRecHit * nextgood(integer pattern, AMSTrRecHit* phit);
  static AMSTrRecHit * firstgood_path(integer pattern, integer index,number par[2][3]);
  static AMSTrRecHit * nextgood_path(integer pattern, AMSTrRecHit* phit,number par[2][3]);
  static integer next_combination(AMSTrTrack& ptrack, int index_min, 
                  int index_max, number par[2][3]);
  static integer Use_for_vtx(AMSTrRecHit* phit, integer pattern);
  static integer Inside_Road(number par[2][3], AMSTrRecHit *ptr);
  void print(){_printEl(cout);}

// AMSVtx Set data
  integer set_all();
  void set_vertex();

// AMSVtx Get data
  number getmom() const {return _Momentum;}
  number geterrmom() const {return _ErrMomentum;}
  number gettheta() const {return _Theta;}
  number getphi() const {return _Phi;}
  number getmass() const {return _Mass;}
  integer getcharge() const {return _Charge;}
  number getchi2() const {return _Chi2;}
  integer getndof() const {return _Ndof;}
  integer getntracks() const {return _Ntracks;}
  AMSTrTrack * gettrack(integer i){return i>=0 && i<_Ntracks? _Ptrack[i]:0;}
  AMSTRDTrack * gettrd(){return _Ptrd;}
  AMSPoint getvert()const {return _Vertex;}

// Friends
  friend class AMSTrTrack;
  friend class AMSTRDTrack;

};
// End AMSVtx CLASS @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif
