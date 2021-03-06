//  $Id$
#ifndef __AMSTRDID__
#define __AMSTRDID__
#include "typedefs.h"
#include "amsdbc.h"
#include "amsstl.h"
#include "commons.h"
#include "link.h"
#include "job.h"
#include "trddbc.h"
using trdconst::maxo;
using trdconst::mtrdo;
using trdconst::maxtube;

namespace trdid{
  const uinteger ncrt=2;
  const uinteger nudr=6;
  const uinteger nufe=7;
  const uinteger nute=4;
}
class AMSTRDIdSoft;
class AMSTRDIdGeom{
uinteger _octagon;  // from 0 to 0l
uinteger _layer;    // from 0 to TRDDBc::LayersNo(_octagon)-1
uinteger _ladder;   // from 0 to TRDDBc::LaddersNo(_octagon,_layer)-1
uinteger _tube;   // from 0 to TRDDBc::TubesNo(_octagon,_layer,_ladder)-1
void _check();

public:
friend ostream &operator << (ostream &o, const  AMSTRDIdGeom &b )
   {return o<<" lay "<<b._layer<<" lad "<<b._ladder<<" tube "<<b._tube<<endl;}
AMSTRDIdGeom():_octagon(0),_layer(0),_ladder(0),_tube(0){};
~AMSTRDIdGeom(){};
AMSTRDIdGeom(integer layer,integer ladder,integer tube, integer octagon=0):
_octagon(octagon),_layer(layer),_ladder(ladder),_tube(tube){
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTRDIdGeom(integer idsoft);    // from geant to class
AMSID crgid() const;   // based on cmpt
integer operator == (const AMSTRDIdGeom &o);
inline uinteger getlayer() const {return _layer;}
inline uinteger getladder()const {return _ladder;}
inline uinteger gettube()const {return _tube;}
uinteger cmpt() const;
friend class AMSTRDIdSoft;
AMSTRDIdGeom(const  AMSTRDIdSoft & o);
};



class AMSTRDIdSoft{
uinteger _layer;    // from 0 to TRDDBc::LayersNo(_octagon)-1
uinteger _ladder;   // from 0 to TRDDBc::LaddersNo(_octagon,_layer)-1
uinteger _tube;   // from 0 to TRDDBc::TubesNo(_octagon,_layer,_ladder)-1
uinteger _address; // aux address (== cmpta)
int16u   _haddr;   // hardware address
int16u   _crate;   // crate no
integer  _dead;    //  dead if 1 ; alive otherwise
static uinteger _CrateNo[trdid::ncrt];
void _check();
void _mkhaddr();
void _mkcrate();
 static integer _Calib[trdid::ncrt][trdid::nudr];   
 static integer _GetGeo[trdid::ncrt][trdid::nudr][trdid::nufe][trdid::nute][3];   // crate,nufe,nudr,nute ->     //layer,ladder,direction(0,1) 
 static integer _GetHard[trdconst::maxlay][trdconst::maxlad][4];     // layer,ladder ->// nute,nufe,nudr, crate
static geant *_ped;
static geant *_sig;
static geant *_mcgain;
static uinteger *_status;
static integer _NROCh;
public:
static geant *_gain;
integer getchannel() const {return _address+_tube;}
bool dead()const{return _dead==1;}
friend ostream &operator << (ostream &o, const  AMSTRDIdSoft &b )
   {return o<<" lay "<<b._layer<<" lad "<<b._ladder<<" tube "<<b._tube<<" crate "<<b._crate<<" ute "<<((b._haddr>>4)&3)<<" ufe "<<((b._haddr>>6)&7)<<" udr "<<((b._haddr>>9)&7)<<endl;}
AMSTRDIdSoft():_layer(0),_ladder(0),_tube(0),_address(0){};
~AMSTRDIdSoft(){};

AMSTRDIdSoft(integer layer,integer ladder,integer tube):
_layer(layer),_ladder(ladder),_tube(tube),_dead(0){
_address=cmpta();
_mkhaddr();
_mkcrate();
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTRDIdSoft(uinteger crate, uinteger haddr):_crate(crate){
uinteger ute=(haddr>>4)&3;
uinteger ufe=(haddr>>6)&7;
uinteger udr=(haddr>>9)&7;
if(_GetGeo[crate][udr][ufe][ute][0]<0){
  _dead=1;
}
else{
_dead=0;
_layer=_GetGeo[crate][udr][ufe][ute][0];
_ladder=_GetGeo[crate][udr][ufe][ute][1];
_tube=haddr&15;
if(_GetGeo[crate][udr][ufe][ute][2]!=0)_tube=trdconst::maxtube-1-_tube;
_mkhaddr();
_address=cmpta();
}
}

AMSTRDIdSoft(uinteger crate, uinteger udr, uinteger ufe, uinteger ute, uinteger roch):_tube(roch),_crate(crate){
#ifdef __AMSDEBUG__
assert(crate<ncrates());
assert(udr<trdid::nudr);
assert (ufe<trdid::nufe);
assert (ute<trdid::nute);
assert (roch<trdconst::maxtube);
#endif
if(_GetGeo[crate][udr][ufe][ute][0]<0){
  _dead=1;
}
else{
_dead=0;
_layer=_GetGeo[crate][udr][ufe][ute][0];
_ladder=_GetGeo[crate][udr][ufe][ute][1];
if(_GetGeo[crate][udr][ufe][ute][2]!=0)_tube=trdconst::maxtube-1-_tube;
_mkhaddr();
_address=cmpta();
}
#ifdef __AMSDEBUG__
_check();
#endif
}

integer operator == (const AMSTRDIdSoft &o);
void upd(uinteger tubeno){
#ifdef __AMSDEBUG__
assert(tubeno<maxtube);
#endif
_tube=tubeno;
}

static uinteger ncrates() {return trdid::ncrt;}
uinteger getcrate(){return _crate;}
uinteger getudr()const{return _dead?0:_GetHard[_layer][_ladder][2];}
uinteger getufe()const{return _dead?0:_GetHard[_layer][_ladder][1];}

geant & setped()  {return _ped[_address+_tube];}
geant & setsig()  {return _sig[_address+_tube];}
geant & setgain()  {return _gain[_address+_tube];}
void  setstatus(integer changer)  
{_status[_address+_tube]=_status[_address+_tube] | changer;}
void  clearstatus(integer changer)  
{_status[_address+_tube]=_status[_address+_tube] & ~changer;}
integer checkstatus(integer checker) const 
{return _status[_address+_tube] & checker;}


inline uinteger getlayer() const {return _layer;}
inline uinteger getladder()const {return _ladder;}
inline uinteger gettube()const {return _tube;}

// Have to be changed in the future
geant getped(){return _ped[_address+_tube];}
geant getsig(){return _sig[_address+_tube];}
geant getgain(){return _gain[_address+_tube];}
geant getmcgain(){return _mcgain[_address+_tube];}
static integer overflow(){return TRDMCFFKEY.adcoverflow;}
//
static uinteger NROCh(){return _NROCh;}
static void ResetGains();
uinteger cmpt() const;
uinteger cmpta() const;
uinteger gethaddr() const;   //  hardware address to be defined
static void init();
static void inittable();
friend class AMSTRDIdGeom;
friend class AMSTRDRawHit;
friend class AMSJob;
AMSTRDIdSoft(const  AMSTRDIdGeom & o);
static uinteger CalcBadCh(uinteger crate);
static uinteger getnchan() {return maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;}
static uinteger getpedsize(){return maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;}
static uinteger getsigsize(){return maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;}
static uinteger getstasize(){return maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;}
static uinteger getgaisize(){return maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;}
};



#endif

