//  $Id: AMSTOFHist.cxx,v 1.40 2011/04/04 15:23:04 choumilo Exp $
// v1.0 E.Choumilov, 12.05.2005
// v1.1 E.Choumilov, 19.01.2006
// 
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSTOFHist.h"

const Int_t kNants=8;//ANTI sectors
const Int_t kNtofl=4;//TOF layers
const Int_t kNtofb[4]={8,8,10,8};//TOF bars per layer
const Float_t kTDCbin=0.024414;//TDC-bin(ns)
const Int_t tofscales[5]={1,1,1,1,1};//(1-3)scale_type for 5 time-evolution graphs:
//                                        TimeHits/HistHits/Temp_SFET/_SFEC/_PMT
//                                     and tempor.used for trk/trd tracks/ftc graphs
//--------------------------------
class RunPar{ 
//
private:
//
  static Int_t evsel[80];//events, passed particular cuts
//
  static Bool_t mcdata;
//
  static Char_t dat1[30];
  static Char_t dat2[30];
  static Char_t dat3[30];
public:
//
  static void SetMCF1(){
    mcdata=1;
  }
  static void SetMCF0(){
    mcdata=0;
  }
  static Bool_t GetMCF(){
    return mcdata;
  }
  static void init(){
    for(Int_t i=0;i<80;i++)evsel[i]=0;
  }
  static void addsev(Int_t i){
    assert(i>=0 && i< 80);
    evsel[i]+=1;
  }
  static Int_t getsev(Int_t i){
    assert(i>=0 && i< 80);
    return evsel[i];;
  }
//
  inline static void setdat1(Char_t *d){strcpy(dat1,d);}
  inline static void setdat2(Char_t *d){strcpy(dat2,d);}
  inline static void setdat3(Char_t *d){strcpy(dat3,d);}
  inline static Char_t* getdat1(){return dat1;}
  inline static Char_t* getdat2(){return dat2;}
  inline static Char_t* getdat3(){return dat3;}
};
//------------------------------------
  Int_t RunPar::evsel[80];//events, selected by particular cuts
//
  Bool_t RunPar::mcdata;
//
  Char_t RunPar::dat1[30];
  Char_t RunPar::dat2[30];
  Char_t RunPar::dat3[30];
//
//------------------------------------

void AMSTOFHist::Book(){ 
  RunPar::init();//clear counters
  RunPar::SetMCF0();//set def(real data)
//set-0
  AddSet("TOFMultiplicity");
  
  _filled.push_back(new TH1F("tofh0","TOF:TotalPaddles",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Paddles per event");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh1","TOF:TotalClusters(WhenBetaFound)",25,0.,25.));
  _filled[_filled.size()-1]->SetXTitle("Clusters per event");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh2","TOF:PaddleNumber(UsedByBeta)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("Npaddles+10*(Layer-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh3","TOF:Paddles/cluster(UsedByBeta)",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("Npaddles");
  _filled[_filled.size()-1]->SetFillColor(8);
//set-1  
  AddSet("TofBetaParameters");
  
  _filled.push_back(new TH1F("tofh4","TofBeta Chi2space",50,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("space-fit chi2");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh5","TofBeta Chi2time",50,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("time-fit chi2");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh6","TofBeta UsedLayers Pattern",10,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("0->4Layers, 1-4->miss(4-1), 5:1+3, 6:1+4, 7:2+3, 8:2+4, 9:1+2");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh7","TofBeta",480,-1.2,1.2));
  _filled[_filled.size()-1]->SetXTitle("Velocity/C");
  _filled[_filled.size()-1]->SetFillColor(8);
  
//set-2  
  AddSet("TrkParameters");

  _filled.push_back(new TH1F("tofh8","TRKClusters",75,0.,150.));
  _filled[_filled.size()-1]->SetXTitle("Clusters per event");
  _filled[_filled.size()-1]->SetFillColor(9);
  
  _filled.push_back(new TH1F("tofh9","TRKChi2",80,0.,80.));
  _filled[_filled.size()-1]->SetXTitle("Chi2");
  _filled[_filled.size()-1]->SetFillColor(9);
  
  _filled.push_back(new TProfile("tofh10","TrkTracks/FTC vs Time",120,0,toftrange[tofscales[0]],0,1));
  _filled[_filled.size()-1]->SetYTitle("TrkTracks/Ftc");
 
  
  
  _filled.push_back(new TH1F("tofh11","TRKChi2(MultScattOff)",50,0.,500.));
  _filled[_filled.size()-1]->SetXTitle("chi2");
  _filled[_filled.size()-1]->SetFillColor(9);
  
  _filled.push_back(new TH1F("tofh12","TRKHalfRigAss",50,-1.,1.));
  _filled[_filled.size()-1]->SetXTitle("ass=r1-r2/r1+r2");
  _filled[_filled.size()-1]->SetFillColor(9);
  
    _filled.push_back(new TProfile("tofh13","RigErr/Rig vs Rigidity(gv)",100,0,50,0,200));
    _filled[_filled.size()-1]->SetYTitle("%");
    _filled[_filled.size()-1]->SetXTitle("|Rigidity| (Gv)");
//set-3  
  AddSet("LongitTofTrkMatching");

  _filled.push_back(new TProfile("tofh14","LongTofCoord-TrkCross, Layer1",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
//
  _filled.push_back(new TProfile("tofh15","LongTofCoord-TrkCross, Layer2",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
//
  _filled.push_back(new TProfile("tofh16","LongTofCoord-TrkCross, Layer3",10,1,11,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
//
  _filled.push_back(new TProfile("tofh17","LongTofCoord-TrkCross, Layer4",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
//
//set-4  
  AddSet("TofResponceUniformity");

  _filled.push_back(new TH1F("tofh18","TofTruncEdep(pass/mip-norm)",200,0.,50.));
  _filled[_filled.size()-1]->SetXTitle("Edep(mev)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TProfile("tofh19","Edep(mip)<->PadNumber, Layer1",8,1,9,0.1,8));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("NormEdep(Mev)");

  _filled.push_back(new TProfile("tofh20","Edep(mip)<->PadNumber, Layer2",8,1,9,0.1,8));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("NormEdep(Mev)");

  _filled.push_back(new TProfile("tofh21","Edep(mip)<->PadNumber, Layer3",10,1,11,0.1,8));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("NormEdep(Mev)");

  _filled.push_back(new TProfile("tofh22","Edep(mip)<->PadNumber, Layer4",8,1,9,0.1,8));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("NormEdep(Mev)");
  
  _filled.push_back(new TProfile("tofh23","Edep(aver,mip)<->longCoo, id=204",13,-65,65,0.1,8));
  _filled[_filled.size()-1]->SetXTitle("TofLongCoord(cm)");
  _filled[_filled.size()-1]->SetYTitle("NormEdep(Mev)");
//set-5
  AddSet("RecoStagesEff");
  
  _filled.push_back(new TH1F("tofh24","RecoStagesEfficiency",21,1.,22.));
  _filled[_filled.size()-1]->SetYTitle("efficiency");
  _filled[_filled.size()-1]->SetFillColor(44);
//set-6
  AddSet("TofTimeStability");
  
    _filled.push_back(new TProfile("tofh25","LBBS=1041 TimeHits/side vs Time",120,0,toftrange[tofscales[0]],0,16));
    _filled[_filled.size()-1]->SetYTitle("Number of hits");
    
    _filled.push_back(new TProfile("tofh26","LBBS=1041 HistHits/side vs Time",120,0,toftrange[tofscales[1]],0,16));
    _filled[_filled.size()-1]->SetYTitle("Number of hits");
    
    _filled.push_back(new TProfile("tofh27","LBBS=1041 AverTemperature vs Time",120,0,toftrange[tofscales[2]],-40,40));
    _filled[_filled.size()-1]->SetYTitle("Temperature(degree)");
    
    _filled.push_back(new TProfile("tofh28","LBBS=1041 AverTemperature vs Time",120,0,toftrange[tofscales[3]],-40,40));
    _filled[_filled.size()-1]->SetYTitle("Temperature(degree)");
  
    _filled.push_back(new TProfile("tofh29","LBBS=1041 AverTemperature vs Time",120,0,toftrange[tofscales[4]],-40,40));
    _filled[_filled.size()-1]->SetYTitle("Temperature(degree)");
//----
//set-7
  AddSet("TofOccupancy-1");
  
  _filled.push_back(new TH1F("tofh30","TOF-pads AmplCh-activity map (Anode,Side1)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
   
  _filled.push_back(new TH1F("tofh31","TOF-pads TimeCh-activity map (Side1)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
   
  _filled.push_back(new TH1F("tofh32","TOF-pads (Ampl+Time)Ch-activity map (Side1)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
   
  _filled.push_back(new TH1F("tofh33","TOF-pads AmplCh-activity map (Anode,Side2)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
   
  _filled.push_back(new TH1F("tofh34","TOF-pads TimeCh-activity map (Side2)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
   
  _filled.push_back(new TH1F("tofh35","TOF-pads (Ampl+Time)Ch-activity map (Side2)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
//set-8  
  AddSet("TofOccupancy-2");
  
  _filled.push_back(new TH1F("tofh36","TOF-pads DynodeCh-activity map(Aanode>thr,Side1)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH2F("tofh37","Tof Time-channel occupancy(side1)",40,0.,40.,16,0.,16.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetYTitle("Hits/channel/event");
   
  _filled.push_back(new TH1F("tofh38","TOF-pads DynodeCh-activity map(Aanode>thr,Side2)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH2F("tofh39","Tof Time-channel occupancy(side2)",40,0.,40.,16,0.,16.));
  _filled[_filled.size()-1]->SetXTitle("TOF-paddle number+10*(L-1)");
  _filled[_filled.size()-1]->SetYTitle("Hits/channel/event");
   
//set-9  
  AddSet("TofChargeMass");
  
  _filled.push_back(new TH2F("tofh40","Tof Edep(Truncated-1, Beta>0)",50,0.2,1.2,80,0.,48.));
  _filled[_filled.size()-1]->SetXTitle("TofVelocity/LightVelocity");
  _filled[_filled.size()-1]->SetYTitle("Edep(mev)");
  
  _filled.push_back(new TH2F("tofh41","Tof Edep(Truncated-1, Beta<0)",50,0.2,1.2,80,0.,48.));
  _filled[_filled.size()-1]->SetXTitle("TofVelocity/LightVelocity");
  _filled[_filled.size()-1]->SetYTitle("Edep(mev)");
  
  _filled.push_back(new TH2F("tofh42","Tof Edep(Truncated-1,betcorr)",10,0.5,10.5,80,0.,16.));
  _filled[_filled.size()-1]->SetXTitle("TofCharge");
  _filled[_filled.size()-1]->SetYTitle("SQRT[Edep(mev)]");
  
  _filled.push_back(new TH1F("tofh43","Tof-Trk mass (beta>0)",100,0.5,4.5));
  _filled[_filled.size()-1]->SetXTitle("Mass(Gev)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh44","Tof-Trk mass (beta<0)",100,0.5,4.5));
  _filled[_filled.size()-1]->SetXTitle("Mass(Gev)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
//set-10  
  AddSet("Rigidity/Momentum");
  
  _filled.push_back(new TH1F("tofh45","ParticleMomentum(TofBeta>0)",100,-25,25));
  _filled[_filled.size()-1]->SetXTitle("Momentum(Gev)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh46","ParticleMomentum(TofBeta<0)",100,-25,25));
  _filled[_filled.size()-1]->SetXTitle("Momentum(Gev)");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh47","1/FastFitRigidity(Beta>0)",100,-5,5));
  _filled[_filled.size()-1]->SetXTitle("InvGev");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh48","1/FastFitRigidity(Beta<0)",100,-5,5));
  _filled[_filled.size()-1]->SetXTitle("InvGev");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh49","Tof Beta, PartMom>0",480,-1.2,1.2));
  _filled[_filled.size()-1]->SetXTitle("TofVelocity/C");
  _filled[_filled.size()-1]->SetFillColor(8);
  
  _filled.push_back(new TH1F("tofh50","Tof Beta, PartMom<0",480,-1.2,1.2));
  _filled[_filled.size()-1]->SetXTitle("TofVelocity/C");
  _filled[_filled.size()-1]->SetFillColor(8);
  
//set-11  
  AddSet("TRD");
  
  _filled.push_back(new TProfile("tofh51","TrdTracks/FTC vs Time",120,0,toftrange[tofscales[0]],0,1));
  _filled[_filled.size()-1]->SetYTitle("TrdTracks/Ftc");
  
  _filled.push_back(new TH1F("tofh52","TrdRawHits",100,0,100));
  _filled[_filled.size()-1]->SetFillColor(8);
  _filled.push_back(new TH1F("tofh53","TrdTrackChi2",50,0,5));
  _filled[_filled.size()-1]->SetFillColor(8);
}
//------------------------------------

void AMSTOFHist::ShowSet(Int_t Set){
  TAxis *xax;
  TText *txt=new TText();
  Float_t binc[30]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Int_t nentr;
  Float_t p1min,p1max,p2min,p2max;
//
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i,j;
  Char_t name[60],dat[30];
  Char_t text[100];
//
  switch(Set){
case 0:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i]->Draw();//TOF multiplicities
    gPadSave->cd();
  }
  break;
case 1:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+4]->Draw();//TOF beta-parameters
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(100011);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(tofscales[i]==1){
      strcpy(name,"Last 120 mins since ");
      strcpy(dat,RunPar::getdat1());
    }
    else if(tofscales[i]==2){
      strcpy(name,"Last 120 hours since ");
      strcpy(dat,RunPar::getdat2());
    }
    else if(tofscales[i]==3){
      strcpy(name,"Last 120 days since ");
      strcpy(dat,RunPar::getdat3());
    }
    if(i==2 || i==5){
      if(i==2){
        _filled[i+8]->SetMaximum(1.);
        strcat(name,dat);
        xax=_filled[i+8]->GetXaxis();
        xax->SetTitle(name);
        xax->SetTitleSize(0.05);
      }
      if(i==5)_filled[i+8]->SetMaximum(50.);
      _filled[i+8]->SetMarkerStyle(20);
      _filled[i+8]->SetMarkerSize(0.5);
      _filled[i+8]->SetMarkerColor(9);
      _filled[i+8]->Draw("P");//TRK-tracks parameters
    }
    else{
      _filled[i+8]->Draw();//TRK-tracks parameters
    }
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+14]->SetMinimum(-2.5);
    _filled[i+14]->SetMaximum(2.5);
    _filled[i+14]->SetMarkerStyle(21);
    _filled[i+14]->SetMarkerColor(8);
    _filled[i+14]->Draw("PE");//TOF-TRK matching profiles
    gPadSave->cd();
  }
  break;
case 4:
  gPad->Divide(3,2);
  gPad->cd(1);
  gStyle->SetOptStat(11);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(1);
//  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  _filled[18]->Draw();//TOFEdep-truncated,normalized
  gPadSave->cd();
  for(i=0;i<4;i++){
    gPad->cd(i+2);
    gPad->SetGrid();
    gStyle->SetOptStat(1);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+19]->SetMinimum(0);
    _filled[i+19]->SetMaximum(5);
    _filled[i+19]->SetMarkerStyle(21);
    _filled[i+19]->SetMarkerColor(8);
    _filled[i+19]->Draw("P");//TOFEdep<->padd# profiles
    gPadSave->cd();
  }
  gPad->cd(6);
  gPad->SetGrid();
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  _filled[23]->SetMinimum(0);
  _filled[23]->SetMaximum(5);
  _filled[23]->SetMarkerStyle(21);
  _filled[23]->SetMarkerColor(8);
  _filled[23]->Draw("P");//TOFEdep<->Long_coo profiles
  gPadSave->cd();
  break;
case 5:
  gPad->Divide(1,1);
  gPad->cd(1);
  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  nentr=RunPar::getsev(0);//entries to analysis
  if(nentr>0){
    binc[1]=Float_t(RunPar::getsev(1))/nentr;//lvl1 
    binc[2]=Float_t(RunPar::getsev(2))/nentr;//TOF-multiplicity OK 
    binc[3]=Float_t(RunPar::getsev(3))/nentr;//TRK-multiplicity OK 
    binc[4]=Float_t(RunPar::getsev(4))/nentr;//particles 
    binc[5]=Float_t(RunPar::getsev(5))/nentr;//+ TRK-track 
    binc[6]=0;
    binc[7]=Float_t(RunPar::getsev(6))/nentr;// Part with TOF-beta 
    binc[8]=Float_t(RunPar::getsev(7))/nentr;//+ chi2s OK 
    binc[9]=Float_t(RunPar::getsev(8))/nentr;//+ chi2t OK 
    binc[10]=Float_t(RunPar::getsev(9))/nentr;//+ Pattern OK 
    binc[11]=Float_t(RunPar::getsev(10))/nentr;//+ BetaRange OK 
    binc[12]=0;
    binc[13]=Float_t(RunPar::getsev(11))/nentr;//part.with TRK-track
    binc[14]=Float_t(RunPar::getsev(12))/nentr;//  + TRKTrueX OK
    binc[15]=Float_t(RunPar::getsev(13))/nentr;//  + Chi2's OK
    binc[16]=Float_t(RunPar::getsev(14))/nentr;//  + AdvancedFit
    binc[17]=Float_t(RunPar::getsev(15))/nentr;//  + HalfRigAssim OK
    binc[18]=0;
    binc[19]=Float_t(RunPar::getsev(16))/nentr;//(TOFBeta & TRK)FitOK
    binc[20]=Float_t(RunPar::getsev(17))/nentr;// + Transv.matching OK
    binc[21]=Float_t(RunPar::getsev(18))/nentr;// + Longit.matching OK
  }
  xax=_filled[24]->GetXaxis();
  
  xax->SetBinLabel(1,"Lvl1-trigg");
  _filled[24]->SetBinContent(1,binc[1]);
  
  xax->SetBinLabel(2,"TofRClMult");
  _filled[24]->SetBinContent(2,binc[2]);
  
  xax->SetBinLabel(3,"TrkClMult");
  _filled[24]->SetBinContent(3,binc[3]);
  
  xax->SetBinLabel(4,"Particle");
  _filled[24]->SetBinContent(4,binc[4]);
  
  xax->SetBinLabel(5,"+ TrkTtrack");
  _filled[24]->SetBinContent(5,binc[5]);
  
  _filled[24]->SetBinContent(6,binc[6]);
  
  
  xax->SetBinLabel(7,"Part+TfBet");
  _filled[24]->SetBinContent(7,binc[7]);
  
  xax->SetBinLabel(8,"+..FitHi2Sp");
  _filled[24]->SetBinContent(8,binc[8]);
  
  xax->SetBinLabel(9,"+...Hi2Time");
  _filled[24]->SetBinContent(9,binc[9]);
  
  xax->SetBinLabel(10,"+...PattOk");
  _filled[24]->SetBinContent(10,binc[10]);
  
  xax->SetBinLabel(11,"+ |beta|<1.1");
  _filled[24]->SetBinContent(11,binc[11]);
  
  _filled[24]->SetBinContent(12,binc[12]);
  
  
  xax->SetBinLabel(13,"Part+Track");
  _filled[24]->SetBinContent(13,binc[13]);
  
  xax->SetBinLabel(14,"+ TrkTrueX");
  _filled[24]->SetBinContent(14,binc[14]);
  
  xax->SetBinLabel(15,"+ TrkFitHi2");
  _filled[24]->SetBinContent(15,binc[15]);
  
  xax->SetBinLabel(16,"+ AdvFitOk");
  _filled[24]->SetBinContent(16,binc[16]);
  
  xax->SetBinLabel(17,"+ HRigAss");
  _filled[24]->SetBinContent(17,binc[17]);
  
  _filled[24]->SetBinContent(18,binc[18]);
  
  
  xax->SetBinLabel(19,"Tof/TrkFit");
  _filled[24]->SetBinContent(19,binc[19]);
  
  xax->SetBinLabel(20,"+tranMatch");
  _filled[24]->SetBinContent(20,binc[20]);
  
  xax->SetBinLabel(21,"longMatch");
  _filled[24]->SetBinContent(21,binc[21]);
  
  _filled[24]->SetStats(kFALSE);
  _filled[24]->Draw("hbar2");  
  gPadSave->cd();
  break;
//
case 6:
  gPad->Divide(1,3);
  p1min=0;
  p1max=8;
  p2min=0;
  p2max=8;
  for(i=0;i<5;i++){
    if(i<=2)gPad->cd(i+1);
    else gPad->cd(3);
    gPad->SetGrid();
    gStyle->SetOptStat(100010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    
    if(tofscales[i]==1){
      strcpy(name,"Last 120 mins since ");
      strcpy(dat,RunPar::getdat1());
    }
    else if(tofscales[i]==2){
      strcpy(name,"Last 120 hours since ");
      strcpy(dat,RunPar::getdat2());
    }
    else if(tofscales[i]==3){
      strcpy(name,"Last 120 days since ");
      strcpy(dat,RunPar::getdat3());
    }
    
    _filled[i+25]->SetMarkerStyle(20);
    _filled[i+25]->SetMarkerSize(0.5);
    
    if(i==0){
      _filled[i+25]->SetMinimum(p1min);
      _filled[i+25]->SetMaximum(p1max);
      _filled[i+25]->SetMarkerColor(2);
    }
    if(i==1){
      _filled[i+25]->SetMinimum(p2min);
      _filled[i+25]->SetMaximum(p2max);
      _filled[i+25]->SetMarkerColor(2);
    }
    if(i>=2){
      _filled[i+25]->SetMinimum(-40);
      _filled[i+25]->SetMaximum(40);
      if(i==2){
        _filled[i+25]->SetMarkerColor(2);
      }
      else if(i==3){
        _filled[i+25]->SetMarkerColor(3);
      }
      else{
        _filled[i+25]->SetMarkerColor(4);
      }
    }
    strcat(name,dat);
    xax=_filled[i+25]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    if(i<=2){
      _filled[i+25]->Draw("P");//nLThits/nHThits/tempT vs Time
    }
    else _filled[i+25]->Draw("PSAME");//superimpose tempC/tempP vs Time
    if(i==2){
      txt->SetTextColor(2);
      txt->SetTextSize(0.05);
      strcpy(text,"SFET");
      txt->DrawText(90.,35.,text);
    }
    if(i==3){
      txt->SetTextColor(3);
      txt->DrawText(95.,35.,"SFEC");
    }
    if(i==4){
      txt->SetTextColor(4);
      txt->DrawText(100.,35.,"PMTs");
    }
    gPadSave->cd();
  }//--->endof loop
  break;
case 7:
  gPad->Divide(3,2);
  for(i=0;i<2;i++){//side-loop
  for(j=0;j<3;j++){
    gPad->cd(j+1+3*i);
    gPad->SetGrid();
    gStyle->SetOptStat(0);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[j+30+3*i]->Draw();//TOF-ch occupancy-1
    gPadSave->cd();
  }
  }
  break;
case 8:
  gPad->Divide(2,2);
  for(i=0;i<2;i++){//side-loop
  for(j=0;j<2;j++){
    gPad->cd(j+1+2*i);
    gPad->SetGrid();
    gStyle->SetOptStat(0);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(j==0)_filled[j+36+2*i]->Draw();//TOF-ch occupancy-2
    if(j==1)_filled[j+36+2*i]->Draw("colz");//TOF-ch occupancy-2
    gPadSave->cd();
  }
  }
  break;
case 9:
  gPad->Divide(3,2);
  for(i=0;i<5;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i<3){
      _filled[40+i]->SetMarkerStyle(20);
      _filled[40+i]->SetMarkerSize(0.3);
      _filled[40+i]->Draw("P");//TOF Edep/Q
    }
    else{
//       _filled[40+i]->SetStats(kFALSE);
       _filled[40+i]->Draw();//TOF-TRK mass
    }
    gPadSave->cd();
  }
  break;
case 10:
  gPad->Divide(3,2);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    if(i<4)gStyle->SetOptStat(110011);
    else gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[45+i]->Draw();//PartMom/TrkRig
    gPadSave->cd();
  }
  break;
case 11:
  gPad->Divide(2,2);
  for(i=0;i<3;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    if(i>0)gStyle->SetOptStat(110011);
    else gStyle->SetOptStat(11);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    if(i==0){
      if(tofscales[i]==1){
        strcpy(name,"Last 120 mins since ");
        strcpy(dat,RunPar::getdat1());
      }
      else if(tofscales[i]==2){
        strcpy(name,"Last 120 hours since ");
        strcpy(dat,RunPar::getdat2());
      }
      else if(tofscales[i]==3){
        strcpy(name,"Last 120 days since ");
        strcpy(dat,RunPar::getdat3());
      }
      _filled[i+51]->SetMaximum(1.);
      strcat(name,dat);
      xax=_filled[i+51]->GetXaxis();
      xax->SetTitle(name);
      xax->SetTitleSize(0.05);
      _filled[i+51]->SetMarkerStyle(20);
      _filled[i+51]->SetMarkerSize(0.5);
      _filled[i+51]->SetMarkerColor(8);
      _filled[i+51]->Draw("P");//TRD/Ftc
    }
    else{
      _filled[51+i]->Draw();//Trd rawhits, chi2
    }
    gPadSave->cd();
  }
  break;
//
  }//--->endof switch
//
}

//------------------------------------

void AMSTOFHist::Fill(AMSNtupleR *ntuple){
// 
  Bool_t cutf[20];
  Int_t etime[2],evnum,runum;
  Char_t date[30];
  static Float_t range[3],timez[3];
  static Int_t first(1),etime0(0),evnloc;
  Float_t time[3];
  static Float_t tinpp,toutp;
  Float_t Rigid(0),GRigid(0),PiRigid(0);
  Float_t trkin(0),trdin(0);
//
  RunPar::addsev(0);//<--counts inputs
  if(ntuple->nMCEventg()>0)RunPar::SetMCF1();//MC data
  else RunPar::SetMCF0();//Real data
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
    cout<<"TOF: 1st event Run/event:"<<runum<<" "<<evnum<<" date:"<<date<<" evloc="<<evnloc<<endl;
    cout<<"Ntuple-version:"<<ntuple->Version()<<endl;
    for(int i=0;i<3;i++){
      timez[i]=0;
    }
    evnloc=0;
    tinpp=-9999;
    toutp=-9999;
    RunPar::setdat1(ntuple->Time());
    RunPar::setdat2(ntuple->Time());
    RunPar::setdat3(ntuple->Time());
  }
  time[0]=(etime[0]-etime0)/60;//ev.time starting from beg.of.run(min)
  time[1]=(etime[0]-etime0)/3600;//ev.time starting from beg.of.run(hour)
  time[2]=(etime[0]-etime0)/86400;//ev.time starting from beg.of.run(day)
//
//---> Time-evolution Histogr. resets:
//
  for(int i=0;i<5;i++){
    if((time[tofscales[i]-1]-timez[tofscales[i]-1])>=toftrange[tofscales[i]-1]){
      ((TProfile*)_filled[25+i])->Reset("");
      ((TProfile*)_filled[10])->Reset("");//trk/ftc
      ((TProfile*)_filled[51])->Reset("");//trd/ftc
      timez[tofscales[i]-1]=time[tofscales[i]-1];
      time[tofscales[i]-1]+=0.001;
      if(tofscales[i]==1)RunPar::setdat1(ntuple->Time());
      if(tofscales[i]==2)RunPar::setdat2(ntuple->Time());
      if(tofscales[i]==3)RunPar::setdat3(ntuple->Time());
    }
  }
//--->short Time-range histogr. resets:
//  if((time[0]-timez[0])>=toftrange[0]){
//    timez[0]=time[0];
//    RunPar::setdat1(ntuple->Time());
//  }
//
//--->mid Time-range histogr. resets:
//  if((time[1]-timez[1])>=toftrange[1]){
//    timez[1]=time[1];
//    RunPar::setdat2(ntuple->Time());
//  }
//
//--->long Time-range histogr. resets:
//
//  if((time[2]-timez[2])>=toftrange[2]){
  //cout<<"---->Reset"<<endl;
//    ((TProfile*)_filled[25])->Reset("");
//    ((TProfile*)_filled[26])->Reset("");
//    ((TProfile*)_filled[27])->Reset("");
//    ((TProfile*)_filled[28])->Reset("");
//    ((TProfile*)_filled[29])->Reset("");
//    timez[2]=time[2];
//    RunPar::setdat3(ntuple->Time());
 // }
//<-------------
//
//---> LVL-1 params:
  Level1R *p2lev1(0);
  Bool_t LVL1OK(0);
  Int_t ECTrigFl(0);
  Int_t TOFTrigFl1(0);
  Int_t TOFTrigFl2(0);
  if(ntuple->nLevel1()>0){
    p2lev1=ntuple->pLevel1(0);
    LVL1OK=1;
    TOFTrigFl1=p2lev1->TofFlag1;
    TOFTrigFl2=p2lev1->TofFlag2;
    ECTrigFl=p2lev1->EcalFlag;
  }
  if(!LVL1OK)return;//=====> no LVL1-trig
  RunPar::addsev(1);//<--passed lvl1 check
//
//--------> accupancy/temperature behaviour study(based on TofRawSide-Obj):
//
  Int_t swid,hwid,crat,slot,nfthits,ntmhits,nhihits;
  Int_t lay,bar,sid,bin,ndyns;
  Float_t ama,amd[3];
  Float_t athra(15);//Anode amplitude threshold in adc-ch
  Float_t athrd(10);//Dynode amplitude threshold in adc-ch
  Float_t athrad(90);//Anode ampl.thr. to see Dynode signal
  Float_t monval[10];
  Float_t tinp,tout,strr,offs;
  Float_t temperT(999),temperC(999),temperP(999);
  Float_t strtms[4];
  
  Int_t ntofrs=ntuple->NTofRawSide();//total tof-raw_sides
  TofRawSideR * p2raws;//pointer to raw-side members
  for(int i=0;i<ntofrs;i++){ // <--- loop over TOF2RawSide hits
    p2raws=ntuple->pTofRawSide(i);
    swid=p2raws->swid;//LBBS
    sid=(swid%10)-1;//0,1
    lay=(swid/1000)-1;//0-3
    bar=((swid%1000)/10)-1;//0-9
    hwid=(p2raws->hwidt)/10000;//CS
    crat=hwid/10;
    slot=hwid%10;
    nfthits=p2raws->nftdc;
    ntmhits=p2raws->nstdc;
    nhihits=p2raws->nsumh;
    ama=p2raws->adca;
    ndyns=0;
    for(int i=0;i<3;i++){
      amd[i]=p2raws->adcd[i];
      if(amd[i]>athrd)ndyns+=1;
    }
    bin=bar+lay*10;
    if(ama>athra)_filled[30+3*sid]->Fill(bin,1.);//A-map
    if(ntmhits>0)_filled[31+3*sid]->Fill(bin,1.);//T-map
    if(ama>athra && ntmhits>0)_filled[32+3*sid]->Fill(bin,1.);//A+T-map
    
    if(ama>athrad && ndyns>0)_filled[36+2*sid]->Fill(bin,1.);//D-map
    if(ama>athra)_filled[37+2*sid]->Fill(bin,ntmhits);//T-occupancy
    
//    if(ntmhits>1 && ntmhits<=3)_filled[37+3*sid]->Fill(bin,1.);//T1-map
//    if(ntmhits>3)_filled[38+3*sid]->Fill(bin,1.);//T2-map
//
    monval[0]=p2raws->nstdc;
    monval[1]=p2raws->nsumh;
    monval[2]=p2raws->temp;//SFET-temp
//    monval[3]=p2raws->tempC;//SFEC-temp
//    monval[4]=p2raws->tempP;//PMT-temp
    monval[3]=10;//SFEC-temp
    monval[4]=-5;//PMT-temp
    if(swid==1041){
      for(int his=0;his<5;his++){
        if(his<2){
	  ((TProfile*)_filled[25+his])->Fill(time[tofscales[his]-1]-timez[tofscales[his]-1],monval[his],1.);
	}
	else{
	  if(monval[his]>-273 && monval[his]<999){
	    ((TProfile*)_filled[25+his])->Fill(time[tofscales[his]-1]-timez[tofscales[his]-1],monval[his],1.);
	  }
	}
      }
    }
  }// --- end of hits loop --->
//<--------
//

  Int_t ntofrcls=ntuple->NTofRawCluster();//total tof-raw_clusters(paddles)
//  Int_t ntrkrcls=ntuple->NTrRawCluster();//total trk-raw_clusters
  Int_t ntrkcls=ntuple->NTrCluster();//total trk-clusters
  _filled[0]->Fill(ntofrcls,1);
  _filled[8]->Fill(ntrkcls,1);
  if(ntofrcls>20)return;//=====> too many TOF rawclusters(too noisy event)
  RunPar::addsev(2);//<--- passed TOF-multiplisity check
  if(ntrkcls>99)return;//=====> too many TRK rawclusters(too noisy event)
  RunPar::addsev(3);//<--- passed TRK-multiplisity check
//
  if(ntuple->NParticle()==0)return;//======> no Part
  RunPar::addsev(4);//<--passed Part check
//  
  Int_t itrktr(-1),itrdtr(-1),iecshow(-1),ircring(-1),pindex(-1),pindextrd(-1);
  Int_t PartMemb[4][5]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//[ipart][imemb]
  Bool_t TRKtrPart(0),TRDtrPart(0),ECALshowPart(0),RICHringPart(0);
  for(int i=0;i<ntuple->NParticle();i++){//search for 1st Part. with trk-track
    itrktr = ntuple->Particle(i).iTrTrack();//TRKtrack index used by i-th Part. 
    itrdtr = ntuple->Particle(i).iTrdTrack();//TRDtrack index used by i-th Part.
    iecshow = ntuple->Particle(i).iEcalShower();//EcShower index used by i-th Part.
    ircring = ntuple->Particle(i).iRichRing();//RichRing index used by i-th Part.
    if(itrktr>=0)PartMemb[i][0]=1;
    if(itrdtr>=0)PartMemb[i][1]=1;
    if(iecshow>=0)PartMemb[i][2]=1;
    if(ircring>=0)PartMemb[i][3]=1;
//    if(itrktr>=0)pindex=i;//store index of 1st Part with trk-track 
//    if(itrktr>=0)break;
  }
  for(int i=0;i<ntuple->NParticle();i++){//<=============select 1st particle with needed members
    if(
        PartMemb[i][0]==1
                         || PartMemb[i][1]==1		                
//                         || PartMemb[i][2]==1		                
					      ){
      TRKtrPart    = (PartMemb[i][0]==1);
      TRDtrPart    = (PartMemb[i][1]==1);
      ECALshowPart = (PartMemb[i][2]==1);
      RICHringPart = (PartMemb[i][3]==1);
      pindex=i;//store index of 1st Part with needed members
      break;
					       }
  }
//---> fill the TRK-acceptance vs time hist:
//
  if(TOFTrigFl1>=0 && TOFTrigFl1<5){//FTC with >=3of4
    if(TRKtrPart)trkin=1.;
    ((TProfile*)_filled[10])->Fill(time[tofscales[0]-1]-timez[tofscales[0]-1],trkin,1.);
  }
//
//---> Fill the TRD-acceptance vs time and some other pars::
//
  Int_t ntrdrawh=ntuple->nTrdRawHit();
  if(TOFTrigFl1>=0 && TOFTrigFl1<5){//FTC with >=3of4
    _filled[52]->Fill(ntrdrawh,1);
    if(TRDtrPart)trdin=1.;
    ((TProfile*)_filled[51])->Fill(time[tofscales[0]-1]-timez[tofscales[0]-1],trdin,1.);
  }
  if(TRDtrPart){//<---- particle contains TRD-track
    TrdTrackR *p2trdtr = ntuple->Particle(pindex).pTrdTrack();//pointer to part-used TRD-track
    Float_t trdchi2=p2trdtr->Chi2;
    _filled[53]->Fill(trdchi2,1);
  }
//
// <======= Select track(here TRK) based Particle :
//
//  if(pindex<0)return;//======> no Part. with TRK||TRD-track
  if(!TRKtrPart)return;//======> no Part. with TRK-track
  RunPar::addsev(5);//<--- passed TRK-track check
//
  ChargeR *p2charge=ntuple->Particle(pindex).pCharge();
  Int_t TofQ=p2charge->ChargeTOF;
  Int_t TrkQ=p2charge->ChargeTracker;
  Float_t pmom=ntuple->Particle(pindex).Momentum;//particle gev/c
//
  Bool_t TOFBetaOK(0);
  Float_t tofbet(0);
  BetaR *p2beta =  ntuple->Particle(pindex).pBeta(); //pointer to TOF-beta, used by Part. with trk-track
  Int_t il,ib,id;
  Float_t xtof[4]={0,0,0,0};
  Float_t ytof[4]={0,0,0,0};
  Float_t etof[4]={0,0,0,0},ttof[4]={0,0,0,0},zctof[4]={0,0,0,0};//vs layer #
  Float_t xtofh[4]={0,0,0,0};//vs layer #
  Float_t ytofh[4]={0,0,0,0};//vs layer #
  Float_t ztofh[4]={0,0,0,0};//vs layer #
  Int_t bltof[4]={0,0,0,0},ntofp[4]={0,0,0,0};//vs layer
  Int_t bltofb[4]={0,0,0,0};//bar #(1:) with bad tof/trk matching(vs layer)
  Int_t ntofcls=ntuple->NTofCluster();//total tof-clusters
  Int_t nbetofc(0);//number of Beta-used tof-clusters
  Int_t ltof[4]={0,0,0,0},btof[4]={0,0,0,0};//vs used-cluster #
  TofRawClusterR * p2tofrc[4][2]={0,0,0,0,0,0,0,0};//pointers(il) to raw-clust members(max 2) of TofCluster
//
  if(p2beta>0){//<====  TOF-beta measurement found in particle
    RunPar::addsev(6);//<--passed TOF-beta presence check
    _filled[1]->Fill(ntofcls,1);
//
//
    tofbet=p2beta->Beta;
    nbetofc=p2beta->NTofCluster();//number of Beta-used tof-clusters
    for(int i=0;i<nbetofc;i++){//loop over beta-used tof-clusters
      xtof[i]=p2beta->pTofCluster(i)->Coo[0];
      ytof[i]=p2beta->pTofCluster(i)->Coo[1];
      ltof[i]=p2beta->pTofCluster(i)->Layer;//1:4
      btof[i]=p2beta->pTofCluster(i)->Bar;//1:8(10)
      bltof[ltof[i]-1]=btof[i];//bar(1:10) vs layer
      ntofp[ltof[i]-1]=p2beta->pTofCluster(i)->NTofRawCluster();//# cluster members(paddles)
      etof[ltof[i]-1]=p2beta->pTofCluster(i)->Edep;
      ttof[ltof[i]-1]=p2beta->pTofCluster(i)->Time;
      xtofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[0];
      ytofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[1];
      ztofh[ltof[i]-1]=p2beta->pTofCluster(i)->Coo[2];
      for(int j=0;j<ntofp[ltof[i]-1];j++)p2tofrc[ltof[i]-1][j]=p2beta->pTofCluster(i)
                                                                   ->pTofRawCluster(j);//RawCl-pointers
      _filled[2]->Fill(btof[i]+(ltof[i]-1)*10,1);
      if(ntofp[i]>0)_filled[3]->Fill(ntofp[i],1);
    }
//
    _filled[4]->Fill(p2beta->Chi2S,1);
    if(p2beta->Chi2S < 5){
      RunPar::addsev(7);//<--passed "beta chi2S" test
      _filled[5]->Fill(p2beta->Chi2,1);
      if(p2beta->Chi2 < 8){
        RunPar::addsev(8);//<--passed "beta chi2t" test
        _filled[6]->Fill(p2beta->Pattern,1);
        if(p2beta->Pattern <= 4){
	  RunPar::addsev(9);//<---- "beta pattern" test
	  if(fabs(p2beta->Beta) < 1.2){
            _filled[7]->Fill(p2beta->Beta,1);
            TOFBetaOK=1;
            RunPar::addsev(10);//<--passed "beta range" test
	  }
	}
      }
    }
//
  }// --- endof "TOF in particle" check --->
//-------------------------------------------------------------
//                                <--- check TRK-track quality:
//
  Bool_t TRKtrOK(0);

#ifndef _PGTRACK_

//
//cout<<"  itrktr="<<itrktr<<endl;
  TrTrackR *p2trktr = ntuple->Particle(pindex).pTrTrack();//pointer to TRK-track used by Part.
  Float_t trkthe(0);
  Float_t trkphi(0);
//
  if(p2trktr>0){//<---- TRKtrack in Particle presence check
    RunPar::addsev(11);//<--found TRKtrack in part
    Bool_t trkisGood=p2trktr->IsGood();//true,if track has true x-points, otherwise =false
    UInt_t trksta=p2trktr->Status;//
    Int_t id=(trksta%16384)/8192;//  bit 14
    cutf[4]=(id==0);// --> Not falseX
    id=(trksta%32768)/16384;//  bit 15
    cutf[5]=(id==0);// --> Not falseTOFX

    Int_t trkafd=p2trktr->AdvancedFitDone;
    Int_t trkgfd=p2trktr->GeaneFitDone;
    
    Int_t trkpat=p2trktr->Patternf();
    //Int_t trladd=p2trktr->Address;//tempor by VC ???
//cout<<"---> TrkAfd="<<trkafd<<" TrkIsGood="<<trkisGood<<" TrkGfd="<<trkgfd<<" C4/5="<<cutf[4]<<" "<<cutf[5]<<endl;
  
    Float_t PiRigErr=p2trktr->PiErrRig;//PiRigidity err (to 1/rig) (<0 means fit is failed)
    Float_t repirig=0;//dR/R
    if(PiRigErr>=0){
      PiRigid=p2trktr->PiRigidity;//PathInt.Rigidity
      repirig=PiRigErr*fabs(PiRigid);//abs. dR/R from JA
    }
    
    if(trkgfd!=0)GRigid=p2trktr->GRigidity;
    
    Float_t trkch2sz=p2trktr->Chi2StrLinef();//str-line chi2
    
    Float_t trkch2cms=p2trktr->Chi2WithoutMSf();//chi2 of circular-fit, MSattOff
    Float_t trkrigcms=p2trktr->RigidityWithoutMS;//rigid ....................
    
    Float_t trkch2=p2trktr->Chi2FastFitf();//fast nonl. fit
    Rigid=p2trktr->Rigidityf();//fast nonl.fit Rigidity
    Float_t RigErr=p2trktr->ErrRigidity;//err to 1/above
    Float_t rerig=RigErr*fabs(Rigid);//abs. dR/R
    Float_t trkrigms=p2trktr->RigidityMS;//fast nonl. fit, MScattOff
    Float_t trkch2ms=p2trktr->FChi2MSf();//chi2 for above
    trkthe=p2trktr->Theta;
    trkphi=p2trktr->Phi;
  
    Float_t trkch2h[2]={999.,999.}; //chi2 for 2 halves
    Float_t trkhrig[2]={0.,0.}; //2 halves rigs
    Float_t hrigass=-999;
    if(trkafd!=0){
      trkch2h[0]=p2trktr->HChi2f(0);
      trkch2h[1]=p2trktr->HChi2f(1);
      trkhrig[0]=p2trktr->HRigidityf(0);
      trkhrig[1]=p2trktr->HRigidityf(1);
      
//cout<<"    Hrig="<<trkhrig[0]<<" "<<trkhrig[1]<<" Hchi2="<<trkch2h[0]<<" "<<trkch2h[1]<<endl;
      if((trkhrig[0]+trkhrig[1])!=0)hrigass=(trkhrig[0]-trkhrig[1])/(trkhrig[0]+trkhrig[1]);
      if(hrigass>1)hrigass=0.9999;
      if(hrigass<-1)hrigass=-1;
    }
//
    if(cutf[5]){//<---- Not FalsTOFX
      RunPar::addsev(12);//<--passed "trueX" test
//
      _filled[9]->Fill(trkch2,1);//V+PG
//      _filled[10]->Fill(trkch2sz,1);//V
      _filled[11]->Fill(trkch2ms,1);//V+PG
      if(trkch2<120
                   && trkch2sz<20
                                 && trkch2ms<10000
	                                        ){//<---- chi2's check
        RunPar::addsev(13);//<--passed "chi2's" test
        if(trkafd!=0){//<---- AdvFitDone check
	  _filled[12]->Fill(hrigass,1);//V+PG
          RunPar::addsev(14);//<--passed "AdvancFitDone" test
//	  if(fabs(hrigass)<0.5){
//            _filled[13]->Fill(rerig,1);
	    ((TProfile*)_filled[13])->Fill(fabs(Rigid),rerig*100,1.);
            TRKtrOK=1;
            RunPar::addsev(15);//<--passed "HalfRigAssim" test
//	  }
	}
//
      }//--- endof "chi2's" check --->
    }//--- endof "FalsTOFX check" check --->
  }// --- endof "TRKtr in Particle" check --->
//-------------------------------------------------
#endif
//========================> PG-version:
#ifdef _PGTRACK_
//
//cout<<"  pindex="<<pindex<<endl;
  TrTrackR *p2trktr = ntuple->Particle(pindex).pTrTrack();//pointer to TRK-track used by Part.
  itrktr = ntuple->Particle(pindex).iTrTrack();//TRKtrack index used by Part(-1 if missing) 
  TrTrackR trktr=ntuple->TrTrack(itrktr);//ref to TrkTrack object used by particle
  TrTrackPar ref2MS,ref2HR;//refs to TRK-track params.objects
  Int_t fitcodMS[2]={-1,-1}; 
  Int_t fitcodHR[2]={-1,-1};
  double RigErr(0),trkch2(0),trkch2ms(0);
  Bool_t FitDone(0); 
  Bool_t HRFitDone[2]={0,0}; 
  Float_t trkhrig[2]={0.,0.}; //2 halves rigs
  Int_t TrkFitMet(1);//
// 1->Choutko,2->Geane,4->Alcaraz,5->Chicanian,6->ChicanianF,12->Simple
//
  if(p2trktr>0){//<---- TRKtrack in Particle presence check
    RunPar::addsev(11);//<--found TRKtrack in part
    for(int j=0;j<2;j++){//<---with/no MS (+10 ->MS off) || top/bot_halfs
      fitcodMS[j]=trktr.iTrTrackPar(10*j+TrkFitMet,0,1);//w/nMS,0->all_hits, 1->refit_ifnotexist
      fitcodHR[j]=trktr.iTrTrackPar(TrkFitMet,j+1,1);//wMS,top/bot_halfs,refit_ifnotexist
      if(fitcodMS[j]>0){
        ref2MS=trktr.gTrTrackPar(fitcodMS[j]);//ref.to TrTrackPar object
	if(j==0){//incl.MS
	  Rigid=ref2MS.Rigidity;
	  RigErr=ref2MS.ErrRinv;
	  trkch2=ref2MS.Chisq;
	  FitDone=ref2MS.FitDone;
	}
	if(j==1){//noMS
	  trkch2ms=ref2MS.Chisq;
	}
      }
      if(fitcodHR[j]>0){
        ref2HR=trktr.gTrTrackPar(fitcodHR[j]);//ref.to TrTrackPar object
        trkhrig[j]=ref2HR.Rigidity;
	HRFitDone[j]=ref2HR.FitDone;
      }
    }
    
    cutf[4]=true;// --> Not falseX
    cutf[5]=true;// --> Not falseTOFX
    
    Float_t hrigass=-999;
    Int_t trkafd(0);
    if(HRFitDone[0] && HRFitDone[1]){
      trkafd=1;
      if((trkhrig[0]+trkhrig[1])!=0)hrigass=(trkhrig[0]-trkhrig[1])/(trkhrig[0]+trkhrig[1]);
      if(hrigass>1)hrigass=0.9999;
      if(hrigass<-1)hrigass=-1;
    }  
    Float_t rerig=RigErr*fabs(Rigid);//abs. dR/R
    
/*
    trkafd=p2trktr->AdvancedFitDone(0);
    Float_t trkch2sz=1;//str-line chi2
    trkch2=p2trktr->Chi2FastFitf();//fast nonl. fit
    Rigid=p2trktr->Rigidityf();//fast nonl.fit Rigidity
    RigErr=0;//err to 1/above
    rerig=RigErr*fabs(Rigid);//abs. dR/R
    Float_t trkrigms=1;//fast nonl. fit, MScattOff
    trkch2ms=p2trktr->FChi2MSf();//chi2 for above
  
    hrigass=-999;
    if(trkafd!=0){
      trkhrig[0]=p2trktr->HRigidityf(0);
      trkhrig[1]=p2trktr->HRigidityf(1);
      
//cout<<"    Hrig="<<trkhrig[0]<<" "<<trkhrig[1]<<" Hchi2="<<trkch2h[0]<<" "<<trkch2h[1]<<endl;
      if((trkhrig[0]+trkhrig[1])!=0)hrigass=(trkhrig[0]-trkhrig[1])/(trkhrig[0]+trkhrig[1]);
      if(hrigass>1)hrigass=0.9999;
      if(hrigass<-1)hrigass=-1;
    }
*/
    if(cutf[5]){//<---- Not FalsTOFX
      RunPar::addsev(12);//<--passed "trueX" test
//
      _filled[9]->Fill(trkch2,1);//V+PG
//      _filled[10]->Fill(trkch2sz,1);//V
      if(TOFTrigFl1>=0 && TOFTrigFl1<5){
        ((TProfile*)_filled[10])->Fill(time[tofscales[0]-1]-timez[tofscales[0]-1],1.,1.);
      }
      
      _filled[11]->Fill(trkch2ms,1);//V+PG
      if(trkch2<120
                                 && trkch2ms<10000
	                                        ){//<---- chi2's check
        RunPar::addsev(13);//<--passed "chi2's" test
        if(trkafd!=0){//<---- AdvFitDone check
	  _filled[12]->Fill(hrigass,1);//V+PG
          RunPar::addsev(14);//<--passed "AdvancFitDone" test
//	  if(fabs(hrigass)<0.5){
//            _filled[13]->Fill(rerig,1);
	    ((TProfile*)_filled[13])->Fill(fabs(Rigid),rerig*100,1.);
            TRKtrOK=1;
            RunPar::addsev(15);//<--passed "HalfRigAssim" test
//	  }
	}
//
      }//--- endof "chi2's" check --->
    }//--- endof "FalsTOFX check" check --->
  }// --- endof "TRKtr in Particle" check --->
//-------------------------------------------------
#endif
//===========================
//
//                      <---- Check TOF-TRKtrack matching:
//
  Bool_t TOFTRKmatchOK(0);
  Int_t Ntoftrmatch(0);
  Float_t ztof[4]={0,0,0,0};
  Float_t tofcro[4][3]={0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t dx,dy,dz;
//
  if(TOFBetaOK && TRKtrOK){//<---- TOFbeta/TRKtr presence check
    RunPar::addsev(16);//<-- count matching tests 
    for(int i=0;i<4;i++){
      for(int j=0;j<3;j++){
        tofcro[i][j]=ntuple->pParticle(pindex)->TOFCoo[i][j];//TOFpln/TRKtrack cross.points
        if(j==2)ztof[i]=tofcro[i][j];//store Zcr vs Layer
      }
    }
//
    Float_t dclg[4]={999.,999.,999.,999.};
    Float_t dctr[4]={999.,999.,999.,999.};
    for(int ic=0;ic<nbetofc;ic++){//beta-used cluster loop
      il=ltof[ic]-1;//0:3
      ib=btof[ic];//1:
      dx=xtof[ic]-tofcro[il][0];
      dy=ytof[ic]-tofcro[il][1];
      if(il==0 || il==3){
        dclg[ic]=dx;
        dctr[ic]=dy;
        if(il==0)((TProfile*)_filled[14+il])->Fill(ib,dx,1.);
        if(il==3)((TProfile*)_filled[14+il])->Fill(ib,dx,1.);
      }
      else{
        dclg[ic]=dy;
        dctr[ic]=dx;
        if(il==1)((TProfile*)_filled[14+il])->Fill(ib,dy,1.);
        if(il==2)((TProfile*)_filled[14+il])->Fill(ib,dy,1.);
      }
    }
//
//
    Float_t toftc,toflc;
    cutf[6]=true;// good long tof-track matching 
    cutf[7]=true;// good trans tof-track matching 
    for(int ic=0;ic<nbetofc;ic++){//<--beta-used cluster loop
      il=ltof[ic]-1;//0:3
      ib=btof[ic];//1:
      if(ib>1 && ib<kNtofb[il]){//dx/dy-cuts for center
        toflc=9;//cm
        toftc=9;
      }
      else{//... for outer 
        toflc=1.5*9;
        toftc=2*9;
      }
//
      if(fabs(dclg[ic])>toflc){
        cutf[6]=false;//bad long. tof/trk-matching
        bltofb[il]=ib;
      }
      if(fabs(dctr[ic])>toftc){
        cutf[7]=false;//bad transv. tof/trk-matching
        bltofb[il]=ib;
      }
    }//--->endof beta-used cluster loop
    
    if(cutf[7]){//<---- Good Tof-Trk transv.matching in ALL!!! beta-used tof-layers
      RunPar::addsev(17);//<--passed "Tof-Trk transv.matching" test
      if(cutf[6]){//<---- Good Tof-Trk long.matching in ALL!!! beta-used tof-layers  
//
        TOFTRKmatchOK=1;
        RunPar::addsev(18);//<--passed "Tof-Trk long.matching" test
      }
//
    }
  }//--- endof "TOFbeta/TRKtr-presence" check --->
//---------------------------------------------------------
//
//                <-------- check TOFResponceUniformity:
//
  Int_t tofmsk[4]={1,1,1,1};//to keep layer mask after removing highest layer
  Int_t lemx(0),ntrtof(0);
  Float_t edmx(0),tofedn[4]={0.,0.,0.,0.};//TOF Edep norm to pass and MIP
  Float_t tofedpn[4]={0.,0.,0.,0.};//TOF Edep norm to pass
  Float_t tofcs[2]={0,0};//Tof/track cross. cos12,cos34
  Float_t rrr,betnor,etrtof(0),absbet,clong[4]={0,0,0,0};
  Float_t etrtofb(0);
  absbet=fabs(tofbet);
  Float_t betpow=1.83;//Pr  
  if(absbet<0.94)betnor=pow(absbet,betpow)
                       /pow(Float_t(0.94),betpow);//norm.factor to MIP(prot)
  else betnor=1;
  Bool_t TOFTRKnormOK(0);
//
  if(TOFTRKmatchOK==1 && nbetofc>=3){
//---> calc. trk-crossing cos with top/bot TOF's

    int ltop(0),lbot(0);
    if(bltof[0]>0&&bltof[1]>0){// 2/?(top/bot)
      dx=tofcro[0][0]-tofcro[1][0];
      dy=tofcro[0][1]-tofcro[1][1];
      dz=tofcro[0][2]-tofcro[1][2];
      tofcs[0]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L12 cos
      ltop=2;
    }
    if(bltof[2]>0&&bltof[3]>0){// ?/2
      dx=tofcro[2][0]-tofcro[3][0];
      dy=tofcro[2][1]-tofcro[3][1];
      dz=tofcro[2][2]-tofcro[3][2];
      tofcs[1]=dz/sqrt(dx*dx+dy*dy+dz*dz);//L34 cos
      lbot=3;
    }
    if(tofcs[0]==0){//top:<=1hit
      if(bltof[0]>0)ltop=1;
      if(bltof[1]>0)ltop=2;
    }
    if(tofcs[1]==0){//bot:<=1hit
      if(bltof[2]>0)lbot=3;
      if(bltof[3]>0)lbot=4;
    }
    if(ltop>0 && lbot>0){// 1/1,1/2,2/1
      dx=tofcro[ltop-1][0]-tofcro[lbot-1][0];
      dy=tofcro[ltop-1][1]-tofcro[lbot-1][1];
      dz=tofcro[ltop-1][2]-tofcro[lbot-1][2];
      rrr=dz/sqrt(dx*dx+dy*dy+dz*dz);//top/bot cos
      if(tofcs[0]==0)tofcs[0]=rrr;
      if(tofcs[1]==0)tofcs[1]=rrr;
    }
//---> create tof-EdepNorm arrays
    for(int il=0;il<4;il++){//create some tof-EdepNorm arrays
      int itb=il/2;
      if(bltof[il]>0){
        tofedpn[il]=etof[il]*tofcs[itb];//norm to norm. inc.
        tofedn[il]=etof[il]*tofcs[itb]*betnor;//norm to norm.inc. and MIP area
        ib=bltof[il];//1:10
        id=100*(il+1)+ib;//tof id (LBB)
        if(il==0 || il==3)clong[il]=tofcro[il][0];
        else clong[il]=tofcro[il][1];
        if(tofedn[il]>edmx){
          edmx=tofedn[il];
	  lemx=il;
        }
      }
      else tofmsk[il]=0;
    }
    if(edmx>0){
      tofmsk[lemx]=0;//mark Edep-highest layer for trunc.mean calc.
    }
//---> calc. trunc. Edep(pass/mip_norm) to select mip-area:
    for(int il=0;il<4;il++){
      if(ntofp[il]==1 && tofedn[il]>0 && tofmsk[il]==1){
        ntrtof+=1;
        etrtof+=tofedn[il];//beta-independent
	etrtofb+=tofedpn[il];//beta-dependent
      }
    }
//---> fill Edep-profiles:
    if(ntrtof>0){
      etrtof/=ntrtof;
      etrtofb/=ntrtof;
      _filled[18]->Fill(etrtof,1);
      if(etrtof<4){//select MIP-area
        for(int il=0;il<4;il++){//fill Edep-profiles
          ib=bltof[il];//1:10
          id=100*(il+1)+ib;//tof id (LBB)
	  if(tofedn[il]>0 && ntofp[il]==1 && tofmsk[il]==1){//use 1-member clusters, not spike
//	    ((TProfile*)_filled[19+il])->Fill(ib,etrtof,1.);//profiles Edep vs TOF-pad
	    ((TProfile*)_filled[19+il])->Fill(ib,tofedn[il],1.);//profiles Edep vs TOF-pad
            if(id==204)((TProfile*)_filled[23])->Fill(clong[il],tofedn[il],1.);
          }

        }
      }
    }
//
    TOFTRKnormOK=1;
    RunPar::addsev(19);//<--good TOF/TRK-normalization
  }//<--- endof TOFUnif.block
//---
  Float_t pmass(0),massq;
  if(TOFTRKnormOK){
    if(tofbet>0)_filled[40]->Fill(tofbet,etrtofb);//Etrunc(beta-depend) vs beta>0
    if(tofbet<0)_filled[41]->Fill(-tofbet,etrtofb);//Etrunc(beta-depend) vs beta<0
    _filled[42]->Fill(TofQ,sqrt(etrtof));//Sqrt(Etrunc,beta-independ) vs Z
    massq=pmom*pmom*(1-tofbet*tofbet)/tofbet/tofbet;
    if(fabs(tofbet)<0.9){
      pmass=sqrt(massq);
      if(tofbet>0)_filled[43]->Fill(pmass,1);
      if(tofbet<0)_filled[44]->Fill(pmass,1);
    }
  }
//--- 
  if(TOFBetaOK && TRKtrOK){
    if(tofbet>0)_filled[45]->Fill(pmom,1);
    if(tofbet<0)_filled[46]->Fill(pmom,1);
    if(tofbet>0){
      if(fabs(Rigid)>0)_filled[47]->Fill(1/Rigid,1);
    }
    if(tofbet<0){
      if(fabs(Rigid)>0)_filled[48]->Fill(1/Rigid,1);
    }
    if(pmom>0)_filled[49]->Fill(tofbet,1);
    if(pmom<0)_filled[50]->Fill(tofbet,1);
  }
//---
  evnloc+=1;
// 
}


