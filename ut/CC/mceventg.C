// Author V. Choutko 24-may-1996
 
#include <mceventg.h>
#include <math.h>
#include <amsdbc.h>
#include <event.h>
#include <commons.h>
#include <ntuple.h>
#include <io.h>
#include <extC.h>
orbit AMSmceventg::Orbit;
integer AMSmceventg::_hid=20001;
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

AMSmceventg::AMSmceventg(const AMSIO & io){
_nskip=io._nskip;
_seed[0]=io._seed[0];
_seed[1]=io._seed[1];
init(io._ipart);
_mom=io._mom;
int i;
for(i=0;i<3;i++){
 _coo[i]=io._coo[i];
 _dir[i]=io._dir[i];
}
InitSeed();
}

void AMSmceventg::gener(){
  if(CCFFKEY.low ==2){
    geant mom,themu,phimu,chmu,xmu,ymu,zmu;
    CMGENE(mom,themu,phimu,chmu,xmu,ymu,zmu);
    _mom=mom;
    _coo[0]=xmu;
    _coo[1]=ymu;
    _coo[2]=zmu;
    _dir[0]=sin(themu)*cos(phimu);
    _dir[1]=sin(themu)*sin(phimu);
    _dir[2]=-cos(themu);
    if(chmu < 0)_ipart=6;
    else _ipart=5;
    init(_ipart);
  }
  else {
static integer ini=0;
integer curp=0;
 number phi;
 number theta;
 if(_fixedmom)_mom=_momrange[0];
else {
 _mom=HRNDM1(_hid)/1000.;  // momentum in GeV
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
if(_fixedplane == 0)_coo=_coo/2;
}
  }
}


void AMSmceventg::setspectra(integer begindate, integer begintime, 
                             integer enddate, integer endtime, 
                              integer ipart,  integer low){

 if(low ==2)ipart=5;
 char chp[22]="";
 integer itrtyp;
 geant mass;
 geant charge;
 geant tlife;
 geant ub[1];
 integer nwb=0;
 GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
 charge=fabs(charge);
  const number MIR=51.65;
  Orbit.Begin.tm_year  =  begindate%10000-1900;
  Orbit.Begin.tm_mon = (begindate/10000)%100-1;
  Orbit.Begin.tm_mday   = (begindate/1000000)%100;
  Orbit.Begin.tm_hour  = (begintime/10000)%100;
  Orbit.Begin.tm_min= (begintime/100)%100;
  Orbit.Begin.tm_sec=(begintime)%100;
  Orbit.Begin.tm_isdst =  Orbit.Begin.tm_mon>=3 &&  Orbit.Begin.tm_mon<=8;

  Orbit.End.tm_year  =  enddate%10000-1900;
  Orbit.End.tm_mon = (enddate/10000)%100-1;
  Orbit.End.tm_mday   = (enddate/1000000)%100;
  Orbit.End.tm_hour  = (endtime/10000)%100;
  Orbit.End.tm_min=(endtime/100)%100;
  Orbit.End.tm_sec=(endtime)%100;
  Orbit.End.tm_isdst = Orbit.End.tm_mon>=3 &&  Orbit.End.tm_mon<=8;
  Orbit.FlightTime=difftime(mktime(&Orbit.End),mktime(&Orbit.Begin));
  if(Orbit.FlightTime < 0){
    cerr <<"AMSmceventg::setspectra-F-FlighTime < 0 "<<Orbit.FlightTime<<endl;
    exit(1);
  }  
  Orbit.ThetaI=CCFFKEY.theta/AMSDBc::raddeg;
  Orbit.PhiI=fmod(CCFFKEY.phi/AMSDBc::raddeg+AMSDBc::twopi,AMSDBc::twopi);
  Orbit.AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  number r= tan(Orbit.ThetaI)/Orbit.AlphaTanThetaMax;
  if(r > 1 || r < -1){
    cerr <<"AMSMCEVENTG::setspectra-ThetaI too high "<<Orbit.ThetaI<<endl;
    if(Orbit.ThetaI < 0 )Orbit.ThetaI = -MIR/AMSDBc::raddeg;
    else Orbit.ThetaI= MIR/AMSDBc::raddeg;
    cerr <<"AMSMCEVENTG::setspectra-ThetaI brought to "<<Orbit.ThetaI<<endl;
    r=tan(Orbit.ThetaI)/Orbit.AlphaTanThetaMax;
  }
  Orbit.PhiZero=Orbit.PhiI-atan2(r,CCFFKEY.sdir*sqrt(1-r*r));
  Orbit.Axis[0]=-sin(MIR/AMSDBc::raddeg)*cos(Orbit.PhiZero);
  Orbit.Axis[1]=-sin(MIR/AMSDBc::raddeg)*sin(Orbit.PhiZero);
  Orbit.Axis[2]=cos(MIR/AMSDBc::raddeg);
  Orbit.AlphaSpeed=AMSDBc::twopi/92.36/60.;
  Orbit.EarthSpeed=AMSDBc::twopi/24/3600;
  Orbit.PolePhi=CCFFKEY.polephi/AMSDBc::raddeg;
  Orbit.PoleTheta=78.6/AMSDBc::raddeg;
  Orbit.Nskip=0;
  Orbit.Ntot=AMSIO::getntot();
if(ipart == 3 && low ){
 HBOOK1(_hid,"Low Electron Spectrum",12,0.,6.,0.);
 HF1(_hid,0.75,12900.);
 HF1(_hid,1.25,4550.);
 HF1(_hid,1.75,1810.);
 HF1(_hid,2.25,846.);
 HF1(_hid,2.75,376.);
 HF1(_hid,3.25,177.);
 HF1(_hid,3.75,67.);
 HF1(_hid,4.25,29.);
 HF1(_hid,4.75,7.3);
 HF1(_hid,5.25,2.7);
 HF1(_hid,5.75,.4);
}
else if(low ==2){
  cout <<"AMSMceventg::setspectra-W-Sea Level muons Generator Chosen"<<endl;
  CMINIT();
}
else {
 integer nchan=1000;

 geant binw;
 if(mass < 0.938)binw=100;
 else  binw=100*mass/0.938;
 geant al=binw/2;
 geant bl=binw/2+nchan*binw;
 HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
 HBOOK1(_hid+1,"Spectrum",nchan,al,bl,0.);
//
// find a modulation
//
 geant modul[8]={400.,350.,550.,650.,950.,1300.,1200.,1000.};
 integer year=(begindate%10000+enddate%10000)/2-1997; 
 if(year <=0 || year > 7){
  cerr<<"AMSmceventg::setspectra-F-year not supported yet: "<<year<<endl;
  exit(1);
 }
 integer i; 
 for(i=0;i<nchan;i++){
  geant xm=i*binw+al+binw/2;
  number xmom=xm/1000;
  number xkin=(sqrt(xmom*xmom+mass*mass)-mass)*1000;
  number amass=mass*1000;
  number z=charge;
  number xkm=xkin+z*modul[year];
  number xt=xkm/1000+mass;
  number beta=sqrt(1.-mass*mass/xt/xt);
  number xrig=beta*xt/z;
  geant y;
  if(ipart ==2){
    // positron          
    y=700./1.5/pow(xt,3.3)*(0.02+0.1/sqrt(xt));
    y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
   
 
  }
  else if(ipart==3){
    //electron
    y=700./1.5/pow(xt,3.3);
    y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
  }
   else if (ipart < 15 ){
   // a-la proton
    y=1.5e4/beta/pow(xrig,2.74);
    y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
   }    
   else if (ipart > 15 && ipart < 100){
    // He etc...
    y=.5e4/beta/pow(xrig,2.68);
    y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
   }    
   else if (ipart == 15 || ipart > 100){
    number xb=1.5;
    number xa=3.0;
    number fact=pow(xa/xb,1./(xa+xb));
    fact=2.7/fact;
    number zz=xrig/fact;
    number ampl=0.1;
    y=ampl*pow(zz,xa)/(pow(zz,xa+xb)+1.);
    y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
   }
  HF1(_hid,xm,y);
  HF1(_hid+1,xm,y);
}
}
#ifdef __AMSDEBUG__
//HPRINT(_hid);
#endif
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
  _nskip=Orbit.Ntot;
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
        geant d;
        if (_fixeddir || _dir[2]<_albedocz || RNDM(d)< _albedorate){
          if(CCFFKEY.low == 0  && CCFFKEY.earth == 1 && !_fixeddir && !_fixedmom) 
           return EarthModulation();
          else return 1;
        }
      }
    }
  }
  return 0;
}
integer AMSmceventg::acceptio(){
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
          return 1;
      }
    }
  }
  return 0;
}

integer AMSmceventg::EarthModulation(){
  // Get current station position from event bank
  number polephi,theta,phi;
  
  AMSEvent::gethead()->GetGeographicCoo(polephi, theta, phi);
  //
  // direction to magnetic field
  //
  number um=sin(AMSDBc::pi/2-Orbit.PoleTheta)*cos(polephi);
  number vm=sin(AMSDBc::pi/2-Orbit.PoleTheta)*sin(polephi);
  number wm=cos(AMSDBc::pi/2-Orbit.PoleTheta);
  //
  // direction to station position
  //
  number up=sin(AMSDBc::pi/2-theta)*cos(phi);
  number vp=sin(AMSDBc::pi/2-theta)*sin(phi);
  number wp=cos(AMSDBc::pi/2-theta);
  number cts=um*up+vm*vp+wm*wp;
  number xl=acos(cts);
  number cl=fabs(sin(xl));
  number uv=vm*wp-wm*vp;
  number vv=wm*up-wp*um;
  number wv=um*vp-vm*up;
  //
  // particle dir in global system
  // AMS x along the shuttle/station flight
  //
  number amsx[3],amsy[3],amsz[3];
  amsz[0]=up;
  amsz[1]=vp;
  amsz[2]=wp;
  amsy[0]=Orbit.Axis[0];
  amsy[1]=Orbit.Axis[1];
  amsy[2]=Orbit.Axis[2];
  amsx[0]=amsy[1]*amsz[2]-amsy[2]*amsz[1];
  amsx[1]=amsy[2]*amsz[0]-amsy[0]*amsz[2];
  amsx[2]=amsy[0]*amsz[1]-amsy[1]*amsz[0];


  number ue=_dir[0]*amsx[0]+_dir[1]*amsy[0]+_dir[2]*amsz[0];
  number ve=_dir[0]*amsx[1]+_dir[1]*amsy[1]+_dir[2]*amsz[1];
  number we=_dir[0]*amsx[2]+_dir[1]*amsy[2]+_dir[2]*amsz[2];
  
  number cth=ue*uv+ve*vv+we*wv;
  number mom=52.5*pow(cl,4)/pow(sqrt(1.-cth*pow(cl,3))+1,2)*fabs(_charge);
  if (_mom > mom)return 1;
  else {
    ++Orbit.Nskip;   
   return 0;
  }
}
  
  void AMSmceventg::init(integer ipart){
_ipart=ipart;
char chn[22]="";
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
    // Set seed
   GRNDMQ(_seed[0],_seed[1],0,"G");
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
    //    do{
    //      gener();
    //    }while(!accept());
    //

   _coo=coo;
   _dir=dir;
   _mom=mom;
   if(acceptio()){
    vertex[0]=_coo[0];
    vertex[1]=_coo[1];
    vertex[2]=_coo[2];
    plab[0]=_mom*_dir[0];
    plab[1]=_mom*_dir[1];
    plab[2]=_mom*_dir[2];
    GSVERT(vertex,0,0,0,0,nvert);
    GSKINE(plab,_ipart,nvert,0,0,nt);
   }   
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
  if(IOPA.mode ==2 || IOPA.mode==3){
  // write elem  for simple i/o
   integer run,event;
   run=AMSEvent::gethead()->getrun();
   event=AMSEvent::gethead()->getEvent();
   number pole,theta,phi;
   AMSEvent::gethead()->GetGeographicCoo(pole,theta,phi);
   AMSIO io(run,event,AMSEvent::gethead()->gettime(),_ipart,_seed,_coo,_dir,_mom,
   pole,theta,phi,_nskip);
   io.write();
  }
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
  "EventNoMCEventG:I*4, Nskip:I, Particle:I*4,  Coo(3):R*4, Dir(3):R*4, Momentum:R*4, Mass:R*4, Charge:R*4");

}
  GN.Event()=AMSEvent::gethead()->getid();
  GN.Nskip=_nskip;
  GN.Particle=_ipart;
  for(i=0;i<3;i++)GN.Coo[i]=_coo[i];
  for(i=0;i<3;i++)GN.Dir[i]=_dir[i];
  GN.Momentum=_mom;
  GN.Mass=_mass;
  GN.Charge=_charge;
  HFNTB(IOPA.ntuple,"MCEventG");

}


