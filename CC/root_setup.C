//  $Id$

#include "root_setup.h"
#include "root.h"
#include <fstream>
#include "SlowControlDB.h"
#include "ntuple.h"
#include "DynAlignment.h"
#include "RichConfig.h"
#include "FrameTrans.h"
#include "dirent.h"
#include <sys/stat.h>

#ifndef __ROOTSHAREDLIBRARY__
#include "job.h"
#include "commons.h"
#include "commonsi.h"
#endif

#ifdef __DARWIN__
#include <unistd.h> // For getpid()
#endif

extern "C" int ISSGTOD(float *r,float *t,float *p, float *v, float *vt, float *vp, float *grmedphi, double time);
extern "C" int ISSLoad(const char *name, const char *line1, const char *line2);

#include "timeid.h"
class trio{
public: 
  unsigned int t1;
  unsigned int t2;
  unsigned int tmod;
  string filename;
};

class ISSICRS{
public:
  float x; ///< x (km) in ICRS
  float y; ///< y
  float z; ///< z
  float vx; ///< v_x m/s
  float vy; ///< v_y m/s
  float vz; ///< v_z m/s
};

typedef map <unsigned int,ISSICRS> ISSICRS_m;
typedef map <unsigned int,ISSICRS>::iterator ISSICRS_i;
static ISSICRS_m fISSICRS;           ///< ISS ICRS coordinates & velocity vector map
typedef map <unsigned int,TMatrixD> RotMatrices_m;
typedef map <unsigned int,TMatrixD>::iterator RotMatrices_i;
static RotMatrices_m fRotMatrices;   ///< CTRS->ICRS rotation matrices map


bool AMSSetupR::LoadISSMC=true;

AMSSetupR* AMSSetupR::_Head=0;
#ifdef __LZMA__
#include "Compression.h"
#endif
void AMSSetupR::CreateBranch(TTree *tree, int branchSplit){
  if(tree){
    _Head=this;
    tree->Branch("run.","AMSSetupR",&_Head,12800000,branchSplit);
    TBranch * branch=tree->GetBranch("run.");
    int clevel=branch->GetCompressionLevel();
#ifdef __LZMA__
    if(clevel<6 && branch->GetCompressionAlgorithm()!=ROOT::kLZMA)clevel=6;
#else
    if(clevel<6)clevel=6;
#endif
    branch->SetCompressionLevel(clevel);
    cout <<" SetupR CompressionLevel "<<branch->GetCompressionLevel()<<" "<<branch->GetSplitLevel()<<endl;
  }
}

int  AMSSetupR::Header::ClearRTB(unsigned int Time,unsigned int bit){
if(bit>31)return 1;
if(fRunTag.size()==0){
fRunTag.insert(make_pair(Time,0));
return 0;
}
runtag_i k=fRunTag.upper_bound(Time);
if(k==fRunTag.begin()){
fRunTag.insert(make_pair(Time,0));
return 0;
}
else k--;
unsigned int runtag=k->second;
runtag =runtag & (~(1<<bit));
if(k->first==Time)fRunTag.erase(k);
fRunTag.insert(make_pair(Time,runtag));
return 0;


}


int  AMSSetupR::Header::SetRTB(unsigned int Time,unsigned int bit){
if(bit>31)return 1;
if(fRunTag.size()==0){
fRunTag.insert(make_pair(Time,(1<<bit)));
return 0;
}
runtag_i k=fRunTag.upper_bound(Time);
if(k==fRunTag.begin()){
fRunTag.insert(make_pair(Time,(1<<bit)));
return 0;
}
else k--;
unsigned int runtag=k->second;
runtag =runtag | (1<<bit);
if(k->first==Time)fRunTag.erase(k);
fRunTag.insert(make_pair(Time,runtag));
return 0;


}

bool  AMSSetupR::Header::CheckRTB(unsigned int Time,unsigned int bit){
if(fRunTag.size()==0){
return false;
}
runtag_i k=fRunTag.upper_bound(Time);
if(k==fRunTag.begin()){
return false;
}
else k--;
unsigned int runtag=k->second;
if(bit<32)runtag =runtag & (1<<bit);
if(runtag)return true;
else return false;

}

AMSSetupR::GPS AMSSetupR::GetGPS(unsigned int run,unsigned int event){
static AMSSetupR::GPS gps;
for( GPS_ri i=fGPS.rbegin();i!=fGPS.rend();i++){
 if(i->second.Run==run && i->second.Event<=event){
  return i->second;
 }
}
return gps;
}

int AMSSetupR::UpdateHeader(AMSEventR *head){
int ret=0;
if(!head)return 1;
unsigned long long runev=((unsigned long long)head->Run())<<32;
runev+=head->Event();
if(fEntries.find(runev)!=fEntries.end())ret=2;
else fEntries.insert(make_pair(runev,fEntries.size()));
unsigned int nsec;
unsigned int sec;
if(!head->fHeader.GetGPSEpoche(sec,nsec)){
 GPS gps;
 GPS_i it=fGPS.find(sec);
 if(it!=fGPS.end()){
   it->second.EventLast=head->Event();
 }
 else { 
  gps.Run=head->Run();
  gps.Event=head->Event();
  gps.EventLast=head->Event();
  gps.Epoche=head->fHeader.GPSTime;
 }
 fGPS.insert(make_pair(sec,gps));
 // gps=GetGPS(gps.Run,gps.Event);
 head->fHeader.GPSTime.clear();

}   
if(!fHeader.Run){
cerr<<"AMSSetupR::UpdateHeader-W-RunNotSet.SettingTo "<<head->Run()<<endl;
fHeader.Run=head->Run();
//add tsql attempt
}
if(fHeader.LEvent<head->Event()){
 fHeader.LEvent=head->Event();
 fHeader.LEventTime=head->UTime();
}
if(fHeader.FEvent==0){
 fHeader.FEvent=head->Event();
 fHeader.FEventTime=head->UTime();
}
return ret;
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

int AMSSetupR::GetEntry(unsigned int run,unsigned int event){
unsigned long long runev=((unsigned long long)run)<<32;
runev+=event;
htable_i it=fEntries.find(runev);
if(it==fEntries.end())return -1;
else return it->second;

}
void AMSSetupR::Reset(){
fEntries.clear();
fGPS.clear();
fRTI.clear();
fGPSWGS84.clear();
fISSData.clear();
fISSAtt.clear();
fAMSSTK.clear();
fDSPError.clear();
fISSINTL.clear();
fISSCTRS.clear();
fISSGTOD.clear();
fScalers.clear();
fScalersReturn.clear();
fLVL1Setup.clear();
fDynAlignment.clear();
fBadRun.clear();
Header a;
fHeader=a;
SlowControlR b;
fSlowControl=b;
Purge();
}
void AMSSetupR::ResetMC(){
fGPS.clear();
fRTI.clear();
fGPSWGS84.clear();
fISSData.clear();
fISSAtt.clear();
fAMSSTK.clear();
fDSPError.clear();
fISSINTL.clear();
fISSCTRS.clear();
fISSGTOD.clear();
fScalers.clear();
fScalersReturn.clear();
fLVL1Setup.clear();
fDynAlignment.clear();
fBadRun.clear();
SlowControlR b;
fSlowControl=b;
}


bool AMSSetupR::SlowControlR::ReadFromExternalFile=true;
AMSSetupR::AMSSetupR(){
Reset();
}
  ClassImp(AMSSetupR::ISSAtt)
  ClassImp(AMSSetupR::ISSSA)
  ClassImp(AMSSetupR::ISSINTL)
  ClassImp(AMSSetupR::ISSCTRS)
  ClassImp(AMSSetupR::ISSGTOD)
  ClassImp(AMSSetupR::TDVR)
  ClassImp(AMSSetupR::DSPError)
  ClassImp(AMSSetupR::Header)
  ClassImp(AMSSetupR)
  ClassImp(AMSSetupR::SlowControlR)
  ClassImp(AMSSetupR::SlowControlR::Element)
  //    ClassImp(AMSSetupR::SlowControlR)
  //  ClassImp(AMSSetupR::SlowControlR::Node)
  //  ClassImp(AMSSetupR::SlowControlR::DataType)
  //  ClassImp(AMSSetupR::SlowControlR::SubType)

/*
 void AMSSetupR::Add(SlowControlDB *p){
if(!p){
 cerr<<"AMSSetupR::add-E-SlowcontrolPointerIsNull"<<endl;
}
else{

    TObjArray *branchlist=p->GetListOfBranches();
  for(int i=0;i<(int)branchlist->GetEntries();i++){
    TObject* obj=(TObject*)branchlist->At(i);
      ::Node *node=new ::Node();
      AMSSetupR::SlowControlR::Node mynode;
    if(strcmp(obj->ClassName(),"TBranchElement")==0){
      string name=branchlist->At(i)->GetName();
      p->SetBranchAddress(branchlist->At(i)->GetName(),&node);
      TBranch*b=p->GetBranch(branchlist->At(i)->GetName());
      if(b){
        b->GetEntry(0);
      }
      for(std::map<int,DataType>::iterator dt=node->datatypes.begin();dt!=node->datatypes.end();dt++){
      AMSSetupR::SlowControlR::DataType mydatatype;
          for(std::map<int,SubType>::iterator st= dt->second.subtypes.begin();st!=dt->second.subtypes.end();st++){
      AMSSetupR::SlowControlR::SubType mysubtype;
           for(std::map<unsigned int,float>::iterator it=st->second._table.begin();it!=st->second._table.end();it++){
                 std::map<unsigned int,float>::iterator itp=++it;
                
               bool addp=it->first>=fHeader.FEventTime || (itp!=st->second._table.end() && itp->first>fHeader.FEventtime);
                 std::map<unsigned int,float>::iterator itm=--it;
               bool addm=it->first<=fHeader.LEventTime || (itm!=st->second._table.begin() && itm->first<fHeader.LEventtime);

               if(addp && addm )mysubtype.fTable[it->first]=it->second;
           }
           unsigned long long sd;
           sd=(it->first)<<32;
           sd+=(st->first);
           mynode.fRTable[st->second.tag]=sd;
           mydatatype.fSTable[st->first]=mysubtype;
         }
         mynode.Number=dt->second.number;
         mynode.fDTable[dt->first]=mydatatype;
        }
        string anothername=node->GetName();
        fSlowControl.fNTable[anothername]=mynode;
        fSlowControl.fBegin=p->begin;
        fSlowControl.fEnd=p->end;
    }
  delete node;
  }
}
}
*/

 void AMSSetupR::Add(SlowControlDB *p, int verb){
if(!p){
 cerr<<"AMSSetupR::add-E-SlowcontrolPointerIsNull"<<endl;
}
else{
      fSlowControl.fETable.clear();
      fSlowControl.fRTable.clear();
      for(map<string,::Node>::iterator mt=p->nodemap.begin();mt!=p->nodemap.end();mt++){
      ::Node snode=mt->second;
      ::Node *node=&snode;
      string name=mt->first;
      for(std::map<int,DataType>::iterator dt=node->datatypes.begin();dt!=node->datatypes.end();dt++){
          for(std::map<int,SubType>::iterator st= dt->second.subtypes.begin();st!=dt->second.subtypes.end();st++){
           AMSSetupR::SlowControlR::Element myelement;      
           for(std::map<unsigned int,float>::iterator it=st->second._table.begin();it!=st->second._table.end();it++){
               myelement.fTable[it->first]=it->second;
           }
           myelement.datatype=dt->first;
           myelement.subtype=st->first;
           myelement.NodeName=node->GetName();
           myelement.BranchName=name.c_str();
           string s=(const char*)st->second.tag;
           fSlowControl.fETable.insert(make_pair(s,myelement));
           unsigned long long sd=(dt->first );
           sd=sd<<32;
           sd+=st->first;
           fSlowControl.fRTable[sd]=s;
        }
        }
        fSlowControl.fBegin=p->begin;
        fSlowControl.fEnd=p->end;
    }
}
if(verb)fSlowControl.print();
}

int AMSSetupR::LoadExt(){
if(!SlowControlR::ReadFromExternalFile)return -1;
string slc;
time_t gettime=getSlowControlFilePath(slc);
if(gettime<1200000000 || gettime> 2100000000){
 cerr<<"AMSSetupR::LoadExt-W-EventTimeoutOfRange "<<gettime<<endl;
 if( AMSEventR::nMCEventgC()){
cerr<<"AMSSetupR::LoadExt-W-DisablingReadFromExternalfileForMC "<<endl;
SlowControlR::ReadFromExternalFile=false;
}
}
return LoadSlowcontrolDB(slc.c_str())?0:1;
}

int AMSSetupR::SlowControlR::GetData(const char * elementname, unsigned int time, float frac,  vector<float> &value,int method , const char *nodename,int dt, int st){
const int le=-1;
int retcode=le;
value.clear();
string elem;
bool usenodename=false;
if(nodename && strlen(nodename)){
 usenodename=true;
}
static unsigned int loadextdone=0;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate(loadextdone)
#else
#pragma omp critical (loadext)
#endif
if(loadextdone!=time && _Head && (!fRTable.size() || !fETable.size())){
 if(_Head->LoadExt())loadextdone=time;

}

if(elementname && strlen(elementname)>0){
 elem=elementname;
}
else{
 unsigned long long key=dt;
 key=key<<32;
 key+=st;
 rtable_i it=fRTable.find(key);
if(it!=fRTable.end())elem=it->second;
else{
    retcode=3;
    return retcode;
}
}
 pair<etable_i,etable_i> ret=fETable.equal_range(elem);
 retcode=1;
 for(etable_i i=ret.first;i!=ret.second;++i){
  retcode=le;
  for(Element::table_i j=i->second.fTable.begin();j!=i->second.fTable.end();j++){
//cout <<time <<" "<<frac<<" "<<j->first<<" "<<j->second<<endl;
} 
  Element::table_i j=i->second.fTable.lower_bound(time);
  if(i->second.fTable.size()>0){
   bool add=false;
   if(j==i->second.fTable.begin()){
      if(j->first==time)add=true;
      else {
        add=false;
        //cout <<" error add "<<time<<" "<<j->first<<endl;
      }
   }
   else if(j!=i->second.fTable.end()){
    if(j->first>time)j--;
    add=true;
   }
   else{
     j--;
     add=true;
 } 
 if(add){
 if(method==0){
if(usenodename){
if(strstr((const char*)i->second.NodeName,nodename)){
 value.push_back(j->second);
}
else retcode=2;
}
else  value.push_back(j->second);
}
else if(method==1){
  Element::table_i k=j;
 if(++k==i->second.fTable.end())retcode=4;
 else{
  float val=k->second*(time-j->first+frac)+j->second*(k->first-time-frac);
  val=val/(k->first-j->first);
if(usenodename){
if(strstr((const char*)i->second.NodeName,nodename)){
 value.push_back(val);
}
else retcode=2;
}
else  value.push_back(val);
}

}

else retcode=7;
 }
 else retcode=5;
}
else retcode=6;
}
if(value.size()>0)retcode=0;
return retcode;
}

 void AMSSetupR::Init(TTree *tree){
  //   Set branch addresses
  if (tree == 0) return;
  _Head=this;
  tree->SetBranchAddress("run.",&_Head);
   
}
#ifndef __ROOTSHAREDLIBRARY__
void AMSSetupR::TDVRC_Add(unsigned int time,AMSTimeID *tdv){
TDVR a;
a.Begin=tdv->_Begin;
a.Insert=tdv->_Insert;
a.End=tdv->_End;
a.Size=tdv->_Nbytes;
a.CRC=tdv->_CRC;
a.DataMC=tdv->getid()?2:0;
a.Name=tdv->getname();
a.FilePath=(const char*)tdv->_fname;
if(AMSJob::gethead()->isProduction() || IOPA.WriteTDVDataInRoot){
a.Data.clear();
a.Size/=sizeof(uinteger);
a.Size--;
for(int i=0;i<a.Size;i++){
a.Data.push_back(*(tdv->_pData+i));
}
}
string s=tdv->getname();
fTDVRC[s];
TDVRC_i i=fTDVRC.find(s);
i->second.insert(make_pair(time,a));
//cout <<"  inserting "<<a.Name<<" "<<a.FilePath<<" "<<time<<endl;

}
#else
void AMSSetupR::TDVRC_Add(unsigned int time,AMSTimeID *tdv){
TDVR a;
a.Begin=tdv->_Begin;
a.Insert=tdv->_Insert;
a.End=tdv->_End;
a.Size=tdv->_Nbytes;
a.CRC=tdv->_CRC;
a.DataMC=tdv->getid();
a.Name=tdv->getname();
a.FilePath=(const char*)tdv->_fname;
a.Data.clear();
a.Size/=sizeof(uinteger);
a.Size--;
for(int i=0;i<a.Size;i++){
a.Data.push_back(*(tdv->_pData+i));
}
string s=tdv->getname();
fTDVRC[s];
TDVRC_i i=fTDVRC.find(s);
i->second.insert(make_pair(time,a));
cout <<"  inserting "<<a.Name<<" "<<a.FilePath<<" "<<time<<endl;

}
#endif

ostream &operator << (ostream &o, const AMSSetupR::TDVR &b )
{return b.print(o);}
  
ostream & AMSSetupR::TDVR::print(ostream &o)const{
  return(o <<Name << "  " <<FilePath<<" Size "<<Size<<" RealSize "<<Data.size() );
}

bool AMSSetupR::TDVR::CopyOut(void * Out){
if(Data.size()!=Size){
cerr<<"AMSSetupR::TDVR::CopyOut-E-SizeDeclared "<<Size <<" Real "<<Data.size()<<endl;
return false;
}
for(int k=0;k<Data.size();k++){
 memcpy((unsigned int*)Out+k,&Data[k],sizeof(Data[0]));
}
return true;
}

int AMSSetupR::getTDV(const string & name, unsigned int time, AMSSetupR::TDVR & tdv){
TDVRC_i i=fTDVRC.find(name);
if(i!=fTDVRC.end()){
TDVR_i j=i->second.lower_bound(time);
if(i->second.size()>0){
bool add=false;
if(j==i->second.begin()){
if(j->first==time)add=true;
else add=false;
}
else if(j!=i->second.end()){
if(j->first >time)j--;
add=true;
}
else{
j--;
add=true;
}
if( add){
tdv=j->second;
return 0;
}
else return 2;
}
else return 2;
}
else return 1;
}

int AMSSetupR::getTDVi(const string & name, unsigned int time, TDVR_i &j){
TDVRC_i i=fTDVRC.find(name);
if(i!=fTDVRC.end()){
 j=i->second.lower_bound(time);
if(i->second.size()>0){
bool add=false;
if(j==i->second.begin()){
if(j->first==time)add=true;
else add=false;
}
else if(j!=i->second.end()){
if(j->first >time)j--;
add=true;
}
else{
j--;
add=true;
}
if( add){
return 0;
}
else return 2;
}
else return 2;
}
else return 1;
}

 int AMSSetupR::getAllTDV(unsigned int time){
  vector<TDVR> &v= fTDV_Time;
  v.clear();
for(TDVRC_i i=fTDVRC.begin();i!=fTDVRC.end();i++){
TDVR_i j=i->second.lower_bound(time);
if(i->second.size()>0){
bool add=false;
if(j==i->second.begin()){
if(j->first==time)add=true;
else add=false;
}
else if(j!=i->second.end()){
if(j->first >time)j--;
add=true;
}
else{
j--;
add=true;
}
if( add){

  v.push_back(j->second);
}
}}
return v.size();
}

 int   AMSSetupR::getAllTDV(const string &name){
  vector<TDVR> &v=fTDV_Name; 
  v.clear();
  TDVRC_i i=fTDVRC.find(name);
  if(i!=fTDVRC.end()){
    for( TDVR_i j=i->second.begin();j!=i->second.end();j++)v.push_back(j->second);
  }
return v.size();
}

void AMSSetupR::Purge(){
TDVRC_Purge();
}

void AMSSetupR::TDVRC_Purge(){
for(TDVRC_i i=fTDVRC.begin();i!=fTDVRC.end();i++){
 //cout<<"AMSSetupR::TDVRC_Purge-I-Purging "<<i->first<<" "<<i->second.size()<<endl;
if(i->second.size()>1){
 i->second.erase(i->second.begin(),--(i->second.end()));
// cout<<"AMSSetupR::TDVRC_Purge-I-Purged "<<i->first<<" "<<i->second.begin()->second.Size<<" "<<i->second.begin()->second.FilePath<<" "<<endl;
 }
}
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
 setenv("TNS_ADMIN",t1,0);
 if(getenv("AMSOracle")){
 setenv("LD_LIBRARY_PATH",getenv("AMSOracle"),1);
}
else{
 setenv("LD_LIBRARY_PATH","/afs/cern.ch/ams/oracle/10205/lib",1);
}
   char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
   char *localbin=0;
   if(getenv("AMSDataDir")){
    localbin=getenv("AMSDataDir");
   }
   else localbin=local;
   AString systemc=localbin;
  systemc+="/DataManagement/exe/linux/timeout --signal 9 240 ";
  systemc+=exedir;
 systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  if(strstr(nvr,".el6")){
   systemc+=".so";
  }
  else if(strstr(nvr,"2.6")){
   systemc+=".so";
  }
  char u[128];
  sprintf(u," -r %u",fHeader.Run);
    systemc+=u;
  systemc+="  > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%u",getpid());
  systemc+=tmp;
  int i=system(systemc);
  if(i){
   cerr <<"  AMSSetupR::FillHeader-E-UnableTo "<<systemc<<endl;
   const int dt=120;
   fHeader.FEventTime=fHeader.Run-dt;
   fHeader.LEventTime=fHeader.Run+3600;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   fISSData.clear();
   fISSAtt.clear();
   fAMSSTK.clear();
   fISSINTL.clear();
   fISSCTRS.clear();
   fISSGTOD.clear();
   fRTI.clear();
   fGPSWGS84.clear();
   fJGCR.clear();
   fJGC.clear();
   fDSPError.clear();
   LoadISS(fHeader.Run-dt,fHeader.Run+3600);
   LoadISSAtt(fHeader.Run-dt,fHeader.Run+3600);
   LoadISSSA(fHeader.Run-dt,fHeader.Run+3600);
   LoadISSINTL(fHeader.Run-dt,fHeader.Run+3600);
   LoadISSCTRS(fHeader.Run-dt,fHeader.Run+3600);
   LoadGPSWGS84(fHeader.Run-dt,fHeader.Run+3600);
   LoadISSGTOD(fHeader.Run-dt,fHeader.Run+3600);
   LoadRTI(fHeader.Run-dt,fHeader.Run+3600);
   LoadAMSSTK(fHeader.Run-dt,fHeader.Run+3600);
   LoadDSPErrors(fHeader.Run-dt,fHeader.Run+3600);
   LoadDynAlignment(fHeader.Run);
   LoadJMDCGPSCorr();
   if(!IOPA.BuildRichConfig)LoadRichConfig(fHeader.Run);
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
   else {
      const int dt=120;
      cerr<<"AMSSetupR::FillHeader-E-UnableToOpenfile "<<systemc<<endl;
      fHeader.FEventTime=fHeader.Run-dt;
      fHeader.LEventTime=fHeader.Run+3600;

   }
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   fISSData.clear();
   fISSAtt.clear();
   fDSPError.clear();
   fAMSSTK.clear();
   fJGCR.clear();
   fJGC.clear();
   const int dt=120;
   LoadISS(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadISSAtt(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadISSSA(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadISSINTL(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadISSCTRS(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadGPSWGS84(fHeader.Run-dt,fHeader.LEventTime+dt);
   LoadISSGTOD(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadRTI(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadAMSSTK(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadDSPErrors(fHeader.FEventTime-dt,fHeader.LEventTime+dt);
   LoadDynAlignment(fHeader.Run);
   LoadJMDCGPSCorr();
   if (!IOPA.BuildRichConfig)LoadRichConfig(fHeader.Run);
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

void AMSSetupR::updateSlowControlFilePath(string & slc){
#ifndef __ROOTSHAREDLIBRARY__
	  int valid=0;
	  int kvalid=-1;
	  int kvalid1=-1;
	  int kvalid2=-1;
	  int kvalid3=-1;
          unsigned int t1=0;
          unsigned int t2=0;
          size_t pos=slc.find("/SCDB");
	  for(int k=pos;k<strlen(slc.c_str());k++){
           
            if(slc.c_str()[k]=='.'){
                 valid++;
                 kvalid=k;

	  if(valid==1 ){
            kvalid1=kvalid;
          }
	  if(valid==2 ){
            kvalid2=kvalid;
          }
	  if(valid==3 ){
            kvalid3=kvalid;
          }
          }
          }
	  if(valid>=3){
          char tmp[80];
          
          strncpy(tmp,slc.c_str()+kvalid1+1,kvalid2-kvalid1+1);
	  if(isdigit(tmp[0])){
            t1=atol(tmp);
          }
          strncpy(tmp,slc.c_str()+kvalid2+1,kvalid3-kvalid2+1);
	  if(isdigit(tmp[0])){
            t2=atol(tmp);
          }
          }
          
         if(t1 && t2){
            if(fSlowControl.fBegin>t1)t1=fSlowControl.fBegin;
            if(fSlowControl.fEnd<t2)t2=fSlowControl.fEnd;
            char ns[1024];
            strncpy(ns,slc.c_str(),kvalid1+1);
            ns[kvalid1+1]='\0';
            char tmp[80];
            sprintf(tmp,"%u",t1);
            cout << " t1 "<<ns<<" "<<tmp<<" "<<slc.c_str()<<" "<<kvalid1<<endl;  
          strcat(ns,tmp);
            strcat(ns,".");
            sprintf(tmp,"%u",t2);
            strcat(ns,tmp);
            strcat(ns,slc.c_str()+kvalid3);
            slc=ns;             
           cout<<"AMSSetupR::updateSlowcotrolFilePath-I-UpdatedTo "<<slc<<endl;
         }
         else{
           cerr<<"AMSSetupR::updateSlowcotrolFilePath-E-UnableToDecodeFile "<<t1<<" "<<t2<<" "<<slc<<endl;
        }
#else
#endif
}


time_t AMSSetupR::getSlowControlFilePath(string & slc){
//#ifndef __ROOTSHAREDLIBRARY__
// check first if needed file exists in 
//
const char * local=getenv("SlowControlDir");
if(local && strlen(local)){
slc=local;
}
else{
const char *amsdatadir=getenv("AMSDataDir");
if(amsdatadir && strlen(amsdatadir)){
slc=amsdatadir;
slc+="/SlowControlDir";
}
}
  char tmps[512];
   time_t t1=fHeader.FEventTime;
  tm * tm1=localtime(&t1);
  int tzz=timezone;
  tm * tmp=gmtime(&t1);
  tmp->tm_hour=0;
  tmp->tm_min=0;
  tmp->tm_sec=0;

// run in this directory and find all files like SCDB.t1.t2.root 
// with t1<feventtime<leventtime<t2

//  no sub dir anymore
//sprintf(tmps,"/%u",mktime(tmp)-3600-tzz);
    string sdir=slc;
#ifdef __DARWIN__
    //29 October 2012 found a compiling error in Mountain Lion (10.8.x)
    //../CC/root_setup.C:803: error: invalid conversion from ‘int (*)(dirent*)’ to ‘int (*)(const dirent*)’
    // this is due to /usr/include/dirent.h
    //------------------------------------------
    // Before Mountain Lion --
    
    //   /usr/include/dirent.h
    
    // #if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
    //   int scandir(const char *, struct dirent ***,
    // 	      int (*)(struct dirent *), int (*)(const void *, const void *)) __DARWIN_INODE64(scandir);
    //  #ifdef __BLOCKS__
    //  int scandir_b(const char *, struct dirent ***,
    // 	       int (^)(struct dirent *), int (^)(const void *, const void *)) __DARWIN_INODE64(scandir_b);
    // #endif /* __BLOCKS__ */
    // #endif /* not POSIX */
    
    //  In Mountain Lion --
    
    //    /usr/include/dirent.h
    
    //    int scandir(const char *, struct dirent ***,
    // 	       int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **)) __DARWIN_INODE64(scandir);
    //  #ifdef __BLOCKS__
    //  int scandir_b(const char *, struct dirent ***,
    // 	       int (^)(const struct dirent *), int (^)(const struct dirent **, const struct dirent **)) __DARWIN_INODE64(scandir_b) __OSX_AVAILABLE_STARTING(__MAC_10_6, __IPHONE_3_2);
    // #endif /* __BLOCKS__ */
    //--------------------------
    // 9 November 2013 with first release of Mavericks (10.9)
    // found the problem that OSXVER was bad defined (109 instead 1090 since sw_vers was giving 10.9 instead of 10.9.0) so the
    // #if OSXVER >= 108 was not working correctly
    // fixed 'by hand' in Makefile.macosx64 but it could happen also with 10.8, 10.10, ...
    dirent ** namelist;
    int nptr=scandir(sdir.c_str(),&namelist,_select,NULL);
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
    dirent64 ** namelist;
    int nptr=scandir64(sdir.c_str(),&namelist,_select,NULL);
#endif

    bool found=false;
        vector <trio> tv;
	for(int i=0;i<nptr;i++) {
	  int valid=0;
	  int kvalid=-1;
	  int kvalid1=-1;
	  int kvalid2=-1;
	  int kvalid3=-1;
            trio t;
            t.t1=0;
            t.t2=0;
           struct stat64 statbuf;
            t.filename=sdir;
            t.filename+="/";
            t.filename+=namelist[i]->d_name;
            stat64(t.filename.c_str(),&statbuf);
            t.tmod=statbuf.st_mtime;
            //cout <<" slow "<<t.filename<<endl;
	  for(int k=0;k<strlen(namelist[i]->d_name);k++){
	    if((namelist[i]->d_name)[k]=='.' ){
              valid++;
	      kvalid=k;
	      if(valid==1)kvalid1=k;
	      if(valid==2)kvalid2=k;
	      if(valid==3)kvalid3=k;
            }
          }
          if(valid>=3){
           char tmp[80];
           strncpy(tmp,namelist[i]->d_name+kvalid1+1,kvalid2-kvalid1+1);
  	   if(isdigit(tmp[0])){
	     t.t1=atol(tmp);
           }

           strncpy(tmp,namelist[i]->d_name+kvalid2+1,kvalid3-kvalid2+1);
  	   if(isdigit(tmp[0])){
	     t.t2=atol(tmp);
           }
          }
            free(namelist[i]);
          tv.push_back(t);
         //cout << "slow "<<t.t1<<" "<<t.t2<<" "<<t.tmod<<endl;
         }
         
	if(nptr>0)free(namelist);
         int k=-1;
         unsigned int maxt=0;
         for(int i=0;i<tv.size();i++){
           if(tv[i].t1<=fHeader.FEventTime && tv[i].t2>=fHeader.LEventTime && maxt<=tv[i].tmod){
              maxt=tv[i].tmod;
             k=i;
           }
         }
         if(k>=0){          cout <<"AMSSetupR::getSlowControlFilePath-I-FoundFile "<<tv[k].filename<<endl;
          slc=tv[k].filename;
          }
         else{
          sprintf(tmps,"/SCDB.%u.%u.root",fHeader.FEventTime-86400,fHeader.LEventTime+86400);
         slc+=tmps;
         }

//          sprintf(tmps,"/%u/SCDB.%u.%u.root",mktime(tmp)-3600-tzz,fHeader.FEventTime,fHeader.LEventTime);
//         slc+=tmps;
          cout <<"AMSSetupR::getslowcontrolfilepath-I-"<<slc<<endl;                    
//#endif
return t1;
}
#ifndef __ROOTSHAREDLIBRARY__
bool AMSSetupR::FillSlowcontrolDB(string & slc){
// Fill SlowcontrolDB via AMI interface


bool force=false;
oncemore:

getSlowControlFilePath(slc);

 bool uncompleted=true;
  SlowControlDB::KillPointer();
  SlowControlDB* scdb=SlowControlDB::GetPointer();
  if(scdb && scdb->Load(slc.c_str(),fHeader.FEventTime,fHeader.LEventTime)){
    uncompleted=scdb->uncompleted==1 || (IOPA.ReadAMI>=2 && scdb->uncompleted==2);
// uncompleted 2 no records in root files and root files is old enough (>week)    
  }
  SlowControlDB::KillPointer();
  string slc_orig=slc;


if(uncompleted  || IOPA.ReadAMI==2){
if(!IOPA.ReadAMI){
AMSNtuple::UnLock();
return false;
}
char tmps[255];
int delta=43200;
sprintf(tmps,"/tmp/SCDB.%u.%u.root",fHeader.FEventTime-delta,fHeader.LEventTime+delta);
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
int maxtry=12;
    unsigned int tmout=600;
   int slept=0;
   bool wait=false;
   for(;;){
    int ok=AMSNtuple::Lock(tmout*4);
    if(ok && !force)sleep(2);
    else break;
    wait=true;
    slept+=2;
    if(slept>tmout){
        AMSNtuple::Bell();
        slept=0;
    }
   }
   if(!force  && wait){
        AMSNtuple::Bell();
      sleep(30);
      force=true;
      goto oncemore;
    }
slc=tmps;
for (int ntry=0;ntry<maxtry;ntry++){
 char t1[1024];
 strcpy(t1,exedir);
 strcat(t1,"/../prod");
 setenv("TNS_ADMIN",t1,0);
   char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
   char *localbin=0;
   if(getenv("AMSDataDir")){
    localbin=getenv("AMSDataDir");
   }
   else localbin=local;
   AString systemc=localbin;
  systemc+="/DataManagement/exe/linux/timeout --signal 9 240 ";
  systemc+=exedir;
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  char u[255];
  sprintf(u,"  %u %u %s %s %u",fHeader.FEventTime-delta,fHeader.LEventTime+delta,slc_orig.c_str(),slc.c_str(),tmout);
    slc_orig=slc;
    systemc+=u;
  systemc+="  > /tmp/getior.";
  char tmp[80];
  sprintf(tmp,"%u",getpid());
  systemc+=tmp;

  int i=system(systemc);
   
  if(i){
   cerr <<"  AMSSetupR::FillslowcontrolDB-E-UnableTo "<<systemc<<" "<<i<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   if((i&255) || (i>>8)!=4){
      AMSNtuple::UnLock();
      return false; 
   }
   else AMSNtuple::Lock(tmout*4);
   AMSNtuple::Bell();
  }
  else{
   cout <<"  AMSSetupR::FillslowcontrolDB-I- "<<systemc<<endl;
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   AMSNtuple::UnLock();
   return true;
   }

}
AMSNtuple::UnLock();
return true;
}
else{
    cerr<<" AMSSetupR::FillSlowcontrolDB-E-UnableToToGetami2rootBecauseSomeVarAreNull"<<endl;
AMSNtuple::UnLock();
return false;
}
}
else{
cout <<" FillSlowControlDB-I-OPenedFile "<<slc<<endl;
if(IOPA.ReadAMI)AMSNtuple::UnLock();
return true;
}
}

#else
bool AMSSetupR::FillSlowcontrolDB( string & pfile){
return false;
}
#endif



bool AMSSetupR::LoadSlowcontrolDB(const char* file, int verb){
  SlowControlDB::KillPointer();
if(verb)cout <<"AMSSetupR::LoadSlowcontrolDB-I-ResetSCDBPointer "<<file<<endl;
  SlowControlDB* scdb=SlowControlDB::GetPointer();
if(verb)cout <<"AMSSetupR::LoadSlowcontrolDB-I-GetSCDBPointer "<<file<<endl;
  if(scdb && scdb->Load(file,fHeader.FEventTime,fHeader.LEventTime)){
if(verb)cout <<"AMSSetupR::LoadSlowcontrolDB-I-LoadSCDBFile "<<file<<endl;
    scdb->BuildSearchIndex(0);
if(verb)cout <<"AMSSetupR::LoadSlowcontrolDB-I-AddSearchIndex "<<file<<endl;
  Add(scdb, verb);
if(verb)cout <<"AMSSetupR::LoadSlowcontrolDB-I-fillSlowControl "<<file<<endl;
  }
  else{
    cerr<<"AMSSetupR::LoadSlowcontrolDB-E-UnabletoLoadFile "<<scdb<<" "<<file<<endl;
    return false;
  }
  return true;
}


void AMSSetupR::SlowControlR::print(){
  for( etable_i i=fETable.begin();i!=fETable.end();i++){
    if(i->second.fTable.size())cout <<i->first<<" "<<i->second.NodeName<<" "<<i->second.BranchName<<" "<<i->second.datatype<<" "<<i->second.subtype<<" "<<i->second.fTable.size()<<endl;
  }
}
void AMSSetupR::SlowControlR::printElementNames(const char *name){
  for( etable_i i=fETable.begin();i!=fETable.end();i++){
    if(!name || !strlen(name) || strstr(i->first.c_str(),name))cout <<i->first<<" DT "<<i->second.datatype<<" ST "<<i->second.subtype<<" NN "<<i->second.NodeName<<"  SIZE "<<i->second.fTable.size()<<endl;
  }
}

#ifdef __DARWIN__
#if __OSXVER__ >= 1080
integer AMSSetupR::_select( const dirent *entry)
#else
integer AMSSetupR::_select( dirent *entry)
#endif
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
integer AMSSetupR::_select(  const dirent64 *entry)
#endif
{
 return strstr(entry->d_name,"SCDB.")!=NULL;
}

int AMSSetupR::GetJMDCGPSCorr(double &corr, double &err, unsigned int time){
static int hint=-1;
static int init_error=0;
const double ti=1306000000;
#pragma omp threadprivate(hint)
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate(init_error)
#endif
 if(init_error==0 && !fJGCR.size()){
#ifndef __ROOTSHAREDLIBRARY__
#pragma omp critical (loadjmdc)
#endif
{
init_error=LoadJMDCGPSCorr();
}
 }
 if(!fJGCR.size())return 2;
 if(time<fJGCR[0].Validity[0]){
   corr=0;
   err=10;
   hint=-1;
   return 3;
 }
 else if (time>fJGCR[fJGCR.size()-1].Validity[1]){
   corr=0;
   err=0.3;
   hint=-1;
   return 3;
 }
 else{
  if(hint>=0 && hint<fJGCR.size() && time>=fJGCR[hint].Validity[0] && time<fJGCR[hint].Validity[1]){
   JGCR &a=fJGCR[hint];
   corr=a.A[0]+a.A[1]*(time-ti)+a.Par[0]*sin(a.Par[1]*(time-ti)+a.Par[2]);
   err=a.Err[1];
   return 0;
  } 
  for(hint=0;hint<fJGCR.size();hint++){
  if(time>=fJGCR[hint].Validity[0] && time<fJGCR[hint].Validity[1]){
   JGCR &a=fJGCR[hint];
   corr=a.A[0]+a.A[1]*(time-ti)+a.Par[0]*sin(a.Par[1]*(time-ti)+a.Par[2]);
   err=a.Err[1];
   return 0;
  }  
 }
}
corr=0;
err=10;
return 1;
}

int AMSSetupR::LoadJMDCGPSCorr(){
char amsdatadir[]="/afs/cern.ch/ams/Offline/AMSDataDir";
char *amsd=getenv("AMSDataDir");
if(!amsd || !strlen(amsd))amsd=amsdatadir;
string ifile=amsd;
ifile+="/v5.00/";
ifile+="gps.csv";
ifstream fbin;
fbin.clear();
fbin.open(ifile.c_str());
     if(fbin){
      fJGCR.clear();
      while(fbin.good() && !fbin.eof()){
        string line;
         while(getline(fbin,line)){
         vector<string>vout;
         istringstream str(line);
         string word;
         while(getline(str,word,',')){
          vout.push_back(word);
          if(vout.size()==9){
           JGCR a;
           istringstream convert;
           convert.clear();
           convert.str(vout[0]);
           convert>>a.Validity[0]; 
           convert.clear();
           convert.str(vout[1]);
           convert>>a.Validity[1]; 
           convert.clear();
           convert.str(vout[2]);
           convert>>a.A[0]; 
           convert.clear();
           convert.str(vout[3]);
           convert>>a.A[1]; 
           convert.clear();
           convert.str(vout[4]);
           convert>>a.Par[0]; 
           convert.clear();
           convert.str(vout[5]);
           convert>>a.Par[1]; 
           convert.clear();
           convert.str(vout[6]);
           convert>>a.Par[2]; 
           convert.clear();
           convert.str(vout[7]);
           convert>>a.Err[0]; 
           convert.clear();
           convert.str(vout[8]);
           convert>>a.Err[1]; 
           fJGCR.push_back(a);    
          }
        }
       }
     }
    }
     else{
       cerr<<"AMSSetupR::LoadJMDCGPSCorr-E-UnabletoOpenFile "<<ifile<<endl;
     }
     
    cout<< "AMSSetupR::LoadJMDCGPSCorr-I- "<<fJGCR.size()<<" Entries Loaded "<<endl;
    return fJGCR.size()?0:2;
}

void AMSSetupR::LoadISS(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return;
char amsdatadir[]="/afs/cern.ch/ams/Offline/AMSDataDir";
char *amsd=getenv("AMSDataDir");
if(!amsd || !strlen(amsd))amsd=amsdatadir;
string ifile=amsd;
ifile+="/v5.00/";
ifile+="ISS_tlefile.txt";
ifstream ifbin;
ifbin.clear();
ifbin.open(ifile.c_str());
                         if(ifbin){
	char stime[80],namei[80], line1i[80], line2i[80];
                           while(!ifbin.eof()){
			namei[0]=0;
			line1i[0]=0;
			line2i[0]=0;
                        stime[0]=0;
                        unsigned int tme;
                        ifbin.getline(stime,75);
                        tme=atol(stime);
			ifbin.getline(namei,75);
			ifbin.getline(line1i,75);
			ifbin.getline(line2i,75);
                        if(!ifbin.eof() ){
                          ISSData a;
                          a.TL1=line1i;
                          a.TL2=line2i;
                          a.Name=namei;
                          if(tme<t1)fISSData.clear();
                          fISSData.insert(make_pair(tme,a));
                          if(tme>t2)break;
                        }
                       }
                       ifbin.close();
                       cout<< "AMSSetupR::LoadISS-I- "<<fISSData.size()<<" Entries Loaded"<<endl;
                    }
                    else{
                       cerr<< "AMSSetupR::LoadISS-E-UnableToOpenFile "<<ifile<<endl;
                    }
                }



int AMSSetupR::getScalers(unsigned int time, unsigned int usec){
fScalersReturn.clear();

if(fScalers.size()==0)return 0;
unsigned long long t=time;
unsigned long long utm;

utm=t<<32;
utm+= usec;
//alway discard 1st scaler
Scalers_i k=fScalers.lower_bound(utm);
if(k==fScalers.begin()){
if(++k!=fScalers.end())fScalersReturn.push_back(k);
 else fScalersReturn.push_back(fScalers.begin());
}
else if(k!=fScalers.end()){
 if(--k!=fScalers.begin())fScalersReturn.push_back(k);
 fScalersReturn.push_back(++k);
}
else{
 fScalersReturn.push_back(--k);
}
return fScalersReturn.size();
}
#include "TMath.h"
#include "GM_SubLibrary.h"
///---RTI function
int AMSSetupR::RTI::Version=0;
int AMSSetupR::RTI::Loadopt=0;

int AMSSetupR::RTI::UseLatest(){

  RTI::Version=3;
  static int vrti=-1;
  if(RTI::Version!=vrti){
     cout<< "AMSSetupR::RTI::UseLatest Version="<<RTI::Version<<endl;
     vrti=RTI::Version;
  }
  return RTI::Version;
}

double AMSSetupR::RTI::gettime(int itm, int iev){
   
  double xtime=0;
  if     (itm==0)xtime=double(utime)+double(usec[iev])/1000000.;
  else if(itm==1)xtime=utctime[iev];
  else if(itm==2)xtime=double(utime)+double(usec[iev])/1000000.-utctime[iev];
  return xtime;
}

float AMSSetupR::RTI::getthetam(){
  
   double deg2rad = TMath::DegToRad();
   time_t Utime =utime;
        //...km
        double Re =  6371.2; //km Earth radius
        double Altitude = r/1.e5-Re;
        //...ISS rad-->deg
        double ThetaISS=  theta/deg2rad;
        double PhiISS=    phi/deg2rad;

        //--deg
        float thetaM = GM_GetThetaM(Utime, Altitude, ThetaISS, PhiISS);
        //--opposite sign and deg-->rad:
        thetaM= thetaM*(-1)*deg2rad;

      return thetaM;
}

TGraph *AMSSetupR::RTI::SAAedge = NULL;

TGraph *AMSSetupR::RTI::GetSAAedge()
{
	if(AMSSetupR::RTI::SAAedge != NULL)
		return AMSSetupR::RTI::SAAedge;

	AMSSetupR::RTI::SAAedge = new TGraph();
	AMSSetupR::RTI::SAAedge->SetPoint(0 , -46.9020531516 , 1.07065020944);
	AMSSetupR::RTI::SAAedge->SetPoint(1 , -42.4324570724 , 0.278767621219);
	AMSSetupR::RTI::SAAedge->SetPoint(2 , -38.7963908651 , -0.864383087583);
	AMSSetupR::RTI::SAAedge->SetPoint(3 , -32.3643804368 , -7.86605177782);
	AMSSetupR::RTI::SAAedge->SetPoint(4 , -32.1564301222 , -8.2925091686);
	AMSSetupR::RTI::SAAedge->SetPoint(5 , -30.4732034929 , -10.2380772476);
	AMSSetupR::RTI::SAAedge->SetPoint(6 , -28.9780074759 , -11.1541721933);
	AMSSetupR::RTI::SAAedge->SetPoint(7 , -24.9729186857 , -13.9219707474);
	AMSSetupR::RTI::SAAedge->SetPoint(8 , -22.3514515118 , -15.8035549547);
	AMSSetupR::RTI::SAAedge->SetPoint(9 , -17.864105537 , -17.5329648531);
	AMSSetupR::RTI::SAAedge->SetPoint(10 , -13.7360395887 , -18.4514556443);
	AMSSetupR::RTI::SAAedge->SetPoint(11 , -9.31195234438 , -20.5579164624);
	AMSSetupR::RTI::SAAedge->SetPoint(12 , -7.18498632474 , -21.1949557893);
	AMSSetupR::RTI::SAAedge->SetPoint(13 , -3.59895538649 , -23.8036939586);
	AMSSetupR::RTI::SAAedge->SetPoint(14 , -2.34504587932 , -24.8012082521);
	AMSSetupR::RTI::SAAedge->SetPoint(15 , -0.819716509312 , -27.065064314);
	AMSSetupR::RTI::SAAedge->SetPoint(16 , -0.0558141297819 , -29.4952077252);
	AMSSetupR::RTI::SAAedge->SetPoint(17 , -1.39465186809 , -31.1867845581);
	AMSSetupR::RTI::SAAedge->SetPoint(18 , -4.48059829104 , -33.3607272689);
	AMSSetupR::RTI::SAAedge->SetPoint(19 , -8.42183566883 , -35.9587368092);
	AMSSetupR::RTI::SAAedge->SetPoint(20 , -9.27876931794 , -36.2964186885);
	AMSSetupR::RTI::SAAedge->SetPoint(21 , -13.8165106415 , -37.7311180334);
	AMSSetupR::RTI::SAAedge->SetPoint(22 , -17.4924677527 , -39.0468547418);
	AMSSetupR::RTI::SAAedge->SetPoint(23 , -18.408230558 , -39.407844502);
	AMSSetupR::RTI::SAAedge->SetPoint(24 , -19.3555119436 , -39.7809321206);
	AMSSetupR::RTI::SAAedge->SetPoint(25 , -26.2402929635 , -42.5628157663);
	AMSSetupR::RTI::SAAedge->SetPoint(26 , -29.3390198221 , -43.7512422146);
	AMSSetupR::RTI::SAAedge->SetPoint(27 , -30.4468807826 , -44.1487676525);
	AMSSetupR::RTI::SAAedge->SetPoint(28 , -36.2608202879 , -44.9657315842);
	AMSSetupR::RTI::SAAedge->SetPoint(29 , -38.6471514882 , -45.8362408379);
	AMSSetupR::RTI::SAAedge->SetPoint(30 , -39.884535768 , -46.2670553531);
	AMSSetupR::RTI::SAAedge->SetPoint(31 , -41.1537099863 , -46.7007904009);
	AMSSetupR::RTI::SAAedge->SetPoint(32 , -46.4231376314 , -47.035405079);
	AMSSetupR::RTI::SAAedge->SetPoint(33 , -50.5517527888 , -46.9389068642);
	AMSSetupR::RTI::SAAedge->SetPoint(34 , -56.1886816966 , -45.9179669008);
	AMSSetupR::RTI::SAAedge->SetPoint(35 , -60.4620401598 , -44.5064118787);
	AMSSetupR::RTI::SAAedge->SetPoint(36 , -65.9797837207 , -40.9661101604);
	AMSSetupR::RTI::SAAedge->SetPoint(37 , -68.6756029734 , -39.1105256272);
	AMSSetupR::RTI::SAAedge->SetPoint(38 , -69.9926612952 , -38.1896928803);
	AMSSetupR::RTI::SAAedge->SetPoint(39 , -72.5874587403 , -35.1855408369);
	AMSSetupR::RTI::SAAedge->SetPoint(40 , -74.9768865609 , -32.1834974998);
	AMSSetupR::RTI::SAAedge->SetPoint(41 , -79.386936716 , -24.8704780079);
	AMSSetupR::RTI::SAAedge->SetPoint(42 , -81.5015374707 , -20.6638534546);
	AMSSetupR::RTI::SAAedge->SetPoint(43 , -83.3018473547 , -16.5956399645);
	AMSSetupR::RTI::SAAedge->SetPoint(44 , -83.4855953538 , -15.6706377827);
	AMSSetupR::RTI::SAAedge->SetPoint(45 , -83.4016563744 , -13.3880324461);
	AMSSetupR::RTI::SAAedge->SetPoint(46 , -79.0157111546 , -9.8025691601);
	AMSSetupR::RTI::SAAedge->SetPoint(47 , -76.0062621477 , -8.49383318946);
	AMSSetupR::RTI::SAAedge->SetPoint(48 , -71.9675022979 , -7.9008874195);
	AMSSetupR::RTI::SAAedge->SetPoint(49 , -69.1989836702 , -6.34481990494);
	AMSSetupR::RTI::SAAedge->SetPoint(50 , -68.320940027 , -6.15577355353);
	AMSSetupR::RTI::SAAedge->SetPoint(51 , -60.8270728968 , -4.71604157559);
	AMSSetupR::RTI::SAAedge->SetPoint(52 , -57.4730300335 , -3.41729725291);
	AMSSetupR::RTI::SAAedge->SetPoint(53 , -53.25104737 , -1.64190214253);
	AMSSetupR::RTI::SAAedge->SetPoint(54 , -51.5811868613 , -0.10182123818);
	AMSSetupR::RTI::SAAedge->SetPoint(55 , -49.9470795728 , 1.16581717215);
	AMSSetupR::RTI::SAAedge->SetPoint(56 , -46.9020531516 , 1.07065020944);

	return AMSSetupR::RTI::SAAedge;
	
}

bool AMSSetupR::RTI::IsInSAA()
{
	AMSSetupR::RTI::GetSAAedge();

	float rtiLong = phi*TMath::RadToDeg();
	float rtiLat  = theta*TMath::RadToDeg();

	while (rtiLong > 180.0f) rtiLong-=360.0f;

	if( AMSSetupR::RTI::SAAedge->IsInside(rtiLong,rtiLat) )
		return true;

	return false;
}

float AMSSetupR::RTI::getphim(){

   double deg2rad = TMath::DegToRad();
   time_t Utime =utime;
        //...km
        double Re =  6371.2; //km Earth radius
        double Altitude = r/1.e5-Re;
        //...ISS rad-->deg
        double ThetaISS=  theta/deg2rad;
        double PhiISS=    phi/deg2rad;

        //--deg
        float phiM = GM_GetPhiM(Utime, Altitude, ThetaISS, PhiISS);
        //--opposite sign and deg-->rad:
        phiM= phiM*(-1)*deg2rad;

      return phiM;
}


///----
int AMSSetupR::LoadRTI(unsigned int t1, unsigned int t2, const char *dir){
   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }

   AMSISSlocal+=postfix;
    const char * AMSISS=getenv("AMSISS");
   if (AMSISS && strlen(AMSISS)){
    AMSISSlocal=AMSISS;
   }
 AMSISSlocal+="RTI/";

if(t1>t2){
cerr<< "AMSSetupR::LoadAMSRTI-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadAMSRTI-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}

//---NewV
 bool isnewv   =((t2>1368950397) ||(RTI::Version>=1));
 bool isnewv2  =((t1>=1374268537)||(RTI::Version>=2));
 bool isnewv3  =((t2>1385484728) ||(RTI::Version>=3));
 bool isnewv3p5=((t2>1385484728) ||(RTI::Version>=503));
// bool isnewv3=(RTI::Version>=3);
 if     (isnewv3p5)AMSISSlocal+="V3_20140324_P5/";
 else if(isnewv3)  AMSISSlocal+="V3_20140324/";
 else if(isnewv2)  AMSISSlocal+="V2_20131220/";
 else if(isnewv )  AMSISSlocal+="V1_20130802/";
 AMSISS=AMSISSlocal.c_str();
 if(dir!=0)AMSISS=dir;


const char fpatb[]="RTI";
const char fpate[]="24H.csv";

//----
    char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);

//---
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     char fname[1000];
     sprintf(fname,"%s/%s_%u_%03u-%s",AMSISS,fpatb,yc,dc,fpate); 
     ifstream fbin;
     fbin.close();
     fbin.open(fname);
//----
     if(fbin){
      string line;
      getline(fbin,line);
//--
      while(fbin.good()&& !fbin.eof()){
         unsigned int nt; RTI a;
//---Input
         fbin>>nt;//JMDCtime
         fbin>>a.run;
         if(a.run!=0){//missing second
            fbin>>a.evno;
            if(isnewv2){ //Add Version 2
              fbin>>a.evnol;
              fbin>>a.usec[0]>>a.usec[1];
              fbin>>a.mtrdh;
            }
            fbin>>a.lf;//begin ev+ livetime
            for(int ifv=0;ifv<4;ifv++){//cutoff
              for(int ipn=0;ipn<2;ipn++)fbin>>a.cf[ifv][ipn];
            } 
           fbin>>a.mphe;//Helium Most Prob
           fbin>>a.theta>>a.phi>>a.r>>a.zenith>>a.glat>>a.glong;//Position
           fbin>>a.nev>>a.nerr>>a.ntrig>>a.npart;
//---Add Version 1
           if(isnewv){
             for(int ifv=0;ifv<4;ifv++){
               for(int ipn=0;ipn<2;ipn++)fbin>>a.cfi[ifv][ipn];
             }
             //---use IGRF table to  fill cfi
             if(RTI::Loadopt%10==1){
               double fv,cfn[2];
               for(int ifv=0;ifv<4;ifv++){
                 if     (ifv==0)fv=25;
                 else if(ifv==1)fv=30;
                 else if(ifv==2)fv=35;
                 else if(ifv==3)fv=40;
                 AMSEventR::GetMaxIGRFCutoff(fv,cfn,nt);
                 a.cfi[ifv][0]=cfn[0]; a.cfi[ifv][1]=cfn[1];
               }
             }
             //---
             for(int iexl=0;iexl<2;iexl++){
               for(int ixy=0;ixy<2;ixy++)fbin>>a.nl1l9[iexl][ixy];
             }
             for(int iexl=0;iexl<2;iexl++){
               for(int ico=0;ico<3;ico++)fbin>>a.dl1l9[iexl][ico];
             }
             fbin>>a.nhwerr;
           }
           if(isnewv3){
             fbin>>a.utctime[0]>>a.utctime[1];
           }
//---
           fbin>>a.good;
           a.utime=nt;
         }
         else continue;
         if(!fbin.good())continue;

//--Result
         if(nt<t1){bfound=1;continue;}//beg found
         else if(nt>=t1&&nt<=t2){
            if(bfound!=2){
               fRTI.clear();bfound=2;  
            }
            fRTI.insert(make_pair(nt,a));
         }//found 
         else if(nt>t2){efound=1;goto nah;}//end
//----
      }
      fbin.close();
     }//file exit

//--
     else{
       cerr<<"AMSSetupR::LoadISSRTI-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
        dc=1;
        yc++;
     }
   }//end second while


nah: 
int ret;
if(bfound&&efound)ret=0;//All found
else if(!bfound&&!efound)ret=2;//All not found
else ret=1;//Only One Found
    cout<< "AMSSetupR::LoadAMSRTI-I- "<<fRTI.size()<<" Entries Loaded "<<ret<<endl;

return ret;
  
}

int AMSSetupR::LoadAMSSTK(unsigned int t1, unsigned int t2){
   if(!LoadISSMC)return 3;
   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
   if (AMSISS && strlen(AMSISS)){ 
    AMSISSlocal=AMSISS;   
   }  
 AMSISSlocal+="STK/";
 AMSISS=AMSISSlocal.c_str();
if(t1>t2){
cerr<< "AMSSetupR::LoadAMSSTK-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadAMSSTK-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<" "<<t1<<" "<<t2<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}
const char fpatb[]="ECI_STK_";
const char fpatb2[]="ABL_STK_";
const char fpate[]=".csv";

    tm tmf;


// check tz
   unsigned int tzd=0;
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadAMSSTK-I-TZDSeconds "<<tzd<<endl;

}

{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadAMSSTK-I-TZDSeconds "<<tzd<<endl;

}



   char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);
    
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     string fname=AMSISS;
     fname+=fpatb;
     char utmp[255];
     sprintf(utmp,"%u/%s%u%03u",yc,fpatb,yc,dc);
     fname+=utmp;
     fname+=fpate;
     ifstream fbin;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     if(!fbin){
//   change partially to fpatb2
     fname=AMSISS;
     fname+=fpatb;
     char utmp[255];
     sprintf(utmp,"%u/%s%u%03u",yc,fpatb2,yc,dc);
     fname+=utmp;
     fname+=fpate;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     }
     if(fbin){
      while(fbin.good() && !fbin.eof()){


 string line;
 while(getline(fbin,line)){
  vector<string>vout;
  istringstream str(line);
  string word;
  while(getline(str,word,',')){
   vout.push_back(word);
  }
// cout <<vout.size()<<endl;
   if(vout.size()==23){
    AMSSTK a;
    if(vout[0][0]=='S' && vout[0][1]=='T' &&vout[0][2]=='K'){
    int solved=0;
    istringstream convert;
    convert.str(vout[1]);
    convert>>solved;
    if(solved==1){
     double tc=0;
     convert.clear();
     convert.str(vout[2]);
     convert>>tc;
     convert.clear();
     convert.str(vout[3]);
     convert>>a.cam_id;
     convert.clear();
     convert.str(vout[5]);
     convert>>a.cam_ra;     
     convert.clear();
     convert.str(vout[6]);
     convert>>a.cam_dec;     
     convert.clear();
     convert.str(vout[7]);
     convert>>a.cam_or;     
     convert.clear();
     convert.str(vout[13]);
     convert>>a.ams_ra;     
     convert.clear();
     convert.str(vout[14]);
     convert>>a.ams_dec;
     convert.clear();
     convert.str(vout[20]);
     strptime(vout[20].c_str(),"%Y-%j-%H-%M-%S",&tmf);
     time_t tf=mktime(&tmf);
     istringstream ct(vout[20]);
     string smsec;
     int msec=0;
     while(getline(ct,smsec,'-')){
      istringstream ismsec(smsec);
      ismsec>>msec;  
     }     



    double dt2=tf+double(msec)/1000.+tzd;   
    if(tc==dt2){
      fAMSSTK.insert(make_pair(tc,a));      
          if(tc>=t1 && tc<=t2){
           if(abs(bfound)!=2){
               fAMSSTK.clear();
               fAMSSTK.insert(make_pair(tc,a));
 //          cout<<setprecision(14);
 //          cout <<t1<<" "<<tc<<"  "<<t2<<endl;
               bfound=bfound?2:-2;
           }
          }
         else if(tc<t1)bfound=1;
         else if(tc>t2){
             efound=1;
//           cout<<setprecision(14);
//           cout <<t1<<" "<<tc<<"  "<<t2<<endl;
             goto nah;
         }


    }
    else{
     static int xer=0;
     if(xer++<100){
       cerr<<"AMSSetupR:::LoadAMSSTK-E-TimesAreDifferent "<<tc-dt2<<" "<<vout[20]<<" "<<tc<<endl;
     }
    }
   }
  }
}
}

}
}
     else{
       cerr<<"AMSSetupR::LoadAMSSTK-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
      dc=1;
      yc++;
     }

}
nah:
int ret;
if(bfound>0 &&efound)ret=0;
else if(!bfound && !efound )ret=2;
else ret=1;
    cout<< "AMSSetupR::LoadAMSSTK-I- "<<fAMSSTK.size()<<" Entries Loaded "<<ret<<endl;

return ret;

}


int AMSSetupR::LoadISSAtt(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;

   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
   if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();



if(t1>t2){
cerr<< "AMSSetupR::LoadISSAtt-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSAtt-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}
const char fpatb[]="ISS_ATT_EULR_LVLH-";
const char fpate[]="-24H.csv";
const char fpate2[]="-24h.csv";

// convert time to GMT format

// check tz
   unsigned int tzd=0;
tm tmf;
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

}
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

}

   char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);
    
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     string fname=AMSISS;
     fname+=fpatb;
     char utmp[80];
     sprintf(utmp,"%u-%03u",yc,dc);
     fname+=utmp;
     fname+=fpate;
     ifstream fbin;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     if(!fbin){
// change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
         char *pch;
         pch=strtok(line,".");
         ISSAtt a;
         if(pch){
          strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
          time_t tf=mktime(&tmf)+tzd;
          pch=strtok(NULL,",");
          char tm1[80];
          sprintf(tm1,".%s",pch);
          double tc=tf+atof(tm1);
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Yaw=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Pitch=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Roll=atof(pch)*3.14159267/180;
	  fISSAtt.insert(make_pair(tc,a));
          
          if(tc>=t1 && tc<=t2){
           if(abs(bfound)!=2){
               fISSAtt.clear();
               fISSAtt.insert(make_pair(tc,a));
               bfound=bfound?2:-2;
//               cout <<" line "<<line<<" "<<tc<<endl;
           }
          }
         else if(tc<t1)bfound=1;
         else if(tc>t2){
             efound=1;
             break;
         }
      }   
     }
     }
     }
     else{
       cerr<<"AMSSetupR::LoadISSAtt-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
      dc=1;
      yc++;
     }
    }


int ret;
if(bfound>0 &&efound)ret=0;
else if(!bfound && !efound )ret=2;
else ret=1;
    cout<< "AMSSetupR::LoadISSAtt-I- "<<fISSAtt.size()<<" Entries Loaded"<<endl;

return ret;
}
//#endif

int AMSSetupR::getISSSA(AMSSetupR::ISSSA & a, double xtime){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSSA.clear();
if(fISSSA.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadISSSA(stime[0],stime[1]);
ssize=fISSSA.size();

}
}
#endif 


if (fISSSA.size()==0)return 2;



AMSSetupR::ISSSA_i k=fISSSA.lower_bound(xtime);
if(k==fISSSA.begin()){
a=k->second;
a.alpha*=180./3.14159267;
a.b1a*=180./3.14159267;
a.b3a*=180./3.14159267;
a.b1b*=180./3.14159267;
a.b3b*=180./3.14159267;
return 1;
}
if(k==fISSSA.end()){
k--;
a=k->second;
a.alpha*=180./3.14159267;
a.b1a*=180./3.14159267;
a.b3a*=180./3.14159267;
a.b1b*=180./3.14159267;
a.b3b*=180./3.14159267;
return 1;
}


if(xtime==k->first){
a=k->second;
a.alpha*=180./3.14159267;
a.b1a*=180./3.14159267;
a.b3a*=180./3.14159267;
a.b1b*=180./3.14159267;
a.b3b*=180./3.14159267;
return 1;
}


k--;

  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::ISSSA_i l=k;
  l++;
  tme[1]=l->first;
//  cout <<" alpha "<< k->second.alpha<<" "<<l->second.alpha<<endl;
//   cout << int(k->first)<<" "<<int(l->first)<<" "<<int(xtime)<<endl;
  double ang1=k->second.alpha;
  double ang2=l->second.alpha;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.alpha=atan2(s1,c1)*180/3.1415926;
{
  double ang1=k->second.b1a;
  double ang2=l->second.b1a;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.b1a=atan2(s1,c1)*180/3.1415926;
}

{
  double ang1=k->second.b3a;
  double ang2=l->second.b3a;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.b3a=atan2(s1,c1)*180/3.1415926;

}
{ 
 double ang1=k->second.b1b;
  double ang2=l->second.b1b;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.b1b=atan2(s1,c1)*180/3.1415926;
}

{
  double ang1=k->second.b3b;
  double ang2=l->second.b3b;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.b3b=atan2(s1,c1)*180/3.1415926;
}


  const float dmax=60;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}





int AMSSetupR::getISSAtt(float &roll, float&pitch,float &yaw,double xtime){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSAtt.clear();
if(fISSAtt.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadISSAtt(stime[0],stime[1]);
ssize=fISSAtt.size();

}
}
#endif 




if(fISSAtt.size()==0)return 2;

AMSSetupR::ISSAtt_i k=fISSAtt.lower_bound(xtime);
if(k==fISSAtt.begin()){
roll=k->second.Roll;
pitch=k->second.Pitch;
yaw=k->second.Yaw;
return 1;
}
if(k==fISSAtt.end()){
k--;
roll=k->second.Roll;
pitch=k->second.Pitch;
yaw=k->second.Yaw;
return 1;
}
if(xtime==k->first){
 roll=k->second.Roll;
 pitch=k->second.Pitch;
 yaw=k->second.Yaw;
 return 0;
}
  k--;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::ISSAtt_i l=k;
  l++;
  tme[1]=l->first;
  s0[0]=k->second.Roll;
  s0[1]=l->second.Roll;
  roll=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Pitch;
  s0[1]=l->second.Pitch;
  pitch=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Yaw;
  s0[1]=l->second.Yaw;
  yaw=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  const float dmax=301;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}


int AMSSetupR::getISSINTL(float &roll, float&pitch,float &yaw,double xtime){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSINTL.clear();
if(fISSINTL.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadISSINTL(stime[0],stime[1]);
ssize=fISSINTL.size();

}
}
#endif 




if(fISSINTL.size()==0)return 2;

AMSSetupR::ISSINTL_i k=fISSINTL.lower_bound(xtime);
if(k==fISSINTL.begin()){
roll=k->second.Roll;
pitch=k->second.Pitch;
yaw=k->second.Yaw;
return 1;
}
if(k==fISSINTL.end()){
k--;
roll=k->second.Roll;
pitch=k->second.Pitch;
yaw=k->second.Yaw;
return 1;
}
if(xtime==k->first){
 roll=k->second.Roll;
 pitch=k->second.Pitch;
 yaw=k->second.Yaw;
 return 0;
}
  k--;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::ISSINTL_i l=k;
  l++;
  tme[1]=l->first;
  s0[0]=k->second.Roll;
  s0[1]=l->second.Roll;
  roll=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Pitch;
  s0[1]=l->second.Pitch;
  pitch=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=k->second.Yaw;
  s0[1]=l->second.Yaw;
  yaw=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  const float dmax=60;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}


AMSSetupR::ISSCTRSR::ISSCTRSR(const  AMSSetupR::ISSCTRS &a){
r=sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
phi=atan2(a.y,a.x);
theta=asin(a.z/r);
v=sqrt(a.vx*a.vx+a.vy*a.vy+a.vz*a.vz);
vphi=atan2(a.vy,a.vx);
vtheta=asin(a.vz/v);
v=v/r;
r*=100000;
}


int AMSSetupR::getISSTLE(float RTP[3],float VelTP[3],double xtime){
bool notagain=true;
again:
#ifdef __ROOTSHAREDLIBRARY__
  static unsigned int ssize=0;
  static unsigned int stime[2]={1,1};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
  if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSData.clear();
  if(fISSData.size()==0){
    const int dt=120;
    if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
	stime[0]=fHeader.FEventTime-dt;
	stime[1]=fHeader.LEventTime+dt;
      }
      if(xtime<stime[0])stime[0]=xtime-dt;
      if(xtime>stime[1])stime[1]=xtime+dt;
      LoadISS(stime[0],stime[1]);
      ssize=fISSData.size();
    }
  }
#endif 
  bool resetmaybe=false;
  static unsigned int time=0;
#pragma omp threadprivate (time)
  const double pi2= 3.1415926535*2;
  if (fISSData.size()==0)return 2;
  unsigned int tl=0;
  for(ISSData_i i=fISSData.begin();i!=fISSData.end();i++){
    if(fabs(i->first-xtime)<fabs(tl-xtime))tl=i->first;
  }
  if(tl!=time && tl){
    resetmaybe=true;
    time=tl;
    ISSData_i i=fISSData.find(time);
    if(i!=fISSData.end() && ISSLoad((const char *)i->second.Name,(const char *)i->second.TL1,(const char*)i->second.TL2)){
      float NPhi;
      ::ISSGTOD(&RTP[0],&RTP[1],&RTP[2],&VelTP[0],&VelTP[1],&VelTP[2],&NPhi,xtime);
      RTP[2]=fmod(RTP[2]-NPhi+3.1415926*2,3.1415926*2);
      VelTP[2]=fmod(VelTP[2]-NPhi+pi2,pi2);
      return 0;
    }
    else{
      static int print=0;
      time=0;
#ifdef __ROOTSHAREDLIBRARY__
      if(resetmaybe && notagain){
       stime[1]=1;
       notagain=false;
       goto again;
      }
#endif      
     if(print++<10)cerr<<"AMSSetupR::getISSTLE-E-UnableToLoad "<<i->second.Name<<" "<<i->first<<endl;
      return 1 ;
    }	}
  if(time!=0){
    float NPhi;
    ::ISSGTOD(&RTP[0],&RTP[1],&RTP[2],&VelTP[0],&VelTP[1],&VelTP[2],&NPhi,xtime);
    RTP[2]=fmod(RTP[2]-NPhi+3.1415926*2,3.1415926*2);
    VelTP[2]=fmod(VelTP[2]-NPhi+pi2,pi2);
    return 0;
  }
  return 3;
}



int AMSSetupR::getISSCTRS(AMSSetupR::ISSCTRSR &a, double xtime)
{
  static int debug_level = 0;
  if (debug_level>0) cout<<Form("====CALLING AMSSetupR::%s====",__func__)<<endl;

  static int is_begin = 1;

#ifdef __ROOTSHAREDLIBRARY__ 
  static unsigned int ssize=0;
  static unsigned int rotsize=0;
  static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
#pragma omp threadprivate (rotsize)
  if (debug_level>2) cout<<"AMSSetupR::getISSCTRS stime[0]="<<stime[0]<<" stime[1]="<<stime[1]<<" xtime="<<xtime<<endl;
  if (debug_level>1) cout<<Form("1-    AMSSetupR::%s: ",__func__)<<" fISSCTRS.size()="<<fISSCTRS.size()<<endl;         
  if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1])) {
    fISSCTRS.clear();
    fRotMatrices.clear();
  }

  // clear ISSCTRS map already in header
  if (is_begin==1) {
    fISSCTRS.clear();
    is_begin=0;
  }

  if(fISSCTRS.size()==0 || fRotMatrices.size()==0){
    const int dt=120;
    if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
	stime[0]=fHeader.FEventTime-dt;
	stime[1]=fHeader.LEventTime+dt;
      }
      if(xtime<stime[0])stime[0]=xtime-dt;
      if(xtime>stime[1])stime[1]=xtime+dt;
      if(fISSCTRS.size()==0) {    
	if (debug_level>1) cout<<"AMSSetupR::getISSCTRS Going to call LoadISSCTRS"<<endl;
	LoadISSCTRS(stime[0],stime[1]);
	ssize=fISSCTRS.size();
      }
      if(!ssize && fRotMatrices.size()==0){
	int rot=LoadRotationMatrices(stime[0],stime[1]);
	rotsize=fRotMatrices.size();
      }
    }
  }
#endif 

  if (debug_level>1) cout<<Form("2-    AMSSetupR::%s: ",__func__)<<" fISSCTRS.size()="<<fISSCTRS.size()<<endl;
  static int prnt=0;
  if (fISSCTRS.size()==0) {
    if(prnt++<100)cerr<<"AMSSetupR::getISSCTRS-No-CTRS-Coord-Entries"<<endl; 
    return 2;
  }
  if (fRotMatrices.size()==0) {
    if(prnt++<100)cerr<<"AMSSetupR::getISSCTRS-No-RotMatrices-Entries"<<endl; 
    return 4;
  }

  ISSCTRS_i k=fISSCTRS.lower_bound(xtime);

  // if CTRS exists for this time, no need to interpolate
  if(k==fISSCTRS.begin()){
    a=ISSCTRSR(k->second);
    return 1;
  }
  if(k==fISSCTRS.end()){
    k--;
    a=ISSCTRSR(k->second);
    return 1;
  }
  if(xtime==k->first){
    a=ISSCTRSR(k->second);
    return 0;
  }
  k--;


  // get rotation matrix interpolated at the time of every CTRS
  // create map of (time,ICRS)
  time_t ktime; struct tm *ktm;
  int kk=0;
  for( k = fISSCTRS.begin(); k != fISSCTRS.end(); k++){
    ktime = k->first;
    ktm = gmtime(&ktime);
    TMatrixD RM = getRotationMatrix(ktime);
    const double earth_w = 7.2921158553e-5; //!< Earth's angular velocity [1/s]

    if (debug_level>1) {
      if (kk<3 || kk>fISSCTRS.size()-3)
	cout<<"===AMSSetupR::getISSCTRS::Looking for "<<fixed<<setprecision(3)<<(double) k->first<<" -- "
	    <<(ktm->tm_year+1900)<<"-"<<(ktm->tm_mon+1)<<"-"<<ktm->tm_mday<<":"<<ktm->tm_hour
	    <<":"<<ktm->tm_min<<":"<<ktm->tm_sec<<endl;
      
      if (kk<3 || kk>fISSCTRS.size()-3) { 
	cout<<"===AMSSetupR::getISSCTRS::getRotationMatrix-Dump "<<ktime<<' ';
	for(int r=0; r<3; r++) for(int c=0; c<3; c++) cout<<RM[r][c]<<' ';
	cout<<endl;                         
      }
      if (kk==3) cout<<"[...]"<<endl; kk++;
    }

    double U[3][3]={0}, dUdt[3][3]={0}, w[3]={0};
    double U_T[3][3]={0}, dUdt_T[3][3]={0};
    if (debug_level>2) cout<<"AMSSetupR::U_T: ";      
    for(int r=0; r<3; r++) for(int c=0; c<3; c++) {
	U_T[r][c] = RM[r][c];
	if (debug_level>2) cout<<U_T[r][c]<<' ';      
      }
    if (debug_level>2) cout<<endl;                    

    // apply Coordinate Transformation and store results
    ISSICRS k_icrs;
    //Position
    k_icrs.x = U_T[0][0]*k->second.x+U_T[0][1]*k->second.y+U_T[0][2]*k->second.z;
    k_icrs.y = U_T[1][0]*k->second.x+U_T[1][1]*k->second.y+U_T[1][2]*k->second.z;
    k_icrs.z = U_T[2][0]*k->second.x+U_T[2][1]*k->second.y+U_T[2][2]*k->second.z;

    //Velocity
    //Compute rotation matrix derivative
    //1. Transpose the rotation matrix
    for(int i=0; i<3; i++){
      U[i][i]=U_T[i][i];
      for(int j=i+1; j<3; j++){
	U[i][j]=U_T[j][i];
	U[j][i]=U_T[i][j];
      }
    }
    //2. Compute the derivative
    dUdt[0][0] = earth_w*(0*U[0][0]+1*U[1][0]+0*U[2][0]);
    dUdt[0][1] = earth_w*(0*U[0][1]+1*U[1][1]+0*U[2][1]);
    dUdt[0][2] = earth_w*(0*U[0][2]+1*U[1][2]+0*U[2][2]);
    dUdt[1][0] = earth_w*(-1*U[0][0]+0*U[1][0]+0*U[2][0]);
    dUdt[1][1] = earth_w*(-1*U[0][1]+0*U[1][1]+0*U[2][1]);
    dUdt[1][2] = earth_w*(-1*U[0][2]+0*U[1][2]+0*U[2][2]);
    dUdt[2][0] = earth_w*(0*U[0][0]+0*U[1][0]+0*U[1][0]);
    dUdt[2][1] = earth_w*(0*U[0][1]+0*U[1][1]+0*U[1][1]);
    dUdt[2][2] = earth_w*(0*U[0][2]+0*U[1][2]+0*U[1][2]);
    //3. Transpose the derivative rotation matrix
    for(int i=0; i<3; i++){
      dUdt_T[i][i]=dUdt[i][i];
      for(int j=i+1; j<3; j++){
	dUdt_T[i][j]=dUdt[j][i];
	dUdt_T[j][i]=dUdt[i][j];
      }
    }
    //4. Apply the transformation to the velocity
    k_icrs.vx = U_T[0][0]*k->second.vx+U_T[0][1]*k->second.vy+U_T[0][2]*k->second.vz;
    k_icrs.vx += dUdt_T[0][0]*k->second.x+dUdt_T[0][1]*k->second.y+dUdt_T[0][2]*k->second.z;
    k_icrs.vy = U_T[1][0]*k->second.vx+U_T[1][1]*k->second.vy+U_T[1][2]*k->second.vz;
    k_icrs.vy += dUdt_T[1][0]*k->second.x+dUdt_T[1][1]*k->second.y+dUdt_T[1][2]*k->second.z;
    k_icrs.vz = U_T[2][0]*k->second.vx+U_T[2][1]*k->second.vy+U_T[2][2]*k->second.vz;
    k_icrs.vz += dUdt_T[2][0]*k->second.x+dUdt_T[2][1]*k->second.y+dUdt_T[2][2]*k->second.z;

    fISSICRS.insert(make_pair(k->first,k_icrs));
  }

  // interpolate btw. previous and following for xtime's event
  ISSICRS c;
  ISSICRS_i i=fISSICRS.lower_bound(xtime);
  
  // if ICRS exists for this time, no need to interpolate
  if(i==fISSICRS.begin()){
    c=ISSICRS(i->second);
  }
  if(i==fISSICRS.end()){
    i--;
    c=ISSICRS(i->second);
  }
  if(xtime==i->first){
    c=ISSICRS(i->second);
  }
  i--;

  double tme_icrs[2]={0,0};
  tme_icrs[0]=i->first;
  ISSICRS_i m=i;
  m++;
  tme_icrs[1]=m->first;

  // interpolate with orbit at xtime
  // forward from tme_icrs[0]. backward from tme_icrs[1], then average with weight
  double dt0 = xtime-tme_icrs[0];
  double x0=i->second.x, y0=i->second.y, z0=i->second.z; 
  double vx0=i->second.vx, vy0=i->second.vy, vz0=i->second.vz;
  cout<<"AMSSetupR::getISSICRS:: xtime: "<<fixed<<xtime<<" - Found t0: "<<tme_icrs[0]<<" [dt0="<<dt0<<"]"<<endl;              // DEBUG
  cout<<"AMSSetupR::getISSICRS:: using point in ICRS map: "<<x0<<' '<<y0<<' '<<z0<<' '<<vx0<<' '<<vy0<<' '<<vz0<<endl;   // DEBUG
  double dt1 = xtime-tme_icrs[1];
  double x1=m->second.x, y1=m->second.y, z1=m->second.z; 
  double vx1=m->second.vx, vy1=m->second.vy, vz1=m->second.vz;

  AMSPoint B, B1;
  if (debug_level>2) cout<<"getISSICRS:: dt0-5/5dt0 "<<dt0-5./5.*dt0<<endl;
  B = getOrbitPoint(dt0-5./5.*dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt0-4/5dt0 "<<dt0-4./5.*dt0<<endl;
  B = getOrbitPoint(dt0-4./5.*dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt0-3/5dt0 "<<dt0-3./5.*dt0<<endl;
  B = getOrbitPoint(dt0-3./5.*dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt0-2/5dt0 "<<dt0-2./5.*dt0<<endl;
  B = getOrbitPoint(dt0-2./5.*dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt0-1/5dt0 "<<dt0-1./5.*dt0<<endl;
  B = getOrbitPoint(dt0-1./5.*dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt0 "<<dt0<<endl;                 
  B = getOrbitPoint(dt0, x0, y0, z0, vx0, vy0, vz0, 0);
  x0=m->second.x, y0=m->second.y, z0=m->second.z; 
  vx0=m->second.vx, vy0=m->second.vy, vz0=m->second.vz;
  if (debug_level>1) cout<<"getISSICRS:: next point in ICRS map: "<<x0<<' '<<y0<<' '<<z0<<' '<<vx0<<' '<<vy0<<' '<<vz0<<endl;   // DEBUG

  if (debug_level>2) cout<<"getISSICRS:: dt1-5/5dt1 "<<dt1-5./5.*dt1<<endl;
  B1 = getOrbitPoint(dt1-5./5.*dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt1-4/5dt1 "<<dt1-4./5.*dt1<<endl;
  B1 = getOrbitPoint(dt1-4./5.*dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt1-3/5dt1 "<<dt1-3./5.*dt1<<endl;
  B1 = getOrbitPoint(dt1-3./5.*dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt1-2/5dt1 "<<dt1-2./5.*dt1<<endl;
  B1 = getOrbitPoint(dt1-2./5.*dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt1-1/5dt1 "<<dt1-1./5.*dt1<<endl;
  B1 = getOrbitPoint(dt1-1./5.*dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  if (debug_level>2) cout<<"getISSICRS:: dt1 "<<dt1<<endl;                 
  B1 = getOrbitPoint(dt1, x1, y1, z1, vx1, vy1, vz1, 0);
  float W  = (float)(tme_icrs[1]-tme_icrs[0]); 
  float w0 = (tme_icrs[1]-xtime)/W;
  float w1 = (xtime-tme_icrs[0])/W;

  c.x = w0*B.x() + w1*B1.x();
  c.y = w0*B.y() + w1*B1.y();
  c.z = w0*B.z() + w1*B1.z();

  // now go back to CTRS with inverse matrix
  TMatrixD RMB = getRotationMatrix(xtime);
  if (debug_level>2) {                     
    cout<<"===getCTRSfromICRS::RotationMatrix-Dump "; 
    for(int R=0; R<3; R++) for(int C=0; C<3; C++) cout<<RMB[R][C]<<' '; 
    cout<<endl;
  }                                        

  Double_t det;
  RMB.Invert(&det);
  if (debug_level>2) {                     
    cout<<"===getCTRSfromICRS::InvertedRotationMatrix-Dump ";
    for(int R=0; R<3; R++) for(int C=0; C<3; C++) cout<<RMB[R][C]<<' '; 
    cout<<endl;                                                         
  }                                        

  ISSCTRS b;
  b.x = RMB[0][0]*c.x+RMB[0][1]*c.y+RMB[0][2]*c.z;
  b.y = RMB[1][0]*c.x+RMB[1][1]*c.y+RMB[1][2]*c.z;
  b.z = RMB[2][0]*c.x+RMB[2][1]*c.y+RMB[2][2]*c.z;

  // for the moment keep velocities with linear interpolation of CTRS
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  ISSCTRS_i l=k;
  l++;
  tme[1]=l->first;

// OLD
//   //  cout <<" alpha "<< k->second.alpha<<" "<<l->second.alpha<<endl;
//   //  cout << int(k->first)<<" "<<int(l->first)<<" "<<int(xtime)<<endl;
//   {
//     double ang1=k->second.x;
//     double ang2=l->second.x;
//     s0[0]=ang1;
//     s0[1]=ang2;
//     double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
//     // try quadratic interpolation
//     double v1=k->second.vx;
//     double v2=l->second.vx;
//     double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
//     double corr=(ang2-ang1)/(dx+1.e-16);
//     b.x=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);
//   }
//   {
//     double ang1=k->second.y;
//     double ang2=l->second.y;
//     s0[0]=ang1;
//     s0[1]=ang2;
//     double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
//     double v1=k->second.vy;
//     double v2=l->second.vy;
//     double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
//     double corr=(ang2-ang1)/(dx+1.e-16);
//     b.y=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);

//   }
//   {
//     double ang1=k->second.z;
//     double ang2=l->second.z;
//     s0[0]=ang1;
//     s0[1]=ang2;
//     double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
//     double v1=k->second.vz;
//     double v2=l->second.vz;
//     double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
//     double corr=(ang2-ang1)/(dx+1.e-16);
//     b.z=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);
//   }

  {
    double ang1=k->second.vx;
    double ang2=l->second.vx;
    s0[0]=ang1;
    s0[1]=ang2;
    double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
    b.vx=s1;
  }
  {
    double ang1=k->second.vy;
    double ang2=l->second.vy;
    s0[0]=ang1;
    s0[1]=ang2;
    double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
    b.vy=s1;
  }

  {
    double ang1=k->second.vz;
    double ang2=l->second.vz;
    s0[0]=ang1;
    s0[1]=ang2;
    double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
    b.vz=s1;
  }

  a=ISSCTRSR(b);
  const float dmax=60;
  if(fabs(tme[1]-tme[0])>dmax) return 3;
  else return 0;
}


TMatrixD AMSSetupR::getRotationMatrix(double xtime) 
{
  static int debug_level = 0;
  if (debug_level>0) cout<<Form("====CALLING AMSSetupR::%s====",__func__)<<endl;

  TMatrixD a(3,3);

  if (fRotMatrices.size()==0) {
    cout<<"AMSSetupR::getRotationMatrix-No-Matrix-Entries"<<endl; 
    for(int r=0; r<3; r++) for(int c=0; c<3; c++) a[r][c]=0;	 
    return a;
  }

  RotMatrices_i k;
  for( k = fRotMatrices.begin(); k != fRotMatrices.end(); k++) {
    if (k->first > xtime ) {
      if (k == fRotMatrices.begin()) {
	return k->second;
      }
      else { k--; break;}
    }
    else if (xtime==k->first) {
      return k->second;
    }
  }

  if (k==fRotMatrices.end()) {
    k--;
    return k->second;
  }
 
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  RotMatrices_i l=k;
  l++;
  tme[1]=l->first;

  // interpolate linearly
  for(int r=0; r<3; r++) for(int c=0; c<3; c++) {
      double par1=k->second[r][c];
      double par2=l->second[r][c];
      s0[0]=par1;
      s0[1]=par2;
      double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
      if (debug_level>2) cout<<Form("AMSSetupR::getRotationMatrix-- a[%d][%d]  par1 %f   par2 %f   s1 %f",r,c,par1,par2,s1)<<endl;
      a[r][c]=s1;
  }

  return a;
}



int AMSSetupR::getGPSWGS84(AMSSetupR::GPSWGS84R & a, double xtime){
//  gpswgs84 time is gps time
xtime+=AMSEventR::gpsdiff(floor(xtime));
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fGPSWGS84.clear();
if(fGPSWGS84.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadGPSWGS84(stime[0],stime[1]);
ssize=fGPSWGS84.size();

}
}
#endif 
if (fGPSWGS84.size()==0)return 2;



AMSSetupR::GPSWGS84_i k=fGPSWGS84.lower_bound(xtime);
if(k==fGPSWGS84.begin()){
a=GPSWGS84R(k->second);
return 1;
}
if(k==fGPSWGS84.end()){
k--;
a=GPSWGS84R(k->second);
return 1;
}

if(xtime==k->first){
a=GPSWGS84R(k->second);
return 0;
}

k--;
  AMSSetupR::GPSWGS84 b;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::GPSWGS84_i l=k;
  l++;
  tme[1]=l->first;
//  cout <<" alpha "<< k->second.alpha<<" "<<l->second.alpha<<endl;
//   cout << int(k->first)<<" "<<int(l->first)<<" "<<int(xtime)<<endl;
{
  double ang1=k->second.x;
  double ang2=l->second.x;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
// try quadratic interpolation
  double v1=k->second.vx;
  double v2=l->second.vx;
  double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
  double corr=(ang2-ang1)/(dx+1.e-10);
  b.x=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);
}
{
  double ang1=k->second.y;
  double ang2=l->second.y;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  double v1=k->second.vy;
  double v2=l->second.vy;
  double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
  double corr=(ang2-ang1)/(dx+1.e-10);
  b.y=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);
}
{
  double ang1=k->second.z;
  double ang2=l->second.z;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  double v1=k->second.vz;
  double v2=l->second.vz;
  double dx=v1*(tme[1]-tme[0]) +(v2-v1)*(tme[1]-tme[0])/2;
  double corr=(ang2-ang1)/(dx+1.e-10);
  b.z=s0[0]+corr*(v1*(xtime-tme[0]) +(v2-v1)*(xtime-tme[0])*(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)/2);
}
{
  double ang1=k->second.vx;
  double ang2=l->second.vx;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.vx=s1;
}
{
  double ang1=k->second.vy;
  double ang2=l->second.vy;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.vy=s1;
}

{
  double ang1=k->second.vz;
  double ang2=l->second.vz;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.vz=s1;
}

a=GPSWGS84R(b);
  const float dmax=600;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}




int AMSSetupR::IsBadRun(string & reason, unsigned int time,unsigned int run){
int retn=0;
string mr=reason;
reason="";
if(fBadRun.size()==0){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int stime=0;
#pragma omp threadprivate (stime)
if(!time && run)time=run;
if(stime!=time){
stime=time;
LoadISSBadRun();
}

#endif
}
if (fBadRun.size()==0)return 2;
pair <BadRun_i,BadRun_i> ret;
ret=fBadRun.equal_range(run?run:fHeader.Run);
for (BadRun_i it=ret.first;it!=ret.second;++it){
  BadRun f=it->second;
  int pos=-1;
  if(mr.size()>0){
       pos=f.reason.find(mr);
  }
  else pos=0;
  if(it->first!=f.run){
     cerr<<"AMSSetupR::IsBadRun-E-LogicError" <<it->first<<" "<<f.run<<endl;
     return 3;
  }
  if((!time || (time>=f.begin && time<=f.end)) && pos>=0){
   retn=1;
   reason+=f.reason;
   reason+=" ";  
  }
 }
 return retn;


}
#include <dirent.h>
bool AMSSetupR::LoadISSBadRun(){
  fBadRun.clear();
  char AMSl[]="/afs/cern.ch/ams/Offline/AMSDataDir";
 char * AMSDD=getenv("AMSDataDir");
 if(!AMSDD || !strlen(AMSDD))AMSDD=AMSl;
  string amsdd=AMSDD;
  amsdd+="/BadRuns/"; 
  int total=0;
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
	dirent64 ** namelistsubdir;
	int nptrdir=scandir64(amsdd.c_str(),&namelistsubdir,NULL,NULL);
#endif
#ifdef __DARWIN__
	dirent ** namelistsubdir;
	int nptrdir=scandir(amsdd.c_str(),&namelistsubdir,NULL,NULL);
#endif
	for(int is=0;is<nptrdir;is++){
	  TString b(amsdd.c_str());
	  b+=namelistsubdir[is]->d_name;
          while(b.Index("_")>=0){b=b(b.Index("_")+1,b.Length());}
          if(b.Index(":")>=0 && TString(b(0,b.Index(":"))).IsDigit() && TString(b(b.Index(":")+1,b.Length())).IsDigit()){
           unsigned int bi=atoll(TString(b(0,b.Index(":"))).Data());
           unsigned int be=atoll(TString(b(b.Index(":")+1,b.Length())));
           string c=amsdd;
           c+=  namelistsubdir[is]->d_name;          
           ifstream ftxt;
           ftxt.clear();
           ftxt.open(c.c_str());
           if(ftxt){
           //cout<<"AMSSetupR::LoadISSBadRuns-I-OpenedFile "<<c<<endl;
             int totr=0;
             while(ftxt.get()=='#')ftxt.ignore(1024,'\n');
             ftxt.seekg(int(ftxt.tellg())-sizeof(char));
             string ss;
             ftxt>>ss;
             while(ss.find(',')<ss.size()){
              int pos=ss.find(',');
              TString rg(ss.substr(0,pos).c_str());
              ss=ss.substr(ss.find(',')+1,ss.size());
              if(rg.IsDigit()){
                //  foundrun
                unsigned int run=atoll(rg.Data());
                if(run>=bi && run<=be){
                 fBadRun.insert(make_pair(run,BadRun((c.substr(c.rfind('/')+1,c.rfind('_'))).c_str(),run)));
                 totr++;
                 total++; 
               }
                else{
                 cerr<<"AMSSetupR::LoadISSBadRuns-E-runOutOfRange "<<run<<" "<<bi<<" "<<be<<endl;
                }
              }
              else{
               bool ok=false;
               TString srun=TString(rg(0,rg.Index("_")));
               TString sbeg= TString(rg(rg.Index("_")+1,rg.Index(":")-rg.Index("_")-1));
               TString send=TString(rg(rg.Index(":")+1,rg.Length()-rg.Index(":")-1));
//               cout <<" srun "<<srun<<endl;
//               cout <<" sbeg "<<sbeg<<endl;
//               cout <<" send "<<send<<endl;
               if(srun.IsDigit() && sbeg.IsDigit() && send.IsDigit()){
                unsigned int run=atoll(srun);  
                unsigned int beg=atoll(sbeg);  
                unsigned int end=atoll(send);  
//               cout <<"ber "<<beg<<" "<<end<<" "<<run<<" "<<bi<<" "<<be<<" "<<endl;
                if(beg<=end && run>=bi && run<=be){
                  fBadRun.insert(make_pair(run,BadRun((c.substr(c.rfind('/')+1,c.rfind('_'))).c_str(),run)));
                  ok=true;
                  totr++;
                  total++; 
                }         
               
               }
               if(!ok)cerr<<"AMSSetupR::LoadISSBadRuns-E-BadEntryFor "<<c<<" "<<rg<<endl;
              }
             
             } 
           if(totr){
	     //cout<<"AMSSetupR::LoadISSBadRuns-I-TotalOf "<<totr<<" LoadedFor "<<c<<endl;
           }  
           }
           ftxt.close();

        }
	  free( namelistsubdir[is]);
        }
	if(nptrdir>0){
	  free (namelistsubdir);
	}
                  cout<<"AMSSetupR::LoadISSBadRuns-I-GrandTotalOf "<<total<<" BadRunsLoaded "<<endl;

return total>0;
}       

int AMSSetupR::getISSGTOD(AMSSetupR::ISSGTOD & a, double xtime){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fISSGTOD.clear();
if(fISSGTOD.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadISSGTOD(stime[0],stime[1]);
ssize=fISSGTOD.size();

}
}
#endif 
if (fISSGTOD.size()==0)return 2;




AMSSetupR::ISSGTOD_i k=fISSGTOD.lower_bound(xtime);
if(k==fISSGTOD.begin()){
a=(k->second);
return 1;
}
if(k==fISSGTOD.end()){
k--;
a=(k->second);
return 1;
}

if(xtime==k->first){
a=(k->second);
return 0;
}
  k--;
  AMSSetupR::ISSGTOD b;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::ISSGTOD_i l=k;
  l++;
  tme[1]=l->first;
//  cout <<" alpha "<< k->second.alpha<<" "<<l->second.alpha<<endl;
//   cout << int(k->first)<<" "<<int(l->first)<<" "<<int(xtime)<<endl;
{
  double ang1=k->second.r;
  double ang2=l->second.r;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.r=s1;
}
   double bc=0;
   double bs=0;
{
  double ang1=cos(k->second.phi);
  double ang2=cos(l->second.phi);
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  bc=s1;
}
{
  double ang1=sin(k->second.phi);
  double ang2=sin(l->second.phi);
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  bs=s1;
}
  b.phi=atan2(bs,bc);
{
  double ang1=k->second.theta;
  double ang2=l->second.theta;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.theta=s1;
}
{
  double ang1=cos(k->second.vphi);
  double ang2=cos(l->second.vphi);
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  bc=s1;
}
{
  double ang1=sin(k->second.vphi);
  double ang2=sin(l->second.vphi);
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  bs=s1;
}
  b.vphi=atan2(bs,bc);
{
  double ang1=k->second.vtheta;
  double ang2=l->second.vtheta;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.vtheta=s1;
}

{
  double ang1=k->second.v;
  double ang2=l->second.v;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  b.v=s1;
}

a=b;
  const float dmax=60;
  if(fabs(tme[1]-tme[0])>dmax)return 3;
  else return 0;


}

int AMSSetupR::getRTI(AMSSetupR::RTI & a, unsigned int  xtime){
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={1,1};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
static int vrti=-1;
#pragma omp threadprivate (vrti)
if(AMSSetupR::RTI::Version!=vrti){
  cout<<"<<----------RTI Version="<<AMSSetupR::RTI::Version<<"---------->>"<<endl;
  vrti=AMSSetupR::RTI::Version;
}
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fRTI.clear();
if(fRTI.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadRTI(stime[0],stime[1]);
ssize=fRTI.size();
//cout <<" load rti "<<stime[0]<<" "<<stime[1]<<" "<<ssize<<endl;

}
}
#endif
if (fRTI.size()==0)return 2;
AMSSetupR::RTI_i k=fRTI.lower_bound(xtime);
if(k==fRTI.begin()){//first
a=(k->second);
return 1;
}
if(k==fRTI.end()){//end goback
k--;
a=(k->second);
return 1;
}

if(xtime==k->first){//find
a=(k->second);
return 0;
}

cerr<<"AMSSetupR::RTI- Missing Time Information"<<xtime<<endl;
return 2;
}

int AMSSetupR::getAMSSTK(AMSSetupR::AMSSTK & a, double xtime){
// 
// note amsstk table is in  POCC time
//
#ifdef __ROOTSHAREDLIBRARY__
static unsigned int ssize=0;
static unsigned int stime[2]={0,0};
#pragma omp threadprivate (stime)
#pragma omp threadprivate (ssize)
if(stime[0] && stime[1] && (xtime<stime[0] || xtime>stime[1]))fAMSSTK.clear();
if(fAMSSTK.size()==0){
const int dt=120;
if(xtime<stime[0] || xtime>stime[1] || ssize){
unsigned int ref=fHeader.Run;
if(AMSEventR::Head()){
  if(AMSEventR::Head()->nMCEventg()>0){
     ref=fHeader.FEventTime;
}
}
stime[0]=ref?ref-dt:xtime-dt;
stime[1]=ref?ref+3600:xtime+3600;
if(fHeader.FEventTime-dt<ref && fHeader.LEventTime+1>ref && ref!=0){
 stime[0]=fHeader.FEventTime-dt;
 stime[1]=fHeader.LEventTime+dt;
}
if(xtime<stime[0])stime[0]=xtime-dt;
if(xtime>stime[1])stime[1]=xtime+dt;
LoadAMSSTK(stime[0],stime[1]);
ssize=fAMSSTK.size();

}
}
#endif 


if (fAMSSTK.size()==0)return 2;


AMSSetupR::AMSSTK_i k=fAMSSTK.lower_bound(xtime);
if(k==fAMSSTK.begin()){
a=(k->second);
return 1;
}
if(k==fAMSSTK.end()){
k--;
a=(k->second);
return 1;
}

if(xtime==k->first){
a=(k->second);
return 0;
}
  k--;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::AMSSTK_i l=k;
  l++;
  tme[1]=l->first;
{
  double ang1=k->second.cam_or*3.1415926/180;
  double ang2=l->second.cam_or*3.1415926/180;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.cam_or=atan2(s1,c1)*180/3.1415926;

}


{
  double ang1=k->second.cam_dec*3.1415926/180;
  double ang2=l->second.cam_dec*3.1415926/180;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.cam_dec=atan2(s1,c1)*180/3.1415926;

}

{
  double ang1=k->second.cam_ra*3.1415926/180;
  double ang2=l->second.cam_ra*3.1415926/180;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.cam_ra=atan2(s1,c1)*180/3.1415926;

}
{
  double ang1=k->second.ams_ra*3.1415926/180;
  double ang2=l->second.ams_ra*3.1415926/180;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.ams_ra=atan2(s1,c1)*180/3.1415926;
}
{
  double ang1=k->second.ams_dec*3.1415926/180;
  double ang2=l->second.ams_dec*3.1415926/180;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-16)*(s0[1]-s0[0]);
  a.ams_dec=atan2(s1,c1)*180/3.1415926;

}
  a.cam_id=k->second.cam_id;
  if(k->second.cam_id!=l->second.cam_id)return 3;
  const float dmax=60;
  if(fabs(tme[1]-tme[0])>dmax)return 4;
   
  else return 0;
}




int AMSSetupR::LoadISSSA(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;


   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/isssa/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISSSA");
   if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();



if(t1>t2){
cerr<< "AMSSetupR::LoadISSSA-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSSA-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}
const char fpatb[]="ISS_solar_arrays_";
const char fpate[]="-24H.csv";
const char fpate2[]="-24h.csv";

// convert time to GMT format

// check tz
   unsigned int tzd=0;
    tm tmf;
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

}
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSAtt-I-TZDSeconds "<<tzd<<endl;

}

   char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);
    
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     string fname=AMSISS;
     fname+=fpatb;
     char utmp[80];
     sprintf(utmp,"%u_%03u",yc,dc);
     fname+=utmp;
     fname+=fpate;
     ifstream fbin;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     if(!fbin){
// change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u_%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
         char *pch;
         pch=strtok(line,".");
         ISSSA a;
         if(pch){
          strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
          //cout <<" pch "<<pch<<endl;
          time_t tf=mktime(&tmf)+tzd;
          pch=strtok(NULL,",");
          char tm1[80];
          sprintf(tm1,".%s",pch);
          double tc=tf+atof(tm1);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.alpha=atof(pch)*3.14159267/180;
          //cout <<" alpha "<<a.alpha<<" "<<tf<<" "<<tc<<endl;
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.b1a=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.b3a=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.b1b=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.b3b=atof(pch)*3.14159267/180;
           fISSSA.insert(make_pair(tc,a));
          
          if(tc>=t1 && tc<=t2){
           if(abs(bfound)!=2){
               fISSSA.clear();
               fISSSA.insert(make_pair(tc,a));
               bfound=bfound?2:-2;
//               cout <<" line "<<line<<" "<<tc<<endl;
           }
          }
         else if(tc<t1)bfound=1;
         else if(tc>t2){
             efound=1;
             break;
         }
      }   
     }
     }
     }
     else{
       cerr<<"AMSSetupR::LoadISSSA-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
      dc=1;
      yc++;
     }
    }


int ret;
if(bfound>0 &&efound)ret=0;
else if(!bfound && !efound )ret=2;
else ret=1;
    cout<< "AMSSetupR::LoadISSSA-I- "<<fISSSA.size()<<" Entries Loaded"<<endl;

return ret;
}


int AMSSetupR::LoadISSINTL(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;

   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
   if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();



if(t1>t2){
cerr<< "AMSSetupR::LoadISSINTL-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSINTL-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}
const char fpatb[]="ISS_ATT_EULR_INTL-";
const char fpate[]="-24H.csv";
const char fpate2[]="-24h.csv";

// convert time to GMT format

// check tz
   unsigned int tzd=0;
tm tmf;
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSINTL-I-TZDSeconds "<<tzd<<endl;

}
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSINTL-I-TZDSeconds "<<tzd<<endl;

}

   char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);
    
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     string fname=AMSISS;
     fname+=fpatb;
     char utmp[80];
     sprintf(utmp,"%u-%03u",yc,dc);
     fname+=utmp;
     fname+=fpate;
     ifstream fbin;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     if(!fbin){
// change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
         char *pch;
         pch=strtok(line,".");
         ISSINTL a;
         if(pch){
          strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
          time_t tf=mktime(&tmf)+tzd;
          pch=strtok(NULL,",");
          char tm1[80];
          sprintf(tm1,".%s",pch);
          double tc=tf+atof(tm1);
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Yaw=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Pitch=atof(pch)*3.14159267/180;
          pch=strtok(NULL,",");
          if(!pch)continue;
          a.Roll=atof(pch)*3.14159267/180;
	  fISSINTL.insert(make_pair(tc,a));
          
          if(tc>=t1 && tc<=t2){
           if(abs(bfound)!=2){
               fISSINTL.clear();
               fISSINTL.insert(make_pair(tc,a));
               bfound=bfound?2:-2;
//               cout <<" line "<<line<<" "<<tc<<endl;
           }
          }
         else if(tc<t1)bfound=1;
         else if(tc>t2){
             efound=1;
             break;
         }
      }   
     }
     }
     }
     else{
       cerr<<"AMSSetupR::LoadISSINTL-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
      dc=1;
      yc++;
     }
    }


int ret;
if(bfound>0 &&efound)ret=0;
else if(!bfound && !efound )ret=2;
else ret=1;
    cout<< "AMSSetupR::LoadISSINTL-I- "<<fISSINTL.size()<<" Entries Loaded"<<endl;

return ret;
}



int AMSSetupR::LoadRotationMatrices(unsigned int t1, unsigned int t2)
{
  static int debug_level = 0;
  if (debug_level>0) cout<<Form("====CALLING AMSSetupR::%s====",__func__)<<endl;

  string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
  char postfix[]="/altec/";
  char * AMSDataDir=getenv("AMSDataDir");
  if (AMSDataDir && strlen(AMSDataDir)){
    AMSISSlocal=AMSDataDir;
  }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
  if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();


  if (t1>t2) {
    cerr<< "AMSSetupR::LoadRotationMatrices-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if (t2-t1>864000) {
    cerr<< "AMSSetupR::LoadRotationMatrices-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
  }

  const char fpatb[]="CTRS_ICRS_Matrices/CTRS_ICRS_Matrix_";
  const char fpate[]="-24H.csv";

  // convert time to GMT format
  // check tz
  unsigned int tzd=0;
  tm tmf;
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    //tc=mktime(&tmf);
    tzd=tz-tc;
    if (debug_level>2) cout<< "1-AMSSetupR::LoadRotationMatrices-I-TZDSeconds "<<tzd<<endl;
  }
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    //tc=mktime(&tmf);
    tzd=tz-tc;
    if (debug_level>2) cout<< "2-AMSSetupR::RotationMatrices-I-TZDSeconds "<<tzd<<endl;
  }

  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  if (debug_level>1) cout<<"Run-60   (t1) = "<<t1<<", corr. day of the year: "<<db<<endl;
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
  if (debug_level>1) cout<<"Run+3600 (t2) = "<<t2<<", corr. day of the year: "<<de<<endl;
  
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;

    while(yc<ye || (dc<=de && yc==ye)){
    
    string fname=AMSISS;
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u_%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    if (debug_level>0) cout<<"RotationMatrices file: "<<fname<<endl; 

    ifstream _rotmatrix_file;
    if (_rotmatrix_file.is_open()) _rotmatrix_file.close();
    _rotmatrix_file.clear();
    _rotmatrix_file.open(fname.c_str(), ifstream::in );
    _rotmatrix_file.seekg(0,ios::beg);
    
    if (_rotmatrix_file){
      while(_rotmatrix_file.good() && !_rotmatrix_file.eof()){
	char line[320];
	_rotmatrix_file.getline(line,319);
	
	if (isdigit(line[0])) {
 	  TMatrixD CTRS2ICRS(3,3);
	  double U_T[3][3]={0};
	  char *pch;
	  pch=strtok(line,".");
	  if (pch) {
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
	    sprintf(tm1,".%s",pch);
	    double tc=tf+atof(tm1);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.') continue;
	    U_T[0][0]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[0][1]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[0][2]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.') continue;
	    U_T[1][0]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[1][1]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[1][2]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.') continue;
	    U_T[2][0]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[2][1]=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    U_T[2][2]=atof(pch);
	    
	    for(int r=0; r<3; r++) for(int c=0; c<3; c++) CTRS2ICRS[r][c]=U_T[r][c];
	     
	    fRotMatrices.insert(make_pair(tc,CTRS2ICRS));
	    
	    if (tc>=t1 && tc<=t2) {
	      if (abs(bfound)!=2) {
		fRotMatrices.clear();
		fRotMatrices.insert(make_pair(tc,CTRS2ICRS));
		bfound=bfound?2:-2;
	      }
	    }
	    else if (tc<t1) bfound=1;
	    else if (tc>t2) {
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else {
      cerr<<"AMSSetupR::LoadRotationMatrices-E-UnabletoOpenFile "<<fname<<endl;
    }
    dc++;
    if (dc>366) {
      dc=1;
      yc++;
    }
  }

  int ret;
  if (bfound>0 && efound) ret=0;
  else if (!bfound && !efound ) ret=2;
  else ret=1;
  if (debug_level>0) cout<< "AMSSetupR::LoadRotationMatrices-I- "<<fRotMatrices.size()<<" Entries Loaded"<<endl;

  return ret;
}




int AMSSetupR::LoadISSCTRS(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;

  string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";

  char postfix[]="/isssa/";  
  char * AMSDataDir=getenv("AMSDataDir");
  if (AMSDataDir && strlen(AMSDataDir)){  
    AMSISSlocal=AMSDataDir;
  }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISSSA");
  if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();


  if(t1>t2){
    cerr<< "AMSSetupR::LoadISSCTRS-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSCTRS-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
  }
  const char fpatb[]="ISS_CTRS_Vectors_";
  const char fpatb2[]="ISS_CTRS_vectors_";
  const char fpate[]="-24H.csv";
  const char fpate2[]="-24h.csv";

  // convert time to GMT format

  // check tz
  unsigned int tzd=0;
  tm tmf;
  {
    char tmp2[255];
    {
      time_t tz=t1;
      strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
      strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
      time_t tc=mktime(&tmf);
      tc=mktime(&tmf);
      tzd=tz-tc;
      cout<< "AMSSetupR::LoadISSCTRS-I-TZDSeconds "<<tzd<<endl;
    }
    {
      time_t tz=t1;
      strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
      strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
      time_t tc=mktime(&tmf);
      tc=mktime(&tmf);
      tzd=tz-tc;
      cout<< "AMSSetupR::LoadISSCTRS-I-TZDSeconds "<<tzd<<endl;
    }
  }

  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
    
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
    string fname=AMSISS;
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u_%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    cout<<"AMSSetupR::LoadISSCTRS-ISSCTRS file: "<<fname<<endl; 

    ifstream fbin;
    fbin.close();    
    fbin.clear();
    fbin.open(fname.c_str());
    if(!fbin){
      // change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u_%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
    }
    if(!fbin){
      // change Vec to vec
      fname=AMSISS;
      fname+=fpatb2;
      char utmp[80];
      sprintf(utmp,"%u_%03u",yc,dc);
      fname+=utmp;
      fname+=fpate;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
    }
    if(!fbin){
      // change 24h back
      fname=AMSISS;
      fname+=fpatb2;
      char utmp[80];
      sprintf(utmp,"%u_%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
    }
    if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if(isdigit(line[0])){
	  char *pch;
	  pch=strtok(line,".");
	  ISSCTRS a;
	  if(pch){
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    //cout <<" pch "<<pch<<endl;
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
	    sprintf(tm1,".%s",pch);
	    double tc=tf+atof(tm1);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.')continue;
	    a.x=atof(pch);
	    //cout <<" alpha "<<a.x<<" "<<tf<<" "<<tc<<endl;
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
	    a.y=atof(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
	    a.z=atof(pch);
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
	    a.vx=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
	    a.vy=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if(!pch)continue;
	    if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
	    a.vz=atof(pch)/1000.;
	    fISSCTRS.insert(make_pair(tc,a));
          
	    if(tc>=t1 && tc<=t2){
	      if(abs(bfound)!=2){
		fISSCTRS.clear();
		fISSCTRS.insert(make_pair(tc,a));
		bfound=bfound?2:-2;
		//               cout <<" line "<<line<<" "<<tc<<endl;
	      }
	    }
	    else if(tc<t1)bfound=1;
	    else if(tc>t2){
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else{
      cerr<<"AMSSetupR::LoadISSCTRS-E-UnabletoOpenFile "<<fname<<endl;
    }
    dc++;
    if(dc>366){
      dc=1;
      yc++;
    }
  }


  int ret;
  if(bfound>0 &&efound)ret=0;
  else if(!bfound && !efound )ret=2;
  else ret=1;
  cout<< "AMSSetupR::LoadISSCTRS-I- "<<fISSCTRS.size()<<" Entries Loaded"<<endl;

  return ret;
}



int AMSSetupR::LoadGPSWGS84(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;

   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
   if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();


  if (t1>t2) {
    cerr<< "AMSSetupR::LoadGPSWGS84-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
    return 2;
  }
  else if (t2-t1>864000) {
    cerr<< "AMSSetupR::LoadGPSWGS84-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
  }
  const char fpatb[]="GPS/GPS_WGS84_Vectors_";
  const char fpate[]="-24H.csv";
  
  // convert time to GMT format
  // check tz
  unsigned int tzd=0;
  tm tmf;

  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadGPSWGS84-I-TZDSeconds "<<tzd<<endl;
  }
  {
    char tmp2[255];
    time_t tz=t1;
    strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
    strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadGPSWGS84-I-TZDSeconds "<<tzd<<endl;
  }
  
  char tmp[255];
  time_t utime=t1;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int yb=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int db=atol(tmp);
  utime=t2;
  strftime(tmp, 40, "%Y", gmtime(&utime));
  unsigned int ye=atol(tmp);
  strftime(tmp, 40, "%j", gmtime(&utime));
  unsigned int de=atol(tmp);
  
  unsigned int yc=yb;
  unsigned int dc=db;
  int bfound=0;
  int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){

    string fname=AMSISS;
    fname+=fpatb;
    char utmp[80];
    sprintf(utmp,"%u_%03u",yc,dc);
    fname+=utmp;
    fname+=fpate;
    ifstream fbin;
    fbin.close();    
    fbin.clear();
    fbin.open(fname.c_str());
    cout<<"GPS file: "<<fname<<endl; 

    if (fbin) {
      while(fbin.good() && !fbin.eof()){
        char line[120];
        fbin.getline(line,119);
        
        if (isdigit(line[0])) {
	  char *pch;
	  pch=strtok(line,".");
	  GPSWGS84 a;
	  if (pch) {
	    strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
	    time_t tf=mktime(&tmf)+tzd;
	    pch=strtok(NULL,",");
	    char tm1[80];
//	    sprintf(tm1,".%s",pch);
//	    double tc=tf+atof(tm1);
            double frac=atol(pch);
            frac/=1000;
	    double tc=tf+frac;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.') continue;
	    a.x=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.y=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.z=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.vx=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.vy=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.vz=atof(pch)/1000.;
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
            sscanf(pch,"%x",&(a.val));
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.fom=atof(pch);
	    pch=strtok(NULL,",");
	    if (!pch) continue;
	    if (!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.') continue;
	    a.fomv=atof(pch);

	    fGPSWGS84.insert(make_pair(tc,a));
          
	    if (tc>=t1 && tc<=t2) {
	      if (abs(bfound)!=2) {
		fGPSWGS84.clear();
		fGPSWGS84.insert(make_pair(tc,a));
		bfound=bfound?2:-2;
	      }
	    }
	    else if (tc<t1)bfound=1;
	    else if (tc>t2) {
	      efound=1;
	      break;
	    }
	  }   
	}
      }
    }
    else {
      cout<<"AMSSetupR::LoadGPSWGS84-E-UnabletoOpenFile "<<fname<<endl;
    }
    dc++;
    if (dc>366) {
      dc=1;
      yc++;
    }
  }

  int ret;
  if (bfound>0 &&efound) ret=0;
  else if (!bfound && !efound ) ret=2;
  else ret=1;
  cout<< "AMSSetupR::LoadGPSWGS84-I- "<<fGPSWGS84.size()<<" Entries Loaded"<<endl;

  return ret;
}



int AMSSetupR::LoadISSGTOD(unsigned int t1, unsigned int t2){
if(!LoadISSMC)return 3;

   string AMSISSlocal="/afs/cern.ch/ams/Offline/AMSDataDir";
   char postfix[]="/altec/";
   char * AMSDataDir=getenv("AMSDataDir");
   if (AMSDataDir && strlen(AMSDataDir)){
     AMSISSlocal=AMSDataDir;
   }
  
  AMSISSlocal+=postfix;
  const char * AMSISS=getenv("AMSISS");
   if (!AMSISS || strlen(AMSISS))AMSISS=AMSISSlocal.c_str();


if(t1>t2){
cerr<< "AMSSetupR::LoadISSGTOD-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSGTOD-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
if(t1<1305000000){
  t1=t2-864000;
}
else{
   t2=t1+864000;
}
}
const char fpatb[]="GTOD/ISS_SV_GTOD-";
const char fpatb2[]="GTOD/ISS_SV_GTOD_01_";
const char fpate[]="-24H.csv";
const char fpate2[]="-24h.csv";

// convert time to GMT format

// check tz
   unsigned int tzd=0;
    tm tmf;
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSGTOD-I-TZDSeconds "<<tzd<<endl;

}
{
char tmp2[255];
   time_t tz=t1;
          strftime(tmp2,80,"%Y_%j:%H:%M:%S",gmtime(&tz));
          strptime(tmp2,"%Y_%j:%H:%M:%S",&tmf);
    time_t tc=mktime(&tmf);
    tc=mktime(&tmf);
    tzd=tz-tc;
    cout<< "AMSSetupR::LoadISSGTOD-I-TZDSeconds "<<tzd<<endl;

}

   char tmp[255];
    time_t utime=t1;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int yb=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int db=atol(tmp);
    utime=t2;
    strftime(tmp, 40, "%Y", gmtime(&utime));
    unsigned int ye=atol(tmp);
    strftime(tmp, 40, "%j", gmtime(&utime));
    unsigned int de=atol(tmp);
    
    unsigned int yc=yb;
    unsigned int dc=db;
    int bfound=0;
    int efound=0;
    while(yc<ye || (dc<=de && yc==ye)){
     string fname=AMSISS;
     fname+=fpatb;
     char utmp[80];
     sprintf(utmp,"%u-%03u",yc,dc);
     fname+=utmp;
     fname+=fpate;
     ifstream fbin;
     fbin.close();    
     fbin.clear();
     fbin.open(fname.c_str());
     if(!fbin){
// change 24H to 24h
      fname=AMSISS;
      fname+=fpatb;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(!fbin){
// change Vec to vec
      fname=AMSISS;
      fname+=fpatb2;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(!fbin){
// change 24h back
      fname=AMSISS;
      fname+=fpatb2;
      char utmp[80];
      sprintf(utmp,"%u-%03u",yc,dc);
      fname+=utmp;
      fname+=fpate2;
      fbin.clear();
      fbin.close();    
      fbin.clear();
      fbin.open(fname.c_str());
     }
     if(fbin){
      while(fbin.good() && !fbin.eof()){
        char line[4096];
        fbin.getline(line,4095);
        
        if(isdigit(line[0])){
         char *pch;
         pch=strtok(line,".");
         ISSGTOD a;
         double vx,vy,vz,rx,ry,rz;
         if(pch){
          strptime(pch,"%Y_%j:%H:%M:%S",&tmf);
          //cout <<" pch "<<pch<<endl;
          time_t tf=mktime(&tmf)+tzd;
          pch=strtok(NULL,",");
          char tm1[80];
          sprintf(tm1,".%s",pch);
          double tc=tf+atof(tm1);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0]) && pch[0]!='-' && pch[0]!='.')continue;
          a.r=atof(pch)*100;
          //cout <<" alpha "<<a.x<<" "<<tf<<" "<<tc<<endl;
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.theta=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.phi=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.v=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.vtheta=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          a.vphi=atof(pch);
/*
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          rx=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          ry=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          rz=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          vx=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          vy=atof(pch);
          pch=strtok(NULL,",");
          if(!pch)continue;
          if(!isdigit(pch[0])&& pch[0]!='-'&& pch[0]!='.')continue;
          vz=atof(pch);
          double v=sqrt(vx*vx+vy*vy+vz*vz);
          double r=sqrt(rx*rx+ry*ry+rz*rz);
          a.vtheta=asin(vz/v);
          a.vphi=atan2(vy,vx);
          a.theta=asin(rz/r);
          a.phi=atan2(ry,rx); 
*/
           fISSGTOD.insert(make_pair(tc,a));
          
          if(tc>=t1 && tc<=t2){
           if(abs(bfound)!=2){
               fISSGTOD.clear();
               fISSGTOD.insert(make_pair(tc,a));
               bfound=bfound?2:-2;
//               cout <<" line "<<line<<" "<<tc<<endl;
           }
          }
         else if(tc<t1)bfound=1;
         else if(tc>t2){
             efound=1;
             break;
         }
      }   
     }
     }
     }
     else{
       cerr<<"AMSSetupR::LoadISSGTOD-E-UnabletoOpenFile "<<fname<<endl;
     }
     dc++;
     if(dc>366){
      dc=1;
      yc++;
     }
    }


int ret;
if(bfound>0 &&efound)ret=0;
else if(!bfound && !efound )ret=2;
else ret=1;
    cout<< "AMSSetupR::LoadISSGTOD-I- "<<fISSGTOD.size()<<" Entries Loaded"<<endl;

return ret;
}








int AMSSetupR::LoadDynAlignment(unsigned int run){
#ifdef __ROOTSHAREDLIBRARY__
  return false;
}
#else
/************************************ OLD VERSION 
   static int runError=-1;
   char defaultDir[]="/afs/cern.ch/ams/local/ExtAlig/AlignmentFiles";
   char *directory=getenv("AMSDynAlignment");
   if(!directory || !strlen(directory)) directory=defaultDir;

#define USETDVFIRST
#ifdef USETDVFIRST
   bool prev=DynAlManager::useTDV;
   TString prevString=DynAlManager::defaultDir;
   DynAlManager::useTDV=true;
   DynAlManager::defaultDir="";
   if(!DynAlManager::UpdateParameters(run,run+30)){
     if(runError!=run){
       cout<<"AMSSetupR::LoadDynAlignment-W-Failed to find TDV external alignment in "<<directory<<" for run "<<run<<endl;
       runError=run;
     }
#else
   bool prev=DynAlManager::useTDV;
   TString prevString=DynAlManager::defaultDir;
   DynAlManager::useTDV=false;
   DynAlManager::defaultDir="";
   if(!DynAlManager::UpdateParameters(run,0,directory)){
     if(runError!=run){
       cout<<"AMSSetupR::LoadDynAlignment-W-Failed to find external alignment in "<<directory<<" for run "<<run<<endl;
       runError=run;
     }
#endif


     DynAlManager::useTDV=prev;
     DynAlManager::defaultDir=prevString;

     return false;
   }

   // Copy the objects into the map
   fDynAlignment[1]=DynAlManager::dynAlFitContainers[1];
   fDynAlignment[9]=DynAlManager::dynAlFitContainers[9];
   DynAlManager::useTDV=prev;
   DynAlManager::defaultDir=prevString;
*******************************************************/
   return true;
   }
#endif



#ifndef __ROOTSHAREDLIBRARY__
bool AMSSetupR::BuildRichConfig(uinteger run){


  const char *nve=getenv("RichCnfHkdExec");
  char ior[]="rich_cnf_hkd";
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

  if( nve &&strlen(nve) && exedir  && AMSCommonsI::getosname()){
    char t1[1024];
    strcpy(t1,exedir);
    strcat(t1,"/../prod");
    setenv("TNS_ADMIN",t1,0);
   char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
   char *localbin=0;
   if(getenv("AMSDataDir")){
    localbin=getenv("AMSDataDir");
   }
   else localbin=local;
    AString systemc=localbin;
    systemc+="/DataManagement/exe/linux/timeout --signal 9 100 ";
    systemc+=exedir;
    systemc+="/";
    systemc+=AMSCommonsI::getosname();
    systemc+="/";
    systemc+=nve;
    char u[128];
    sprintf(u," %u",fHeader.Run);
    systemc+=u;
    systemc+="  > /tmp/rich_cnf_hkd.";
    char tmp[80];
    sprintf(tmp,"%u",getpid());
    systemc+=tmp;
    cout << "AMSSetupR::BuildRichConfig-I-Execute "<<systemc<<endl;
    int i=system(systemc);
    if(i){
      cerr <<"  AMSSetupR::BuildRichConfig-E-UnableTo "<<systemc<<endl;
      systemc="rm /tmp/rich_cnf_hkd."; 
      systemc+=tmp;
      system(systemc);
      LoadRichConfig(fHeader.Run);
      return false;
    }
    else{
      systemc="rm /tmp/rich_cnf_hkd."; 
      systemc+=tmp;
      system(systemc);
      LoadRichConfig(fHeader.Run);
      return true;
    }
  }
  else{
    cerr<<" AMSSetupR::BuildRichConfig-E-UnableToGetRichCnfHkdBecauseSomeVarAreNull"<<endl;
    return false;
  }
}
#else
bool AMSSetupR::BuildRichConfig(uinteger run){return true;}
#endif



int AMSSetupR::LoadRichConfig(unsigned int run){
#ifdef __ROOTSHAREDLIBRARY__
  return false;
}
#else
   static int runError=-1;
   char defaultDir[1024];
#ifndef _PGTRACK_
   sprintf(defaultDir,"%s/v4.00/RichDefaultPMTCalib",getenv("AMSDataDir"));
#else
   sprintf(defaultDir,"%s/v5.00/RichDefaultPMTCalib",getenv("AMSDataDir"));
#endif
   char *directory=getenv("AMSRichConfig");
   if(!directory || !strlen(directory)) directory=defaultDir;

   bool prev=RichConfigManager::useExternalFiles;
   TString prevString=RichConfigManager::defaultDir;
   RichConfigManager::useExternalFiles=true;
   RichConfigManager::defaultDir="";
   if(!RichConfigManager::UpdateParameters(run,0,directory)){
     if(runError!=run){
       cout<<"AMSSetupR::LoadRichConfig-W-Failed to find files in "<<directory<<" for run "<<run<<endl;
       runError=run;
     }
     RichConfigManager::useExternalFiles=prev;
     RichConfigManager::defaultDir=prevString;
     return false;
   }

   // Copy the objects into the map
   fRichConfig.clear(); fRichConfig.push_back(RichConfigManager::richConfigContainer);
   RichConfigManager::useExternalFiles=prev;
   RichConfigManager::defaultDir=prevString;

   return true;
}
#endif


//-------------------------DSP Errors-------------------------------------

int AMSSetupR::LoadDSPErrors(unsigned int t1, unsigned int t2){
  
  const char DSPPathlocal[]="/afs/cern.ch/ams/Offline/AMSDataDir/";
  
  char DSPPath[256];
  char amsdsp_add[256];
  
  char* AMSDataDir=getenv("AMSDataDir");//if you defined a "custom" AMSDataDir
  if (!AMSDataDir || !strlen(AMSDataDir)) strcpy(DSPPath, DSPPathlocal);
  else strcpy(DSPPath, AMSDataDir);
  sprintf(amsdsp_add, "%s/altec/DSP/", DSPPath);
  strcpy(DSPPath, amsdsp_add);
  //  printf("%s\n", DSPPath);//only for debug
  
  char* AMSDSPEnv=getenv("AMSDSP");//if you defined a "custom" dir just for DSP stuff
  if (!(!AMSDSPEnv || !strlen(AMSDSPEnv))) strcpy(DSPPath, AMSDSPEnv);
  
  // printf("%s\n", DSPPath);//only for debug
  // sleep(10);//only for debug

  if (t2<(unsigned int)(1305500000)){
    cerr<< "AMSSetupR::LoadDSPErrors-S-TimeEndLessThanLaunch "<<t1<<" "<<t2<<" "<<endl;
    return -2;
  }
  
  if (t1>=t2){
    cerr<< "AMSSetupR::LoadDSPErrors-S-TimeIntervalNotValid "<<t1<<" "<<t2<<" "<<endl;
    return -2;
  }

  int oldsize=fDSPError.size();

  //when a new file will be produced we will have to implement the loop over files...
  //  const char fpath[256]="DSPPERIODS_1305911355_1339661827.csv";
  //better to "merge" the new files and use a 'forever valid' "alias" (a symbolic link) 
  const char fpath[256]="DSPPERIODS.csv";
  
  string fname=DSPPath;
  //  cout<<fname<<endl;//only for debug
  fname+=fpath;
  //  cout<<fname<<endl;//only for debug
  ifstream fbin;
  fbin.close();    
  fbin.clear();
  fbin.open(fname.c_str());
  
  if(fbin){
    //    printf("Found %s\n", fname.c_str());
    int number_of_lines=0;
    while(fbin.good() && !fbin.eof()){
      char line[31];
      fbin.getline(line, 30);
      number_of_lines++;
      //      printf("%d\n", number_of_lines);//only for debug
      if (number_of_lines>=1) {
	//	printf("%s is good -> %d\n", fname.c_str(), number_of_lines);//only for debug
	//	printf("%s\n", line);//only for debug
	if(isdigit(line[0])){
	  char *pch;
	  pch=strtok(line,",");
	  if (!pch) continue;
	  //	  printf("%s\n", pch);//only for debug
	  unsigned int TimeStart=atoi(pch);
	  //	  printf("%d\n", TimeStart);//only for debug
	  pch=strtok(NULL,",");
	  if(!pch) continue;
	  //	  printf("%s\n", pch);//only for debug
	  unsigned int TimeEnd=atoi(pch);
	  TimeEnd++;//since the information is given in second the end will be put and the end of this second (aka the start of following second)
	  //	  printf("%d\n", TimeEnd);//only for debug
	  pch=strtok(NULL,",");
	  if(!pch) continue;
	  //	  printf("%s\n", pch);//only for debug
	  unsigned int NA=atoi(pch);
	  sscanf(pch, "%03X\n", &NA);
	  if (TimeStart<t1 && TimeEnd<t1) continue;//they are outside t1-t2 interval
	  if (TimeStart>t2 && TimeEnd>t2) continue;//they are outside t1-t2 interval
	  DSPError a;
	  a.TimeStart=TimeStart; 
	  a.TimeEnd=TimeEnd;
	  if (!a.SetNA(NA)) {
	    continue;
	  }
	  //	  printf("%d %d %03X\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	  static unsigned int maxdiff=0;
	  if ((TimeEnd-TimeStart)>maxdiff) maxdiff=(TimeEnd-TimeStart);
	  //	  printf("MaxDiff = %d\n", maxdiff);//only for debug
	  if (fDSPError.size()) {//the map is not empty
	    DSPError tmp;
	    DSPError_i it;
	    unsigned int START=TimeStart;
	    //	    printf("Searching for %d...\n", TimeStart);//only for debug
	    it=fDSPError.lower_bound(TimeStart);//if not found any item 'after' it gives back fDSPError.end(
	    //	    if (it == fDSPError.begin()) printf("begin()\n");//only for debug
	    //	    if (it == fDSPError.end()) printf("end()\n");//only for debug
	    for (; it != fDSPError.begin(); it--) {
	      DSPError_i it2=it;
	      it2--;//last item in map before TimeStart and otherwise begin() is never watched...
	      tmp=it2->second;
	      //	      printf("Found on map: %d %d\n", tmp.TimeStart, tmp.TimeEnd);
	      if (tmp.TimeEnd>=TimeStart) {
		START=tmp.TimeStart;
		//		printf("New start: %d (%d)\n", START, TimeStart);
	      }
	      else {
		// break;// if cannot break 'simply' since, even if they are ordered so would be stupid going even back, there could be, back, another item, with the start before this (so ordered before this) but with the end after TimeStart
		if ((TimeStart-tmp.TimeStart)>maxdiff) {
		  //		  printf("Breaking since %d-%d=%d>%d\n", TimeStart, tmp.TimeStart, TimeStart-tmp.TimeStart, maxdiff);//only for debug
		  break;//in the map there's no entry with TimeEnd>TimeStart+maxdiff, so I can break safely. This is true even if I call LoadDSPErrors several times since maxdiff is static and so remembering its value up to the end of program
		}
	      }
	    }
	    //	    printf("Start: %d (%d)\n", START, TimeStart);
	    DSPError_i it_start=fDSPError.lower_bound(START);
	    if (it_start != fDSPError.end()) {
	      DSPError_i it_stop=fDSPError.lower_bound(TimeEnd);
	      if (it_stop != fDSPError.end()) it_stop++;//otherwise last will be never processed. if after ++ end() is reached is OK however
	      //	      if (it_stop == fDSPError.begin()) printf("begin()\n");//only for debug
	      //	      if (it_stop == fDSPError.end()) printf("end()\n");//only for debug
	      vector<DSPError> vec;
	      vector<DSPError>::iterator it_vec;
	      vector<DSPError_i> vec_of_it;
	      vector<DSPError_i>::iterator it_vec_of_it;
	      vec.clear();
	      vec_of_it.clear();
	      vec.push_back(a);
	      for (it=it_start; it != it_stop; it++) {
		tmp=it->second;
		//		printf("Found on map: %d %d %03X\n", tmp.TimeStart, tmp.TimeEnd, tmp.GetFirstNA());//only for debug
		//I remove it from the map (on a separated loop otherwise iterators during this loop will be screwed up)
		//and I add to the temporary vec.
		//After RearrangeManyDSPErrors it will be inserted again to map (divided into pieces if needed)
		vec.push_back(tmp);
		vec_of_it.push_back(it);
	      }
	      for (it_vec_of_it=vec_of_it.begin(); it_vec_of_it<vec_of_it.end(); it_vec_of_it++) {
		fDSPError.erase(*it_vec_of_it);
	      }
	      //	      printf("%d\n", (int)vec.size());//only for debug
	      RearrangeManyDSPErrors(vec);
	      for (it_vec=vec.begin(); it_vec<vec.end(); it_vec++) {
		DSPError fromvec=*it_vec;
		//		printf("Inserting: %d %d %03X\n", fromvec.TimeStart, fromvec.TimeEnd, fromvec.GetFirstNA());//only for debug
		if (fDSPError.find(fromvec.TimeStart) != fDSPError.end())
		  cerr<<"AMSSetupR::LoadDSPErrors-W-KeyAlreadyPresent "<<fromvec.TimeStart<<endl;
		fDSPError.insert(make_pair(fromvec.TimeStart,fromvec));
	      }
	    }
	    else {
	      if (fDSPError.find(TimeStart) != fDSPError.end())
		cerr<<"AMSSetupR::LoadDSPErrors-W-KeyAlreadyPresent "<<TimeStart<<endl;
	      fDSPError.insert(make_pair(TimeStart,a));//there's no item already filled from START to end(). I can insert without problems
	    }
	  }
	  else {
	    if (fDSPError.find(TimeStart) != fDSPError.end())
	      cerr<<"AMSSetupR::LoadDSPErrors-W-KeyAlreadyPresent "<<TimeStart<<endl;
	    fDSPError.insert(make_pair(TimeStart,a));//this is the first inception in the map...
	  }
	}
	//	printf("Map size=%d\n", (int)fDSPError.size());//only for debug
      }
    }
    fbin.close();    
    fbin.clear();
  }
  else{
    cerr<<"AMSSetupR::LoadDSPErrors-E-UnabletoOpenFile "<<fname<<endl;
    return -1;
  }
  
  int newsize=fDSPError.size();

  int ret=(newsize-oldsize);
  cout<< "AMSSetupR::LoadDSPErrors-I-LoadedEntries "<<ret<<endl;
  cout<< "AMSSetupR::LoadDSPErrors-I-TotalEntries "<<newsize<<endl;
  // //  only for debug
  // for (DSPError_i it=fDSPError.begin(); it!=fDSPError.end(); it++) {
  //   DSPError tmp=it->second;
  //   printf("Found on map: %d %d %03X\n", tmp.TimeStart, tmp.TimeEnd, tmp.GetFirstNA());
  // }
  
  return ret;
}

void AMSSetupR::RearrangeManyDSPErrors(vector<DSPError>& vec){
  vector<DSPError> vec2;
  vec2.clear();

  vector<DSPError>::iterator it_vec;
  vector<DSPError>::iterator it_vec2;

  int index_vec2=-1;

  //  printf("really before) vec.size=%d\n", (int)vec.size());//only for debug
  //  printf("really before) vec2.size=%d\n", (int)vec2.size());//only for debug

  while (1) {
    vec2.push_back(*vec.begin());//first element of vec moved to vec2
    vec.erase(vec.begin());//first element of vec erased from vec
    //    printf("before) vec.size=%d\n", (int)vec.size());//only for debug
    //    printf("before) vec2.size=%d\n", (int)vec2.size());//only for debug
    index_vec2++;
    //    printf("index_vec2=%d\n", index_vec2);//only for debug
    int start_vec_size=vec.size();
    for (int ii=0; ii<start_vec_size; ii++) {
      //      printf("ii=%d of %d\n", ii, start_vec_size);//only for debug
      RearrangeTwoDSPErrors(*(vec.begin()), vec2, index_vec2);//I process the first element in vec with 'index_vec2'th element if vec2
      vec.erase(vec.begin());//I remove processed element in vec
      //      printf("after_rearrange) vec.size=%d\n", (int)vec.size());//only for debug
      //      printf("after_rearrange) vec2.size=%d\n", (int)vec2.size());//only for debug
      while (vec2.size()>(index_vec2+1)) {//I move all elements after 'index_vec2'th in vec2 to vec
	vec.push_back(*(vec2.rbegin()));
	vec2.pop_back();
	//	printf("second_while) vec.size=%d\n", (int)vec.size());//only for debug
	//	printf("second_while) vec2.size=%d\n", (int)vec2.size());//only for debug
      }
    }
    if (!vec.size()) break;
  }

  vec=vec2;

  return;
}

void AMSSetupR::RearrangeTwoDSPErrors(DSPError dsperr, vector<DSPError>& vec, int in){

  //  printf("Comparing (%d,%d,%03X) with (%d,%d,%03X) \n", dsperr.TimeStart, dsperr.TimeEnd, dsperr.GetFirstNA(), vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug

  if ( (dsperr.TimeEnd<=vec[in].TimeStart) || //dsperr completely (disjoint) before vec[index]
       (dsperr.TimeStart>=vec[in].TimeEnd)    ) {//dsperr completely (disjoint) after vec[index]
    //    printf("rearrange) vec2.size %d -> ", (int)vec.size());//only for debug
    vec.push_back(dsperr);
    //    printf("%d\n", (int)vec.size());//only for debug
  }
  else if ( (dsperr.TimeStart==vec[in].TimeStart) && (dsperr.TimeEnd==vec[in].TimeEnd) ) {//very same interval
    //    printf("%d %d ->", dsperr.GetFirstNA(), vec[in].GetFirstNA());//only for debug
    dsperr.AddNA(vec[in]);
    //    printf("%d\n", dsperr.GetFirstNA());//only for debug
    //    printf("rearrange) vec2.size %d -> ", (int)vec.size());//only for debug
    vec.erase(vec.begin()+in);//I remove the one already in vec
    vec.push_back(dsperr);//and I add the combined one
    //    printf("%d\n", (int)vec.size());//only for debug
  }
  else {
    int vec_size_prev=(int)vec.size();
    if (dsperr.TimeStart<vec[in].TimeStart) {//d starts before v start
      //      printf("dsperr starts before vec[%d] start\n", in);//only for debug
      if (dsperr.TimeEnd==vec[in].TimeEnd) {//d and v end togheter --> they overlap between vec[in].TimeStart and vec[in].TimeEnd, 2 pieces
	DSPError a=dsperr; a.TimeEnd=vec[in].TimeStart;
	DSPError b=vec[in]; b.AddNA(dsperr);
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b);//and I add the resulting ones
      }
      else if (dsperr.TimeEnd<vec[in].TimeEnd) {//d ends before v end --> they overlap between vec[in].TimeStart and dsperr.TimeEnd, 3 pieces
	DSPError a=dsperr; a.TimeEnd=vec[in].TimeStart;
	DSPError b=dsperr; b.TimeStart=vec[in].TimeStart; b.AddNA(vec[in]);
	DSPError c=vec[in]; c.TimeStart=dsperr.TimeEnd;
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", c.TimeStart, c.TimeEnd, c.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b); vec.push_back(c);//and I add the resulting ones
      }
      else if (dsperr.TimeEnd>vec[in].TimeEnd) {//d ends after v end --> they overlap between vec[in].TimeStart and vec[in].TimeEnd, 3 pieces
	DSPError a=dsperr; a.TimeEnd=vec[in].TimeStart;
	DSPError b=vec[in]; b.AddNA(dsperr);
	DSPError c=dsperr; c.TimeStart=vec[in].TimeEnd;
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", c.TimeStart, c.TimeEnd, c.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b); vec.push_back(c);//and I add the resulting ones
      }	
    }
    else if (dsperr.TimeStart>vec[in].TimeStart) {//d starts after v start
      //      printf("dsperr starts after vec[%d] start\n", in);//only for debug
      if (vec[in].TimeEnd==dsperr.TimeEnd) {//v and d end togheter --> they overlap between dsperr.TimeStart and dsperr.TimeEnd, 2 pieces
	DSPError a=vec[in]; a.TimeEnd=dsperr.TimeStart;
	DSPError b=dsperr; b.AddNA(vec[in]);
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b);//and I add the resulting ones
      }
      else if (vec[in].TimeEnd<dsperr.TimeEnd) {//v ends before d end --> they overlap between dsperr.TimeStart and vec[in].TimeEnd, 3 pieces
	DSPError a=vec[in]; a.TimeEnd=dsperr.TimeStart;
	DSPError b=vec[in]; b.TimeStart=dsperr.TimeStart; b.AddNA(dsperr);
	DSPError c=dsperr; c.TimeStart=vec[in].TimeEnd;
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", c.TimeStart, c.TimeEnd, c.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b); vec.push_back(c);//and I add the resulting ones
      }
      else if (vec[in].TimeEnd>dsperr.TimeEnd) {//v ends after d end --> they overlap between dsperr.TimeStart and dsperr.TimeEnd, 3 pieces
	DSPError a=vec[in]; a.TimeEnd=dsperr.TimeStart;
	DSPError b=dsperr; b.AddNA(vec[in]);
	DSPError c=vec[in]; c.TimeStart=dsperr.TimeEnd;
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", c.TimeStart, c.TimeEnd, c.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b); vec.push_back(c);//and I add the resulting ones
      }	
    }
    else { // they start togheter
      //      printf("dsperr and vec[%d] start togheter\n", in);//only for debug
      if (dsperr.TimeEnd==vec[in].TimeEnd) {//d ends before v end --> they overlap completely, 1 pieces SHOULD NOT HAPPEN: ALREADY DONE!!!!
	cerr<<"AMSSetupR::LoadDSPErrors-W-SameIntervalShouldNotHappen "<<endl;
	dsperr.AddNA(vec[in]);
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", dsperr.TimeStart, dsperr.TimeEnd, dsperr.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(dsperr);//and I add the combined one
      }
      if (dsperr.TimeEnd<vec[in].TimeEnd) {//d ends before v end --> they overlap between dsperr.TimeStart and dsperr.TimeEnd, 2 pieces
	DSPError a=dsperr; a.AddNA(vec[in]);
	DSPError b=vec[in]; b.TimeStart=dsperr.TimeEnd; 
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b);//and I add the resulting ones
      }
      else {//d ends after v end --> they overlap between dsperr.TimeStart and vec[in].TimeEnd, 2 pieces
	DSPError a=vec[in]; a.AddNA(dsperr);
	DSPError b=dsperr; b.TimeStart=vec[in].TimeEnd;
	// printf("Removing (%d,%d,%03X)\n", vec[in].TimeStart, vec[in].TimeEnd, vec[in].GetFirstNA());//only for debug
	// printf("Removing (%d,%d,%03X)\n", (*(vec.begin()+in)).TimeStart, (*(vec.begin()+in)).TimeEnd, (*(vec.begin()+in)).GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", a.TimeStart, a.TimeEnd, a.GetFirstNA());//only for debug
	// printf("Pushing (%d,%d,%03X)\n", b.TimeStart, b.TimeEnd, b.GetFirstNA());//only for debug
	vec.erase(vec.begin()+in);//I remove the one already in vec
	vec.push_back(a); vec.push_back(b);//and I add the resulting ones
      }
    }
    //    printf("rearrange) vec2.size %d -> %d\n", vec_size_prev, (int)vec.size());//only for debug
  }
  
  return;
}

int AMSSetupR::getDSPError(DSPError& dsperror, unsigned int time){
  
  static unsigned int tstartloaded=0;
  static unsigned int tendloaded=0;

  if(
     fDSPError.size()==0 ||
     (time<tstartloaded || time>tendloaded)
     ){
#ifdef __ROOTSHAREDLIBRARY__
    static unsigned int stime=0;
#pragma omp threadprivate (stime)
    if(stime!=floor(time)){
      stime=time;
     const int dt=120;
      if(fHeader.FEventTime==0 && fHeader.Run==0 && fHeader.LEventTime==0){//"dummy" AMSSetupR
	tstartloaded=min(tstartloaded, time-dt);
	tendloaded=max(tendloaded, time+dt);
	LoadDSPErrors(tstartloaded, tendloaded);
      }
      else if(fHeader.FEventTime-dt<fHeader.Run && fHeader.LEventTime+dt>fHeader.Run){
	tstartloaded=min(tstartloaded, fHeader.FEventTime-dt);
	tendloaded=max(tendloaded, fHeader.LEventTime+dt);
	LoadDSPErrors(tstartloaded, tendloaded);
      }
      else {
	tstartloaded=min(tstartloaded, fHeader.Run-dt);
	tendloaded=max(tendloaded, fHeader.Run+3600);
	LoadDSPErrors(tstartloaded, tendloaded);
      }
    }
#endif
    if(fDSPError.size()==0)
      return 0;
  }

  AMSSetupR::DSPError_i k=fDSPError.upper_bound(time);//it gives the first found element after (strictly!!!) time
  k--;//previous item
  AMSSetupR::DSPError _dsperr=k->second;
  if (time>=_dsperr.TimeStart && time<_dsperr.TimeEnd) {
    dsperror=_dsperr;
    return 1;
  }

  return 0;
}

bool AMSSetupR::DSPError::SetNA(unsigned int NA){

  // //only used for debug
  // printf("Before SetNA\n");
  // printf("%08X\n", Nodes_000_01F);
  // printf("%08X\n", Nodes_020_03F);
  // printf("%08X\n", Nodes_040_05F);
  // printf("%08X\n", Nodes_060_07F);
  // printf("%08X\n", Nodes_080_09F);
  // printf("%08X\n", Nodes_0A0_0BF);
  // printf("%08X\n", Nodes_0C0_0DF);
  // printf("%08X\n", Nodes_0E0_0FF);
  // printf("%08X\n", Nodes_100_11F);
  // printf("%08X\n", Nodes_120_13F);
  // printf("%08X\n", Nodes_140_15F);
  // printf("%08X\n", Nodes_160_17F);
  // printf("%08X\n", Nodes_180_19F);
  // printf("%08X\n", Nodes_1A0_1BF);
  // printf("%08X\n", Nodes_1C0_1DF);
  // printf("%08X\n", Nodes_1E0_1FF);

  if (NA<=0x01F)      Nodes_000_01F=(1<< NA      );
  else if (NA<=0x03F) Nodes_020_03F=(1<<(NA-   32));
  else if (NA<=0x05F) Nodes_040_05F=(1<<(NA- 2*32));
  else if (NA<=0x07F) Nodes_060_07F=(1<<(NA- 3*32));
  else if (NA<=0x09F) Nodes_080_09F=(1<<(NA- 4*32));
  else if (NA<=0x0BF) Nodes_0A0_0BF=(1<<(NA- 5*32));
  else if (NA<=0x0DF) Nodes_0C0_0DF=(1<<(NA- 6*32));
  else if (NA<=0x0FF) Nodes_0E0_0FF=(1<<(NA- 7*32));
  else if (NA<=0x11F) Nodes_100_11F=(1<<(NA- 8*32));
  else if (NA<=0x13F) Nodes_120_13F=(1<<(NA- 9*32));
  else if (NA<=0x15F) Nodes_140_15F=(1<<(NA-10*32));
  else if (NA<=0x17F) Nodes_160_17F=(1<<(NA-11*32));
  else if (NA<=0x19F) Nodes_180_19F=(1<<(NA-12*32));
  else if (NA<=0x1BF) Nodes_1A0_1BF=(1<<(NA-13*32));
  else if (NA<=0x1DF) Nodes_1C0_1DF=(1<<(NA-14*32));
  else if (NA<=0x1FF) Nodes_1E0_1FF=(1<<(NA-15*32));
  else {
    cerr<<"DSPError::SetNA-W-NodeAddressNotPossible "<<NA<<endl;
    return false;
  }

  // //only used for debug
  // printf("After SetNA\n");
  // printf("%08X\n", Nodes_000_01F);
  // printf("%08X\n", Nodes_020_03F);
  // printf("%08X\n", Nodes_040_05F);
  // printf("%08X\n", Nodes_060_07F);
  // printf("%08X\n", Nodes_080_09F);
  // printf("%08X\n", Nodes_0A0_0BF);
  // printf("%08X\n", Nodes_0C0_0DF);
  // printf("%08X\n", Nodes_0E0_0FF);
  // printf("%08X\n", Nodes_100_11F);
  // printf("%08X\n", Nodes_120_13F);
  // printf("%08X\n", Nodes_140_15F);
  // printf("%08X\n", Nodes_160_17F);
  // printf("%08X\n", Nodes_180_19F);
  // printf("%08X\n", Nodes_1A0_1BF);
  // printf("%08X\n", Nodes_1C0_1DF);
  // printf("%08X\n", Nodes_1E0_1FF);
  
  return true;
}
bool AMSSetupR::DSPError::AddNA(unsigned int NA) {

  // //only used for debug (see below)
  // unsigned int _Nodes_000_01F=Nodes_000_01F;
  // unsigned int _Nodes_020_03F=Nodes_020_03F;
  // unsigned int _Nodes_040_05F=Nodes_040_05F;
  // unsigned int _Nodes_060_07F=Nodes_060_07F;
  // unsigned int _Nodes_080_09F=Nodes_080_09F;
  // unsigned int _Nodes_0A0_0BF=Nodes_0A0_0BF;
  // unsigned int _Nodes_0C0_0DF=Nodes_0C0_0DF;
  // unsigned int _Nodes_0E0_0FF=Nodes_0E0_0FF;
  // unsigned int _Nodes_100_11F=Nodes_100_11F;
  // unsigned int _Nodes_120_13F=Nodes_120_13F;
  // unsigned int _Nodes_140_15F=Nodes_140_15F;
  // unsigned int _Nodes_160_17F=Nodes_160_17F;
  // unsigned int _Nodes_180_19F=Nodes_180_19F;
  // unsigned int _Nodes_1A0_1BF=Nodes_1A0_1BF;
  // unsigned int _Nodes_1C0_1DF=Nodes_1C0_1DF;
  // unsigned int _Nodes_1E0_1FF=Nodes_1E0_1FF;

  if (NA<=0x01F)      Nodes_000_01F|=(1<< NA      );
  else if (NA<=0x03F) Nodes_020_03F|=(1<<(NA-   32));
  else if (NA<=0x05F) Nodes_040_05F|=(1<<(NA- 2*32));
  else if (NA<=0x07F) Nodes_060_07F|=(1<<(NA- 3*32));
  else if (NA<=0x09F) Nodes_080_09F|=(1<<(NA- 4*32));
  else if (NA<=0x0BF) Nodes_0A0_0BF|=(1<<(NA- 5*32));
  else if (NA<=0x0DF) Nodes_0C0_0DF|=(1<<(NA- 6*32));
  else if (NA<=0x0FF) Nodes_0E0_0FF|=(1<<(NA- 7*32));
  else if (NA<=0x11F) Nodes_100_11F|=(1<<(NA- 8*32));
  else if (NA<=0x13F) Nodes_120_13F|=(1<<(NA- 9*32));
  else if (NA<=0x15F) Nodes_140_15F|=(1<<(NA-10*32));
  else if (NA<=0x17F) Nodes_160_17F|=(1<<(NA-11*32));
  else if (NA<=0x19F) Nodes_180_19F|=(1<<(NA-12*32));
  else if (NA<=0x1BF) Nodes_1A0_1BF|=(1<<(NA-13*32));
  else if (NA<=0x1DF) Nodes_1C0_1DF|=(1<<(NA-14*32));
  else if (NA<=0x1FF) Nodes_1E0_1FF|=(1<<(NA-15*32));
  else {
    cerr<<"DSPError::AddNA-W-NodeAddressNotPossible "<<NA<<endl;
    return false;
  }
  
  // //only used for debug
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_000_01F, NA, Nodes_000_01F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_020_03F, NA, Nodes_020_03F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_040_05F, NA, Nodes_040_05F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_060_07F, NA, Nodes_060_07F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_080_09F, NA, Nodes_080_09F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_0A0_0BF, NA, Nodes_0A0_0BF);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_0C0_0DF, NA, Nodes_0C0_0DF);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_0E0_0FF, NA, Nodes_0E0_0FF);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_100_11F, NA, Nodes_100_11F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_120_13F, NA, Nodes_120_13F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_140_15F, NA, Nodes_140_15F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_160_17F, NA, Nodes_160_17F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_180_19F, NA, Nodes_180_19F);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_1A0_1BF, NA, Nodes_1A0_1BF);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_1C0_1DF, NA, Nodes_1C0_1DF);
  // printf("%08X | NA=%03X -> %08X\n", _Nodes_1E0_1FF, NA, Nodes_1E0_1FF);

  return true;
}

bool AMSSetupR::DSPError::AddNA(DSPError dsperr) {

  //only used for debug
  // printf("%08X | %08X -> %08X\n", Nodes_000_01F, dsperr.Nodes_000_01F, Nodes_000_01F|=dsperr.Nodes_000_01F);
  // printf("%08X | %08X -> %08X\n", Nodes_020_03F, dsperr.Nodes_020_03F, Nodes_020_03F|=dsperr.Nodes_020_03F);
  // printf("%08X | %08X -> %08X\n", Nodes_040_05F, dsperr.Nodes_040_05F, Nodes_040_05F|=dsperr.Nodes_040_05F);
  // printf("%08X | %08X -> %08X\n", Nodes_060_07F, dsperr.Nodes_060_07F, Nodes_060_07F|=dsperr.Nodes_060_07F);
  // printf("%08X | %08X -> %08X\n", Nodes_080_09F, dsperr.Nodes_080_09F, Nodes_080_09F|=dsperr.Nodes_080_09F);
  // printf("%08X | %08X -> %08X\n", Nodes_0A0_0BF, dsperr.Nodes_0A0_0BF, Nodes_0A0_0BF|=dsperr.Nodes_0A0_0BF);
  // printf("%08X | %08X -> %08X\n", Nodes_0C0_0DF, dsperr.Nodes_0C0_0DF, Nodes_0C0_0DF|=dsperr.Nodes_0C0_0DF);
  // printf("%08X | %08X -> %08X\n", Nodes_0E0_0FF, dsperr.Nodes_0E0_0FF, Nodes_0E0_0FF|=dsperr.Nodes_0E0_0FF);
  // printf("%08X | %08X -> %08X\n", Nodes_100_11F, dsperr.Nodes_100_11F, Nodes_100_11F|=dsperr.Nodes_100_11F);
  // printf("%08X | %08X -> %08X\n", Nodes_120_13F, dsperr.Nodes_120_13F, Nodes_120_13F|=dsperr.Nodes_120_13F);
  // printf("%08X | %08X -> %08X\n", Nodes_140_15F, dsperr.Nodes_140_15F, Nodes_140_15F|=dsperr.Nodes_140_15F);
  // printf("%08X | %08X -> %08X\n", Nodes_160_17F, dsperr.Nodes_160_17F, Nodes_160_17F|=dsperr.Nodes_160_17F);
  // printf("%08X | %08X -> %08X\n", Nodes_180_19F, dsperr.Nodes_180_19F, Nodes_180_19F|=dsperr.Nodes_180_19F);
  // printf("%08X | %08X -> %08X\n", Nodes_1A0_1BF, dsperr.Nodes_1A0_1BF, Nodes_1A0_1BF|=dsperr.Nodes_1A0_1BF);
  // printf("%08X | %08X -> %08X\n", Nodes_1C0_1DF, dsperr.Nodes_1C0_1DF, Nodes_1C0_1DF|=dsperr.Nodes_1C0_1DF);
  // printf("%08X | %08X -> %08X\n", Nodes_1E0_1FF, dsperr.Nodes_1E0_1FF, Nodes_1E0_1FF|=dsperr.Nodes_1E0_1FF);

  Nodes_000_01F|=dsperr.Nodes_000_01F;
  Nodes_020_03F|=dsperr.Nodes_020_03F;
  Nodes_040_05F|=dsperr.Nodes_040_05F;
  Nodes_060_07F|=dsperr.Nodes_060_07F;
  Nodes_080_09F|=dsperr.Nodes_080_09F;
  Nodes_0A0_0BF|=dsperr.Nodes_0A0_0BF;
  Nodes_0C0_0DF|=dsperr.Nodes_0C0_0DF;
  Nodes_0E0_0FF|=dsperr.Nodes_0E0_0FF;
  Nodes_100_11F|=dsperr.Nodes_100_11F;
  Nodes_120_13F|=dsperr.Nodes_120_13F;
  Nodes_140_15F|=dsperr.Nodes_140_15F;
  Nodes_160_17F|=dsperr.Nodes_160_17F;
  Nodes_180_19F|=dsperr.Nodes_180_19F;
  Nodes_1A0_1BF|=dsperr.Nodes_1A0_1BF;
  Nodes_1C0_1DF|=dsperr.Nodes_1C0_1DF;
  Nodes_1E0_1FF|=dsperr.Nodes_1E0_1FF;

  return true;
}

int AMSSetupR::DSPError::GetFirstNA(){

  // //only used for debug
  // printf("%08X\n", Nodes_000_01F);
  // printf("%08X\n", Nodes_020_03F);
  // printf("%08X\n", Nodes_040_05F);
  // printf("%08X\n", Nodes_060_07F);
  // printf("%08X\n", Nodes_080_09F);
  // printf("%08X\n", Nodes_0A0_0BF);
  // printf("%08X\n", Nodes_0C0_0DF);
  // printf("%08X\n", Nodes_0E0_0FF);
  // printf("%08X\n", Nodes_100_11F);
  // printf("%08X\n", Nodes_120_13F);
  // printf("%08X\n", Nodes_140_15F);
  // printf("%08X\n", Nodes_160_17F);
  // printf("%08X\n", Nodes_180_19F);
  // printf("%08X\n", Nodes_1A0_1BF);
  // printf("%08X\n", Nodes_1C0_1DF);
  // printf("%08X\n", Nodes_1E0_1FF);

  vector<unsigned int> vec_NA;
  
  for (unsigned int NA=0; NA<32; NA++) {
    if ((Nodes_000_01F)&(1<<NA)) vec_NA.push_back(NA      );
    if ((Nodes_020_03F)&(1<<NA)) vec_NA.push_back(NA+0x020);
    if ((Nodes_040_05F)&(1<<NA)) vec_NA.push_back(NA+0x040);
    if ((Nodes_060_07F)&(1<<NA)) vec_NA.push_back(NA+0x060);
    if ((Nodes_080_09F)&(1<<NA)) vec_NA.push_back(NA+0x080);
    if ((Nodes_0A0_0BF)&(1<<NA)) vec_NA.push_back(NA+0x0A0);
    if ((Nodes_0C0_0DF)&(1<<NA)) vec_NA.push_back(NA+0x0C0);
    if ((Nodes_0E0_0FF)&(1<<NA)) vec_NA.push_back(NA+0x0E0);
    if ((Nodes_100_11F)&(1<<NA)) vec_NA.push_back(NA+0x100);
    if ((Nodes_120_13F)&(1<<NA)) vec_NA.push_back(NA+0x120);
    if ((Nodes_140_15F)&(1<<NA)) vec_NA.push_back(NA+0x140);
    if ((Nodes_160_17F)&(1<<NA)) vec_NA.push_back(NA+0x160);
    if ((Nodes_180_19F)&(1<<NA)) vec_NA.push_back(NA+0x180);
    if ((Nodes_1A0_1BF)&(1<<NA)) vec_NA.push_back(NA+0x1A0);
    if ((Nodes_1C0_1DF)&(1<<NA)) vec_NA.push_back(NA+0x1C0);
    if ((Nodes_1E0_1FF)&(1<<NA)) vec_NA.push_back(NA+0x1E0);
  }

  sort(vec_NA.begin(), vec_NA.end()); 

  //  for (int ii=0; ii<vec_NA.size(); ii++) printf("%d\n", vec_NA[ii]);//only for debug

  if (vec_NA.size()==1) return vec_NA[0];
  else return -vec_NA[0];
}

bool AMSSetupR::DSPError::SearchNA(unsigned int NA){
  
  bool ret=false;
  
  if (NA<=0x01F)      ret=(Nodes_000_01F)&(1<< NA      );
  else if (NA<=0x03F) ret=(Nodes_020_03F)&(1<<(NA-   32));
  else if (NA<=0x05F) ret=(Nodes_040_05F)&(1<<(NA- 2*32));
  else if (NA<=0x07F) ret=(Nodes_060_07F)&(1<<(NA- 3*32));
  else if (NA<=0x09F) ret=(Nodes_080_09F)&(1<<(NA- 4*32));
  else if (NA<=0x0BF) ret=(Nodes_0A0_0BF)&(1<<(NA- 5*32));
  else if (NA<=0x0DF) ret=(Nodes_0C0_0DF)&(1<<(NA- 6*32));
  else if (NA<=0x0FF) ret=(Nodes_0E0_0FF)&(1<<(NA- 7*32));
  else if (NA<=0x11F) ret=(Nodes_100_11F)&(1<<(NA- 8*32));
  else if (NA<=0x13F) ret=(Nodes_120_13F)&(1<<(NA- 9*32));
  else if (NA<=0x15F) ret=(Nodes_140_15F)&(1<<(NA-10*32));
  else if (NA<=0x17F) ret=(Nodes_160_17F)&(1<<(NA-11*32));
  else if (NA<=0x19F) ret=(Nodes_180_19F)&(1<<(NA-12*32));
  else if (NA<=0x1BF) ret=(Nodes_1A0_1BF)&(1<<(NA-13*32));
  else if (NA<=0x1DF) ret=(Nodes_1C0_1DF)&(1<<(NA-14*32));
  else if (NA<=0x1FF) ret=(Nodes_1E0_1FF)&(1<<(NA-15*32));
  else {
    cerr<<"DSPError::AddNA-W-NodeAddressNotPossible "<<NA<<endl;
    return false;
  }
  
  return ret;
}

//-------------------------------------------------------------------------------------------------------
