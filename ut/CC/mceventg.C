// Author V. Choutko 24-may-1996
 
#include <mceventg.h>
#include <math.h>
#include <amsdbc.h>
#include <event.h>
#include <commons.h>
#include <ntuple.h>

extern "C" void spectra_();
AMSPoint AMSmceventg::_coorange[2];
AMSPoint AMSmceventg::_dirrange[2];
number   AMSmceventg::_momrange[2];
integer  AMSmceventg::_fixedpoint;
integer  AMSmceventg::_fixedplane;
integer  AMSmceventg::_fixeddir;
integer  AMSmceventg::_fixedmom;
number   AMSmceventg::_albedorate;
number   AMSmceventg::_albedocz; 
number   AMSmceventg::_planesw[6];

void AMSmceventg::gener(){
static integer ini=0;
integer curp=0;
 number phi;
 number theta;
if(_fixedmom)_mom=_momrange[0];
else {
 integer id;
 if(_ipart%100==67)id=20006;  
 else if(_ipart%100==65 || _ipart%100==66)id=20005;
 else if(_ipart%100==63 || _ipart%100==64)id=20004;
 else if(_ipart%100==61 || _ipart%100==62)id=20003;
 else if(_ipart%100==47)id=20002;
 else if(_ipart==3 && CCFFKEY.low==1)id=30001;   // special low energy electron background !!!!!!!
 else id=20001;
 if(ini==0){
   ini=1;
   spectra_();
   HPRINT(id);
 }
 number ee=HRNDM1(id);
 ee=pow(10.,ee);
 ee=ee/1000.;
  // _mom ; Not so evident ...
 if(_mass< 0.9)_mom=sqrt(ee*(ee+2*_mass));
 else _mom=sqrt(ee*(ee+2*0.938))*_mass/0.938;
 // special low energy electron background !!!!
 if(_ipart==3 && CCFFKEY.low==1)_mom=HRNDM1(id)/1000.;
}

if(_fixeddir){
 _dir=AMSDir(_dirrange[0]);
 if(_fixedpoint){
  _coo=_coorange[0];
 }
 else {
  number lx=_coorange[1][0]-_coorange[0][0];
  number ly=_coorange[1][1]-_coorange[0][1];
  number lz=_coorange[1][2]-_coorange[0][2];
  number x=_coorange[0][0];
  number y=_coorange[0][1];
  number z=_coorange[0][2];
  geant d;
  _coo=AMSPoint(x+lx*RNDM(d),y+ly*RNDM(d),z+lz*RNDM(d));
 }
}
else {
 geant d;
 phi=2*AMSDBc::pi*RNDM(d);
 theta=sqrt((double)RNDM(d));
 theta=acos(theta);
 if(_fixedpoint){
  _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
  _coo=_coorange[0];
  return;
 }

 if(_fixedplane)curp=_fixedplane;
 else {
  geant r=RNDM(d)-1.e-5;
  int i;
  for ( i=0;i<6;i++){
  if(r<_planesw[i]){
   curp=i+1;
   break;
  }
  }
 } 
 switch(curp){
 case 1:
  _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
  _coo=AMSPoint(AMSDBc::ams_size[0]*(RNDM(d)-0.5),AMSDBc::ams_size[1]*(RNDM(d)-0.5),
  AMSDBc::ams_size[2]/2);
  break;
 case 2:  
  _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
  _coo=AMSPoint(AMSDBc::ams_size[0]*(RNDM(d)-0.5),AMSDBc::ams_size[1]*(RNDM(d)-0.5),
  -AMSDBc::ams_size[2]/2);
  break;
 case 3:  
  _dir=AMSDir(-cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(AMSDBc::ams_size[0]/2,AMSDBc::ams_size[1]*(RNDM(d)-0.5),
  AMSDBc::ams_size[2]*(RNDM(d)-0.5));
  break;
 case 4:  
  _dir=AMSDir(cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(-AMSDBc::ams_size[0]/2,AMSDBc::ams_size[1]*(RNDM(d)-0.5),
  AMSDBc::ams_size[2]*(RNDM(d)-0.5));
  break;
 case 5:  
  _dir=AMSDir(cos(phi)*sin(theta),-cos(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(AMSDBc::ams_size[0]*(RNDM(d)-0.5),AMSDBc::ams_size[1]/2,
  AMSDBc::ams_size[2]*(RNDM(d)-0.5));
  break;
 case 6:  
  _dir=AMSDir(cos(phi)*sin(theta),cos(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(AMSDBc::ams_size[0]*(RNDM(d)-0.5),-AMSDBc::ams_size[1]/2,
  AMSDBc::ams_size[2]*(RNDM(d)-0.5));
  break;
 default:  
  cerr <<" AMSmceventg-F-plane problem "<<curp<<endl;
  exit(1);
 }
}
}


void AMSmceventg::setcuts(geant coo[6], geant dir[6],
   geant momr[2],integer fxp=0,geant albedor=0.1 ,geant albedocz=0.05){
    _albedorate=albedor;
    _albedocz=albedocz;
    _coorange[0]=AMSPoint(coo[0],coo[1],coo[2]);
    _coorange[1]=AMSPoint(coo[3],coo[4],coo[5]);
    _dirrange[0]=AMSPoint(dir[0],dir[1],dir[2]);
    _dirrange[1]=AMSPoint(dir[3],dir[4],dir[5]);
    _fixedplane=fxp;
    _momrange[0]=momr[0];
    _momrange[1]=momr[1];
    _fixedmom=0;
    _fixedpoint=0;
    _fixeddir=0;
    if(momr[0]>=momr[1])_fixedmom=1;
    if(coo[0]>=coo[3] && coo[1]>=coo[4] && coo[2]>=coo[5])_fixedpoint=1;
    if(dir[0]>=dir[3] && dir[1]>=dir[4] && dir[2]>=dir[5])_fixeddir=1;
    number area[6];
    area[0]=AMSDBc::ams_size[0]*AMSDBc::ams_size[1];
    area[1]=AMSDBc::ams_size[0]*AMSDBc::ams_size[1];
    area[2]=AMSDBc::ams_size[1]*AMSDBc::ams_size[2];
    area[3]=AMSDBc::ams_size[1]*AMSDBc::ams_size[2];
    area[4]=AMSDBc::ams_size[0]*AMSDBc::ams_size[2];
    area[5]=AMSDBc::ams_size[0]*AMSDBc::ams_size[2];
    int i;
    for ( i=1;i<6;i++)area[i]=area[i]+area[i-1];
    for ( i=0;i<6;i++)_planesw[i]=area[i]/area[5];
}

integer AMSmceventg::accept(){
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
        geant d;
        if (_dir[2]<_albedocz || RNDM(d)< _albedorate){
          return 1;
        }
      }
    }
  }
  return 0;
}
  void AMSmceventg::init(integer ipart){
_ipart=ipart;
char chn[20];
integer itr=0;
geant amass=0;
geant charge=0;
geant tlife=0;
geant ubb[1];
integer one=1;
GFPART(_ipart,chn,itr,amass,charge,tlife,ubb,one);
_mass=amass;
_charge=charge;
  }

  void AMSmceventg::run(integer ipart){
   init(ipart);
   geant plab[3],vertex[3];
   integer nvert=0;
   integer nt=0;
    do{
      gener();
    }while(!accept());
   vertex[0]=_coo[0];
   vertex[1]=_coo[1];
   vertex[2]=_coo[2];
   plab[0]=_mom*_dir[0];
   plab[1]=_mom*_dir[1];
   plab[2]=_mom*_dir[2];
   GSVERT(vertex,0,0,0,0,nvert);
   GSKINE(plab,_ipart,nvert,0,0,nt);
   
  }

  void AMSmceventg::run(){
   geant plab[3],vertex[3];
   integer nvert=0;
   integer nt=0;
    AMSPoint coo(_coo);
    AMSDir dir(_dir);
    number mom(_mom);
    do{
      gener();
    }while(!accept());
    //
    // RNDM synchronization
    //

   _coo=coo;
   _dir=dir;
   _mom=mom;
   vertex[0]=_coo[0];
   vertex[1]=_coo[1];
   vertex[2]=_coo[2];
   plab[0]=_mom*_dir[0];
   plab[1]=_mom*_dir[1];
   plab[2]=_mom*_dir[2];
   GSVERT(vertex,0,0,0,0,nvert);
   GSKINE(plab,_ipart,nvert,0,0,nt);
   
  }

void AMSmceventg::InitSeed(){

  //C
  //C             Initialise the random number generator
  //C
  //      IF(NRNDM(2).NE.0)THEN
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),1,'S')
  //      ELSEIF(NRNDM(1).GT.0)THEN
  //         ISEQ=NRNDM(1)
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'Q')
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'S')
  //      ENDIF
  //
  GCFLAG.NRNDM[0]=_seed[0];
  GCFLAG.NRNDM[1]=_seed[1];
  if(_seed[1] !=0)GRNDMQ(_seed[0],_seed[1],1,"S");
  else if(_seed[0] > 0){
   integer ISEQ=_seed[0];
   GRNDMQ(_seed[0],_seed[1],ISEQ,"Q");
   GRNDMQ(_seed[0],_seed[1],ISEQ,"S");
  }
}  

void AMSmceventg::_printEl(ostream &stream){
geant phi=atan2(_dir[1],_dir[0]);
geant theta=acos(_dir[2]);
stream <<" Particle " << _ipart <<" Theta "<<theta<<" Phi "<<phi<<
       " Ridgidity " << _mom/_charge <<endl;
stream <<" Random numbers "<<_seed[0]<<" "<<_seed[1]<<endl;
}

void AMSmceventg::_copyEl(){

}

void AMSmceventg::_writeEl(){
  // fill the ntuple
static integer init=0;
static MCEventGNtuple GN;
int i;

if(init++==0){
  // get memory
  //book the ntuple block
  HBNAME(IOPA.ntuple,"MCEventG",GN.getaddress(),
  "EventNoMCEventG:I*4, Particle:I*4,  Coo(3):R*4, Dir(3):R*4, Momentum:R*4, Mass:R*4, Charge:R*4");

}
  GN.Event()=AMSEvent::gethead()->getid();
  GN.Particle=_ipart;
  for(i=0;i<3;i++)GN.Coo[i]=_coo[i];
  for(i=0;i<3;i++)GN.Dir[i]=_dir[i];
  GN.Momentum=_mom;
  GN.Mass=_mass;
  GN.Charge=_charge;
  HFNTB(IOPA.ntuple,"MCEventG");

}


