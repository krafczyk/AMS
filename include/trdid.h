//  $Id: trdid.h,v 1.3 2001/04/27 21:50:33 choutko Exp $
#ifndef __AMSTRDID__
#define __AMSTRDID__
#include <typedefs.h>
#include <amsdbc.h>
#include <iostream.h>
#include <amsstl.h>
#include <commons.h>
#include <link.h>
#include <job.h>
#include <trddbc.h>
using trdconst::maxo;
using trdconst::mtrdo;
using trdconst::maxtube;

namespace trdid{

}
class AMSTRDIdGeom{
uinteger _octagon;  // from 0 to 0l
uinteger _layer;    // from 0 to TRDDBc::LayersNo(_octagon)-1
uinteger _ladder;   // from 0 to TRDDBc::LaddersNo(_octagon,_layer)-1
uinteger _tube;   // from 0 to TRDDBc::TubesNo(_octagon,_layer,_ladder)-1
void _check();
public:
friend ostream &operator << (ostream &o, const  AMSTRDIdGeom &b )
   {return o<<" lay "<<b._layer<<" lad "<<b._ladder<<" tube "<<b._tube<<endl;}
AMSTRDIdGeom():_layer(0),_ladder(0),_tube(0),_octagon(0){};
~AMSTRDIdGeom(){};
AMSTRDIdGeom(integer layer,integer ladder,integer tube, integer octagon=0):
_layer(layer),_ladder(ladder),_tube(tube),_octagon(octagon){
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
void _check();
public:
friend ostream &operator << (ostream &o, const  AMSTRDIdSoft &b )
   {return o<<" lay "<<b._layer<<" lad "<<b._ladder<<" tube "<<b._tube<<endl;}
AMSTRDIdSoft():_layer(0),_ladder(0),_tube(0){};
~AMSTRDIdSoft(){};
static uinteger ncrates() {return 1;}
AMSTRDIdSoft(integer layer,integer ladder,integer tube):
_layer(layer),_ladder(ladder),_tube(tube){
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
inline uinteger getlayer() const {return _layer;}
inline uinteger getladder()const {return _ladder;}
inline uinteger gettube()const {return _tube;}
// Have to be changed in the future
geant getped(){return TRDMCFFKEY.ped;}
geant getsig(){return TRDMCFFKEY.sigma;}
geant getgain(){return TRDMCFFKEY.gain;}
integer checkstatus(integer checker) const 
{return 0 & checker;}
static integer overflow(){return TRDMCFFKEY.adcoverflow;}
uinteger getcrate(){return 0;}
//
static integer _NROCh;
static integer NROCh(){return _NROCh;}
uinteger cmpt() const;
uinteger cmpta() const;
static void init();
friend class AMSTRDIdGeom;
AMSTRDIdSoft(const  AMSTRDIdGeom & o);

};



#endif

