//  $Id$
#include <TRegexp.h>
#include <TRootApplication.h>
#include <TFile.h>
#include <TChainElement.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGeometry.h>
#include <TPDF.h>
#include "AMSDisplay.h"
#include <iostream>
#include <fstream>
#ifndef WIN32
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#endif
#ifdef CASTORSTATIC
#include <TRFIOFile.h>
#include <TCastorFile.h>
#include <TXNetFile.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include "AMSNtupleV.h"
#include "main.h"
#include <TString.h>
#include <getopt.h>
TString * Selector=0;
TString * Selectorf=0;
extern void * gAMSUserFunction;
void OpenChain(AMSChain & chain, char * filename);

#ifndef WIN32
#ifdef __APPLE__
#if __OSXVER__ >= 1080
static int Selectsdir(const dirent *entry=0);
static int Select(const dirent *entry=0);
#else
static int Selectsdir(dirent *entry=0);
static int Select(dirent *entry=0);
#endif
static int Sort( dirent ** e1,   dirent ** e2);
#else
static int Selectsdir(const dirent64 *entry=0);
static int Select(const dirent64 *entry=0);
static int Sort(dirent64 ** e1,  dirent64 ** e2);

#endif
#endif
int add=0;
bool notlast=true;
bool monitor=false;
time_t lasttime=0;;
AMSChain *pchain;
char *filename=0;
char lastfile[32767]="\0";
void Myapp::HandleIdleTimer(){
  
  if(fDisplay){
    StopIdleing();
    bool ok= fDisplay->ShowNextEvent(1);
    if(!ok){
      cout<<"AMSDisplay-I-FinishedScan "<<endl;
      if(monitor){
	add=0;
	for(int k=0;k<30;k++){
	  sleep(1);
	  if(gSystem->ProcessEvents()){
	    cout <<"InteruptReceived"<<endl;
	  }        

	}
	char tmp[127];
	ofstream fbout (".amsedc");
	if(fbout){
	  fbout<<lasttime;
	  cout <<"AMSDisplay-I-LastTimeModified "<<lasttime<<endl;
	  fbout.close();
	}
	OpenChain(*pchain,filename);
      }
    }
    StartIdleing();
  }
  //  SetReturnFromRun(1);
  //Terminate();
  //cout <<"exiting handler"<<endl;
}

void (handler)(int);


int main(int argc, char *argv[]){

  // First create application environment. If you replace TApplication
  // by TRint (which inherits from TApplication) you will be able
  // to execute CINT commands once in the eventloop (via Run()).
#ifndef WIN32
  *signal(SIGFPE, handler);
  *signal(SIGCONT, handler);
  *signal(SIGTERM, handler);
  *signal(SIGINT, handler);
  *signal(SIGQUIT, handler);
#endif
  bool pall=false;
  int c;
  int sec=10;
  char title[256];
  strcpy(title,"AMS Event Display");
  int option_index = 0;
  static struct option long_options[] = {
    {"title", 1, 0, 't'},
    {"monitor", 1, 0, 'm'},
    {"help",    0, 0, 'h'},
    {"scan",  1, 0, 's'},
    {"nosetup",0,0,'n'},
    {"printsel",0,0,'p'},
    {0, 0, 0, 0}
  };

  if ( argc > 1 ) {		// now take the file name
    filename = argv[1];
  }   
  fstream fbin;
  while (1) {
    c = getopt_long (argc, argv, "pnt:hHmMs:?", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 'n':
      AMSEventR::ProcessSetup=0;
      break;
    case 's':             /* display */
      sec=atoi(optarg);
      break;
    case 't':             
      strcpy(title, optarg);
      break;
    case 'm':             
      monitor=true;
      break;
    case 'M':             
      monitor=true;
      fbin.open(".amsedc");
      if(fbin){
	fbin>>lasttime;
	cout <<"  LastTime Set "<<lasttime;
	fbin.close();
      }
      fbin.open(".amsedc");
      if(fbin){
	fbin<<(lasttime-1000);
	fbin.close();
      }
      break;
    case 'p':
      pall=true;
      break;
    case 'h':
    case 'H':
    case '?':
    default:            /* help */
      cout<<"$amsed(c) file -scan[s] -title[t] -m(M)onitor -nosetup"<<endl;
      return 0;
      break;
    }
  }
  AMSNtupleV *pntuple= new AMSNtupleV();
  AMSChain chain(pntuple,"AMSRoot");
  pchain=&chain;
  if(filename){
    //chain.Reset();
    OpenChain(chain,filename); 
  }  
  printf("opening file %s...\n", filename);
  int err=0;
  int argcc=1;
#if defined(WIN32) || defined(__APPLE__)
  Myapp *theApp = new Myapp("App", &argcc, argv);
#else
  // new TGClient();
  //gVirtualX=new TGX11("X11","Root Interface to X11");
  //gGuiFactory=new TRootGuiFactory();
  //gDebug=2;
  Myapp *theApp = new Myapp("App", &argcc, argv);
  gGuiFactory=new TRootGuiFactory();  
  theApp->SetStatic();
#endif



  //
  //Get the Geometry from the ROOT file  (obsolete, should be done via geomanager)
  //        --------
  char *geo_dir=getenv("AMSGeoDir");
  if ( !geo_dir || strlen(geo_dir) == 0 ) {
    cerr <<"AMSEventDisplay-F-AMSGeoDir Not Defined"<<endl;
    exit(1);
  }

  char geoFile[256];
  strcpy(geoFile,geo_dir);
  const char *geoFile_new  = "ams02.geom";
  const char *geofile_perm = "ams02.pm.geom";
  cout <<" pchain->getsetup() "<<pchain->getsetup()<<endl; 
  if(!strcmp(pchain->getsetup(),"AMS02P")){
    strcat(geoFile,geofile_perm);
  }
  else strcat(geoFile,geoFile_new);
  cout<<"  open geo file "<<geoFile<<endl; 
  TFile fgeo(geoFile);

  TGeometry *geo = (TGeometry *)fgeo.Get("ams02");
  if (geo==0) {
    cerr << "AMSEventDisplay-F-Failed to open " << geoFile
         << "\n or cannot find the Geometry in it" << endl;
    exit(1);
  }


  AMSDisplay * amd= new AMSDisplay(title,geo,&chain,sec,monitor,pall);
  amd->SetApplication(theApp);
  amd->Init();
  theApp->SetDisplay(amd);  
  theApp->RemoveIdleTimer();
  amd->DrawEvent();
  if(monitor){
    amd->StartStop(true);
    amd->ReLoad();
  }
  theApp->Run();
  return 0;
  


}
//---------------------------------------




void (handler)(int sig){
#ifndef WIN32
  switch(sig){
  case  SIGFPE:
    cerr <<" FPE intercepted"<<endl;
    break;
  case  SIGTERM:
    cerr <<" SIGTERM intercepted"<<endl;
    exit(1);
    break;
  case  SIGINT:
    cerr <<" SIGTERM intercepted"<<endl;
    exit(1);
    break;
  case  SIGQUIT:
    cerr <<" Process suspended"<<endl;
    pause();
    break;
  case  SIGCONT:
    cerr <<" Process resumed"<<endl;
    break;
  }
#endif
}


void OpenChain(AMSChain & chain, char * filenam){
  //  
  //  root can;t cope with multiple wild card so we must do it ourselves
  //
  char filename[255];
  if(strlen(filenam)==0 || strlen(filenam)>sizeof(filename)-1){
    cerr <<"OpenChain-F-InvalidFileName "<<filenam<<endl;
    return;
  }
  TString a(filenam);
  TRegexp b("^castor:",false);
  TRegexp c("^http:",false);
  TRegexp d("^root:",false);
  TRegexp e("^rfio:",false);
  TRegexp f("^/castor",false);
  bool wildsubdir=false;
  bool wild=false;
  bool remote=false;
  if(a.Contains(b)){
#ifdef CASTORSTATIC
    TRFIOFile f(""); 	 
    TXNetFile g(""); 	 
    TCastorFile h(""); 	 
#endif
    strcpy(filename,filenam);
    remote=true;
  }
  else if(a.Contains(c)){
    strcpy(filename,filenam);
    remote=true;
  }
  else if(a.Contains(d)){
    strcpy(filename,filenam);
    remote=true;
  }
  else if(a.Contains(e)){
    strcpy(filename,filenam);
    remote=true;
  }
  else if(a.Contains(f)){
    strcpy(filename,"rfio:");
    strcat(filename,filenam);
    remote=true;
  }
  else{ 
#ifndef WIN32
    if(filenam[0]!='/'){
      strcpy(filename,"./");
      strcat(filename,filenam);
    }
    else strcpy(filename,filenam); 
#else
    strcpy(filename,filenam); 
#endif
    bool go=false;
    for(int i=strlen(filename)-1;i>=0;i--){
      if(filename[i]=='/')go=true;
      if(go && filename[i]=='*'){
	wildsubdir=true;
	break;
      }
      else if(filename[i]=='*'){
	wild=true;
      }
    }
  }
#ifndef WIN32
  if(wildsubdir){
    for(int i=0;i<strlen(filename);i++){
      if (filename[i]=='*'){
        for(int k=i-1;k>=0;k--){
	  if(filename[k]=='/'){
	    TString ts(filename,k+1);
	    for(int l=i+1;l<strlen(filename);l++){
	      if(filename[l]=='/'){
                
                if(l-k-1>0){
		  if(Selector)delete Selector;
		  Selector= new TString(filename+k+1,l-k-1);
                }
                else if(Selector){
		  delete Selector;
		  Selector=0;
                }
		//                cout <<"  scanning "<<ts<<" "<<Selector<<" l "<<l<<" "<<i<<endl;
#ifdef __APPLE__
                dirent ** namelistsubdir;
#if __OSXVER__ >= 1080
                int nptrdir=scandir(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const dirent**, const dirent**)>(&Sort));
#else
		int nptrdir=scandir(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#endif
#elif defined(__LINUXNEW__)
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&Sort));
#elif defined(__LINUXGNU__)
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#else
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,&Sort);
#endif
                for( int nsd=0;nsd<nptrdir;nsd++){
                  char fsdir[1023];
                  strcpy(fsdir,ts.Data());
                  strcat(fsdir,namelistsubdir[nsd]->d_name);
                  strcat(fsdir,filename+l);
                  cout << "found dir "<<fsdir<<endl;
                  OpenChain(chain,fsdir);
                } 
                return;               
	      }
	    }
	  }
	}                  
      }
    }
  }
  else if(wild){
    for(int i=0;i<strlen(filename);i++){
      if (filename[i]=='*'){
        for(int k=i-1;k>=0;k--){
	  if(filename[k]=='/'){
	    TString ts(filename,k+1);
	    if(strlen(filename)-k-1>0){
	      if(Selectorf)delete Selectorf;
	      Selectorf= new TString(filename+k+1,strlen(filename)-k-1);
	    }
	    else if(Selectorf){
	      delete Selectorf;
	      Selectorf=0;
	    }
                 
	    cout <<"  scanning wild"<<ts<<endl;
#ifdef __APPLE__
	    dirent ** namelistsubdir;
#if __OSXVER__ >= 1080
	    int nptrdir=scandir(ts.Data(),&namelistsubdir,Select,reinterpret_cast<int(*)(const dirent**, const dirent**)>(&Sort));
#else
	    int nptrdir=scandir(ts.Data(),&namelistsubdir,Select,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#endif
#elif defined(__LINUXNEW__)
	    dirent64 ** namelistsubdir;
	    int nptrdir=scandir64(ts.Data(),&namelistsubdir,Select,reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&Sort));
#elif defined(__LINUXGNU__)
	    dirent64 ** namelistsubdir;
	    int nptrdir=scandir64(ts.Data(),&namelistsubdir,Select,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#else
	    dirent64 ** namelistsubdir;
	    int nptrdir=scandir64(ts.Data(),&namelistsubdir,Select,&Sort);
#endif
	    for( int nsd=0;nsd<nptrdir;nsd++){
	      char fsdir[1023];
	      strcpy(fsdir,ts.Data());
	      strcat(fsdir,namelistsubdir[nsd]->d_name);
	      if(nsd<nptrdir-1)notlast=false;
	      else notlast=false;   
	      OpenChain(chain,fsdir);
               
	    } 
	    return;               
	  }
	}
      }
    }
  }
#endif
#ifdef __APPLE__
  struct stat statbuf;
  stat((const char*)filename, &statbuf);
#else
  struct stat64 statbuf;
  stat64((const char*)filename, &statbuf);
#endif
  time_t t;
  time(&t);
  if(remote || (statbuf.st_mtime>=lasttime &&statbuf.st_size)){
    if( (t-statbuf.st_mtime>60 || notlast || !monitor) && (strlen(lastfile)<2 || strcmp(lastfile,filename))){     
      strcpy(lastfile,filename);
      cout <<" added "<<filename<<" "<<statbuf.st_size<<endl;
      if(add++<3000){
	lasttime=statbuf.st_mtime;
	chain.Add(filename);
	static int init =0;
	if(init==0){
	  init=1;
	  TObjString s("");
	  TObjArray* arr=chain.GetListOfFiles();
	  TIter next(arr);
	  TChainElement* el=(TChainElement*) next();
	  if(el){
            TFile * rfile=TFile::Open(el->GetTitle(),"READ");
            if(rfile){
              s.Read("DataCards");
              cout <<s.String()<<endl; 
            }
	  }
	}
      }
    }
    else {
      cout<<"main-W-OpenChainFileNotAdded "<<filename<<t<<" "<<statbuf.st_mtime<<endl;
      return;
    }
  }
}




#ifndef WIN32

#ifdef __APPLE__
#if __OSXVER__ >= 1080
int Selectsdir(const dirent *entry)
#else
  int Selectsdir(dirent *entry)
#endif
#else
  int Selectsdir(  const dirent64 *entry)
#endif
{
  if(Selector){
    TString a(entry->d_name);
    TRegexp b(Selector->Data(),true);
    return a.Contains(b) && entry->d_name[0]!='.';
  }
  else return entry->d_name[0]!='.';
}
#endif



#ifndef WIN32

#ifdef __APPLE__
#if __OSXVER__ >= 1080
int Select(const dirent *entry)
#else
  int Select(dirent *entry)
#endif
#else
  int Select( const dirent64 *entry)
#endif
{
  if(Selectorf){
    TString a(entry->d_name);
    TRegexp b(Selectorf->Data(),true);
    return a.Contains(b) && entry->d_name[0]!='.';
  }
  else return entry->d_name[0]!='.';
}

#ifdef __APPLE__
int Sort( dirent ** e1,   dirent ** e2)
#else
  int Sort(dirent64 ** e1,  dirent64 ** e2)
#endif
{
  return strcmp((*e1)->d_name,(*e2)->d_name);
}


#endif
