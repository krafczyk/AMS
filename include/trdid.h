//  $Id: trdid.h,v 1.2 2001/01/22 17:32:45 choutko Exp $
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
};
#endif

