//  $Id: AMSECALHist.cxx,v 1.6 2008/07/31 14:49:19 choumilo Exp $
//   E.Choumilov v1.0 12.03.2008 
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSECALHist.h"

const Int_t kNants=8;//ANTI sectors
const Int_t kNtofl=4;//TOF layers
const Int_t kNtofb[4]={8,8,10,8};//TOF bars per layer
const Int_t kNsupl=9;//ECAL-superlayesr
const Int_t kNpmts=36;//ECAL-pms/supl
//--------------------------------
class EcalRunPar{ 
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
  static Bool_t patbcheck(Int_t sl, Int_t pm, UShort_t ecpatt[6][3]){
//   sl=0-8, pm=0-35, BUT "IN TRIG-PATT" SL-RANGE IS 1-6
    Int_t slp,word,bit;
    word=pm/16;
    bit=pm%16;
    slp=sl-1;
    return (sl>0 && sl<7 && pm>=0 && pm<36 && (ecpatt[slp][word]&(1<<bit)) != 0);
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
  Int_t EcalRunPar::evsel[80];//events, selected by particular cuts
//
  Bool_t EcalRunPar::mcdata;
//
  Char_t EcalRunPar::dat1[30];
  Char_t EcalRunPar::dat2[30];
  Char_t EcalRunPar::dat3[30];
//
//------------------------------------
//
void AMSECALHist::Book(){ 
  EcalRunPar::init();//clear counters
  EcalRunPar::SetMCF0();//set def(real data)
//
  AddSet("Ecal Y(bend_plane)-Cells Accupancy");//set-0
  
  _filled.push_back(new TH2F("emch0","HighGainChannelOccupancy[Y(bend)-proj]",73,0.5,73.5,19,0.5,19.5));
  _filled[_filled.size()-1]->SetXTitle("Cell number(1-72)");
  _filled[_filled.size()-1]->SetYTitle("Plane number(1-18)->top2bot ");
  
  _filled.push_back(new TH2F("emch1"," LowGainChannelOccupancy[Y(bend)-proj]",73,0.5,73.5,19,0.5,19.5));
  _filled[_filled.size()-1]->SetXTitle("Cell number(1-72)");
  _filled[_filled.size()-1]->SetYTitle("Plane number(1-18)->top2bot");
  
  _filled.push_back(new TH2F("emch2","DynodeChannelOccupancy[Y(bend)-proj]",37,0.5,37.5,10,0.5,10.5));
  _filled[_filled.size()-1]->SetXTitle("PMT number(1-36)");
  _filled[_filled.size()-1]->SetYTitle("SLayer number(1-9)->top2bot");
  
//
  AddSet("Ecal X(not_bend_plane)-Cells Accupancy");//set-1
  
  _filled.push_back(new TH2F("emch3","HighGainChannelOccupancy[X(nonbend)-proj]",73,0.5,73.5,19,0.5,19.5));
  _filled[_filled.size()-1]->SetXTitle("Cell number(1-72)");
  _filled[_filled.size()-1]->SetYTitle("Plane number(1-18)->top2bot");
  
  _filled.push_back(new TH2F("emch4","LowGainChannelOccupancy[X(nonbend)-proj]",73,0.5,73.5,19,0.5,19.5));
  _filled[_filled.size()-1]->SetXTitle("Cell number(1-72)");
  _filled[_filled.size()-1]->SetYTitle("Plane number(1-18)->top2bot");
  
  _filled.push_back(new TH2F("emch5","DynodeChannelOccupancy[X(nonbend)-proj]",37,0.5,37.5,10,0.5,10.5));
  _filled[_filled.size()-1]->SetXTitle("PMT number(1-36)");
  _filled[_filled.size()-1]->SetYTitle("SLayer number(1-9)->top2bot");
  
//
  AddSet("Ecal Dynode Trigger-Cells Accupancy");//set-2
  
  _filled.push_back(new TH2F("emch6","DynodeChanOccupancy(HiAmpl,TrigPatt-bit ON)",37,0.5,37.5,10,0.5,10.5));
  _filled[_filled.size()-1]->SetXTitle("PMT number(1-36)");
  _filled[_filled.size()-1]->SetYTitle("SLayer number(1-9)->top2bot");
  
  _filled.push_back(new TH2F("emch7","DynodeChanOccupancy(HiAmpl,TrigPatt-bit OFF)",37,0.5,37.5,10,0.5,10.5));
  _filled[_filled.size()-1]->SetXTitle("PMT number(1-36)");
  _filled[_filled.size()-1]->SetYTitle("SLayer number(1-9)->top2bot");
  
//
  AddSet("EcalTotalHits time evolution");//set-3
  
  _filled.push_back(new TProfile("emch8","TotalHits vs Time",120,0.,emctrange[0],0,1000.));
  _filled[_filled.size()-1]->SetYTitle("Hits");
  _filled.push_back(new TProfile("emch9","TotalHits vs Time",120,0.,emctrange[1],0,1000.));
  _filled[_filled.size()-1]->SetYTitle("Hits");
  _filled.push_back(new TProfile("emch10","TotalHits vs Time",120,0.,emctrange[2],0,1000.));
  _filled[_filled.size()-1]->SetYTitle("Hits");
  
  AddSet("DynodeTriggerEfficiency");//set-4
  
  _filled.push_back(new TProfile("emch11","TrgPattBitON-efficiency vs DynAmpl(SL=X2)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
  
  _filled.push_back(new TProfile("emch12","TrgPattBitON-efficiency vs DynAmpl(SL=Y3)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
  
  _filled.push_back(new TProfile("emch13","TrgPattBitON-efficiency vs DynAmpl(SL=X4)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
  
  _filled.push_back(new TProfile("emch14","TrgPattBitON-efficiency vs DynAmpl(SL=Y5)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
  
  _filled.push_back(new TProfile("emch15","TrgPattBitON-efficiency vs DynAmpl(SL=X6)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
  
  _filled.push_back(new TProfile("emch16","TrgPattBitON-efficiency vs DynAmpl(SL=Y7)",100,0.,250.,0.,1.1));
  _filled[_filled.size()-1]->SetXTitle("DynAmpl[adc-chan]");
  _filled[_filled.size()-1]->SetYTitle("Eff");
//  
//-----------  
//  
  AddSet("ECAL spectra");//set-5
  
  _filled.push_back(new TH1F("emch17","EcalPixHg(all) amplitude",100,0.,100.));
  _filled[_filled.size()-1]->SetXTitle("ADC channels");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("emch18","EcalPixLg(all) amplitude",100,0.,100.));
  _filled[_filled.size()-1]->SetXTitle("ADC channels");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("emch19","EcalDyn(all) amplitude",100,0.,250.));
  _filled[_filled.size()-1]->SetXTitle("ADC channels");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("emch20","EcalHits per event",100,0.,100.));
  _filled[_filled.size()-1]->SetXTitle("Number of hits above threshold");
  _filled[_filled.size()-1]->SetFillColor(44);
  
//  
//-----------  
//  
  AddSet("Ecal-Events Rates");//set-6
  
  _filled.push_back(new TH1F("emch21","Ecal-events rates",9,1.,10.));
  _filled[_filled.size()-1]->SetYTitle("number of events");
  _filled[_filled.size()-1]->SetFillColor(44);
  // 
}


void AMSECALHist::ShowSet(Int_t Set){
  TAxis *xax;
  TText *txt=new TText();
//
  gPad->Clear();
  gStyle->SetPalette(1,0);
  TVirtualPad * gPadSave = gPad;
  int i,j,k;
  Char_t name[60],dat[30];
  Char_t text[100];
//
  switch(Set){
case 0:
  gPad->Divide(1,3);
  for(j=0;j<3;j++){//hist#
    gPad->cd(j+1);
    gPad->SetGrid();
//    gStyle->SetPalette(1,0);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[j]->SetStats(kFALSE);
    _filled[j]->Draw("colz");//ECAL-Ycells accupancy
    gPadSave->cd();
  }
  break;
case 1:
  gPad->Divide(1,3);
    for(j=0;j<3;j++){//hist#
    gPad->cd(j+1);
    gPad->SetGrid();
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[3+j]->SetStats(kFALSE);
    _filled[3+j]->Draw("colz");//ECAL-Xcells accupancy
    gPadSave->cd();
  }
  break;
case 2:
  gPad->Divide(1,2);
    for(j=0;j<2;j++){//hist#
    gPad->cd(j+1);
    gPad->SetGrid();
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[6+j]->SetStats(kFALSE);
    _filled[6+j]->Draw("colz");//ECAL-Xcells accupancy
    gPadSave->cd();
  }
  break;
case 3:
  gPad->Divide(1,3);
  for(i=0;i<3;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(110010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+8]->SetMinimum(0);
    _filled[i+8]->SetMaximum(101);
    _filled[i+8]->SetMarkerStyle(20);
    _filled[i+8]->SetMarkerColor(2);
    _filled[i+8]->SetMarkerSize(0.5);
    if(i==0){
      strcpy(name,"Last 120 mins since ");
      strcpy(dat,Lvl1Pars::getdat1());
    }
    if(i==1){
      strcpy(name,"Last 120 hours since ");
      strcpy(dat,Lvl1Pars::getdat2());
    }
    if(i==2){
      strcpy(name,"Last 120 days since ");
      strcpy(dat,Lvl1Pars::getdat3());
    }
    strcat(name,dat);
    xax=_filled[i+8]->GetXaxis();
    xax->SetTitle(name);
    xax->SetTitleSize(0.05);
    _filled[i+8]->Draw("P");//Nhits total 
    gPadSave->cd();
  }
  break;
case 4:
  gPad->Divide(2,3);
  for(i=0;i<6;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(110010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+11]->SetMinimum(-0.01);
    _filled[i+11]->SetMaximum(1.1);
    _filled[i+11]->SetMarkerStyle(20);
    _filled[i+11]->SetMarkerColor(2);
    _filled[i+11]->SetMarkerSize(0.5);
    _filled[i+11]->Draw("P");// sl-by-sl Trig-eff 
    gPadSave->cd();
  }
  break;
case 5:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
    gPad->SetGrid();
    gStyle->SetOptStat(110010);
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+17]->Draw();// Spectra 
    gPadSave->cd();
  }
  break;
case 6:
  gPad->Divide(1,1);
  gPad->cd(1);
//  gPad->SetLogx(gAMSDisplay->IsLogX());
  gPad->SetLogx(1);
  gPad->SetLogy(gAMSDisplay->IsLogY());
  gPad->SetLogz(gAMSDisplay->IsLogZ());
  gPad->SetGrid();
  xax=_filled[21]->GetXaxis();
  xax->SetLabelSize(0.04);
  xax->SetBinLabel(1,"Entries");
  _filled[21]->SetBinContent(1,EcalRunPar::getsev(0));
  xax->SetBinLabel(2,"Lvl1OK");
  _filled[21]->SetBinContent(2,EcalRunPar::getsev(1));
  xax->SetBinLabel(3,"FTE");
  _filled[21]->SetBinContent(3,EcalRunPar::getsev(2));
  xax->SetBinLabel(4,"FTE+FTC|Z");
  _filled[21]->SetBinContent(4,EcalRunPar::getsev(3));
  xax->SetBinLabel(5,"FTE-FTC|Z");
  _filled[21]->SetBinContent(5,EcalRunPar::getsev(4));
  xax->SetBinLabel(6,"EcActive");
  _filled[21]->SetBinContent(6,EcalRunPar::getsev(5));
  xax->SetBinLabel(7,"+Particle");
  _filled[21]->SetBinContent(7,EcalRunPar::getsev(6));
   xax->SetBinLabel(8,"+TrkTrack");
  _filled[21]->SetBinContent(8,EcalRunPar::getsev(7));
 xax->SetBinLabel(9,"+TrackCros");
  _filled[21]->SetBinContent(9,EcalRunPar::getsev(8));
  
  _filled[21]->SetStats(kFALSE);
  _filled[21]->Draw("hbar2");//Ecal-events Rates  
  gPadSave->cd();
  break;
//
  }//--->endof switch
}



void AMSECALHist::Fill(AMSNtupleR *ntuple){ 
  Bool_t cutf[20];
  Int_t etime[2],evnum,runum;
  Char_t date[30];
  static Float_t range[3],timez[3];
  static Int_t first(1),etime0(0),evnloc;
  Float_t time[3];
  static Float_t tinpp,toutp;
  UShort_t ecpatt[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t dynamap[kNsupl][kNpmts];//event dyn-ampls
  Float_t dynamapA[kNsupl][kNpmts];//event dyn-ampls made from AnodeH
  Int_t i,j,k,ih;
//
  EcalRunPar::addsev(0);//<--counts inputs
  if(ntuple->nMCEventg()>0)EcalRunPar::SetMCF1();//MC data
  else EcalRunPar::SetMCF0();//Real data
//
  etime[0]=ntuple->fHeader.Time[0];//unix-time(sec)
  etime[1]=ntuple->fHeader.Time[1];
  evnum=ntuple->fHeader.Event;
  runum=ntuple->fHeader.Run;
  strcpy(date,ntuple->GetTime());
//
  if(first==1){
    first=0;
    etime0=etime[0];
    cout<<"ECAL: 1st event Run/event:"<<runum<<" "<<evnum<<" date:"<<date<<" evloc="<<evnloc<<endl;
    cout<<"Ntuple-version:"<<ntuple->Version()<<endl;
    for(int i=0;i<3;i++){
      timez[i]=0;
    }
    evnloc=0;
    tinpp=-9999;
    toutp=-9999;
    EcalRunPar::setdat1(ntuple->GetTime());
    EcalRunPar::setdat2(ntuple->GetTime());
    EcalRunPar::setdat3(ntuple->GetTime());
  }
  time[0]=(etime[0]-etime0)/60;//ev.time starting from beg.of.run(min)
  time[1]=(etime[0]-etime0)/3600;//ev.time starting from beg.of.run(hour)
  time[2]=(etime[0]-etime0)/86400;//ev.time starting from beg.of.run(day)
//
//---> LVL-1 params:
//
  Level1R *p2lev1(0);
  Bool_t LVL1OK(0);
  Int_t ECTrigFl(0);
  Int_t TOFTrigFl1(0);
  Int_t TOFTrigFl2(0);
  Int_t etflg(0),ewflg(0);
  Bool_t ftc(0),ftz(0),bz(0),fte(0),glft(0);
  Int_t physbpat(0),membpat(0);
//
  if(ntuple->nLevel1()>0){
    p2lev1=ntuple->pLevel1(0);
    LVL1OK=1;
    TOFTrigFl1=p2lev1->TofFlag1;
    TOFTrigFl2=p2lev1->TofFlag2;
    ECTrigFl=p2lev1->EcalFlag;
    etflg=ECTrigFl/10;//ECEtot(multiplicity) flag
    ewflg=ECTrigFl%10;//ECAngle(width) flag
    membpat=p2lev1->JMembPatt;
    physbpat=p2lev1->PhysBPatt;
    ftc=((membpat&1)>0);//Z>=1
    ftz=((membpat&1<<5)>0);//SlowZ>=2
    fte=((membpat&1<<6)>0);//ec-FT
    bz= ((membpat&1<<9)>0);//Z>=2(when ftc)
    glft=(ftc||ftz||fte);//globFT
  }
  if(!(LVL1OK && glft))return;//=====> no LVL1(glFT)-trig
  EcalRunPar::addsev(1);//<--passed lvl1 check
  if(fte)EcalRunPar::addsev(2);//<--fte
  if(fte && (ftc||ftz))EcalRunPar::addsev(3);//<-- ftcz + fte
  if(fte && !(ftc||ftz))EcalRunPar::addsev(4);//<-- !ftcz + fte
//
  for(i=0;i<6;i++){//read EC-trig pattern
    for(j=0;j<3;j++){
      ecpatt[i][j]=ntuple->pLevel1(0)->EcalPatt[i][j];
    }
  }
//
//--------> ECAL-hit check :
//
  Int_t nechts=ntuple->NEcalHit();//total ecal-hits
  if(nechts <= 1 && ECTrigFl==0)return;//no ECAL activity
  EcalRunPar::addsev(5);//<--passed activity check
//  
  UInt_t stat;
  Int_t swid,plane,proj,cell,pm,sl;
  Int_t nhtot(0);
  Float_t adc[3],coo[3],ped[3],edep;
  Float_t tinp,tout;
  Float_t temperT(999),temperC(999),temperP(999);
  Float_t ahthr0=3;//<--- anode(hig) hit-threshold(adc-ch) 
  Float_t ahthr1=3;//<--- anode(low) hit-threshold(adc-ch) 
  Float_t dhthr=3;//<--- dynodehit-threshold(adc-ch) 
  Float_t trgthr[9]={0.,50.,50.,50.,50.,50.,50.,0.,0.};//<--- trig-threshold(vs SL adc-ch)
// 
  EcalHitR * p2echt;//pointer to hit members
  for(i=0;i<kNsupl;i++){
    for(j=0;j<kNpmts;j++){
      dynamap[i][j]=0;
      dynamapA[i][j]=0;
    }
  }
  for(ih=0;ih<nechts;ih++){ // <--- loop over Ecal-hits
    p2echt=ntuple->pEcalHit(ih);
    stat=p2echt->Status;
    swid=p2echt->Idsoft;//SPPC=SuperLayer/PM/subCell  1:9/1:36/1:4
    sl=swid/1000;//1-9
    pm=((swid/10)%100);//1-36
    plane=p2echt->Plane;//(0,...17)
    cell=p2echt->Cell;//(0,...71)
    proj=p2echt->Proj;//(0-x,1-y)
    edep=p2echt->Edep;//mev
    for(i=0;i<3;i++){
      adc[i]=p2echt->ADC[i];
      coo[i]=p2echt->Coo[i];
      ped[i]=p2echt->Ped[i];
    }
    if(adc[0]>ahthr0)nhtot+=1;
    dynamap[sl-1][pm-1]=adc[2];//save Dyn-ampl
    if(adc[0]>ahthr0)dynamapA[sl-1][pm-1]+=adc[0];//same from AnodeH
    if(proj==1){
      if(adc[0]>ahthr0)_filled[0]->Fill(cell+1,plane+1);//Y-pr, hg
      if(adc[1]>ahthr1)_filled[1]->Fill(cell+1,plane+1);//Y-pr, lg
    }
    if(proj==0){
      if(adc[0]>ahthr0)_filled[3]->Fill(cell+1,plane+1);//X-pr, hg
      if(adc[1]>ahthr1)_filled[4]->Fill(cell+1,plane+1);//X-pr, lg
    }
    if(adc[0]>0)_filled[17]->Fill(adc[0],1);
    if(adc[1]>0)_filled[18]->Fill(adc[1],1);
  }//--->endof ECAL-hits loop
//
  _filled[20]->Fill(nhtot,1);//Nhits total
//
  for(i=0;i<kNsupl;i++){//create dyn-occupancy(all) hist
    for(j=0;j<kNpmts;j++){
      if(dynamap[i][j]>0)_filled[19]->Fill(dynamap[i][j],1);
      if(dynamap[i][j]>dhthr){
        if((i+1)%2==1)_filled[2]->Fill(j+1,i+1);//Y-proj
        if((i+1)%2==0)_filled[5]->Fill(j+1,i+1);//X-proj
      }
    }
  }
//
//---> prepare/create dyn-occupancy(when trig-patt bit on/off) hist:
  Int_t word,bit,slabs;
  if(fte){
    for(int sl=0;sl<6;sl++){
      slabs=sl+2-1;// 0,1,...8 counting
      for(int pm=0;pm<36;pm++){
        word=pm/16;
        bit=pm%16;
        if((ecpatt[sl][word]&(1<<bit)) >0 && dynamap[slabs][pm]>trgthr[slabs])_filled[6]->Fill(pm+1,slabs+1);
        if((ecpatt[sl][word]&(1<<bit))==0 && dynamap[slabs][pm]>trgthr[slabs])_filled[7]->Fill(pm+1,slabs+1);
      }
    }
//---> dyn-trig eff (when patt-bit on) vs Adyn
    for(int sl=1;sl<7;sl++){//abs SL-loop for "in trig" layers
      for(int pm=0;pm<36;pm++){
        ((TProfile*)_filled[11+sl-1])->Fill(dynamap[sl][pm],(EcalRunPar::patbcheck(sl,pm,ecpatt)?1:0),1.);
      }
    }
//
  }
//
//--------------> Time-evolution params business:
//
//--->short Time-range histogr. resets/fill:
  if((time[0]-timez[0])>=emctrange[0]){
  //cout<<"---->Reset"<<endl;
    ((TProfile*)_filled[8])->Reset("");
    timez[0]=time[0];
    EcalRunPar::setdat1(ntuple->GetTime());
  }
  ((TProfile*)_filled[8])->Fill(time[0]-timez[0],nechts,1.);
//
//--->mid Time-range histogr. resets/fill:
  if((time[1]-timez[1])>=emctrange[1]){
    ((TProfile*)_filled[9])->Reset("");
    timez[1]=time[1];
    EcalRunPar::setdat2(ntuple->GetTime());
  }
  ((TProfile*)_filled[9])->Fill(time[1]-timez[1],nechts,1.);
//
//--->long Time-range histogr. resets//fill:
//
  if((time[2]-timez[2])>=emctrange[2]){
    ((TProfile*)_filled[10])->Reset("");
    timez[2]=time[2];
    EcalRunPar::setdat3(ntuple->GetTime());
  }
  ((TProfile*)_filled[10])->Fill(time[2]-timez[2],nechts,1.);
//
//----------------> Correlations with TRK-track:
//
  if(ntuple->NParticle()==0)return;
  EcalRunPar::addsev(6);//<--passed Part check
//  
  Int_t itrktr(-1),itrdtr(-1),pindex(-1);
  for(int i=0;i<ntuple->NParticle();i++){//search for 1st Part. with trk-track
    itrktr = ntuple->Particle(i).iTrTrack(); 
    itrdtr = ntuple->Particle(i).iTrdTrack();
    if(itrktr>=0)pindex=i; 
    if(itrktr>=0)break;
  }
  if(pindex<0)return;
  EcalRunPar::addsev(7);//<--- passed TRK-track check
//
  Float_t eccros[3][3];//[entr/cent/exit][x/y/z]
  Float_t pmom;
  pmom=ntuple->pParticle(pindex)->Momentum;
  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      eccros[i][j]=ntuple->pParticle(pindex)->EcalCoo[i][j];//[front/mid/back][x,y,z]
    }
  }
  Bool_t crosok(true);
  for(i=0;i<3;i+=2){//front/back
    for(j=0;j<2;j++){//x,y
      if(fabs(eccros[i][j])>65.5)crosok=false;
    }
  }
  if(!crosok)return;
  EcalRunPar::addsev(8);//<--- passed TRK-track/EC crossing test
//
//<---------------
//
}


