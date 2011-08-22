//  $Id: root_setup.C,v 1.47 2011/08/22 21:53:40 choutko Exp $
#include "root_setup.h"
#include "root.h"
#include <fstream>
#include "SlowControlDB.h"
#include "ntuple.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "job.h"
#include "commons.h"
#include "commonsi.h"
#include "timeid.h"
        class trio{
           public: 
           unsigned int t1;
           unsigned int t2;
           unsigned int tmod;
           string filename;
        };
#include "dirent.h"
#include <sys/stat.h>
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

AMSSetupR::GPS AMSSetupR::GetGPS(unsigned int run,unsigned int event){
static AMSSetupR::GPS gps;
for( GPS_ri i=fGPS.rbegin();i!=fGPS.rend();i++){
 if(i->second.Run==run && i->second.Event<=event){
  return i->second;
 }
}
return gps;
}

void AMSSetupR::UpdateHeader(AMSEventR *head){
if(!head)return;
unsigned int nsec;
unsigned int sec;
if(!head->fHeader.GetGPSEpoche(sec,nsec)){
 GPS gps;
 gps.Run=head->Run();
 gps.Event=head->Event();
 gps.Epoche=head->fHeader.GPSTime;
 fGPS.insert(make_pair(sec,gps));
gps=GetGPS(gps.Run,gps.Event);
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
fGPS.clear();
fISSData.clear();
fScalers.clear();
fLVL1Setup.clear();
Header a;
fHeader=a;
SlowControlR b;
fSlowControl=b;
Purge();
}

AMSSetupR::AMSSetupR(){
Reset();
}
  ClassImp(AMSSetupR::ISSAtt)
  ClassImp(AMSSetupR::ISSSA)
  ClassImp(AMSSetupR::ISSCTRS)
  ClassImp(AMSSetupR::TDVR)
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

 void AMSSetupR::Add(SlowControlDB *p){
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
fSlowControl.print();
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
a.DataMC=tdv->getid();
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
   AString systemc("/afs/cern.ch/ams/local/bin/timeout --signal 9 2400 ");
  systemc+=exedir;
 systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  if(strstr(nvr,"2.6")){
   systemc+=".6";
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
   systemc="rm /tmp/getior."; 
   systemc+=tmp;
   system(systemc);
   fISSData.clear();
   fISSAtt.clear();
   LoadISS(fHeader.Run,fHeader.Run);
   LoadISSAtt(fHeader.Run-60,fHeader.Run+3600);
   LoadISSSA(fHeader.Run-60,fHeader.Run+3600);
   LoadISSCTRS(fHeader.Run-60,fHeader.Run+3600);
   
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
   fISSData.clear();
   fISSAtt.clear();
   LoadISS(fHeader.FEventTime,fHeader.LEventTime);
   LoadISSAtt(fHeader.FEventTime-60,fHeader.LEventTime+1);
   LoadISSSA(fHeader.FEventTime-60,fHeader.LEventTime+1);
   LoadISSCTRS(fHeader.FEventTime-60,fHeader.LEventTime+1);
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


void AMSSetupR::getSlowControlFilePath(string & slc){
#ifndef __ROOTSHAREDLIBRARY__
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
      dirent64 ** namelist;
    int nptr=scandir64(sdir.c_str(),&namelist,_select,NULL);
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
         
 	free(namelist);
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
#endif
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
  AString systemc("/afs/cern.ch/ams/local/bin/timeout --signal 9 2400 ");
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



bool AMSSetupR::LoadSlowcontrolDB(const char* file){
  SlowControlDB::KillPointer();
  cout <<"AMSSetupR::LoadSlowcontrolDB-I-ResetSCDBPointer "<<file<<endl;
  SlowControlDB* scdb=SlowControlDB::GetPointer();
  cout <<"AMSSetupR::LoadSlowcontrolDB-I-GetSCDBPointer "<<file<<endl;
  if(scdb && scdb->Load(file,fHeader.FEventTime,fHeader.LEventTime)){
  cout <<"AMSSetupR::LoadSlowcontrolDB-I-LoadSCDBFile "<<file<<endl;
    scdb->BuildSearchIndex(0);
  cout <<"AMSSetupR::LoadSlowcontrolDB-I-AddSearchIndex "<<file<<endl;
    Add(scdb);
  cout <<"AMSSetupR::LoadSlowcontrolDB-I-fillSlowControl "<<file<<endl;
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
void AMSSetupR::SlowControlR::printElementNames(){
  for( etable_i i=fETable.begin();i!=fETable.end();i++){
    cout <<i->first<<" "<<i->second.datatype<<" "<<i->second.subtype<<" "<<i->second.NodeName<<" "<<i->second.BranchName<<" "<<i->second.fTable.size()<<endl;
  }
}

#ifndef __ROOTSHAREDLIBRARY__
integer AMSSetupR::_select(  const dirent64 *entry){
 return strstr(entry->d_name,"SCDB.")!=NULL;
}
#else
integer AMSSetupR::_select(  const dirent64 *entry){
 return 0;
}
#endif

void AMSSetupR::LoadISS(unsigned int t1, unsigned int t2){
#ifdef __ROOTSHAREDLIBRARY__
}
#else
char amsdatadir[]="/afs/cern.ch/ams/Offline/AMSDataDir";
char *amsd=getenv("AMSDataDir");
if(!amsd || !strlen(amsd))amsd=amsdatadir;
string ifile=amsd;
ifile+="/";
ifile+=AMSCommonsI::getversion();
ifile+="/ISS_tlefile.txt";
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
                    }
                }

#endif


int AMSSetupR::getScalers(unsigned int time, unsigned int usec){
fScalersReturn.clear();

if(fScalers.size()==0)return 0;
unsigned long long t=time;
unsigned long long utm;

utm=t<<32;
utm+= usec;
Scalers_i k=fScalers.lower_bound(utm);
if(k==fScalers.begin()){
 fScalersReturn.push_back(k);
 return 1;
}
else{
fScalersReturn.push_back(--k);
if(++k!=fScalers.end())fScalersReturn.push_back(k);
return fScalersReturn.size();
}
}





int AMSSetupR::LoadISSAtt(unsigned int t1, unsigned int t2){
#ifdef __ROOTSHAREDLIBRARY__
return 0;
}
#else

 char AMSISSlocal[]="/afs/cern.ch/ams/local/altec/";
char * AMSISS=getenv("AMSISS");
if(!AMSISS || !strlen(AMSISS))AMSISS=AMSISSlocal;

if(t1>t2){
cerr<< "AMSSetupR::LoadISSAtt-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSAtt-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
   t2=t1+864000;
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
    while(yc<ye || dc<=de){
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
#endif

int AMSSetupR::getISSSA(AMSSetupR::ISSSA & a, double xtime){
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
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  a.alpha=atan2(s1,c1)*180/3.1415926;
{
  double ang1=k->second.b1a;
  double ang2=l->second.b1a;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  a.b1a=atan2(s1,c1)*180/3.1415926;
}

{
  double ang1=k->second.b3a;
  double ang2=l->second.b3a;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  a.b3a=atan2(s1,c1)*180/3.1415926;

}
{ 
 double ang1=k->second.b1b;
  double ang2=l->second.b1b;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  a.b1b=atan2(s1,c1)*180/3.1415926;
}

{
  double ang1=k->second.b3b;
  double ang2=l->second.b3b;
  s0[0]=sin(ang1);
  s0[1]=sin(ang2);
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  s0[0]=cos(ang1);
  s0[1]=cos(ang2);
  double c1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  a.b3b=atan2(s1,c1)*180/3.1415926;
}


  return 0;


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




int AMSSetupR::getISSCTRS(AMSSetupR::ISSCTRSR & a, double xtime){
if (fISSCTRS.size()==0)return 2;



AMSSetupR::ISSCTRS_i k=fISSCTRS.lower_bound(xtime);
if(k==fISSCTRS.begin()){
a=ISSCTRSR(k->second);
return 1;
}
if(k==fISSCTRS.end()){
k--;
a=ISSCTRSR(k->second);
return 1;
}
  AMSSetupR::ISSCTRS b;
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  tme[0]=k->first;
  AMSSetupR::ISSCTRS_i l=k;
  l++;
  tme[1]=l->first;
//  cout <<" alpha "<< k->second.alpha<<" "<<l->second.alpha<<endl;
//   cout << int(k->first)<<" "<<int(l->first)<<" "<<int(xtime)<<endl;
{
  double ang1=k->second.x;
  double ang2=l->second.x;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.x=s1;
}
{
  double ang1=k->second.y;
  double ang2=l->second.y;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.y=s1;
}
{
  double ang1=k->second.z;
  double ang2=l->second.z;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.z=s1;
}
{
  double ang1=k->second.vx;
  double ang2=l->second.vx;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.vx=s1;
}
{
  double ang1=k->second.vy;
  double ang2=l->second.vy;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.vy=s1;
}

{
  double ang1=k->second.vz;
  double ang2=l->second.vz;
  s0[0]=ang1;
  s0[1]=ang2;
  double s1=s0[0]+(xtime-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
  b.vz=s1;
}

a=ISSCTRSR(b);
  return 0;


}




int AMSSetupR::LoadISSSA(unsigned int t1, unsigned int t2){
#ifdef __ROOTSHAREDLIBRARY__
return 0;
}
#else

 char AMSISSlocal[]="/afs/cern.ch/ams/local/isssa/";
char * AMSISS=getenv("AMSISSSA");
if(!AMSISS || !strlen(AMSISS))AMSISS=AMSISSlocal;

if(t1>t2){
cerr<< "AMSSetupR::LoadISSSA-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSSA-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
   t2=t1+864000;
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
    while(yc<ye || dc<=de){
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
#endif



int AMSSetupR::LoadISSCTRS(unsigned int t1, unsigned int t2){
#ifdef __ROOTSHAREDLIBRARY__
return 0;
}
#else

 char AMSISSlocal[]="/afs/cern.ch/ams/local/isssa/";
char * AMSISS=getenv("AMSISSSA");
if(!AMSISS || !strlen(AMSISS))AMSISS=AMSISSlocal;

if(t1>t2){
cerr<< "AMSSetupR::LoadISSCTRS-S-BegintimeNotLessThanEndTime "<<t1<<" "<<t2<<endl;
return 2;
}
else if(t2-t1>864000){
    cerr<< "AMSSetupR::LoadISSCTRS-S-EndBeginDifferenceTooBigMax864000 "<<t2-t1<<endl;
   t2=t1+864000;
}
const char fpatb[]="ISS_CTRS_Vectors_";
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
    cout<< "AMSSetupR::LoadISSCTRS-I-TZDSeconds "<<tzd<<endl;

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
    while(yc<ye || dc<=de){
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


#endif
