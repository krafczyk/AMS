// Author V.Choutko.
// modified by E.Choumilov 20.06.96. - add some TOF materials.
#include <gmat.h>
#include <snode.h>
#include <amsgobj.h>
#include <job.h>
integer AMSgmat::debug=0;
void AMSgmat::_init(){
   geant *a=new geant[_npar];
   geant *z=new geant[_npar];
   geant *w=new geant[_npar];
   for(int i=0;i<_npar;i++){
     a[i]=_a[i];
     if(_npar>1)w[i]=_w[i];
     z[i]=_z[i];
   }
   
  if(_npar == 1)   GSMATE(_imate,_name,a[0],z[0],_rho,_radl,_absl,_ubuf,1);

  else    GSMIXT(_imate,_name,a,z,_rho,-_npar,w);
   delete []a;
   delete []z;
   delete []w;
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
a[0]=12.01;a[1]=28.1;
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
     // low density(1.3 eff) carbon (carb.fiber+mylar) for TOF sc_cover :
mat.add (new AMSgmat(26,"CARBONF", 12.01, 6., 1.3 , 33., 66.));

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

// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
{
  geant a[]={209.,72.61,16.};
  geant z[]={83.,32.,8.};
  geant w[]={4.,3.,12.};
   mat.add (new AMSgmat( 34,"BGO",a,z,w,3, 7.1));
}


// AL honeycomb structure for Tracker (as for TOF now !) :
mat.add (new AMSgmat( 35,"AL-HONEYC-Tr",26.98, 13., 0.04, 600., 2660.));

// Foam structure for Tracker  :
mat.add (new AMSgmat( 36,"FOAM",12.01, 6., 0.1 , 425.82, 900.));
{
  geant a[]={28.,16.};
  geant z[]={14.,8.};
  geant w[]={1.,2.};
mat.add (new AMSgmat(37,"PMT_WINDOW",a,z,w,2,2.64));
}
//




{

  // RICH materials by Carlos Delgado (CIEMAT) based on the official
  // rich simulation by Fernando Barao at al.

// Cerenkov photon properties for the radiator


  // Wave length
geant wave_l[]={608.696, 605.797, 602.899, 600.000, 595.652, 591.304,
		585.507, 578.261, 573.913, 568.116, 563.768, 556.522,
		550.725, 543.478, 536.232, 528.986, 520.739, 511.594,
		502.899, 494.203, 482.609, 471.014, 459.42,  447.826,
		431.884, 426.087, 404.348, 391.304, 376.812, 369.565,
		355.012, 340.58,  328.986, 314.493, 304.348, 295.304, 
		288.406, 284.058, 279.71,  275.812, 272.464, 270.014,
		268.116, 266.667};


// Refractive index
geant index[]={1.136,   1.13602, 1.13605, 1.13608, 1.13612, 1.13617,
	       1.13623, 1.13631, 1.13635, 1.13642, 1.13647, 1.13656,
	       1.13663, 1.13672, 1.13681, 1.13691, 1.13703, 1.13717,
	       1.13731, 1.13745, 1.13766, 1.13787, 1.13811, 1.13837,
	       1.13875, 1.1389,  1.13953, 1.13996, 1.1405,  1.14079,
	       1.14144, 1.14217, 1.14284, 1.1438,  1.14456, 1.14532,
	       1.14596, 1.14639, 1.14684, 1.14727, 1.14766, 1.14795,
	       1.14818, 1.14837};


// Absortion length
geant abs_l[]={1372.78, 1346.82, 1321.23,    1296, 1258.84, 1222.49, 1175.25,
	       1118.14, 1084.89, 1041.71, 1010.19,  959.24,   919.9,  872.43,
	        826.82,  783.02,  735.33,  685.02,  639.62,  596.52,  542.48,
	         492.2,  445.49,   402.2,  347.91,   329.6,  267.31,  234.46,
	         201.6,  186.54,  158.84,  134.55,  117.14,   97.82,    85.8,
	         76.05,   69.19,   65.11,   61.21,   57.87,   55.11,   53.16,
	         51.68,   50.57};

// PMT efficiency
geant eff[]={1.296, 1.476, 1.717, 1.853, 2.041, 2.324, 2.646, 3.214, 3.504,
	     3.904, 4.350, 5.171, 5.518, 6.420, 7.153, 8.143, 9.271,10.330,
	    11.509,12.280,13.981,15.244,16.984,18.122,19.337,20.191,20.633,
	    20.633,20.633,20.633,20.633,20.010,18.923,17.355,16.266,14.918, 
	     13.682,11.509,10.555, 8.321, 7.153, 6.282, 6.148, 4.953};

geant p[44];
geant dummy[44];
integer iw;

for(iw=0;iw<44;iw++)
  {
    p[iw]=2*3.1415926*197.327e-9/wave_l[iw]; // Photon momentum in GeV
    dummy[iw]=1;
  }

{ // Aerogel  
  geant a[]={28.09,16.0};
  geant z[]={14.,8.};
  geant w[]={1.,2.};

  mat.add(new AMSgmat(38,"RICH_AEROGEL",a,z,w,2,.7368));
  
  // Define optical properties
  GSCKOV(38,44,p,abs_l,dummy,index);
}

{ // Mirrors: plexiglass
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat(39,"RICH_MIRRORS",a,z,w,3,1.16));

  //Define optical properties
  for(iw=0;iw<44;iw++)
      abs_l[iw]=1.-0.9; // Reflectivity=90%
  index[0]=0;
  GSCKOV(39,44,p,abs_l,dummy,index);
}

{ // PMTs
  geant a[]={28.,16.};
  geant z[]={14.,8.};
  geant w[]={1.,2.};
  mat.add (new AMSgmat(40,"PMT_WINDOW",a,z,w,2,2.64));

  // Define optical properties
  for(iw=0;iw<44;iw++)
    {
      abs_l[iw]=1e5;
      index[iw]=1.458;
    }
  GSCKOV(40,44,p,abs_l,dummy,index);
}


{ // Absorber walls: plexiglass too... 
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat(41,"RICH_WALLS",a,z,w,3,1.16));

  //Define optical properties
  for(iw=0;iw<44;iw++)
      abs_l[iw]=1.; // Reflectivity=90%
  index[0]=0;
  GSCKOV(41,44,p,abs_l,dummy,index);
}

// Define vaccum optical properties

for(iw=0;iw<44;iw++)
{
  abs_l[iw]=1e5;
  index[iw]=1;
}

GSCKOV(16,44,p,abs_l,dummy,index);
}
//-------------------
// Light lead for ECAL test :
{
mat.add (new AMSgmat(45,"LIGHTLEAD",207.19,82., 6.36 ,1.,32.5));
}
//-------------------
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
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed(10,"TOF_SCINT",21,1,'Y',birks,1,5,10,
                       -0.25, -1, 0.001, -0.05));
}
//(for tof_scint.: max_step=0.25cm/autom, min_step=0.05cm/autom )
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed(11,"ANTI_SCINT",21,1,'Y',birks));
}
//
tmed.add (new AMSgtmed(12,"TOF_HONEYCOMB",24,0));
tmed.add (new AMSgtmed(13,"TOF_PMT_BOX",25,0));
tmed.add (new AMSgtmed(14,"TOF_SC_COVER",26,0));
tmed.add (new AMSgtmed(15,"IRON",10,0));
//
tmed.add (new AMSgtmed(16,"CTC_WLS",27,1));
//
tmed.add (new AMSgtmed(17,"CTC_HONEYCOMB",28,0));
//
tmed.add (new AMSgtmed(18,"CTC_WALL",29,0));
//
tmed.add (new AMSgtmed(19,"CTC_SEP",30,0));
//
tmed.add (new AMSgtmed(20,"ATC_PTFE",31,1));
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
tmed.add (new AMSgtmed(26,"TUNGSTEN",12,0));
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed(27,"BGO",34,1,'Y',birks));
}
int ip=27;
tmed.add (new AMSgtmed(++ip,"Tr_Honeycomb",35,0));//28
tmed.add (new AMSgtmed(++ip,"Tr_Foam",36,0));//29

tmed.add (new AMSgtmed(++ip,"ATC_PTAE",31,0));//30
tmed.add (new AMSgtmed(++ip,"TOF_PMT_WINDOW",37,1));//31
//---------------
// RICH media

{
  tmed.add (new AMSgtmed(++ip,"RICH RAD",38,0));   //32
  tmed.add (new AMSgtmed(++ip,"RICH MIRRORS",39,0));//33
  tmed.add (new AMSgtmed(++ip,"RICH WALLS",41,0)); //34
  tmed.add (new AMSgtmed(++ip,"RICH PMTS",40,0));   //35
// DON'T use media numbers above 39 !!!!
}
//---------------
//  ECAL nedia
//
tmed.add (new AMSgtmed(40,"EC_DUMMYMOTH",45,0));// tempor for ECAL test(not sens !!!)
//---------------
AMSgObj::GTrMedMap.map(tmed);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPTMED(0);
#endif
}








