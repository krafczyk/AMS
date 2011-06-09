//  $Id: trdrec.h,v 1.24 2011/06/09 16:53:47 choutko Exp $
#ifndef __AMSTRDREC__
#define __AMSTRDREC__
#include "trdid.h"
#include "commons.h"
#include "link.h"
#include "cont.h"
#include "trdsim.h"
class AMSTrTrack;
class AMSTRDCluster: public AMSlink{
protected:
AMSPoint _Coo;   // Global Coordinates 
integer _layer;  //  layer from 0 to 19 increasing with z 
number _ClSizeR;       // tube radius
number _ClSizeZ;      // tube 1/2 length
AMSDir   _CooDir;      // Dir Cosinuses 
uinteger _Multiplicity;
uinteger _HighMultiplicity;
float    _Edep;    // in KeV;
AMSTRDRawHit *_pmaxhit;
void _init(){}
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static AMSTRDCluster* _Head[trdconst::maxlay];
#pragma omp threadprivate (_Head)
public:
AMSTRDCluster(uinteger status, uinteger layer,AMSPoint coo, number hsr, number hdz,AMSDir dir, float edep,int multip, int hmultip, AMSTRDRawHit* pmaxhit):AMSlink(status,0),_Coo(coo),_ClSizeR(hsr),_ClSizeZ(hdz),_CooDir(dir),_Edep(edep),_Multiplicity(multip),_HighMultiplicity(hmultip),_pmaxhit(pmaxhit),_layer(layer){};
static integer build(int rerun=0);
static integer Out(integer status);
static double RangeCorr(double range,double norm);
number getEdep() const {return _Edep;}
integer operator < (AMSlink & o) const {
 
  AMSTRDCluster * p= dynamic_cast<AMSTRDCluster*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _layer < p->_layer;
}
static AMSTRDCluster * gethead(uinteger i=0);
number getHit(uinteger i);
number getEHit(){return _ClSizeR;}
number getHitL(){return _ClSizeZ;}
uinteger getlayer()const {return _layer;}
const AMSDir & getCooDir()const {return _CooDir;}
 AMSPoint & getCoo() {return _Coo;}
inline integer Good(int bit,int exist=false) {
#ifdef _PGTRACK_ 
   //PZ FIXME TRFITFFKEY
   bool cond=(( checkstatus(AMSDBc::USED)==0)&& checkstatus(AMSDBc::GOOD));
#else
  bool cond=((TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0)&& checkstatus(AMSDBc::GOOD));
#endif
  if(exist){
  return bit<0?cond:cond && (checkstatus(bit)!=0);
  }
  else{
  return bit<0?cond:cond && (checkstatus(bit)==0);
  } 
             
}
uinteger getmult()const{return _Multiplicity;}
uinteger gethmult()const {return _HighMultiplicity;}

AMSTRDCluster *  next(){return (AMSTRDCluster*)_next;}
void resethash(integer i, AMSlink *head){
if(i>=0 && i <TRDDBc::nlay())_Head[i]=(AMSTRDCluster*)head;
}
~AMSTRDCluster(){if(_pos==1)_Head[_layer]=0;};

#ifdef __WRITEROOT__
 friend class TrdClusterR;
#endif
};

class AMSTRDSegment: public AMSlink{
protected:
static integer _case;
#pragma omp threadprivate (_case)
integer _Orientation;
number _FitPar[2];
number _Chi2;
uinteger _NHits;
integer _Pattern;
integer _SuperLayer;
AMSTRDCluster * _pCl[trdconst::maxhits];
void _init(){}
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static integer pat;
#pragma omp threadprivate (pat)
static number par[2];
#pragma omp threadprivate (par)
static AMSTRDCluster * phit[trdconst::maxhits];
#pragma omp threadprivate (phit)
static integer _TrSearcher(int icall,uinteger iseg);
static integer _addnext(integer pat, integer nhit, uinteger iseg,AMSTRDCluster* pthit[]);
void _addnextR(uinteger iseg);
public:
int getori()const {return _Orientation;};
AMSTRDSegment():AMSlink(),_Chi2(-1),_Orientation(-1),_NHits(0),_Pattern(-1),_SuperLayer(-1){
_FitPar[0]=_FitPar[1]=0;
for(int i=0;i<trdconst::maxhits;i++){
  _pCl[i]=0;
}
}
AMSTRDSegment(integer slay,integer Pattern,uinteger nhits,integer ori,AMSTRDCluster *pcl[]):_Chi2(-1),_Orientation(ori),_NHits(nhits),_Pattern(Pattern),_SuperLayer(slay){
_FitPar[0]=_FitPar[1]=0;
for(int i=0;i<(_NHits<trdconst::maxhits?_NHits:trdconst::maxhits);i++){
 _pCl[i]=pcl[i];
}
}
integer operator < (AMSlink & o) const {
 
  AMSTRDSegment * p= dynamic_cast<AMSTRDSegment*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _Pattern < p->_Pattern;
}
integer getslayer()const{return _SuperLayer;}
uinteger getNHits()const {return _NHits;}
number getFitPar(uinteger i)const {return i<2?_FitPar[i]:0;}
AMSTRDCluster *getpHit(uinteger i){return i<_NHits?_pCl[i]:0;}
static AMSTRDSegment * gethead(uinteger i=0);
static integer build(int rerun=0);
void Fit();
static integer Out(integer status);

static bool Distance1D(number par[2], AMSTRDCluster *ptr);

inline integer Good(int bit,int exist=false) {
#ifdef _PGTRACK_
//PZ FIXME   TRFITFFKEY
  bool cond=( checkstatus(AMSDBc::USED)==0);
#else
  bool cond=(TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0);
#endif
  if(exist){
  return bit<0?cond:cond && (checkstatus(bit)!=0);
  }
  else{
  return bit<0?cond:cond && (checkstatus(bit)==0);
  } 
             
}
AMSTRDSegment *  next(){return (AMSTRDSegment*)_next;}

#ifdef __WRITEROOT__
 friend class TrdSegmentR;
#endif
};



class AMSTRDTrack: public AMSlink{
protected:
class TrackPar{
public:
bool _FitDone;
AMSPoint _Coo;
AMSPoint _ErCoo;
number  _InvRigidity;
number _ErrInvRigidity;
number _Phi;
number _Theta;
number _ErPhi;
number _ErTheta;
number _Chi2;
TrackPar():_FitDone(false),_Coo(0,0,0),_ErCoo(0,0,0),_InvRigidity(0),_ErrInvRigidity(0),_Phi(0),_Theta(0),_ErPhi(0),_ErTheta(0),_Chi2(0){};
TrackPar(const TrackPar & o):_FitDone(o._FitDone),_Coo(o._Coo),_ErCoo(o._ErCoo),_InvRigidity(o._InvRigidity),_ErrInvRigidity(o._ErrInvRigidity),_Phi(o._Phi),_Theta(o._Theta),_ErPhi(o._ErPhi),_ErTheta(o._ErTheta),_Chi2(o._Chi2){};
};
class TrackBase{
public:
integer _NHits;
integer _Pattern;
AMSTRDCluster * _PCluster[trdconst::maxlay];
AMSPoint _Hit[trdconst::maxlay];
AMSPoint _EHit[trdconst::maxlay];
AMSDir _HDir[trdconst::maxlay];
TrackBase(const TrackBase & o):_NHits(o._NHits),_Pattern(o._Pattern){
 for (int i=0;i<trdconst::maxlay;i++){
  _PCluster[i]=o._PCluster[i];
  _Hit[i]=o._Hit[i];
  _EHit[i]=o._EHit[i];
  _HDir[i]=o._HDir[i];
 }
}
TrackBase():_NHits(0),_Pattern(-1){
 for (int i=0;i<trdconst::maxlay;i++){
  _PCluster[i]=0;
  _Hit[i]=0;
  _EHit[i]=0;
  _HDir[i]=AMSDir(0,0,1);
 }
}
};
public:
class TrackCharge{
public:
float Q;
float Nused;
short int Charge[10];
float ChargeP[10];
TrackCharge():Q(0),Nused(0){
for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=-1;
for(int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=10000;
}
static float ChargePDF[10030]; //0-100 kev; [1000]=0.1  [1001] charge [1002] not set

static float* getTRDPDF(){return ChargePDF;}
static int  getTRDPDFsize(){return sizeof(ChargePDF);}
};

class TrackBaseS{
public:
integer _NSeg;
integer _Pattern;
AMSTRDSegment * _PSeg[trdconst::maxseg];
TrackBaseS(const TrackBaseS & o):_NSeg(o._NSeg),_Pattern(o._Pattern){
 for (int i=0;i<trdconst::maxseg;i++){
  _PSeg[i]=o._PSeg[i];
 }
}
TrackBaseS(int pat, int nhit, AMSTRDSegment * ps[]):_NSeg(nhit),_Pattern(pat){
 for (int i=0;i<nhit;i++){
  _PSeg[i]=ps[i];
 }
}
TrackBaseS():_NSeg(0),_Pattern(-1){
 for (int i=0;i<trdconst::maxseg;i++){
  _PSeg[i]=0;
 }
}
};
public:
TrackPar _StrLine;
TrackPar _Real;
TrackBase _Base;
TrackBaseS _BaseS;
TrackCharge _Charge;
static integer _case;
#pragma omp threadprivate (_case)
bool _update;
void _init(){};
void _printEl(ostream &o);
void _copyEl();
void _writeEl();
static integer pat;
#pragma omp threadprivate (pat)
static number Distance3D(AMSPoint p, AMSDir d, AMSTRDCluster *ptr);
static AMSTRDSegment * phit[trdconst::maxseg];
#pragma omp threadprivate (phit)
static integer _TrSearcher(int icall);
static integer _addnext(integer pat, integer nhit, AMSTRDSegment* pthit[]);
void _addnextR();
public:
AMSTRDTrack():AMSlink(),_Base(),_Charge(),_BaseS(),_StrLine(),_Real(),_update(false){};
AMSTRDTrack(const AMSTRDTrack::TrackBase & Base, const AMSTRDTrack::TrackBaseS & BaseS, const AMSTRDTrack::TrackPar & StrLine):AMSlink(),_Charge(),_Base(Base),_BaseS(BaseS),_StrLine(StrLine),_Real(),_update(false){};
AMSTRDTrack(const AMSTRDTrack::TrackBaseS & BaseS):AMSlink(),_Base(),_BaseS(BaseS),_StrLine(),_Real(),_update(false),_Charge(){};
static integer build(int rerun=0);
static bool ResolveAmb(AMSTrTrack *ptrack);
void StrLineFit(bool update=true);
void ComputeCharge(double bc);
void RealFit();
static bool CreatePDF( char *fnam=0);
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall){};
static void alfun(integer & n, number xc[], number & fc, AMSTRDTrack * ptr);
static integer Out(integer status);
AMSPoint getCooStr()const {return _StrLine._Coo;}
AMSDir getCooDirStr()const {return AMSDir(_StrLine._Theta,_StrLine._Phi);}
AMSPoint getECooStr()const {return _StrLine._ErCoo;}
bool IsEcalCandidate();
bool Veto(int last);
AMSTRDTrack *  next(){return (AMSTRDTrack*)_next;}
integer operator < (AMSlink & o) const {
 
  AMSTRDTrack * p= dynamic_cast<AMSTRDTrack*>(&o);
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
  else return !p || _BaseS._Pattern < p->_BaseS._Pattern;
}

bool IsHighGammaTrack();
static void _Start(){TIMEX(_Time);}
static geant _CheckTime(){geant tt1;TIMEX(tt1);return tt1-_Time;}
static bool _NoMoreTime();
static geant _Time;
#pragma omp threadprivate (_Time)
static geant _TimeLimit;
#pragma omp threadprivate (_TimeLimit)

number getphi(){return _StrLine._Phi;}
number gettheta(){return _StrLine._Theta;}
AMSPoint getcoo(){return _StrLine._Coo;}

#ifdef __WRITEROOT__
 friend class  TrdTrackR;
#endif
};

#endif
