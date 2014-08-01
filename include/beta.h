//  $Id$
// V. Choutko 5-june-96
//
// July 10, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak  _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. friend class implementation
//
#ifndef __AMSBETA__
#define __AMSBETA__
#include "link.h"
#include "tofrec02.h"
#include "trrec.h"
#include "point.h"
#include "commons.h"
class AMSBeta: public AMSlink{
protected:
  AMSTOFCluster * _pcluster[4];
  AMSTrTrack * _ptrack;
  integer _Pattern;
  number _Beta ;
  number _InvErrBeta;
  number _BetaC ;
  number _InvErrBetaC;
  number _Chi2;
  number _Chi2Space;
  //! The signed distance along the track from the tof cluster to the plan Z=0
  number _sleng[4];
  void _printEl(ostream & stream);
  void _copyEl();
  void _writeEl();
  number betacorr(number zint,number z0, number part);
  static integer patpoints[npatb];
  static integer patconf[npatb][4];
  static bool BadBetaAlreadyExists(int npatb);
#ifdef _PGTRACK_
  static AMSTrTrack* FindFalseTrackForBeta(integer refit);
  static int BuildBeta(AMSTrTrack* ptrack,int Master=1);
  static integer build_old(integer refit=0,int Master=1);
  
#endif
 public:
  ~AMSBeta();
  AMSBeta(): AMSlink(),_ptrack(0)
  { for(int i=0;i<4;i++){_pcluster[i]=0;_sleng[i]=0;} }
  
  AMSBeta(integer pattern, AMSTOFCluster * pcluster[4], AMSTrTrack * ptrack, number chi2s, number* sleng ): 
    AMSlink(),_ptrack(ptrack),_Pattern(pattern),_Chi2Space(chi2s)
  {for(int i=0;i<4;i++){_pcluster[i]=pcluster[i];  _sleng[i]=sleng[i];}}
    
  double GetTRDBetaCorr();
  AMSBeta(const AMSBeta & o): 
    AMSlink(o._status,o._next),_ptrack(o._ptrack),_Pattern(o._Pattern),_Beta(o._Beta),
    _InvErrBeta(o._InvErrBeta),_Chi2(o._Chi2),_Chi2Space(o._Chi2Space)
  {for(int i=0;i<4;i++){_pcluster[i]=o._pcluster[i];_sleng[i]=o._sleng[i]; }}
      
  AMSBeta *  next(){return (AMSBeta*)_next;}
  

  number getchi2()const{return _Chi2;}
  number getchi2S()const{return _Chi2Space;}
  number getbeta()const{return (checkstatus(AMSDBc::BAD)|| BETAFITFFKEY.MassFromBetaRaw)?_Beta:_BetaC;}
  number getebeta()const{return (checkstatus(AMSDBc::BAD)|| BETAFITFFKEY.MassFromBetaRaw)?_InvErrBeta:_InvErrBetaC;}
  integer getpattern()const{return _Pattern&0x7ffff;}
  void SetAlternateContainer(){_Pattern|=0x80000;}
  bool IsAlternateContainer(){return (_Pattern&0x80000)>0;}
  //! The signed distance along the track from the tof cluster to the plan Z=0
  number getlenght(int tofclusindex) {return (tofclusindex>=0 && tofclusindex<4 )? _sleng[tofclusindex]:0;}
  AMSTrTrack * getptrack()const {return _ptrack;}
  AMSTOFCluster * getpcluster(integer i){return i>=0 && i<4? _pcluster[i]:0;}
  void SimpleFit(integer nhit, number sleng[]);
  static integer _addnextP(integer pat, integer nhit, number sleng[],
			  AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number chi2s,int Master=1);
  static integer _addnext(integer pat, integer nhit, number sleng[],
			  AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number theta, number chi2s,int Master=1);
  static integer _addnext(integer pat, integer nhit, number sleng[],
			  AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number chi2s,int Master=1);
  static AMSPoint Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
			   number & sleng, number & theta);
  static integer build(integer refit=0);
  static void print();
#ifdef __WRITEROOT__
  friend class BetaR;
#endif
};

#endif
