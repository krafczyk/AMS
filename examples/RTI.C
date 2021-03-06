///  This is an simple example to use AMS-RTI 
// -----------------------------------------------------------
//        Created:       2013-Jan-26  Q.Yan  qyan@cern.ch (Example)
//                       Time Cut by V.Choutko
//        Run:           root -b -q RTI.C+
//        Update:        2013-Dec-21   Q.Yan  RTI::Version Using Guide 
// -----------------------------------------------------------

#include <signal.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream>
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
int RTI_CINT(AMSChain *ch,char *outfile,int Tcalopt);


///--Main Function 
int RTI(){

   AMSChain a;
   a.Add("root://castorpublic.cern.ch///castor/cern.ch/ams/Data/AMS02/2011B/ISS.B620/pass4/1355240756.00000001.root");
   a.Add("root://castorpublic.cern.ch///castor/cern.ch/ams/Data/AMS02/2011B/ISS.B620/pass4/1355242081.00000001.root");
   int Tcalopt=1;//0 by Time; 1 by Process RunList; 2 by Run Id
   RTI_CINT(&a,"./RTI_Test.root",Tcalopt);
   return 0;
}

//---------------------------------------------
int RTI_CINT(AMSChain *ch,char *outfile,int Tcalopt){ 

   hman1.Clear();
   hman1.Enable();
   hman1.Setname(outfile);

//--Book Hist
   BookHistos();
 
//--Using Latest pass4(or pss6) RTI
//-------------------------
   int pass=4;//(pass4)  
//   int pass=6;//(pass6)
   AMSSetupR::RTI::UseLatest(pass);//pass4 or pass6 
//--Process ROOT Events Counting
   cout<<"Proces ROOT"<<endl;
   unsigned int time[2]={0};
   ProcessROOT(ch,time);

//--Time Calculation
  switch(Tcalopt){

   case 0 : //---Calculate Time method 1  //by Begin and End Time
     cout<<" Time="<<time[0]<<" "<<time[1]<<endl;
     ProcessTime(time);
     break;

   case 1:  //---Calculate Time method 2  //by Processed RunList from Recording AMSEventR:fRunList
     for(map<unsigned int,AMSSetupR::RunI>::iterator it=AMSEventR::fRunList.begin();it!=AMSEventR::fRunList.end();it++){//Get Run by Run
       unsigned int run=it->first;//run id
       AMSSetupR::RunI runi=it->second; //run information
       unsigned int rtime[2];//run begin and end time
       AMSEventR::GetRTIRunTime(run,rtime);//Find Run begin and end time by RTI database
       cout<<"Process Run="<<run<<"  Time="<<rtime[0]<<"~"<<rtime[1]<<endl;//can first write runid to txtfile, then use case 2 to process
       for(int k=0;k<runi.fname.size();k++){
          cout<<runi.fname[k]<<endl;//Print root file
      }
      ProcessTime(rtime);//calculate time by Run begin and end time 
    }
    break;

   case 2:  //---Calculate Time method 3 //by Run Id
      const int nrun=2;
      unsigned int runlist[nrun]={//run id
        1355240756,
        1355242081,
      };
     for(int ir=0;ir<nrun;ir++){
       unsigned int run=runlist[ir];//run id
       unsigned int rtime[2];//run begin and end time
       AMSEventR::GetRTIRunTime(run,rtime); //Find Run begin and end time by RTI database
       cout<<"Run="<<run<<"  Time="<<rtime[0]<<"~"<<rtime[1]<<endl;
       ProcessTime(rtime);//calculate time by Run begin and end time
     }
     break;
  } 

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


///--RTI Time Selection(Provided by VC + QY)
//---------------------------------------------
bool TPreSelect(AMSSetupR::RTI &a){

  bool cut[10]={0};
  cut[0]=(a.ntrig/a.nev>0.98);
  cut[1]=(a.npart/a.ntrig>0.07/1600*a.ntrig&&a.npart/a.ntrig<0.25);
  cut[2]=(a.lf>0.5);
  cut[3]=(a.zenith<40);
  cut[4]=(a.nerr>=0&&a.nerr/a.nev<0.1);
  cut[5]=(a.npart>0&&a.nev<1800);
  cut[6]=((a.good&0x3F)==0);
  bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]&&cut[5]&&cut[6]);
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
//--Find Cutoff
   double ucutr=1.2*a.cfi[3][1];
//---Search Bin LowEdge>=Cutoff as Cutoff(much higher)
   TH1D* th=(TH1D *)hman1.Get("HeEvent"); 
   for(int ibr=1;ibr<=th->GetNbinsX();ibr++){
       if(th->GetBinLowEdge(ibr)>=ucutr){ucutr=th->GetBinLowEdge(ibr);break;}
   }
   if(rig<ucutr)return false;//> Bin  Cutoff
//--Fill Above Cutoff Ev++
    th->Fill(rig);
    return true;
}

//All Events-Loop
//--------------------------------------
void ProcessROOT(AMSChain *ch,unsigned int time[2]){

   Long64_t num2=ch->GetEntries();
   AMSEventR::fRunList.clear();//Clear AMSEventR:fRunList
   for(Long64_t entry=0; entry<num2; entry++){
      AMSEventR *ev=(AMSEventR *)ch->GetEvent();
      if(ev==NULL)continue;
      if(entry%100000==0) printf("Processed %7ld out of %7ld\n",entry,num2);
      PreEvent(ev);
      ev->RecordRTIRun();//Record Processing Run to AMSEventR:fRunList
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
     double ucutr=1.2*a.cfi[3][1];//IGRF Cutoff  Recommended
//--Above Cutoff LowBin>=Cutof T+ 
     for(int ibr=1;ibr<=th->GetNbinsX();ibr++){//Above CutOff Time++
        if(th->GetBinLowEdge(ibr)>=ucutr){
          th->AddBinContent(ibr,nt);
        }
     }
     th->SetEntries(th->GetEntries()+1);
   }

}
