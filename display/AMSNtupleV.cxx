#include "AMSNtupleV.h"

char * AMSNtupleV::GetObjInfo(int px, int py){
static char* info=0;
int dist=99999;
info=0;
{
 int cand=-1;
 for(int i=0;i<fTofClusterV.size();i++){
   int current=fTofClusterV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTofClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fTrRecHitV.size();i++){
   int current=fTrRecHitV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrRecHitV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fAntiClusterV.size();i++){
   int current=fAntiClusterV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fAntiClusterV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fTrMCClusterV.size();i++){
   int current=fTrMCClusterV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrMCClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fTrdClusterV.size();i++){
   int current=fTrdClusterV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdClusterV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fEcalClusterV.size();i++){
   int current=fEcalClusterV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fEcalClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fRichHitV.size();i++){
   int current=fRichHitV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichHitV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fEcalShowerV.size();i++){
   int current=fEcalShowerV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fEcalShowerV[cand].GetObjectInfo(px,py);
}


if(info)return info;







{
 int cand=-1;
 for(int i=0;i<fTrTrackV.size();i++){
   int current=fTrTrackV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrTrackV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fTrdTrackV.size();i++){
   int current=fTrdTrackV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdTrackV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fRichRingV.size();i++){
   int current=fRichRingV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichRingV[cand].GetObjectInfo(px,py);
}
{
 int cand=-1;
 for(int i=0;i<fMCEventgV.size();i++){
   int current=fMCEventgV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fMCEventgV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fParticleV.size();i++){
   int current=fParticleV[i].DistancetoPrimitive(px,py);
  if(current<dist){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fParticleV[cand].GetObjectInfo(px,py);
}




return info;
}

void AMSNtupleV::Prepare( EAMSType type){

if(type==kall || type==kusedonly || type==kanticlusters){
 fAntiClusterV.clear();
 if(gAMSDisplay->DrawObject(kanticlusters)){
  for(int i=0;i<NAntiCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pAntiCluster(i)->Status)/32)%2)fAntiClusterV.push_back( AntiClusterV(this,i));
  }
 }
}


if(type==kall || type==kusedonly || type==ktofclusters){
 fTofClusterV.clear();
 if(gAMSDisplay->DrawObject(ktofclusters)){
  for(int i=0;i<NTofCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTofCluster(i)->Status)/32)%2)fTofClusterV.push_back( TofClusterV(this,i));
  }
 }
}


if(type==kall || type==kusedonly || type==ktrclusters){
 fTrRecHitV.clear();
 if(gAMSDisplay->DrawObject(ktrclusters)){
  for(int i=0;i<NTrRecHit();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrRecHit(i)->Status)/32)%2)fTrRecHitV.push_back( TrRecHitV(this,i));
  }
 }
}



if(type==kall || type==kusedonly || type==krichhits){
 fRichHitV.clear();
 if(gAMSDisplay->DrawObject(krichhits)){
  for(int i=0;i<NRichHit();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pRichHit(i)->Status)/32)%2)fRichHitV.push_back( RichHitV(this,i));
  }
 }
}


if(type==kall || type==kusedonly || type==ktrdclusters){
 fTrdClusterV.clear();
 if(gAMSDisplay->DrawObject(ktrdclusters)){
  for(int i=0;i<NTrdCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrdCluster(i)->Status)/32)%2)fTrdClusterV.push_back( TrdClusterV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==kecalclusters){
 fEcalClusterV.clear();
 if(gAMSDisplay->DrawObject(kecalclusters)){
  for(int i=0;i<NEcalCluster();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pEcalCluster(i)->Status)/32)%2){
     if(pEcalCluster(i)->Edep>0)fEcalClusterV.push_back( EcalClusterV(this,i));
   }
  }
 }
}


if(type==kall || type==kusedonly || type==ktrtracks){
 fTrTrackV.clear();
 if(gAMSDisplay->DrawObject(ktrtracks)){
  for(int i=0;i<NTrTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->Status)/32)%2){
     if(pTrTrack(i)->IsGood()){
       fTrTrackV.push_back( TrTrackV(this,i));
     }
   }
  }
  for(int i=0;i<NTrTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->Status)/32)%2){
     if(!pTrTrack(i)->IsGood())fTrTrackV.push_back( TrTrackV(this,i));
   }
  }

 }
}


if(type==kall || type==kusedonly || type==ktrdtracks){
 fTrdTrackV.clear();
 if(gAMSDisplay->DrawObject(ktrdtracks)){
  for(int i=0;i<NTrdTrack();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pTrdTrack(i)->Status)/32)%2)fTrdTrackV.push_back( TrdTrackV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==kecalshowers){
 fEcalShowerV.clear();
 if(gAMSDisplay->DrawObject(kecalshowers)){
  for(int i=0;i<NEcalShower();i++){
   if(!gAMSDisplay->DrawUsedOnly() || ((pEcalShower(i)->Status)/32)%2)fEcalShowerV.push_back( EcalShowerV(this,i));
  }
 }
}

if(type==kall || type==kusedonly || type==krichrings){
 fRichRingV.clear();
 if(gAMSDisplay->DrawObject(krichrings)){
  for(int i=0;i<NRichRing();i++){
  // if(!gAMSDisplay->DrawUsedOnly() || ((pRichRing(i)->Status)/32)%2)fRichRingV.push_back( RichRingV(this,i));
  }
 }
}

if(type==kall ||  type==kmcinfo){
 fTrMCClusterV.clear();
 if(gAMSDisplay->DrawObject(kmcinfo)){
  for(int i=0;i<NTrMCCluster();i++){
    fTrMCClusterV.push_back( TrMCClusterV(this,i));
  }
 }
}

if(type==kall ||  type==kmcinfo){
 fMCEventgV.clear();
 if(gAMSDisplay->DrawObject(kmcinfo)){
  for(int i=0;i<NMCEventg();i++){
    fMCEventgV.push_back( MCEventgV(this,i));
  }
 }
}



if(type==kall || type==kparticles){
 fParticleV.clear();
 if(gAMSDisplay->DrawObject(kparticles)){
  for(int i=0;i<NParticle();i++){
   fParticleV.push_back( ParticleV(this,i));
  }
 }
}


}


void AMSNtupleV::Draw( EAMSType type){


 for(int i=0;i<fRichRingV.size();i++){
   fRichRingV[i].AppendPad();
  }


 for(int i=0;i<fTrTrackV.size();i++){
   fTrTrackV[i].AppendPad();
  }

 for(int i=0;i<fTrdTrackV.size();i++){
   fTrdTrackV[i].AppendPad();
  }

 for(int i=0;i<fParticleV.size();i++){
  fParticleV[i].AppendPad();
 }

 for(int i=0;i<fEcalClusterV.size();i++){
   fEcalClusterV[i].AppendPad();
  }

 for(int i=0;i<fEcalShowerV.size();i++){
   fEcalShowerV[i].AppendPad();
  }

 for(int i=0;i<fTofClusterV.size();i++){
   fTofClusterV[i].AppendPad();
  }

 for(int i=0;i<fAntiClusterV.size();i++){
   fAntiClusterV[i].AppendPad();
  }

 for(int i=0;i<fTrdClusterV.size();i++){
   fTrdClusterV[i].AppendPad();
  }


 for(int i=0;i<fRichHitV.size();i++){
   fRichHitV[i].AppendPad();
  }


 for(int i=0;i<fTrRecHitV.size();i++){
   fTrRecHitV[i].AppendPad();
  }

 for(int i=0;i<fTrMCClusterV.size();i++){
   fTrMCClusterV[i].AppendPad();
  }

 for(int i=0;i<fMCEventgV.size();i++){
   fMCEventgV[i].AppendPad();
  }


}


bool AMSNtupleV::GetEvent(unsigned int run, unsigned int event){
int entry=0;
if(Run()==run && Event() <event)entry=fCurrentEntry;
while(ReadOneEvent(entry++)!=-1){
 if(Run() == run && Event()>=event)return true;
}
return false;
}
