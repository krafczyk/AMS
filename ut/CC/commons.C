

#include <commons.h>
#include <stdlib.h>
#include <iostream.h>
//
STATUSFFKEY_DEF STATUSFFKEY;
SELECTFFKEY_DEF SELECTFFKEY;
AMSFFKEY_DEF AMSFFKEY;
IOPA_DEF IOPA;
//
TOFMCFFKEY_DEF TOFMCFFKEY;
TOFRECFFKEY_DEF TOFRECFFKEY;
TOFCAFFKEY_DEF TOFCAFFKEY;
//
ECMCFFKEY_DEF ECMCFFKEY;
ECREFFKEY_DEF ECREFFKEY;
//ECCAFFKEY_DEF ECCAFFKEY;
//
ANTIGEOMFFKEY_DEF ANTIGEOMFFKEY;
ANTIMCFFKEY_DEF ANTIMCFFKEY;
ANTIRECFFKEY_DEF ANTIRECFFKEY;
ANTICAFFKEY_DEF ANTICAFFKEY;
//
CTCMCFFKEY_DEF CTCMCFFKEY;
CTCGEOMFFKEY_DEF CTCGEOMFFKEY;
CTCRECFFKEY_DEF CTCRECFFKEY;
CTCCAFFKEY_DEF CTCCAFFKEY;
//
TKGEOMFFKEY_DEF TKGEOMFFKEY;
TRMCFFKEY_DEF TRMCFFKEY;
MISCFFKEY_DEF MISCFFKEY;
BETAFITFFKEY_DEF BETAFITFFKEY;
CHARGEFITFFKEY_DEF CHARGEFITFFKEY;
TRFITFFKEY_DEF TRFITFFKEY;
CCFFKEY_DEF CCFFKEY;
TRCLFFKEY_DEF TRCLFFKEY;
TRCALIB_DEF TRCALIB;
TRALIG_DEF TRALIG;
G4FFKEY_DEF G4FFKEY;
LVL1FFKEY_DEF LVL1FFKEY;
DAQCFFKEY_DEF DAQCFFKEY;
LVL3FFKEY_DEF LVL3FFKEY;
LVL3SIMFFKEY_DEF LVL3SIMFFKEY;
LVL3EXPFFKEY_DEF LVL3EXPFFKEY;
TRMFFKEY_DEF TRMFFKEY;
//
RICGEOM_DEF RICGEOM;


// Geant  commons

GCFLAG_DEF GCFLAG;
GCLINK_DEF GCLINK;
GCVOLU_DEF GCVOLU;
GCTMED_DEF GCTMED;
GCTRAK_DEF GCTRAK;
GCKINE_DEF GCKINE;

// Fortran commons
// TKFIELD_DEF TKFIELD;
// AMSDATADIR_DEF AMSDATADIR

char AMSCommonsI::_version[]="v3.00";
integer AMSCommonsI::_build=801;
AMSCommonsI::AMSCommonsI(){
  init();
}
void AMSCommonsI::init(){ 
  if(_Count++==0){
   char dt[128]="/afs/cern.ch/user/c/choutko/public/AMSDataDir";
   char* gtv=getenv("AMSDataDir");
   if(gtv && strlen(gtv)>0){
    AMSDATADIR.amsdlength=strlen(gtv)+strlen(getversion())+2;
    if(AMSDATADIR.amsdlength>127){
      cerr <<"AMSCommonsI::init-F-AMSDataDirLength>127 "<<
        AMSDATADIR.amsdlength<<endl;
       exit(1);  
    }
      strcpy(AMSDATADIR.amsdatadir,gtv);
      strcat(AMSDATADIR.amsdatadir,"/");
      strcat(AMSDATADIR.amsdatadir,getversion());
      strcat(AMSDATADIR.amsdatadir,"/");
      AMSDATADIR.amsdlength=strlen(AMSDATADIR.amsdatadir);
      strcpy(AMSDATADIR.amsdatabase,gtv);
      strcat(AMSDATADIR.amsdatabase,"/");
      strcat(AMSDATADIR.amsdatabase,"DataBase");
      strcat(AMSDATADIR.amsdatabase,"/");
      AMSDATADIR.amsdblength=strlen(AMSDATADIR.amsdatabase);
     
   }
   else {
     cout<<"AMSCommonsI-W-AMSDataDir variable is not defined."<<endl;
     cout <<"AMSCommonsI-W-Default value "<<
        dt<< " will be used."<<endl;
      strcpy(AMSDATADIR.amsdatadir,dt);
      strcat(AMSDATADIR.amsdatadir,"/");
      strcat(AMSDATADIR.amsdatadir,getversion());
      strcat(AMSDATADIR.amsdatadir,"/");
      AMSDATADIR.amsdlength=strlen(AMSDATADIR.amsdatadir);
      strcpy(AMSDATADIR.amsdatabase,dt);
      strcat(AMSDATADIR.amsdatabase,"/");
      strcat(AMSDATADIR.amsdatabase,"DataBase");
      strcat(AMSDATADIR.amsdatabase,"/");
      AMSDATADIR.amsdblength=strlen(AMSDATADIR.amsdatabase);
      
   }
  }
}
integer AMSCommonsI::_Count=0;
