//  $Id: AMSLVL1Hist.cxx,v 1.34 2011/05/24 19:46:16 choumilo Exp $
//       v1.0/E.Choumilov/20.06.2003
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSLVL1Hist.h"

char Lvl1Pars::dat1[30];
char Lvl1Pars::dat2[30];
char Lvl1Pars::dat3[30];

void AMSLVL1Hist::Book(){

  AddSet("FT/LVL1 Members_Sharing");//0
  
  _filled.push_back(new TH1F("trigh0","Exclusive PhysBranches sharing",11,0.,11.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("trigh1","Lvl1InputBranches Sharing",16,0.,16.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("trigh2","Lvl1PhysBranches Sharing",8,0.,8.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
   

  AddSet("TOF-TotMultiplicity in LVL1");//1
  
  _filled.push_back(new TH1F("trigh3","TOF-layers per events, Z>=1(ftc)",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh4","TOF-layers per events, Z>=2(bz)",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh5","TOF-paddles per events, Z>=1(ftc)",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(3);

  _filled.push_back(new TH1F("trigh6","TOF-paddles per events, Z>=2(bz)",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  
  AddSet("TOF-patterns in LVL1");//2
  
  _filled.push_back(new TH1F("trigh7","TOF PlanesPatternCode, Z>=1(ftc)",12,-1.,11.));
  _filled[_filled.size()-1]->SetXTitle("0:4L,(1-4):missL,5:13,6:14,7:23,8:24,9:12,10:34");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh8","TOF-pads pattern in Z>=1(Side1,globFT)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(3);
   
  _filled.push_back(new TH1F("trigh9","TOF-pads pattern Z>=1(Side2,globFT)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(3);
   
  _filled.push_back(new TH1F("trigh10","TOF PlanesPatternCode, Z>=2(bz)",12,-1.,11.));
  _filled[_filled.size()-1]->SetXTitle("0:4L,(1-4):missL,5:13,6:14,7:23,8:24,9:12,10:34");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh11","TOF-pads pattern in Z>=2(Side1,globFT)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(4);
   
  _filled.push_back(new TH1F("trigh12","TOF-pads pattern in Z>=2(Side2,globFT)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(4);
   
  AddSet("ANTIpatterns in LVL1");//3
  
  _filled.push_back(new TH1F("trigh13","ANTI-sectors(logical) pattern(globFT)",8,1.,9.));
  _filled[_filled.size()-1]->SetXTitle("Sector number");
  _filled[_filled.size()-1]->SetFillColor(2);
  
  _filled.push_back(new TH1F("trigh14","Total Anti-sectors, Z>=1(ftc)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh15","Total Anti-sectors, Z>=2(bz)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh16","Total Anti-sectors, SlowZ>=2(ftz)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh17","Total Anti-sectors, EC&TOF(fte+tof)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh18","Total Anti-sectors, EC&!TOF(fte-tof)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  AddSet("ECAL in LVL1(1)");//4
  
  _filled.push_back(new TH1F("trigh19","FTE_Flag(when !=0, noTOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("noFTE(0)/FTE&NoPrBits(1)/FTE&OrBit(2)/FTE&AndBit(3)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh20","Lvl1(Angle)_Flag(when !=0, noTOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("undef(0)/FTE&NoPrBits(1)/FTE&OrBit(2)/FTE&AndBit(3)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh21","FTE_Flag(GlobFT-OK) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("noFTE(0)/FTE&NoPrBits(1)/FTE&OrBit(2)/FTE&AndBit(3)");
  _filled[_filled.size()-1]->SetFillColor(6);
  
  _filled.push_back(new TH1F("trigh22","FTE_Flag(when !=0, & TOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("noFTE(0)/FTE&NoPrBits(1)/FTE&OrBit(2)/FTE&AndBit(3)");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("trigh23","Lvl1(Angle)_Flag(when !=0, & TOF) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("undef(0)/FTE&NoPrBits(1)/FTE&OrBit(2)/FTE&AndBit(3)");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  AddSet("ECAL in LVL1(2)");//5
  
  _filled.push_back(new TH1F("trigh24","EcalTrigPatt(X-proj:Slayers 2,4,6)",120,1.,121.));
  _filled[_filled.size()-1]->SetXTitle("PhMultiplierNumber: Npm+(Sl/2-1)*40");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("trigh25","EcalTrigPatt(Y-proj:Slayers 3,5,7)",120,1.,121.));
  _filled[_filled.size()-1]->SetXTitle("PhMultiplierNumber: Npm+((Sl-1)/2-1)*40");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  
  AddSet("LiveTime in LVL1");//6
  
  _filled.push_back(new TProfile("trigh26","AMSDAQLiveTime vs Time",120,0,lev1trange[0],0,1.1));
  _filled[_filled.size()-1]->SetYTitle("LiveTime fraction");
  _filled.push_back(new TProfile("trigh27","AMSDAQLiveTime vs Time",120,0,lev1trange[1],0,1.1));
  _filled[_filled.size()-1]->SetYTitle("LiveTime fraction");
  _filled.push_back(new TProfile("trigh28","ThetaM vs Time",120,0,lev1trange[0],-3.1415926/2,3.1415926/2));
  _filled[_filled.size()-1]->SetYTitle("ThetaM (rad)");
  
  
  AddSet("TrigRates in LVL1");//7
  
  _filled.push_back(new TProfile("trigh29","FTtof(FTC)Trig-rate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("RATE (Hz)");
  _filled.push_back(new TProfile("trigh30","FTec(FTE)Trig-rate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("RATE (Hz)");
  _filled.push_back(new TProfile("trigh31","Lev1Trig-rate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("RATE (Hz)");
  _filled.push_back(new TProfile("trigh32","TofPlaneSide-MaxRate vs Time",120,0,lev1trange[0],0,10000.));
  _filled[_filled.size()-1]->SetYTitle("RATE (Hz)");
  
  
  AddSet("EventTimeDiff in LVL1");//8
  
  _filled.push_back(new TH1F("trigh33","ConsecEvents TrigTimeDiff",200,0,1000.));
  _filled[_filled.size()-1]->SetXTitle("TrigTimeDiff(Mksec)");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled.push_back(new TH1F("trigh34","ConsecEvents TrigTimeDiff",2000,0,25.));
  _filled[_filled.size()-1]->SetXTitle("TrigTimeDiff(Msec)");
  _filled[_filled.size()-1]->SetYTitle("Number of events");
  _filled[_filled.size()-1]->SetFillColor(3);


AddSet("ExtBits in LVL1");//9

  _filled.push_back(new TH1F("trigh35","Number of ACC-sectors, IonTrig(incl)",9,0.,9.));
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled.push_back(new TH1F("trigh36","Number of ACC-sectors, IonTrig(excl)",9,0.,9.));
  _filled[_filled.size()-1]->SetFillColor(3);
}


void AMSLVL1Hist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  TAxis *xax;
  char name[60],dat[30];
//

  switch(Set){
case 0:
  gPad->Divide(1,3);
  gPad->cd(1);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[0]->GetXaxis();
  xax->SetLabelSize(0.1);
  xax->SetBinLabel(1,"Entries");
  xax->SetBinLabel(2,"Lvl1");
  xax->SetBinLabel(3,"unbTof");
  xax->SetBinLabel(4,"Z>=1");
  xax->SetBinLabel(5,"Ion");
  xax->SetBinLabel(6,"SlowIon");
  xax->SetBinLabel(7,"el/pos");
  xax->SetBinLabel(8,"Photon");
  xax->SetBinLabel(9,"UnbEcal");
  xax->SetBinLabel(10,"restoredUnbTof");
  xax->SetBinLabel(11,"restoredUnbEc");
  _filled[0]->SetStats(kFALSE);
  _filled[0]->Draw("hbar2");//FTMembers sharing  
  gPadSave->cd();
//  
  gPad->cd(2);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[1]->GetXaxis();
  xax->SetLabelSize(0.08);
  xax->SetBinLabel(1,"Z>=1(ftc)");
  xax->SetBinLabel(2,"Lut1(ftcp0)");
  xax->SetBinLabel(3,"Lut2(ftcp1)");
  xax->SetBinLabel(4,"ftct0");
  xax->SetBinLabel(5,"ftct1");
  xax->SetBinLabel(6,"SlowZ>=2(ftz)");
  xax->SetBinLabel(7,"EcFt(fte)");
  xax->SetBinLabel(8,"NAntiS<=1(acco)");
  xax->SetBinLabel(9,"NAntiS=0(acc1)");
  xax->SetBinLabel(10,"Z>=2(bz)");
  xax->SetBinLabel(11,"EcMultAnd");
  xax->SetBinLabel(12,"EcMultOr");
  xax->SetBinLabel(13,"EcAngleAnd");
  xax->SetBinLabel(14,"EcAngleOr");
  xax->SetBinLabel(15,"ExtGate0");
  xax->SetBinLabel(16,"ExtGate1");
  _filled[1]->SetStats(kFALSE);
  _filled[1]->Draw("hbar2");//BasisTrigMembers sharing  
  gPadSave->cd();
//  
  gPad->cd(3);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[2]->GetXaxis();
  xax->SetLabelSize(0.1);
  xax->SetBinLabel(1,"UnbiasedTof");
  xax->SetBinLabel(2,"Z>=1(ftc+anti)");
  xax->SetBinLabel(3,"Z>=2(bz)");
  xax->SetBinLabel(4,"SlowZ>=2(ftz)");
  xax->SetBinLabel(5,"El(ftc+fte)");
  xax->SetBinLabel(6,"Ph(fte+EcAng)");
  xax->SetBinLabel(7,"UnbEc(fte)");
  xax->SetBinLabel(8,"Ext");
  _filled[2]->SetStats(kFALSE);
  _filled[2]->Draw("hbar2");//Lvl1PhysBranches sharing  
  gPadSave->cd();
//
  break;
case 1:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+3]->Draw();//TOF-TotMultiplicity
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+7]->Draw();//TOF-patterns
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+13]->Draw();//ANTIpatterns
    gPadSave->cd();
  }
  break;

case 4:
  gPad->Divide(3,2);
  for(i=0;i<5;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+19]->Draw();//ECAL(1) 
    gPadSave->cd();
  }
  break;
case 5:
  gPad->Divide(1,2);
  for(i=0;i<2;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+24]->Draw();//ECAL(2) 
    gPadSave->cd();
  }
  break;
case 6:
  gPad->Divide(1,3);
  for(i=0;i<3;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i<2){
    _filled[i+26]->SetMinimum(0);
    _filled[i+26]->SetMaximum(1.05);
    }
   else{
    _filled[i+26]->SetMinimum(-1.5);
    _filled[i+26]->SetMaximum(1.5);
   }
    _filled[i+26]->SetMarkerStyle(20);
    _filled[i+26]->SetMarkerColor(2);
    _filled[i+26]->SetMarkerSize(0.5);
    if(i==0){
      strcpy(name,"Last 120mins since ");
      strcpy(dat,Lvl1Pars::getdat1());
    }
    if(i==1){
      strcpy(name,"Last 60hours since ");
      strcpy(dat,Lvl1Pars::getdat2());
    }
    if(i==2){
      strcpy(name,"Last 120mins since ");
      strcpy(dat,Lvl1Pars::getdat3());
    }
    strcat(name,dat);
    xax=_filled[i+26]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    _filled[i+26]->Draw("P");//thetaM
    gPadSave->cd();
  }
  break;
case 7:
  gPad->Divide(1,4);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
//    _filled[i+26]->SetMinimum(0);
//    _filled[i+26]->SetMaximum(1.05);
    _filled[i+29]->SetMarkerStyle(20);
    _filled[i+29]->SetMarkerColor(2);
    _filled[i+29]->SetMarkerSize(0.5);
    strcpy(name,"Last 120mins since ");
    strcpy(dat,Lvl1Pars::getdat1());
    strcat(name,dat);
    xax=_filled[i+29]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    _filled[i+29]->Draw("P");//Rates 
    gPadSave->cd();
  }
  break;
case 8:
  gPad->Divide(1,2);
  for(i=0;i<2;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100110);
    gStyle->SetOptFit(1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i==0){
//       cout <<" fit "<<endl;
       char func[]="expo";
#if defined (__ICC)
#else
       gStyle->SetOptFit(1);
       ((TH1F*)_filled[i+33])->Fit(func,"","",150.,1000.);
#endif
     }
    else if(i==1){
//       cout <<" fit "<<endl;
       char func[]="expo";
#if defined (__ICC)
#else
       gStyle->SetOptFit(1);
       ((TH1F*)_filled[i+33])->Fit(func,"","",0.2,10.);
#endif
     }
    _filled[i+33]->Draw();//ConcecEvents TrigTimeDiff 
    gPadSave->cd();
  }
  break;
case 9:
  gPad->Divide(1,2);
  for(int i=0;i<2;i++){
    gStyle->SetOptStat(10);
    gPad->cd(i+1);
    _filled[i+35]->Draw();//Nacc z>1 incl/excl 
    gPadSave->cd();
  }
  break;
  }
//
}



void AMSLVL1Hist::Fill(AMSNtupleR *ntuple){
  int lmax(4);//tot tof-layers
  int padpl[4]={8,8,10,8};//tof-paddles per layer
  int tofflg1(-1),tofflg2(-1),tofpat1[4],tofpat2[4],npds1(0),npds2(0);
  int ecflg(-1),etflg(0),ewflg(0);
  int physbpat(0),membpat(0);
  int mflg(0),ntofl(0),ntofp(0),mlcode1(-1),mlcode2(-1),tgmode,lsbt;
  float ltime(1);
  int etime[2],evnum,runum;
  char date[30];
  bool ftc(0),ftz(0),bz(0),fte(0),glft(0);
  static float range[3],timez[3];
  static int first(1),etime0(0),evnloc;
  float time[3];
  float rates[19];
  Double_t trigt;
  Float_t trigtdif;
  static Double_t trigtprev(-1);



  
  UShort_t ecpatt[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//
//  cout<<"Start evnum:"<<evnloc<<endl;
  _filled[0]->Fill(float(0),1.);//entries
//
  etime[0]=ntuple->fHeader.Time[0];//unix-time(sec)
  etime[1]=ntuple->fHeader.Time[1];
  evnum=ntuple->fHeader.Event;
  runum=ntuple->fHeader.Run;
  strcpy(date,ntuple->Time());
//
  if(first==1){
    first=0;
    etime0=etime[0];
    cout<<"LVL1: 1st event Run/event:"<<runum<<" "<<evnum<<" date:"<<date<<" evloc="<<evnloc<<endl;
    for(int i=0;i<3;i++){
      timez[i]=0;
    }
    evnloc=0;
    Lvl1Pars::setdat1(ntuple->Time());
    Lvl1Pars::setdat2(ntuple->Time());
    Lvl1Pars::setdat3(ntuple->Time());
  }
  time[0]=(etime[0]-etime0)/60;//ev.time starting from beg.of.run(min)
  time[1]=(etime[0]-etime0)/3600;//ev.time starting from beg.of.run(hour)
  time[2]=(etime[0]-etime0)/86400;//ev.time starting from beg.of.run(day)
//
  if(ntuple->nLevel1()){
    _filled[0]->Fill(1,1.);//lvl1_ok
    ecflg=ntuple->pLevel1(0)->EcalFlag;
    etflg=ecflg/10;//ECEtot(FTE=multiplicity) flag
    ewflg=ecflg%10;//ECAngle(width) flag
//
    tofflg1=ntuple->pLevel1(0)->TofFlag1;//for z>=1(ftc)
    tofflg2=ntuple->pLevel1(0)->TofFlag2;//for z>=2(bz)
    membpat=ntuple->pLevel1(0)->JMembPatt;
    physbpat=ntuple->pLevel1(0)->PhysBPatt;

    ftc=((membpat&1)>0);//Z>=1
    ftz=((membpat&1<<5)>0);//SlowZ>=2
    fte=((membpat&1<<6)>0);//ec-FT
    bz= ((membpat&1<<9)>0);//Z>=2(when ftc)
    glft=(ftc||ftz||fte);//globFT
//---
    for(int i=0;i<7;i++){//<--- lvl1 phys.branches(exclusive)
      lsbt=1<<i;
      if((physbpat&127)==lsbt)_filled[0]->Fill(i+2,1.);
    }
    if((physbpat&127)==0){//suspect that was unbtrig case, try restore which one
      if((membpat&1)>0)_filled[0]->Fill(7+2,1.);//was ftc in jmembpatt
      if((membpat&64)>0)_filled[0]->Fill(8+2,1.);//was fte
    }
//---
    for(int i=0;i<16;i++){//<-- basis trig-members 
      lsbt=1<<i;
      if(membpat&lsbt)_filled[1]->Fill(i,1.);
    }
//---
    for(int i=0;i<8;i++){//<--- lvl1 phys.branches
      lsbt=1<<i;
      if((physbpat&lsbt)>0)_filled[2]->Fill(i,1.);
    }
//---
//                                       <------ TOF
    ntofl=0;
    mlcode1=tofflg1;//miss layers code(ftc)
    if(mlcode1>=0){
      if(mlcode1==0)ntofl=4;
      else if(mlcode1<5)ntofl=3;
      else ntofl=2;
      _filled[3]->Fill(ntofl,1.);//tot. tof-layers(z>=1)
    }
    ntofl=0;
    mlcode2=tofflg2;//miss layers code(bz)
    if(mlcode2>=0 && bz){//tofflg2 shows input patt., so it can be >=0 even when bz=0
      if(mlcode2==0)ntofl=4;
      else if(mlcode2<5)ntofl=3;
      else ntofl=2;
      _filled[4]->Fill(ntofl,1.);//....(z>=2)
    }
//
    npds1=0;
    npds2=0;
    for(int i=0;i<lmax;i++)tofpat1[i]=ntuple->pLevel1(0)->TofPatt1[i];
    for(int i=0;i<lmax;i++)tofpat2[i]=ntuple->pLevel1(0)->TofPatt2[i];
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        if((tofpat1[il]&1<<ip)>0 || (tofpat1[il]&1<<(16+ip))>0)npds1+=1;  
        if((tofpat2[il]&1<<ip)>0 || (tofpat2[il]&1<<(16+ip))>0)npds2+=1;
      }
    }
    if(ftc)_filled[5]->Fill(npds1,1.);//tot. tof-paddles(ORed sides)
    if(bz)_filled[6]->Fill(npds2,1.);
//
    if(ftc)_filled[7]->Fill(mlcode1,1.);//miss.layer code
    if(bz)_filled[10]->Fill(mlcode2,1.);
//
    int nch;
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        nch=il*10+ip+1;
        if((tofpat1[il]&(1<<ip))>0)_filled[8]->Fill(nch,1.);//s1-patt, z>=1
	if((tofpat1[il]&(1<<(16+ip)))>0)_filled[9]->Fill(nch,1.);//s2-patt, z>=1
	if(bz){  
          if((tofpat2[il]&(1<<ip))>0)_filled[11]->Fill(nch,1.);//s1-patt, Z>=2
	  if((tofpat2[il]&(1<<(16+ip)))>0)_filled[12]->Fill(nch,1.);//s2-patt, Z>=2
	}  
      }
    }
//----                                       
//                                       <------ ANTI
    int asmax(8),nast(0);
    int antipat=ntuple->pLevel1(0)->AntiPatt;
    for(int is=0;is<asmax;is++){
      if((antipat & 1<<is)>0)nast+=1;
      if((antipat & 1<<is)>0)_filled[13]->Fill(is+1,1.);//sectors-patt(globFT)
    }
    if(ftc)_filled[14]->Fill(nast,1.);//tot sectors, when Z>=1(ftc)
    if(bz)_filled[15]->Fill(nast,1.);//             ...., when Z>=2(bz)
    if(ftz)_filled[16]->Fill(nast,1.);//             ...., when slowZ>=2
    if(fte && (ftc||ftz))_filled[17]->Fill(nast,1.);// ...., when EC & TOF
    if(fte && !(ftc||ftz))_filled[18]->Fill(nast,1.);// ...., when EC & !TOF("gammas")
//----    
//                                       <------ ECAL
    for(int i=0;i<6;i++){
      for(int j=0;j<3;j++){
        ecpatt[i][j]=ntuple->pLevel1(0)->EcalPatt[i][j];
      }
    }
    if(fte){
      if(!(ftc||ftz)){//EC-drived FT
        _filled[19]->Fill(etflg,1.);//Etot(fte=mult)-flag
        _filled[20]->Fill(ewflg,1.);//ShWidth-flag
      }
      else{//TOF-driven FT
        _filled[22]->Fill(etflg,1.);//Etot(fte=mult)-flag
        _filled[23]->Fill(ewflg,1.);//ShWidth-flag
      }
    }
    if(glft)_filled[21]->Fill(etflg,1.);//Etot(fte=mult)-flag
    if(fte){
      int word,bit,sset;
      for(int sl=0;sl<6;sl+=2){
        for(int pm=0;pm<36;pm++){
          word=pm/16;
          bit=pm%16;
          sset=sl/2;
          if((ecpatt[sl][word]&(1<<bit))>0)_filled[24]->Fill(float(pm+1+40*sset),1.);//x-pr
        }
      }
      for(int sl=1;sl<6;sl+=2){
        for(int pm=0;pm<36;pm++){
          word=pm/16;
          bit=pm%16;
          sset=(sl-1)/2;
          if((ecpatt[sl][word]&(1<<bit))>0)_filled[25]->Fill(float(pm+1+40*sset),1.);//y-pr
        }
      }
    }
//----
//                                       <------ LiveTime, Rates:
    ltime=ntuple->pLevel1(0)->LiveTime;
    for(int i=0;i<19;i++)rates[i]=ntuple->pLevel1(0)->TrigRates[i];
//    if(evnloc%100==0){
//      cout<<"Event/date="<<evnum<<" "<<date<<endl;
//      cout<<"time="<<time<<" ltime="<<ltime<<endl;
//    }
//
    if((time[0]-timez[0])>=lev1trange[0]){
      ((TProfile*)_filled[26])->Reset("");
      ((TProfile*)_filled[29])->Reset("");
      ((TProfile*)_filled[30])->Reset("");
      ((TProfile*)_filled[31])->Reset("");
      ((TProfile*)_filled[32])->Reset("");
      timez[0]=time[0];
      Lvl1Pars::setdat1(ntuple->Time());
    }
    ((TProfile*)_filled[26])->Fill(time[0]-timez[0],ltime,1.);
    ((TProfile*)_filled[29])->Fill(time[0]-timez[0],rates[1],1.);//FTC
    ((TProfile*)_filled[30])->Fill(time[0]-timez[0],rates[3],1.);//FTE
    ((TProfile*)_filled[31])->Fill(time[0]-timez[0],rates[5],1.);//lvl1
    ((TProfile*)_filled[32])->Fill(time[0]-timez[0],rates[14],1.);//TofPlaneSide max
//
    if((time[1]-timez[1])>=lev1trange[1]){
      ((TProfile*)_filled[27])->Reset("");
      timez[1]=time[1];
      Lvl1Pars::setdat2(ntuple->Time());
    }
    ((TProfile*)_filled[27])->Fill(time[1]-timez[1],ltime,1.);
//
    if((time[2]-timez[2])>=lev1trange[2]){
      ((TProfile*)_filled[28])->Reset("");
      timez[2]=time[2];
      Lvl1Pars::setdat3(ntuple->Time());
    }
    ((TProfile*)_filled[28])->Fill(time[0]-timez[0],ntuple->fHeader.ThetaM,1.);
    
//---- trig-time histogr: 
 UInt_t itrtime[5];

    for(int i=0;i<5;i++)itrtime[i]=ntuple->pLevel1(0)->TrigTime[i];
    trigt=itrtime[2]*0.64+itrtime[3]*pow(2.,32)*0.64+1000000.*itrtime[1] ;//mksec
      trigtdif=Float_t(itrtime[4]);
      _filled[33]->Fill(trigtdif,1.);//TrigTimeDiff of 2 consecutive events (mksec)
      _filled[34]->Fill(trigtdif/1000.,1.);//...............................(msec)
      
//---- aux. ACC hist:
    if((physbpat&4)>0)_filled[35]->Fill(nast,1.);//Nacc phys.tr= Z>=2(incl)
    if((physbpat&127)==4)_filled[36]->Fill(nast,1.);//................(excl)
//
  }
  evnloc+=1; 
}


