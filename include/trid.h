// Author V. Choutko 24-may-1996
//
// Last edit : Mar 19, 1997. ak. add AMSTrIdSoft::getidgeom() function 
//
#ifndef __AMSTRID__
#define __AMSTRID__
#include <typedefs.h>
#include <amsdbc.h>
#include <trrawcluster.h>
#include <iostream.h>
#include <amsstl.h>
#include <commons.h>
#include <link.h>
#include <job.h>
const int ncrt=6;
const int ntdr=32;
class AMSTrIdGeom{
integer _layer;    // from 1 to 6
integer _ladder;   // from 1 to AMSDBc::nlad(_layer)
integer _sensor;   // from 1 to AMSDBc::nsen(_layer,_ladder)
integer _stripx;   // from 0 to AMSDBc::NStripsSen(_layer,0)-1
integer _stripy;   // from 0 to AMSDBc::NStripsSen(_layer,1)-1
void _check();
public:
friend class AMSTrIdSoft;
AMSTrIdGeom():_layer(1),_ladder(1),_sensor(1),_stripx(0),_stripy(0){};
~AMSTrIdGeom(){};
AMSTrIdGeom( const AMSTrIdGeom& o):_layer(o._layer),_ladder(o._ladder),
_sensor(o._sensor),_stripx(o._stripx),_stripy(o._stripy){}
AMSTrIdGeom(integer layer,integer ladder,integer sensor, integer stripx,
integer stripy);
AMSTrIdGeom(integer idsoft, integer stripx=0, integer stripy=0);
AMSTrIdGeom & operator = (const AMSTrIdGeom &o);
AMSID crgid();
integer operator == (const AMSTrIdGeom &o);
void R2Gx(integer stripx);
number  getsize(integer side)const;
number  getcofg(integer side, integer shift, integer & error)const;
inline void upd(integer side,integer strip){
#ifdef __AMSDEBUG__
       assert(side>=0 && side<2);
       assert (strip>=0 && strip <getmaxstrips(side));
#endif
if(side==0)_stripx=strip;
else _stripy=strip;
}
inline integer getlayer() const {return _layer;}
inline integer getladder()const {return _ladder;}
inline integer gethalf()const {return _sensor<=AMSDBc::nhalf(_layer,_ladder)?0:1;}
inline integer getsensor()const {return _sensor;}        
inline integer cmpt()const {return _layer+10*_ladder+1000*_sensor;}
inline integer getstrip (integer i) const{if(i==0)return _stripx;else return _stripy;} 
inline number getmaxsize(integer i)  {return AMSDBc::ssize_active(_layer-1,i);}
inline integer getmaxstrips(integer i) const
{return AMSJob::gethead()->isRealData()? AMSDBc::NStripsSenR(_layer,i):AMSDBc::NStripsSen(_layer,i);}
integer size2strip(integer side, number size);


inline number strip2size(integer side){
     #ifdef __AMSDEBUG__
       assert(side>=0 && side<2 && !AMSJob::gethead()->isRealData());
     #endif
     if(side==0)return _swxyl[_layer-1][0][_stripx];
     else       return _swxyl[_layer-1][1][_stripy];
}

static integer debug;
static number *  _swxy[nl][2] ;  // strip size x,y
static number *  _swxyl[nl][2];   // integral of strip size
//
// we have to have yet different apporiach to real data
//
static number *  _swxyR[nl][2] ;  // strip size x,y
static number *  _swxyRl[nl][2];   // integral of strip size

static void init();
};
const integer ms=4000;
class AMSTrIdSoft{
protected:
integer _dead;    // dead   if 1  ; alive otherwise
integer _layer;   // from 1 to 6
integer _drp;     // from 1 to AMSDBc::nlad(_layer)
integer _side;    // 0=x 1=y
integer _half;    // from 0 to 1
integer _strip;   // from 0 to getmaxstrips()-1
integer _addr;    // Calculated from the above
int16u _haddr;    // Hardware address
int16u _crate;    // Crate no
integer _VANumber; // from 0 to 5 (9)
AMSTrIdGeom * _pid;
static integer idsoft2linear[ms];
static integer *status;
static geant *peds;
static geant *gains;
static geant *sigmas;
static geant *sigmaraws;
static geant cmnnoise[10][ms];
static geant *indnoise;
static uinteger * rhomatrix;
static integer _numel;
static integer _numell;
const static integer _VAChannels;
void _check();
void _mkhaddr();
void _mkcrate();
 static integer _GetGeo[ncrt][ntdr][2][3];   // crate,tdrs,side ->
                                             //layer,ladder,half
 static integer _GetHard[nl][nld][2][3];     // layer,ladder,half ->
                                             // tdrs(0&1), crate
  

public:
friend ostream &operator << (ostream &o, const  AMSTrIdSoft &b )
   {return o<<" "<<b._layer<<" "<<b._drp<<" "<<b._side<<" "<<b._half<<" "<<b._strip<<" "<<b._dead<<endl;}
static void init();
static void inittable();
integer operator < (const AMSTrIdSoft & o)const;

static inline integer getnchan() {return _numel;}
inline integer dead() const {return _dead==1;}
inline int16u gethaddr(integer pedantic=0)const {
                        return pedantic!=0 ? _haddr : _haddr | _half<<15;}
inline int16u getcrate() const {return _crate;}
inline integer gettdr() const {return (_haddr>>10)&31;}
inline integer getaddr() const {return _addr;}
inline integer getchannel() const { return idsoft2linear[_addr]+_strip;}
inline integer checkstatus(integer checker) const 
{return status[idsoft2linear[_addr]+_strip] & checker;}
inline geant getped() const {return peds[idsoft2linear[_addr]+_strip];}
inline geant getsig() const {return sigmas[idsoft2linear[_addr]+_strip];}
inline geant getsigraw() const {return sigmaraws[idsoft2linear[_addr]+_strip];}
inline geant getsignsigraw() const {return sigmaraws[idsoft2linear[_addr]+_strip]>0?1:0;}
inline geant getgain() const {return gains[idsoft2linear[_addr]+_strip];}
inline geant getindnoise() const{return indnoise[idsoft2linear[_addr]+_strip];}
integer getprob(geant r);
virtual  geant getcmnnoise() const {return cmnnoise[_VANumber][_addr];}
void  setrhomatrix(integer bit);
void  clearrhomatrix(integer bit);
integer  getrhomatrix(integer bit);
geant & setped()  {return peds[idsoft2linear[_addr]+_strip];}
geant & setsig()  {return sigmas[idsoft2linear[_addr]+_strip];}
geant & setsigraw()  {return sigmaraws[idsoft2linear[_addr]+_strip];}
geant & setgain() {return gains[idsoft2linear[_addr]+_strip];}
geant & setindnoise() {return indnoise[idsoft2linear[_addr]+_strip];}
geant & setcmnnoise()  {return cmnnoise[_VANumber][_addr];}
void  setstatus(integer changer)  
{status[idsoft2linear[_addr]+_strip]=status[idsoft2linear[_addr]+_strip] | changer;}
void  clearstatus(integer changer)  
{status[idsoft2linear[_addr]+_strip]=status[idsoft2linear[_addr]+_strip] & ~changer;}
friend class AMSTrIdGeom;
friend class AMSJob;
AMSTrIdSoft(int16u crate, int16u haddr);
AMSTrIdSoft(const AMSTrIdGeom &,integer side);
AMSTrIdSoft():_pid(0),_dead(0){};
AMSTrIdSoft( const AMSTrIdSoft& o):_layer(o._layer),_drp(o._drp),
_half(o._half),_side(o._side),_strip(o._strip),_addr(o._addr),
_VANumber(o._VANumber),_pid(0),_dead(o._dead),_haddr(o._haddr),_crate(o._crate){}
AMSTrIdSoft(integer idsoft);
AMSTrIdSoft(integer layer,integer drp,integer half,integer side,integer strip);
AMSTrIdSoft(int16u crate,int16u tdrs, int16u side, int16u strip);
AMSTrIdSoft & operator = (const AMSTrIdSoft &o);
integer operator == (const AMSTrIdSoft &o);
inline integer cmpt() const {return _layer+10*_drp+1000*_half+2000*_side+10000*_strip;}
inline integer getmaxstrips(){return AMSDBc::NStripsDrp(_layer,_side);}
inline void upd(integer strip){
#ifdef __AMSDEBUG__
       if(_side==1)assert (strip>=0 && strip <getmaxstrips());
       else assert (strip>=-TRCLFFKEY.ThrClNEl[_side]/2 
       && strip <getmaxstrips()+TRCLFFKEY.ThrClNEl[_side]/2);
#endif
if(strip >=0 && strip < getmaxstrips())_strip=strip;
else if(strip < 0)_strip=getmaxstrips()+strip;
else              _strip=strip-getmaxstrips();
_VANumber=_strip/_VAChannels;
_mkhaddr();
}
inline integer getlayer() const {return _layer;}
inline integer getdrp() const {return _drp;}
inline integer getside()const {return _side;}        
inline integer gethalf()const {return _half;}        
inline integer getstrip()const {return _strip;}
inline integer getstripa()const {return _strip%_VAChannels;}
inline integer getva()const {return _side==0? _VANumber+10:_VANumber;}
AMSTrIdGeom *  getidgeom()     {return _pid;}
AMSTrIdGeom * ambig(const AMSTrIdSoft &, integer & nambig) ;
                     
static integer debug;
};
class AMSTrIdSoftI{
public:
AMSTrIdSoftI();
private:
static integer _Count;
};
static AMSTrIdSoftI rI;






#endif

