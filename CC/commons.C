//  $Id: commons.C,v 1.220 2003/03/25 11:46:11 choutko Exp $


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
ATGEFFKEY_DEF ATGEFFKEY;
ATMCFFKEY_DEF ATMCFFKEY;
ATREFFKEY_DEF ATREFFKEY;
ATCAFFKEY_DEF ATCAFFKEY;
//=============================
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
RICFFKEY_DEF RICFFKEY;
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

char AMSCommonsI::_version[]="v4.00";
uinteger AMSCommonsI::_build=55;
uinteger AMSCommonsI::_os=0;
char AMSCommonsI::_osname[255];
AMSCommonsI::AMSCommonsI(){
  init();
}
#include <sys/utsname.h>
void AMSCommonsI::init(){ 
  if(_Count++==0){
   struct utsname u;
   uname(&u);
     if(strstr(u.sysname,"OSF1")){
      strcpy(_osname,"osf1");
      _os=1;
     }
     else if(strstr(u.sysname,"Linux")){
      strcpy(_osname,"linux");
      _os=2;
     }
     else if(strstr(u.sysname,"SunOS")){
      strcpy(_osname,"sunos");
      _os=3;
     }
     else {
      _os=0;
      _osname[0]='\0';
     }
     if(_os){
      cout <<"AMSCommonsI-I-HardwareIdentifiedAs "<< u.sysname<<" "<<u.machine<<endl;
     }
     else{
      cerr<<"AMSCommonsI-E-CouldNotMap "<<u.sysname<<" "<<u.machine<<endl;
      cerr<<"Production Job Will Be Aborted"<<endl;
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
