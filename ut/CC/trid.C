// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include <trid.h>
#include <cern.h>
#include <commons.h>
#include <amsstl.h>
AMSID AMSTrIdGeom::crgid(){
         static char name[5];
         static ostrstream ost(name,sizeof(name));
         ost.seekp(0);
         ost<<"SEA"<<getlayer();
         return AMSID(name,cmpt());
}
void AMSTrIdGeom::_check(){
if(AMSTrIdGeom::debug){
  if(_layer <1 || _layer>AMSDBc::nlay())
  cerr <<"AMSTrIdGeom-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_ladder<1 || _ladder>AMSDBc::nlad(_layer))
  cerr <<"AMSTrIdGeom-ctor-ladder out of bounds "<<_ladder<<endl; 
  else if(_sensor<1 || _sensor>AMSDBc::nsen(_layer,_ladder))
  cerr <<"AMSTrIdGeom-ctor-sensor out of bounds "<<_sensor<<endl; 
  else if(_stripx<0 || _stripx>AMSDBc::NStripsSen(_layer,0)-1)
  cerr <<"AMSTrIdGeom-ctor-stripx out of bounds "<<_sensor<<endl; 
  else if(_stripy<0 || _stripy>AMSDBc::NStripsSen(_layer,1)-1)
  cerr <<"AMSTrIdGeom-ctor-stripy out of bounds "<<_sensor<<endl; 
}
}
integer AMSTrIdGeom::debug=1;
AMSTrIdGeom::AMSTrIdGeom(integer layer,integer ladder,integer sensor,
                         integer stripx, integer stripy):
_layer(layer),_ladder(ladder),_sensor(sensor),_stripx(stripx),_stripy(stripy){
#ifdef __AMSDEBUG__
_check();
#endif
}
AMSTrIdGeom & AMSTrIdGeom::operator = (const AMSTrIdGeom &o){
  if (this != &o){
 _layer=o._layer;
 _ladder=o._ladder;
 _sensor=o._sensor;
 _stripx=o._stripx;
 _stripy=o._stripy;
   }
  return *this;
}
integer AMSTrIdGeom::operator == (const AMSTrIdGeom &o){
  return (_layer==o._layer && _ladder==o._ladder && _sensor==o._sensor);
}

AMSTrIdGeom::AMSTrIdGeom(integer idsoft, integer stripx, integer stripy){
_layer=idsoft%10;
_ladder=(idsoft/10)%100;
_sensor=(idsoft/1000)%100;
_stripx=stripx;
_stripy=stripy;
#ifdef __AMSDEBUG__
_check();
#endif
}
number AMSTrIdGeom::getsize(integer side) const{
if(side==0)return _swxy[_layer-1][0][_stripx];
else return _swxy[_layer-1][1][_stripy];
}
number * AMSTrIdGeom::_swxyl[nl][2];
number * AMSTrIdGeom::_swxy[nl][2];
geant * AMSTrIdSoft::gains=0;
geant * AMSTrIdSoft::peds=0;
integer * AMSTrIdSoft::status=0;
geant * AMSTrIdSoft::sigmas=0;
geant * AMSTrIdSoft:: cmnnoise=0;
number * AMSTrIdSoft:: indnoise=0;
integer AMSTrIdSoft::_numel=0;
integer AMSTrIdSoft::idsoft2linear[ms];
integer AMSTrIdSoft::getprob (number r){
integer iad=_strip+AMSbiel(&(indnoise[idsoft2linear[_addr]+_strip]),
_strip>0? r+getindnoise(): r ,getmaxstrips()-_strip );
if(iad<getmaxstrips())_strip=iad;
return iad;
}

void AMSTrIdSoft::_check(){
if(AMSTrIdSoft::debug){
  if(_layer <1 || _layer>AMSDBc::nlay())
  cerr <<"AMSTrIdSoft-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_drp<1 || _drp>AMSDBc::nlad(_layer))
  cerr <<"AMSTrIdSoft-ctor-drp out of bounds "<<_drp<<endl; 
  else if(_side<0 || _side>1)
  cerr <<"AMSTrIdSoft-ctor-side out of bounds "<<_side<<endl; 
  else if(_strip<0 || _strip>=AMSDBc::NStripsDrp(_layer,_side))
  cerr <<"AMSTrIdSoft-ctor-strip out of bounds "<<_strip<<endl; 
  if(_half<0 || _half>1)
  cerr <<"AMSTrIdSoft-ctor-halfladder out of bounds "<<_half<<endl; 
  
}
}
integer AMSTrIdSoft::debug=1;

AMSTrIdSoft::AMSTrIdSoft(integer layer,integer drp,integer half, integer side, integer strip):
_layer(layer),_drp(drp),_half(half),_side(side),_strip(strip),_pid(0){
_addr=_layer+10*_drp+1000*_half+2000*_side;
#ifdef __AMSDEBUG__
_check();
#endif
}
AMSTrIdSoft & AMSTrIdSoft::operator = (const AMSTrIdSoft &o){
  if (this != &o){
 _layer=o._layer;
 _drp=o._drp;
 _half=o._half;
 _side=o._side;
 _strip=o._strip;
 _addr=o._addr;
 _pid=0;
   }
  return *this;
}
integer AMSTrIdSoft::operator == (const AMSTrIdSoft &o){
  return (_layer==o._layer && _drp==o._drp && _strip==
o._strip && _half == o._half && _side==o._side);
}

AMSTrIdSoft::AMSTrIdSoft(integer idsoft){
_layer=idsoft%10;
_drp=(idsoft/10)%100;
_half=(idsoft/1000)%2;
_side=((idsoft/1000)%10)/2;
_strip=(idsoft/10000)%1000;
_addr=_layer+10*_drp+1000*_half+2000*_side;
_pid=0;
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTrIdSoft::AMSTrIdSoft(const AMSTrIdGeom& idg, integer side){
 _pid=0;
 _layer=idg._layer;
 _drp=  idg._ladder;
 _side=side;
 if(idg._sensor <= AMSDBc::nhalf(_layer,_drp))_half=0;
    else _half=1;
_addr=_layer+10*_drp+1000*_half+2000*_side;
if(side==1)_strip=idg._stripy;
else{
  integer tot=(idg._sensor-1-AMSDBc::nhalf(_layer,_drp)*_half)*
    AMSDBc::NStripsSen(_layer,0)+idg._stripx;
  _strip=tot%AMSDBc::NStripsDrp(_layer,0);
}
#ifdef __AMSDEBUG__
_check();
#endif


}
AMSTrIdGeom * AMSTrIdSoft::ambig(const AMSTrIdSoft &o, integer & namb) {
  static AMSTrIdGeom spid[20];
  if( _side==0 && o._side==1) {
    integer strip=_strip;
    namb=0; 
    _pid=spid;
   if(_half==0 ){
     integer isen=strip/AMSDBc::NStripsSen(_layer,_side)+1;
     do {
      (_pid+namb)->_layer=_layer;
      (_pid+namb)->_ladder=_drp;
      (_pid+namb)->_sensor=isen;
      (_pid+namb)->_stripy=o._strip;
      (_pid+namb)->_stripx=strip%AMSDBc::NStripsSen(_layer,_side);
      strip=strip+AMSDBc::NStripsDrp(_layer,_side);
      isen=strip/AMSDBc::NStripsSen(_layer,_side)+1;
      namb++;    
#ifdef __AMSDEBUG__
      assert(namb<19);
#endif    
     }while (isen<=AMSDBc::nhalf(_layer,_drp));
    }         
   else{
     integer isen=strip/AMSDBc::NStripsSen(_layer,_side)+1;
     do {
      (_pid+namb)->_layer=_layer;
      (_pid+namb)->_ladder=_drp;
      (_pid+namb)->_sensor=isen+AMSDBc::nhalf(_layer,_drp);
      (_pid+namb)->_stripy=o._strip;
      (_pid+namb)->_stripx=strip%AMSDBc::NStripsSen(_layer,_side);
      strip=strip+AMSDBc::NStripsDrp(_layer,_side);
      isen=strip/AMSDBc::NStripsSen(_layer,_side)+1;
      namb++;
#ifdef __AMSDEBUG__
      assert(namb<19);
#endif    
     }while (isen<= AMSDBc::nsen(_layer,_drp)-AMSDBc::nhalf(_layer,_drp));
    }         
  }
  else{
   namb=0;
   _pid=0;
   if(AMSTrIdSoft::debug)cerr <<"AMSTrIdSoft::ambig-S-Side Error "<<
    _side<<" "<<o._side<<endl;
  } 
return _pid; 
}


integer AMSTrIdSoft::operator < (const AMSTrIdSoft & o)const {
       if(getlayer() < o.getlayer())return 1;
  else if(o.getlayer() < getlayer())return 0;
  else if(getdrp()< o.getdrp())return 1;
  else if(o.getdrp()< getdrp())return 0;
  else if(getside()< o.getside())return 1;
  else if(o.getside()< getside())return 0;
  else if(gethalf()< o.gethalf())return 1;
  else if(o.gethalf()< gethalf())return 0;
  else if(getstrip()< o.getstrip())return 1;
  else return 0; 
}
void AMSTrIdSoft::init(){
     integer num=0;
     integer nc=0;
     integer idsoft;
     VZERO(idsoft2linear,ms*sizeof(integer)/4);
     int i;
     for ( i=0;i<AMSDBc::nlay();i++){
       int j;
       for ( j=0;j<AMSDBc::nlad(i+1);j++){
         int k;
         for ( k=0;k<2;k++){
           idsoft=AMSTrIdSoft(i+1,j+1,k,0).getaddr();
           idsoft2linear[idsoft]=num;
           num=num+AMSDBc::NStripsDrp(i+1,0);
           nc++;
         }
       }
       for (j=0;j<AMSDBc::nlad(i+1);j++){
         int k;
         for ( k=0;k<2;k++){
           idsoft=AMSTrIdSoft(i+1,j+1,k,1).getaddr();
           idsoft2linear[idsoft]=num;
           num=num+AMSDBc::NStripsDrp(i+1,1);
           nc++;
         }
       }
     }
     cout <<
     "AMSTrClusterInitPed-I-Total of " <<num<< " channels  and "<<
     nc<<" drps found."<<endl;
     status=new integer[num];
     peds=new geant[num];
     gains=new geant[num];
     sigmas=new geant[num];
     cmnnoise= new geant[ms];
     indnoise=new number[num];
     _numel=num;
     assert(status && peds && gains && sigmas && cmnnoise && indnoise);



}
void AMSTrIdGeom::init(){
    integer i,j,k;
    for (i=0;i<AMSDBc::nlay();i++){
      for(j=0;j<2;j++){
        _swxy[i][j]=new number[AMSDBc::NStripsSen(i+1,j)];
        if(j==0){
          if(i==0 || i==5){
           for(k=0;k<64;k++)_swxy[i][j][k]=0.0156;
           for(k=64;k<64+96;k++)_swxy[i][j][k]=0.0208;
           for(k=64+96;k<64+96+63;k++)_swxy[i][j][k]=0.0156;
           for(k=64+96+63;k<64+96+64;k++)_swxy[i][j][k]=0.0104;
          }
          else{
           for(k=0;k<191;k++)_swxy[i][j][k]=0.0208;
           for(k=191;k<192;k++)_swxy[i][j][k]=0.0156;
          }
        }
        else{
           for(k=0;k< AMSDBc::NStripsSen(i+1,j);k++)_swxy[i][j][k]=0.011;

        }
      }
    }


     for(i=0;i<AMSDBc::nlay();i++){
      for(j=0;j<2;j++){
        _swxyl[i][j]=new number[AMSDBc::NStripsSen(i+1,j)+1];
        _swxyl[i][j][0]=0;
       for(k=1;k<AMSDBc::NStripsSen(i+1,j)+1;k++){
        _swxyl[i][j][k]=_swxyl[i][j][k-1]+_swxy[i][j][k-1];
       }
       if(fabs(_swxyl[i][j][AMSDBc::NStripsSen(i+1,j)] -
          AMSDBc::ssize_active(i,j)) > 1.e-4){
         cerr <<"AMSTrIdGeom::init-F-SizeDoesNotMatch"<<i<<" "<<j<<" "<<
         _swxyl[i][j][AMSDBc::NStripsSen(i+1,j)]<<" "<<
         AMSDBc::ssize_active(i,j)<<endl;
         exit(1);
       }
      }
     }
     
}

integer AMSTrIdGeom::size2strip(integer side, number size){
     #ifdef __AMSDEBUG__
       assert(side>=0 && side<2);
     #endif
       integer strip;
       strip= AMSbiel( _swxyl[_layer-1][side],  size,  
       AMSDBc::NStripsSen(_layer,side)+1)-1;
     #ifdef __AMSDEBUG__  
       if(strip <0 || strip >= AMSDBc::NStripsSen(_layer,side))
         cerr<<"AMSTrIdGeom::size2strip-E-InvalidStrip "<<strip<<" "<<
           size<<endl;
     #endif       
     if(strip<0)strip=0;
     if(strip>=AMSDBc::NStripsSen(_layer,side))
     strip=AMSDBc::NStripsSen(_layer,side)-1;
     return strip;
}


AMSTrIdSoftI::AMSTrIdSoftI(){
  if(_Count++==0){
       AMSTrIdSoft::init();
       AMSTrIdGeom::init();
  }
}
integer AMSTrIdSoftI::_Count=0;
