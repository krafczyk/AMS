// Author V. Choutko 24-may-1996
//
// May 27, 1996. ak. add functions to AMSTrRecHit
//                   _addnext is public now (though not neccessary)
//                   AMSTrRecHit::setClusterP
//                   AMSTrTrack:: (see getParFastFit), getNumbers
//                   _Pthit[6] ??? ! it is correct
//                   default constructor for AMSTrTrack
// June, 1996. ak.   getNumbers, setNumbers, setid for AMSTrCluster&AMSTrRecHit
// July 03, 1996. ak. V1.09 + alpha code
// Aug  07, 1996. ak. V1.24 (AMSTrTrack functions are modified accord)
// Sep  10, 1996. ak. V1.25
// Oct  10, 1996. ak. implement friend class
// Mar  15, 1997. ak. there were two same functions AMSTrTrack::getHitP and 
//                    getpthit. getHitP is removed
//                    new function AMSTrREcHit::setSensorP
// Last Edit : Mar 20, 1997. ak.
//
#ifndef __AMSTRBANK__
#define __AMSTRBANK__
#include <upool.h>
#include <apool.h>
#include <gsen.h>
#include <trid.h>
#include <link.h>
#include <commons.h>
#include <cont.h>
#include <event.h>


class Test: public AMSlink{
protected:
 integer _pid;
 void _copyEl(){}
 void _printEl(ostream & stream){}
 void _writeEl(){}
public:
 Test(integer pid=0):_pid(pid){}
};





class AMSTrCluster: public AMSlink {
protected:
  void _ErrorCalc();
  void _printEl(ostream & stream)
  { stream << _Id << " Nelem " << -_NelemL+_NelemR << " Sum " << _Sum <<endl;}
  void _copyEl();
  void _writeEl();
AMSTrIdSoft _Id; // center
integer _NelemL; // to left
integer _NelemR; // to right
number _Sum;
number _Sigma;
number _Mean;    // with respect to center
number _Rms;
number _ErrorMean; // Guessed Error

number * _pValues;

static void _addnext(const AMSTrIdSoft& id, integer status, integer nelemL,
  integer nelemR, number sum,  number ssum, number pos, number rms, 
  number val[]);





public:
     static const integer WIDE;
     static const integer NEAR;
     static const integer REFITTED;
     static const integer WEAK;
     static const integer AwayTOF;
static integer Out(integer);
number getVal(){return _Sum;}
number getcofg(integer side, AMSTrIdGeom * id);
number cfgCorFun(number s, AMSTrIdGeom * id);
number getecofg(){return _ErrorMean;}
//+
#ifdef __DB__
   friend class AMSTrClusterD;
#endif

     integer getnelem()      {return -_NelemL+_NelemR;}
     number  getrms()        {return _Rms;}
     number  getsigma()      {return _Sigma;}

inline void setid(const AMSTrIdSoft& id) { _Id = id;}
number getMean() { return _Mean;}
void   getValues(number* values)
       { if(getnelem() > 0) 
         UCOPY (_pValues, values, sizeof(number)*getnelem()/4); }
void   setValues(number* values)
  {
   if(getnelem()>0){
    _pValues=(number*)UPool.insert(getnelem()*sizeof(number));
    for(int i=0;i<getnelem();i++) _pValues[i] = values[i];
   }
  }
integer getSide() {return _Id.getside();}
//-

AMSTrIdSoft getid()const {return _Id;}
AMSTrCluster(const AMSTrIdSoft& id, integer status, integer nelemL, 
integer nelemR, number sum,number ssum, number pos, number rms, 
number val[]);
AMSTrCluster *  next(){return (AMSTrCluster*)_next;}

//default builder
static integer build(integer refit=0);
static integer buildWeak(integer refit=0);

static void print();
AMSTrCluster():AMSlink(){_NelemL=0; _NelemR=0;_pValues=0;};
~AMSTrCluster(){if(_pValues)UPool.udelete(_pValues);}
integer operator < (AMSlink & o) const {
// No RTTI - take a "risk" here
  AMSTrCluster * p= (AMSTrCluster*)(&o);

 integer ithis =   (!checkstatus(AMSDBc::USED))
                 + 2*(checkstatus(AMSTrCluster::AwayTOF));
 integer iother =   (!p->checkstatus(AMSDBc::USED))
                  + 2*(p->checkstatus(AMSTrCluster::AwayTOF));

 if (ithis<iother)return 1;
 else return 0;

}

};

class AMSTrRecHit: public AMSlink{
public:
     static const integer FalseX;
     static const integer FalseTOFX;
     static const integer AwayTOF;
protected:
AMSgSen * _pSen;
AMSTrCluster *_Xcl;
AMSTrCluster *_Ycl;
integer      _Layer;
AMSPoint     _Hit;
AMSPoint     _EHit;
number       _Sum;
number       _DifoSum;


static AMSTrRecHit* _Head[6];
static void _addnext(AMSgSen * p, integer ,integer ,AMSTrCluster *, 
            AMSTrCluster *,  const AMSPoint &, const AMSPoint &);
  void _copyEl();
  void _writeEl();

public:
  void _printEl(ostream & stream){ stream << " Status " << _status << " Layer " << 
  _Layer <<" Coo " << _Hit<< endl;}
integer operator < (AMSlink & o) const {
  AMSTrRecHit * p= (AMSTrRecHit*)(&o);

  integer ithis =   (!checkstatus(AMSDBc::USED))
                  + 2*(checkstatus(AMSTrRecHit::AwayTOF));
  integer iother =   (!p->checkstatus(AMSDBc::USED))
                   + 2*(p->checkstatus(AMSTrRecHit::AwayTOF));

  if (ithis<iother)return 1;
  else return 0;



} 

inline integer Good() { 
  return (TRFITFFKEY.FullReco!=0 || checkstatus(AMSDBc::USED)==0) 
            && checkstatus(AMSTrRecHit::AwayTOF)==0;
}
static integer markAwayTOFHits();

AMSTrRecHit *  next(){return (AMSTrRecHit*)_next;}

void resethash(integer i, AMSlink *head){
if(i>=0 && i <6)_Head[i]=(AMSTrRecHit*)head;
}


static AMSTrRecHit * gethead(integer i=0){
   if(i>=0 && i<6){
    if(!_Head[i])_Head[i]=(AMSTrRecHit*)AMSEvent::gethead()->getheadC("AMSTrRecHit",i);
    return _Head[i];
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTrRecHit:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}





AMSTrRecHit(AMSgSen *p, integer good,integer layer, AMSTrCluster * xcl, AMSTrCluster * ycl,
            const AMSPoint & hit, const AMSPoint & ehit, number sum, number dfs): AMSlink(good,0),
            _pSen(p), _Layer(layer),_Xcl(xcl),
            _Ycl(ycl), _Hit(hit), _EHit(ehit),_Sum(sum),_DifoSum(dfs){};
AMSTrRecHit(): AMSlink(),_pSen(0),_Xcl(0),_Ycl(0){};
static integer build(integer refit=0);
static integer buildWeak(integer refit=0);
static void print();
static integer Out(integer);
number getsum()const{return _Sum;}
AMSgSen * getpsen()const{return _pSen;}
inline  AMSPoint  getHit(){return _Hit;}
inline  AMSPoint  getEHit(){return _EHit;}
//+
#ifdef __DB__
   friend class AMSTrRecHitD;
#endif
inline  integer       getLayer()             {return _Layer;}
inline  number        getSum()               { return _Sum;}
inline  number        getDSum()              { return _DifoSum;}
inline  AMSTrCluster* getClusterP(integer n) {
                                              AMSTrCluster* p = _Xcl;
                                              if(n == 1)    p = _Ycl;
                                              return p;
}
void          setClusterP(AMSTrCluster* p,integer n) {
              if (n == 0) _Xcl = p;
              if (n == 1) _Ycl = p;
}
void          setSensorP(AMSgSen* p) { _pSen = p;}

//-
~AMSTrRecHit(){int i;for( i=0;i<6;i++)_Head[i]=0;};
   friend class AMSTrTrack;

};

class AMSTrTrackError{
private:
 char msg[256];
public:
 AMSTrTrackError(char * name);
 char * getmessage();
};

class AMSTrTrack: public AMSlink{
protected:
AMSTrRecHit * _Pthit[6];

integer _Pattern;
integer _NHits;
integer _FastFitDone;
integer _GeaneFitDone;
integer _AdvancedFitDone;

number _Chi2StrLine;
number _Chi2Circle;
number _CircleRidgidity;
number _Chi2FastFit;
number _Ridgidity;
number _ErrRidgidity;
number _Theta;
number _Phi;
AMSPoint _P0;
number _GChi2;
number _GRidgidity;    //  Ridgidity or momentum if (geanefitdone != 14)
number _GErrRidgidity; //  err of  see above
number _GTheta;
number _GPhi;
AMSPoint _GP0;
number _HChi2[2];
number _HRidgidity[2];
number _HErrRidgidity[2];
number _HTheta[2];
number _HPhi[2];
AMSPoint _HP0[2];
number _Chi2MS;
number _GChi2MS;
number _RidgidityMS;
number _GRidgidityMS;
void SimpleFit(AMSPoint err=0);
void TOFFit(integer ntof, AMSPoint tofhit, AMSPoint etofhit);
static void _Start(){_Time=HighResTime();}
static geant _CheckTime(){return HighResTime()-_Time;}
static geant _Time;
  void _printEl(ostream & stream){ stream << " Pattern " << _Pattern << " Rigidity (Circ)" << 
  _CircleRidgidity <<" Rigidity (Fast) "<<_Ridgidity <<" Chi2Fast " << 
  _Chi2FastFit << " ThetaFast "<<_Theta<<" PhiFast "<<_Phi<<endl;}
  void _copyEl();
  void _writeEl();

static integer _addnext(integer pat, integer nhits, AMSTrRecHit* phit[]);
static void   _addnextR(AMSTrTrack* ptr, integer pat, integer nhits, AMSTrRecHit* phit[]);
static integer _addnextFalseX(integer pat, integer nhits, AMSTrRecHit* phit[]);
static integer Distance(number par[2][2], AMSTrRecHit *ptr);
static integer DistanceTOF(number par[2][2], AMSTrRecHit *ptr);
static integer patpoints[npat];
static integer patconf[npat][6];
static integer patmiss[npat][6];
static integer _RefitIsNeeded;
public:
  integer intercept(AMSPoint &P1, integer layer, number &theta, number &phi);
static integer & RefitIsNeeded(){return _RefitIsNeeded;}
integer operator < (AMSlink & o) const {
  AMSTrTrack * p= (AMSTrTrack*)(&o);
  if (checkstatus(AMSDBc::USED) && !(p->checkstatus(AMSDBc::USED)))return 1;
  else return 0;
}

void   AdvancedFit(int forced=0);
integer getpattern()const{return _Pattern;}
static integer Out(integer);
number Fit(integer i=0, integer ipart=14);
AMSTrTrack(const AMSTrTrack & o):AMSlink(o._status,o._next),_Pattern(o._Pattern),
_NHits(o._NHits),_GeaneFitDone(o._GeaneFitDone),_FastFitDone(o._FastFitDone),
_AdvancedFitDone(o._AdvancedFitDone),_Chi2StrLine(o._Chi2StrLine),
_Chi2Circle(o._Chi2Circle),_CircleRidgidity(o._CircleRidgidity),
_Chi2FastFit(o._Chi2FastFit),_Ridgidity(o._Ridgidity),
_RidgidityMS(o._RidgidityMS),_GRidgidityMS(o._GRidgidityMS),
_Chi2MS(o._Chi2MS),_GChi2MS(o._GChi2MS),
_ErrRidgidity(o._ErrRidgidity),_Theta(o._Theta), _Phi(o._Phi),_P0(o._P0),
_GChi2(o._GChi2), _GRidgidity(o._GRidgidity),_GErrRidgidity(o._GErrRidgidity),
_GTheta(o._GTheta),_GPhi(o._GPhi),_GP0(o._GP0){
int i;
for( i=0;i<6;i++)_Pthit[i]=o._Pthit[i];
for(i=0;i<2;i++){
  _HChi2[i]=o._HChi2[i];
  _HRidgidity[i]=o._HRidgidity[i];
  _HErrRidgidity[i]=o._HErrRidgidity[i];
  _HTheta[i]=o._HTheta[i];
  _HPhi[i]=o._HPhi[i];
  _HP0[i]=o._HP0[i];
} 
}
~AMSTrTrack(){};
AMSTrTrack *  next(){return (AMSTrTrack*)_next;}
AMSTrTrack (integer pattern, integer nhits, AMSTrRecHit * phit[]): 
AMSlink(0,0),_Pattern(pattern), _NHits(nhits),_GeaneFitDone(0), _AdvancedFitDone(0),_FastFitDone(0)
  {init(  phit);}
void init( AMSTrRecHit * phit[]);
static integer build(integer refit=0);
static integer buildWeak(integer refit=0);
static integer buildFalseX(integer refit=0);
static integer makeFalseTOFXHits();
static integer buildFalseTOFX(integer refit=0);
static void print();
AMSTrRecHit * getphit(integer i){return i>=0 && i<6? _Pthit[i]:0;}
void interpolate(AMSPoint  pnt, AMSDir  dir,  AMSPoint & P1, 
                 number & theta, number & phi, number & length);
void interpolateCyl(AMSPoint  pnt,  AMSDir dir, number rad, number idir, 
                    AMSPoint & P1, number & theta, number & phi, 
                    number & length);
void getParFastFit(number&  Chi2, number& Rid, number&  Err, 
number&  Theta, number & Phi, AMSPoint&  X0)const ; 

void getParSimpleFit(number & Chi2xy, number &Chi2sz, number & Rid)const;
integer AdvancedFitDone()const{return _AdvancedFitDone;}
integer GeaneFitDone()const{return _GeaneFitDone;}
void getParAdvancedFit(number&   GChi2, number&  GRid, number&  GErr,
number&  GTheta, number&  GPhi, AMSPoint&  GP0,
number HChi2[2], number HRid[2], number HErr[2], number HTheta[2], 
number HPhi[2], AMSPoint  HP0[2] ) const;

//+
#ifdef __DB__
   friend class AMSTrTrackD;
#endif
AMSTrTrack() {_Pattern = -1; 
              _NHits   = -1; 
              for (int i=0; i<6; i++) _Pthit[i] = NULL; }
void   setHitP(AMSTrRecHit* p, integer n) {if (n< 6)  _Pthit[n] = p;}
//-
integer TOFOK();
integer getnhits() const {return _NHits;}
number getgrid() const {return _GRidgidity;}
number getrid() const {return _Ridgidity;}
number gettheta() const {return _Theta;}
number getphi() const {return _Phi;}
};

#endif
