// Author V.Choutko.
// modified by E.Choumilov 20.06.96. - add some TOF materials.
#include <gmat.h>
#include <snode.h>
#include <amsgobj.h>
#include <job.h>
integer AMSgmat::debug=0;
void AMSgmat::_init(){
  if(_npar == 1)   GSMATE(_imate,_name,_a[0],_z[0],_rho,_radl,_absl,_ubuf,1);

  else    GSMIXT(_imate,_name,_a,_z,_rho,-_npar,_w);

}      
ostream & AMSgmat::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSMATE" << endl);
}
integer AMSgtmed::debug=1;
void AMSgtmed::_init(){
  GSTMED(_itmed,_name,_itmat,_isvol,_ifield,_fieldm,_tmaxfd,
  _stemax,_deemax,_epsil,_stmin,_ubuf,1);
  if(_yb=='Y'){
   GSTPAR(_itmed,"BIRK1",_birks[0]);
   GSTPAR(_itmed,"BIRK2",_birks[1]);
   GSTPAR(_itmed,"BIRK3",_birks[2]);
  }
}      
ostream & AMSgtmed::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSTMED" << endl);
}


void AMSgmat::amsmat(){
static AMSgmat mat;
static AMSgtmed tmed;
mat.setname("Materials:");
AMSJob::gethead()->addup(&mat);
tmed.setname("TrackingMedia:");
AMSJob::gethead()->addup(&tmed);
mat.add (new AMSgmat(1,"HYDROGEN",1.01, 1.,0.0708,865.,790.));
mat.add (new AMSgmat(2,"DEUTERIUM",  2.01,1.,0.162 ,757.,342.));
mat.add (new AMSgmat(3,"HELIUM",  4.  , 2.,0.125 ,755.,478.));
mat.add (new AMSgmat( 4,"LITHIUM",  6.94, 3.,0.534 ,155.,120.6));
mat.add (new AMSgmat(5,"BERILLIUM",  9.01, 4., 1.848 ,35.3,36.7));
mat.add (new AMSgmat( 6,"CARBON", 12.01, 6., 2.265 ,18.8,49.9));
mat.add (new AMSgmat( 7,"NITROGEN", 14.01, 7.,0.808 ,44.5,99.4));
mat.add (new AMSgmat( 8,"NEON", 20.18,10., 1.207 , 24.,74.9));
         // Half density alum
mat.add (new AMSgmat( 9,"ALUMINIUM",26.98, 13., 1.35, 17.8, 74.4));
mat.add (new AMSgmat(10,"IRON", 55.85,26., 7.87  ,1.76,17.1));
mat.add (new AMSgmat(11,"COPPER", 63.54,29.,8.96  ,1.43,14.8));
mat.add (new AMSgmat(12,"TUNGSTEN",183.85,74., 19.3  ,0.35,10.3));
mat.add (new AMSgmat(13,"LEAD",207.19,82., 11.35 ,0.56,18.5));
mat.add (new AMSgmat(14,"URANIUM",238.03,92., 18.95 ,0.32,12. ));
mat.add (new AMSgmat(15,"AIR",14.61,7.3, 0.001205,30423.24,67500.));
mat.add (new AMSgmat(16,"VACUUM",1.E-16,1.E-16, 1.E-16,1.E+16,1.E+16));
mat.add (new AMSgmat(17,"MYLAR",  8.7   ,4.5,  1.39  ,28.7, 43.));

geant a[]={20.18,12.01,1.01,0};
geant z[]={10.,6.,1.,0};
geant w[]={4.,1.,4.,0};
mat.add (new AMSgmat(18,"TPC GAS",a,z,w,3,0.8634e-3));
a[0]=39.95;a[1]=12.01;a[2]=1.01;
z[0]=18.;  z[1]=6.;   z[2]=1.;
w[0]=1.;   w[1]=2.;   w[2]=6.;
mat.add (new AMSgmat(19,"DRIFT GAS ",a,z,w,3,1.568e-3));
a[0]=2.01;a[1]=28.1;
z[0]=6.;  z[1]=14.;
w[0]=2.66;w[1]=1.;
mat.add (new AMSgmat(20,"SICARBON",a,z,w,2,2.286));
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat(21,"SCINT",a,z,w,2,1.032));
mat.add (new AMSgmat(22,"SILICON",28.09,14.,2.32,9.36,45.));
a[0]=144.24;a[1]=55.85;a[2]=10.8;
z[0]=60.;   z[1]=26.;  z[2]=5.;
w[0]=30.;   w[1]=69.;  w[2]=1.;
mat.add (new AMSgmat(23,"MAGNET",a,z,w,3,7.45));
         // AL honeycomb structure for TOF :
mat.add (new AMSgmat( 24,"AL-HONEYC",26.98, 13., 0.04, 600., 2660.));
     // effective material for PMT-boxes (low dens.(1:10) iron):
mat.add (new AMSgmat(25,"LOW_DENS_Fe",55.85,26.,0.787,17.6,168.));
     // low density(1.2:2.265) carbon (carb.fiber) for TOF sc_cover :
mat.add (new AMSgmat(26,"CARBONF", 12.01, 6., 1.2 , 35.5, 72.));

// WLS for CTC (same polystiren as for TOF scint)
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat(27,"WLS",a,z,w,2,1.03));
//
// AL honeycomb structure for CTC (as for TOF now !) :
mat.add (new AMSgmat( 28,"AL-HONEYC2",26.98, 13., 0.04, 600., 2660.));
//
// Teflon(C2F4) cover for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( 29,"TEFLON1",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( 30,"TEFLON2",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for ATC rho=0.3 gr/cm^3 :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( 31,"TEFLON3",a,z,w,2, 0.3));
//
// Si-aerogel(SiO4H4) for CTC :
a[0]=28.; a[1]=16.; a[2]=1.;
z[0]=14.; z[1]=8.;  z[2]=1.;
w[0]=1.;  w[1]=4.;  w[2]=4.;
mat.add (new AMSgmat( 32,"SIAEROGEL",a,z,w,3, 0.24));
//
// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
a[0]=28.; a[1]=16.; a[2]=12.; a[3]=1.;
z[0]=14.; z[1]=8.;  z[2]=6.; z[3]=1.;
w[0]=1.;  w[1]=2.;  w[2]=3.; w[3]=9.;
mat.add (new AMSgmat( 33,"MEWAEROGEL",a,z,w,4, 0.125));




#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPMATE(0);
#endif



tmed.add (new AMSgtmed(1,"AIR",15,0));
tmed.add (new AMSgtmed(2,"MAGNET",23,0));
tmed.add (new AMSgtmed(4,"VACUUM",16,0));
tmed.add (new AMSgtmed(5,"1/2ALUM",9,0));
tmed.add (new AMSgtmed(6,"ACTIVE_SILICON",22,1));
tmed.add (new AMSgtmed(7,"NONACTIVE_SILICON",22));
tmed.add (new AMSgtmed(8,"CARBON",6,0));
tmed.add (new AMSgtmed(9,"ELECTRONICS",6,0));
//
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed(10,"TOF_SCINT",21,1,'Y',birks,1,5,10,
                       -0.25, -1, 0.001, -0.05));
//(for tof_scint.: max_step=0.25cm/autom, min_step=0.05cm/autom )
//
tmed.add (new AMSgtmed(12,"TOF_HONEYCOMB",24,0));
tmed.add (new AMSgtmed(13,"TOF_PMT_BOX",25,0));
tmed.add (new AMSgtmed(14,"TOF_SC_COVER",26,0));
tmed.add (new AMSgtmed(15,"IRON",10,0));
//
tmed.add (new AMSgtmed(11,"ANTI_SCINT",21,1,'Y',birks));
//
tmed.add (new AMSgtmed(16,"CTC_WLS",27,1));
//
tmed.add (new AMSgtmed(17,"CTC_HONEYCOMB",28,0));
//
tmed.add (new AMSgtmed(18,"CTC_WALL",29,0));
//
tmed.add (new AMSgtmed(19,"CTC_SEP",30,0));
//
tmed.add (new AMSgtmed(20,"ATC_PTFE",31,0));
//
tmed.add (new AMSgtmed(21,"CTC_AEROGEL",32,1));
//
tmed.add (new AMSgtmed(22,"CTC_DUMMYMED",16,0));// fill all gaps inside CTC
//
tmed.add (new AMSgtmed(23,"ATC_AEROGEL",33,1));
//
tmed.add (new AMSgtmed(24,"ANTI_WRAP",17,0));//  tempor. mylar
//
tmed.add (new AMSgtmed(25,"ANTI_SUPTB",26,0));//  tempor. carb.fiber
//
AMSgObj::GTrMedMap.map(tmed);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPTMED(0);
#endif
}


