//  $Id: AMSAntiHist.cxx,v 1.17 2013/05/11 11:52:08 mduranti Exp $
// By V. Choutko & D. Casadei
// Last changes: 27 Feb 1998 by D.C.
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSAntiHist.h"

const Int_t kNants=8;//ANTI sectors
const Float_t kTDCbin=0.024414;//TDC-bin(ns)
const Int_t accscale=1;//(1-3)scale for time-evolutions
char AntiPars::dat1[30];
char AntiPars::dat2[30];
char AntiPars::dat3[30];
Int_t AntiPars::evcount[20];
const Int_t hbias[7]={0,0,0,0,19,27,31};//bias for set-i(0,1,...)
//
Bool_t AntiPars::patbcheck(int sect, int patt){
// sect=0-7
    return (sect>=0 && sect<8 && (patt&(1<<sect)) != 0);
}


void AMSAntiHist::Book(){
//
   char title[128];
   char hname[128];
//-------------
  AddSet("Anti-Visibility");//set-0
  
  _filled.push_back(new TH1F("antih0","NAntiSectors/event(when found)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total fired sectors");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih1","NAntiSectors/event(FTcoincEvent)",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total fired sectors");
  _filled[_filled.size()-1]->SetFillColor(4);
  
  _filled.push_back(new TH1F("antih2","AntiSectorsVisibility",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");

  _filled.push_back(new TH1F("antih3","AntiSectorsVisibility(FTcoincEvent)",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");

  _filled.push_back(new TH1F("antih4","AntiSectorsVisibility(FTcoinc+PairedEvents)",30,0.,30.));
  _filled[_filled.size()-1]->SetXTitle("AnySideConfig                   S2-miss                                 S1-miss                                 ");
  _filled[_filled.size()-1]->SetFillColor(3);
  _filled[_filled.size()-1]->SetTitleSize(0.05,"X");
//-----------
//  

  AddSet("Anti-TimeHitsMultipl");//set-1
  
  _filled.push_back(new TH1F("antih5","Sectors 1-4 TimeHitsMult(FTcoincEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect1                 Sect2                   Sect3                  Sect4                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih6","Sectors 5-8 TimeHitsMult(FTcoincEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect5                 Sect6                   Sect7                  Sect8                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih7","Sectors 1-4 TimeHitsMult(FTcoinc+PairedEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect1                 Sect2                   Sect3                  Sect4                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih8","Sectors 5-8 TimeHitsMult(FTcoinc+PairedEvents)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Sect5                 Sect6                   Sect7                  Sect8                       ");
  _filled[_filled.size()-1]->SetFillColor(3);
//------------  
//
  AddSet("Anti-Edep");//set-2
  
  _filled.push_back(new TH1F("antih9","Sector-1 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih10","Sector-2 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih11","Sector-3 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih12","Sector-4 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih13","Sector-5 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih14","Sector-6 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih15","Sector-7 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
  
  _filled.push_back(new TH1F("antih16","Sector-8 Edep(FTcoincEvents,1paired t-hit)",100,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Energy deposition (Mev)");
  _filled[_filled.size()-1]->SetFillColor(3);
//-----------  
//  
  AddSet("Anti-Events Rates");//set-3
  
  _filled.push_back(new TH1F("antih17","Anti-events rates",6,0.,6.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("antih18","Anti-event FT-correlations",12,0.,12.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
//-----------  
//  
  AddSet("AntiTrigPatt Efficiency");//set-4(bias=19)
  
  for (int j = 0; j < kNants; j++){
     sprintf(hname,"antih%2d",hbias[4]+j);
     sprintf(title,"ACCTrigPatt efficiency, Sector=%1d",j+1);
    _filled.push_back(new TProfile(hname,title,50,0.,750.,0.,1.1));
    _filled[_filled.size()-1]->SetYTitle("Efficiency");
    _filled[_filled.size()-1]->SetXTitle("Sector signal(adc-channels)");
    _filled[_filled.size()-1]->SetFillColor(44);
  }
//-----------
//
  AddSet("AntiParans time evolution");//set-5(bias=27)
  
  sprintf(hname,"antih%2d",hbias[5]);
  _filled.push_back(new TProfile(hname,"Aver/MaxRawSideTimeHits vs Time",120,0.,acctrange[accscale],0,20.));
  _filled[_filled.size()-1]->SetYTitle("RawSideHits");
  
  sprintf(hname,"antih%2d",hbias[5]+1);
  _filled.push_back(new TProfile(hname,"Aver/MaxRawSideTimeHits vs Time",120,0.,acctrange[accscale],0,20.));
  _filled[_filled.size()-1]->SetYTitle("RawSideHits");
  
  sprintf(hname,"antih%2d",hbias[5]+2);
  _filled.push_back(new TProfile(hname,"Min/MaxSFEATemperature vs Time",120,0.,acctrange[accscale],-45,45.));
  _filled[_filled.size()-1]->SetYTitle("Temperature(oC)");
  
  sprintf(hname,"antih%2d",hbias[5]+3);
  _filled.push_back(new TProfile(hname,"Min/MaxSFEATemperature vs Time",120,0.,acctrange[accscale],-45,45.));
  _filled[_filled.size()-1]->SetYTitle("Temperature(oC)");
//-------------
//  
  AddSet("FTtime-Time");//set-6(bias=31)
  
  for (Int_t j = 0; j < 2; j++){
     sprintf(hname,"antih%2d",hbias[6]+2*j);
     sprintf(title,"FastTrigTime-SectorTime(1hit case), Sector=1, Side=%1d",j+1);
    _filled.push_back(new TH1F(hname,title,100,-50.,250.));
    _filled[_filled.size()-1]->SetYTitle("Events");
    _filled[_filled.size()-1]->SetXTitle("TimeDifference(ns)");
    _filled[_filled.size()-1]->SetFillColor(3);
     sprintf(hname,"antih%2d",hbias[6]+2*j+1);
     sprintf(title,"FastTrigTime-SectorTime(1hit case), Sector=5, Side=%1d",j+1);
    _filled.push_back(new TH1F(hname,title,100,-50.,250.));
    _filled[_filled.size()-1]->SetYTitle("Events");
    _filled[_filled.size()-1]->SetXTitle("TimeDifference(ns)");
    _filled[_filled.size()-1]->SetFillColor(3);
  }
//
}



void AMSAntiHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  TAxis *xax;
  TText *txt=new TText();
  char name[60],dat[30];
  Char_t text[100];
//

  switch(Set){
case 0:
  gPad->Divide(2,3);
  for(i=0;i<5;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i>1)_filled[i]->SetStats(kFALSE);
    _filled[i]->Draw();//Anti-visibility
    gPadSave->cd();
  }
  break;
case 1:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+5]->SetStats(kFALSE);
    _filled[i+5]->Draw();//Anti-TimeHitsMult
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(4,2);
  for(i=0;i<8;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+9]->Draw();//Anti-Edep
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(1,2);
  gPad->cd(1);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[17]->GetXaxis();
  xax->SetLabelSize(0.04);
  xax->SetBinLabel(1,"Entries");
  _filled[17]->SetBinContent(1,AntiPars::getstat(0));
  xax->SetBinLabel(2,"Lvl1OK");
  _filled[17]->SetBinContent(2,AntiPars::getstat(1));
  xax->SetBinLabel(3,"+AntiEvent");
  _filled[17]->SetBinContent(3,AntiPars::getstat(2));
  xax->SetBinLabel(4,"+FTcoinc");
  _filled[17]->SetBinContent(4,AntiPars::getstat(3));
  xax->SetBinLabel(5,"+2Sides");
  _filled[17]->SetBinContent(5,AntiPars::getstat(4));
  xax->SetBinLabel(6," ");
  _filled[17]->SetBinContent(6,0);
  
  _filled[17]->SetStats(kFALSE);
  _filled[17]->Draw("hbar2");//Anti-events Rates  
  gPadSave->cd();
  
  
  gPad->cd(2);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[18]->GetXaxis();
  xax->SetLabelSize(0.04);
  xax->SetBinLabel(1,"GlobFTOK");
  _filled[18]->SetBinContent(1,AntiPars::getstat(6));
  xax->SetBinLabel(2,"ChargedEM");
  _filled[18]->SetBinContent(2,AntiPars::getstat(7));
  xax->SetBinLabel(3,"+ AntiCoinc");
  _filled[18]->SetBinContent(3,AntiPars::getstat(8));
  xax->SetBinLabel(4,"CharNotEM");
  _filled[18]->SetBinContent(4,AntiPars::getstat(9));
  xax->SetBinLabel(5,"+ AntiCoinc");
  _filled[18]->SetBinContent(5,AntiPars::getstat(10));
  xax->SetBinLabel(6,"NeutrEM");
  _filled[18]->SetBinContent(6,AntiPars::getstat(11));
  xax->SetBinLabel(7,"+ AntiCoinc");
  _filled[18]->SetBinContent(7,AntiPars::getstat(12));
  xax->SetBinLabel(8,"Z>=2");
  _filled[18]->SetBinContent(8,AntiPars::getstat(13));
  xax->SetBinLabel(9,"+ AntiCoinc");
  _filled[18]->SetBinContent(9,AntiPars::getstat(14));
  xax->SetBinLabel(10,"SlowZ>=2");
  _filled[18]->SetBinContent(10,AntiPars::getstat(15));
  xax->SetBinLabel(11,"+ AntiCoinc");
  _filled[18]->SetBinContent(11,AntiPars::getstat(16));
  
  xax->SetBinLabel(12," ");
  _filled[18]->SetBinContent(12,0);
  
  _filled[18]->SetStats(kFALSE);
  _filled[18]->Draw("hbar2");//Anti-event Correlations  
  gPadSave->cd();
  break;
//  
case 4:
  gPad->Divide(4,2);
  for(i=0;i<kNants;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(110010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+hbias[4]]->SetMinimum(-0.01);
    _filled[i+hbias[4]]->SetMaximum(1.1);
    _filled[i+hbias[4]]->SetMarkerStyle(20);
    _filled[i+hbias[4]]->SetMarkerColor(2);
    _filled[i+hbias[4]]->SetMarkerSize(0.5);
    _filled[i+hbias[4]]->Draw("P");// Anti TrigPatt-eff 
    gPadSave->cd();
  }
  break;
//
case 5:
  gPad->Divide(1,2);
  for(i=0;i<4;i++){
    if(i<2)gPad->cd(1);
    else gPad->cd(2);
    gPad->SetGrid();
    gStyle->SetOptStat(110010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+hbias[5]]->SetMarkerSize(0.75);
    _filled[i+hbias[5]]->SetMarkerStyle(20);
    if(i<2){
      _filled[i+hbias[5]]->SetMinimum(0);
      _filled[i+hbias[5]]->SetMaximum(18);
      _filled[i+hbias[5]]->SetMarkerColor(2+i);
    }
    else{
      _filled[i+hbias[5]]->SetMinimum(-45);
      _filled[i+hbias[5]]->SetMaximum(45);
      _filled[i+hbias[5]]->SetMarkerColor(2+i-2);
    }
    if(accscale==1){
      strcpy(name,"Last 120 mins since ");
      strcpy(dat,AntiPars::getdat1());
    }
    else if(accscale==2){
      strcpy(name,"Last 120 hours since ");
      strcpy(dat,AntiPars::getdat2());
    }
    else if(accscale==3){
      strcpy(name,"Last 120 days since ");
      strcpy(dat,AntiPars::getdat3());
    }
    strcat(name,dat);
    xax=_filled[i+hbias[5]]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    if(i==0)_filled[i+hbias[5]]->Draw("P");//Nhits aver
    else if(i==1)_filled[i+hbias[5]]->Draw("PSAME");//Nhits max 
    else if(i==2)_filled[i+hbias[5]]->Draw("P");//SFEA-temper min
    else if(i==3)_filled[i+hbias[5]]->Draw("PSAME");//SFEA-temper max 
    if(i<2){
      txt->SetTextSize(0.05);
      txt->SetTextColor(2+i);
      if(i==0)txt->DrawText(20.,17.,"Aver");
      else txt->DrawText(30.,17.,"Max");
    }
    else{
      txt->SetTextSize(0.05);
      txt->SetTextColor(2+i-2);
      if(i==2)txt->DrawText(20.,41.,"Min");
      else txt->DrawText(30.,41.,"Max");
    }
    gPadSave->cd();
  }
  break;
case 6:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    gStyle->SetOptStat(111111);
    _filled[i+hbias[6]]->Draw();//Sect1/5(side=1,2) FTtime-Time
    gPadSave->cd();
  }
  break;
  }//--->endof switch
}



void AMSAntiHist::Fill(AMSNtupleR *ntuple){
// 
  AntiPars::addstat(0);//<--- Entries
//
  Level1R *p2lev1(0);
  Bool_t LVL1OK(0);
  Int_t ECTrigFl(0);
  Int_t TOFTrigFl1(0),TOFTrigFl2(0);
  Bool_t ftc(0),ftz(0),bz(0),fte(0),glft(0);
  Int_t physbpat(0),membpat(0),antipat(0);
  Int_t is,ih,i,j;
  Float_t ltime(1);
  Int_t etime[2],evnum,runum;
  Char_t date[300];
  static Float_t range[3],timez[3];
  static Int_t first(1),etime0(0),evnloc;
  Float_t time[3];
  Float_t dt;
//
//-----
  etime[0]=ntuple->fHeader.Time[0];//unix-time(sec)
  etime[1]=ntuple->fHeader.Time[1];
  evnum=ntuple->fHeader.Event;
  runum=ntuple->fHeader.Run;
  strcpy(date,ntuple->Time());
//
  if(first==1){
    first=0;
    etime0=etime[0];
    cout<<"Anti: 1st event Run/event:"<<runum<<" "<<evnum<<" date:"<<date<<" evloc="<<evnloc<<endl;
    for(i=0;i<3;i++){
      timez[i]=0;
    }
    evnloc=0;
    AntiPars::setdat1(ntuple->Time());
    AntiPars::setdat2(ntuple->Time());
    AntiPars::setdat3(ntuple->Time());
  }
  time[0]=(etime[0]-etime0)/60;//ev.time starting from beg.of.run(min)
  time[1]=(etime[0]-etime0)/3600;//ev.time starting from beg.of.run(hour)
  time[2]=(etime[0]-etime0)/86400;//ev.time starting from beg.of.run(day)
//-----
  if(ntuple->nLevel1()>0){
    p2lev1=ntuple->pLevel1(0);
    LVL1OK=1;
    TOFTrigFl1=p2lev1->TofFlag1;
    TOFTrigFl2=p2lev1->TofFlag2;
    ECTrigFl=p2lev1->EcalFlag;
    membpat=ntuple->pLevel1(0)->JMembPatt;
    antipat=ntuple->pLevel1(0)->AntiPatt;
    ftc=((membpat&1)>0);//Z>=1
    ftz=((membpat&1<<5)>0);//SlowZ>=2
    fte=((membpat&1<<6)>0);//ec-FT
    bz= ((membpat&1<<9)>0);//Z>=2(when ftc)
    glft=(ftc||ftz||fte);//globFT
  }
  else return;
  AntiPars::addstat(1);//<--- Lev1 OK
//
  Float_t eantit(0),eanti;
#ifdef __USEANTICLUSTERPG__
  ntuple->RebuildAntiClusters();
#endif
  Int_t nasects=ntuple->NAntiCluster();
  Int_t nparts=ntuple->nParticle();
  Int_t ntrktrs=ntuple->nTrTrack();
  Int_t necshow=ntuple->nEcalShower();
  Int_t nasftc(0),nasprd(0),nasftpr(0);
//
  if(nasects>0){//Anti-event (clust)
    AntiPars::addstat(2);//<--- Found any fired Anti
//
    AntiClusterR *p;
    Int_t sect,nthits,ntpairs;
    UInt_t status;
    Float_t htime[16];
    Bool_t ftcok(0),miside(0),s1ok(0),s2ok(0);
//
    for(is=0;is<nasects;is++){//<-- loop over Clusters(fired sectors)
      p=ntuple->pAntiCluster(is);
      sect=p->Sector;//1-8

#ifndef __USEANTICLUSTERPG__
      status=p->Status;// Bit"128"->No FT-coinc. on 2 sides;"256"->1sideSector;"1024"->miss.side#
      ftcok=((status&1<<7)==0);//FTcoins ok (on any side)
      miside=((status&1<<8)==1);//1-sided sector
      if(miside){
        s1ok=((status&1<<10)==1);
        s2ok=((status&1<<10)==0);
      }
      eanti=p->Edep;
      if(ftcok)nasftc+=1;//count sectors with FTcoinc
      nthits=p->Ntimes;
      ntpairs=p->Npairs;
      if(ntpairs>0)nasprd+=1;//count sectors with t-paired sides
      if(ftcok && ntpairs>0)nasftpr+=1;//count sectors with FTcoins+paired 
      for(i=0;i<nthits;i++)htime[i]=p->Times[i];
#else
      //      p->ReBuildMe();
      status = 0;  // dummy 0
      ftcok=(1==1);  // dummy true
      miside=(p->Npairs<0);   // is true if 1-sides sector
      if(miside){
        s1ok=(1==1);        // always true no info for this 
        s2ok=(1==0);        // alwais false no info for this 
      }
      eanti=p->Edep;
      if(ftcok)nasftc+=1;   //count sectors with FTcoinc
      nthits=abs(p->Npairs); // dummy is the number of pairs or number of 1-sides times
      ntpairs=p->Npairs;     // if >0 is the number of pairs
      if (ntpairs<0) ntpairs = 0;
      if(ntpairs>0)nasprd+=1;//count sectors with t-paired sides
      if(ftcok && ntpairs>0)nasftpr+=1;//count sectors with FTcoins+paired 
      //for(i=0;i<nthits;i++)htime[i]=p->Times[i];
#endif

      eantit+=eanti;
       
      _filled[2]->Fill(sect,1.);
      if(s1ok)_filled[2]->Fill(sect+10,1.);
      if(s2ok)_filled[2]->Fill(sect+20,1.);
    
      if(ftcok){
        _filled[3]->Fill(sect,1.);
        if(s1ok)_filled[3]->Fill(sect+10,1.);
        if(s2ok)_filled[3]->Fill(sect+20,1.);
        if(sect<5)_filled[5]->Fill(nthits+(sect-1)*10,1.);//time-hits multipl.(FTcoinc)
        else _filled[6]->Fill(nthits+(sect-5)*10,1.);
      }
    
      if(ftcok && ntpairs>0){
        _filled[4]->Fill(sect,1.);
        if(s1ok)_filled[4]->Fill(sect+10,1.);
        if(s2ok)_filled[4]->Fill(sect+20,1.);
        if(sect<5)_filled[7]->Fill(nthits+(sect-1)*10,1.);//time-hits multipl.(FTcoinc+paired)
        else _filled[8]->Fill(nthits+(sect-5)*10,1.);
      }
    
      if(ftcok && ntpairs==1)_filled[9+sect-1]->Fill(eanti,1.);
    }//--->endof cluster loop
//
    _filled[0]->Fill(nasects,1.);
    _filled[1]->Fill(nasftc,1.);
    if(nasftc>0)AntiPars::addstat(3);//<--- event with at least 1 fired+FTcoinc sector
    if(nasftc>0 && nasprd>0)AntiPars::addstat(4);//<--- event with at least 1 fired+FTcoinc+Paired sector
  }//--->endof "anti-event"(clust) check
//-----------------
  AntiRawSideR * p2raws;
  Int_t nantrs=ntuple->NAntiRawSide();
  int swid,sect,side,stat,tdct[2][kNants][16],ftdc[2][kNants][8];
  Int_t ntdct[2][kNants]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t nftdc[2][kNants]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t ampl,temper,mxtemp(-273),mntemp(9999),ampls[2]={0,0};
  Int_t nhmin(999),nhmax(0),nhnonz(0);
  Float_t nhavr(0);
  Float_t adca[2][kNants]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t temp[2][kNants]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t FtCoinc[2][kNants]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  for(is=0;is<nantrs;is++){//<-- loop over RawSide-objects
    p2raws=ntuple->pAntiRawSide(is);
    swid=p2raws->swid;//BBS
    sect=swid/10-1;//0-7
    side=swid%10-1;//0-1
    stat=p2raws->stat;
    adca[side][sect]=p2raws->adca;
    ntdct[side][sect]=p2raws->ntdct;
    if(ntdct[side][sect]>0){
      nhavr+=ntdct[side][sect];
      nhnonz+=1;
    }
    if(ntdct[side][sect]>nhmax)nhmax=ntdct[side][sect];
    for(ih=0;ih<ntdct[side][sect];ih++)tdct[side][sect][ih]=p2raws->tdct[ih];
    nftdc[side][sect]=p2raws->nftdc;
    for(ih=0;ih<nftdc[side][sect];ih++)ftdc[side][sect][ih]=p2raws->ftdc[ih];
    temper=p2raws->temp;
    if(temper==999)temper=20-40*(is%2);//tempor for debugging
    temp[side][sect]=temper;
    if(temp[side][sect]<mntemp)mntemp=temp[side][sect];
    if(temp[side][sect]>mxtemp)mxtemp=temp[side][sect];
  }
  if(nhnonz>0)nhavr/=nhnonz;
//-------->AccTrigPatt Eff:
//FT-Time correlation(sect=1/5 only)
  for(i=0;i<2;i++){//side loop
    if(nftdc[i][0]==1 && ntdct[1][0]==1){//sect-1
      dt=(ftdc[i][0][0]-tdct[i][0][0])*kTDCbin;
      _filled[hbias[6]+2*i]->Fill(dt,1.);
    }
    if(nftdc[i][4]==1 && ntdct[1][4]==1){//sect-5
      dt=(ftdc[i][4][0]-tdct[i][4][0])*kTDCbin;
      _filled[hbias[6]+2*i+1]->Fill(dt,1.);
    }
  }
//make coinc-map:
  for(int sec=0;sec<kNants;sec++){//<-- loop over sectors
    for(i=0;i<2;i++){//side loop
      if(nftdc[i][sec]>0){
        for(ih=0;ih<nftdc[i][sec];ih++){
          dt=(ftdc[i][sec][0]-tdct[i][sec][ih])*kTDCbin;
	  if(dt<190 && dt>80)FtCoinc[i][sec]=1;
	}
      }
    }
  }
//check acc trig.patt.bits correlation:
  for(is=0;is<kNants;is++){//<-- loop over sectors
    ampl=0;
    if((adca[0][is]>0 && FtCoinc[0][is]==1) || (adca[1][is]>0 && FtCoinc[1][is]==1)){
      if(adca[0][is]>0 && FtCoinc[0][is]==1)ampls[0]=adca[0][is];
      if(adca[1][is]>0 && FtCoinc[1][is]==1)ampls[1]=adca[1][is];
      ampl=ampls[0];
      if(ampl<ampls[1])ampl=ampls[1];//take max ampl
      ((TProfile*)_filled[hbias[4]+is])->Fill(ampl,(AntiPars::patbcheck(is,antipat)?1:0),1.);//bitpatt eff
    }
  }
//---------------------
//---> time-evolution:
//
  if((time[accscale-1]-timez[accscale-1])>=acctrange[accscale-1]){
    ((TProfile*)_filled[hbias[5]])->Reset("");
    ((TProfile*)_filled[hbias[5]+1])->Reset("");
    ((TProfile*)_filled[hbias[5]+2])->Reset("");
    ((TProfile*)_filled[hbias[5]+3])->Reset("");
    timez[accscale-1]=time[accscale-1];
    time[accscale-1]+=0.001;
    if(accscale==1)AntiPars::setdat1(ntuple->Time());
    if(accscale==2)AntiPars::setdat2(ntuple->Time());
    if(accscale==3)AntiPars::setdat3(ntuple->Time());
  }
 
  if(nhnonz>0)((TProfile*)_filled[hbias[5]])->Fill(time[accscale-1]-timez[accscale-1],nhavr,1.);//Aver RawSide-hits
  if(nhmax>0)((TProfile*)_filled[hbias[5]+1])->Fill(time[accscale-1]-timez[accscale-1],nhmax+0.15,1.);//Max RawSide-hits
  if(mntemp<9999)((TProfile*)_filled[hbias[5]+2])->Fill(time[accscale-1]-timez[accscale-1],mntemp,1.);//Min SFEA temper
  if(mxtemp>-273)((TProfile*)_filled[hbias[5]+3])->Fill(time[accscale-1]-timez[accscale-1],mxtemp,1.);//Max SFEA temper
//
//
//-----------------
  if(glft){
    AntiPars::addstat(6);//globFT OK
    if(ftc && fte)AntiPars::addstat(7);//ChargEMTrig
    if(ftc && fte && nasftc>0)AntiPars::addstat(8);//ChargEMTrig+Anti(coinc)
    if(ftc && !fte)AntiPars::addstat(9);//ChargNotEMTrig
    if(ftc && !fte && nasftc>0)AntiPars::addstat(10);//ChargNotEMTrig+Anti(coinc)
    if(fte && !ftc)AntiPars::addstat(11);//NeutrEMTrig
    if(fte && !ftc && nasftc>0)AntiPars::addstat(12);//NeutrEMTrig+Anti(coinc)
    if(ftc && bz)AntiPars::addstat(13);//HiZTrig
    if(ftc && bz && nasftc>0)AntiPars::addstat(14);//HiZTrig+Anti(coinc)
    if(ftz && !bz)AntiPars::addstat(15);//SlowZTrig
    if(ftz && !bz && nasftc>0)AntiPars::addstat(16);//SlowZTrig+Anti(coinc)
  }
}


