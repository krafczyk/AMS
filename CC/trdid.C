//  $Id: trdid.C,v 1.3 2001/01/22 17:32:23 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include <trdid.h>
#include <cern.h>
#include <commons.h>
#include <amsstl.h>
#include <stdio.h>
using namespace trdconst;

uinteger AMSTRDIdGeom::cmpt() const{
return _octagon+mtrdo*_layer+mtrdo*maxlay*_ladder+mtrdo*maxlay*maxlad*_tube+1;
}
AMSID AMSTRDIdGeom::crgid() const{
         static char name[5]="TRDT";
         return AMSID(name,cmpt());
}
void AMSTRDIdGeom::_check(){
  if(_octagon>0)
  cerr <<"AMSTRDIdGeom-ctor-octagon out of bounds "<<_octagon<<endl; 
  else if( _layer>TRDDBc::LayersNo(_octagon)-1)
  cerr <<"AMSTRDIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder>TRDDBc::LaddersNo(_octagon,_layer)-1)
  cerr <<"AMSTRDIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_tube>TRDDBc::TubesNo(_octagon,_layer,_ladder)-1)
  cerr <<"AMSTRDIdGeom-ctor-tube out of bounds "<<_tube<<endl; 
}

integer AMSTRDIdGeom::operator == (const AMSTRDIdGeom &o){
  return (_layer==o._layer && _ladder==o._ladder && _tube==o._tube && _octagon==o._octagon);
}

AMSTRDIdGeom::AMSTRDIdGeom(integer idsoft){
_octagon=(idsoft-1)%mtrdo;
_layer=((idsoft-1)/mtrdo)%maxlay;
_ladder=((idsoft-1)/mtrdo/maxlay)%maxlad;
_tube=((idsoft-1)/mtrdo/maxlay/maxlad)%maxtube;
#ifdef __AMSDEBUG__
_check();
#endif
}
