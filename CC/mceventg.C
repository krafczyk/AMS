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
if(!CCFFKEY.oldformat)Orbit.UpdateAxis(io.getveltheta(),
   io.getvelphi(),io.getstheta(),io.getsphi());
}


AMSmceventg::AMSmceventg(integer ipart, geant mom, AMSPoint & coo,
AMSDir & dir, integer nskip):_nskip(nskip){
init(ipart);
_mom=mom;
int i;
for(i=0;i<3;i++){
 _coo[i]=coo[i];
 _dir[i]=dir[i];
}
}

void AMSmceventg::gener(){
     char cdir[256];
    HCDIR(cdir,"R");
    char hpawc[256]="//PAWC/GEN";
    HCDIR (hpawc," ");
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
   geant d;
   if(_fixedmom)_mom=_momrange[0];
   else if(CCFFKEY.low!=4){
   _mom=HRNDM1(_hid)/1000.;  // momentum in GeV
   }
   else _mom=_momrange[0]+(_momrange[1]-_momrange[0])*RNDM(d);

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
  char hp[9]="//PAWC";
  HCDIR(hp," ");
  HCDIR (cdir, " ");
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

  number xa=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
  number ya=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
  number za=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
  number xb=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
  number yb=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
  number zb=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
  number lx=xb-xa;
  number ly=yb-ya;
  number lz=zb-za;
 switch(curp){
 case 1:
  _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
  _coo=AMSPoint(xa+lx*RNDM(d),ya+ly*RNDM(d),zb);
  break;
 case 2:  
  _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
  _coo=AMSPoint(xa+lx*RNDM(d),ya+ly*RNDM(d),za);
  break;
 case 3:  
  _dir=AMSDir(-cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(xb,ya+ly*RNDM(d),za+lz*RNDM(d));
  break;
 case 4:  
  _dir=AMSDir(cos(theta),cos(phi)*sin(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(xa,ya+ly*RNDM(d),za+lz*RNDM(d));
  break;
 case 5:  
  _dir=AMSDir(cos(phi)*sin(theta),-cos(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(xa+lx*RNDM(d),yb,za+lz*RNDM(d));
  break;
 case 6:  
  _dir=AMSDir(cos(phi)*sin(theta),cos(theta),sin(phi)*sin(theta));
  _coo=AMSPoint(xa+lx*RNDM(d),ya,za+lz*RNDM(d));
  break;
 default:  
  cerr <<" AMSmceventg-F-plane problem "<<curp<<endl;
  exit(1);
 }
//if(_fixedplane == 0)_coo=_coo/2;
}
  }
  char hp[9]="//PAWC";
  HCDIR(hp," ");
  HCDIR (cdir, " ");
}


void AMSmceventg::setspectra(integer begindate, integer begintime, 
                             integer enddate, integer endtime, 
                              integer ipart,  integer low){

  Orbit.Begin.tm_year  =  begindate%10000-1900;
  Orbit.Begin.tm_mon = (begindate/10000)%100-1;
  Orbit.Begin.tm_mday   = (begindate/1000000)%100;
  Orbit.Begin.tm_hour  = (begintime/10000)%100;
  Orbit.Begin.tm_min= (begintime/100)%100;
  Orbit.Begin.tm_sec=(begintime)%100;
  //  Orbit.Begin.tm_isdst =  Orbit.Begin.tm_mon>=3 &&  Orbit.Begin.tm_mon<=8;
  Orbit.Begin.tm_isdst =  0;

  Orbit.End.tm_year  =  enddate%10000-1900;
  Orbit.End.tm_mon = (enddate/10000)%100-1;
  Orbit.End.tm_mday   = (enddate/1000000)%100;
  Orbit.End.tm_hour  = (endtime/10000)%100;
  Orbit.End.tm_min=(endtime/100)%100;
  Orbit.End.tm_sec=(endtime)%100;
  //  Orbit.End.tm_isdst = Orbit.End.tm_mon>=3 &&  Orbit.End.tm_mon<=8;
  Orbit.End.tm_isdst = 0;
  Orbit.FlightTime=difftime(mktime(&Orbit.End),mktime(&Orbit.Begin));
  if(Orbit.FlightTime < 0){
    cerr <<"AMSmceventg::setspectra-F-FlighTime < 0 "<<Orbit.FlightTime<<endl;
    exit(1);
  }  
  Orbit.ThetaI=CCFFKEY.theta/AMSDBc::raddeg;
  Orbit.PhiI=fmod(CCFFKEY.phi/AMSDBc::raddeg+AMSDBc::twopi,AMSDBc::twopi);
  const number MIR=51.65;
  Orbit.AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  Orbit.UpdateOrbit(Orbit.ThetaI,Orbit.PhiI,CCFFKEY.sdir);
  Orbit.AlphaSpeed=AMSDBc::twopi/90.8/60.;
  Orbit.EarthSpeed=AMSDBc::twopi/24/3600;
  Orbit.PolePhi=CCFFKEY.polephi/AMSDBc::raddeg;
  Orbit.EarthR=6371.2e5;                      
  Orbit.AlphaAltitude=Orbit.EarthR+380.e5;  
  Orbit.DipoleR=534.259e5;                       // Dipole Shift Distance  (cm)
  Orbit.DipoleTheta=21.687/AMSDBc::raddeg;       //              Latitude  (rad)
  Orbit.DipolePhi=144.280/AMSDBc::raddeg;        //              Longitude (rad)
  Orbit.PoleTheta=-79.397/AMSDBc::raddeg;        // Dipole Direction Lat   (rad)
  Orbit.PolePhiStatic=108.392/AMSDBc::raddeg;    //                  Lon   (rad)
  Orbit.Nskip=0;
  Orbit.Nskip2=0;
  Orbit.Ntot=AMSIO::getntot();
  if(ipart==0){
    if(IOPA.mode!=1){
      cerr << "AMSmceventg::setspectra-F-PARTNotDefined"<<endl;
      exit(1);
    }
    return;
  }

  if(AMSJob::gethead()->isSimulation()){
    char cdir[256];
    HCDIR(cdir,"R");
    HMDIR("//PAWC/GEN","S");
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

    if(ipart == 3 && low==3 ){
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
    else if(low==1){
      cout <<"AMSMceventg::setspectra-W-UnderCutoffParticleGeneratorChosen"<<endl;
   
      integer nchan=500;
      geant binw;
      if(mass < 0.938)binw=40;
      else  binw=40*mass/0.938/charge;
      geant al=binw/2;
      
      geant bl=binw/2+nchan*binw;
      HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
      for(int i=0;i<nchan;i++){
        geant xm=i*binw+al+binw/2;
        number xmom=xm/1000;
       number xr=xmom/charge;
        number xkin=(sqrt(xmom*xmom+mass*mass)-mass)*1000;
        geant y=10*exp(-2.6*xr);
        if(xkin>5*charge*charge)HF1(_hid,xm,y);
     }
          
          HPRINT(_hid);
    }
    else {
      integer nchan=10000;
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
     char hp[9]="//PAWC";
     HCDIR(hp," ");
     HCDIR (cdir, " ");
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
  number xa=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
  number ya=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
  number za=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
  number xb=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
  number yb=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
  number zb=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
  number lx=xb-xa;
  number ly=yb-ya;
  number lz=zb-za;
    area[0]=lx*ly;
    area[1]=lx*ly;
    area[2]=ly*lz;
    area[3]=ly*lz;
    area[4]=lx*lz;
    area[5]=lx*lz;
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
        if(CCFFKEY.low || _fixeddir || _dir[2]<_albedocz || RNDM(d)< _albedorate){
          if(CCFFKEY.low == 0  && CCFFKEY.earth == 1 && !_fixeddir && !_fixedmom) 
           return EarthModulation();
          else return 1;
        }
      }
    }
  }
  ++Orbit.Nskip;   
  ++Orbit.Nskip2;   
  return 0;
}
integer AMSmceventg::acceptio(){
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
          if(AMSJob::gethead()->isRealData()){
            if(!MISCFFKEY.BeamTest || (_ipart==GCKINE.ikine || (_ipart>0  && GCKINE.ikine==-1)))return 1;
          }
          else return EarthModulation();
      }
    }
  }
  return 0;
}

integer AMSmceventg::EarthModulation(){
  // Get current station position from event bank
  number polephi,theta,phi;
  
  AMSEvent::gethead()->GetGeographicCoo(polephi, theta, phi);
  geant pitch=AMSEvent::gethead()->getpitch();
  geant roll=AMSEvent::gethead()->getroll();
  geant yaw=AMSEvent::gethead()->getyaw();
  //
  // Dipole direction
  //
  number um=sin(AMSDBc::pi/2-Orbit.PoleTheta)*cos(polephi);
  number vm=sin(AMSDBc::pi/2-Orbit.PoleTheta)*sin(polephi);
  number wm=cos(AMSDBc::pi/2-Orbit.PoleTheta);

  //
  // Direction Dipole to Station 
  //
  number dphi=Orbit.DipolePhi+polephi-Orbit.PolePhiStatic;
  number xc=Orbit.AlphaAltitude*sin(AMSDBc::pi/2-theta)*cos(phi)-
   Orbit.DipoleR*sin(AMSDBc::pi/2-Orbit.DipoleTheta)*cos(dphi);
  number yc=Orbit.AlphaAltitude*sin(AMSDBc::pi/2-theta)*sin(phi)-
   Orbit.DipoleR*sin(AMSDBc::pi/2-Orbit.DipoleTheta)*sin(dphi);
  number zc=Orbit.AlphaAltitude*cos(AMSDBc::pi/2-theta)-
   Orbit.DipoleR*cos(AMSDBc::pi/2-Orbit.DipoleTheta);
  number rl=sqrt(xc*xc+zc*zc+yc*yc);
  number  up=xc/rl;
  number  vp=yc/rl;
  number  wp=zc/rl;

  number cts=um*up+vm*vp+wm*wp;
  number xl=acos(cts);
  number cl=fabs(sin(xl));
  number rgm=rl;

  // Magnetic East
  xc=vm*wp-wm*vp;
  yc=wm*up-um*wp;
  zc=um*vp-vm*up;
  rl=sqrt(xc*xc+yc*yc+zc*zc);

  number  uv=xc/rl;
  number  vv=yc/rl;
  number  wv=zc/rl;

  //
  // particle dir in global system
  // AMS x along the shuttle/station flight
  //
  number amsxg[3],amsyg[3],amszg[3];
  amszg[0]=sin(AMSDBc::pi/2-theta)*cos(phi);
  amszg[1]=sin(AMSDBc::pi/2-theta)*sin(phi);
  amszg[2]=cos(AMSDBc::pi/2-theta);
  amsyg[0]=Orbit.Axis[0];
  amsyg[1]=Orbit.Axis[1];
  amsyg[2]=Orbit.Axis[2];
  amsxg[0]=amsyg[1]*amszg[2]-amsyg[2]*amszg[1];
  amsxg[1]=amsyg[2]*amszg[0]-amsyg[0]*amszg[2];
  amsxg[2]=amsyg[0]*amszg[1]-amsyg[1]*amszg[0];

  number amsx[3],amsy[3],amsz[3];
  number cp=cos(pitch); 
  number sp=sin(pitch);
  number cy=cos(yaw);
  number sy=sin(yaw);
  number cr=cos(roll);
  number sr=sin(roll);
  number l1=cy*cp;
  number m1=-sy;
  number n1=cy*sp;
  number l2=cr*sy*cp-sr*sp;
  number m2=cr*cy;
  number n2=cr*sy*sp+sr*cp;
  number l3=-sr*sy*cp-cr*sp;
  number m3=-sr*cy;
  number n3=-sr*sy*sp+cr*cp;
  amsx[0]=-(l1*amsxg[0]+l2*amsyg[0]+l3*amszg[0]);
  amsx[1]=-(l1*amsxg[1]+l2*amsyg[1]+l3*amszg[1]);
  amsx[2]=-(l1*amsxg[2]+l2*amsyg[2]+l3*amszg[2]);
  amsy[0]=-(m1*amsxg[0]+m2*amsyg[0]+m3*amszg[0]);
  amsy[1]=-(m1*amsxg[1]+m2*amsyg[1]+m3*amszg[1]);
  amsy[2]=-(m1*amsxg[2]+m2*amsyg[2]+m3*amszg[2]);
  amsz[0]=n1*amsxg[0]+n2*amsyg[0]+n3*amszg[0];
  amsz[1]=n1*amsxg[1]+n2*amsyg[1]+n3*amszg[1];
  amsz[2]=n1*amsxg[2]+n2*amsyg[2]+n3*amszg[2];
  number ue=_dir[0]*amsx[0]+_dir[1]*amsy[0]+_dir[2]*amsz[0];
  number ve=_dir[0]*amsx[1]+_dir[1]*amsy[1]+_dir[2]*amsz[1];
  number we=_dir[0]*amsx[2]+_dir[1]*amsy[2]+_dir[2]*amsz[2];
  
  number cth=ue*uv+ve*vv+we*wv;
  number xfac=(CCFFKEY.Fast==0?57.576:52.)*Orbit.EarthR/rgm*Orbit.EarthR/rgm;
  number chsgn=_charge/fabs(_charge);
  number cl3=cl*cl*cl;
  number cl4=cl*cl*cl*cl;
  number mom=xfac*cl4/(sqrt(1.-chsgn*cth*cl3)+1)/(sqrt(1.-chsgn*cth*cl3)+1)*fabs(_charge);
  if (_mom > mom)return 1;
  else {
  //cout <<xfac<<" "<<_mom<<" "<<mom<<endl;
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
stream <<" Coo "<<_coo[0]<<" "<<_coo[1]<<" "<<_coo[2]<<endl;
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
   AMSIO io(run,event,AMSEvent::gethead()->gettime(),AMSEvent::gethead()->getusec(),_ipart,_seed,_coo,_dir,
   _mom,pole,theta,phi,_nskip,AMSEvent::gethead()->GetStationRad(),
   AMSEvent::gethead()->getveltheta(),AMSEvent::gethead()->getvelphi(),
   AMSEvent::gethead()->getyaw(),AMSEvent::gethead()->getroll(),
   AMSEvent::gethead()->getpitch(),AMSEvent::gethead()->getangvel());
   io.write();
  }
}

void AMSmceventg::_writeEl(){
  MCEventGNtuple* GN = AMSJob::gethead()->getntuple()->Get_mcg();

  if (GN->Nmcg>=MAXMCG) return;

// Fill the ntuple
  GN->Nskip[GN->Nmcg]=_nskip;
  GN->Particle[GN->Nmcg]=_ipart;
  int i;
  for(i=0;i<3;i++)GN->Coo[GN->Nmcg][i]=_coo[i];
  for(i=0;i<3;i++)GN->Dir[GN->Nmcg][i]=_dir[i];
  GN->Momentum[GN->Nmcg]=_mom;
  GN->Mass[GN->Nmcg]=_mass;
  GN->Charge[GN->Nmcg]=_charge;
  GN->Nmcg++;

}


integer AMSmceventg::checkdaqid(int16u id){
if(id==getdaqid())return 1;
else return 0;
}

void AMSmceventg::builddaq(integer i, integer length, int16u *p){
  AMSmceventg *ptr=(AMSmceventg*)AMSEvent::gethead()->
  getheadC("AMSmceventg",0);
 *p=getdaqid();
while(ptr){ 
 const uinteger c=65535;
 *(p+1)=ptr->_ipart;
 uinteger momentum=uinteger(ptr->_mom*1000);
 *(p+3)=int16u(momentum&c);
 *(p+2)=int16u((momentum>>16)&c);
 int16u d=int16u((ptr->_dir[0]+1)*10000);
 *(p+4)=d;
 d=int16u((ptr->_dir[1]+1)*10000);
 *(p+5)=d;
 d=int16u((ptr->_dir[2]+1)*10000);
 *(p+6)=d;
 integer big=10000;
 for(int k=0;k<3;k++){
 if(ptr->_coo[k]+big<0){
  cerr<<"AMSmceventg::builddaq-E-NegativeCoo "<<ptr->_coo[k]<<endl;
 }
 uinteger cd=(ptr->_coo[k]+big)*10000;
 *(p+8+2*k)=int16u(cd&c);
 *(p+7+2*k)=int16u((cd>>16)&c);
 }
 ptr=ptr->next();
 p+=12;
}

}


void AMSmceventg::buildraw(integer n, int16u *p){
 integer ip;
 geant mom;
 for(int16u *ptr=p+1;ptr<p+n;ptr+=12){ 
  ip=*(ptr);
  uinteger momi= (*(ptr+2)) | (*(ptr+1))<<16;
  mom=momi/1000.;
  AMSDir dir(integer(*(ptr+3))-10000,integer(*(ptr+4))-10000,integer(*(ptr+5))-10000);
  uinteger cdx=  (*(ptr+7)) | (*(ptr+6))<<16;  
  uinteger cdy=  (*(ptr+9)) | (*(ptr+8))<<16;  
  uinteger cdz=  (*(ptr+11)) | (*(ptr+10))<<16;  
  AMSPoint coo(cdx/10000.-10000.,cdy/10000.-10000.,cdz/10000.-10000.);
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), new
        AMSmceventg(ip,mom,coo,dir));

}

} 

integer AMSmceventg::calcdaqlength(integer i){
 AMSContainer *p = AMSEvent::gethead()->getC("AMSmceventg");
 if(p)return 1+12*p->getnelem();
 else return 1;
}

void orbit::UpdateOrbit(number theta, number phi, integer sdir){
  ThetaI=theta;
  PhiI=phi;
  number MIR=atan(AlphaTanThetaMax);
  number r= tan(ThetaI)/AlphaTanThetaMax;
  if(r > 1 || r < -1){
    cerr <<"AMSMCEVENTG::setspectra-ThetaI too high "<<ThetaI<<endl;
    if(ThetaI < 0 )ThetaI = -MIR;
    else ThetaI= MIR;
    cerr <<"AMSMCEVENTG::setspectra-ThetaI brought to "<<ThetaI<<endl;
    r=tan(ThetaI)/AlphaTanThetaMax;
  }
  PhiZero=PhiI-atan2(r,sdir*sqrt(1-r*r));
  Axis[0]=sin(MIR)*sin(PhiZero);
  Axis[1]=-sin(MIR)*cos(PhiZero);
  Axis[2]=cos(MIR);
  AMSDir ax1(AMSDBc::pi/2-ThetaI,PhiI);
  AMSDir ax2(AMSDBc::pi/2,PhiZero);
  if(ax1.prod(Axis)>1.e-5){
   cerr<<"AMSmceventg::setspectra-F-OrbitParametersWrong "<<ax1.prod(Axis);
  exit(1);
  }
}

integer orbit::UpdateOrbit(number xsec, geant & ThetaS, geant & PhiS,
                        geant & PolePhi, time_t & time){

    number t2=
      AlphaTanThetaMax*AlphaTanThetaMax;
    number theta=ThetaI;
    number philocal=
      atan2(sin(PhiI-PhiZero)*sqrt(1+t2),
            cos(PhiI-PhiZero));
    number pole=PolePhi;
    pole=fmod(pole+EarthSpeed*xsec,AMSDBc::twopi);
    philocal=fmod(philocal+AlphaSpeed*xsec,AMSDBc::twopi);
    number phi=atan2(sin(philocal),cos(philocal)*sqrt(1+t2));
    if(phi < 0)phi=phi+AMSDBc::twopi;
    theta=asin(sin(atan(AlphaTanThetaMax))*sin(philocal));
    time=integer(mktime(&Begin)+xsec);
    ThetaS=theta;
    PolePhi=pole;
    PhiS=fmod(phi+PhiZero,AMSDBc::twopi);
    return cos(PhiS-PhiZero)>0?1:-1;
}

orbit::orbit(geant Th,geant Ph, geant Pole, integer Dir):
ThetaI(Th),PolePhi(Pole){
  PhiI=fmod(Ph+AMSDBc::twopi,AMSDBc::twopi);
  const number MIR=51.65;
  AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  UpdateOrbit(ThetaI,PhiI,Dir);
  AlphaSpeed=AMSDBc::twopi/90.8/60.;
  EarthSpeed=AMSDBc::twopi/24/3600;
  Begin.tm_year  =  98;
  Begin.tm_mon = 05;
  Begin.tm_mday   = 2;
  Begin.tm_hour  = 12;
  Begin.tm_min= 0;
  Begin.tm_sec=0;
  Begin.tm_isdst =  0;

}
void orbit::UpdateAxis(number vt, number vp, number t, number p){
  AMSDir ax1(AMSDBc::pi/2-t,p);
  AMSDir ax2(AMSDBc::pi/2-vt,vp);
  Axis=ax1.cross(ax2);
}
