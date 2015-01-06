//  $Id$
// Author V. Choutko 24-may-1996
//#undef __ASTRO__ 

#include "mceventg.h"
#include <math.h>
#include "amsdbc.h"
#include "event.h"
#include "commons.h"
#include "ntuple.h"
#include "io.h"
#include "extC.h"
#include "ecaldbc.h"
#include "tofdbc02.h"
#include "astro.h" //ISN 
#include "root.h"
#ifdef _PGTRACK_
#include "HistoMan.h"
#endif
#include "OrbGen.h" 

#ifdef __AMSVMC__

#include "amsvmc_MCStack.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include <TVirtualMC.h>
#include <TVirtualMCStack.h>
#include <TVirtualMCApplication.h>
#include <TRandom.h>
#include <TPDGCode.h>
#include <TVector3.h>
#include <TParticlePDG.h>
#include <TDatabasePDG.h>
#include "TROOT.h"
#include "TGeant3.h"
#include "TGeant3TGeo.h"
#include "g4physics.h"
//extern TGeant4* geant4;
extern TGeant3* geant3;
#endif


#if  defined __G4AMS__ 
#include "CLHEP/Random/Random.h"
#include "g4util.h"
#include <iostream>
#endif

#if defined __AMSVMC__
#include "CLHEP/Random/Random.h"
using namespace CLHEP;
#endif

orbit AMSmceventg::Orbit;
integer AMSmceventg::_hid=20001;
AMSPoint* AMSmceventg::_r_c;
AMSDir* AMSmceventg::_dir_c;
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
int   AMSmceventg::_particle[30];
int   AMSmceventg::_nucleons[30];
geant*   AMSmceventg::_spectra[30];
number   AMSmceventg::_flux[30];
TH3F* AMSmceventg::ratio=0;

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
 _tbline=0;
 _trkid=1;
 _parentid=0;
InitSeed();
if(!CCFFKEY.oldformat)Orbit.UpdateAxis(io.getveltheta(),
   io.getvelphi(),io.getstheta(),io.getsphi());
}


AMSmceventg::AMSmceventg(integer ipart, geant mom, const AMSPoint & coo,
			 const AMSDir & dir, integer nskip):_coo(coo),_dir(dir),_mom(mom),_delay(0),_ipart(ipart),_trkid(1),_parentid(0),_nskip(nskip),_tbline(0),_parinfo(1){
if(abs(_parinfo)==1)init(ipart);

}

AMSmceventg::AMSmceventg(integer ipart, integer trackid, integer parentid, geant mom, const AMSPoint & coo, const AMSDir & dir, integer parinfo,float charge,float mass,integer nskip):_coo(coo),_dir(dir),_mom(mom),_mass(mass),_charge(charge),_delay(0),_ipart(ipart),_trkid(trackid), _parentid(parentid), _nskip(nskip),_tbline(0),_parinfo(parinfo){
if(abs(_parinfo)==1)init(ipart);
_mom=sqrt( (_mom+_mass)*(_mom+_mass)-_mass*_mass);

 }

double AMSmceventg::extract(float m, float s){
  if (s==0)  return m;
  float s2=fabs(s);
  int d=1;
  if(s>0)
    return (m+(2*RNDM(d)-1)*s2);    
  else
    return (m+rnormx()*s2);    
}



int  AMSmceventg::readposfromfile(char* filename,vector<tbpos> &poslist){
  FILE* ff=fopen(filename,"r");
  if(!ff) return -1;
  while(1){
    float a,b,c,d,e,f;
    tbpos aa;
    fscanf(ff,"%f %f %f %f %f %f",&a,&b,&c,&d, &e, &f);
    if(feof(ff)) break;
    aa.X=a;  aa.Y=b;  aa.Z=c;
    aa.DX=d; aa.DY=e; aa.DZ=f;
    poslist.push_back(aa);
  }
  
  return poslist.size();
}

static char gPAWStandardDirectory[7] = "//PAWC";
static char gPAWGeneratorDirectory[11]="//PAWC/GEN";

void AMSmceventg::gener(){
  HCDIR(gPAWGeneratorDirectory, " ");
  if(CCFFKEY.low==8){ // simple test beam mode
    _coo[0]=extract(CCFFKEY.coo[0],CCFFKEY.coo[3]);
    _coo[1]=extract(CCFFKEY.coo[1],CCFFKEY.coo[4]);
    _coo[2]=extract(CCFFKEY.coo[2],CCFFKEY.coo[5]);
    float Th  =extract(CCFFKEY.dir[0],CCFFKEY.dir[3]);
    float Ph  =extract(CCFFKEY.dir[1],CCFFKEY.dir[4]);
    _dir[0]=sin(Th)*cos(Ph);
    _dir[1]=sin(Th)*sin(Ph);
    _dir[2]=cos(Th);

    _mom   =extract(CCFFKEY.momr[0],CCFFKEY.momr[1]);
    //  printf("Want to fire mom %f from %f %f %f dir %f %f %f\n",_mom,
    //      _coo[0],_coo[1],_coo[2],
    //      _dir[0],_dir[1],_dir[2]);
  }
  else if(CCFFKEY.low==9){ // test beam mode from file
    static int readfile=0;
    static int index=0;
    static int maxlist=0;
    static vector<tbpos> poslist;
    if(readfile==0){

      poslist.clear();
      char filename[400];
      sprintf(filename,"%s/TestBeamPos_%03.0f.txt",AMSDATADIR.amsdatadir,CCFFKEY.dir[5]);
      maxlist=readposfromfile(filename,poslist);
      if(maxlist<0) {
	printf("AMSmceventg::gener-E-: Cannot open file with TB postions filename:%s\n",filename);
	printf("Sorry I give up \n");
	exit(-5);
	return;
      }
      readfile=1;
    }
    if(index>=maxlist) index=0;
    _coo[0]=extract(0.,CCFFKEY.coo[3]);
    _coo[1]=extract(0.,CCFFKEY.coo[4]);
    _coo[2]=0;
    _coo[0]+=poslist[index].X;
    _coo[1]+=poslist[index].Y;
    _coo[2]+=poslist[index].Z;

//     float Th  = RNDM(d)*CCFFKEY.dir[0];
//     float Ph  = RNDM(d) *2*M_PI;    
//     float nn=sqrt(pow(poslist[index].DX,2)+pow(poslist[index].DY,2)+pow(poslist[index].DZ,2));
//     float CT=poslist[index].DZ/nn;
//     float ST=sqrt(pow(poslist[index].DX,2)+pow(poslist[index].DY,2))/nn;
//     float CP=poslist[index].DX/sqrt(pow(poslist[index].DX,2)+pow(poslist[index].DY,2));
//     float SP=poslist[index].DY/sqrt(pow(poslist[index].DX,2)+pow(poslist[index].DY,2));
//     float rotmat[3][3];
//     rotmat[0][0]= CP*CT;    rotmat[0][1]=SP;     rotmat[0][2]=-CP*ST;
//     rotmat[1][0]=-SP*CT;    rotmat[1][1]=CP;     rotmat[1][2]= SP*ST;
//     rotmat[2][0]= ST;       rotmat[2][1]=0;      rotmat[2][2]= CT;
//     float d0[3];
//     d0[0]=sin(Th)*cos(Ph);
//     d0[1]=sin(Th)*sin(Ph);
//     d0[2]=cos(Th);
//     for(int ii=0;ii<3;ii++)
//       _dir[ii]=d0[0]*rotmat[ii][0]+d0[1]*rotmat[ii][1]+d0[2]*rotmat[ii][2];
    
    _dir[0]=poslist[index].DX;
    _dir[1]=poslist[index].DY;
    _dir[2]=poslist[index].DZ;

    _mom   =extract(CCFFKEY.momr[0],CCFFKEY.momr[1]);
    _tbline=index++;
//     printf("Want to fire mom %f from %f %f %f dir %f %f %f\n",_mom,
// 	   _coo[0],_coo[1],_coo[2],
// 	   _dir[0],_dir[1],_dir[2]);

#ifdef _PGTRACK_
  }else if(CCFFKEY.low==10){ // realistic orbit generator for plaen1/9 movement studies

    OrbGen*orb=OrbGen::GetOrbGen();
    orb->GenDir();
    for(int ii=0;ii<3;ii++){
      _dir[ii]=orb->Dir[ii];
      _coo[ii]=orb->Coo[ii];
    }
    _mom=orb->Rigidity;
    _tbline=0;
    init(orb->Pid);
    _mom=orb->Rigidity*fabs(_charge);
#endif
  }else  if(CCFFKEY.low==-2){
    //
    //  wrong method do not use  (VC)
    //
    geant mom,themu,phimu,chmu,xmu,ymu,zmu;
    while(1){
      CMGENE(mom,themu,phimu,chmu,xmu,ymu,zmu);
      if(TFMCFFKEY.fast==0)break;
      if(fastcheck(xmu,ymu,zmu,themu,phimu))break;
    }
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
    number _gamma=0;
    integer curp=0;
    number phi;
    number theta;
    geant d(-1);
    if(_fixedmom)_mom=_momrange[0];
    else if(CCFFKEY.low==2){
      geant mom,gamma,chmu;
      CMGENE2(mom,gamma,chmu);
      _mom=mom;
      _gamma=gamma; 
      if(chmu < 0)_ipart=6;
      else _ipart=5;
      //      cout <<"  mom "<<_mom<<" "<<_gamma<<" "<<_ipart<<endl;
      init(_ipart);
    }
    else if(CCFFKEY.low==4){
      _mom=_momrange[0]+(_momrange[1]-_momrange[0])*RNDM(d);
      if(_momrange[1]>1000000){
        cerr<<"AMSmceventg::gener-F-MomentumRangeIsLimitiedTo 1000 TeV "<<_momrange[1]<<endl;
        exit(1);
      }
    }
    else if(CCFFKEY.low==5 ){
      if(_momrange[0]<=0 ){
        cerr<<"AMSmceventg::gener-F-LogInfinity "<<endl;
        exit(1);
      }              
      number lrange=log(fabs(_momrange[1]))-log(fabs(_momrange[0]));
      _mom=_momrange[0]*exp(lrange*RNDM(d));
    }
    
    else if(CCFFKEY.low==6){
      //  get particle id
      int xp=HRNDM1(_hid);
      GCKINE.ikine=_particle[xp];
      if(GCKINE.ikine<0){
	cerr<<"  AMSmceventg::gener-F-WrongParticle "<<GCKINE.ikine<<endl;
	abort();
      }
      GCKINE.ipart=GCKINE.ikine;
      init(GCKINE.ikine);      
      _mom=exp(HRNDM1(_hid+xp+1))/1000.;
      number etot=_mom*_nucleons[xp]+_mass;
      _mom=sqrt(etot*etot-_mass*_mass);
      //     if(_ipart>50)cout <<"  got "<<_ipart<<" "<<_charge<<" "<<_mom<<" "<<_nucleons[xp]<<" "<<xp<<endl;     
    } 
    // Generates uniformely on 13 Rig Points by PZ
    // documented in datacards.doc
    else if(CCFFKEY.low==7){
      static int my_index=0;
      number mom_array[13]={0.5,1.,2.,5.,
			    10.,20.,50.,
			    100.,200.,500.,
			    1000.,2000.,5000.};
      if(my_index>=13) my_index=0;
      _mom=mom_array[my_index++];
      
    }
    
    else{
#ifdef __G4AMS__
      if(MISCFFKEY.G4On){
        _mom=AMSRandGeneral::hrndm1(_hid)/1000.;
        if(AMSEventR::h1(_hid+1))AMSEventR::hf1(_hid+1,_mom);
         //cout <<" **********mom    .... "<<_mom<<endl; 
      }
      else
#endif
        _mom=HRNDM1(_hid)/1000.;  // momentum in GeV
      //cout<<"mom="<<_mom<<" hid="<<_hid<<endl;
    }
  again:
    geant th,ph; // photon incidence angle (normal to gamma source generation plane)
    if(_fixeddir && CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1<=0){ // when GammaSource>0 -> fixeddir=1 (this is done elsewhere)
      if(!(GMFFKEY.GammaSource==0)){  
	number ra,dec;  // AMS zenithal pointing direction
	number rai,deci; //ISS zenithal pointing direction (AMS not tilted)
        AMSEvent::gethead()->GetISSCoo(rai,deci); //non-tilted case      
        AMSEvent::gethead()->GetAMSCoo(ra,dec);
        skyposition isspos(rai,deci); //non-tilted case
        skyposition amspos(ra,dec); // amspos is filled up      
        
        if(GMFFKEY.GammaSource==1){ // user-defined source 
          skyposition sourcepos(GMFFKEY.SourceCoo[0],GMFFKEY.SourceCoo[1]);
          //isspos.CalcDir(sourcepos,th,ph); //non-tilted case
          amspos.CalcDir(sourcepos,th,ph);
        }
        else { // tabulated source
          geant rs,ds; // to store output from lookupsourcecoo
          lookupsourcecoo(GMFFKEY.GammaSource,rs,ds);
          skyposition sourcepos(rs,ds);
          //isspos.CalcDir(sourcepos,th,ph); //non-tilted case
          amspos.CalcDir(sourcepos,th,ph);
        }
        
        if(fabs(th)<GMFFKEY.SourceVisib){
          _dir=AMSDir(-cos(ph)*sin(th),-sin(ph)*sin(th),-cos(th)); // invert direction TO source to direction FROM source.
        } 
        else _dir=AMSDir(cos(ph)*sin(th),sin(ph)*sin(th),cos(th));
      }// ISN
      else _dir=AMSDir(_dirrange[0]);

      if(_fixedpoint){
        _coo=_coorange[0];
      }
      else {//<---fixeddir+rand.point
        number x0=_coorange[0][0]>-AMSDBc::ams_size[0]/2?_coorange[0][0]:-AMSDBc::ams_size[0]/2;
        number y0=_coorange[0][1]>-AMSDBc::ams_size[1]/2?_coorange[0][1]:-AMSDBc::ams_size[1]/2;
        number z0=_coorange[0][2]>-AMSDBc::ams_size[2]/2?_coorange[0][2]:-AMSDBc::ams_size[2]/2;
        number x1=_coorange[1][0]<AMSDBc::ams_size[0]/2?_coorange[1][0]:AMSDBc::ams_size[0]/2;
        number y1=_coorange[1][1]<AMSDBc::ams_size[1]/2?_coorange[1][1]:AMSDBc::ams_size[1]/2;
        number z1=_coorange[1][2]<AMSDBc::ams_size[2]/2?_coorange[1][2]:AMSDBc::ams_size[2]/2;
        number lx=x1-x0;
        number ly=y1-y0;
        number lz=z1-z0;      
        //number lx=_coorange[1][0]-_coorange[0][0];
        //number ly=_coorange[1][1]-_coorange[0][1];
        //number lz=_coorange[1][2]-_coorange[0][2];
        //number x=_coorange[0][0];
        //number y=_coorange[0][1];
        //number z=_coorange[0][2];
        geant d(-1);
        if(!(GMFFKEY.GammaSource==0)){
          number coox,cooy,cooz;
          //x0 = 0; // the following values are used to create the particle 
          //y0 = 0; // in the middle of the generating plane
          //z1 = 195; // to check orbit generation
          //lx = 0;
          //ly = 0;
          //lz = 0; 
         
          // generating plane has the orientation given by dir
          // transforming to AMS coordinates (coox,cooy,cooz) from generation plane local coordinates (x0+lx*RNDM(d),y0+ly*RNDM(d),0). z1=MCGEN[1][2] is the distance to the plane from AMS coordinate origin.
          //CAREFUL: MCGEN[0][2] is not used for gamma sources         
	  // MCGEN[1][2]=195 for std generation
	  //            coox=(z+lz)*cos(ph)*sin(th)+(x+lx*RNDM(d))*cos(th)*cos(ph)-(y+ly*RNDM(d))*sin(ph);
	  //            cooy=(z+lz)*sin(ph)*sin(th)+(x+lx*RNDM(d))*cos(th)*sin(ph)+(y+ly*RNDM(d))*cos(ph);
	  //            cooz=(z+lz)*cos(th)-(x+lx*RNDM(d))*sin(th);
          coox=z1*cos(ph)*sin(th)+(x0+lx*RNDM(d))*cos(th)*cos(ph)-(y0+ly*RNDM(d))*sin(ph);
          cooy=z1*sin(ph)*sin(th)+(x0+lx*RNDM(d))*cos(th)*sin(ph)+(y0+ly*RNDM(d))*cos(ph);
          cooz=z1*cos(th)-(x0+lx*RNDM(d))*sin(th);
          
          _coo=AMSPoint(coox,cooy,cooz);
      
	  // these transformations are obtained by adding:
	  // a translation in R=z1 cm 
	  // a rotation: theta around y; -phi around z
	  // the orientation was chosen so that cooy is coplanar with y

      
          if(GMFFKEY.GammaBg==1){ //to generate a background photon around the source
            number thprime,phprime; //in radians
            geant d(-1);
            phprime=2*AMSDBc::pi*RNDM(d);
            
            //this is to generate randomly between BgAngle and 0.
            thprime=1-pow(sin(GMFFKEY.BgAngle),2)*(double)RNDM(d);
            thprime=sqrt(thprime);
            thprime=acos(thprime);
            
        
            // direction vector respect to inclined plane
            number xprime=cos(phprime)*sin(thprime);
            number yprime=sin(phprime)*sin(thprime);
            number zprime=cos(thprime);
            
            // direction vector respect to AMS ref. system
            number xbg=(xprime*cos(th)+zprime*sin(th))*cos(ph)-yprime*sin(ph);
            number ybg=(xprime*cos(th)+zprime*sin(th))*sin(ph)+yprime*cos(ph);
            number zbg=-xprime*sin(th)+zprime*cos(th);
            
            xbg=xbg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2)); // to make unitary vector
            ybg=ybg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2));
            zbg=zbg/sqrt(pow(xbg,2)+pow(ybg,2)+pow(zbg,2));
            
            // now we rewrite the direction vector _dir 
            // from: "perpendicular to generating plane"(source direction) to "random direction around source"
            if(acos(zbg)<GMFFKEY.SourceVisib){
              _dir=AMSDir(-xbg,-ybg,-zbg);
            }
            else _dir=AMSDir(xbg,ybg,zbg);
          }
        } // ISN
        else if(_fixedplane==1){// to VC: what about 2(bot.plane) ???
	  //          cout << "fixed plane" << endl;
          _coo=AMSPoint(x0+lx*RNDM(d),y0+ly*RNDM(d),z0+lz);
        }
        else{
          _coo=AMSPoint(x0+lx*RNDM(d),y0+ly*RNDM(d),z0+lz*RNDM(d));
        }
      }//--->endof fixeddir+rand.point
    }//--->endof fixeddir+no_dirs_from_file
    else if(CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1>0){ //<---   Dir From files
      int k=floor(RNDM(d)*(CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1));
      _coo=_r_c[k];
      //   add 1cm of normal distribution 
      //   
      float dummy = 0;
      double t=2*3.1415926*RNDM(dummy);
      double r=sqrt(-0.2*log(RNDM(dummy)));
      _coo[0]+=r*cos(t);
      _coo[1]+=r*sin(t);
      _dir=_dir_c[k];
      //     cout <<" k "<<k<<" "<<CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1<<endl;
      //     cout <<_coo<<" "<<_dir<<endl;
    }//--->endof dirs_from_file
    else {   // <--- random dir
      geant d(-1);
      phi=2*AMSDBc::pi*RNDM(d);
      theta=sqrt((double)RNDM(d));//uniform cos**2
      theta=acos(theta);
      if(_fixedpoint){
        _dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
        _coo=_coorange[0];
        HCDIR(gPAWStandardDirectory, " ");
        return;
      }
     
      if(_fixedplane){
        curp=_fixedplane;
      }
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
      geant xin,yin;
      //

      static int init = 0;
      if (!init) {
	init = 1;
	cout<<"AMSmceventg::gener-I-coo= "
	    <<xa<<" "<<xb<<" "<<ya<<" "<<yb<<" "<<za<<" "<<zb<<" "
	    <<curp<<endl;
      }
     
      switch(curp){
      case 1:
        //
        // <-- try(if asked) to increas "from top" gener. effic.(Not for accept. calc.!!!)
	if(TFMCFFKEY.fast>0){
	  while(1){
	    xin=xa+lx*RNDM(d);
	    yin=ya+ly*RNDM(d);
	    phi=2*AMSDBc::pi*RNDM(d);
	    theta=sqrt((double)RNDM(d));
	    theta=acos(theta);
	    if(fastcheck(xin,yin,zb,theta,phi))break;
	  }
	  _coo=AMSPoint(xin,yin,zb);
	}
	else{
	  _coo=AMSPoint(xa+lx*RNDM(d),ya+ly*RNDM(d),zb);
	  //cout <<" coo "<<xa<<" "<<ya<<" "<<lx<<" "<<ly<<" "<<_coo<<endl;
	}
	//
	_dir=AMSDir(cos(phi)*sin(theta),sin(phi)*sin(theta),-cos(theta));
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
        abort();
      }//<--- end of switch
      //if(_fixedplane == 0)_coo=_coo/2;
      //
      // add cmuouns (low==2)
      //
      if(CCFFKEY.low==2){//sea-level muons
	if(_dir[2]>0)goto again;
	geant dummy = 0;
	number u=RNDM(dummy);
	if(u>=pow(fabs(_dir[2]),_gamma))goto again;
	//      cout <<"accepted  mom "<<_mom<<" "<<_gamma<<" "<<_ipart<<endl;
	//       cout << " _coo _dir "<< _dir <<" "<<_coo<<endl;
	AMSDBc::transform(_dir);
	AMSDBc::transform(_coo);
	//       cout << " _coo _dir trans"<< _dir <<" "<<_coo<<endl;

      }
    }//--->endof "random dir"
  }//--->endof "low!=2"
  HCDIR(gPAWStandardDirectory, " ");
}


void AMSmceventg::setspectra(integer begindate, integer begintime, 
                             integer enddate, integer endtime, 
                              integer ipart,  integer low){
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
// average values for 2011-2012 
 Orbit.DipoleR=567.946e5;                       // Dipole Shift Distance (cm)
  Orbit.DipoleTheta=22.596/AMSDBc::raddeg;       //              Latitude (rad)
  Orbit.DipolePhi=151.365/AMSDBc::raddeg;        //              Longitude (rad)
  Orbit.PoleTheta=-80.130/AMSDBc::raddeg;        // Dipole Direction Lat (rad)
  Orbit.PolePhiStatic=107.622/AMSDBc::raddeg;    //                  Lon (rad) 
//old - 1998 value
//   Orbit.DipoleR=534.259e5;                       // Dipole Shift Distance  (cm)
//  Orbit.DipoleTheta=21.687/AMSDBc::raddeg;       //              Latitude  (rad)
//  Orbit.DipolePhi=144.280/AMSDBc::raddeg;        //              Longitude (rad)
//  Orbit.PoleTheta=-79.397/AMSDBc::raddeg;        // Dipole Direction Lat   (rad)
//  Orbit.PolePhiStatic=108.392/AMSDBc::raddeg;    //                  Lon   (rad)
  Orbit.Nskip=0;
  Orbit.Nskip2=0;
  Orbit.Ntot=AMSIO::getntot();

  
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
  Orbit.InitTime=difftime(mktime(&Orbit.Begin),mktime(&Orbit.Init));


  if(Orbit.FlightTime <= 0){
    cerr <<"AMSmceventg::setspectra-F-FlighTime < 0 "<<Orbit.FlightTime<<endl;
    exit(1);
  }  

  if(AMSJob::gethead()->isSimulation()){
  if(ipart==0){
    if(IOPA.mode!=1){
      cerr << "AMSmceventg::setspectra-F-PARTNotDefined"<<endl;
      exit(1);
    }
    return;
  }
    //char cdir[256];
    //HCDIR(cdir,"R");
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
      if(ipart!=14){
        cerr<<"AMSMCeventg::setspectra-F-OnlyProtonsAreCurrentlySupprted"<<endl;
       abort();
      }
      integer nchan=500;
      geant binw;
      binw=40;
      geant al=binw/2;
      geant bl=binw/2+nchan*binw;
      
      HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
      for(int i=0;i<nchan;i++){
        geant xm=i*binw+al+binw/2;
        number xmom=xm/1000;
        number xkin=(sqrt(xmom*xmom+mass*mass)-mass);
        geant y=2.4*exp(-0.8*xkin)+100*exp(-4.26*xkin);
        if(xkin>0.005)HF1(_hid,xm,y);
     }
          
          HPRINT(_hid);
    }
    else if (low%1000==11){//Natural Spectrum
      integer nchan=200000;
      geant binw;
      if(mass < 0.938)binw=100;
      else  binw=100*mass/0.938;
      geant al=0;
      geant bl=nchan*binw;
      if(CCFFKEY.momrsp[0]<CCFFKEY.momrsp[1] && CCFFKEY.momrsp[0]>=0){
       al=CCFFKEY.momrsp[0]*990;
       bl=CCFFKEY.momrsp[1]*1010;
       binw=(bl-al)/nchan;
      }
      HBOOK1(_hid,"NaturalFlux",nchan,al,bl,0.);
      HBOOK1(_hid+1,"NaturalFlux",nchan,al,bl,0.);
      AMSEventR::hbook1(_hid,"NaturalFlux",nchan,al,bl);
      AMSEventR::hbook1(_hid+1,"NaturalFlux Generated GeV",nchan/20,al/1000,bl/1000);
 
       string option="";
       int opt=low/1000;
       if(opt==0){
        if(ipart>=47)option="flat";
       }
       else if(opt==1){
        if(ipart>=47)option="parabolic";
       }  
      double xmin=1e30;
      double xmax=0;
      for (int k=0;k<nchan;k++){
       int error=0;
       float mom_mev=(binw/2+k*binw);
       double r=NaturalFlux(ipart,mom_mev/charge/1000,option.c_str(),error);
       if(r>xmax)xmax=r;
       if(r<xmin && r>0)xmin=r;
       HF1(_hid,mom_mev,float(r));
       HF1(_hid+1,mom_mev,float(r));
       AMSEventR::hf1(_hid,mom_mev,r);
       if(error){
         cerr<<"AMSmceventg::setspectra-F-NotImplementedYet "<<error<<" "<<ipart<<" "<<mom_mev<<endl;
         abort();
       }
      }
      double rat=xmax/xmin;
      if(rat>2000000000){
         cerr<<"AMSmceventg::setspectra-E-SpectralRatioTooBig "<<xmax<<" "<<xmin<<" "<<rat<<endl;
      }
      else{
         cout<<"AMSmceventg::setspectra-I-SpectralRatioOK "<<xmax<<" "<<xmin<<" "<<rat<<endl;
      }
    }
    else if (low==0 || !(GMFFKEY.GammaSource==0)){//ISN
      integer nchan=200000;
      geant binw;
      if(mass < 0.938)binw=100;
      else  binw=100*mass/0.938;
      geant al=binw/2;
      geant bl=binw/2+nchan*binw;
      if(CCFFKEY.momrsp[0]<CCFFKEY.momrsp[1] && CCFFKEY.momrsp[0]>=0){
       al=CCFFKEY.momrsp[0]*990;
       bl=CCFFKEY.momrsp[1]*1010;
       binw=(bl-al)/nchan;
      }
      HBOOK1(_hid,"Spectrum",nchan,al,bl,0.);
      HBOOK1(_hid+1,"Spectrum",nchan,al,bl,0.);
     AMSEventR::hbook1(_hid,"NaturalFlux",nchan,al,bl);
     AMSEventR::hbook1(_hid+1,"NaturalFlux Generated GeV",nchan/20,al/1000,bl/1000);

      //
      // find a modulation
      //
      geant modul[18]={400.,350.,550.,650.,950.,1000.,1300.,1200.,1000.,900.,800.,600.,500.,500.,500.,600.,600.,600.};
      integer year=(begindate%10000+enddate%10000)/2-1997; 
      if(year <=0 || year > 17){
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
        number y;
        number xrig=z!=0?beta*xt/z:0;
        number constant=0.0137;
        number index=-2.10;
        if(ipart ==1&&(GMFFKEY.GammaSource==0||GMFFKEY.GammaBg==1)){//ISN
          // EGDB gamma-ray photon energy spec.(EGRET) per (m^2-sr-s-GeV)

          y= 0.0137/pow(xt,2.10);	            
        }
        else if((ipart==1)&&(!(GMFFKEY.GammaSource==0))&&GMFFKEY.GammaBg==0){

          lookupsourcesp(GMFFKEY.GammaSource,constant,index);
          y=constant*pow(xt,index);
          //ISN 
        }

        else if(ipart ==2){
          // positron          
          y=700./1.5/pow(xt,3.3)*(0.02+0.1/sqrt(xt));
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
          
          
        }
        else if(ipart==3){
          //electron
          y=700./1.5/pow(xt,3.3);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
        }
        else if (ipart ==14 ){
          y=1.7e4/beta/pow(xrig,2.80);
          y=y*(xkin*xkin+2*amass*xkin)/(xkm*xkm+2*amass*xkm);
          
        }    
        else if (ipart > 15 && ipart < 100){
          // He etc...
          y=.252e4/beta/pow(xrig,2.68);
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
        else if(!_fixedmom){
	
         cerr<<"AMSmceventg::setspectra-F-NoPrimarySpectraDefinedForParticle "<<ipart<<endl;
         exit(1);
        }
        else{
          HCDIR(gPAWStandardDirectory, " ");
          return;
        }
//      geomag transmittion

double par[3]={ -2.5489251E-0002,6.2432960E-0002, -9.056250E-0004};
double xr[2]={0.412,27};
//   add 
//   smearing 
//
double sm=CCFFKEY.momrsp[1]?CCFFKEY.momrsp[1]/CCFFKEY.momr[1]:1;
double trueRigidity=xm/1000.*sm;
double rm=par[0]+par[1]*xr[1]+par[2]*xr[1]*xr[1];
if( trueRigidity>xr[1]) trueRigidity=xr[1];
double fac=par[0]+par[1]*trueRigidity+par[2]*trueRigidity*trueRigidity;
if(fac<0)fac=0;
fac/=rm;
if(CCFFKEY.earth)fac=1;
 y*=fac;   
        
        HF1(_hid,xm,y);
        HF1(_hid+1,xm,y);
AMSEventR::hf1(_hid,xm,y);

      }//--->endof "nchan"-loop
    }//--->endof "low=0"
//
    else if(low==6){
    //read fluxfile
      char fname[161];
      UHTOC(CCFFKEY.FluxFile,40,fname,160);
      char fnam[256];
      strcpy(fnam,AMSDATADIR.amsdatadir);
      strcat(fnam,fname);  
      ifstream iftxt(fnam,ios::in);
      if(!iftxt){
       cerr <<"MCEventg::setcuts-F-UnableToOpenFluxFile "<<fnam<<endl;
       exit(1);
      }
      while(iftxt.get()!='Z'  && iftxt.good()){
      iftxt.ignore(1024,'%');
      }      
      if(iftxt.good()){
       iftxt.ignore(1024,'=');
       int zmin,zmax;
       iftxt>>zmin;
       iftxt.ignore(1024,'=');
       iftxt>>zmax;
       iftxt.ignore(1024,'%');
       if(iftxt.good() && iftxt.get()=='P'){
         iftxt.ignore(1024,'=');
         if(zmax-zmin+1>int(sizeof(_particle)/sizeof(_particle[0]))){
          cerr <<"MCEventg::setcuts-F-ToManySpectraIn "<<fnam<<endl;
          exit(1);
         }
         else {
          for(unsigned int i=0;i<sizeof(_particle)/sizeof(_particle[0]);i++){
           _particle[i]=-1;          
           _flux[i]=0;          
           _spectra[i]=0;          
           }
           HBOOK1(_hid,"Fluxes",zmax-zmin+1,0,zmax-zmin+1,0.);
          }
         for(int i=0;i<zmax-zmin+1;i++)iftxt>>_particle[i];
         iftxt.ignore(1024,'%');
         if(iftxt.good() && iftxt.get()=='N'){
          iftxt.ignore(1024,'=');
          for(int i=0;i<zmax-zmin+1;i++)iftxt>>_nucleons[i];
         }
         iftxt.ignore(1024,'%');
         iftxt.ignore(1024,'%');
         iftxt.ignore(1024,'\n');
         double emin,emax;
         int nbin;          
         iftxt>>emin;
         iftxt>>emax;
         iftxt>>nbin;
          double a=log(emin);
          double b=log(emax);
          double* ene=new double[nbin];
         for(int i=0;i<nbin;i++){
          ene[i]=exp(a+(b-a)*(float(i)/float(nbin-1)));
         }        
         double sum=0;
         for(int i=0;i<zmax-zmin+1;i++){
          _spectra[i]=new float[nbin];
          HBOOK1(_hid+i+1,"Spectra",nbin-1,a,b,0.);
         }
         int ntry=nbin/6;
         iftxt.ignore(1024,'\n');
         iftxt.ignore(1024,'\n');
         iftxt.ignore(1024,'\n');
         for (int i=0;i<zmax-zmin+1;i++){
         for(int j=0;j<ntry;j++){
           for(int k=0;k<6;k++){
            iftxt>>_spectra[i][j*6+k];

           }
           iftxt.ignore(1024,'\n');
         }
         for(int j=1;j<nbin;j++){
          double x=a+(b-a)/float(nbin-1)*float(j-1)+(b-a)/(nbin-1)/2.;
          double y=(_spectra[i][j]+_spectra[i][j-1])*0.5*(ene[j]-ene[j-1]);
          _flux[i]+=y;
           sum+=y;
          HF1(_hid+i+1,float(x),float(y));
         }
         cout <<"  AMSmceventg::setspectra-I-Charge "<<zmin+i<<" Flux "<<_flux[i]<< " "<<sum<<endl;
         HF1(_hid,float(i)+0.5,float(_flux[i]));
         iftxt.ignore(1024,'\n');
        }
        HPRINT(_hid);
        
        delete[] ene;
        if(!iftxt.good()){
         cerr <<"MCEventg::setspectra-F-ProblemsToReadSpectraIn "<<fnam<<endl;
         exit(1);
        }
        else{
         cout <<"MCEventg::setspectra-I-"<<zmax-zmin+1<<" SpectraReadIn "<<fnam<<endl;
        }
       }
       else{
       cerr <<"MCEventg::setspectra-F-ProblemsToFindParticlesIn "<<fnam<<endl;
       exit(1);

       }              
      }
      else{
       cerr <<"MCEventg::setspectra-F-ProblemsToReadFluxFile "<<fnam<<endl;
       exit(1);
      }
      for(unsigned int i=0;i<sizeof(_spectra)/sizeof(_spectra[0]);i++){
         if(_spectra[i])delete[] _spectra[i];
      }        

    }
    if(CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1>0){
      cout <<" AMSMCeventg::setspectra-I-PositionFileRequested "<<endl;
      _r_c=  new AMSPoint[CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1];
      _dir_c=new AMSDir[CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1];
    //read firfile
      char fname[161];
      UHTOC(CCFFKEY.DirFile,40,fname,160);
      char fnam[256];
      strcpy(fnam,AMSDATADIR.amsdatadir);
      strcat(fnam,fname);  
      ifstream iftxt(fnam,ios::in);
      if(!iftxt){
       cerr <<"MCEventg::setcuts-F-UnableToOpenDirFile "<<fnam<<endl;
       exit(1);
      }
      int ndir=0;
      iftxt>>ndir;
      iftxt.ignore(1024,'\n');
      if(ndir<CCFFKEY.DirFilePositions[1])CCFFKEY.DirFilePositions[1]=ndir;
      double acc[2]={0,0};
      while(iftxt.get()=='#'){
       iftxt.ignore(1024,'\n');
      }      
      for (int idir=0;idir<CCFFKEY.DirFilePositions[1];idir++){
       float x,y,z;
       double nx,ny,nz;
       int ipos;
       iftxt>>ipos;
       iftxt>>x;
       iftxt>>y;
       iftxt>>z;
       iftxt>>nx;
       iftxt>>ny;
       iftxt>>nz;
       iftxt>>acc[1];
//       cout <<x<<y<<z<<nx<<ny<<nz<<acc[1]<<endl;
       if(idir<CCFFKEY.DirFilePositions[0]){
         acc[0]=acc[1];
         continue;
       }
       _r_c[idir-CCFFKEY.DirFilePositions[0]]=AMSPoint(x,y,z);
       _dir_c[idir-CCFFKEY.DirFilePositions[0]]=AMSDir(nx,ny,nz);
      }
      iftxt.close();
      cout <<"AMCEventg::setspectra-I-"<<CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1<<" read "<<acc[1]-acc[0]<<" Acceptance "<<endl;
      
     }      

#ifdef __AMSDEBUG__
    //HPRINT(_hid);
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G4On)AMSRandGeneral::book(_hid);
#endif
     HCDIR(gPAWStandardDirectory," ");
  }
}
void AMSmceventg::lookupsourcesp(int sourceid, number & constant, number & index){
  switch(sourceid){
  case 2: //Crab approximate pulsed emission (at GeV, from EGRET)
    constant=226.2E-9;
    index=-2.1;
    break;
  case 3: //faint source at galactic center
    constant=5.E-9;
    index=-2.;
    break; 
  case 4: //Vela approximate pulsed emission (at GeV, from EGRET)
    constant=2100.E-9;
    index=-1.6;
    break;  
  default: // using average index for default;
    constant=100.2E-9;
    index=-2.;
  }
}

void AMSmceventg::lookupsourcecoo(int sourceid, geant & rasource, geant & decsource){
  switch(sourceid){ //if sourceid=GMFFKEY.GammaSource=1 --> user defined coo
  case 2: //Crab aproximate pulsed emission (at GeV, from EGRET)
    rasource=1.46;
    decsource=0.384;
    break;
  case 3: // faint source at galactic center
    rasource=4.65;
    decsource=-0.505;
    break;
  case 4: //Vela approximate pulsed emission (at GeV, from EGRET)
    rasource=2.24;
    decsource=-0.789;
    break;
  default: //Crab aproximate pulsed emission (at GeV, from EGRET)
    rasource=1.46;
    decsource=0.384;
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
    if(!(GMFFKEY.GammaSource==0))_fixeddir=1; //ISN 

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
    for ( i=0;i<6;i++)_planesw[i]=area[5]>0?area[i]/area[5]:(i+1.)/6.;

 

}

bool AMSmceventg::SpecialCuts(integer cut){
  if(cut ==1 ) {
   //  EcalFocusing
   static bool InitDone=false;
   static AMSPoint cross;
   if(!InitDone){
    AMSgvolume *ecal=AMSJob::gethead()->getgeomvolume(AMSID("ECMO",1));
    if(ecal){
     InitDone=true;
     AMSPoint cooA,par;
     for(int i=0;i<3;i++)cooA[i]=ecal->getcooA(i);
     for(int i=0;i<3;i++)par[i]=ecal->getpar(i);
      cross[2]=cooA[2]+par[2];
      cross[0]=par[0];
      cross[1]=par[1];
      cout <<" EcalFocusing-I-CrossingParameters: "<<cross<<endl;
    }
    else{
     cerr<<"AMSmceventg::SpecialCuts-S-NoEcalVulmefound "<<endl;
     return true;
    }   
   }
    if(_dir[2]){
     AMSPoint extrap=_coo+_dir*((cross[2]-_coo[2])/_dir[2]);
     if(fabs(extrap[0])<fabs(cross[0]) && fabs(extrap[1])<fabs(cross[1]))return true;
     else return false; 
    }
    else return false;
  }
  else if(cut==2){
    // TRD Top HC Focusing
   static bool InitDone=false;
   static number ZI,RI,ZO,RO;
   if(!InitDone){
    AMSgvolume *trdhc=AMSJob::gethead()->getgeomvolume(AMSID("TRD5",6));
    if(trdhc){
     InitDone=true;
     AMSPoint cooA;
     number par[10];
     for(int i=0;i<3;i++)cooA[i]=trdhc->getcooA(i);
     for(int i=0;i<10;i++)par[i]=trdhc->getpar(i);
      ZI=cooA[2]+par[4];
      RI=par[5];
      ZO=cooA[2]+par[7];
      RO=par[8];
      cout <<" TRDHCFocusing-I-CrossingParameters: "<<ZI<<" "<<RI<<" "<<ZO<<" "<<RO<<endl;
    }
    else{
     cerr<<"AMSmceventg::SpecialCuts-S-NoTRD5Volumefound "<<endl;
     return true;
    }   
   }
     AMSPoint extrapI=_coo+_dir*((ZI-_coo[2])/_dir[2]);
     AMSPoint extrapO=_coo+_dir*((ZO-_coo[2])/_dir[2]);
     if(extrapI[0]>RI && extrapO[0]>RO)return false;
     if(extrapI[0]<-RI && extrapO[0]<-RO)return false;
     if(extrapI[1]>RI && extrapO[1]>RO)return false;
     if(extrapI[1]<-RI && extrapO[1]<-RO)return false;
     if(extrapI[0]>RI)extrapI[0]=RI;
     if(extrapI[0]<-RI)extrapI[0]=-RI;
     if(extrapI[1]>RI)extrapI[1]=RI;
     if(extrapI[1]<-RI)extrapI[1]=-RI;
     if(extrapO[0]>RO)extrapO[0]=RO;
     if(extrapO[0]<-RO)extrapO[0]=-RO;
     if(extrapO[1]>RO)extrapO[1]=RO;
     if(extrapO[1]<-RO)extrapO[1]=-RO;
     number path=(extrapO-extrapI).norm();
     if(path<(RO+RI)/2){
      return false;
     }
     else return true; 
  }
  else if(cut==3){
   // solar panel - ams focusing
   static bool InitDone=false;
     static AMSDir n;
     static AMSPoint p;
     static AMSPoint ps;
     static AMSPoint amss;
   if(!InitDone){
    AMSgvolume *spa=AMSJob::gethead()->getgeomvolume(AMSID("SPA1",1));
    if(spa){
     InitDone=true;
     
     for(int i=0;i<3;i++)n[i]=spa->getnrmA(i,0);
     for(int i=0;i<3;i++)p[i]=spa->getcooA(i);
     for(int i=0;i<3;i++)ps[i]=spa->getpar(i);
    AMSgvolume *ams=AMSJob::gethead()->getgeomvolume(AMSID("FMOT",1));
     for(int i=0;i<3;i++)amss[i]=ams->getpar(i);
     }  
     else{ 
       cerr<<"AMSmceventg::SpecialCuts-S-NoSolarPanelVolumefound "<<endl;
       return true;
     }
   }
     // check if track pass by solar panel
     
     number ns=n.prod(_dir);
     if(ns){
      number t=(_coo-p).prod(n)/(ns);
      AMSPoint psec=_coo-_dir*t;
      if(t<0 && fabs(psec[1]-p[1])<ps[1] && fabs(psec[2]-p[2])<ps[2]){
       
         // check if pass in the vicinity of 0 
         number t=_coo.prod(_dir);
         if(t<0 && (_coo-_dir*t).norm()<amss[0]){
//           cout <<" passed 1 "<<(_coo-_dir*t).norm()<<endl;
           return true;
         }
         else return false;
      }
      else return false;
     } 
      else return true;

  }
  else if(cut>=4 && cut<=7){

      static AMSPoint cross1;
      static double par1[4]={63.14,48.4,158.920,67.14};
      static AMSPoint cross9;
      static double par9[4]={46.62 ,34.5,-135.882,67.14};
      static bool initdone=false;
      if(!initdone){
      cross1[0]=par1[0];
      cross1[1]=par1[1];
      cross1[2]=par1[2];
      cout <<" AMSmceventg::Specualcut-I-CrossingParametersLayer1: "<<cross1<<endl;
      cross9[0]=par9[0];
      cross9[1]=par9[1];
      cross9[2]=par9[2];
      cout <<" AMSmceventg::Specualcut-I-CrossingParametersLayer9: "<<cross9<<endl;
     }
      bool layer1=false;
      if(_dir[2]){
       AMSPoint extrap=_coo+_dir*((cross1[2]-_coo[2])/_dir[2]);
      if(fabs(extrap[0])<fabs(cross1[0]) && fabs(extrap[1])<fabs(cross1[1]) && sqrt(extrap[0]*extrap[0]+extrap[1]*extrap[1])<par1[3])layer1=true;
      }



      bool layer9=false;
      if(_dir[2]){
       AMSPoint extrap=_coo+_dir*((cross9[2]-_coo[2])/_dir[2]);
      if(fabs(extrap[0])<fabs(cross9[0]) && fabs(extrap[1])<fabs(cross9[1]) && sqrt(extrap[0]*extrap[0]+extrap[1]*extrap[1])<par9[3])layer9=true;
      }


      if(!initdone){
       if(cut==4)cout<<" AMSmceventg::Specualcut-I-Layer1FocusSelected"<<endl;
       else if (cut==5)cout<<" AMSmceventg::Specualcut-I-Layer9FocusSelected"<<endl;
     else if (cut==6)cout<<" AMSmceventg::Specualcut-I-Layer1Or9FocusSelected"<<endl;
     else cout<< " AMSmceventg::Specualcut-I-Layer1And9FocusSelected"<<endl;
    initdone=true;
    }
    if(cut==4)return layer1;
     else if (cut==5)return layer9;
     else if (cut==6)return (layer1 || layer9);
     else return (layer1 &&layer9);       
  }


/*

Removed by VC 2014.02.03 as no documentation for this codde exists
#ifdef _PGTRACK_
  if (cut == 5 || cut == 6) {
/// AMS-B Ecal/TRD-acceptance
    if (_dir.z() != 0) {
      static bool first = true;
      if (first) {
	first = false;
	cout << "SpecialCut: " << cut 
	     << " AMS-B full Tracker cut applied" << endl;
      }
      geant z1 = 155, z2 = -135, z0 = 0;
      AMSPoint pnt0 = _coo+_dir*(z0-_coo.z())/_dir.z();
      AMSPoint pnt1 = _coo+_dir*(z1-_coo.z())/_dir.z();
      AMSPoint pnt2 = _coo+_dir*(z2-_coo.z())/_dir.z();
      bool cut0 = (sqrt(pnt0.x()*pnt0.x()+pnt0.y()*pnt0.y()) < 55 && 
		                               fabs(pnt0.y()) < 40);
      bool cut1 = (sqrt(pnt1.x()*pnt1.x()+pnt1.y()*pnt1.y()) < 60);
      bool cut2 = (fabs(pnt2.x()) < 45 && fabs(pnt2.y()) < 30);
      if (!cut0) return false;
      if (cut == 5 && (cut1 && cut2)) return true;
      if (cut == 6 && (cut1 || cut2)) return true;
    }
    return false;
/// AMS-B Ecal/TRD-acceptance
  }
#endif
*/
     return true;
}

integer AMSmceventg::accept(){
  _nskip=Orbit.Ntot;
 if(CCFFKEY.low==8|| CCFFKEY.low==9 || CCFFKEY.low==10)  return 1;
  if(!(GMFFKEY.GammaSource==0) || CCFFKEY.DirFilePositions[1]-CCFFKEY.DirFilePositions[0]+1>0) return 1; //ISN
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
          if(SpecialCuts(CCFFKEY.SpecialCut%10)){
//        if(CCFFKEY.low || _fixeddir || _dir[2]<_albedocz || RNDM(d)< _albedorate)
            if((CCFFKEY.low==0 || CCFFKEY.low==6 || CCFFKEY.low==11)  && CCFFKEY.earth == 1 && !_fixeddir && !_fixedmom) 
            return EarthModulation();
            else if((CCFFKEY.low==0 || CCFFKEY.low==6 || CCFFKEY.low==11)  && CCFFKEY.earth == 2 && !_fixeddir && !_fixedmom){ 
            float theta=acos(_dir[2]);
            float phi=atan2(_dir[1],_dir[0]);
            float rig=_charge?_mom/fabs(_charge):10000;
            return NaturalFlux_stormer(theta,phi,  rig);
            }
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
//          else return EarthModulation();
          else return 1;
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
  number xfac=57.576*Orbit.EarthR/rgm*Orbit.EarthR/rgm;
  number chsgn=_charge/fabs(_charge);
  number cl3=cl*cl*cl;
  number cl4=cl*cl*cl*cl;
  number mom=CCFFKEY.Fast==0?
  xfac*cl4/(sqrt(1.-chsgn*cth*cl3)+1)/(sqrt(1.-chsgn*cth*cl3)+1)*fabs(_charge):
  xfac*cl4/4;
  if (_mom > mom)return 1;
  else {
  //cout <<xfac<<" "<<_mom<<" "<<mom<<endl;
  ++Orbit.Nskip;
   return 0;
  }
}
  
void AMSmceventg::gendelay(){

  // Generate a random delay for a track between 0 and 10 microsecs

  geant d(-1);
  _delay=RNDM(d)*10.;

}


void AMSmceventg::init(integer ipart){
_ipart=ipart;
_delay = 0.;
char chn[22]="";
integer itr=0;
geant amass=0;
geant charge=0;
geant tlife=0;
geant ubb[1];
integer one=1;
GFPART(abs(_ipart)%256,chn,itr,amass,charge,tlife,ubb,one);
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
#ifdef _PGTRACK_
    hman.Fill("Pgen", _mom, _dir[2]);
#endif
  }while(!accept());

#ifdef _PGTRACK_
  hman.Fill("Pacc", _mom, _dir[2]);
#endif
  
  // Set seed
#ifdef __G4AMS__
  if(!MISCFFKEY.G3On){
    _seed[0]=HepRandom::getTheSeeds()[0];
    _seed[1]=HepRandom::getTheSeeds()[1];
  }
  else
#endif
    GRNDMQ(_seed[0],_seed[1],0,"G");
  //   cout <<"seed[ "<<_seed[0]<<" "<<_seed[1]<<endl;
  //   cout << "  direction "<<_dir/_dir[2]<<" coo "<<_coo<<endl;
  vertex[0]=_coo[0];
  vertex[1]=_coo[1];
  vertex[2]=_coo[2];
   plab[0]=_mom*_dir[0];
   plab[1]=_mom*_dir[1];
   plab[2]=_mom*_dir[2];
//   printf("firing mom %f from %f %f %f dir %f %f %f\n",_mom,
//       _coo[0],_coo[1],_coo[2],
//       _dir[0],_dir[1],_dir[2]);
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


//============AMSVMC========= ZHili.Weng 2009-10-29

#ifdef __AMSVMC__
void AMSmceventg::runvmc(integer ipart, TVirtualMCStack* fStack,int vmc_version){
   init(ipart);
   geant plab[3],vertex[3];
   integer nvert=0;
   integer nt=0;

                     do{
           gener();
			         }while(!accept());

    // Set seed
if(vmc_version==2){
  _seed[0]=CLHEP::HepRandom::getTheSeeds()[0];
  _seed[1]=CLHEP::HepRandom::getTheSeeds()[1];
}
else
   GRNDMQ(_seed[0],_seed[1],0,"G");

    vertex[0]=_coo[0];
    vertex[1]=_coo[1];
    vertex[2]=_coo[2];
    plab[0]=_mom*_dir[0];
    plab[1]=_mom*_dir[1];
    plab[2]=_mom*_dir[2];

    cout<<"Primary particle mom:"<<plab[0]<<", "<<plab[1]<<", "<<plab[2]<<endl;
 // Track ID (filled by stack)
 int ntr;
 
 // Option: to be tracked
 int toBeDone = 1; 

 // PDG

 int pdg;
 TParticlePDG* particlePDG;
 if(vmc_version==1)
   {  
     pdg = geant3->PDGFromId(GCKINE.ikine);  
     cout<<"Primary particle is:"<<gMC->ParticleName(pdg)<<endl;
  particlePDG = TDatabasePDG::Instance()->GetParticle(pdg);
   }

 if(vmc_version==2)  {
   const char* pdgname = AMSJob::gethead()->getg4physics()->G3toG4(GCKINE.ikine);
   cout<<"Primary particle is:"<<pdgname<<endl;
 
  particlePDG = TDatabasePDG::Instance()->GetParticle(pdgname);
  cout<<"particlePDG defined"<<endl;
   pdg = particlePDG->PdgCode();
   cout<<"particlePDG defined,pdg:"<<pdg<<endl;
  }

 
 // Polarization
 Double_t polx = 0.; 
 Double_t poly = 0.; 
 Double_t polz = 0.; 

 // Position
 Double_t vx  = vertex[0];
 Double_t vy  = vertex[1]; 
 Double_t vz =  vertex[2];

 Double_t tof = 0.;

 Double_t px, py, pz, p0;

 px = plab[0];
 py = plab[1];
 pz = plab[2];
 p0 = sqrt(plab[0]*plab[0]+plab[1]*plab[1]+plab[2]*plab[2]);

 // Energy (in GeV)
       Double_t mass = particlePDG->Mass(); 
       Double_t e  = sqrt(mass*mass + p0*p0); 
       cout<<"Particle generated:"<<e<<"GeV"<<endl;

 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx,  vy,  vz, tof, polx, poly, polz,kPPrimary, ntr, 1., 0);

}

  void AMSmceventg::runvmc(TVirtualMCStack* fStack,int vmc_version){
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

    //   cout<<"DEBUG: about to generate particle in VMC Stack"<<endl;
   {

 // Track ID (filled by stack)
 int ntr;
 
 // Option: to be tracked
 int toBeDone = 1; 
 
 // PDG
 int pdg  = kElectron;     //To be Changed to convert geant3 coding to PDG coding
 TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(pdg);





 // Polarization
 Double_t polx = 0.; 
 Double_t poly = 0.; 
 Double_t polz = 0.; 

 // Position
 Double_t vx  = vertex[0];
 Double_t vy  = vertex[1]; 
 Double_t vz =  vertex[2];

 Double_t tof = 0.;

 Double_t px, py, pz, p0;

 px = plab[0];
 py = plab[1];
 pz = plab[2];
 p0 = sqrt(plab[0]*plab[0]+plab[1]*plab[1]+plab[2]*plab[2]);

 // Energy (in GeV)

 Double_t mass = particlePDG->Mass(); //To be Changed to convert geant3 coding to PDG coding 
 Double_t e  = sqrt(mass*mass + p0*p0); //To be Changed to convert geant3 coding to PDG coding

/// Create a new particle and push into stack;
/// adds it to the particles array (fParticles) and if not done to the 
/// stack (fStack).
/// Use TParticle::fMother[1] to store Track ID. 
/// \param toBeDone  1 if particles should go to tracking, 0 otherwise
/// \param parent    number of the parent track, -1 if track is primary
/// \param pdg       PDG encoding
/// \param px        particle momentum - x component [GeV/c]
/// \param py        particle momentum - y component [GeV/c]
/// \param pz        particle momentum - z component [GeV/c]
/// \param e         total energy [GeV]
/// \param vx        position - x component [cm]
/// \param vy        position - y component  [cm]
/// \param vz        position - z component  [cm]
/// \param tof       time of flight [s]
/// \param polx      polarization - x component
/// \param poly      polarization - y component
/// \param polz      polarization - z component
/// \param mech      creator process VMC code
/// \param ntr       track number (is filled by the stack
/// \param weight    particle weight
/// \param is        generation status code

 fStack->PushTrack(toBeDone, -1, pdg, px, py, pz, e, vx,  vy,  vz, tof, polx, poly, polz,kPPrimary, ntr, 1., 0);

   }   
   }
   
  }
#endif


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
#if  defined (__G4AMS__) || defined (__AMSVMC__)
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
#else
if(IOPA.VMCVersion==2){
#endif
 long seedl[3]={0,0,0};
 seedl[0]=_seed[0];
 seedl[1]=_seed[1];
 HepRandom::setTheSeeds(seedl);
}
else{
#endif
  if(_seed[1] !=0)GRNDMQ(_seed[0],_seed[1],1,"S");
  else if(_seed[0] > 0){
   integer ISEQ=_seed[0];
   GRNDMQ(_seed[0],_seed[1],ISEQ,"Q");
   GRNDMQ(_seed[0],_seed[1],ISEQ,"S");
  }
#if  defined (__G4AMS__) || defined (__AMSVMC__)
}
#endif
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
   if (Primary()){
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
}

void AMSmceventg::_writeEl(){
if( Out(_ipart>0 || IOPA.WriteAll%10==1 )){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  MCEventGNtuple02* GN = AMSJob::gethead()->getntuple()->Get_mcg02();

  if (GN->Nmcg>=MAXMCG02) return;
// added specifically to reduce ntuple size

// Fill the ntuple
  int i;
  GN->Nskip[GN->Nmcg]=_nskip;
  GN->Particle[GN->Nmcg]=_ipart;
  for(i=0;i<3;i++)GN->Coo[GN->Nmcg][i]=_coo[i];
  for(i=0;i<3;i++)GN->Dir[GN->Nmcg][i]=_dir[i];
  GN->Momentum[GN->Nmcg]=_mom;
  GN->Mass[GN->Nmcg]=_mass;
  GN->Charge[GN->Nmcg]=_charge;
  GN->Nmcg++;
*/
}
}


integer AMSmceventg::checkdaqid(int16u id){
if(id==getdaqid())return 1;
else return 0;
}

void AMSmceventg::builddaq(integer i, integer length, int16u *p){
  AMSmceventg *ptr=(AMSmceventg*)AMSEvent::gethead()->
  getheadC("AMSmceventg",0);
 p--;
 bool sec=false;
int l=1;
int maxl=1200;
while(ptr){ 
if(ptr->Primary() || !sec){
//  if(!ptr->Primary())sec=true;
if(l>maxl)sec=true;
if(l>maxl)break;
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
 p+=12;
 l+=12;
}
 ptr=ptr->next();
}
*(p+1)=getdaqid() ;

}


void AMSmceventg::buildraw(integer n, int16u *p){
 integer ip;
 geant mom;
 for(int16u *ptr=p;ptr<p+n-1;ptr+=12){ 
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
 AMSEvent::gethead()->getC("AMSmceventg");
int maxl=1200;
   integer l=1;
   bool sec=false;
    for(AMSmceventg* pm=
      (AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);pm;pm=pm->next()){
       if(l>maxl)break;
       if(pm->Primary())l+=12;
       else if(!sec){
        l+=12;
        if(l>maxl)sec=true;
       }
       
      }
 return -l;   //in jinj mode

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

integer orbit::UpdateOrbit(number xsec2, geant & ThetaS, geant & PhiS, geant & PolePhiS, number & RaS, number & DecS, number & GLatS, number & GLongS, time_t & time){ 
    static int first=0;

    double xsec=xsec2+InitTime;
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

    time=integer(mktime(&Begin)+xsec2); 
    ThetaS=theta;
    PolePhiS=pole;
    PhiS=fmod(phi+PhiZero+NodeSpeed*xsec,AMSDBc::twopi);
     ///// NodeSpeed is  ascending node precession due to oblateness
    number alt = AlphaAltitude;

    //number correction=fmod(1.848384291E-7*xsec,AMSDBc::twopi);
    //number correction=fmod(1.98694E-7*xsec,AMSDBc::twopi);
    //number correction=0;
    //number correction=fmod((6.27894582393474/(365*86400))*xsec,AMSDBc::twopi);
    number truephi=fmod(PhiS-(PolePhiS-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi);  
   
    skyposition isspos(ThetaS,truephi,alt,time); // calculate celestial position 

    isspos.GetRa(RaS); // Values are stored in radians through Get**
    isspos.GetDec(DecS); 
    isspos.GetLat(GLatS); 
    isspos.GetLong(GLongS); 
    if(first==0){
        first=1;
        float a=360/2/3.1415926;
        cout <<"AMSmceventg-I-OrbitPar "<<"Theta "<<ThetaS*a<<" Phi "<<PhiS*a<<" Pole "<<PolePhiS*a<<endl;
     }
    return cos(PhiS-PhiZero)>0?1:-1;
}
//ISN
orbit::orbit(geant Th,geant Ph, geant Pole, integer Dir):
ThetaI(Th),PolePhi(Pole){
  PhiI=fmod(Ph+AMSDBc::twopi,AMSDBc::twopi);
  const number MIR=51.65;
  AlphaTanThetaMax=tan(MIR/AMSDBc::raddeg);
  UpdateOrbit(ThetaI,PhiI,Dir);
  AlphaSpeed=AMSDBc::twopi/90.8/60.;
  EarthSpeed=AMSDBc::twopi/86400.; //ISN rad/s
  NodeSpeed=9.88E-7; // ISN ascending node precession due to oblateness
  Init.tm_year  =  111;
  Init.tm_mon = 04;
  Init.tm_mday   = 1;
  Init.tm_hour  = 12;
  Init.tm_min= 0;
  Init.tm_sec=0;
  Init.tm_isdst =  0;

}
void orbit::UpdateAxis(number vt, number vp, number t, number p){
  AMSDir ax1(AMSDBc::pi/2-t,p);
  AMSDir ax2(AMSDBc::pi/2-vt,vp);
  Axis=ax1.cross(ax2);
}

#ifdef __G4AMS__
#include "geant4.h"
  void AMSmceventg::runG4(integer ipart){
  if(ipart){
   init(ipart);
    do{
      gener();
#ifdef _PGTRACK_
    hman.Fill("Pgen", _mom, _dir[2]);
#endif
    }while(!accept());

#ifdef _PGTRACK_
  hman.Fill("Pacc", _mom, _dir[2]);
#endif

    // Set seed
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
_seed[0]=HepRandom::getTheSeeds()[0];
_seed[1]=HepRandom::getTheSeeds()[1];
}
else
#endif
   GRNDMQ(_seed[0],_seed[1],0,"G");
// cout <<"seed[ "<<_seed[0]<<" "<<_seed[1]<<endl;
// cout <<_coo<<" "<<_dir<<endl;
   AMSJob::gethead()->getg4generator()->SetParticleGun(_ipart,_mom,_coo,_dir);
  }
  else{
   if(acceptio()){
    AMSJob::gethead()->getg4generator()->SetParticleGun(_ipart,_mom,_coo,_dir);
   }   
  }
}

#endif

extern "C" void getscanfl_(int &scan);
void AMSmceventg::FillMCInfo(){
static number radl=0;
static number absl=0;
static integer init=0;
static number ECALZ=ECALDBc::ZOfEcalTopHoneycombSurface();
if(!init) init=1;

if(GCTRAK.sleng==0){
 radl=0;
 absl=0;
 
}
int scan=0;
getscanfl_(scan);
if(GCKINE.itra==1 && scan && GCKINE.ikine==GCKINE.ipart){
 radl+=GCTRAK.step/GCMATE.radl;
 absl+=GCTRAK.step/GCMATE.absl;
 char nvol[5]="EXIT";
 if(GCTRAK.inwvol==1 && GCVOLU.nlevel<500){
       for(int i=0;i<4;i++)nvol[i]=GCVOLU.names[GCVOLU.nlevel-1][i];
       AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);

 }
 else if(GCTRAK.inwvol==3){
       AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);
 }
}
if(GCKINE.itra==1 && CCFFKEY.SpecialCut/10 && GCKINE.ikine==GCKINE.ipart){
 char nvol[5]="EXIT";
 if(GCTRAK.inwvol==1 && GCTRAK.vect[2]<ECALZ+2){
       AMSContainer *p=AMSEvent::gethead()->getC("AMSmctrack",1);
        if(p && p->getnelem()==0 && fabs(GCTRAK.vect[0])<65/2. &&  fabs(GCTRAK.vect[1])<65/2. ){
          AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
          AMSEvent::gethead()->addnext(AMSID("AMSmctrack",1), genp);
       }
 }
}
     for (int i=0;i<GCKING.ngkine;i++){
       number mom=(GCKING.gkin[i][0]*GCKING.gkin[i][0]+
                       GCKING.gkin[i][1]*GCKING.gkin[i][1]+
                       GCKING.gkin[i][2]*GCKING.gkin[i][2]);
       mom=sqrt(mom);
       if(GCKIN3.GPOS[i][2]>ECALZ){
        AMSmceventg* genp=new AMSmceventg(-GCKING.gkin[i][4],mom,
        AMSPoint(GCKIN3.GPOS[i][0],GCKIN3.GPOS[i][1],GCKIN3.GPOS[i][2]),
        AMSDir(GCKING.gkin[i][0],GCKING.gkin[i][1],GCKING.gkin[i][2]));
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
       }
     }

}



#ifdef __G4AMS__
#include "job.h"
#include "g4physics.h"
void AMSmceventg::FillMCInfoG4( G4Track const * aTrack )
{
   /**
    *
    * Record *secondary* particles generated in the simulation 
    * no need for all the particles, e.g. particles in the ecal shower
    * so just follow the logic of G3 version [ AMSmceventg::FillMCInfo() ]
    *
    * tzc. Nov 2011.
    *
   **/

   //
   // simple postional cut value of ecal top
   //
   static double ECAL_Z = ECALDBc::ZOfEcalTopHoneycombSurface();
   //G4cerr << "ecal_z = " << ECAL_Z << G4endl;

   G4ThreeVector pos = aTrack->GetPosition();
   bool isPrimary = aTrack->GetParentID() == 0;
   G4ParticleDefinition * pdef = aTrack->GetDynamicParticle()->GetDefinition();
   //bool isNeutrino = abs(pdgid)==14 or abs(pddid)==16 or abs(pdgid)==18;
   bool isNeutrino = pdef->GetPDGCharge()==0 and pdef->GetLeptonNumber()!=0;

   G4double ekin = aTrack->GetKineticEnergy();
   ///G4clog << Form( "tracking  particle %s (%d<-%d) %g MeV @ ", name.c_str(), aTrack->GetTrackID(), aTrack->GetParentID(), aTrack->GetKineticEnergy() ) << pos/cm << G4endl;
   
   //
   // to save resources do a precut
   // skip the particles which ould be generated in ECAL shower, 
   // or particles with too small energy
   // or neutrino ( not too many, but their G3 ids are too large for plot
   //
   if( isPrimary or pos.z() < ECAL_Z*cm or ekin < 1*MeV or isNeutrino ) return;
   ///if( pdgid != -11 /*e+*/) return;

   ///G4clog << Form( "secondary particle %s (%d<-%d) @ ", name.c_str(), aTrack->GetTrackID(), aTrack->GetParentID() ) << pos/cm << G4endl;
   G4String name = aTrack -> GetDynamicParticle() -> GetDefinition() -> GetParticleName();
   G4ThreeVector mom = aTrack->GetMomentumDirection();

   // 
   // ams form of the track/particle information
   //
   int parinfo;
   int g3code = AMSJob::gethead()->getg4physics()->G4toG3( name,parinfo );
   if(g3code==AMSG4Physics::_G3DummyParticle)return;
   parinfo++;
   parinfo=-parinfo;
   float charge=pdef->GetPDGCharge();
   float mass=pdef->GetPDGMass()/GeV;
   AMSPoint point( pos.x(), pos.y(), pos.z() );
   AMSDir dir( mom.x(), mom.y(), mom.z() );

///--Create Process Record Adding by Q.Yan
   const G4VProcess *creator=aTrack->GetCreatorProcess();
   int nskip = creator ? (creator->GetProcessType() << 24) | (creator->GetProcessSubType() & 0xFFFFFF) : 0;
//---

   //
   // just record it
   //
   AMSEvent::gethead()->addnext(
         AMSID("AMSmceventg",0),
         new AMSmceventg( g3code,  aTrack->GetTrackID(), aTrack->GetParentID(),  ekin/GeV, point/cm, dir,parinfo,mass,charge,nskip) // negetive code for secondary
         );


   AMSG4EventAction* evt_act =(AMSG4EventAction*)G4EventManager::GetEventManager()->GetUserEventAction();
   evt_act->AddRegisteredTrack( aTrack->GetTrackID() );   
}
#endif




#ifdef __AMSVMC__
// AMSmceventg::FillMCInfoVMC  Still need to check    Zhili.Weng
void AMSmceventg::FillMCInfoVMC(int vmc_ipart,int vmc_inwvol,int CurrentLevel,const char* CurrentVolName,float* vmc_vect){
static number radl=0;
static number absl=0;
static integer init=0;
static number ECALZ=ECALDBc::ZOfEcalTopHoneycombSurface();
if(!init) init=1;

if(gMC->TrackLength()==0){
 radl=0;
 absl=0;
 
}
int scan=0;
getscanfl_(scan);

  
if(1+gMC->GetStack()->GetCurrentTrackNumber()==1 && scan && GCKINE.ikine==vmc_ipart){

  float _tmp_a,_tmp_z,_tmp_dens,_tmp_radl,_tmp_absl;     
  int CurrentMatId = gMC->CurrentMaterial(_tmp_a,_tmp_z,_tmp_dens,_tmp_radl,_tmp_absl);
  radl+=  (geant) gMC->TrackStep()/_tmp_radl;
  absl+=  gMC->TrackStep()/_tmp_absl;

 char nvol[5]="EXIT";
 if(vmc_inwvol==1 && CurrentLevel<500){
       for(int i=0;i<4;i++)nvol[i]=CurrentVolName[i];
       AMSmctrack* genp=new AMSmctrack(radl,absl,vmc_vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);

 }
 else if(vmc_inwvol==3){
       AMSmctrack* genp=new AMSmctrack(radl,absl,vmc_vect,nvol);
       AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);
 }
}

     for (int i=0;i<gMC->NSecondaries();i++){
       int sec_id;
       TLorentzVector sec_pos;
       TLorentzVector sec_mom;
       gMC->GetSecondary(i, sec_id, sec_pos, sec_mom);
       number  mom=sec_mom.P();
       if(sec_pos.Z()>ECALZ){
	 AMSmceventg* genp=new AMSmceventg(-sec_id,mom,
					   AMSPoint(sec_pos.X(),sec_pos.Y(),sec_pos.Z()),
					   AMSDir(sec_mom.Px(),sec_mom.Py(),sec_pos.Pz()));
	   AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
       }
     }

}
#endif






integer AMSmceventg::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSmceventg",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

bool AMSmceventg::Primary(){
 return _ipart>0 && _ipart<256;
}

void AMSmceventg::endjob(){
  HCDIR(gPAWGeneratorDirectory, " ");
    if(CCFFKEY.low==2){
     CMEND();
    }
  HCDIR(gPAWStandardDirectory," ");
}
//
integer AMSmceventg::fastcheck(geant xin, geant yin, geant zb, geant theta, geant phi){
  static integer first=1;
  geant zanti;
  geant ranti;
  geant zcal=ECALDBc::gendim(7);
  geant calhs=ECALDBc::gendim(1)/2.+ECMCFFKEY.safext;//cal.half-size(+ extention)
  geant dxy,xcr,ycr;
//
     zanti=ATGEFFKEY.scleng/2.;
     ranti=ATGEFFKEY.scradi;
     if(first){
       first=0;
       cout<<"AMSmceventg::fastcheck:z/r-anti="<<zanti<<" "<<ranti<<" zcal="<<zcal<<endl;
     }
     dxy=(zb-zanti)*tan(theta); // cr.with anti-top
     xcr=xin+dxy*cos(phi);
     ycr=yin+dxy*sin(phi);
     if((xcr*xcr+ycr*ycr)>(ranti*ranti))return 0;
     dxy=(zb+zanti)*tan(theta); // cr.with anti-bot
     xcr=xin+dxy*cos(phi);
     ycr=yin+dxy*sin(phi);
//     if((xcr*xcr+ycr*ycr)>(ranti*ranti))return 0;
     if(TFMCFFKEY.fast==2){//<-- check ECAL sens.volume if needed
       dxy=(zb-zcal)*tan(theta); // cr.with calor-top
       xcr=xin+dxy*cos(phi);
       ycr=yin+dxy*sin(phi);
       if((fabs(xcr)>calhs) || (fabs(ycr)>calhs))return 0;

//       dxy=(zb-ztof1t)*tan(theta); // cr.with tof1top
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)>xtof1) || (fabs(ycr)>ytof1))return 0;
       
//       dxy=(zb-ztof1b)*tan(theta); // cr.with tof1bot
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof1) && (fabs(ycr)<ytof1))return 0;
       
//       dxy=(zb-ztof2t)*tan(theta); // cr.with tof2top
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof2) && (fabs(ycr)<ytof2))return 0;
       
//       dxy=(zb-ztof2b)*tan(theta); // cr.with tof2bot
//       xcr=xin+dxy*cos(phi);
//       ycr=yin+dxy*sin(phi);
//       if((fabs(xcr)<xtof2) && (fabs(ycr)<ytof2))return 0;
     }
//
     TOF2JobStat::addmc(14);
     return 1;
}

void AMSmceventg::SaveSeeds(){
#if  defined (__G4AMS__) || defined (__AMSVMC__)
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
#else
if(IOPA.VMCVersion==2){
#endif
GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
}
else
#endif
   GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
}

void AMSmceventg::PrintSeeds(ostream & o){
long tmp[2];
tmp[0]=GCFLAG.NRNDM[0];
tmp[1]=GCFLAG.NRNDM[1];
SaveSeeds();
o<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
GCFLAG.NRNDM[0]=tmp[0];
GCFLAG.NRNDM[1]=tmp[1];
}
void AMSmceventg::RestoreSeeds(){
#if  defined (__G4AMS__) || defined (__AMSVMC__)
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
#else
if(IOPA.VMCVersion==2){
#endif
 long seedl[3]={0,0,0};
 seedl[0]=GCFLAG.NRNDM[0];
 seedl[1]=GCFLAG.NRNDM[1];
 HepRandom::setTheSeeds(seedl);
}
else
#endif
  GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
}


void AMSmceventg::SetSeed( int seed){

#if  defined (__G4AMS__) || defined (__AMSVMC__)
#ifdef __G4AMS__
if(!MISCFFKEY.G3On){
#else
if(IOPA.VMCVersion==2){
#endif
 HepRandom::setTheSeed(seed);
}
else{
#endif
   integer __seed[2];
   GRNDMQ(__seed[0],__seed[1],seed+1,"Q");
   GRNDMQ(__seed[0],__seed[1],seed+1,"S");
#if  defined (__G4AMS__) || defined (__AMSVMC__)
}

#endif


}



void AMSmctrack::_writeEl(){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple
/*
  MCTrackNtuple* GN = AMSJob::gethead()->getntuple()->Get_mct();

  if (GN->Nmct>=MAXMCVOL) return;
  GN->radl[GN->Nmct]=_radl;
  GN->absl[GN->Nmct]=_absl;
  for(int i=0;i<3;i++)GN->pos[GN->Nmct][i]=_pos[i];
  for(int i=0;i<4;i++)GN->vname[GN->Nmct][i]=_vname[i];
  GN->Nmct++;
*/
}



bool AMSmceventg::CalcLiveTime(int icase){
static vector<double>daqbuffer;
static number curtimeprev=0;  //last accepted event time
static unsigned int inittime=0;
static unsigned int DaqTotalLastSec[2]={0,0};
static unsigned int DaqAcceptedLastSec[2]={0,0};
static double ExposureTime=0;
static number curtime=0;
if(!CCFFKEY.earth)return true;
if(AMSEvent::gethead()){
unsigned int sec=AMSEvent::gethead()->gettime();
if(curtime==0){
inittime=sec;
}
unsigned int usec=AMSEvent::gethead()->getutime();
curtime=sec-inittime+usec/1000000000.;
}
else {
cerr <<"AMSmceventg::CalcLiveTime-E-NoEventDefined "<<endl;
return false;
}
number dt=curtime-curtimeprev;
DaqTotalLastSec[icase]++;
if(icase==0){
// fast trigger signal
//
  if( dt<LVTMFFKEY.MinFTime){
     return false;
  }
  else{
   DaqAcceptedLastSec[icase]++;
   return true;
  }
}
else if(icase==1){
// lvl1 trigger signal
//
  if( dt<LVTMFFKEY.MinTime){
     return false;
  }
 int processed=0;
 for(unsigned int i=0;i<daqbuffer.size();i++){
  if(curtime-daqbuffer[i]>LVTMFFKEY.MeanTime*(i+1)){
   processed=i+1;
  }
  else break;
 }
 for(int i=daqbuffer.size()-1;i>=0;i--){
  if(i<processed){
    daqbuffer.erase(daqbuffer.begin()+i);
  }
  else daqbuffer[i]+= LVTMFFKEY.MeanTime*processed;
 }
if(int(daqbuffer.size())>=LVTMFFKEY.BufSize){
     return false;
}
else if(daqbuffer.size()==0)daqbuffer.push_back(curtime);
else daqbuffer.push_back(daqbuffer[0]);

DaqAcceptedLastSec[icase]++;
ExposureTime+=dt;
_DT=(curtime-curtimeprev)*1000000;
curtimeprev=curtime;
if(ExposureTime>1){
 _Livetime=double(DaqAcceptedLastSec[0])/double(DaqTotalLastSec[0]+1);
 _Livetime*=double(DaqAcceptedLastSec[1])/double(DaqTotalLastSec[1]+1);
  for(int i=0;i<2;i++){
   DaqTotalLastSec[i]=0;
   DaqAcceptedLastSec[i]=0;
  }
  cout <<" AMSmceventg::CalcLiveTime-I- "<<_Livetime<<endl;
  ExposureTime=0;
}
 return true;
}
else return true;
}

number AMSmceventg::_Livetime=1;
number AMSmceventg::_DT=0;




#ifdef NATURAL_FLUX_TEST
// root.exe -l NaturalFlux.C+
#include "TF1.h"
#include "TCanvas.h"
#endif

/******************************************************************************/

/*
Given arrays x[1..n] and y[1..n] containing a tabulated function, i.e., yi = f(xi), with
x1 < x2 < .. . < xN, and given values yp1 and ypn for the first derivative of the interpolating
function at points 1 and n, respectively, this routine returns an array y2[1..n] that contains
the second derivatives of the interpolating function at the tabulated points xi. If yp1 and/or
ypn are equal to 1 × 1030 or larger, the routine is signaled to set the corresponding boundary
condition for a natural spline, with zero second derivative on that boundary.
*/

void AMSmceventg::NaturalFlux_spline(double x[], double y[], int n, double yp1, double ypn, double y2[])
{
 int i,k;
 double p,qn,sig,un;

 double u[n];
 if (yp1 > 0.99e30) // The lower boundary condition is set either to be “natural”
  y2[1]=u[1]=0.0; 
 else  // or else to have a specified first derivative.
 {
  y2[1] = -0.5;
  u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
 }

 for (i=2;i<=n-1;i++) { // This is the decomposition loop of the tridiagonal algorithm.
  // y2 and u are used for temporary storage of the decomposed factors.
  sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
  p=sig*y2[i-1]+2.0;
  y2[i]=(sig-1.0)/p;
  u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
  u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
 }
 if (ypn > 0.99e30) // The upper boundary condition is set either to be “natural”
  qn=un=0.0;
  else { // or else to have a specified first derivative.
   qn=0.5;
   un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
  }
  y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
  for (k=n-1;k>=1;k--) // This is the backsubstitution loop of the tridiagonal algorithm.
   y2[k]=y2[k]*y2[k+1]+u[k];


}
/******************************************************************************/

/*
Given the arrays xa[1..n] and ya[1..n], which tabulate a function (with the xai’s in order),
and given the array y2a[1..n], which is the output from NaturalFlux_spline above, and given a value of
x, this routine returns a cubic-spline interpolated value y.
*/

void AMSmceventg::NaturalFlux_splint(double xa[], double ya[], double y2a[], int n, double x, double *y)
{

 int klo,khi,k;
 double h,b,a;

 // We will find the right place in the table by means of
 // bisection. This is optimal if sequential calls to this
 // routine are at random values of x. If sequential calls
 // are in order, and closely spaced, one would do better
 // to store previous values of klo and khi and test if
 // they remain appropriate on the next call.

 klo=1; 
 khi=n;
 while (khi-klo > 1) {
  k=(khi+klo) >> 1;
  if (xa[k] > x) khi=k;
  else klo=k;
 } // klo and khi now bracket the input value of x.
 h=xa[khi]-xa[klo];
 if (h == 0.0) cerr<<"Bad xa input to routine NaturalFlux_splint\n"; // The xa’s must be distinct.
  a=(xa[khi]-x)/h; 
  b=(x-xa[klo])/h; // Cubic spline polynomial is now evaluated.
  *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}
/******************************************************************************/
#include <memory>
double AMSmceventg::NaturalFlux_TrueFluxFun_CubSpline(double * x, double * par)
{

 const double trueRigidity = x[0];
 if(trueRigidity<=0) return 1e-20;

 const double firstDerStart  = par[0];
 const double firstDerFinish = par[1];
 const int nPoints = par[2]+0.5;
 const int nextPar = 3;
 std::auto_ptr<double> xp(new double[nPoints+1]); // x-values
 std::auto_ptr<double> yp(new double[nPoints+1]); // y-values
 std::auto_ptr<double> sd(new double[nPoints+1]); // second derivatives
 for(int i=nextPar; i<(nPoints+nextPar); i++)
 {
      xp.get()[i-nextPar+1] = par[i];
      yp.get()[i-nextPar+1] = par[i+nPoints];
 }// end of for

 #ifdef NATURAL_FLUX_TEST
 const double factor = 1.0;
 #else
 const double factor = pow(trueRigidity,-2.7);
 #endif

 const double xMin = pow(10.0, xp.get()[1]),
                xMax = pow(10.0, xp.get()[nPoints]);

 if( xMin<trueRigidity && trueRigidity<xMax )
 {
      NaturalFlux_spline(xp.get(), yp.get(), nPoints, firstDerStart, firstDerFinish, sd.get());
      double result;
      NaturalFlux_splint(xp.get(), yp.get(), sd.get(), nPoints, log10(trueRigidity), &result);
      return factor * result;
 }// end of if

 else if(trueRigidity>=xMax)
 {
      NaturalFlux_spline(xp.get(), yp.get(), nPoints, firstDerStart, firstDerFinish, sd.get());
      double result_at_xMax;
      const double log10_xMax = xp.get()[nPoints];
      NaturalFlux_splint(xp.get(), yp.get(), sd.get(), nPoints, log10_xMax, &result_at_xMax);
      return factor * (result_at_xMax + firstDerFinish*(log10(trueRigidity)-log10_xMax));
 }// end of if

 else {
      NaturalFlux_spline(xp.get(), yp.get(), nPoints, firstDerStart, firstDerFinish, sd.get());
      double result_at_xMin;
      const double log10_xMin = xp.get()[1];
      NaturalFlux_splint(xp.get(), yp.get(), sd.get(), nPoints, log10_xMin, &result_at_xMin);

      const double alpha = firstDerStart/(xMin*log(10.0)) /
                             result_at_xMin *
                             xMin;
      return factor * result_at_xMin*pow(trueRigidity/xMin, alpha);
 }// end of block

}// end of function
/******************************************************************************/
double AMSmceventg::NaturalFlux(int gpid, double trueRigidity, const char *  acceptance, int &error){
//  error = 0 ok
//  error = 1 no such part
//  error = 2 no such acceptance 
error=0;
double ret=0;
double par[3]={ -2.5489251E-0002,6.2432960E-0002, -9.056250E-0004};
double xr[2]={0.412,27};
double rm=par[0]+par[1]*xr[1]+par[2]*xr[1]*xr[1];

double fac=par[0]+par[1]*trueRigidity+par[2]*trueRigidity*trueRigidity;
if(fac<0)fac=0;
if(fac>rm)fac=rm;
fac/=rm;
if(CCFFKEY.earth)fac=1;
if(gpid>=47 && gpid<=69){ // He to O Geant ID

           if( strcmp(acceptance,"flat")==0 )
           {
                static double par[] = { 2488.268552523628, 230.9280546822297, 
                                          4,
                                          0.3010299956639812, 1, 1.477121254719662, 2, 
                                          369.5532687414981, 2064.072100117866, 2455.854924375252, 2606.0412271868 };
                
               ret= NaturalFlux_TrueFluxFun_CubSpline(&trueRigidity, par);
           }// end of if
           else if( strcmp(acceptance,"parabolic")==0 )
           {
                static double par[] = { 2362.589734949727, 197.6460307149418, 
                                          4, 
                                          0.3010299956639812, 1, 1.477121254719662, 2, 
                                          351.2988657483565, 2048.492403848601, 2483.212224330382, 2649.087992727587 };
                ret=NaturalFlux_TrueFluxFun_CubSpline(&trueRigidity, par);
           }// end of if
else{

           cerr<<"NaturalFlux-E-UndefinedAcceptanceOption=\""<<acceptance<<"\" for gpid="<<gpid<<endl;
           error=2;
           return 0;
}

if(gpid>=61 && gpid<=66) {  // Secondaries

  double x=trueRigidity;
  double par[4] = {1.6835,-3.00849,-0.661628,0.0710779};
  ret*=par[0]*(x/(x-par[1]))*pow(x-par[1],par[2])+par[3];

}
return ret*fac;
}
else{
         cerr<<"AMSmceventg::NaturalFlux-E-UndefinedParticle="<<gpid<<endl;
         error=1;
         return 0;
}

}// end of function
/******************************************************************************/
#ifdef NATURAL_FLUX_TEST
double NaturalFlux(double * x, double * par) // for testing
{
 if(fabs(par[0]-1)<0.1) return NaturalFlux(47, x[0], "flat");
 if(fabs(par[0]-2)<0.1) return NaturalFlux(47, x[0], "parabolic");
 return 0.0;
}// end of function
/******************************************************************************/
void NaturalFlux(void)
{
 TF1 * f1 = new TF1("NaturalFlux", NaturalFlux, 0.1, 60000.0, 1);
 f1->SetMinimum(0.0);
 f1->SetMaximum(3800.0);
 f1->SetParameter(0,1); f1->SetLineColor(kRed);  f1->DrawCopy();       // He,flat
 f1->SetParameter(0,2); f1->SetLineColor(kBlue); f1->DrawCopy("same"); // He,parabolic
 gPad->GetCanvas()->SetLogx();
}// end of function
/******************************************************************************/
#endif




///Returns if an particle coming forma given direction in the AMS frame (Theta,Phi);
/// and with a given rigidity must be accepted or not
/// Rigidity in GV[.1, 10^4], theta[0, pi/2]  phi[0, pi] 
  // PZuccon-- MIT 2014
int AMSmceventg::NaturalFlux_stormer(float theta,float phi, float rig){

  if(!ratio){
    char fnam[256];
    char fname[400]="NaturalFluxStormer.root";
    strcpy(fnam,AMSDATADIR.amsdatadir);
    strcat(fnam,fname);  
    TFile* ff=TFile::Open(fname);
    TH3F* rr=(TH3F*)ff->Get("ratio");
    gROOT->cd();
    ratio= new TH3F(*rr);
    ff->Close();
  }
  
  float rig_log=log10(rig);

  int bx=ratio->GetXaxis()->FindBin(theta);
  int nbx=ratio->GetXaxis()->GetNbins();

  int by=ratio->GetYaxis()->FindBin(phi);
  int nby=ratio->GetYaxis()->GetNbins();

  int bz=ratio->GetZaxis()->FindBin(rig_log);
  int nbz=ratio->GetZaxis()->GetNbins();

  if(
     (bx<1|| bx>nbx) ||
     (by<1|| by>nby) ||
     (bz<1|| bz>nbz) )
    return 1;

  float th=ratio->GetBinContent(bx,by,bz);
  // printf("%10f %10f %10f %10f %3d %3d %3d  %10f\n",theta,phi,rig,rig_log,bx,by,bz,th);
  float d=1;
  float  tf=RNDM(d);
    
  if(tf<th) return 1;
  else return 0;
}
 
