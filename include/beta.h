// V. Choutko 5-june-96
//
// July 10, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak  _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. friend class implementation
//
#ifndef __AMSBETA__
#define __AMSBETA__
#include <link.h>
#include <tofrec.h>
#include <trrec.h>
#include <point.h>
#include <commons.h>
#include <ctcrec.h>
class AMSBeta: public AMSlink{
protected:
 AMSTOFCluster * _pcluster[4];
 AMSTrTrack * _ptrack;
 integer _Pattern;
 number _Beta ;
 number _InvErrBeta;
 number _Chi2;
 void _printEl(ostream & stream);
 void _copyEl();
 void _writeEl();
 static integer patpoints[npatb];
 static integer patconf[npatb][4];

public:
 AMSBeta(): AMSlink(),_ptrack(0){for(int i=0;i<4;i++)_pcluster[i]=0;}
 AMSBeta(integer pattern, AMSTOFCluster * pcluster[4], AMSTrTrack * ptrack): 
         AMSlink(),_Pattern(pattern),_ptrack(ptrack)
         {for(int i=0;i<4;i++)_pcluster[i]=pcluster[i];}
 AMSBeta(const AMSBeta & o): 
         AMSlink(o._status,o._next),_Pattern(o._Pattern),_Beta(o._Beta),
         _InvErrBeta(o._InvErrBeta),_Chi2(o._Chi2),_ptrack(o._ptrack)
         {for(int i=0;i<4;i++)_pcluster[i]=o._pcluster[i];}
AMSBeta *  next(){return (AMSBeta*)_next;}
number getchi2()const{return _Chi2;}
number getbeta()const{return _Beta;}
number getebeta()const{return _InvErrBeta;}
integer getpattern()const{return _Pattern;}
AMSTrTrack * getptrack()const {return _ptrack;}
AMSTOFCluster * getpcluster(integer i){return i>=0 && i<4? _pcluster[i]:0;}
 void SimpleFit(integer nhit, number sleng[]);
static integer _addnext(integer pat, integer nhit, number sleng[],
AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number theta);
static AMSPoint Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
                       number & sleng, number & theta);
static void build();
static void print();
//+
#ifdef __DB__
   friend class AMSBetaD;
#endif
 void setTrackP (AMSTrTrack* p)  { _ptrack = p;}
 void setTOFClP (AMSTOFCluster* p, integer ntof)  
                                 { if (ntof>=0 && ntof<4) _pcluster[ntof]= p;}
//-
};

#endif
