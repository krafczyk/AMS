//  $Id: amschain.C,v 1.64 2013/02/27 13:21:43 choutko Exp $
#include "amschain.h"
#include "TChainElement.h"
#include "TRegexp.h"
#ifdef CASTORSTATIC 
#include "TRFIOFile.h"
#include "TXNetFile.h"
#else
#include "TSystem.h"
#endif
#ifdef _PGTRACK_
#include "TrRecon.h"
#include "TrExtAlignDB.h"
#endif
#include <dlfcn.h>
bool AMSNtupleHelper::IsGolden(AMSEventR *o){
  return true;
}

char * AMSChain::getsetup(){
    static char ams02[]="AMS02";
    static char ams02p[]="AMS02P";
    TObjString s("");
   s.Read("AMS02Geometry");
   
#ifdef _PGTRACK_
   if(
      s.String().Contains("STK9")||
      s.String().Contains("STK6")
      )return ams02p;
   else return ams02; 
#else
   if(s.String().Contains("STK9"))return ams02p;
   else return ams02; 
#endif
}

AMSNtupleHelper *AMSNtupleHelper::fgHelper=0;

ClassImp(AMSChain);


AMSChain::AMSChain(AMSEventR* event,const char* name, unsigned int thr,unsigned int size)
  :TChain(name),fThreads(thr),fSize(size),
   _ENTRY(-1),_NAME(name),_EVENT(NULL),_TREENUMBER(-1),_FILE(0)
{
  fout=0;
  amsnew=0;
  rsetup=0;
  Init(event);
}

void AMSChain::Init(AMSEventR* event){
  if (_EVENT==NULL) {
    if (event) 
      _EVENT = event;
    else
      _EVENT = new AMSEventR;
    this->SetBranchAddress("ev.",&_EVENT);
    _EVENT->Head() = _EVENT;
    _EVENT->Tree() = NULL;
#ifdef _PGTRACK_
    // 	     TrRecon::Init();
    // TrRecon::SetParFromDataCards();
    // TrRecon::TRCon= new VCon_root();
#endif
    
  }
}

Int_t AMSChain::ReadOneEvent(Int_t entry){
      if(gSystem->ProcessEvents()){
          cout <<"InteruptReceived"<<endl;
          return -1;
      }
   if(entry>=GetEntries())return -1;
  if(!GetEvent(entry)) return 0;
  // if the user selection function exists and it fails retuern NULL
  if(AMSNtupleHelper::fgHelper && 
     !AMSNtupleHelper::fgHelper->IsGolden(_EVENT))
    return 0;
  
  return 1;
}

AMSEventR* AMSChain::GetEvent(Int_t entry){
  return  _getevent(entry, false);
}

AMSEventR* AMSChain::GetEventGlobal(Int_t entry){
  return  _getevent(entry, false);
}

AMSEventR* AMSChain::GetEventLocal(Int_t entry){
  return _getevent(entry, true);
}

AMSEventR* AMSChain::_getevent(Int_t entry, Bool_t kLocal){
  Init();
  if (!kLocal) {//The old/standard way to call this method...
    if(entry>=GetEntries()) return _EVENT;
    _ENTRY = entry;
    m_tree_entry = LoadTree(_ENTRY);
  }
  else {// The "local" way to call this method.
    if(entry>=(GetTree()->GetEntries())) return _EVENT;
    _ENTRY = 0;//The "correct" setting of _EVENT is not supported in this mode...
    m_tree_entry = entry;
  }

  if (GetTreeNumber()!=_TREENUMBER) {
    _TREENUMBER = GetTreeNumber();
    _EVENT->Init(GetTree());
    _EVENT->GetBranch(_EVENT->Tree());
  }
  
  if (_EVENT->ReadHeader(m_tree_entry)==false) {
    delete _EVENT; _EVENT = NULL;
    _ENTRY = -1;
  }
#ifdef _PGTRACK_
//if(TkDBc::Head==0&& _EVENT!=0){
//  TkDBc::CreateTkDBc();
//  TkDBc::Head->init((_EVENT->Run()>=1257416200)?2:1);
//}
//   if (GetFile() && GetFile()!=_FILE){
//     _FILE=GetFile();
//     _EVENT->InitDB(_FILE);
//     if(TrCalDB::Head) delete TrCalDB::Head;
//     TrCalDB::Head = (TrCalDB*)_FILE->Get("TrCalDB");
//     if(!TkDBc::Head){
//       if (!TkDBc::Load(_FILE)) { // by default get TkDBc from _FILE
// 	TkDBc::CreateTkDBc();    // Init nominal TkDBc if not found in _FILE
// 	TkDBc::Head->init((_EVENT->Run()>=1257416200)?2:1);
//       }
//     }
    
//     if(TrParDB::Head) delete TrParDB::Head;
//     TrParDB::Head = (TrParDB*) _FILE->Get("TrParDB");
//     if (!TrParDB::Head) {
//       TrParDB* cc = new TrParDB();
//       cc->init();
//     }
//     TrClusterR::UsingTrParDB(TrParDB::Head);
    
//     TrClusterR::UsingTrCalDB(TrCalDB::Head);
//     TrRawClusterR::UsingTrCalDB(TrCalDB::Head);
//     TrRecon::UsingTrCalDB(TrCalDB::Head);
//  }
#endif
  
  return _EVENT;
};



AMSEventR* AMSChain::GetEvent(){ 
  GetEvent(_ENTRY+1);
  return _EVENT;
};

AMSEventR* AMSChain::GetEvent(UInt_t run, Int_t ev, Bool_t kDontRewind){
  if (!kDontRewind) Rewind();//Go to start of chain
  // Get events in turn
  while  (GetEvent() &&
	  !(_EVENT->Run()==run && _EVENT->Event()==ev) ){
    //    printf("%u) Looking for run=%u, event=%d into run=%u, event=%d...\n", _ENTRY, run, ev, _EVENT->Run(), _EVENT->Event());//only for debug
    if (_ENTRY==(GetEntries()-1)) {
      static bool kRewindAlreadyDid=false;
      if (!kRewindAlreadyDid) {
	printf("AMSChain::GetEvent(%u, %d, %s): End of chain reached! Rewinding...\n", run, ev, (kDontRewind)?"true":"false");
	Rewind();
	kRewindAlreadyDid=true;
      }
      else {
	printf("AMSChain::GetEvent(%u, %d, %s): Event not found, returning 0...\n", run, ev, (kDontRewind)?"true":"false");
	kRewindAlreadyDid=false;
	return 0;
      }
    }
    if(gSystem->ProcessEvents()){
      //          cout <<"InteruptReceived"<<endl;
      //          return -1;
    }
  }         
  return _EVENT; 
};

int AMSChain::ValidateFromFile(const char *fname,bool stage){
  FILE* listfile = fopen(fname,"r");
  int i=0;
  bool castor=false;
  if (listfile) {
    char rname[1024];
    while (!feof(listfile)){
      fscanf(listfile,"%s\n", rname );
       if(strstr(rname,"/castor/") && stage){
       string rn=rname;
       int pos=rn.find("/castor/");
       string stager_get="stager_get -M ";
       stager_get+=(rname+pos);
       stager_get+=" 1>/dev/null 2>&1 &";
       system(stager_get.c_str());
       }
      i++; 
    }
    fclose(listfile); 
  }
  else {
    cerr << "AMSChain::ValidateFromFile-E-  Error opening file '" << fname << "';";
    return -1;
  }
  
  return i;
}
int AMSChain::AddFromFile(const char *fname,int first,int last, bool stagedonly,unsigned int timeout,char *pattern){
  ofstream  rejfile;
  ofstream  rejfile2;
  //TFile::SetOnlyStaged(stagedonly);
  //stagedonly=false;
  unsigned int ftimeout=TFile::GetOpenTimeout();
  if(timeout)TFile::SetOpenTimeout(timeout);
  FILE* listfile = fopen(fname,"r");
  if (listfile) {
    char rname[1024];
    int i=0;
    while (!feof(listfile)){
      fscanf(listfile,"%s\n", rname );
      if(i>=first && i<last &&(!pattern || strstr(rname,pattern))){
          bool staged=true;
          if(strstr(rname,"/castor/") ){
           string rn=rname;
           int pos=rn.find("/castor/");
           string stager_get="stager_get -M ";
           stager_get+=(rname+pos);
           stager_get+=" 1>/dev/null 2>&1 ";
           system(stager_get.c_str());
           stager_get="stager_qry -M ";
           stager_get+=(rname+pos);
           stager_get+=" | grep -c STAGED 2>&1";
           FILE *fp=popen(stager_get.c_str(),"r");
           char path[1024];
           if(fp==NULL){
             staged=false;
           }
           else if(fgets(path, sizeof(path), fp) != NULL && strstr(path,"1")){
             staged=true;  
           }
           else staged=false;
           pclose(fp);
           }
          if(staged || !stagedonly){
            int radd=   Add(rname);
            if(!radd){
            cerr<<"AMSChain::AddFromFile-W-FileNotStagedOrTimeOutAndWillNotBeProcessed"<<rname<<endl;
             char rejfilename[4095];
             sprintf(rejfilename,"%s_%06d_%06d_TMOUT",fname,first,last);
            if(!rejfile2){
             rejfile2.clear();
             rejfile2.open(rejfilename);
            }
            if(rejfile2){
              rejfile2<<rname<<endl;
            }
            else cerr<<"AMSChain::AddFromFile-W_UnableToOpenRejFile2 "<<rejfilename<<endl;
          } 

            }
          
          else {
            cerr<<"AMSChain::AddFromFile-W-FileNotStagedAndWillNotBeProcessed"<<rname<<endl;
             char rejfilename[4095];
             sprintf(rejfilename,"%s_%06d_%06d_STAGEIN",fname,first,last);
            if(!rejfile){
             rejfile.clear();
             rejfile.open(rejfilename);
            }
            if(rejfile){
              rejfile<<rname<<endl;
            }
            else cerr<<"AMSChain::AddFromFile-W_UnableToOpenRejFile "<<rejfilename<<endl;
          } 
         }       
      i++; 
    }
    fclose(listfile);
    if(rejfile)rejfile.close();
  }
  else {
    cerr << "AMSChain::AddFromFile-E-  Error opening file '" << fname << "';";
    TFile::SetOpenTimeout( ftimeout);
    return -1;
  }
  
  TFile::SetOpenTimeout( ftimeout);
  return 0;
}


int  AMSChain::LoadUF(char* fname){
  char cmd[200];
  char cmd1[200];
	
  std::string filename(fname);
  int pos=filename.find_last_of(".");
  std::string nameonly=filename.substr(0,pos);

#ifndef WIN32
  static void *handle=0;
  char *CC=getenv("CC");
  if(!CC){
    setenv("CC","g++",0);
  }
  char *AMSSRC=getenv("AMSSRC");
  if(!AMSSRC){
    setenv("AMSSRC","..",0);
  }
#ifdef __X8664__
  char m32[6]="-fPIC";
  char elf[11]="";
#else
  char m32[11]="-m32 -fPIC";
  char elf[11]="-melf_i386";
#endif
#ifdef _PGTRACK_
  sprintf(cmd,"$CC %s -g -D_PGTRACK_ -Wno-deprecated -I. -I$ROOTSYS/include -I$AMSSRC/include -c %s.C",m32,nameonly.c_str());
#else
  sprintf(cmd,"$CC %s  -Wno-deprecated -I. -I$ROOTSYS/include -I$AMSSRC/include -c %s.C",m32,nameonly.c_str());
#endif
  cout<< " Launching the Handle compilation with command: "<<cmd<<endl; 
  int $i=system(cmd);
  if(!$i){
#ifdef __APPLE__
    sprintf(cmd1,"ld  -dylib -ldylib1.o -undefined dynamic_lookup %s.o -o libuser.so",nameonly.c_str());
#else
    sprintf(cmd1,"ld %s  -shared %s.o -o libuserAMSEVD.so",elf,nameonly.c_str());
#endif
    char * error;
    $i=system(cmd1);
    if( !$i){  
      if(handle){
	dlclose(handle);
      }
      if(handle=dlopen("./libuserAMSEVD.so",RTLD_NOW)){
	void (*fhandler)()=(void (*)())
		dlsym(handle,"fgSelect");
	if((error=dlerror())!=0){
           fputs(error,stderr);
	   return 1;
	} else
	   fhandler();
	return 0;
      }
      cout <<dlerror()<<endl;
      return 1;
            
    }
  }
#else
  static HINSTANCE handle=0;
  typedef AMSNtupleHelper * (*MYPROC)(VOID);
  if(handle){
    FreeLibrary(handle);
    handle=0;
  }
  sprintf(cmd,"cl.exe -c %s.C -I%ROOTSYS%\\include  /EHsc /TP",nameonly.c_str());
  int i=system(cmd);
  if(!i){
    sprintf(cmd1,"cl.exe %s.obj -o libuser.so /LD /link -nologo -export:gethelper",nameonly.c_str());
    i=system(cmd1);
    if(!i){
      handle=LoadLibrary(".\\libuser.so");
      if(!handle){
	cout <<"  Unable to load lib "<<endl; 
	return 1;
      }
      else {
	MYPROC pa=(MYPROC)GetProcAddress(handle,"gethelper");
	if(pa){
	  AMSNtupleHelper::fgHelper= ((pa)()); 
	  cout << " ok "<< ((pa)())<<endl;
	  return 0;
	}  
	return 1;
      }
    }
  }
       
#endif
  return -1;

}



Long64_t AMSChain::Process(TSelector*pev,Option_t*option, Long64_t nentri, Long64_t firstentry){
#ifndef __ROOTSHAREDLIBRARY__
  return 0;
#else
  nentries=nentri;
   cout<<" nentires requested "<<nentries<<endl;
  int nthreads=fThreads;
  //TStreamerInfo**ts =new TStreamerInfo*[nthreads];
  //for(int i=0;i<nthreads;i++)ts[i]=0;
  long long nentr=0;

  AMSEventR::_NFiles=-fNtrees;
        
  //	#pragma omp parallel  default(none), shared(std::cout,option,nentries,firstentry,nentr,pev)
  ntree=fNtrees;
  if(nentries<0){
    ntree=-nentries;
    if(ntree>fNtrees)ntree=fNtrees;
    nentries=10000000000LL;
  }
  typedef multimap<uinteger,TChainElement*> fmap_d;
  typedef multimap<uinteger,TChainElement*>::iterator fmapi;
  fmap_d fmap;
  for(int i=0;i<fNtrees;i++){
    TString t1("/");
    TString t2(".");
    TString name(((TNamed*)fFiles->At(i))->GetTitle());
    TObjArray *arr=name.Tokenize(t1);
    TObjString *s=(TObjString*)arr->At(arr->GetEntries()-1); 
    TObjArray *ar1=s->GetString().Tokenize(t2);
    unsigned int k=atoi(((TObjString* )ar1->At(0))->GetString().Data());
    bool bad=false;
    for(int is=0;is<AMSEventR::BadRunList.size();is++){
         if(k==AMSEventR::BadRunList[is]){
           bad=true;
             break;
         }
    }
    if(!bad && k>=AMSEventR::MinRun && k<AMSEventR::MaxRun)fmap.insert(make_pair(k,(TChainElement*) fFiles->At(i)) );
      delete arr;
    delete ar1;
  }
  fmapi it=fmap.begin();
  if(ntree>fmap.size())ntree=fmap.size();
  cout <<"  AMSChain::Process-I-Files to be processed "<<ntree<<" out of "<<fNtrees<<endl;
  if(nthreads>ntree)nthreads=ntree;
  int*ia= new int[nthreads];
  for(int i=0;i<nthreads;i++)ia[i]=0;
#ifdef _OPENMP
  omp_set_num_threads(nthreads);
  AMSEventR::fgThreads=nthreads;
#endif
#pragma omp parallel 
  {
    int thr=0;
#ifdef _OPENMP
    thr=omp_get_thread_num();
#endif
#pragma omp  for schedule (dynamic)  nowait
    for(int i=0;i<ntree;i++){
      if(nentr>nentries || it==fmap.end()){
	continue;
      }
      TChainElement* element;
      TFile* file;
      TTree *tree;
      TSelector *curp=(TSelector*)((char*)pev+thr*fSize);
        TDirectory *gdir=0;
      fmapi itt=it;
        it++;
        for(int subdir=0;subdir<32;subdir++){

#pragma omp critical 
      {
      if(subdir==0){
	//  if(ts[thr]==0){
	//TStreamerInfo::fgInfoFactory=ts[thr]=new TStreamerInfo();
	// }
	//cout <<"thr "<<thr<<endl;
        // element=(TChainElement*) fFiles->At(i);
#ifdef CASTORSTATIC
        TRegexp d("^root:",false);
        TRegexp e("^rfio:",false);
       TString name(itt->second->GetTitle());
        //cout << " thr "<<thr<<" "<<name<<endl;          
        if(name.Contains(d))file=new TXNetFile(itt->second->GetTitle(),"READ");
        else if(name.Contains(e))file=new TRFIOFile(itt->second->GetTitle(),"READ");
        else file=new TFile(itt->second->GetTitle(),"READ");
#else 
	file= TFile::Open(itt->second->GetTitle(),"READ");
#endif
          gdir=gDirectory;
//          cout <<"gdirectory!!!! "<<gdir->GetName()<<" " <<endl;
}
          tree=0;
        bool subdirexist=false;
//        cout <<"  SUBDIR****** "<<subdir<<endl;
//          cout <<"gdirectory!!!! "<<gdir<<endl;
//          cout <<"gdirectory!!!!!! "<<gdir->GetName()<<" " <<endl;
        if(subdir>0){
         char dir[1024];
          sprintf(dir,"_%d",subdir);
          TDirectory*ok=gdir->GetDirectory(dir,false,"cd");
          if(ok){
             ok->cd();
             subdirexist=true;
          }
//          cout <<"gdirectory "<<gdir->GetName()<<" "<<subdirexist <<endl;
        }
        else subdirexist=true;
	if(file && subdirexist)tree=(TTree*)file->Get(itt->second->GetName());
        if(!tree){
          if(subdirexist)cerr<<"  AMSChain::Process-E-NoTreeFound file tree "<<itt->second->GetTitle()<<" "<<itt->second->GetName()<<endl;
	}
	else{
	  curp->SetOption(option);
	  curp->Init(tree);
	  curp->Notify();
	  cout <<"  "<<i<<" "<<itt->second<<" "<<AMSEventR::_Tree->GetEntries()<<" "<<nentr<<" "<<nentries<<endl;
	  //cout <<"  "<<i<<" "<<element->GetTitle()<<" "<<AMSEventR::_Tree->GetEntries()<<" "<<nentr<<" "<<nentries<<endl;
        }
      }
      if(tree){
        curp->Begin(tree);
        for(int n=0;n<AMSEventR::_Tree->GetEntries();n++){
           if(nentr>nentries)break;
	  try{
//            while(AMSEventR::_Lock&0x100000000){
//            }
//#pragma omp atomic 
//           AMSEventR::_Lock+=(1<<thr);
	    curp->Process(n);
//#pragma omp atomic 
//           AMSEventR::_Lock-=(1<<thr);
	  }
	  catch (...){
#pragma omp critical(rd)
	    if(AMSEventR::pService){
	      (*AMSEventR::pService).BadEv++;    
	      (*AMSEventR::pService).TotalEv++;
	      (*AMSEventR::pService).TotalTrig++;
	    }    
	  }
	}
      }

#pragma omp critical (cls)  
      {
	if(tree && AMSEventR::_Tree)nentr+=AMSEventR::_Tree->GetEntries();
	//        cout <<" finished "<<i<<" "<<endl;
      }
    }
	if(file)file->Close("R");
        if(file)delete file;

}  
#ifdef _OPENMP        
    //  this clause is because intel throutput mode deoesn;t work
    //   so simulating it
    ia[thr]=1;
      int nt=0;
      for(int j=0;j<nthreads;j++){
        if(!ia[j])nt++;
      } 
    cout <<" Thread "<<thr<<" Deactivated "<<nt <<" Threads Left"<<endl;
  for(;;){
      bool work=false;
      for(int j=0;j<nthreads;j++){
	if(!ia[j]){
	  work=true;
	  break;
	}
      }
      if(work)usleep(kmp_get_blocktime());  
      else break;
    }
#endif
  }
  AMSEventR::_NFiles=1;
  pev->Terminate();
  delete[] ia;
  return nentr;
#endif
}



int AMSChain::OpenOutputFile(const char* filename){
  fout= TFile::Open(filename,"RECREATE");
  if(!fout){
    cout <<" AMSChain::OpenOutpuFile-E- Cannot open "<<filename<<" for output"<<endl;
    return -1;
  }
  amsnew = CloneTree(0);
  TFile * input=GetFile();
  if(!input){cerr<<"AMSEventList::Write- Error - Cannot find input file"<<endl;return -2;}
  if(!rsetup){
    TTree* rsetup0=(TTree*) GetFile()->Get("AMSRootSetup");
    fout->cd();
    rsetup=rsetup0->CloneTree();
  }

#ifdef _PGTRACK_
  // Parameters
  enum {nfiles=7};
  char objlist[nfiles][40]={"TkDBc","TrCalDB","TrParDB","TrGainDB","TrReconPar","TrExtAlignDB","TrInnerDzDB"};
  for(int ii=0;ii<nfiles;ii++){
    TObject* obj=input->Get(objlist[ii]);
    if(obj) {fout->cd();obj->Write();}
  }
  // Datacards
  TDirectory *dir = (TDirectory *)input->Get("datacards");
  if (dir) {
    fout->cd();
    TDirectoryFile *dout = new TDirectoryFile(dir->GetName(),
					      dir->GetTitle());
    TList *klst = dir->GetListOfKeys();
    for (int ii=0; ii<klst->GetSize(); ii++) {
      if (klst->At(ii)) {
	TObject* obj=dir->Get(klst->At(ii)->GetName());
	if (obj) {dout->cd(); obj->Write();}
      }
    }
    fout->cd();
  }
#endif




  TObjString* obj2=(TObjString*)input->Get("AMS02Geometry");
  if(obj2) {fout->cd();obj2->Write("AMS02Geometry");}
  TObjString* obj3=(TObjString*)input->Get("DataCards");
  if(obj3) {fout->cd();obj3->Write("DataCards");}
  cout <<" AMSChain::OpenOutpuFile-I- The outputfile "<<filename<<" Has been opened"<<endl;
  return 0;
}  

void AMSChain::SaveCurrentEvent(){
  if(!fout) OpenOutputFile("SelectedEvents.root");
  if(!fout){
  cout <<" AMSChain::SaveCurrentEntry-E- Cannot open file  for output no events are saved"<<endl;
    return;
  }
  if(_EVENT){ _EVENT->GetAllContents();
  amsnew->Fill();
  }
  return;
}

void AMSChain::SaveCurrentEventCont(){
  if(!fout) OpenOutputFile("SelectedEvents.root");
  if(!fout){
  cout <<" AMSChain::SaveCurrentEntry-E- Cannot open file  for output no events are saved"<<endl;
    return;
  }
  if(_EVENT){
    _EVENT->SetCont();
    
    amsnew->Fill();
  }
  return;
}
 
void AMSChain::CloseOutputFile(){
  if(!fout) return;
  cout << "AMSChain::CloseOutputFile AMS ROOT file \"";
  cout << fout->GetName() << "\" with " << amsnew->GetEntries(); 
  cout << " selected events" << endl;
  fout->Write();
  fout->Close();
  fout=0;
  return;
}

//##################################################################
//#####################    AMSEventList      #######################
//##################################################################


ClassImp(AMSEventList);

ull  AMSEventList::GetRE(int run,int event){  
  ull aa=run;  
  ull aa2=aa<<32;
  ull bb=aa2|event; 
  return bb;
}

AMSEventList::AMSEventList(){
  //  _RUNs.reserve(10000);
  //_EVENTs.reserve(10000);
};

AMSEventList::AMSEventList(const char* filename){
  // _RUNs.reserve(10000);
  // _EVENTs.reserve(10000);
  Read(filename);
};
void AMSEventList::Add(int run, int event){
  _RUNs.insert(run);
  //  _EVENTs.push_back(event);
  _runeve.insert(GetRE(run,event));
};

void AMSEventList::Add(AMSEventR* pev){
  Add(pev->Run(),pev->Event());
};

void AMSEventList::Remove(int run, int event){
  set<ull>::iterator pp;
  _runeve.erase(GetRE(run,event));
  pp=_runeve.lower_bound(GetRE(run,0));
  if((  pp      == _runeve.end()) || 
     ((*pp)>>32 != run  )     ){
    _RUNs.erase(run);
  }
  return;
};

void AMSEventList::Remove(AMSEventR* pev){
  Remove(pev->Run(),pev->Event());
};

bool AMSEventList::Contains(int run, int event){
  return (_runeve.count(GetRE(run,event))>0);
};

bool AMSEventList::Contains(AMSEventR* pev){
  return Contains(pev->Run(),pev->Event());
};

void AMSEventList::Reset(){
  _RUNs.clear();
  //  _EVENTs.clear();
  _runeve.clear();
};

int  AMSEventList::GetUniqeRun(int i){
  set<int>::iterator pp=_RUNs.begin();
  int kk=0; 
  while(kk++<i) pp++; 
  return *pp;
}

int  AMSEventList::GetRun(int i){
  ullp pp=_runeve.begin();
  int kk=0; 
  while(kk++<i) pp++; 
  return GetRun(*pp);
}

int  AMSEventList::GetEvent(int i){
  ullp pp=_runeve.begin();
  int kk=0; 
  while(kk++<i) pp++; 
  return GetEvent(*pp);
}


void AMSEventList::Read(const char* filename){
  FILE* listfile = fopen(filename,"r");
  if (listfile) {
    int run, event;
    while ( fscanf(listfile,"%d %d\n", &run, &event)==2 ) Add(run, event);
    fclose(listfile);
  } else {
    cout << "AMSEventlist: Error opening file '" << filename << "';";
    cout << " assuming an empty list" << endl;
  }
};

void AMSEventList::Write(){
  cout << "AMSEventList::Dumping a list with ";
  cout << this->GetEntries(); 
  cout << " selected events..." << endl;
  set<ull>::iterator pp;
  for (pp=_runeve.begin(); pp!=_runeve.end(); pp++) {
    cout << GetRun(*pp) << "\t" << GetEvent(*pp) << endl;
  }
};

void AMSEventList::Write(const char* filename){
  cout << "AMSEventList::Writing ASCII file \"";
  cout << filename << "\" with " << this->GetEntries(); 
  cout << " selected events" << endl;
  FILE* listfile = fopen(filename,"w");
  set<ull>::iterator pp;
  for (pp=_runeve.begin(); pp!=_runeve.end(); pp++) {
    fprintf(listfile,"%10d %10d\n",GetRun(*pp),GetEvent(*pp));
  }
  fclose(listfile);
};

void AMSEventList::Write(AMSChain* chain,const char * outfilename){
  if(!dynamic_cast<AMSChain*>(chain)){
    cerr<<"AMSEventList::Write-E-ChainIsNULL "<<endl;
    return;
  }
  if(chain->OpenOutputFile(outfilename)!=0) return;
  ull max=chain->GetEntries();
  ull grain=100;
  ull ff=max/grain;
  ull found=0;
  cout <<"AMSEventList::ExtractFromDirs-I- Event int the chain are "<<max<<endl;
  chain->Rewind();
  for( ull  jj=0;jj< max ;jj++){
    if(jj%ff==0) printf("AMSEventList::ExtractFromDirs-I- Done %2d %7d out of %d \n",jj/ff*grain,jj,max);
    AMSEventR*  ev=chain->GetEvent();
    if (ev &&  Contains(ev) ){
      chain->SaveCurrentEvent();
      found++;
      printf("AMSEventList::Writing event ............ %12d %12d\n"
	     , ev->Run(), ev->Event());
    }
  }
  printf("AMSEventList found %d events out of %d\n",found,GetEntries());
  chain->CloseOutputFile();
  return;
};

int AMSEventList::ExtractFromDirs(char* fname){

  vector<string> dirs;
  FILE* listfile = fopen(fname,"r");
  if (listfile) {
    char rname[600];
    while (1){
      fscanf(listfile,"%s\n", rname );
      if(feof(listfile)) { fclose(listfile); break;}
      dirs.push_back(string(rname));
    }
  }
  else {
    cerr << "AMSEventList::ExtractFromDirs-E-  Error opening file '" << fname << "';";
    return -1;
  }
  AMSChain chain;
  set<int>::iterator rr;
  for (rr=_RUNs.begin();rr!=_RUNs.end();rr++){
    for(int dd=0;dd<dirs.size();dd++){
      char nnn[700];
      sprintf(nnn,"%s/%d.*.root",dirs[dd].c_str(), *rr);
      printf("AMSEventList::ExtractFromDirs-I- addinf %s\n",nnn);
      chain.Add(nnn);
    }
  }
  TObjArray* arr=chain.GetListOfFiles();
  TChainElement* el=0;
  TIter next(arr);
  printf("AMSEventList::ExtractFromDirs-I- chain added with %d files\nThey are:\n",arr->GetEntries());
  while(el=(TChainElement*) next())
    printf("%s\n",el->GetTitle());

  Write(&chain);
  return 0;


}


int AMSChain::GenUFSkel(char* filename){
  FILE*fname= fopen(filename,"w");
  if(!fname){
    printf("Problem opening new file %s\n",filename);
    return -1;
  }
  fprintf(fname,"\n");
  fprintf(fname,"#include \"AMSNtupleHelper.h\"\n");
  fprintf(fname,"\n");
  fprintf(fname,"static AMSNtupleHelper * fgHelper=0;\n");
  fprintf(fname,"\n");
  fprintf(fname,"extern \"C\" AMSNtupleHelper * gethelper();\n");
  fprintf(fname,"\n");
  fprintf(fname,"void* __dso_handle=0;\n");
  fprintf(fname,"\n");
  fprintf(fname,"class AMSNtupleSelect: public AMSNtupleHelper{\n");
  fprintf(fname,"public:\n");
  fprintf(fname,"  AMSNtupleSelect(){};\n");
  fprintf(fname,"  bool IsGolden(AMSEventR *ev){\n");
  fprintf(fname,"    static int a=0;\n");
  fprintf(fname,"    // \n");
  fprintf(fname,"    // This is a user function to be modified\n");
  fprintf(fname,"    //  return true if event has to be drawn false otherwise.\n");
  fprintf(fname,"    // Example take the even event numbers\n");
  fprintf(fname,"    if(ev && ev->Event()%2==0) return true;\n");
  fprintf(fname,"    else return false;\n");
  fprintf(fname,"    }\n");
  fprintf(fname,"};\n");
  fprintf(fname,"//\n");
  fprintf(fname,"//  The code below should not be modified\n");
  fprintf(fname,"//\n");
  fprintf(fname,"#ifndef WIN32\n");
  fprintf(fname,"extern \"C\" void fgSelect(){\n");
  fprintf(fname,"  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); \n");
  fprintf(fname,"  cout <<\"  Handle Loadedd \"<<endl;\n");
  fprintf(fname,"}\n");
  fprintf(fname,"\n");
  fprintf(fname,"#else\n");
  fprintf(fname,"#include <windows.h>\n");
  fprintf(fname,"BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle\n");
  fprintf(fname,"             DWORD fdwReason,              // reason called \n");
  fprintf(fname,"             LPVOID lpvReserved)           // reserved \n");
  fprintf(fname,"{ \n");
  fprintf(fname," \n");
  fprintf(fname,"  switch (fdwReason) \n");
  fprintf(fname,"    { \n");
  fprintf(fname,"      // The DLL is loading due to process \n");
  fprintf(fname,"      // initialization or a call to LoadLibrary. \n");
  fprintf(fname," \n");
  fprintf(fname,"    case DLL_PROCESS_ATTACH: \n");
  fprintf(fname,"      fgHelper=new AMSNtupleSelect();\n");
  fprintf(fname,"      cout <<\"  Handle Loadedd \"<<fgHelper<<endl;\n");
  fprintf(fname,"      break;\n");
  fprintf(fname,"\n");
  fprintf(fname,"    case DLL_PROCESS_DETACH: \n");
  fprintf(fname,"      if(fgHelper){\n");
  fprintf(fname,"        delete fgHelper;\n");
  fprintf(fname,"        fgHelper=0;\n");
  fprintf(fname,"      } \n");
  fprintf(fname," \n");
  fprintf(fname,"      break; \n");
  fprintf(fname," \n");
  fprintf(fname,"    default: \n");
  fprintf(fname,"      break; \n");
  fprintf(fname,"    } \n");
  fprintf(fname,"  return TRUE;\n");
  fprintf(fname,"}\n");
  fprintf(fname,"extern \"C\" AMSNtupleHelper * gethelper(){return fgHelper;}\n");
  fprintf(fname,"\n");
  fprintf(fname,"#endif\n");
  fclose(fname);

  printf("AMSChain::GenUFSkel --> Successfully generated skeleton file  %s\n",filename);

  return 0;
}

Int_t AMSChain::Add(const char* name, Long64_t nentries){

//check if svcClass present in name

         string cname=name;
         int  found=cname.find("?svcClass=");
         string aname=cname;
         if(found>=0){
          aname=cname.substr(0,found);
        }
        Int_t ret=TChain::Add(aname.c_str(),nentries);
        TRegexp d("^root:",false);
        TRegexp e("^xroot:",false);
        TString tname(name);
        if(tname.Contains(d) || tname.Contains(e)){
          if(found>=0){
           aname=cname.substr(found);
          }
          else if(getenv("STAGE_SVCCLASS")){
            aname="?svcClass=";
            aname+=getenv("STAGE_SVCCLASS");
          }
          else{
           aname="";
          }
           TObjArray* arr=GetListOfFiles();
           TChainElement* el=0;
            TIter next(arr);
            while(el=(TChainElement*) next()){
               string title=el->GetTitle();
               int pos=title.find(aname);
               if(pos<0){
                 title+=aname.c_str();
                 el->SetTitle(title.c_str());
#ifdef CASTORSTATIC 
                 TXNetFile tfile((const char*)el->GetTitle());
                 if(tfile.IsZombie()){
                  arr->Remove(el);
//                   cout<<"  zombie...  "<<el->GetTitle()<<" "<<endl;
                  return 0;
                 }
#endif
              }
 //              cout <<" title "<<el->GetTitle()<<endl;
             }
        }
        return ret; 
}
