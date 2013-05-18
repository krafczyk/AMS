#include "MVAtest.h"
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

/** test class to do MVA optimization
 *  using MVAmanager. Have three steps:
 *  -store selected samples from AMSChain to custom tree
 *  -train with MVA using parameters defined in config
 *  -use the produced MVA templates for analysis while looping on AMSchain
 *   or custom tree
 *
 *   Config:
 *   bmva -use MVA for training or analysis
 *   bmvatrain -MVA training
 *   bmvaana - MVA analysis
 *   bamstree -use AMSchain for analysis
 *   binitouttree - create a new custom tree to be store
 *   bloadouttree - load existing custom tree
 *   bstoreouttree  - store outtree for further training
 *   bstoreouthisto - store out histos
 *
 *
 *  To run:
 * >mainmvatest config.cfg
 *
 *see https://twiki.cern.ch/twiki/bin/view/AMS/MVAmanager
 *
 *  04.2013 -VZ
*/

using namespace std;


// main test

int  main(int argc, char **argv)
{

  cout<<"----main  cfg= "<<argv[1]<<endl;
 TStopwatch *timer_tot = new TStopwatch();
 timer_tot->Start();
  int Debug=1;
  string configfile=argv[1]; // general config file 
  ConfigParser* conf=new ConfigParser(configfile);
  Debug=conf->Value("general","Debug");
 bool bmva=conf->Value("general","bmva")>0;
 bool bmvatrain=conf->Value("general","bmvatrain")>0;  // train stored outtree
 bool bmvaana=conf->Value("general","bmvaana")>0;  // analayze AMS(1) or Out outtree(2)
 bool bamstree=conf->Value("general","bamstree")>0;  
 bool bloadouttree=conf->Value("general","bloadouttree")>0;
 bool bstoreouttree=conf->Value("general","bstoreouttree")>0;
 bool bstoreouthisto=conf->Value("general","bstoreouthisto")>0;
 int ntoanalyze=conf->Value("general","nevents");


 AMSChain* run_tree=0;
 int ndata=-1;
 if(bamstree) {
run_tree = new AMSChain(); 
string filelist=conf->Value("general","filelist");
ifstream infile (filelist.c_str());
 if(!infile)  {
    cout<<" *********cannot open  filelist "<<filelist<<endl;;
  }
string sfile;
while(infile>>sfile) {
   run_tree->Add(sfile.c_str()); 
   if(Debug>2) cout<<" added= "<<sfile<<endl;
   }

ndata = run_tree->GetEntries();
  if(Debug) cout<<" read ndata "<<ndata<<endl;
 }// bamstree


 MVAtest* mvatest=new MVAtest(conf); 
 int ntotev=0;
 // have 3 possibbilities
 // - produce  OutTree file, which can be further used for training 
 // - train already stored OutTree files and store MVA weights
 // - analyse either AMSevent or OutTree events with MBA weights 

 // if training
 // the list of alreay produced OutTree files should be defined
 // in config fnames=..  and fnameb=...
 // can be the same files, but signal and background is selected with cuts
 // defined in sigcutsfile=.. and bkgcutsfile=..
 // or different preselected files can be used, then cuts can be empty
 if(bmvatrain) {
   mvatest->trainoutree();
 } 

 //use custom outtree for analysis
 //if bmvaana, use produced MVA weights 
 // all MVA settings and files locations  are in config
if(bloadouttree&bmvaana) {
 // custom tree event loop 
   if(Debug) cout<<" ===start outtree loop "<<endl;
 while (mvatest->fTreeMan->NextEvent()) {
if(ntoanalyze>0&&ntotev>ntoanalyze) break;
double mvaout= mvatest->getmva();
// do things
// for example store histo
 if(mvatest->fHistoMan) {
 mvatest->fHistoMan->FillH1("hMVA_a", mvaout);
if( mvatest->fTreeMan->GetTreePar("mcfragm")==0) {
 mvatest->fHistoMan->FillH1("hMVA_s", mvaout);
 }
 if( mvatest->fTreeMan->GetTreePar("mcfragm")>0) {
 mvatest->fHistoMan->FillH1("hMVA_b", mvaout);
   }
 }//histoman

 //cout<<ntotev<<" mvaout= "<<mvaout<<endl;
 ntotev++;
   }// outtree evloop
 }//bmvaana

//can use AMSroot tree to produce custom files for durther trainign
// or for analysis, using existing MVA weights
  if(bamstree) {
    // ams tree use
 AMSEventR* pev = NULL;
 if(ntoanalyze<0) ntoanalyze=ndata;
  cout<<" ====>will analyze "<<ntoanalyze<<endl;
   // start loop
  //mvatest->exptime0=run_tree->GetEvent(0)->UTime();
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

// if MVA  analysis
// populate intermidiate outtree branches 
 if(bmvaana) {
   // get MVA out, fill tree 'onfly'
   double mvaout= mvatest->getmva(pev, ntotev);
   if(Debug)  cout<<iev<<"== mvaout"<<mvaout<<endl;
//do things here, eg fill histo
//...
 if(mvatest->fHistoMan) {
 mvatest->fHistoMan->FillH1("hMVA_a", mvaout);
if( mvatest->fTreeMan->GetTreePar("mcfragm")==0) {
 mvatest->fHistoMan->FillH1("hMVA_s", mvaout);
 }
 if( mvatest->fTreeMan->GetTreePar("mcfragm")>0) {
 mvatest->fHistoMan->FillH1("hMVA_b", mvaout);
 }
 }//histoman
 //...
 } //bmvaana

 // if bstoreouttree, just produce a new outtree
 if(bstoreouttree) {
   // fill outtree from AMSevent
   if(mvatest->filloutevent(pev, ntotev)==0)  mvatest->fTreeMan->FillTree();
 }
 ntotev++;
 if(Debug) cout<<"==process  event "<<iev<<endl;
      }// ams iev
  }//bamstree

  if(bstoreouthisto&&mvatest->fHistoMan>0) {
string fnameh=conf->Value("general","outfileh");
 mvatest->fHistoMan->WriteAll(fnameh,"Histo");
  }

 if(bstoreouttree) {
   mvatest->fTreeMan->WriteTree();
 }

 cout<<" ====end "<<ntotev<<" "<<timer_tot->CpuTime()<<endl;
 return 0;
}



