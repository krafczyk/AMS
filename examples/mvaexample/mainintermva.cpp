#include "InterMVA.h"
#include "ConfigParser.h"
#include "MVAmanager.h"
#include "OutTreeManager.h"
#include "OutHistoManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <bitset>
#include <sys/resource.h>
#include <omp.h>
#include <dirent.h>


#include <root.h>
#include <amschain.h>
#include "TrdSCalib.h"
#include "TrdHCalib.h"
#include "TrTrackSelection.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "DynAlignment.h"
#include "TrdKCluster.h"
#include "point.h"

#include "TrdQt.hh"
#include "TrkLH.h"

#include "TTree.h"
#include "TFrame.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"


using namespace std;


// main test

int  main(int argc, char **argv)
{
  
  cout<<"----main  cfg= "<<argv[1]<<endl;
 TStopwatch *timer_tot = new TStopwatch();
 timer_tot->Start();
 //
  string configfile=argv[1]; // general config file 
  ConfigParser* conf=new ConfigParser(configfile);
  int Debug=conf->Value("general","Debug");
  int ntoanalyze=conf->Value("general","nevents");
  // declare reader
  InterMVA* fimva=new InterMVA(configfile);

  char infnam[192];
  char outfnam[192];
  char rootdir[128];
  char rootfile[64];
  char listname[64];
  char scdbname[128];
  strcpy(rootdir,  argv[2]);
  strcpy(rootfile, argv[3]);
  strcpy(infnam,rootdir);
  strcat(infnam,rootfile);
  strcat(infnam,".root");
  cout<<" add AMS tree= "<<infnam<<endl;
  AMSChain* run_tree = new AMSChain(); 
  run_tree->Add(infnam); 
 int ndata = run_tree->GetEntries();
AMSEventR* pev = NULL;
 
  int ntotev=0;
 if(ntoanalyze<0) ntoanalyze=ndata;

 OutHistoManager* fHistoMan=new OutHistoManager();
{
string name="hMVA_a";
TH1D* h=new TH1D(name.c_str(),name.c_str(),100,-1.,1);
h->Sumw2();
fHistoMan->AddH1(h);
 }

  cout<<" ====>will analyze "<<ntoanalyze<<endl;
for ( int iev = 0; iev < ndata; iev++ ) {
if(Debug) cout<<" -----------------analyze iev= "<<iev<<endl;
 if (!(iev%1000)) {
   cout<<"  event "<<iev<<"  cputime="<<timer_tot->CpuTime()<<endl;
 timer_tot->Continue();
 }
if(ntoanalyze>0&&ntotev>ntoanalyze) break;
pev = run_tree->GetEvent(iev);
if(!pev) continue;
 if(pev->Status(30)) continue;
double mvaout= fimva->GetMVAOut(pev);
 if(Debug) cout<<iev<<" mvaout="<<mvaout<<endl;
fHistoMan->FillH1("hMVA_a", mvaout);
 ntotev++;
}

 string fnameh=conf->Value("general","outfileh");
fHistoMan->WriteAll(fnameh,"Histo");

 cout<<" ====end "<<ntotev<<" "<<timer_tot->CpuTime()<<endl;
  
 return 0;
}



