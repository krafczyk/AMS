// Author V. Choutko 24-may-1996
 
#include <assert.h>
#include <trid.h>
#include <cern.h>
#include <commons.h>
#include <amsstl.h>
#include <stdio.h>
AMSID AMSTrIdGeom::crgid(){
         static char name[5];
         static ostrstream ost(name,sizeof(name));
         ost.seekp(0);
         ost<<"S"<<getlayer()<<2*getladder()+gethalf()<<ends;
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

number AMSTrIdGeom::getcofg(integer side,integer shift, integer & error) const{
if(side !=0)side=1;
integer strip = side==0?_stripx+shift:_stripy+shift;
if(strip < 0 || strip >= getmaxstrips(side)){
 error=1;
 return 0;
}
else {
 error=0;
 return _swxyl[_layer-1][side][strip]+_swxy[_layer-1][side][strip]/2;
}
}


number * AMSTrIdGeom::_swxyl[nl][2];
number * AMSTrIdGeom::_swxy[nl][2];
number * AMSTrIdGeom::_swxyRl[nl][2];
number * AMSTrIdGeom::_swxyR[nl][2];
geant * AMSTrIdSoft::gains=0;
geant * AMSTrIdSoft::peds=0;
integer * AMSTrIdSoft::status=0;
geant * AMSTrIdSoft::sigmas=0;
geant  AMSTrIdSoft:: cmnnoise[10][ms];
geant * AMSTrIdSoft:: indnoise=0;
integer AMSTrIdSoft::_numel=0;
integer AMSTrIdSoft::idsoft2linear[ms];
const integer AMSTrIdSoft::_VAChannels=64;
integer AMSTrIdSoft::getprob (geant r){
integer iad=_strip+AMSbiel(&(indnoise[idsoft2linear[_addr]+_strip]),
_strip>0? r+getindnoise(): r ,getmaxstrips()-_strip );
if(iad<getmaxstrips())_strip=iad;
return iad;
}

void AMSTrIdSoft::_check(){
if(AMSTrIdSoft::debug){
  if(_dead)return;
  if(_layer <1 || _layer>AMSDBc::nlay())
  cerr <<"AMSTrIdSoft-ctor-layer out of bounds "<<_layer<<endl; 
  else if(_drp<1 || _drp>AMSDBc::nlad(_layer))
  cerr <<"AMSTrIdSoft-ctor-drp out of bounds "<<_drp<<endl; 
  else if(_side<0 || _side>1)
  cerr <<"AMSTrIdSoft-ctor-side out of bounds "<<_side<<endl; 
  else if((_strip<0 ) || _strip>=AMSDBc::NStripsDrp(_layer,_side))
  cerr <<"AMSTrIdSoft-ctor-strip out of bounds "<<_strip<<endl; 
  if(_half<0 || _half>1)
  cerr <<"AMSTrIdSoft-ctor-halfladder out of bounds "<<_half<<endl; 
  if(_VANumber <0 || _VANumber >AMSDBc::NStripsDrp(_layer,_side)/_VAChannels-1)  
  cerr <<"AMSTrIdSoft-ctor-VANumber out of bounds "<<_VANumber<<endl; 
}
}
integer AMSTrIdSoft::debug=1;

AMSTrIdSoft::AMSTrIdSoft(integer layer,integer drp,integer half, integer side, integer strip):
_layer(layer),_drp(drp),_half(half),_side(side),_strip(strip),_pid(0),_dead(0){
_addr=_layer+10*_drp+1000*_half+2000*_side;
_VANumber=_strip/_VAChannels;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif
}
AMSTrIdSoft & AMSTrIdSoft::operator = (const AMSTrIdSoft &o){
  if (this != &o){
 _dead=o._dead;
 _layer=o._layer;
 _drp=o._drp;
 _half=o._half;
 _side=o._side;
 _strip=o._strip;
 _addr=o._addr;
 _VANumber=o._VANumber;
 _crate=o._crate;
 _haddr=o._haddr;
 _pid=0;
   }
  return *this;
}
integer AMSTrIdSoft::operator == (const AMSTrIdSoft &o){
  return (_layer==o._layer && _drp==o._drp && _strip==
o._strip && _half == o._half && _side==o._side && _dead==o._dead);
}

AMSTrIdSoft::AMSTrIdSoft(integer idsoft){
 _dead=0;
_layer=idsoft%10;
_drp=(idsoft/10)%100;
_half=(idsoft/1000)%2;
_side=((idsoft/1000)%10)/2;
_strip=(idsoft/10000)%1000;
_addr=_layer+10*_drp+1000*_half+2000*_side;
_VANumber=_strip/_VAChannels;
_pid=0;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif
}

AMSTrIdSoft::AMSTrIdSoft(const AMSTrIdGeom& idg, integer side){
 _dead=0;
 _pid=0;
 _layer=idg._layer;
 _drp=  idg._ladder;
 _side=side;
 if(idg._sensor <= AMSDBc::nhalf(_layer,_drp))_half=0;
    else _half=1;
_addr=_layer+10*_drp+1000*_half+2000*_side;
if(side==1){
 if(idg._stripy==0)_strip=idg._stripy;
 else if(idg._stripy==1 || idg._stripy==AMSDBc::NStripsSen(_layer,1)-2){
  _strip=-1;
  _dead=1;
 }
 else if(idg._stripy==AMSDBc::NStripsSen(_layer,1)-1)
 _strip=AMSDBc::NStripsDrp(_layer,1)-1;
 else _strip=idg._stripy-1;
}
else{
  integer tot=(idg._sensor-1-AMSDBc::nhalf(_layer,_drp)*_half)*
    AMSDBc::NStripsSen(_layer,0)+idg._stripx;
  _strip=tot%AMSDBc::NStripsDrp(_layer,0);
}
_VANumber=_strip/_VAChannels;
_mkcrate();
_mkhaddr();
#ifdef __AMSDEBUG__
_check();
#endif


}

 integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
 integer AMSTrIdSoft::_GetHard[nl][nld][2][3];
AMSTrIdSoft::AMSTrIdSoft(int16u crate, int16u haddr):_crate(crate),
_haddr(haddr),_dead(0),_pid(0){
_VANumber=(_haddr>>6)&15;
_side=_VANumber>9?0:1;
_VANumber=_VANumber%10;
_strip=((_haddr)&63)+_VANumber*64;
integer tdrs=gettdr();
#ifdef __AMSDEBUG__
 assert (tdrs>=0 && tdrs < ntdr) ;
#endif
_layer=_GetGeo[_crate][tdrs][_side][0];
_drp=_GetGeo[_crate][tdrs][_side][1];
_half=_GetGeo[_crate][tdrs][_side][2];
_addr=_layer+10*_drp+1000*_half+2000*_side;
#ifdef __AMSDEBUG__
_check();
#endif

}

AMSTrIdSoft::AMSTrIdSoft(int16u crate, int16u tdrs, int16u side, int16u strip)
:_crate(crate),_dead(0),_strip(strip),_side(side),_pid(0){
#ifdef __AMSDEBUG__
 assert (tdrs>=0 && tdrs < ntdr) ;
#endif
_layer=_GetGeo[_crate][tdrs][_side][0];
_drp=_GetGeo[_crate][tdrs][_side][1];
_half=_GetGeo[_crate][tdrs][_side][2];
_VANumber=_strip/_VAChannels;
if(_half < 0){
  // ugly dead condition
     _dead=1;
}
else {
_addr=_layer+10*_drp+1000*_half+2000*_side;
_mkhaddr();
}
#ifdef __AMSDEBUG__
_check();
#endif

}


void AMSTrIdSoft::_mkcrate(){
_crate=_GetHard[_layer-1][_drp-1][_half][2];
}

void AMSTrIdSoft::_mkhaddr(){
integer va= getva();
integer strip=getstripa();
integer tdrs=_GetHard[_layer-1][_drp-1][_half][_side];
#ifdef __AMSDEBUG__
 assert (tdrs>=0 && tdrs < ntdr);
#endif
  _haddr=strip | va<<6 | tdrs << 10 ;
}

AMSTrIdGeom * AMSTrIdSoft::ambig(const AMSTrIdSoft &o, integer & namb) {
  static AMSTrIdGeom spid[20];
  if( _side==0 && o._side==1) {
    integer strip=_strip;
    integer stripy;
    namb=0; 
    _pid=spid;
   if(_half==0 ){
     integer isen=strip/AMSDBc::NStripsSen(_layer,_side)+1;
     do {
      (_pid+namb)->_layer=_layer;
      (_pid+namb)->_ladder=_drp;
      (_pid+namb)->_sensor=isen;
      if(o._strip==0)stripy=0;
      else if(o._strip==AMSDBc::NStripsDrp(o._layer,o._side)-1)
      stripy=AMSDBc::NStripsSen(o._layer,o._side)-1;
      else stripy=o._strip+1;
      (_pid+namb)->_stripy=stripy;
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
      if(o._strip==0)stripy=0;
      else if(o._strip==AMSDBc::NStripsDrp(o._layer,o._side)-1)
      stripy=AMSDBc::NStripsSen(o._layer,o._side)-1;
      else stripy=o._strip+1;
      (_pid+namb)->_stripy=stripy;
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
     int i,k;
     for(k=0;k<2;k++){
      for ( i=0;i<AMSDBc::nlay();i++){
       int j;
       for ( j=0;j<AMSDBc::nlad(i+1);j++){
           idsoft=AMSTrIdSoft(i+1,j+1,k,0,0).getaddr();
           idsoft2linear[idsoft]=num;
           num=num+AMSDBc::NStripsDrp(i+1,0);
           nc++;
       }
       for (j=0;j<AMSDBc::nlad(i+1);j++){
           idsoft=AMSTrIdSoft(i+1,j+1,k,1,0).getaddr();
           idsoft2linear[idsoft]=num;
           num=num+AMSDBc::NStripsDrp(i+1,1);
           nc++;
       }
      }
     }
     cout <<
     "AMSTrIdSoft::init-I-Total of " <<num<< " channels  and "<<
     nc<<" drps found."<<endl;
     status=new integer[num];
     for(int l=0;l<num;l++)status[l]=AMSDBc::BAD;    // All are bad
     peds=new geant[num];
     gains=new geant[num];
     sigmas=new geant[num];
     for(i=0;i<10;i++){
      for(int k=0;k<ms;k++)cmnnoise[i][k]=0;
     }     
     indnoise=new geant[num];
     _numel=num;
     assert(status && peds && gains && sigmas && indnoise);

     //     integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
     //     integer AMSTrIdSoft::_GetHard[nl][nld][2][3];     
     for(i=0;i<ncrt;i++){
       for(int j=0;j<ntdr;j++){
        for(k=0;k<2;k++){
         for(l=0;l<3;l++)_GetGeo[i][j][k][l]=-1;
        }
       }
     }
     for(i=0;i<nl;i++){
       for(int j=0;j<nld;j++){
         for(k=0;k<2;k++){
          for(int l=0;l<3;l++)_GetHard[i][j][k][l]=-1;
         }
       }
     }
     
}


void AMSTrIdSoft::inittable(){


        //     integer AMSTrIdSoft::_GetHard[nl][nld][2][3];     

  _GetHard[0][6][0][2]=0;            //crate
  _GetHard[0][6][0][0]=7 | 3<<3;     //side x
  _GetHard[0][6][0][1]=3 | 6<<2;     //side y

  _GetHard[0][7][0][2]=0;            //crate
  _GetHard[0][7][0][0]=6 | 3<<3;     //side x
  _GetHard[0][7][0][1]=2 | 6<<2;     //side y

  _GetHard[0][8][0][2]=0;            //crate
  _GetHard[0][8][0][0]=5 | 3<<3;     //side x
  _GetHard[0][8][0][1]=1 | 6<<2;     //side y

  _GetHard[0][9][0][2]=0;            //crate
  _GetHard[0][9][0][0]=4 | 3<<3;     //side x
  _GetHard[0][9][0][1]=0 | 6<<2;     //side y


  _GetHard[0][6][1][2]=1;            //crate
  _GetHard[0][6][1][0]=4 | 3<<3;     //side x
  _GetHard[0][6][1][1]=0 | 6<<2;     //side y


  _GetHard[0][7][1][2]=1;            //crate
  _GetHard[0][7][1][0]=5 | 3<<3;     //side x
  _GetHard[0][7][1][1]=1 | 6<<2;     //side y


  _GetHard[0][8][1][2]=1;            //crate
  _GetHard[0][8][1][0]=6 | 3<<3;     //side x
  _GetHard[0][8][1][1]=2 | 6<<2;     //side y


  _GetHard[0][9][1][2]=1;            //crate
  _GetHard[0][9][1][0]=7 | 3<<3;     //side x
  _GetHard[0][9][1][1]=3 | 6<<2;     //side y

  // Layer 2


  _GetHard[1][4][0][2]=0;            //crate
  _GetHard[1][4][0][0]=1 | 2<<3;     //side x
  _GetHard[1][4][0][1]=1 | 5<<2;     //side y


  _GetHard[1][5][0][2]=0;            //crate
  _GetHard[1][5][0][0]=3 | 2<<3;     //side x
  _GetHard[1][5][0][1]=3 | 5<<2;     //side y


  _GetHard[1][6][0][2]=0;            //crate
  _GetHard[1][6][0][0]=5 | 2<<3;     //side x
  _GetHard[1][6][0][1]=1 | 4<<2;     //side y


  _GetHard[1][7][0][2]=0;            //crate
  _GetHard[1][7][0][0]=7 | 2<<3;     //side x
  _GetHard[1][7][0][1]=3 | 4<<2;     //side y


  _GetHard[1][8][0][2]=0;            //crate
  _GetHard[1][8][0][0]=1 | 3<<3;     //side x
  _GetHard[1][8][0][1]=1 | 7<<2;     //side y


  _GetHard[1][9][0][2]=0;            //crate
  _GetHard[1][9][0][0]=3 | 3<<3;     //side x
  _GetHard[1][9][0][1]=3 | 7<<2;     //side y

  _GetHard[1][4][1][2]=1;            //crate
  _GetHard[1][4][1][0]=3 | 3<<3;     //side x
  _GetHard[1][4][1][1]=3 | 7<<2;     //side y

  _GetHard[1][5][1][2]=1;            //crate
  _GetHard[1][5][1][0]=1 | 3<<3;     //side x
  _GetHard[1][5][1][1]=1 | 7<<2;     //side y

  _GetHard[1][6][1][2]=1;            //crate
  _GetHard[1][6][1][0]=7 | 2<<3;     //side x
  _GetHard[1][6][1][1]=3 | 4<<2;     //side y

  _GetHard[1][7][1][2]=1;            //crate
  _GetHard[1][7][1][0]=5 | 2<<3;     //side x
  _GetHard[1][7][1][1]=1 | 4<<2;     //side y

  _GetHard[1][8][1][2]=1;            //crate
  _GetHard[1][8][1][0]=3 | 2<<3;     //side x
  _GetHard[1][8][1][1]=3 | 5<<2;     //side y

  _GetHard[1][9][1][2]=1;            //crate
  _GetHard[1][9][1][0]=1 | 2<<3;     //side x
  _GetHard[1][9][1][1]=1 | 5<<2;     //side y

  // layer 3

  _GetHard[2][5][0][2]=0;            //crate
  _GetHard[2][5][0][0]=2 | 2<<3;     //side x
  _GetHard[2][5][0][1]=2 | 5<<2;     //side y

  _GetHard[2][6][0][2]=0;            //crate
  _GetHard[2][6][0][0]=4 | 2<<3;     //side x
  _GetHard[2][6][0][1]=0 | 5<<2;     //side y

  _GetHard[2][7][0][2]=0;            //crate
  _GetHard[2][7][0][0]=6 | 2<<3;     //side x
  _GetHard[2][7][0][1]=2 | 4<<2;     //side y

  _GetHard[2][8][0][2]=0;            //crate
  _GetHard[2][8][0][0]=0 | 3<<3;     //side x
  _GetHard[2][8][0][1]=0 | 7<<2;     //side y

  _GetHard[2][5][1][2]=1;            //crate
  _GetHard[2][5][1][0]=0 | 3<<3;     //side x
  _GetHard[2][5][1][1]=0 | 7<<2;     //side y

  _GetHard[2][6][1][2]=1;            //crate
  _GetHard[2][6][1][0]=6 | 2<<3;     //side x
  _GetHard[2][6][1][1]=2 | 4<<2;     //side y

  _GetHard[2][7][1][2]=1;            //crate
  _GetHard[2][7][1][0]=4 | 2<<3;     //side x
  _GetHard[2][7][1][1]=0 | 4<<2;     //side y

  _GetHard[2][8][1][2]=1;            //crate
  _GetHard[2][8][1][0]=2 | 2<<3;     //side x
  _GetHard[2][8][1][1]=2 | 5<<2;     //side y

  _GetHard[2][9][1][2]=1;            //crate
  _GetHard[2][9][1][0]=0 | 2<<3;     //side x
  _GetHard[2][9][1][1]=0 | 5<<2;     //side y

  // layer 4


  _GetHard[3][5][0][2]=0;            //crate
  _GetHard[3][5][0][0]=5 | 1<<3;     //side x
  _GetHard[3][5][0][1]=1 | 2<<2;     //side y

  _GetHard[3][6][0][2]=0;            //crate
  _GetHard[3][6][0][0]=3 | 1<<3;     //side x
  _GetHard[3][6][0][1]=3 | 3<<2;     //side y

  _GetHard[3][7][0][2]=0;            //crate
  _GetHard[3][7][0][0]=1 | 1<<3;     //side x
  _GetHard[3][7][0][1]=1 | 3<<2;     //side y

  _GetHard[3][8][0][2]=0;            //crate
  _GetHard[3][8][0][0]=7 | 0<<3;     //side x
  _GetHard[3][8][0][1]=3 | 0<<2;     //side y

  _GetHard[3][4][1][2]=1;            //crate
  _GetHard[3][4][1][0]=5 | 0<<3;     //side x
  _GetHard[3][4][1][1]=1 | 0<<2;     //side y

  _GetHard[3][5][1][2]=1;            //crate
  _GetHard[3][5][1][0]=7 | 0<<3;     //side x
  _GetHard[3][5][1][1]=3 | 0<<2;     //side y

  _GetHard[3][6][1][2]=1;            //crate
  _GetHard[3][6][1][0]=1 | 1<<3;     //side x
  _GetHard[3][6][1][1]=1 | 3<<2;     //side y

  _GetHard[3][7][1][2]=1;            //crate
  _GetHard[3][7][1][0]=3 | 1<<3;     //side x
  _GetHard[3][7][1][1]=3 | 3<<2;     //side y

  _GetHard[3][8][1][2]=1;            //crate
  _GetHard[3][8][1][0]=5 | 1<<3;     //side x
  _GetHard[3][8][1][1]=1 | 2<<2;     //side y

  _GetHard[3][9][1][2]=1;            //crate
  _GetHard[3][9][1][0]=7 | 1<<3;     //side x
  _GetHard[3][9][1][1]=3 | 2<<2;     //side y

  // layer 5

  _GetHard[4][4][0][2]=0;            //crate
  _GetHard[4][4][0][0]=6 | 1<<3;     //side x
  _GetHard[4][4][0][1]=2 | 2<<2;     //side y

  _GetHard[4][5][0][2]=0;            //crate
  _GetHard[4][5][0][0]=4 | 1<<3;     //side x
  _GetHard[4][5][0][1]=0 | 2<<2;     //side y

  _GetHard[4][6][0][2]=0;            //crate
  _GetHard[4][6][0][0]=2 | 1<<3;     //side x
  _GetHard[4][6][0][1]=2 | 3<<2;     //side y

  _GetHard[4][7][0][2]=0;            //crate
  _GetHard[4][7][0][0]=0 | 1<<3;     //side x
  _GetHard[4][7][0][1]=0 | 3<<2;     //side y

  _GetHard[4][8][0][2]=0;            //crate
  _GetHard[4][8][0][0]=6 | 0<<3;     //side x
  _GetHard[4][8][0][1]=2 | 0<<2;     //side y

  _GetHard[4][9][0][2]=0;            //crate
  _GetHard[4][9][0][0]=4 | 0<<3;     //side x
  _GetHard[4][9][0][1]=0 | 0<<2;     //side y

  _GetHard[4][4][1][2]=1;            //crate
  _GetHard[4][4][1][0]=4 | 0<<3;     //side x
  _GetHard[4][4][1][1]=0 | 0<<2;     //side y

  _GetHard[4][5][1][2]=1;            //crate
  _GetHard[4][5][1][0]=6 | 0<<3;     //side x
  _GetHard[4][5][1][1]=2 | 0<<2;     //side y

  _GetHard[4][6][1][2]=1;            //crate
  _GetHard[4][6][1][0]=0 | 1<<3;     //side x
  _GetHard[4][6][1][1]=0 | 3<<2;     //side y

  _GetHard[4][7][1][2]=1;            //crate
  _GetHard[4][7][1][0]=2 | 1<<3;     //side x
  _GetHard[4][7][1][1]=2 | 3<<2;     //side y

  _GetHard[4][8][1][2]=1;            //crate
  _GetHard[4][8][1][0]=4 | 1<<3;     //side x
  _GetHard[4][8][1][1]=0 | 2<<2;     //side y

  _GetHard[4][9][1][2]=1;            //crate
  _GetHard[4][9][1][0]=6 | 1<<3;     //side x
  _GetHard[4][9][1][1]=2 | 2<<2;     //side y

  // layer 6

  _GetHard[5][7][0][2]=0;            //crate
  _GetHard[5][7][0][0]=1 | 0<<3;     //side x
  _GetHard[5][7][0][1]=1 | 1<<2;     //side y

  _GetHard[5][8][0][2]=0;            //crate
  _GetHard[5][8][0][0]=2 | 0<<3;     //side x
  _GetHard[5][8][0][1]=2 | 1<<2;     //side y

  _GetHard[5][9][0][2]=0;            //crate
  _GetHard[5][9][0][0]=3 | 0<<3;     //side x
  _GetHard[5][9][0][1]=3 | 1<<2;     //side y

  _GetHard[5][7][1][2]=1;            //crate
  _GetHard[5][7][1][0]=2 | 0<<3;     //side x
  _GetHard[5][7][1][1]=2 | 1<<2;     //side y

  _GetHard[5][8][1][2]=1;            //crate
  _GetHard[5][8][1][0]=1 | 0<<3;     //side x
  _GetHard[5][8][1][1]=1 | 1<<2;     //side y

  _GetHard[5][9][1][2]=1;            //crate
  _GetHard[5][9][1][0]=0 | 0<<3;     //side x
  _GetHard[5][9][1][1]=0 | 1<<2;     //side y

  int i,j,k,l,i1,j1,k1,l1;
#ifdef __AMSDEBUG__
  // perform duplicate check
  for(k=0;k<2;k++){
   for(l=0;l<2;l++){
     int tot=0;
     for(i=0;i<nl;i++){
      for(j=0;j<nld;j++){
          int a=_GetHard[i][j][k][l];
          if(a>=0){
            tot++;
            for(i1=0;i1<nl;i1++){
             for(j1=0;j1<nld;j1++){
               if(j1==j && i1==i)continue;
               if(a == _GetHard[i1][j1][k][l]){
                 cerr <<"AMSTrIdSoft::inittable-F-duplicate entry found "<<
                   a<<" i "<<i<<" j "<<j<<" k "<<k<<" l "<<l<<" i1 "<<i1<<
                   " j1 "<<j1<<endl;
                 exit(1);
               }
             }
            }            
          }
          
        }
     }
     cout <<" k "<<k<<" l "<<l<<" total "<<tot<<endl; 
   }
  }
#endif


        //     integer AMSTrIdSoft::_GetGeo[ncrt][ntdr][2][3];
  for(k=0;k<2;k++){            
   for(i=0;i<ncrt;i++){
    for(j=0;j<ntdr;j++){
      for(i1=0;i1<nl;i1++){
        for(j1=0;j1<nld;j1++){
          for(k1=0;k1<2;k1++){
            if(_GetHard[i1][j1][k1][2]==i && _GetHard[i1][j1][k1][k]==j){
             _GetGeo[i][j][k][0]=i1+1;
             _GetGeo[i][j][k][1]=j1+1;
             _GetGeo[i][j][k][2]=k1;
            }
          }
                                      
        }
      }      
    }
   }
  }

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


    for (i=0;i<AMSDBc::nlay();i++){
      for(j=0;j<2;j++){
        _swxyR[i][j]=new number[AMSDBc::NStripsSenR(i+1,j)];
        if(j==0){
           for(k=0;k<AMSDBc::NStripsSenR(i+1,j);k++)_swxyR[i][j][k]=0.0052;
        }
        else{
           for(k=0;k< AMSDBc::NStripsSenR(i+1,j);k++)_swxyR[i][j][k]=0.0055;

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
         cerr <<"AMSTrIdGeom::init-F-SizeDoesNotMatch "<<i<<" "<<j<<" "<<
         _swxyl[i][j][AMSDBc::NStripsSen(i+1,j)]<<" "<<
         AMSDBc::ssize_active(i,j)<<endl;
         exit(1);
       }
      }
     }
     for(i=0;i<AMSDBc::nlay();i++){
      for(j=0;j<2;j++){
        _swxyRl[i][j]=new number[AMSDBc::NStripsSenR(i+1,j)+1];
        _swxyRl[i][j][0]=0;
       for(k=1;k<AMSDBc::NStripsSenR(i+1,j)+1;k++){
        _swxyRl[i][j][k]=_swxyRl[i][j][k-1]+_swxyR[i][j][k-1];
       }
       if(fabs(_swxyRl[i][j][AMSDBc::NStripsSenR(i+1,j)] -
          AMSDBc::ssize_active(i,j)) > 1.e-4){
         cerr <<"AMSTrIdGeom::init-F-SizeDoesNotMatch (R) "<<i<<" "<<j<<" "<<
         _swxyRl[i][j][AMSDBc::NStripsSenR(i+1,j)]<<" "<<
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
       if(sizeof(int) <= sizeof(short int)){
         cerr<<"AMSTrIdSoftI-F-16 bit machine is not supported."<<endl;
         exit(1);
       }
       integer b64=0;
       if(sizeof(ulong)>sizeof(uinteger))b64=1;
       uinteger test1,test2;
       test1=1;
       test1+=2<<8;
       test1+=4<<16;
       test1+=8<<24;
       unsigned char *pt= (unsigned char*)&test1;
       test2=pt[0];
       test2+=pt[1]<<8;
       test2+=pt[2]<<16;
       test2+=pt[3]<<24;
       integer lend = test1==test2;
       if(lend)cout <<"AMSTrIdSoftI-I-Identified as LittleEndian";
       else {
         cout <<"AMSTrIdSoftI-I-Identified as BigEndian";
         AMSDBc::BigEndian=1;
       }
       if(b64)cout <<" 64 bit machine."<<endl;
       else cout <<" 32 bit machine."<<endl;
  }
}
integer AMSTrIdSoftI::_Count=0;

