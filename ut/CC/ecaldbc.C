// Author E.Choumilov 14.07.99.
#include <typedefs.h>
#include <math.h>
#include <commons.h>
#include <job.h>
#include <ecaldbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
//#include <tofsim.h>
//#include <tofrec.h>
//
//-----------------------------------------------------------------------
//  =====> ECALDBc class variables definition :
//
integer ECALDBc::debug=1;
//
//
// Initialize ECAL geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> default structural. data:
//
geant ECALDBc::_gendim[10]={
   70.,70.,16., // i=1-3  x,y,z-dimentions of EC-radiator
   4.,          //  =4    dx(dy) thickn.of (PMT+electronics)-volume
   0.,0.,       //  =5,6    center shift in x,y   
  -150.8,       //  =7      front face Z-pozition
   0.,0.,0.     //  =8-10   spare
};
//
geant ECALDBc::_fpitch[3]={
   0.135,      // i=1   fiber pitch in X(Y) projection
   0.165,0.2   // i=2,3 fiber pitch in Z (inside super-layer / between neigbour super-layers)
};
//
geant ECALDBc::_rdcell[10]={
   25.,100.,0.2, // i=1,3  fiber att. length 1st/2nd, % of 1st;
   0.1,          // i=4    fiber diameter
   0.9,          // i=5    size(dx=dz) of "1/4" of PMT-cathod
   0.45,         // i=6    abs(x(z)-position) of "1/4" in PMT coord.syst.
   2.57,2.6,     // i=7,8  X(Y)/Z-pitch of PMT's
   1.,1.         // i=9,10 direction of light readout in X/Y for 1st PMT (= +-1.)
};
//
integer ECALDBc::_slstruc[5]={
    1,           // i=1   1st super-layer projection(0->X, 1->Y)
   11,           // i=2   numb. of fiber-layers per super-layer
   8,            //  =3   numb. of super-layers (X+Y)
   25,           //  =4   numb. of PMT's per super-layers (in X(Y))
   0             //  =5   spare
};
//
integer ECALDBc::_nfibpl[2]={
   517,516       // i=1,2 numb. of fibers per 1st/2nd layer in super-layer
};
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from ECALgeom :
  void ECALDBc::readgconf(){
    int i;
    char fname[80];
    char name[80]="ecalgeom";
    char vers1[3]="01";
    char vers2[4]="001";
    char vers3[3]="02";
    if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE"))
    {
          cout <<" ECALGeom-I-Shuttle setup selected."<<endl;
          strcat(name,vers1);
    }
    else if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<" ECALGeom-I-AMS02 setup selected."<<endl;
          strcat(name,vers3);
    }
    else
    {
          cout <<" ECALGeom-I-AMS001 setup selected."<<endl;
          strcat(name,vers2);
    }
    strcat(name,".dat");
//    if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//    if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
//    strcat(fname,name);
//    cout<<"ECALDBc::readgconf: Open file : "<<fname<<'\n';
//    ifstream tcfile(fname,ios::in); // open needed config-file for reading
//    if(!tcfile){
//      cerr <<"ECgeom-read: missing geomconfig-file "<<fname<<endl;
//      exit(1);
//    }
//    for(int ic=0;ic<SCBLMX;ic++) tcfile >> _brtype[ic];
  }
//---
//
  geant ECALDBc::gendim(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 10);
      }
    #endif
    return _gendim[i-1];
  }
//
  geant ECALDBc::fpitch(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 3);
      }
    #endif
    return _fpitch[i-1];
  }
//
  geant ECALDBc::rdcell(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 10);
      }
    #endif
    return _rdcell[i-1];
  }
//
  integer ECALDBc::slstruc(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 5);
      }
    #endif
    return _slstruc[i-1];
  }
//
  integer ECALDBc::nfibpl(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 2);
      }
    #endif
    return _nfibpl[i-1];
  }
//
//==========================================================================
//
//   EcalJobStat static variables definition (memory reservation):
//
integer EcalJobStat::mccount[ECJSTA];
integer EcalJobStat::recount[ECJSTA];
// function to print Job-statistics at the end of JOB(RUN):
//
//------------------------------------------
void EcalJobStat::clear(){
  int i,j;
  for(i=0;i<ECJSTA;i++)mccount[i]=0;
  for(i=0;i<ECJSTA;i++)recount[i]=0;
}
//------------------------------------------
void EcalJobStat::printstat(){
//
  printf("\n");
  printf("    ====================== ECAL JOB-statistics ======================\n");
  printf("\n");
  printf(" MC: entries               : % 6d\n",mccount[0]);
  printf(" MC: MCHits->RawEvent OK   : % 6d\n",mccount[1]);
  printf(" RECO-entries              : % 6d\n",recount[0]);
  printf("\n\n");
//
}
//------------------------------------------
void EcalJobStat::bookhist(){
  int i,j,k,ich;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
}
//-----------------------------
void EcalJobStat::bookhistmc(){
    if(ECMCFFKEY.mcprtf!=0){ // Book mc-hist
      HBOOK1(ECHIST+1,"Geant-hits (tot)",80,0.,8000.,0.);
      HBOOK1(ECHIST+2,"Total edep(MeV)",80,0.,1600,0.);
      HBOOK1(ECHIST+3,"Aver. geant-time (ns)",80,0.,40.,0.);
    }
}
//----------------------------
void EcalJobStat::outp(){
}
//----------------------------
void EcalJobStat::outpmc(){
    if(ECMCFFKEY.mcprtf!=0){ // print MC-hists
      HPRINT(ECHIST+1);
      HPRINT(ECHIST+2);
      HPRINT(ECHIST+3);
    }
}
//==========================================================================
