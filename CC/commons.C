//  $Id: commons.C,v 1.161 2001/06/26 15:07:13 choutko Exp $


#include <commons.h>
#include <stdlib.h>
#include <iostream.h>
#include <cern.h>
#include <amsdbc.h>
//
STATUSFFKEY_DEF STATUSFFKEY;
SELECTFFKEY_DEF SELECTFFKEY;
AMSFFKEY_DEF AMSFFKEY;
IOPA_DEF IOPA;
//=============================
TOFMCFFKEY_DEF TOFMCFFKEY;
TOFRECFFKEY_DEF TOFRECFFKEY;
TOFCAFFKEY_DEF TOFCAFFKEY;
//-------------
TFMCFFKEY_DEF TFMCFFKEY;
TFREFFKEY_DEF TFREFFKEY;
TFCAFFKEY_DEF TFCAFFKEY;
//=============================
MAGSFFKEY_DEF MAGSFFKEY;
//=============================
ECMCFFKEY_DEF ECMCFFKEY;
ECREFFKEY_DEF ECREFFKEY;
ECCAFFKEY_DEF ECCAFFKEY;
//=============================
ANTIGEOMFFKEY_DEF ANTIGEOMFFKEY;
ANTIMCFFKEY_DEF ANTIMCFFKEY;
ANTIRECFFKEY_DEF ANTIRECFFKEY;
ANTICAFFKEY_DEF ANTICAFFKEY;
//-------------
ATGEFFKEY_DEF ATGEFFKEY;
ATMCFFKEY_DEF ATMCFFKEY;
ATREFFKEY_DEF ATREFFKEY;
ATCAFFKEY_DEF ATCAFFKEY;
//=============================
CTCMCFFKEY_DEF CTCMCFFKEY;
CTCGEOMFFKEY_DEF CTCGEOMFFKEY;
CTCRECFFKEY_DEF CTCRECFFKEY;
CTCCAFFKEY_DEF CTCCAFFKEY;
//
TKGEOMFFKEY_DEF TKGEOMFFKEY;
TRMCFFKEY_DEF TRMCFFKEY;
TRDMCFFKEY_DEF TRDMCFFKEY;
TRDFITFFKEY_DEF TRDFITFFKEY;
TRDRECFFKEY_DEF TRDRECFFKEY;
TRDCLFFKEY_DEF TRDCLFFKEY;
MISCFFKEY_DEF MISCFFKEY;
BETAFITFFKEY_DEF BETAFITFFKEY;
CHARGEFITFFKEY_DEF CHARGEFITFFKEY;
TRFITFFKEY_DEF TRFITFFKEY;
CCFFKEY_DEF CCFFKEY;
TRCLFFKEY_DEF TRCLFFKEY;
TRCALIB_DEF TRCALIB;
TRALIG_DEF TRALIG;
G4FFKEY_DEF G4FFKEY;
//=============================
LVL1FFKEY_DEF LVL1FFKEY;
//---------------
TGL1FFKEY_DEF TGL1FFKEY;
//=============================
DAQCFFKEY_DEF DAQCFFKEY;
LVL3FFKEY_DEF LVL3FFKEY;
LVL3SIMFFKEY_DEF LVL3SIMFFKEY;
LVL3EXPFFKEY_DEF LVL3EXPFFKEY;
TRMFFKEY_DEF TRMFFKEY;
//
RICGEOM_DEF RICGEOM;
RICCONTROL_DEF RICCONTROL;
PRODFFKEY_DEF PRODFFKEY;
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
uinteger AMSCommonsI::_build=990;
uinteger AMSCommonsI::_os=0;
AMSCommonsI::AMSCommonsI(){
  init();
}
void AMSCommonsI::init(){ 
  if(_Count++==0){
   char* gtvb=getenv("BINTYPE");
   char* gtvh=getenv("HOSTTYPE");
   if( gtvh){
     if((strstr(gtvh,"alpha") || strstr(gtvh,"Digital")) && gtvb && strstr(gtvb,"OSF")){
      cout <<"AMSCommonsI-I-HardwareIdentifiedAs alpha-OSF"<<endl;
      _os=1;
     }
     else if((strstr(gtvh,"i386") || strstr(gtvh,"LINUX")) && ((gtvb && strstr(gtvb,"Linux"))|| strstr(gtvh,"linux")) ){
      cout <<"AMSCommonsI-I-HardwareIdentifiedAs i386-linux"<<endl;
      _os=2;
     }
     else {
      _os=0;
      cerr<<"AMSCommonsI-E-CouldNotMap "<<gtvh <<" "<<gtvb <<endl;
      cerr<<"Production Job Would Be Aborted"<<endl;
     }
   }
   else{
    cerr<<"AMSCommonsI-F-either $BINTYPE or $HOSTTYPE not defined "<<endl;
    abort();
   }
   char dt[128]="/afs/cern.ch/exp/ams/Offline/AMSDataDir";
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
          if(sizeof(int) <= sizeof(short int)){
         cerr<<"AMSTrIdSoftI-F-16 bit machine is not supported."<<endl;
         exit(1);
       }
       integer b64=0;
       if(sizeof(ulong)>sizeof(uinteger))b64=1;
       uinteger test1,test2;
       test1=1;
       test1+=2<<8;
       test1+=4<<16;
       test1+=8<<24;
       unsigned char *pt= (unsigned char*)&test1;
       test2=pt[0];
       test2+=pt[1]<<8;
       test2+=pt[2]<<16;
       test2+=pt[3]<<24;
       integer lend = test1==test2;
       if(lend)cout <<"AMSTrIdSoftI-I-Identified as LittleEndian";
       else {
         cout <<"AMSTrIdSoftI-I-Identified as BigEndian";
         AMSDBc::BigEndian=1;
       }
       if(b64)cout <<" 64 bit machine."<<endl;
       else cout <<" 32 bit machine."<<endl;
       AMSDBc dummy;
       AMSDBc::amsdatabase=new char[strlen(AMSDATADIR.amsdatabase)+1];
       strcpy(AMSDBc::amsdatabase,AMSDATADIR.amsdatabase);
  }
}
integer AMSCommonsI::_Count=0;
