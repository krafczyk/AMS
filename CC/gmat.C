// Author V.Choutko.
// modified by E.Choumilov 20.06.96. - add some TOF materials.
// modified by E.Choumilov 1.10.99. - add some ECAL materials.
#include <gmat.h>
#include <snode.h>
#include <amsgobj.h>
#include <job.h>
#include <richdbc.h>
#ifdef __G4AMS__
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UnitsTable.hh"
#include <strstream.h>
#endif
integer AMSgmat::debug=0;
void AMSgmat::_init(){
#ifdef __AMSDEBUG__
  for( AMSNode *p=this->prev();p;p=p->prev()){
    if(!strcmp(getname(),p->getname())){
      cerr<<"AMSgmat-F-MaterialalrdyExists "<<getname()<<endl;
      exit(1);
    }
  }
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G3On){
#endif
   geant *a=new geant[_npar];
   geant *z=new geant[_npar];
   geant *w=new geant[_npar];
   for(int i=0;i<_npar;i++){
     a[i]=_a[i];
     w[i]=_w[i];
     z[i]=_z[i];
   }
   
  if(_npar == 1)   GSMATE(_imate,_name,a[0],z[0],_rho,_radl,_absl,_ubuf,1);

  else    GSMIXT(_imate,_name,a,z,_rho,-_npar,w);
   delete []a;
   delete []z;
   delete []w;
#ifdef __G4AMS__
}
if(MISCFFKEY.G4On){
 if(_npar==1 ){
  if(_temp==0)_pamsg4m= new G4Material(G4String(_name),_z[0],_a[0]*g/mole,_rho*g/cm3);
  else _pamsg4m= new G4Material(G4String(_name),_z[0],_a[0]*g/mole,_rho*g/cm3,kStateGas,_temp*kelvin,1.e-18*_rho*g/cm3/universe_mean_density*_temp*kelvin*pascal); 
 }
 else{
  char namz[255];
  ostrstream ost(namz,sizeof(namz));
  _pamsg4m= new G4Material(G4String(_name),_rho*g/cm3,_npar);
  for(int i=0;i<_npar;i++){
   ost.seekp(0);
   ost<<_name<<int(_z[i])<<ends;
   G4String name(namz);
   int natoms=int(_w[i]);
   _pamsg4m->AddElement(new G4Element(name," ",_z[i],_a[i]*g/mole),natoms);
  }
 }
}
#endif
}

      
ostream & AMSgmat::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSMATE" << endl);
}
integer AMSgtmed::debug=1;
void AMSgtmed::_init(){
#ifdef __AMSDEBUG__
  for( AMSNode *p=this->prev();p;p=p->prev()){
//    cout <<getname() <<" "<<p->getname()<<endl;
    if(!strcmp(getname(),p->getname())){
      cerr<<"AMSgtmed-F-MediaalrdyExists "<<getname()<<endl;
      exit(1);
    }
  }
#endif
  GSTMED(_itmed,_name,_pgmat->getmati(),_isvol,_ifield,_fieldm,_tmaxfd,
  _stemax,_deemax,_epsil,_stmin,_uwbuf,_nwbuf);
  if(_yb=='Y'){
   GSTPAR(_itmed,"BIRK1",_birks[0]);
   GSTPAR(_itmed,"BIRK2",_birks[1]);
   GSTPAR(_itmed,"BIRK3",_birks[2]);
  }
  if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd && _uwbuf[1]>1){
   GSTPAR(_itmed,"CUTGAM",1.01e-6);
  }
  if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd && _uwbuf[1]>0){
   if(IsSensitive()){
    GSTPAR(_itmed,"CUTELE",20.01e-6);
    GSTPAR(_itmed,"DCUTE",1.8e-6);
    GSTPAR(_itmed,"STRA",1);
   }
  }
  else if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd ){
   if(IsSensitive()){
    GSTPAR(_itmed,"CUTELE",35.01e-6);
   }
  }
}      
void AMSgtmed::setubuf(int nwbuf,geant ubuf[]){
 _nwbuf=nwbuf;
 if(_nwbuf){
   _uwbuf=new geant[_nwbuf];
   if(_uwbuf){
    for(int i=0;i<_nwbuf;i++)_uwbuf[i]=ubuf[i];
   }
 }
}
ostream & AMSgtmed::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSTMED" << endl);
}

uinteger AMSgmat::_GlobalMatI=0;
uinteger AMSgtmed::_GlobalMedI=0;

void AMSgmat::amsmat(){
static AMSgmat mat;
mat.setname("Materials:");
AMSJob::gethead()->addup(&mat);
mat.add (new AMSgmat("HYDROGEN",1.01, 1.,0.0708,865.,790.));
//mat.add (new AMSgmat("DEUTERIUM",  2.01,1.,0.162 ,757.,342.));
mat.add (new AMSgmat("HELIUM",  4.  , 2.,0.125 ,755.,478.));
//mat.add (new AMSgmat("LITHIUM",  6.94, 3.,0.534 ,155.,120.6));
//mat.add (new AMSgmat("BERILLIUM",  9.01, 4., 1.848 ,35.3,36.7));
mat.add (new AMSgmat("CARBON", 12.01, 6., 2.265 ,18.8,49.9));
//mat.add (new AMSgmat("NITROGEN", 14.01, 7.,0.808 ,44.5,99.4));
//mat.add (new AMSgmat("NEON", 20.18,10., 1.207 , 24.,74.9));
         // Half density alum
mat.add (new AMSgmat("ALUMINIUM",26.98, 13., 1.35, 17.8, 74.4));
mat.add (new AMSgmat("IRON", 55.85,26., 7.87  ,1.76,17.1));
mat.add (new AMSgmat("COPPER", 63.54,29.,8.96  ,1.43,14.8));
mat.add (new AMSgmat("TUNGSTEN",183.85,74., 19.3  ,0.35,10.3));
mat.add (new AMSgmat("LEAD",207.19,82., 11.35 ,0.56,18.5));
mat.add (new AMSgmat("URANIUM",238.03,92., 18.95 ,0.32,12. ));
mat.add (new AMSgmat("AIR",14.61,7.3, 0.001205,30423.24,67500.));
mat.add (new AMSgmat("MYLAR",  8.7   ,4.5,  1.39  ,28.7, 43.));
mat.add (new AMSgmat("MYLARTRD",  8.7   ,4.5,  1.39  ,28.7, 43.));
mat.add (new AMSgmat("VACUUM",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
mat.add (new AMSgmat("VACUUMTRD",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
geant a[]={20.18,12.01,1.01,0};
geant z[]={10.,6.,1.,0};
geant w[]={4.,1.,4.,0};
//mat.add (new AMSgmat("TPC GAS",a,z,w,3,0.8634e-3));
a[0]=39.95;a[1]=12.01;a[2]=1.01;
z[0]=18.;  z[1]=6.;   z[2]=1.;
w[0]=1.;   w[1]=2.;   w[2]=6.;
//mat.add (new AMSgmat("DRIFT GAS ",a,z,w,3,1.568e-3));
a[0]=12.01;a[1]=28.1;
z[0]=6.;  z[1]=14.;
w[0]=2.66;w[1]=1.;
mat.add (new AMSgmat("SICARBON",a,z,w,2,2.286));
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("SCINT",a,z,w,2,1.032));
mat.add (new AMSgmat("SILICON",28.09,14.,2.32,9.36,45.));
a[0]=144.24;a[1]=55.85;a[2]=10.8;
z[0]=60.;   z[1]=26.;  z[2]=5.;
w[0]=30.;   w[1]=69.;  w[2]=1.;
mat.add (new AMSgmat("MAGNET",a,z,w,3,7.45));
         // AL honeycomb structure for TOF :
mat.add (new AMSgmat( "AL-HONEYC",26.98, 13., 0.04, 600., 2660.));
     // effective material for PMT-boxes (low dens.(1:10) iron):
mat.add (new AMSgmat("LOW_DENS_Fe",55.85,26.,0.787,17.6,168.));
     // low density(1.3 eff) carbon (carb.fiber+mylar) for TOF sc_cover :
mat.add (new AMSgmat("CARBONF", 12.01, 6., 1.3 , 33., 66.));

// WLS for CTC (same polystiren as for TOF scint)
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("WLS",a,z,w,2,1.03));
//
// AL honeycomb structure for CTC (as for TOF now !) :
mat.add (new AMSgmat( "AL-HONEYC2",26.98, 13., 0.04, 600., 2660.));
//
// Teflon(C2F4) cover for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON1",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON2",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for ATC rho=0.3 gr/cm^3 :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON3",a,z,w,2, 0.3));
//
// Si-aerogel(SiO4H4) for CTC :
a[0]=28.; a[1]=16.; a[2]=1.;
z[0]=14.; z[1]=8.;  z[2]=1.;
w[0]=1.;  w[1]=4.;  w[2]=4.;
mat.add (new AMSgmat( "SIAEROGEL",a,z,w,3, 0.24));
//
// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
a[0]=28.; a[1]=16.; a[2]=12.; a[3]=1.;
z[0]=14.; z[1]=8.;  z[2]=6.; z[3]=1.;
w[0]=1.;  w[1]=2.;  w[2]=3.; w[3]=9.;
mat.add (new AMSgmat( "MEWAEROGEL",a,z,w,4, 0.125));

// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
{
  geant a[]={209.,72.61,16.};
  geant z[]={83.,32.,8.};
  geant w[]={4.,3.,12.};
   mat.add (new AMSgmat( "BGO",a,z,w,3, 7.1));
}


// AL honeycomb structure for Tracker (as for TOF now !) :
mat.add (new AMSgmat( "AL-HONEYC-Tr",26.98, 13., 0.04, 600., 2660.));

// Foam structure for Tracker  :
mat.add (new AMSgmat( "FOAM",12.01, 6., 0.1 , 425.82, 900.));
//




{

  // RICH materials by Carlos Delgado (CIEMAT) based on the official
  // rich simulation by Fernando Barao at al.


{ // Aerogel  
  geant a[]={28.09,16.0};
  geant z[]={14.,8.};
  geant w[]={1.,2.};

  mat.add(new AMSgmat("RICH_AEROGEL",a,z,w,2,.7368));  
}

{ // Mirrors: plexiglass
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("RICH_MIRRORS",a,z,w,3,1.16));
}

{ // PMTs
  geant a[]={28.,16.};
  geant z[]={14.,8.};
  geant w[]={1.,2.};
  mat.add (new AMSgmat("PMT_WINDOW",a,z,w,2,2.64));
}


{ // Absorber walls: plexiglass too... 
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("RICH_WALLS",a,z,w,3,1.16));

  // Carbon fiber for the aerogel support structure

  mat.add (new AMSgmat("RICH_CARBONF", 12.01, 6., 1.3 , 33., 66.));

  // Magnetic shielding
  mat.add (new AMSgmat("RICH_BSHIELD",55.85,26.,0.787,17.6,168.));
}



}
//----------------------------------------------------------
// Light lead for ECAL test :
{
mat.add (new AMSgmat("LIGHTLEAD",207.19,82., 6.36 ,1.,32.5));
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("ECSCINT",a,z,w,2,1.032));
}
//-------------------
// effective material for ECAL PMT-boxes (low dens.(1:10) iron):
mat.add (new AMSgmat("LOW_DENS_Fe_2",55.85,26.,0.787,17.6,168.));//tempor as for TOF
         // AL honeycomb structure for TOF :
mat.add (new AMSgmat( "EC-AL-HONEYC",26.98, 13., 0.04, 600., 2660.));//tempor as for TOF
//------------------
{ // Fiber wall(cladding+glue, ~ plexiglass):  
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("ECFPLEX",a,z,w,3,1.16));
}
//----------------------------------------------------------


{
 // TRD Materials by V. Choutko - probably wrong ones

  // HC 3% X0
geant rho=0.032;
mat.add (new AMSgmat( "TRDHC",26.98, 13., rho, 24./rho, 106/rho));

 // CF  (2.25 % X0)
rho=1.6;
mat.add (new AMSgmat("TRDCarbonFiber", 12.01, 6., rho , 42.7/rho, 86.3/rho));

// Gas (Xe/CO2) (80/20) 1.1% X0

{
   geant z[]={54.,6.,8.};
   geant a[]={131.3,12.,16.};
   geant w[]={8,2,4};
   mat.add (new AMSgmat("XECO2_80/20",a,z,w,3,5.1e-3));
}
// TRDFoam
//   just plain carbon with 0.08 0.4% X0

rho=0.08;
mat.add (new AMSgmat("TRDFoam", 12.01, 6., rho , 42.7/rho, 86.3/rho));

// TRD tube walls (kapton)  --> using mylar for the moment (1% X0)

//TRD Radiator (polypropylene )  6% X0

{
   geant z[]={6.,1.};
   geant a[]={12.,1.};
   geant w[]={1,2};
   mat.add (new AMSgmat("TRDRadiator",a,z,w,2,0.063));
}

}

// SRD Materials - almost certainly wrong
{
  // Wave length shifter -pure scintillator ?

   // Xtall (YAlO3)
  geant z[]={39,13,8};
  geant a[]={88.9,27.,16.};
  geant w[]={1,1,3};
  mat.add (new AMSgmat("YAlO3",a,z,w,3,5.55));

}
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPMATE(0);
#endif

AMSgObj::GTrMatMap.map(mat);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
cout <<"AMSgmat::amsmat-I-TotalOf "<<GetLastMatNo()<<" materials defined"<<endl;
}

void AMSgtmed::amstmed(){


// NowMedia

static AMSgtmed tmed;
tmed.setname("TrackingMedia:");
AMSJob::gethead()->addup(&tmed);


tmed.add (new AMSgtmed("AIR","AIR",0));
tmed.add (new AMSgtmed("MAGNET","MAGNET",0));

{
// vacuum has to be trd aware
 geant uwbuf[5];
 integer nwbuf=5;
#ifdef __G4AMS__
if (MISCFFKEY.G4On){
uwbuf[0]=0;
}
else{
#endif
 uwbuf[0]=TRDMCFFKEY.g3trd;
#ifdef __G4AMS__
}
#endif
 uwbuf[1]=TRDMCFFKEY.mode;
 if(TRDMCFFKEY.mode<2){
  uwbuf[3]=TRDMCFFKEY.cor;
  uwbuf[4]=0;
 }
 else{
  uwbuf[3]=TRDMCFFKEY.alpha;
  uwbuf[4]=TRDMCFFKEY.beta;
 }
 AMSgtmed * pvac=new AMSgtmed("VACUUM","VACUUMTRD",0);
 uwbuf[2]=6;
 pvac->setubuf(nwbuf,uwbuf);
 tmed.add (pvac );
}

int vac_med=GetLastMedNo();

tmed.add (new AMSgtmed("1/2ALUM","ALUMINIUM",0));
tmed.add (new AMSgtmed("ACTIVE_SILICON","SILICON",1));
tmed.add (new AMSgtmed("NONACTIVE_SILICON","SILICON"));
tmed.add (new AMSgtmed("CARBON","CARBON",0));
tmed.add (new AMSgtmed("ELECTRONICS","CARBON",0));
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed("TOF_SCINT","SCINT",1,'Y',birks,1,5,10,
                       -0.25, -1, 0.001, -0.05));
}
//(for tof_scint.: max_step=0.25cm/autom, min_step=0.05cm/autom )
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed("ANTI_SCINT","SCINT",1,'Y',birks));
}
//
tmed.add (new AMSgtmed("TOF_HONEYCOMB","AL-HONEYC",0));
tmed.add (new AMSgtmed("TOF_PMT_BOX","LOW_DENS_Fe",0));
tmed.add (new AMSgtmed("TOF_SC_COVER","CARBONF",0));
tmed.add (new AMSgtmed("IRON","IRON",0));
//
tmed.add (new AMSgtmed("CTC_WLS","WLS",1));
//
tmed.add (new AMSgtmed("CTC_HONEYCOMB","AL-HONEYC2",0));
//
tmed.add (new AMSgtmed("CTC_WALL","TEFLON1",0));
//
tmed.add (new AMSgtmed("CTC_SEP","TEFLON2",0));
//
tmed.add (new AMSgtmed("ATC_PTFE","TEFLON3",1));
//
tmed.add (new AMSgtmed("CTC_AEROGEL","SIAEROGEL",1));
//
tmed.add (new AMSgtmed("CTC_DUMMYMED","VACUUM",0));// fill all gaps inside CTC
//
tmed.add (new AMSgtmed("ATC_AEROGEL","MEWAEROGEL",1));
//
tmed.add (new AMSgtmed("ANTI_WRAP","MYLAR",0));//  tempor. mylar
//
tmed.add (new AMSgtmed("ANTI_SUPTB","CARBONF",0));//  tempor. carb.fiber
//
tmed.add (new AMSgtmed("TUNGSTEN","TUNGSTEN",0));
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed("BGO","BGO",1,'Y',birks));
}
int ip=27;
tmed.add (new AMSgtmed("Tr_Honeycomb","AL-HONEYC-Tr",0));//28
tmed.add (new AMSgtmed("Tr_Foam","FOAM",0));//29

tmed.add (new AMSgtmed("ATC_PTAE","TEFLON3",0));//30
tmed.add (new AMSgtmed("TOF_PMT_WINDOW","PMT_WINDOW",1));//31
//---------------
// RICH media

{

  // Cerenkov photon properties for the radiator

  geant index[RICmaxentries];
  geant abs_l[RICmaxentries];
  geant p[RICmaxentries];
  geant dummy[RICmaxentries];
  integer iw;
  
  for(iw=0;iw<44;iw++)
    {
      p[iw]=2*3.1415926*197.327e-9/RICHDB::wave_length[iw]; // Photon momentum in GeV
      dummy[iw]=1;
    }
  
  tmed.add (new AMSgtmed("RICH RAD","RICH_AEROGEL",0));   //32
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,RICHDB::abs_length,dummy,RICHDB::index);
  
  tmed.add (new AMSgtmed("RICH MIRRORS","RICH_MIRRORS",0));//33
  for(iw=0;iw<RICHDB::entries;iw++)
    abs_l[iw]=1.-0.9; // Reflectivity=90%
  index[0]=0;         // The mirror is a metal
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index);
  
  tmed.add (new AMSgtmed("RICH PMTS","PMT_WINDOW",1));   //35
  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=1e5;
      index[iw]=1.458;
    }
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index);


  tmed.add (new AMSgtmed("RICH CARBON","RICH_CARBONF",0));
  for(iw=0;iw<RICHDB::entries;iw++)
      abs_l[iw]=1.; 
  index[0]=0;
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index);

  tmed.add (new AMSgtmed("RICH WALLS","RICH_WALLS",0));
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index);

  tmed.add (new AMSgtmed("RICH SHIELD","RICH_BSHIELD",0));
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index); 

  tmed.add (new AMSgtmed("RICH GLUE","RICH_WALLS",1));
  GSCKOV(GetLastMedNo(),RICHDB::entries,p,abs_l,dummy,index);

  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=1e5;
      index[iw]=1;
    }
  GSCKOV(vac_med,RICHDB::entries,p,abs_l,dummy,index);  

}
//---------------
//  ECAL media
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed("EC_EFFRAD","LIGHTLEAD",0));// eff.radiator for fast sim(not sens !!!)
//tmed.add (new AMSgtmed("EC_RADIATOR","LEAD",0)); // 
tmed.add (new AMSgtmed("EC_RADIATOR","LEAD",0,'N',birks,2,20.,10.,1000.,
                                    -1.,0.001,-1.)); // simplif.tracking in magn.f
GSTPAR(GetLastMedNo(),"CUTGAM",ECMCFFKEY.cutge);// special cuts for EC_RADIATOR
GSTPAR(GetLastMedNo(),"CUTELE",ECMCFFKEY.cutge);
tmed.add (new AMSgtmed("EC_FCORE","ECSCINT",1,'Y',birks));//
GSTPAR(GetLastMedNo(),"CUTGAM",ECMCFFKEY.cutge);// special cuts for EC_FIBER-core
GSTPAR(GetLastMedNo(),"CUTELE",ECMCFFKEY.cutge);
tmed.add (new AMSgtmed("EC_FWALL","ECFPLEX",0));// 
GSTPAR(GetLastMedNo(),"CUTGAM",ECMCFFKEY.cutge);// special cuts for EC_FIBER-wall
GSTPAR(GetLastMedNo(),"CUTELE",ECMCFFKEY.cutge);
tmed.add (new AMSgtmed("EC_ELBOX","LOW_DENS_Fe_2",0));
GSTPAR(GetLastMedNo(),"CUTGAM",ECMCFFKEY.cutge);// special cuts for EC_ELBOX
GSTPAR(GetLastMedNo(),"CUTELE",ECMCFFKEY.cutge);
tmed.add (new AMSgtmed("EC_HONEYC","EC-AL-HONEYC",0));
GSTPAR(GetLastMedNo(),"CUTGAM",ECMCFFKEY.cutge);// special cuts for EC_HONEYC
GSTPAR(GetLastMedNo(),"CUTELE",ECMCFFKEY.cutge);
} 
//----------------

{ // TRD Media by V. Choutko  seems to be also wrong
geant uwbuf[5];
integer nwbuf=5;
#ifdef __G4AMS__
if (MISCFFKEY.G4On){
uwbuf[0]=0;
}
else{
#endif
uwbuf[0]=TRDMCFFKEY.g3trd;
#ifdef __G4AMS__
}
#endif
uwbuf[1]=TRDMCFFKEY.mode;
if(TRDMCFFKEY.mode<2){
uwbuf[3]=TRDMCFFKEY.cor;
uwbuf[4]=0;
}
else{
uwbuf[3]=TRDMCFFKEY.alpha;
uwbuf[4]=TRDMCFFKEY.beta;
}
tmed.add (new AMSgtmed("TRDHC","TRDHC",0));
tmed.add (new AMSgtmed("TRDCarbonFiber","TRDCarbonFiber",0));

uwbuf[2]=3;
AMSgtmed * pgas=new AMSgtmed("TRDGas","XECO2_80/20",1);
pgas->setubuf(nwbuf,uwbuf);
tmed.add (pgas);

AMSgtmed *pfoam = new AMSgtmed("TRDFoam","TRDFoam",0);
uwbuf[2]=2;
pfoam->setubuf(nwbuf,uwbuf);
tmed.add (pfoam);

AMSgtmed * pwall=new AMSgtmed("TRDCapton","MYLARTRD",0);
uwbuf[2]=2;
pwall->setubuf(nwbuf,uwbuf);
tmed.add (pwall);

uwbuf[2]=1;
AMSgtmed * prad=new AMSgtmed("TRDRadiator","TRDRadiator",0);
prad->setubuf(nwbuf,uwbuf);
tmed.add (prad);



}

{
  //SRD Media
tmed.add (new AMSgtmed("SRDWLS","SCINT",1));
tmed.add (new AMSgtmed("SRDXtall","YAlO3",1));
tmed.add (new AMSgtmed("SRDPMT","PMT_WINDOW",1));

}



//---------------
AMSgObj::GTrMedMap.map(tmed);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPTMED(0);
#endif
cout <<"AMSgmat::amstmed-I-TotalOf "<<GetLastMedNo()<<" media defined"<<endl;
}








