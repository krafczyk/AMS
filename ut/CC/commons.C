

#include <commons.h>
#include <stdlib.h>
#include <iostream.h>
SELECTFFKEY_DEF SELECTFFKEY;
AMSFFKEY_DEF AMSFFKEY;
IOPA_DEF IOPA;
TOFMCFFKEY_DEF TOFMCFFKEY;
TOFRECFFKEY_DEF TOFRECFFKEY;
CTCMCFFKEY_DEF CTCMCFFKEY;
CTCGEOMFFKEY_DEF CTCGEOMFFKEY;
CTCRECFFKEY_DEF CTCRECFFKEY;
TRMCFFKEY_DEF TRMCFFKEY;
BETAFITFFKEY_DEF BETAFITFFKEY;
CHARGEFITFFKEY_DEF CHARGEFITFFKEY;
TRFITFFKEY_DEF TRFITFFKEY;
CCFFKEY_DEF CCFFKEY;
TRCLFFKEY_DEF TRCLFFKEY;


// Geant  commons

GCFLAG_DEF GCFLAG;
GCLINK_DEF GCLINK;
GCVOLU_DEF GCVOLU;
GCTMED_DEF GCTMED;
GCTRAK_DEF GCTRAK;
GCKINE_DEF GCKINE;

// Fortran commons
//TKFIELD_DEF TKFIELD;
//AMSDATADIR_DEF AMSDATADIR


AMSCommonsI::AMSCommonsI(){
  if(_Count++==0){
   char dt[128]="/afs/cern.ch/user/c/choutko/public/data/";
   char* gtv=getenv("AMSDataDir");
   if(gtv && strlen(gtv)>0){
    AMSDATADIR.amsdlength=strlen(gtv);
    if(AMSDATADIR.amsdlength>127)AMSDATADIR.amsdlength=127;
    strncpy(AMSDATADIR.amsdatadir,gtv,
    AMSDATADIR.amsdlength);
   }
   else {
     cout<<"AMSCommonsI-W-AMSDataDir variable is not defined."<<endl;
     cout <<" AMSCommonsI-W-Default value "<<
        dt<< " will be used."<<endl;
      strcpy(AMSDATADIR.amsdatadir,dt);
      AMSDATADIR.amsdlength=strlen(dt);
   }
  }
}

integer AMSCommonsI::_Count=0;
