// Author V. Choutko 24-may-1996
 
#ifndef __AMSTRID__
#define __AMSTRID__
#include <typedefs.h>
#include <amsdbc.h>
#include <iostream.h>
#include <amsstl.h>
#include <commons.h>
#include <link.h>
class AMSTrIdGeom{
integer _layer;    // from 1 to 6
integer _ladder;   // from 1 to AMSDBc::nlad(_layer)
integer _sensor;   // from 1 to AMSDBc::nsen(_layer,_ladder)
integer _stripx;   // from 0 to AMSDBc::NStripsSen(_layer,0)-1
integer _stripy;   // from 0 to AMSDBc::NStripsSen(_layer,1)-1
void _check();
public:
friend class AMSTrIdSoft;
AMSTrIdGeom(){};
~AMSTrIdGeom(){};
AMSTrIdGeom( const AMSTrIdGeom& o):_layer(o._layer),_ladder(o._ladder),
_sensor(o._sensor),_stripx(o._stripx),_stripy(o._stripy){}
AMSTrIdGeom(integer layer,integer ladder,integer sensor, integer stripx,
integer stripy);
AMSTrIdGeom(integer idsoft, integer stripx=0, integer stripy=0);
AMSTrIdGeom & operator = (const AMSTrIdGeom &o);
AMSID crgid();
integer operator == (const AMSTrIdGeom &o);
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
inline integer getsensor()const {return _sensor;}        
inline integer cmpt()const {return _layer+10*_ladder+1000*_sensor;}
inline integer getstrip (integer i) const{if(i==0)return _stripx;else return _stripy;} 
inline number getmaxsize(integer i)  {return AMSDBc::ssize_active(_layer-1,i);}
inline integer getmaxstrips(integer i) const
{return AMSDBc::NStripsSen(_layer,i);}
integer size2strip(integer side, number size);


inline number strip2size(integer side){
     #ifdef __AMSDEBUG__
       assert(side>=0 && side<2);
     #endif
     if(side==0)return _swxyl[_layer-1][0][_stripx];
     else       return _swxyl[_layer-1][1][_stripy];
}

static integer debug;
static number *  _swxy[nl][2] ;  // strip size x,y
static number *  _swxyl[nl][2];   // integral of strip size

static void init();
};
const integer ms=4000;
class AMSTrIdSoft{
private:
integer _layer;   // from 1 to 6
integer _drp;     // from 1 to AMSDBc::nlad(_layer)
integer _side;    // 0=x 1=y
integer _half;    // from 0 to 1
integer _strip;   // from 0 to getmaxstrips()-1
integer _addr;    // Calculated from the above
AMSTrIdGeom * _pid;
static integer idsoft2linear[ms];
static integer *status;
static geant *peds;
static geant *gains;
static geant *sigmas;
static geant *cmnnoise;
static geant *indnoise;
static integer _numel;
void _check();
public:
friend ostream &operator << (ostream &o, const  AMSTrIdSoft &b )
   {return o<<" "<<b._layer<<" "<<b._drp<<" "<<b._side<<" "<<b._half<<" "<<b._strip<<" ";}
static void init();
integer operator < (const AMSTrIdSoft & o)const;

inline integer getnchan()const {return _numel;}
inline integer getaddr() const {return _addr;}
inline integer getstatus(integer checker) const 
{return status[idsoft2linear[_addr]+_strip] & checker;}
inline geant getped() const {return peds[idsoft2linear[_addr]+_strip];}
inline geant getsig() const {return sigmas[idsoft2linear[_addr]+_strip];}
inline geant getgain() const {return gains[idsoft2linear[_addr]+_strip];}
inline geant getindnoise() const{return indnoise[idsoft2linear[_addr]+_strip];}
integer getprob(geant r);
inline geant getcmnnoise() const {return cmnnoise[_addr];}
geant & setped()  {return peds[idsoft2linear[_addr]+_strip];}
geant & setsig()  {return sigmas[idsoft2linear[_addr]+_strip];}
geant & setgain() {return gains[idsoft2linear[_addr]+_strip];}
geant & setindnoise() {return indnoise[idsoft2linear[_addr]+_strip];}
geant & setcmnnoise()  {return cmnnoise[_addr];}
void  setstatus(integer changer)  
{status[idsoft2linear[_addr]+_strip]=status[idsoft2linear[_addr]+_strip] | changer;}
friend class AMSTrIdGeom;
friend class AMSJob;
AMSTrIdSoft(const AMSTrIdGeom &,integer side);
AMSTrIdSoft():_pid(0){};
AMSTrIdSoft( const AMSTrIdSoft& o):_layer(o._layer),_drp(o._drp),
_half(o._half),_side(o._side),_strip(o._strip),_addr(o._addr),_pid(0){}
AMSTrIdSoft(integer idsoft);
AMSTrIdSoft(integer layer,integer drp,integer half,integer side,integer strip=0);
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
}
inline integer getlayer() const {return _layer;}
inline integer getdrp() const {return _drp;}
inline integer getside()const {return _side;}        
inline integer gethalf()const {return _half;}        
inline integer getstrip()const {return _strip;}
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

