//  $Id: AMSLVL1Hist.cxx,v 1.13 2005/05/11 12:57:49 choumilo Exp $
//      v1.0/E.Choumilov/20.06.2003
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



void AMSLVL1Hist::Book(){

  AddSet("LVL1 BranchSharing");
  
  _filled.push_back(new TH1F("FTBranches","FastTrigger Branch Sharing",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Index:Unkn(0)/Z=1/Z>=2/Z>=2slow/ECAL(4))");
  _filled[_filled.size()-1]->SetFillColor(41);//1
  
  _filled.push_back(new TH1F("LVL1Branches","LVL1 Branch Sharing",10,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("Index: unb(1:4)/Z=1/Z>=2/el/ph(8))");
  _filled[_filled.size()-1]->SetFillColor(44);
   
  

  AddSet("TOF-TotMultiplicity in LVL1");
  
  _filled.push_back(new TH1F("TOFLayersZ1","TOF-layers per events, Z>=1",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(41);//3
  
  _filled.push_back(new TH1F("TOFLayersZ2","TOF-layers per events, slowZ>=2",5,0.,5.));
  _filled[_filled.size()-1]->SetXTitle("Total layers");
  _filled[_filled.size()-1]->SetFillColor(43);//4
  
  _filled.push_back(new TH1F("TOFPaddlesZ1","TOF-paddles per events, Z>=1",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(41);//5

  _filled.push_back(new TH1F("TOFPaddlesZ2","TOF-paddles per events, slowZ>=2",35,0.,35.));
  _filled[_filled.size()-1]->SetXTitle("Total paddles");
  _filled[_filled.size()-1]->SetFillColor(43);//6
  
  
  AddSet("TOF-patterns in LVL1");
  
  _filled.push_back(new TH1F("TOFMLCodeZ1","TOF MissingPlaneCode, Z>=1",10,-1.,9.));
  _filled[_filled.size()-1]->SetXTitle("MissingPlaneCode");
  _filled[_filled.size()-1]->SetFillColor(41);//7
  
  _filled.push_back(new TH1F("TOFZ1PatS1","TOF-pads pattern in FTZ>=1(Side1)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(41);//8
   
  _filled.push_back(new TH1F("TOFZ1PatS2","TOF-pads pattern in FTZ>=1(Side2)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(41);//9
   
  _filled.push_back(new TH1F("TOFMLCodeZ2","TOF MissingPlaneCode, slowZ>=2",10,-1.,9.));
  _filled[_filled.size()-1]->SetXTitle("MissingPlaneCode");
  _filled[_filled.size()-1]->SetFillColor(44);//10
  
  _filled.push_back(new TH1F("TOFZ2PatS1","TOF-pads pattern in FTZ>=2(Side1)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(44);//11
   
  _filled.push_back(new TH1F("TOFZ2PatS2","TOF-pads pattern in FTZ>=2(Side2)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(44);//12
   
  AddSet("ANTIpatterns in LVL1");
  
  _filled.push_back(new TH1F("ANTIPattern","ANTI-sectors(logical) pattern",8,1.,9.));
  _filled[_filled.size()-1]->SetXTitle("Sector number");
  _filled[_filled.size()-1]->SetFillColor(41);//13
  
  _filled.push_back(new TH1F("ANTISectNum1","Total Anti-sectors, Z>=1 TofFtrig",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(44);//14
  
  _filled.push_back(new TH1F("ANTISectNum2","Total Anti-sectors, slowZ>=2 TofFtrig",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(41);//15
  
  _filled.push_back(new TH1F("ANTISectNum3","Total Anti-sectors, EC-driven Ftrig",9,0.,9.));
  _filled[_filled.size()-1]->SetXTitle("Total sectors/event");
  _filled[_filled.size()-1]->SetFillColor(44);//16
  
  AddSet("ECAL in LVL1");
  
  _filled.push_back(new TH1F("ECALEnFlag1","EcalEnergyFlag(EC-driven trig) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("EtotFlag: <MipThr(0)/>MipThr(1)/>LowThr(2)/>HiThr(3)");
  _filled[_filled.size()-1]->SetFillColor(41);//17
  
  _filled.push_back(new TH1F("ECALSwFlag1","EcalShowWidthFlag(EC-driven trig) ",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("WidthFlag:EtFlag<3(0)/EtFlag=3:nonEM(1)/EM(2)");
  _filled[_filled.size()-1]->SetFillColor(41);
  
  _filled.push_back(new TH1F("ECALEnFlag2","EcalEnergyFlag(TOF-driven trig) ",4,0.,4.));
  _filled[_filled.size()-1]->SetXTitle("EtotFlag: <MipThr(0)/>MipThr(1)/>LowThr(2)/>HiThr(3)");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("ECALSwFlag2","EcalShowWidthFlag(TOF-driven trig) ",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("WidthFlag:EtFlag<3(0)/EtFlag=3:nonEM(1)/EM(2)");
  _filled[_filled.size()-1]->SetFillColor(44);
  
}


void AMSLVL1Hist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  switch(Set){
case 0:
  gPad->Divide(2,1);
  for(i=0;i<2;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i]->Draw();//FastTrig/LVL1-BranchSharing
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
    _filled[i+2]->Draw();//TOF-TotMultiplicity
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
    _filled[i+6]->Draw();//TOF-patterns
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+12]->Draw();//ANTIpatterns
    gPadSave->cd();
  }
  break;
case 4:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+16]->Draw();//ECAL 
    gPadSave->cd();
  }
//
  }
//
}



void AMSLVL1Hist::Fill(AMSNtupleR *ntuple){
  int lmax(4);//tot tof-layers
  int padpl[4]={8,8,10,8};//tof-paddles per layer
  int tofflg(-1),tofpat1[4],tofpat2[4],npds1(0),npds2(0);
  int ecflg(-1),etflg(0),ewflg(0);
  int mflg(0),ntofl(0),ntofp(0),mlcode(-1),tgmode,lsbt;
//   
  if(ntuple->nLevel1()){
    ecflg=ntuple->pLevel1(0)->EcalFlag;
    etflg=ecflg/10;//ECEtot flag
    ewflg=ecflg%10;//ECWiwth flag
//
    tofflg=ntuple->pLevel1(0)->TofFlag;
    if(tofflg>=0){//tof in trigger
      if(tofflg/10==1)mflg=2;//z>=2normal
      else if(tofflg/10==2)mflg=3;//z>=2slow
      else mflg=1;//z=1
    }
    else{
      if(ecflg>0)mflg=4;//ec-driven trigger
    }
    _filled[0]->Fill(mflg,1.);//FTBranchCode
    tgmode=ntuple->pLevel1(0)->Mode;
    for(int i=0;i<9;i++){
      lsbt=1<<i;
      if(tgmode & lsbt)_filled[1]->Fill(i+1,1.);//LVL1BranchCode
    }
//                                       <------ TOF
    mlcode=tofflg%10;//miss layers code
    if(mlcode==0)ntofl=4;
    else if(mlcode<5)ntofl=3;
    else ntofl=2;
    if(mflg==1 || mflg==2)_filled[2]->Fill(ntofl,1.);//tot. tof-layers(normal z>=1)
    if(mflg==3)_filled[3]->Fill(ntofl,1.);//....(slow z>=2)
//
    for(int i=0;i<lmax;i++)tofpat1[i]=ntuple->pLevel1(0)->TofPatt[i];
    for(int i=0;i<lmax;i++)tofpat2[i]=ntuple->pLevel1(0)->TofPatt1[i];
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        if((tofpat1[il]&1<<ip)>0 || (tofpat1[il]&1<<(16+ip))>0)npds1+=1;  
        if((tofpat2[il]&1<<ip)>0 || (tofpat2[il]&1<<(16+ip))>0)npds2+=1;
      }
    }
    if(mflg==1 || mflg==2)_filled[4]->Fill(npds1,1.);//tot. tof-paddles(ORed sides)
    if(mflg==3)_filled[5]->Fill(npds2,1.);
//
    if(mflg==1 || mflg==2)_filled[6]->Fill(mlcode,1.);//miss.layer code
    if(mflg==3)_filled[9]->Fill(mlcode,1.);
//
    int nch;
    for(int il=0;il<lmax;il++){
      for(int ip=0;ip<padpl[il];ip++){
        nch=il*10+ip+1;
        if((tofpat1[il]&1<<ip)>0)_filled[7]->Fill(nch,1.);//s1-patt, z>=1
	if((tofpat1[il]&1<<(16+ip))>0)_filled[8]->Fill(nch,1.);//s2patt, z>=1
	if(tofflg/10>0){  
          if((tofpat2[il]&1<<ip)>0)_filled[10]->Fill(nch,1.);//s1-patt, Z>=2
	  if((tofpat2[il]&1<<(16+ip))>0)_filled[11]->Fill(nch,1.);//s2patt, Z>=2
	}  
      }
    }
//                                       <------ ANTI
    int asmax(8),nast(0);
    int antipat=ntuple->pLevel1(0)->AntiPatt;
    for(int is=0;is<asmax;is++){
      if((antipat & 1<<is)>0)nast+=1;
      if((antipat & 1<<is)>0)_filled[12]->Fill(is+1,1.);//sectors-patt
    }
    if(mflg>=1 && mflg<=2)_filled[13]->Fill(nast,1.);//tot sectors, when Z>=1
    else if(mflg==3)_filled[14]->Fill(nast,1.);//             ...., when slowZ>=2
    else if(mflg==4)_filled[15]->Fill(nast,1.);//             ...., when EC-driven
//    
//                                       <------ ECAL
    if(tofflg<0){//EC-drived LVL1
      _filled[16]->Fill(etflg,1.);//Etot-flag
      _filled[17]->Fill(ewflg,1.);//ShWidth-flag
    }
    else{//TOF-driven LVL1
      _filled[18]->Fill(etflg,1.);//Etot-flag
      _filled[19]->Fill(ewflg,1.);//ShWidth-flag
    }
//
  } 
}


