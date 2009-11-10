
//  $Id: main.cxx,v 1.36 2009/11/10 18:47:10 choutko Exp $
#include <TASImage.h>
#include <TRegexp.h>
#include <TRootApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TGeometry.h>
#include "AMSDisplay.h"
#include <iostream>
#include <fstream>
#ifndef WIN32
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#endif
#ifndef __APPLE__
#include <TRFIOFile.h>
#include <TXNetFile.h>
#endif
#include <stdlib.h>
#include <signal.h>
#include "AMSNtupleV.h"
#include "main.h"
#include <TString.h>
#include <getopt.h>
TString * Selector;
TString * Selectorf=0;
extern void * gAMSUserFunction;
void OpenChain(AMSChain & chain, char * filename, char *lastfile);

#ifndef WIN32
#ifdef __APPLE__
static int Selectsdir(dirent64 *entry=0);
static int Select(dirent64 *entry=0);
#else
static int Selectsdir(const dirent64 *entry=0);
static int Select(const dirent64 *entry=0);
static int Sort(dirent64 ** e1,  dirent64 ** e2);

#endif
#endif

  bool lastfilefound=true;
  bool notlast=true;
void Myapp::HandleIdleTimer(){
  
  if(fDisplay){
    StopIdleing();
    fDisplay->ShowNextEvent(1);
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
  char * filename = 0;		// default file name

  int c;
  int sec=10;
  char title[256];
  char lastfile[1024]="\0";
  strcpy(title,"AMS Event Display");
  int option_index = 0;
  static struct option long_options[] = {
    {"title", 1, 0, 't'},
    {"monitor", 1, 0, 'm'},
    {"help",    0, 0, 'h'},
    {"scan",  1, 0, 's'},
    {0, 0, 0, 0}
  };

  if ( argc > 1 ) {		// now take the file name
    filename = argv[1];
  }   
  while (1) {
    c = getopt_long (argc, argv, "t:hHs:?", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 's':             /* display */
      sec=atoi(optarg);
      break;
    case 't':             
      strcpy(title, optarg);
      break;
    case 'm':             
      if(strlen(optarg)){
        strcpy(lastfile, optarg);
        lastfilefound=false;
      }
      break;
    case 'h':
    case 'H':
    case '?':
    default:            /* help */
      cout<<"$amsed(c) file -scan[s] -title[t] -monitor[lastfile]"<<endl;
      return 0;
      break;
    }
  }
  AMSNtupleV *pntuple= new AMSNtupleV();
  AMSChain chain(pntuple,"AMSRoot");
  if(filename){
    OpenChain(chain,filename,lastfile); 
  }  
  printf("opening file %s...\n", filename);
  int err=0;
  int argcc=1;
#if defined(WIN32) || defined(__APPLE__)
  Myapp *theApp = new Myapp("App", &argcc, argv);
#else

  gVirtualX=new TGX11("X11","Root Interface to X11");
  //gGuiFactory=new TRootGuiFactory();
  TASImage a;
  Myapp *theApp = new Myapp("App", &argcc, argv);

  theApp->SetStatic();
#endif
  //  gDebug=6; 



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

  char *geoFile_new = "ams02.geom";
  strcat(geoFile,geoFile_new);
  TFile fgeo(geoFile);

  TGeometry *geo = (TGeometry *)fgeo.Get("ams02");
  if (geo==0) {
    cerr << "AMSEventDisplay-F-Failed to open " << geoFile
         << "\n or cannot find the Geometry in it" << endl;
    exit(1);
  }


  AMSDisplay * amd= new AMSDisplay(title,geo,&chain,sec);
  amd->SetApplication(theApp);
  amd->Init();
  theApp->SetDisplay(amd);  
  theApp->RemoveIdleTimer();
  amd->DrawEvent();
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


void OpenChain(AMSChain & chain, char * filenam, char *lastfile){
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
   if(a.Contains(b)){
#if !defined(WIN32) && !defined(__APPLE__)
    TRFIOFile f("");
    TXNetFile g("");
#endif
    strcpy(filename,filenam);
   }
   else if(a.Contains(c)){
    strcpy(filename,filenam);
  }
  else if(a.Contains(d)){
    strcpy(filename,filenam);
  }
  else if(a.Contains(e)){
    strcpy(filename,filenam);
  }
   else if(a.Contains(f)){
    strcpy(filename,"rfio:");
    strcat(filename,filenam);
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
                
                if(l-k-1>0)Selector= new TString(filename+k+1,l-k-1);
                else Selector=0;
                dirent64 ** namelistsubdir;
                cout <<"  scanning "<<ts<<" "<<Selector<<" l "<<l<<" "<<i<<endl;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
                for( int nsd=0;nsd<nptrdir;nsd++){
                  char fsdir[1023];
                  strcpy(fsdir,ts.Data());
                  strcat(fsdir,namelistsubdir[nsd]->d_name);
                  strcat(fsdir,filename+l);
                  cout << "found dir "<<fsdir<<endl;
                  OpenChain(chain,fsdir,lastfile);
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
                if(!Selectorf)Selectorf= new TString("root$");
                dirent64 ** namelistsubdir;
                cout <<"  scanning wild"<<ts<<endl;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Select,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
                for( int nsd=0;nsd<nptrdir;nsd++){
                  char fsdir[1023];
                  strcpy(fsdir,ts.Data());
                  strcat(fsdir,namelistsubdir[nsd]->d_name);
                  //cout << "found file "<<fsdir<<endl;
                  if(nsd<nptrdir-1)notlast=true;
                  else notlast=false;   
                  OpenChain(chain,fsdir,lastfile);
               
                } 
                return;               
	  }
	}                  
      }
    }
  }
#endif
  struct stat64 statbuf;
  stat64((const char*)filename, &statbuf);
     time_t t;
     time(&t);
  if(lastfilefound &&statbuf.st_size){
     if(t-statbuf.st_mtime>60 || notlast){     
      strcpy(lastfile,filename);
//      cout <<" added "<<lastfile<<" "<<statbuf.st_size<<endl;
      chain.Add(filename);
     }
     else {
       cout<<"main-W-OpenChainFileNotAdded "<<filename<<t<<" "<<statbuf.st_mtime<<endl;
       return;
     }
  }
  if(!lastfilefound && strstr(filename,lastfile)){
       cout<<"main-I-OpenChainLastFileDetected "<<filename<<endl;
    lastfilefound=true;
  }
}



#ifndef WIN32
int Selectsdir(
#ifndef __APPLE__
	       const dirent64 *entry){
#else
  dirent64 *entry){
#endif
  if(Selector){
    TString a(entry->d_name);
    TRegexp b(Selector->Data(),true);
    return a.Contains(b) && entry->d_name[0]!='.';
  }
  else return entry->d_name[0]!='.';
}
#endif



#ifndef WIN32
int Select(
#ifndef __APPLE__
	       const dirent64 *entry){
#else
  dirent64 *entry){
#endif
  if(Selectorf){
    TString a(entry->d_name);
    TRegexp b(Selectorf->Data(),false);
    return a.Contains(b) && entry->d_name[0]!='.';
  }
  else return entry->d_name[0]!='.';
}

int Sort(dirent64 ** e1,  dirent64 ** e2){
return strcmp((*e1)->d_name,(*e2)->d_name);
}


#endif
