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
 cout<<"AMSSetupR::TDVRC_Purge-I-Purging "<<i->first<<" "<<i->second.size()<<endl;
if(i->second.size()>1){
 i->second.erase(i->second.begin(),--(i->second.end()));
 cout<<"AMSSetupR::TDVRC_Purge-I-Purged "<<i->first<<" "<<i->second.begin()->second.Size<<" "<<i->second.begin()->second.FilePath<<" "<<endl;
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
  AString systemc(exedir);
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
int delta=86400;
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
  AString systemc(exedir);
  systemc+="/";
  systemc+=AMSCommonsI::getosname();
  systemc+="/";
  systemc+=nve;
  char u[128];
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
