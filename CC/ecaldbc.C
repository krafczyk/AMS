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
   70.,70.,    // i=1,2    dx at -Z,+Z
   70.,70.,    //  =3,4    dy at -Z,+Z
   16.,        //  =5      full thickness in Z
   0.,0.,      //  =6,7    center shift in x,y   
  -141.,       //  =8      front face Z-pozition (appr.61cm from TOF4)
   0.,0.       //  =9,10 spare
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
//    if(TOFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//    if(TOFCAFFKEY.cafdir==1)strcpy(fname,"");
//    strcat(fname,name);
//    cout<<"TOFDBc::readgconf: Open file : "<<fname<<'\n';
//    ifstream tcfile(fname,ios::in); // open needed config-file for reading
//    if(!tcfile){
//      cerr <<"TOFgeom-read: missing geomconfig-file "<<fname<<endl;
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
