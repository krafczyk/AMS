//  $Id: root.C,v 1.39 2003/05/08 16:41:50 choutko Exp $
//
#include <root.h>
#include <ntuple.h>
#ifndef __ROOTSHAREDLIBRARY__
#include <antirec02.h>
#include <beta.h>
#include <ecalrec.h>
#include <mccluster.h>
#include <mceventg.h>
#include <particle.h>
#include <richrec.h>
#include <tofrec02.h>
#include <antirec02.h>
#include <trdrec.h>
#include <trigger102.h>
#include <trigger302.h>
#include <trrawcluster.h>
#include <trrec.h>
#endif
using namespace root;
#ifdef __WRITEROOT__


  ClassImp(HeaderR)
  ClassImp(EcalHitR)
  ClassImp(EcalClusterR)
  ClassImp(Ecal2DClusterR)
  ClassImp(EcalShowerR)
  ClassImp(RichHitR)
  ClassImp(RichRingR)
  ClassImp(TofRawClusterR)
  ClassImp(TofClusterR)
  ClassImp(AntiClusterR)
  ClassImp(TrRawClusterR)
  ClassImp(TrClusterR)
  ClassImp(TrRecHitR)
  ClassImp(TrTrackR)
  ClassImp(TrdRawHitR)
  ClassImp(TrdClusterR)
  ClassImp(TrdSegmentR)
  ClassImp(TrdTrackR)
  ClassImp(Level1R)
  ClassImp(Level3R)
  ClassImp(BetaR)
  ClassImp(ChargeR)
  ClassImp(ParticleR)
  ClassImp(AntiMCClusterR)
  ClassImp(TrMCClusterR)
  ClassImp(TofMCClusterR)
  ClassImp(TrdMCClusterR)
  ClassImp(RichMCClusterR)
  ClassImp(MCTrackR)
  ClassImp(MCEventgR)



//------------------- constructors -----------------------



TBranch* AMSEventR::bHeader;
TBranch* AMSEventR::bEcalHit;
TBranch* AMSEventR::bEcalCluster;
TBranch* AMSEventR::bEcal2DCluster;
TBranch* AMSEventR::bEcalShower;
TBranch* AMSEventR::bRichHit;
TBranch* AMSEventR::bRichRing;
TBranch* AMSEventR::bTofRawCluster;
TBranch* AMSEventR::bTofCluster;
TBranch* AMSEventR::bAntiCluster;
TBranch* AMSEventR::bTrRawCluster;
TBranch* AMSEventR::bTrCluster;
TBranch* AMSEventR::bTrRecHit;
TBranch* AMSEventR::bTrTrack;
TBranch* AMSEventR::bTrdRawHit;
TBranch* AMSEventR::bTrdCluster;
TBranch* AMSEventR::bTrdSegment;
TBranch* AMSEventR::bTrdTrack;
TBranch* AMSEventR::bLevel1;
TBranch* AMSEventR::bLevel3;
TBranch* AMSEventR::bBeta;
TBranch* AMSEventR::bVertex;
TBranch* AMSEventR::bCharge;
TBranch* AMSEventR::bParticle;
TBranch* AMSEventR::bAntiMCCluster;
TBranch* AMSEventR::bTrMCCluster;
TBranch* AMSEventR::bTofMCCluster;
TBranch* AMSEventR::bTrdMCCluster;
TBranch* AMSEventR::bRichMCCluster;
TBranch* AMSEventR::bMCTrack;
TBranch* AMSEventR::bMCEventg;



AMSEventR* AMSEventR::_Head=0;
int AMSEventR::_Entry=-1;
char* AMSEventR::_Name="ev.";   //  root compatibility

void AMSEventR::SetBranchA(TTree *fChain){
     char tmp[255];

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     fChain->SetBranchAddress(tmp,&fHeader);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     fChain->SetBranchAddress(tmp,&fEcalHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     fChain->SetBranchAddress(tmp,&fEcalCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     fChain->SetBranchAddress(tmp,&fEcal2DCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     fChain->SetBranchAddress(tmp,&fEcalShower);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     fChain->SetBranchAddress(tmp,&fRichHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     fChain->SetBranchAddress(tmp,&fRichRing);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     fChain->SetBranchAddress(tmp,&fTofRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     fChain->SetBranchAddress(tmp,&fTofCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     fChain->SetBranchAddress(tmp,&fAntiCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     fChain->SetBranchAddress(tmp,&fTrRawCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     fChain->SetBranchAddress(tmp,&fTrCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     fChain->SetBranchAddress(tmp,&fTrRecHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     fChain->SetBranchAddress(tmp,&fTrTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     fChain->SetBranchAddress(tmp,&fTrdRawHit);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     fChain->SetBranchAddress(tmp,&fTrdCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     fChain->SetBranchAddress(tmp,&fTrdSegment);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     fChain->SetBranchAddress(tmp,&fTrdTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     fChain->SetBranchAddress(tmp,&fLevel1);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     fChain->SetBranchAddress(tmp,&fLevel3);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     fChain->SetBranchAddress(tmp,&fBeta);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     fChain->SetBranchAddress(tmp,&fCharge);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     fChain->SetBranchAddress(tmp,&fParticle);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     fChain->SetBranchAddress(tmp,&fAntiMCCluster);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     fChain->SetBranchAddress(tmp,&fTrMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     fChain->SetBranchAddress(tmp,&fTofMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     fChain->SetBranchAddress(tmp,&fTrdMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     fChain->SetBranchAddress(tmp,&fRichMCCluster);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     fChain->SetBranchAddress(tmp,&fMCTrack);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     fChain->SetBranchAddress(tmp,&fMCEventg);
    }

}

void AMSEventR::GetBranchA(TTree *fChain){
     char tmp[255];

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fHeader");
     bHeader = fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalHit");
     bEcalHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalCluster");
     bEcalCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcal2DCluster");
     bEcal2DCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fEcalShower");
     bEcalShower=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichHit");
     bRichHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichRing");
     bRichRing=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofRawCluster");
     bTofRawCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofCluster");
     bTofCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiCluster");
     bAntiCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRawCluster");
     bTrRawCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrCluster");
     bTrCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrRecHit");
     bTrRecHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrTrack");
     bTrTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdRawHit");
     bTrdRawHit=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdCluster");
     bTrdCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdSegment");
     bTrdSegment=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdTrack");
     bTrdTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel1");
     bLevel1=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fLevel3");
     bLevel3=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fBeta");
     bBeta=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fCharge");
     bCharge=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fParticle");
     bParticle=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fAntiMCCluster");
     bAntiMCCluster=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrMCCluster");
     bTrMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTofMCCluster");
     bTofMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fTrdMCCluster");
     bTrdMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fRichMCCluster");
     bRichMCCluster=fChain->GetBranch(tmp);
    }


   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCTrack");
     bMCTrack=fChain->GetBranch(tmp);
    }

   {
     strcpy(tmp,_Name);
     strcat(tmp,"fMCEventg");
     bMCEventg=fChain->GetBranch(tmp);
    }
}

void AMSEventR::SetCont(){
// Set container sizes;
 fHeader.EcalHits=fEcalHit.size();
 fHeader.EcalClusters=fEcalCluster.size();
 fHeader.Ecal2DClusters=fEcal2DCluster.size();
 fHeader.EcalShowers=fEcalShower.size();
 fHeader.RichHits=fRichHit.size();
 fHeader.RichRings=fRichRing.size();
 fHeader.TofRawClusters=fTofRawCluster.size();
 fHeader.TofClusters=fTofCluster.size();
 fHeader.AntiClusters=fAntiCluster.size();
 fHeader.TrClusters=fTrCluster.size();
 fHeader.TrRecHits=fTrRecHit.size();
 fHeader.TrTracks=fTrTrack.size();
 fHeader.TrdRawHits=fTrdRawHit.size();
 fHeader.TrdClusters=fTrdCluster.size();
 fHeader.TrdSegments=fTrdSegment.size();
 fHeader.TrdTracks=fTrdTrack.size();
 fHeader.Level1s=fLevel1.size();
 fHeader.Level3s=fLevel3.size();
 fHeader.Betas=fBeta.size();
 fHeader.Charges=fCharge.size();
 fHeader.Particles=fParticle.size();
 fHeader.AntiMCClusters=fAntiMCCluster.size();
 fHeader.TrMCClusters=fTrMCCluster.size();
 fHeader.TofMCClusters=fTofMCCluster.size();
 fHeader.TrdMCClusters=fTrdMCCluster.size();
 fHeader.RichMCClusters=fRichMCCluster.size();
 fHeader.MCTracks=fMCTrack.size();
 fHeader.MCEventgs=fMCEventg.size();
// cout <<" fHeader.TrRecHits "<<fHeader.TrRecHits<<endl;
}

void AMSEventR::ReadHeader(int entry){
   _Entry=entry;
   bHeader->GetEntry(entry);
   clear();
}



AMSEventR::AMSEventR():TObject(){
if(_Head)cerr<<"AMSEventR::ctor-F-OnlyOneSingletonAllowed"<<endl;
_Head=this;


fEcalHit.reserve(MAXECHITS);
fEcalCluster.reserve(MAXECCLUST);
fEcal2DCluster.reserve(MAXEC2DCLUST);
fEcalShower.reserve(MAXECSHOW);

fRichHit.reserve(MAXRICHRIN);
fRichRing.reserve(MAXRICHITS);

fTofRawCluster.reserve(MAXTOFRAW);
fTofCluster.reserve(MAXTOF);
fAntiCluster.reserve(MAXANTICL);

fTrRawCluster.reserve(MAXTRRAW);
fTrCluster.reserve(MAXTRCL);
fTrRecHit.reserve(MAXTRRH02);
fTrTrack.reserve(MAXTRTR02);

fTrdRawHit.reserve(MAXTRDRHT );
fTrdCluster.reserve(MAXTRDCL );
fTrdSegment.reserve(MAXTRDSEG);
fTrdTrack.reserve(MAXTRDTRK);

fLevel1.reserve(MAXLVL1);
fLevel3.reserve(MAXLVL3);

fBeta.reserve(MAXBETA02);
fCharge.reserve(MAXCHARGE02);
fParticle.reserve(MAXPART02);

fAntiMCCluster.reserve(MAXANTIMC);
fTofMCCluster.reserve(MAXTOFMC);
fTrMCCluster.reserve(MAXTRCLMC);
fTrdMCCluster.reserve(MAXTRDCLMC);
fRichMCCluster.reserve(MAXRICMC);

fMCTrack.reserve(MAXMCVOL);
fMCEventg.reserve(MAXMCG02);
}


void AMSEventR::clear(){
fEcalHit.clear();
fEcalCluster.clear();
fEcal2DCluster.clear();
fEcalShower.clear();

fRichHit.clear();
fRichRing.clear();

fTofRawCluster.clear();
fTofCluster.clear();
fAntiCluster.clear();

fTrRawCluster.clear();
fTrCluster.clear();
fTrRecHit.clear();
fTrTrack.clear();

fTrdRawHit.clear();
fTrdCluster.clear();
fTrdSegment.clear();
fTrdTrack.clear();

fLevel1.clear();
fLevel3.clear();

fBeta.clear();
fCharge.clear();
fParticle.clear();

fAntiMCCluster.clear();
fTofMCCluster.clear();
fTrMCCluster.clear();
fTrdMCCluster.clear();
fRichMCCluster.clear();

fMCTrack.clear();
fMCEventg.clear();
}



//------------- AddAMSObject 
#ifndef __ROOTSHAREDLIBRARY__



void AMSEventR::AddAMSObject(AMSEcalHit *ptr)
{
  fEcalHit.push_back(EcalHitR(ptr));
  ptr->SetClonePointer(& fEcalHit.back(),fEcalHit.size()-1);
}

void AMSEventR::AddAMSObject(Ecal1DCluster *ptr) 
{
  if (ptr) {
  fEcalCluster.push_back(EcalClusterR(ptr));
  ptr->SetClonePointer(& fEcalCluster.back(), fEcalCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- Ecal1DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(Ecal2DCluster *ptr) {
  if (ptr) {
  fEcal2DCluster.push_back(Ecal2DClusterR(ptr));
  ptr->SetClonePointer(& fEcal2DCluster.back(),fEcal2DCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- (Ecal2DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(EcalShower *ptr) 
{
  if (ptr) {
  fEcalShower.push_back(EcalShowerR(ptr));
  ptr->SetClonePointer(& fEcalShower.back(),fEcalShower.size()-1);
  }  else {
   cout<<"AddAMSObject -E- EcalShower ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRawEvent *ptr, float x, float y)
{
  if (ptr) {
  fRichHit.push_back(RichHitR(ptr,x,y));
  ptr->SetClonePointer(& fRichHit.back(),fRichHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRing *ptr)
{
  fRichRing.push_back(RichRingR(ptr));
  ptr->SetClonePointer(& fRichRing.back(),fRichRing.size()-1);
}

void AMSEventR::AddAMSObject(AMSTOFCluster *ptr){
  if (ptr) {
  fTofCluster.push_back(TofClusterR(ptr));
  ptr->SetClonePointer(& fTofCluster.back(),fTofCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTofCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TOF2RawCluster *ptr)
{
  if (ptr) {
  fTofRawCluster.push_back(TofRawClusterR(ptr));
  ptr->SetClonePointer(& fTofRawCluster.back(),fTofRawCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTOF2RawCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiCluster *ptr)
{
  if (ptr) {
  fAntiCluster.push_back(AntiClusterR(ptr));
  ptr->SetClonePointer(&fAntiCluster.back(),fAntiCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrRawCluster *ptr)
{
  if (ptr) {
  fTrRawCluster.push_back(TrRawClusterR(ptr));
  ptr->SetClonePointer(& fTrRawCluster.back(), fTrRawCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- TrRawCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrCluster *ptr){
  if (ptr) {
  fTrCluster.push_back(TrClusterR(ptr));
  ptr->SetClonePointer(& fTrCluster.back(),fTrCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTrRecHit *ptr)
{
  if (ptr) {
  fTrRecHit.push_back(TrRecHitR(ptr));
  ptr->SetClonePointer(& fTrRecHit.back(),fTrRecHit.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrRecHit ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrTrack *ptr)
{
  if (ptr) {
  fTrTrack.push_back(TrTrackR(ptr));
  ptr->SetClonePointer(&fTrTrack.back(), fTrTrack.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDRawHit *ptr)
{
  if (ptr) {
  fTrdRawHit.push_back(TrdRawHitR(ptr));
  ptr->SetClonePointer(& fTrdRawHit.back(), fTrdRawHit.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDRawHit ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDCluster *ptr)
{
  if (ptr) {
  fTrdCluster.push_back(TrdClusterR(ptr));
  ptr->SetClonePointer(& fTrdCluster.back(), fTrdCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDSegment *ptr)
{
  if (ptr) {
  fTrdSegment.push_back(TrdSegmentR(ptr));
  ptr->SetClonePointer(& fTrdSegment.back(),fTrdSegment.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDSegment ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDTrack *ptr)
{
  if (ptr) {
  fTrdTrack.push_back(TrdTrackR(ptr));
  ptr->SetClonePointer(& fTrdTrack.back(),fTrdTrack.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(Trigger2LVL1 *ptr)
{
  if (ptr) {
  fLevel1.push_back(Level1R(ptr));
  ptr->SetClonePointer(& fLevel1.back(),fLevel1.size()-1);
  }  else {
   cout<<"AddAMSObject -E- Trigger2LVL1 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TriggerLVL302 *ptr)
{
  if (ptr) {
  fLevel3.push_back(Level3R(ptr));
  ptr->SetClonePointer(& fLevel3.back(),fLevel3.size()-1);
  }  else {
   cout<<"AddAMSObject -E- TriggerLVL302 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSBeta *ptr)
{
  if (ptr) {
   fBeta.push_back(BetaR(ptr));
   ptr->SetClonePointer(&fBeta.back(),fBeta.size()-1);
   }  else {
     cout<<"AddAMSObject -E- AMSBeta ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[], 
                               float probtr[], int chintr[], float probrc[], 
                               int chinrc[], float proballtr)
{
  if (ptr) {
   fCharge.push_back(ChargeR(ptr, probtof, chintof, probtr, chintr, probrc, chinrc, proballtr));
   ptr->SetClonePointer(&fCharge.back(),fCharge.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSCharge ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSParticle *ptr, float phi, float phigl)
{
  if (ptr) {
  fParticle.push_back(ParticleR(ptr, phi, phigl));
  ptr->SetClonePointer(& fParticle.back(),fParticle.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSParticle ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiMCCluster *ptr)
{
  if (ptr) {
  fAntiMCCluster.push_back(AntiMCClusterR(ptr));
  ptr->SetClonePointer(&fAntiMCCluster.back(),fAntiMCCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiMCCluster ptr is NULL"<<endl;
  }
}






void AMSEventR::AddAMSObject(AMSRichMCHit *ptr, int _numgen)
{
  fRichMCCluster.push_back(RichMCClusterR(ptr,_numgen));
  ptr->SetClonePointer(& fRichMCCluster.back(),fRichMCCluster.size()-1);
}





void AMSEventR::AddAMSObject(AMSTOFMCCluster *ptr)
{
  if (ptr) {
  fTofMCCluster.push_back(TofMCClusterR(ptr));
  ptr->SetClonePointer(& fTofMCCluster.back(),fTofMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTofMCCluster ptr is NULL"<<endl;
  }
}




void AMSEventR::AddAMSObject(AMSTrMCCluster *ptr)
{
  if (ptr) {
  fTrMCCluster.push_back(TrMCClusterR(ptr));
  ptr->SetClonePointer(& fTrMCCluster.back(), fTrMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTrMCCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDMCCluster *ptr)
{
  if (ptr) {
  fTrdMCCluster.push_back(TrdMCClusterR(ptr));
  ptr->SetClonePointer(& fTrdMCCluster.back(),fTrdMCCluster.size()-1);
  }  else {
   cout<<"AddAMSObject -E- AMSTRDMCCluster ptr is NULL"<<endl;
  }
}



void AMSEventR::AddAMSObject(AMSmceventg *ptr)
{
  if (ptr) {
  fMCEventg.push_back(MCEventgR(ptr));
  ptr->SetClonePointer(& fMCEventg.back(), fMCEventg.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmceventg ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSmctrack *ptr)
{
  if (ptr) {
  fMCTrack.push_back(MCTrackR(ptr));
  ptr->SetClonePointer(& fMCTrack.back(),fMCTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmctrack ptr is NULL"<<endl;
  }
}


#endif




void HeaderR::Set(EventNtuple02* ptr){
#ifndef __ROOTSHAREDLIBRARY__

    Run=       ptr->Run;
    RunType=   ptr->RunType;
    Event=     ptr->Eventno;
    Status[0]= ptr->EventStatus[0];
    Status[1]= ptr->EventStatus[1];
    Raw=       ptr->RawWords%(1<<18);
    Version=   (ptr->RawWords)>>18;
    Time[0]=   ptr->Time[0];
    Time[1]=   ptr->Time[1];
    RadS=      ptr->RadS;
    ThetaS=    ptr->ThetaS;
    PhiS=      ptr->PhiS;
    Yaw=       ptr->Yaw;
    Pitch=     ptr->Pitch;
    Roll=      ptr->Roll;
    VelocityS= ptr->VelocityS;
    VelTheta=  ptr->VelTheta;
    VelPhi=    ptr->VelPhi;
    ThetaM=    ptr->ThetaM;

    
#endif
}





AntiClusterR::AntiClusterR(AMSAntiCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Sector = ptr->_sector;
  Ntimes = ptr->_ntimes;
  Npairs = ptr->_npairs;
  for(int i=0;i<Ntimes;i++)Times[i] = ptr->_times[i];
  for(int i=0;i<Ntimes;i++)Timese[i] = ptr->_etimes[i];
  Edep   = ptr->_edep;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = ptr->_ecoo[i];
#endif
}

AntiMCClusterR::AntiMCClusterR(AMSAntiMCCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->_idsoft;
  for (int i=0; i<3; i++) Coo[i]=ptr->_xcoo[i];
  TOF    = ptr->_tof;
  Edep   = ptr->_edep;
#endif
}



BetaR::BetaR(AMSBeta *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status   = ptr->_status;
  Pattern  = ptr->_Pattern;
  Beta     = ptr->_Beta;
  BetaC    = ptr->_BetaC;
  Error    = ptr->_InvErrBeta;
  ErrorC   = ptr->_InvErrBetaC;
  Chi2     = ptr->_Chi2;
  Chi2S    = ptr->_Chi2Space;
 fTrTrack    = -1;
#endif
}


ChargeR::ChargeR(AMSCharge *ptr, float probtof[],int chintof[], 
                           float probtr[], int chintr[], float probrc[],
                           int chinrc[], float proballtr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  ChargeTOF     = ptr->_ChargeTOF;
  ChargeTracker = ptr->_ChargeTracker;
  ChargeRich    = ptr->_ChargeRich;
  for (int i=0; i<4; i++) {
    ProbTOF[i] = probtof[i];
    ChInTOF[i] = chintof[i];
    ProbTracker[i] = probtr[i];
    ChInTracker[i] = chintr[i];
    ProbRich[i] = probrc[i];
    ChInRich[i] = chinrc[i];
  }
  ProbAllTracker= proballtr;
  TrunTOF       = ptr->_TrMeanTOF;
  TrunTOFD      = ptr->_TrMeanTOFD;
  TrunTracker   = ptr->_TrMeanTracker;
 fBeta=-1;
 fRich=-1;
#endif
}


Ecal2DClusterR::Ecal2DClusterR(Ecal2DCluster *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->getstatus();
  Proj   = ptr->_proj;
  Nmemb  = ptr->_NClust;
  Edep   = ptr->_EnergyC;
  Coo    = ptr->_Coo;
  Tan    = ptr->_Tan;
  Chi2   = ptr->_Chi2;
#endif
}

EcalClusterR::EcalClusterR(Ecal1DCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status  = ptr->_status;
  Proj    = ptr->_proj;
  Plane   = ptr->_plane;
  Left    = ptr->_Left;
  Center  = ptr->_MaxCell;
  Right   = ptr->_Right;
  Edep    = ptr->_EnergyC;
  SideLeak = ptr->_SideLeak;
  DeadLeak = ptr->_DeadLeak;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  NHits = ptr->_NHits;
#endif
}

EcalShowerR::EcalShowerR(EcalShower *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status      = ptr->_status;
  for (int i=0; i<3; i++) {
   Dir[i]   = ptr->_Dir[i];
   EMDir[i] = ptr->_EMDir[i];
   Entry[i] = ptr->_EntryPoint[i];
   Exit[i]  = ptr->_ExitPoint[i];
   CofG[i]  = ptr->_CofG[i];
  }
  ErTheta   = ptr->_Angle3DError;
  Chi2Dir   = ptr->_AngleTrue3DChi2;
  FirstLayerEdep = ptr->_FrontEnergyDep;
  EnergyC   =   ptr->_EnergyC;
  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  DifoSum     = ptr->_DifoSum;
  SideLeak    = ptr->_SideLeak;
  RearLeak    = ptr->_RearLeak;
  DeadLeak    = ptr->_DeadLeak;
  AttLeak     = ptr->_AttLeak;
  OrpLeak     = ptr->_OrpLeak;
  Orp2DEnergy = ptr->_Orp2DEnergy;
  Chi2Profile = ptr->_ProfilePar[4+ptr->_Direction*5];
  for (int i=0; i<4; i++) ParProfile[i] = ptr->_ProfilePar[i+ptr->_Direction*5];
  Chi2Trans = ptr->_TransFitChi2;
  for (int i=0; i<3; i++) SphericityEV[i] = ptr->_SphericityEV[i];
  N2dCl       = ptr->_N2dCl;
#endif
}

Level1R::Level1R(Trigger2LVL1 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Mode   = ptr->_LifeTime;
  TOFlag = ptr->_tofflag;
  for (int i=0; i<4; i++) {
    TOFPatt[i]  = ptr->_tofpatt[i];
    TOFPatt1[i] = ptr->_tofpatt1[i];
  }
  AntiPatt = ptr->_antipatt;
  ECALflag = ptr->_ecalflag;
  ECALtrsum= ptr->_ectrsum;
#endif
}

Level3R::Level3R(TriggerLVL302 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  TOFTr     = ptr->_TOFTrigger;
  TRDTr     = ptr->_TRDTrigger;
  TrackerTr = ptr->_TrackerTrigger;
  MainTr    = ptr->_MainTrigger;
  Direction = ptr->_TOFDirection;
  NTrHits   = ptr->_NTrHits;
  NPatFound = ptr->_NPatFound;
  for (int i=0; i<2; i++) {Pattern[i] = ptr->_Pattern[i];}
  for (int i=0; i<2; i++) {Residual[i]= ptr->_Residual[i];}
  Time      = ptr->_Time;
  ELoss     = ptr->_TrEnergyLoss;
  TRDHits   = ptr->TRDAux._NHits[0]+ptr->TRDAux._NHits[1];;
  HMult     = ptr->TRDAux._HMult;;
  for (int i=0; i<2; i++) {TRDPar[i] = ptr->TRDAux._Par[i][0];}
  ECemag    = ptr->_ECemag;
  ECmatc    = ptr->_ECmatc;
  for (int i=0; i<4; i++) {ECTOFcr[i] = ptr->_ECtofcr[i];}

#endif
}

MCEventgR::MCEventgR(AMSmceventg *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Nskip    = ptr->_nskip;
  Particle = ptr->_ipart;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) Dir[i] = ptr->_dir[i];
  Momentum = ptr->_mom;
  Mass     = ptr->_mass;
  Charge   = ptr->_charge;
#endif
}           

MCTrackR::MCTrackR(AMSmctrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  _radl = ptr->_radl;
  _absl = ptr->_absl;
  for (int i=0; i<3; i++) _pos[i]   = ptr->_pos[i];
  for (int i=0; i<4; i++) _vname[i] = ptr->_vname[i];
#endif
}


ParticleR::ParticleR(AMSParticle *ptr, float phi, float phigl)
{
#ifndef __ROOTSHAREDLIBRARY__
  Phi      = phi;
  PhiGl    = phigl;
  fBeta   = -1;
  fCharge = -1;
  fTrack  = -1;
  fTrd    = -1;
  fRich   = -1;
  fShower = -1;
  Particle     = ptr->_gpart[0];
  ParticleVice = ptr->_gpart[1];
  for (int i=0; i<2; i++) {Prob[i] = ptr->_prob[i];}
  FitMom   = ptr->_fittedmom[0];
  Mass     = ptr->_Mass;
  ErrMass  = ptr->_ErrMass;
  Momentum = ptr->_Momentum;
  ErrMomentum = ptr->_ErrMomentum;
  Beta     = ptr->_Beta;
  ErrBeta  = ptr->_ErrBeta;
  Charge   = ptr->_Charge;
  Theta    = ptr->_Theta;
  ThetaGl  = ptr->_ThetaGl;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  Cutoff   = ptr->_CutoffMomentum;
  for (int i=0; i<4; i++) {
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = ptr->_TOFCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      AntiCoo[i][j] = ptr->_AntiCoo[i][j];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ptr->_EcalSCoo[i][j];
    }
  }
  for (int i=0; i<8; i++) {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = ptr->_TrCoo[i][j];
    }
  }
  for (int i=0; i<8; i++)  Local[i] = ptr->_Local[i];

  for (int i=0; i<3; i++) {TRDCoo[i] = ptr->_TRDCoo[i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = ptr->_RichCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = ptr->_RichPath[i];
                           RichPathBeta[i] = ptr->_RichPathBeta[i];}
  RichLength = ptr->_RichLength;

  TRDLikelihood = ptr->_TRDLikelihood;
#endif
}




TofClusterR::TofClusterR(AMSTOFCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  Nmemb  = ptr->_nmemb;
  Edep   = ptr->_edep;
  Edepd  = ptr->_edepd;
  Time   = ptr->_time;
  ErrTime= ptr->_etime;
  for (int i=0; i<3; i++) {
    Coo[i] = ptr->_Coo[i];
    ErrorCoo[i] = ptr->_ErrorCoo[i];
  }
#endif
}

TofMCClusterR::TofMCClusterR(AMSTOFMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Edep= ptr->edep;
#endif
}

TofRawClusterR::TofRawClusterR(TOF2RawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__

  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  for (int i=0; i<2; i++) tovta[i]=ptr->_adca[i];
  for (int i=0; i<2; i++) tovtd[i]=ptr->_adcd[i];
  for (int i=0; i<2; i++) tovtdl[i]=ptr->_adcdl[i];
  for (int i=0; i<2; i++) sdtm[i] =ptr->_sdtm[i];
  edepa  = ptr->_edepa;
  edepd  = ptr->_edepd;
  edepdl  = ptr->_edepdl;
  time   = ptr->_time;
  cool   = ptr->_timeD;
#endif
}

TrdClusterR::TrdClusterR(AMSTRDCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_layer;
  for (int i=0; i<3; i++) Coo[i]= ptr->_Coo[i];
  for (int i=0; i<3; i++) CooDir[i]= ptr->_CooDir[i];
  Multip = ptr->_Multiplicity;
  HMultip= ptr->_HighMultiplicity;
  EDep   = ptr->_Edep;
  fTrdRawHit =-1;
#endif
}

TrdMCClusterR::TrdMCClusterR(AMSTRDMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_idsoft.getlayer();
  Ladder = ptr->_idsoft.getladder();
  Tube   = ptr->_idsoft.gettube();
  ParticleNo= ptr->_ipart;
  //  TrackNo= ptr->_itra;
  Edep   = ptr->_edep;
  Ekin   = ptr->_ekin;
  for (int i=0; i<3; i++) {Xgl[i] = ptr->_xgl[i];}
  Step   = ptr->_step;
#endif
}

TrdRawHitR::TrdRawHitR(AMSTRDRawHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_id.getlayer();
  Ladder = ptr->_id.getladder();
  Tube   = ptr->_id.gettube();
  Amp    = ptr->Amp();
#endif
}

TrdSegmentR::TrdSegmentR(AMSTRDSegment *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status        = ptr->_status;
  Orientation   = ptr->_Orientation;
  for (int i=0; i<2; i++) {FitPar[i] = ptr->_FitPar[i];}
  Chi2          = ptr->_Chi2;
  Pattern       = ptr->_Pattern;
  Nhits         = ptr->_NHits;
#endif
}

TrdTrackR::TrdTrackR(AMSTRDTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  for (int i=0; i<3; i++) {
    Coo[i]   = ptr->_StrLine._Coo[i];
    ErCoo[i] = ptr->_StrLine._ErCoo[i];
  }
  Phi   = ptr->_StrLine._Phi;
  Theta = ptr->_StrLine._Theta;
  Chi2  = ptr->_StrLine._Chi2;
  NSeg  = ptr->_BaseS._NSeg;
  Pattern = ptr->_BaseS._Pattern;
#endif
}

TrClusterR::TrClusterR(AMSTrCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr-> _Id.cmpt();
  Status = ptr->_status;
  NelemL = ptr->_NelemL;
  NelemR = ptr->_NelemR;
  Sum    = ptr->_Sum;
  Sigma  = ptr->_Sigma;
  Mean   = ptr->_Mean;
  RMS    = ptr->_Rms;
  ErrorMean = ptr->_ErrorMean;
  for (int i=0; i<ptr->getnelem(); i++)Amplitude.push_back(ptr->_pValues[i]);
#endif
}

TrMCClusterR::TrMCClusterR(AMSTrMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft  = ptr->_idsoft;
  TrackNo = ptr->_itra;
  for (int i=0; i<2; i++) {
         Left[i]   = ptr->_left[i];
         Center[i] = ptr->_center[i];
         Right[i]  = ptr->_right[i];
  }
  for (int i=0; i<3; i++) {
    Xca[i] = ptr->_xca[i];
    Xcb[i] = ptr->_xcb[i];
    Xgl[i] = ptr->_xgl[i];
  }
  Sum = ptr->_sum;
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      SS[i][j] = ptr->_ss[i][j];
    }
  }
#endif
}

TrRawClusterR::TrRawClusterR(AMSTrRawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  address = ptr->_address+ptr->_strip*10000;
  nelem   = ptr->_nelem;
  s2n     = ptr->_s2n;
#endif
}

TrRecHitR::TrRecHitR(AMSTrRecHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  fTrClusterX = -1;
  fTrClusterY = -1;
  Status= ptr->_status;
  Layer = ptr->_Layer;
  for (int i=0; i<3; i++) Hit[i]  = ptr->_Hit[i];
  for (int i=0; i<3; i++) EHit[i] = ptr->_EHit[i];
  Sum     = ptr->_Sum;
  DifoSum = ptr->_DifoSum;
  CofgX   = ptr->_cofgx;
  CofgY   = ptr->_cofgy;
#endif
}

TrTrackR::TrTrackR(AMSTrTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status    = ptr->_status;
  Pattern   = ptr->_Pattern;
  Address   = ptr->_Address;
  NHits     = ptr->_NHits;

  LocDBAver       = ptr->_Dbase[0];
  GeaneFitDone    = ptr->_GeaneFitDone;
  AdvancedFitDone = ptr->_AdvancedFitDone;
  Chi2StrLine     = ptr->_Chi2StrLine;
  Chi2Circle      = ptr->_Chi2Circle;
  CircleRigidity  = ptr->_CircleRidgidity;
  Chi2FastFit     = ptr->_Chi2FastFit;
  Rigidity        = ptr->_Ridgidity;
  ErrRigidity     = ptr->_ErrRidgidity;
  Theta           = ptr->_Theta;
  Phi             = ptr->_Phi;
  for (int i=0; i<3; i++) P0[i] = ptr->_P0[i];
  GChi2           = (float)ptr->_GChi2;
  GRigidity       = (float)ptr->_GRidgidity;;
  GErrRigidity    = (float)ptr->_GErrRidgidity;
  for (int i=0; i<2; i++) {
        HChi2[i]        = (float)ptr->_HChi2[i];
        HRigidity[i]    = (float)ptr->_HRidgidity[i];
        HErrRigidity[i] = (float)ptr->_HErrRidgidity[i];
        HTheta[i]       = (float)ptr->_HTheta[i];
        HPhi[i]         = (float)ptr->_HPhi[i];
        for (int j=0; j<3; j++)  HP0[i][j] = (float)ptr->_HP0[i][j];
  }
  FChi2MS         = ptr->_Chi2MS;
  PiErrRig        = ptr->_PIErrRigidity;
  RigidityMS      = ptr->_RidgidityMS;
  PiRigidity      = ptr->_PIRigidity;

#endif
}


RichMCClusterR::RichMCClusterR(AMSRichMCHit *ptr, int _numgen){
#ifndef __ROOTSHAREDLIBRARY__
  id        = ptr->_id;
  for (int i=0; i<i; i++) {
   origin[i]    = ptr->_origin[i];
   direction[i] = ptr->_direction[i];
  }
  status       = ptr->_status;
  eventpointer = ptr->_hit;
  numgen       = _numgen;
#endif
}

RichHitR::RichHitR(AMSRichRawEvent *ptr, float x, float y){
#ifndef __ROOTSHAREDLIBRARY__
  if (ptr) {
   _channel = ptr->_channel;
   _counts  = ptr->_counts;
   _status  = ptr->_status;
   _npe     = ptr->getnpe();
   _x      = x;
   _y      = y;
  } else {
    cout<<"RICEventR -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
#endif
}

RichRingR::RichRingR(AMSRichRing *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    used  = ptr->_used;
    mused = ptr->_mused;
    beta  = ptr->_beta;
    errorbeta = ptr->_errorbeta;
    quality   = ptr->_quality;
    status    = ptr->_status;
    // betablind=ptr->_betablind;
    collected_npe= ptr->_collected_npe;
    npexp     = ptr->_npexp;
    probkl    = ptr->_probkl;
   
    npexpg    = ptr->_npexpg;
    npexpr    = ptr->_npexpr;
    npexpb    = ptr->_npexpb;

  } else {
    cout<<"RICRingR -E- AMSRichRing ptr is NULL"<<endl;
  }
#endif
}

EcalHitR::EcalHitR(AMSEcalHit *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Idsoft = ptr->_idsoft;
  Proj   = ptr->_proj;
  Plane  = ptr->_plane;
  Cell   = ptr->_cell;
  Edep   = ptr->_edep;
  EdCorr = ptr->_edepc;
  if (Proj) {
   Coo[0]= ptr->_cool;
   Coo[1]= ptr->_coot;
  }
  else{     //<-- x-proj
    Coo[0]= ptr->_coot;
    Coo[1]= ptr->_cool;
  }
  Coo[2]  = ptr->_cooz;

  AttCor  = ptr->_attcor;
  AMSECIdSoft ic(ptr->getid());
  ADC[0] = ptr->_adc[0];
  ADC[1] = ptr->_adc[1]*ic.gethi2lowr();
  ADC[2] = ptr->_adc[2]*ic.getan2dyr();

  Ped[0] = ic.getped(0);
  Ped[1] = ic.getped(1);
  Ped[2] = ic.getpedd();
  Gain   = ic.getgain();

#endif
}





#endif
