#include "root_setup.h"
#include "root.h"
#include <fstream.h>
#ifndef __ROOTSHAREDLIBRARY__
#include "commonsi.h"
#endif
AMSSetupR* AMSSetupR::_Head=0;
void AMSSetupR::CreateBranch(TTree *tree, int branchSplit){
  if(tree){
    _Head=this;
    tree->Branch("run.","AMSSetupR",&_Head,12800000,branchSplit);
    TBranch * branch=tree->GetBranch("run.");
    branch->SetCompressionLevel(6);
    cout <<" SetupR CompressionLevel "<<branch->GetCompressionLevel()<<" "<<branch->GetSplitLevel()<<endl;
  }
}

void AMSSetupR::UpdateHeader(AMSEventR *head){
if(!head)return;
if(!fHeader.Run){
cerr<<"AMSSetupR::UpdateHeader-W-RunNotSet.SettingTo "<<head->Run()<<endl;
fHeader.Run=head->Run();
//add tsql attempt
}
if(fHeader.LEvent<head->Event()){
 fHeader.LEvent=head->Event();
 fHeader.LEventTime=head->UTime();
 //fBValues.insert(make_pair(head->UTime(),BValues()));
}
if(fHeader.FEvent==0){
 fHeader.FEvent=head->Event();
 fHeader.FEventTime=head->UTime();
}

}
bool AMSSetupR::UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime){
fHeader.Run=run;
fHeader.OS=os;
fHeader.BuildNo=buildno;
fHeader.BuildTime=buildtime;
if(run){
//add tsql attempt
}
return true;
}
void AMSSetupR::Reset(){
fGPSTime.clear();
fBValues.clear();
fISSData.clear();
fScalers.clear();
fLVL1Setup.clear();
Header a;
fHeader=a;
}

AMSSetupR::AMSSetupR(){
Reset();
}
  ClassImp(AMSSetupR::Header)
  ClassImp(AMSSetupR)
  ClassImp(AMSSetupR::BValues)

 void AMSSetupR::Init(TTree *tree){
  //   Set branch addresses
  if (tree == 0) return;
  AMSSetupR *dummy=this;
  tree->SetBranchAddress("run.",&dummy);
   
}
#ifndef __ROOTSHAREDLIBRARY__
bool AMSSetupR::FillHeader(uinteger run){
// Fill Header from DB

fHeader.Run=run;



const char * nve=getenv("GetIorExec");
char ior[]="getior.exe";
if(! (nve && strlen(nve)))nve=ior;
const char *exedir=getenv("ExeDir");
const char *amsdatadir=getenv("AMSDataDir");
char local[1024]="";

if(! (exedir && strlen(exedir))){
  if(amsdatadir && strlen(amsdatadir)){
     strcpy(local,amsdatadir);
     strcat(local,"/DataManagement/exe");
   }
   exedir=local;
 }
const char *version=AMSCommonsI::getversion(); 
const char *nvr=AMSCommonsI::getosversion(); 
if( nve &&strlen(nve) && exedir  && AMSCommonsI::getosname()){
 char t1[1024];
 strcpy(t1,exedir);
 strcat(t1,"/../prod");
 setenv("TNS_ADMIN",t1,1);
  AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  if(strstr(nvr,"2.6")){
   systemc+=".6";
  }
  char u[128];
  sprintf(u," -r %d",fHeader.Run);
    systemc+=u;
  systemc+="  > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%d",getpid());
  systemc+=tmp;
  int i=system(systemc);
  if(i){
   cerr <<"  AMSSetupR::FillHeader-E-UnableTo "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   return false; 
  }
  else{
   systemc="/tmp/getior."; 
   systemc+=tmp;
   ifstream fbin;
   fbin.open(systemc);
   if(fbin){
   fbin>>run;
   if(run!=fHeader.Run){
     cerr<<" AMSSetupR::FillHeader-E-WrongRun "<<run<<endl;
      return false;
    }
    fbin>>fHeader.FEvent;
    fbin>>fHeader.LEvent;
    fbin>>fHeader.FEventTime;
    fbin>>fHeader.LEventTime;
    cout <<" AMSSetupR::FillHeader-I-"<<fHeader.Run<<" "<<fHeader.FEvent<<" "<<fHeader.LEvent<<" "<<fHeader.FEventTime<<" "<<fHeader.LEventTime<<endl;
   fbin.close();
   }
   else cerr<<"AMSSetupR::FillHeader-E-UnableToOpenfile "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   return true;
   }
}
else{
    cerr<<" AMSSetupR::FillHeader-E-UnableToTryToGetIORBecauseSomeVarAreNull"<<endl;
return false;
}
}
#else
bool AMSSetupR::FillHeader(uinteger run){return true;}
#endif


#ifndef __ROOTSHAREDLIBRARY__
bool AMSSetupR::FillSlowcontrolDB(const char * file){
// Fill SlowcontrolDB via AMI interface


return false;

const char * nve=getenv("Getami2rootxec");
char ior[]="ami2root.exe";
if(! (nve && strlen(nve)))nve=ior;
const char *exedir=getenv("ExeDir");
const char *amsdatadir=getenv("AMSDataDir");
char local[1024]="";

if(! (exedir && strlen(exedir))){
  if(amsdatadir && strlen(amsdatadir)){
     strcpy(local,amsdatadir);
     strcat(local,"/DataManagement/exe");
   }
   exedir=local;
 }
const char *version=AMSCommonsI::getversion(); 
const char *nvr=AMSCommonsI::getosversion(); 
if( nve &&strlen(nve) && exedir  && AMSCommonsI::getosname()){
 char t1[1024];
 strcpy(t1,exedir);
 strcat(t1,"/../prod");
 setenv("TNS_ADMIN",t1,1);
  AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  char u[128];
  sprintf(u,"  %d %d %s ",fHeader.FEventTime,fHeader.LEventTime,file);
    systemc+=u;
  systemc+="  > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%d",getpid());
  systemc+=tmp;
  int i=system(systemc);
  if(i){
   cerr <<"  AMSSetupR::FillslowcontrolDB-E-UnableTo "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   return false; 
  }
  else{
   cout <<"  AMSSetupR::FillslowcontrolDB-I- "<<systemc<<endl;
   int bad=1;
   systemc="/tmp/getior."; 
   systemc+=tmp;
   ifstream fbin;
   fbin.open(systemc);
   if(fbin){
   fbin>>bad;
   if(bad){
     cerr<<" AMSSetupR::FillSlowcontrolDB-EUnableToFill "<<file<<endl;
      unlink(systemc);
      return false;
    }
   fbin.close();
   }
   else cerr<<"AMSSetupR::FillSlowControlDB-E-UnableToOpenfile "<<systemc<<endl;
   unlink(systemc);
   return true;
   }
}
else{
    cerr<<" AMSSetupR::FillSlowcontrolDB-E-UnableToToGetami2rootBecauseSomeVarAreNull"<<endl;
return false;
}
}
#else
bool AMSSetupR::FillSlowcontrolDB(const char*a){return true;}
#endif

