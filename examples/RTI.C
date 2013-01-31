///  This is an simple example to use AMS-RTI to Calculate Exposure-Time for Diff Rigidity(Helium)
// -----------------------------------------------------------
//        Created:       2013-Jan-26  Q.Yan  qyan@cern.ch (Example and Develop)
//                       Time Cut by V.Choutko
//        Run:           root -b -q RTI.C+
// -----------------------------------------------------------

#include <signal.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream.h>
#include <iostream>
#include "root_RVSP.h"
#include "amschain.h"
#include "Tofrec02_ihep.h"
#include "Tofcharge_ihep.h"
#include "HistoMan.h"

HistoMan hman1;//Histo Hman
void BookHistos();//Histo-Book
void ProcessROOT(AMSChain *ch,unsigned int time[2]);//Event Cal
void ProcessTime(unsigned int time[2]);//StandAlone Time Cal
int RTI_CINT(AMSChain *ch,char *outfile);


///--Main Function 
int RTI(){

   AMSChain a;
   a.Add("root://castorpublic.cern.ch///castor/cern.ch/ams/Data/AMS02/2011B/ISS.B620/pass4/1355244861.00000001.root");
   RTI_CINT(&a,"./RTI_Test.root");
   return 0;
}

//---------------------------------------------
int RTI_CINT(AMSChain *ch,char *outfile){ 

   hman1.Clear();
   hman1.Enable();
   hman1.Setname(outfile);

//--Book Hist
   BookHistos();
  
//--Process ROOT
   cout<<"Proces ROOT"<<endl;
   unsigned int time[2]={0};
   ProcessROOT(ch,time);

  cout<<"ROOT Time="<<time[0]<<" "<<time[1]<<endl;
//--Cal Exposure Time
   ProcessTime(time);
   

//--Hist Save
   hman1.Save();
   return 0;
}

///--Book Histo
//---------------------------------------------
void BookHistos(){

   float pmin=0.5,pmax=100;//Exposure Rigidity (0.5~100GV)
   const int bin=1000;//Rigidity Bin Number-Log
   double mon[bin+1];
   float de=(log(pmax)-log(pmin))/bin;
   for(int ibin=0;ibin<=bin;ibin++){
      mon[ibin]=log(pmin)+de*ibin;
      mon[ibin]=exp(mon[ibin]);
   }

   hman1.Add(new TH1D("ExpoTime", "ExpoTime",bin, mon));//Time
   hman1.Add(new TH1D("HeEvent",  "HeEvent", bin, mon));//Event
}


///--RTI Time PreSelection(Provided by VC)
//---------------------------------------------
bool TPreSelect(AMSSetupR::RTI &a){

  bool cut[10]={0};
  cut[0]=(a.ntrig/a.nev>0.98);
  cut[1]=(a.npart/a.ntrig>0.07/1600*a.ntrig&&a.npart/a.ntrig<0.25);
  cut[2]=(a.lf>0.5);
  cut[3]=(a.zenith<40);
  cut[3]=(a.nerr>=0&&a.nerr/a.nev<0.1);
  cut[4]=(a.npart>0&&a.nev<1800);
  bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]);
  return tcut;
}

///---Single Event Process
//--------------------------------------
bool PreEvent(AMSEventR *pev){

//----Process RTI
  AMSSetupR::RTI a;
  if(pev->GetRTI(a)!=0)return false;//not exist in RTI Table
  if(!TPreSelect(a))return false;//not good second Abandon

//----Process Event Select Helium Using Max-TkQ BetaH
   float mtkq=0; int ibetah=-1;
   for(int ibh=0;ibh<pev->nBetaH();ibh++){
     BetaHR *betah=pev->pBetaH(ibh);
     if(betah->iTrTrack()<0)continue;
     float ntkq=betah->pTrTrack()->GetQ();
     if(ntkq>mtkq){ibetah=ibh;mtkq=ntkq;}
   }
   if(ibetah<0)return false;

//----Z=2 Select By TOFHZ-Likelihood
   BetaHR *betah=pev->pBetaH(ibetah);
   TofChargeHR *tofhc=betah->pTofChargeH();
   int tofql;float tofpz;
   int tofz=tofhc->GetZ(tofql,tofpz);
   if(tofz!=2)return false;//Helium Select by TOF

//---RIT+Event Cutoff-Cut
   float rig=betah->pTrTrack()->GetRigidity();//no select for TkPattern
   if(rig<1.2*a.cf[3][1])return false; //1.2*Cutoff (Positive 40 degree)
 
//--Fill Above Cutoff Ev++
    hman1.Fill("HeEvent",rig);     
    return true;
}

//All Events-Loop
//--------------------------------------
void ProcessROOT(AMSChain *ch,unsigned int time[2]){

   Long64_t num2=ch->GetEntries();
   for(Long64_t entry=0; entry<num2; entry++){
      AMSEventR *ev=(AMSEventR *)ch->GetEvent();
      if(ev==NULL)continue;
      if(entry%100000==0) printf("Processed %7ld out of %7ld\n",entry,num2);
      PreEvent(ev);
   }
   time[0]=ch->GetEvent(0)->UTime();
   time[1]=ch->GetEvent(num2-1)->UTime();
  
}


//--Standalone Expo-Time Calculation
//--------------------------------------
void ProcessTime(unsigned int time[2]){

  TH1D* th=(TH1D *)hman1.Get("ExpoTime");
  for(unsigned int t=time[0];t<=time[1];t++){
//--RTI Selection
     AMSSetupR::RTI a;
     if(AMSEventR::GetRTI(a,t)!=0)continue; //Time no information
     if(!TPreSelect(a))continue;//Abandon Second
//--Time Cal
     double nt=a.lf*a.nev/(a.nev+a.nerr);
     double urig=1.2*a.cf[3][1];//1.2*Cutoff (Positive 40 degree)
//--Above Cutoff T+
     for(int ibr=1;ibr<=th->GetNbinsX();ibr++){//Above CutOff Time++
        if(th->GetBinCenter(ibr)>urig){
          th->AddBinContent(ibr,nt);
        }
     }
     th->SetEntries(th->GetEntries()+1);
   }

}