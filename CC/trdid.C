//  $Id: trdid.C,v 1.7 2001/11/19 14:39:22 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include <trdid.h>
#include <cern.h>
#include <commons.h>
#include <amsstl.h>
#include <stdio.h>
#include <trddbc.h>
using namespace trdconst;

uinteger AMSTRDIdGeom::cmpt() const{
return maxtube*maxlad*maxlay*_octagon+maxtube*maxlad*_layer+maxtube*_ladder+_tube+1;
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
/*
_octagon=(idsoft-1)%mtrdo;
_layer=((idsoft-1)/mtrdo)%maxlay;
_ladder=((idsoft-1)/mtrdo/maxlay)%maxlad;
_tube=((idsoft-1)/mtrdo/maxlay/maxlad)%maxtube;
*/
_tube=(idsoft-1)%maxtube;
_ladder=((idsoft-1)/maxtube)%maxlad;
_layer=((idsoft-1)/maxtube/maxlad)%maxlay;
_octagon=((idsoft-1)/maxtube/maxlad/maxlay)%mtrdo;

#ifdef __AMSDEBUG__
_check();
#endif
}


AMSTRDIdGeom::AMSTRDIdGeom(const AMSTRDIdSoft & s){
_octagon=0;
_layer=s._layer;
_ladder=s._ladder;
_tube=s._tube;
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTRDIdSoft::AMSTRDIdSoft(const AMSTRDIdGeom & s){
_layer=s._layer;
_ladder=s._ladder;
_tube=s._tube;
_address=cmpta();
#ifdef __AMSDEBUG__
_check();
#endif
}

uinteger AMSTRDIdSoft::gethaddr() const{
 return cmpt();   // to be defined !!!
}

uinteger AMSTRDIdSoft::cmpt() const{
return maxtube*maxlad*_layer+maxtube*_ladder+_tube+1;
}

uinteger AMSTRDIdSoft::cmpta() const{
return maxtube*maxlad*_layer+maxtube*_ladder;
}


integer AMSTRDIdSoft::_NROCh=0;

void AMSTRDIdSoft::init(){
for ( int i=0;i<TRDDBc::TRDOctagonNo();i++){
      for (int j=0;j<TRDDBc::LayersNo(i);j++){
	  for(int k=0;k<TRDDBc::LaddersNo(i,j);k++){
           _NROCh+=TRDDBc::TubesNo(i,j,k);
          }
      }
}
_ped=new geant[getpedsize()];
_sig=new geant[getsigsize()];
_gain=new geant[getgaisize()];
_status=new uinteger[getstasize()];
cout <<"AMSTRDIdSoft::init()-I-"<<_NROCh<<"/"<<getpedsize()<<" readout channels initialized"<<endl;
}



void AMSTRDIdSoft::_check(){
  int _octagon=0;
  if( _layer>TRDDBc::LayersNo(_octagon)-1)
  cerr <<"AMSTRDIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder>TRDDBc::LaddersNo(_octagon,_layer)-1)
  cerr <<"AMSTRDIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_tube>TRDDBc::TubesNo(_octagon,_layer,_ladder)-1)
  cerr <<"AMSTRDIdGeom-ctor-tube out of bounds "<<_tube<<endl; 
}


geant * AMSTRDIdSoft::_gain=0;
geant * AMSTRDIdSoft::_ped=0;
uinteger * AMSTRDIdSoft::_status=0;
geant * AMSTRDIdSoft::_sig=0;


uinteger AMSTRDIdSoft::CalcBadCh(uinteger crateno){
uinteger badch=0;
uinteger zerog=0;
for (int i=0;i<TRDDBc::LayersNo(0);i++){
  for(int j=0;j<TRDDBc::LaddersNo(0,i);j++){
    for(int k=0;k<TRDDBc::TubesNo(0,i,j);k++){
    AMSTRDIdSoft id(i,j,k);
    if(id.getcrate()==crateno && id.checkstatus(AMSDBc::BAD))badch++;
    if(id.getcrate()==crateno && id.getgain()==0)zerog++;
     }
  }
}
if(zerog)cerr<<" Some gains are zeros!!! "<<zerog<<endl;
return badch;
}