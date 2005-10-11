//  $Id: AMSTOFHist.cxx,v 1.17 2005/10/11 12:34:13 choumilo Exp $
// v1.0 E.Choumilov, 12.05.2005
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
//--------------------------------
class RunPar{ 
//
private:
//
  static Int_t evsel[80];//events, passed particular cuts
//
  static Bool_t mcdata;
//
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
};
//------------------------------------
  Int_t RunPar::evsel[80];//events, selected by particular cuts
//
  Bool_t RunPar::mcdata;
//
//------------------------------------

void AMSTOFHist::Book(){ 
  RunPar::init();//clear counters
  RunPar::SetMCF0();//set def(real data)
//
  AddSet("TOFMultiplicity");
  
  _filled.push_back(new TH1F("tofh0","TOF:TotalPaddles",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Paddles per event");
  _filled[_filled.size()-1]->SetFillColor(41);
  
  _filled.push_back(new TH1F("tofh1","TOF:TotalClusters(WhenBetaFound)",40,0.,40.));
  _filled[_filled.size()-1]->SetXTitle("Clusters per event");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("tofh2","TOF:PaddleNumber(UsedByBeta)",40,1.,41.));
  _filled[_filled.size()-1]->SetXTitle("Npaddles+10*(Layer-1)");
  _filled[_filled.size()-1]->SetFillColor(41);
  
  _filled.push_back(new TH1F("tofh3","TOF:Paddles/cluster(UsedByBeta)",3,0.,3.));
  _filled[_filled.size()-1]->SetXTitle("Npaddles");
  _filled[_filled.size()-1]->SetFillColor(44);
//  
  AddSet("TofBetaParameters");
  
  _filled.push_back(new TH1F("tofh4","TofBeta",50,-2.5,2.5));
  _filled[_filled.size()-1]->SetXTitle("velocity/c");
  _filled[_filled.size()-1]->SetFillColor(41);
  
  _filled.push_back(new TH1F("tofh5","TofBetaChi2time",50,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("time-fit chi2");
  _filled[_filled.size()-1]->SetFillColor(44);
  
  _filled.push_back(new TH1F("tofh6","TofBetaChi2space",50,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("space-fit chi2");
  _filled[_filled.size()-1]->SetFillColor(41);
  
  _filled.push_back(new TH1F("tofh7","TofBetaLayerPattern",10,0.,10.));
  _filled[_filled.size()-1]->SetXTitle("used layers: 0->all4,1:4->miss.layer#,etc");
  _filled[_filled.size()-1]->SetFillColor(44);
//  
  AddSet("TrkTrackParameters");

  _filled.push_back(new TH1F("tofh8","TRKChi2(FastFit)",50,0.,50.));
  _filled[_filled.size()-1]->SetXTitle("Chi2");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TH1F("togh9","TRKChi2sz",50,0.,50.));
  _filled[_filled.size()-1]->SetXTitle("chi2");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TH1F("tofh10","TRKChi2(FastFit,NoMScat)",80,0.,400.));
  _filled[_filled.size()-1]->SetXTitle("chi2");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TH1F("tofh11","TRKHalfRigAss",50,-1.,1.));
  _filled[_filled.size()-1]->SetXTitle("ass=r1-r2/r1+r2");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TH1F("tofh12","TRK dR/R(FastFit,NoMScat)",50,0.,1.));
  _filled[_filled.size()-1]->SetXTitle("RigidityErr/Rigidity");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TH1F("tofh13","TRK dR/R(JA)",50,0.,1.));
  _filled[_filled.size()-1]->SetXTitle("RigidityErr/Rigidity");
  _filled[_filled.size()-1]->SetFillColor(44);
//  
  AddSet("LongitTofTrkMismatch");

  _filled.push_back(new TProfile("tofh14","LongTofCoord-TrkCross, Layer1",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TProfile("tofh15","LongTofCoord-TrkCross, Layer2",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TProfile("tofh16","LongTofCoord-TrkCross, Layer3",10,1,11,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
  _filled[_filled.size()-1]->SetFillColor(41);
  _filled.push_back(new TProfile("tofh17","LongTofCoord-TrkCross, Layer4",8,1,9,-10,10));
  _filled[_filled.size()-1]->SetXTitle("TofPadNumber");
  _filled[_filled.size()-1]->SetYTitle("AverageMismatch(cm)");
  _filled[_filled.size()-1]->SetFillColor(41);
}
//------------------------------------

void AMSTOFHist::ShowSet(Int_t Set){
  gPad->Clear();
  TVirtualPad * gPadSave = gPad;
  int i;
  switch(Set){
case 0:
  gPad->Divide(2,2);
  for(i=0;i<4;i++){
    gPad->cd(i+1);
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
    gPad->SetLogx(gAMSDisplay->IsLogX());
    gPad->SetLogy(gAMSDisplay->IsLogY());
    gPad->SetLogz(gAMSDisplay->IsLogZ());
    _filled[i+8]->Draw();//TRK-tracks parameters
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
    _filled[i+14]->SetMinimum(-2.5);
    _filled[i+14]->SetMaximum(2.5);
    _filled[i+14]->SetMarkerStyle(21);
    _filled[i+14]->SetMarkerColor(2);
    _filled[i+14]->Draw("P");//TOF-TRK matching pars
    gPadSave->cd();
  }
//
  }
//
}

//------------------------------------

void AMSTOFHist::Fill(AMSNtupleR *ntuple){
// 
  Bool_t cutf[20];
//
  RunPar::addsev(0);//<--counts inputs
  if(ntuple->nMCEventg()>0)RunPar::SetMCF1();//MC data
  else RunPar::SetMCF0();//Real data
//
//---> LVL-1 params:
  Level1R *p2lev1(0);
  Bool_t LVL1OK(0);
  Int_t ECTrigFl(0);
  Int_t TOFTrigFl(0);
  if(ntuple->nLevel1()>0){
    p2lev1=ntuple->pLevel1(0);
    LVL1OK=1;
    TOFTrigFl=p2lev1->TofFlag;
    ECTrigFl=p2lev1->EcalFlag;
  }
//
  if(ntuple->NParticle()==0)return;//======> no Part
  RunPar::addsev(1);//<--passed Part check
//
  Int_t itrktr(-1),itrdtr(-1),pindex(-1);
  for(int i=0;i<ntuple->NParticle();i++){//search for 1st Part. with trk-track
    itrktr = ntuple->Particle(i).iTrTrack();//TRKtrack index used by i-th Part. 
    itrdtr = ntuple->Particle(i).iTrdTrack();//TRDtrack index used by i-th Part.
    if(itrktr>=0)pindex=i;//store index of 1st Part with trk-track 
    if(itrktr>=0)break;
  }
//
// <======= Select track(now TRK) based Particle :
//
  if(pindex<0)return;//======> no Part. with TRK-track
  RunPar::addsev(2);//<--- passed part-type check
  Bool_t TOFBetaOK(0);
  Float_t tofbet(0);
//
  BetaR *p2beta =  ntuple->Particle(pindex).pBeta(); //pointer to TOF-beta, used by 1st Part. with trk-track
  Int_t il,ib,id;
  Float_t etof[4]={0,0,0,0},ttof[4]={0,0,0,0},zctof[4]={0,0,0,0};//vs layer #
  Int_t bltof[4]={0,0,0,0},ntofp[4];//vs layer
  Int_t bltofb[4]={0,0,0,0};//bar #(1:) with bad tof/trk matching(vs layer)
  Int_t ntofrcls=ntuple->NTofRawCluster();//total tof-raw_clusters(paddles)
  Int_t ntofcls=ntuple->NTofCluster();//total tof-clusters
  Int_t nbetofc(0);//number of Beta-used tof-clusters
  Float_t xtof[4],ytof[4];
  Int_t ltof[4],btof[4]={0,0,0,0};//vs used-cluster #
  TofRawClusterR * p2tofrc[4][2]={0,0,0,0,0,0,0,0};//pointers(il) to raw-clust members(max 2) of TofCluster
//
  _filled[0]->Fill(ntofrcls,1);
  if(p2beta>0){//<====  TOF-beta measurement found in particle
    RunPar::addsev(3);//<--passed TOF-beta presence check
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
      for(int j=0;j<ntofp[ltof[i]-1];j++)p2tofrc[ltof[i]-1][j]=p2beta->pTofCluster(i)
                                                                   ->pTofRawCluster(j);//RawCl-pointers
      _filled[2]->Fill(btof[i]+(ltof[i]-1)*10,1);
      _filled[3]->Fill(ntofp[i],1);
    }
//
    _filled[4]->Fill(p2beta->Beta,1);
    _filled[5]->Fill(p2beta->Chi2,1);
    _filled[6]->Fill(p2beta->Chi2S,1);
    _filled[7]->Fill(p2beta->Pattern,1);
    if(fabs(p2beta->Beta) < 2.
	                 && p2beta->Chi2 < 5.
	                 && p2beta->Chi2S < 5.
	                 && p2beta->Pattern <= 4
		         && ntofcls < 10
	                                                ){//<---- TOF-beta quality check
//
      TOFBetaOK=1;
      RunPar::addsev(4);//<--passed "beta quality" test
    }// --- endof "TOF-beta quality check --->
  }// --- endof "TOF in particle" check --->
//-------------------------------------------------------------
//                                <--- check TRK-track quality:
//
  Bool_t TRKtrOK(0);
//
//cout<<"  itrktr="<<itrktr<<endl;
  TrTrackR *p2trktr = ntuple->Particle(pindex).pTrTrack();//pointer to TRK-track used by Part.
  Float_t trkthe(0);
  Float_t trkphi(0);
//
  if(p2trktr>0){//<---- TRKtrack in Particle presence check
    RunPar::addsev(5);//<--found TRKtrack in part
    Bool_t trkisGood=p2trktr->IsGood();//true,if track has true x-points, otherwise =false
    UInt_t trksta=p2trktr->Status;
    Int_t id=(trksta%16384)/8192;//  bit 14
    cutf[4]=(id==0);// --> NOt false X
    id=(trksta%32768)/16384;//  bit 15
    cutf[5]=(id==0);// --> NOt false TOFX

    Int_t trkpat=p2trktr->Pattern;
    Int_t trladd=p2trktr->Address;
  
    Float_t trkrigp=0;//J.A. methode
    Float_t trkrigpe=p2trktr->PiErrRig;//err to 1/above (<0 means fit is failed)
    Float_t rerigp=0;//dR/R
    if(trkrigpe>=0){
      trkrigp=p2trktr->PiRigidity;
      rerigp=trkrigpe*fabs(trkrigp);//abs. dR/R from JA
    }
    
    Int_t trkafd=p2trktr->AdvancedFitDone;
    
    Float_t trkch2sz=p2trktr->Chi2StrLine;//str-line chi2
    
    Float_t trkch2cms=p2trktr->Chi2WithoutMS;//chi2 of circular-fit, MSattOff
    Float_t trkrigcms=p2trktr->RigidityWithoutMS;//rigid ....................
    
    Float_t trkch2=p2trktr->Chi2FastFit;//fast nonl. fit
    Float_t trkrig=p2trktr->Rigidity;//fast nonl. fit
    Float_t trkrige=p2trktr->ErrRigidity;//err to 1/above
    Float_t rerig=trkrige*fabs(trkrig);//abs. dR/R
    Float_t trkrigms=p2trktr->RigidityMS;//fast nonl. fit, MScattOff
    Float_t trkch2ms=p2trktr->FChi2MS;//chi2 for above
    trkthe=p2trktr->Theta;
    trkphi=p2trktr->Phi;
  
    Float_t trkch2h[2]={999.,999.}; //chi2 for 2 halves
    Float_t trkhrig[2]={0.,0.}; //2 halves rigs
    Float_t hrigass=-999;
    if(trkafd){
      trkch2h[0]=p2trktr->HChi2[0];
      trkch2h[1]=p2trktr->HChi2[1];
      trkhrig[0]=p2trktr->HRigidity[0];
      trkhrig[1]=p2trktr->HRigidity[1];
      if((trkhrig[0]+trkhrig[1])!=0)hrigass=(trkhrig[0]-trkhrig[1])/(trkhrig[0]+trkhrig[1]);
      if(hrigass>1)hrigass=0.9999;
      if(hrigass<-1)hrigass=-1;
    }
//
    if(cutf[4] && cutf[5]){//<---- true X
      RunPar::addsev(6);//<--passed "trueX" test
//
      _filled[8]->Fill(trkch2,1);
      _filled[9]->Fill(trkch2sz,1);
      _filled[10]->Fill(trkch2ms,1);
      if(trkch2<100
          && trkch2sz<10
          && trkch2ms<500
	                 ){//<---- chi2's check
//
        if(trkafd)_filled[11]->Fill(hrigass,1);
        _filled[12]->Fill(rerig,1);
        if(trkrigpe>=0)_filled[13]->Fill(rerigp,1);
//
        TRKtrOK=1;
        RunPar::addsev(7);//<--passed "Chi2's" check
//
      }//--- endof "chi2's" check --->
    }//--- endof "trueX" check --->
  }// --- endof "TRKtr in Particle" check --->
//-------------------------------------------------
//
//                      <---- Check TOF-TRKtrack matching:
//
  Bool_t TOFTRKmatchOK(0);
  Int_t Ntoftrmatch(0);
  Float_t ztof[4]={0,0,0,0};
  Float_t tofcro[4][3]={0,0,0,0,0,0,0,0,0,0,0,0};
  Float_t dx,dy;
//
  if(TOFBetaOK && TRKtrOK){//<---- TOFbeta/TRKtr presence check
    RunPar::addsev(8);//<-- count matching tests 
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
    cutf[6]=true;// good tof-track matching 
    for(int ic=0;ic<nbetofc;ic++){//<--beta-used cluster loop
      il=ltof[ic]-1;//0:3
      ib=btof[ic];//1:
      if(ib>1 && ib<kNtofb[il]){//dx/dy-cuts for center
        toflc=8;
        toftc=8;
      }
      else{//... for outer 
        toflc=1.5*8;
        toftc=2*8;
      }
//
      if(ib>1 && ib<kNtofb[il]){//central counters
//        tofh4[il]->Fill(dclg[ic],1);
      }
      else{//outer counters
//        if(ib==1)tofh5[il]->Fill(dclg[ic],1);
//        else tofh5[il+4]->Fill(dclg[ic],1);
      }
//
      if(fabs(dclg[ic])>toflc){
        cutf[6]=false;//bad long. tof/trk-matching
        bltofb[il]=ib;
      }
//      tofh4[il+4]->Fill(dctr[ic],1);
      if(fabs(dctr[ic])>toftc){
        cutf[6]=false;//bad transv. tof/trk-matching
        bltofb[il]=ib;
      }
      if(fabs(dclg[ic])<toflc && fabs(dctr[ic])<toftc)Ntoftrmatch+=1;
    }//--->endof beta-used cluster loop
    
    if(cutf[6]){//<---- Good Tof-TrkTrack matching in ALL!!! beta-used tof-layers  
//
      TOFTRKmatchOK=1;
      RunPar::addsev(11);//<--passed "Tof-TrkTrack matching" test
//
    }//--- endof "good matching" check --->
  }//--- endof "TOFbeta/TRKtr-presence" check --->
}


