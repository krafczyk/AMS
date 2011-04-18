//  $Id: main.cxx,v 1.35 2011/04/18 21:24:17 choutko Exp $
#include <TRegexp.h>
#include <fstream.h>
#include <TChain.h>
#include <TRootApplication.h>
#include <TFile.h>
#include <TTree.h>
#include <TGeometry.h>
#include "AMSDisplay.h"
#include <iostream>
#include <fstream>
#ifndef WIN32
#include <unistd.h> 
#include <sys/stat.h>
#include <sys/file.h>
#endif
#include <TRFIOFile.h>
#include <TCastorFile.h>
#include <TXNetFile.h>

#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include "AMSNtuple.h"
#include "AMSAntiHist.h"
#include "AMSTrackerHist.h"
#include "AMSTOFHist.h"
#include "AMSLVL1Hist.h"
#include "AMSLVL3Hist.h"
#include "AMSRICHHist.h"
#include "AMSECALHist.h"
#include "AMSTRDHist.h"
#include "AMSAxAMSHist.h"
#include "AMSGenHist.h"
#include "AMSEverything.h"
#include "main.h"
#ifndef WIN32
#include <dirent.h>
#endif
#include <TString.h>
#ifndef __APPLE__
#include <TRFIOFile.h>
#include <TXNetFile.h>
#endif
TString * Selector;
TString * Selectorf=0;
extern void * gAMSUserFunction;
void OpenChain(AMSChain & chain, char * filename);
#ifndef WIN32
#ifdef __APPLE__
static int Selectsdir(dirent *entry=0);
static int Select(dirent *entry=0);
static int Sort(dirent ** e1,  dirent ** e2);
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
  StopIdleing();
  if(fDisplay)fDisplay->DispatchProcesses();
  SetReturnFromRun(1);
  StartIdleing(); 
  Terminate();
  cout <<"exiting handler"<<endl;
     if(monitor){
      add=0;
       sleep(1);
       for(int k=0;k<30;k++){
        if(gSystem->ProcessEvents()){
          cout <<"InteruptReceived"<<endl;
        }        

       }
       char tmp[127];
      ofstream fbout (".offmonc");
      if(fbout){
       fbout<<lasttime;
       cout <<"AMSDisplay-I-LastTimeModified "<<lasttime<<endl;
       fbout.close();
      }
       OpenChain(*pchain,filename);
     }

}

void (handler)(int);
int main(int argc, char *argv[])
{
  // First create application environment. If you replace TApplication
  // by TRint (which inherits from TApplication) you will be able
  // to execute CINT commands once in the eventloop (via Run()).
  /*
   *signal(SIGFPE, handler);
   *signal(SIGCONT, handler);
   *signal(SIGTERM, handler);
   *signal(SIGINT, handler);
   *signal(SIGQUIT, handler);
   */
  int c;
  int sec=10;
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

  fstream fbin;
  while (1) {
    c = getopt_long (argc, argv, "t:hHmMs:?", long_options, &option_index);
    if (c == -1) break;

    switch (c) {
    case 'm':             
      monitor=true;
      break;
    case 'M':             
      monitor=true;
      fbin.open(".offmonc");
      if(fbin){
       fbin>>lasttime;
       cout <<"  LastTime Set "<<lasttime;
       fbin.close();
     }
      fbin.open(".offmonc");
      if(fbin){
       fbin<<(lasttime-1000);
       fbin.close();
     }
      break;
    case 'h':
    case 'H':
    case '?':
    default:            /* help */
      cout<<"$offmon(c)(PG) file  -m(M)onitor"<<endl;
      return 0;
      break;
    }
  }
  
  AMSNtupleR *pntuple=new AMSNtupleR();
  AMSChain chain(pntuple,"AMSRoot"); 
  pchain=&chain;

  if(!RichRingR::isCalibrating()) RichRingR::switchDynCalibration();

  if(filename){
    printf("opening file %s...\n", filename);
    OpenChain(chain,filename); 
  }
  int err=0;
  int argcc=1;
#if defined(WIN32) || defined(__APPLE__)
  Myapp *theApp = new Myapp("App", &argcc, argv);
#else
  //gVirtualX=new TGX11("X11","Root Interface to X11");
  //TGuiFactory *p=new TGuiFactory("root");

  TASImage a;
  Myapp *theApp = new Myapp("App", &argcc, argv);
  //  gDebug=6; 
  theApp->SetStatic();
#endif
  AMSOnDisplay * amd= new AMSOnDisplay("AMSRoot Offline Display",pntuple,&chain);
  theApp->SetDisplay(amd);  
  AMSAntiHist  antih("&AntiCounters","Anti counter Hists");
  amd->AddSubDet(antih);

  AMSTRDHist  trdh("&TRD","TRD Hists");
  amd->AddSubDet(trdh);
  AMSTOFHist  TOFh("TO&F","TOF Hists");
  amd->AddSubDet(TOFh);
  AMSTrackerHist  trackerh("T&racker","Tracker  Hists");
  amd->AddSubDet(trackerh);
  AMSRICHHist  richh("R&ICH","RICH  Hists");
  amd->AddSubDet(richh);
  AMSECALHist  ecalh("&ECAL","ECAL  Hists");
  amd->AddSubDet(ecalh);
  AMSLVL1Hist  LVL1h("Level&1","LVL1  Hists");
  amd->AddSubDet(LVL1h);
  AMSLVL3Hist  LVL3h("Level&3","LVL3  Hists");
  amd->AddSubDet(LVL3h);
  AMSAxAMSHist  AxAMSh("A&xAMS","AxAMS  Hists");
  amd->AddSubDet(AxAMSh);
  AMSGenHist  Genh("&General","Gen  Hists");
  amd->AddSubDet(Genh);
  AMSEverything  Everyh("E&verything","All Hists");
  amd->AddSubDet(Everyh);

  amd->SetApplication(theApp);
  amd->Init();
  amd->Begin()=0;
  amd->Sample()=999;
  theApp->SetIdleTimer(6,"");
  TVirtualHistPainter::SetPainter("THistPainter");
  for(;;){
    if(amd->Fill())amd->DispatchProcesses();
    theApp->Run();
  }        
  return 0;
  


}
//---------------------------------------




void (handler)(int sig){
  /*
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
  */
}

/*
void OpenChain(TChain & chain, char * filenam){
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
                dirent ** namelistsubdir;
                cout <<"  scanning "<<ts<<" "<<Selector<<" l "<<l<<" "<<i<<endl;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,NULL);
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
#endif
  chain.Add(filename);
}


*/

void OpenChain(AMSChain & chain, char * filenam){
  //  
  //  root can;t cope with multiple wild card so we must do it ourselves
  //
  char filename[255];
  if(!filenam || strlen(filenam)==0 || strlen(filenam)>sizeof(filename)-1){
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
#if !defined(WIN32) && !defined(__APPLE__) && !defined(__NOCASTOR__)
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
                int nptrdir=scandir(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#elif defined(__LINUXNEW__)
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&Sort));
#else
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
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
                int nptrdir=scandir(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
#elif defined(__LINUXNEW__)
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const dirent64**, const dirent64**)>(&Sort));
#else
                dirent64 ** namelistsubdir;
                int nptrdir=scandir64(ts.Data(),&namelistsubdir,Selectsdir,reinterpret_cast<int(*)(const void*, const void*)>(&Sort));
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
     if( (t-statbuf.st_mtime>6 || notlast || !monitor) && (strlen(lastfile)<2 || strcmp(lastfile,filename) )){     
      strcpy(lastfile,filename);
      cout <<" added "<<filename<<" "<<statbuf.st_size<<endl;
      if(add++<3000){
           lasttime=statbuf.st_mtime;
           chain.Add(filename);
           static int init =0;
           if(init==0){
            init=1;
            TObjString s("");
            TFile * rfile=TFile::Open(filename,"READ");
            if(rfile){
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
int Selectsdir(	 dirent *entry)
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
int Select(  dirent *entry)
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
int Sort(dirent ** e1,  dirent ** e2)
#else
int Sort(dirent64 ** e1,  dirent64 ** e2)
#endif
  {
return strcmp((*e1)->d_name,(*e2)->d_name);
}


#endif
