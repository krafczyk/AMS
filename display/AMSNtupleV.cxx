//  $Id$
#include "AMSNtupleV.h"
#include "TCONE.h"
#include "TNode.h"
#include "amschain.h"
ClassImp(AMSNtupleV)       
void* gAMSUserFunction;

  char * RichRingBV::GetObjectInfo(Int_t px, Int_t py) const{
   return fRef>=0 && fEv->pRichRingB(fRef)?fEv->pRichRingB(fRef)->Info(fRef):0;
}


char * AMSNtupleV::GetObjInfo(int px, int py){
static char* info=0;
int dist=99999;
info=0;

{
 int cand=-1;
 for(int i=0;i<fDaqV.size();i++){
   int current=fDaqV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fDaqV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fTrigger1V.size();i++){
   int current=fTrigger1V[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fTrigger1V[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fTrigger3V.size();i++){
   int current=fTrigger3V[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fTrigger3V[cand].GetObjectInfo(px,py);
}
{
 int cand=-1;
 for(int i=0;i<fHeaderV.size();i++){
   int current=fHeaderV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
  if(dist<7 && cand>=0)info=fHeaderV[cand].GetObjectInfo(px,py);
}

  if(info)return info; 



{
 int cand=-1;
 for(int i=0;i<fTofClusterV.size();i++){
   int current=fTofClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdClusterV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fEcalShowerV.size();i++){
   int current=fEcalShowerV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fEcalShowerV[cand].GetObjectInfo(px,py);
}



{
 int cand=-1;
 for(int i=0;i<fRichHitV.size();i++){
   int current=fRichHitV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichHitV[cand].GetObjectInfo(px,py);
}




if(info)return info;
else dist=1000000;

{
 int cand=-1;
 for(int i=0;i<fEcalClusterV.size();i++){
   int current=fEcalClusterV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
   //cout <<"  cluster "<<i<<dist<<" "<<current<<endl;
  }
 }
 if(dist<7 && cand>=0)info=fEcalClusterV[cand].GetObjectInfo(px,py);
}





{
 int cand=-1;
 for(int i=0;i<fTrTrackV.size();i++){
   int current=fTrTrackV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdTrackV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fTrdHTrackV.size();i++){
   int current=fTrdHTrackV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fTrdHTrackV[cand].GetObjectInfo(px,py);
}


{
 int cand=-1;
 for(int i=0;i<fRichRingV.size();i++){
   int current=fRichRingV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0)info=fRichRingV[cand].GetObjectInfo(px,py);
}

{
 int cand=-1;
 for(int i=0;i<fRichRingBV.size();i++){
   int current=fRichRingBV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
   dist=current;
   cand=i;
  }
 }
 if(dist<7 && cand>=0){
   
   info=fRichRingBV[cand].GetObjectInfo(px,py);
   //cout <<" cand "<<cand<<info<<endl;
 } 
}


{
 int cand=-1;
 for(int i=0;i<fMCEventgV.size();i++){
   int current=fMCEventgV[i].DistancetoPrimitive(px,py);
  if(abs(current)<abs(dist)){
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
  if(abs(current)<abs(dist)){
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
#ifdef __USEANTICLUSTERPG__
      if(gAMSDisplay->RebuildACC()){
	int nold=NAntiCluster();
	int n=RebuildAntiClusters();
	static int c=0;
	const int maxc=100;
	if (c<maxc && n!=nold) {
	  cerr<<"AMSNtupleV-W-NACC changed was "<<nold<<" now "<<n<<endl;
	  c++;
	}
      }
#endif
      for(int i=0;i<NAntiCluster();i++){
	if (!gAMSDisplay->DrawUsedOnly()) {
#ifdef __USEANTICLUSTERPG__
	  if (!(pAntiCluster(i)->Status)) {
#else
	  if (!(((pAntiCluster(i)->Status)/32)%2)) { 
#endif
	    fAntiClusterV.push_back(AntiClusterV(this,i));
	  }
	}
      }
    }
  }  
  
if(type==kall){
  fDaqV.clear();
  for(int i=0;i<NDaqEvent();i++){
   fDaqV.push_back( DaqV(this,i));
  }

  fTrigger1V.clear();
  for(int i=0;i<NLevel1();i++){
   fTrigger1V.push_back( Trigger1V(this,i));
  }
  fTrigger3V.clear();
  for(int i=0;i<NLevel3();i++){
   fTrigger3V.push_back( Trigger3V(this,i));
  }
  fHeaderV.clear();
  for(int i=0;i<1;i++){
   fHeaderV.push_back( HeaderV(this,i));
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


 if(type==kall || type==kusedonly || type==ktrclusters || type==ktrclustersM|| type==ktrclustersY){
   fTrRecHitV.clear();
   if(gAMSDisplay->DrawObject(ktrclusters)){
     for(int i=0;i<NTrRecHit();i++){
#ifdef _PGTRACK_
       bool Mult=GetTkMult();
       bool Used=(pTrRecHit(i)->getstatus()/32)%2;
       bool Yonly=pTrRecHit(i)->OnlyY();
       TrClusterR *py=pTrRecHit(i)-> pTrCluster('y');
       TrClusterR *px=0;
       if(!Yonly)px=pTrRecHit(i)->pTrCluster('x');
        if(py && !TkDBc::Head->FindTkId(py->GetTkId()))continue;
        if(px && !TkDBc::Head->FindTkId(px->GetTkId()))continue;
       int rm=pTrRecHit(i)->GetResolvedMultiplicity();
       if(!gAMSDisplay->DrawUsedOnly() || Used) 
 	 if ((Yonly && GetTkDispY()) || (!Yonly)){
	   //	   if(Mult && !Used ){
	   if(Mult ){
	     for (int jj=0;jj<pTrRecHit(i)->GetMultiplicity();jj++)
	       fTrRecHitV.push_back( TrRecHitV(this,i,jj));
	   }else
	     fTrRecHitV.push_back( TrRecHitV(this,i,(rm>-1)?rm:0));
	 }
       
#else
       if(!gAMSDisplay->DrawUsedOnly() || ((pTrRecHit(i)->Status)/32)%2)fTrRecHitV.push_back( TrRecHitV(this,i));
#endif
       
     }
   }
 }
 

if(type==kall || type==kusedonly || type==krichhits){
 fRichHitV.clear();
 if(gAMSDisplay->DrawObject(krichhits)){
  for(int i=0;i<NRichHit();i++){
   if(!gAMSDisplay->DrawUsedOnly() || (pRichHit(i)->Status)%1024)fRichHitV.push_back( RichHitV(this,i));
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
     fEcalClusterV.push_back( EcalClusterV(this,i));
   }
  }
 }
}


if(type==kall || type==kusedonly || type==ktrtracks){
 fTrTrackV.clear();
 if(gAMSDisplay->DrawObject(ktrtracks)){
  for(int i=0;i<NTrTrack();i++){
#ifdef _PGTRACK_
    if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->getstatus())/32)%2){
      fTrTrackV.push_back( TrTrackV(this,i));
    }
#else
    if(!gAMSDisplay->DrawUsedOnly() || ((pTrTrack(i)->Status)/32)%2){
      if(pTrTrack(i)->IsGood()  ||  (pTrTrack(i)->Status/32)%2){
	fTrTrackV.push_back( TrTrackV(this,i));
      }
    }
#endif
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

 if(type==kall || type==kusedonly || type==ktrdhtracks){
  fTrdHTrackV.clear();
  if(gAMSDisplay->DrawObject(ktrdhtracks)){
    for(int i=0;i<NTrdHTrack();i++){
      if(!gAMSDisplay->DrawUsedOnly() )fTrdHTrackV.push_back( TrdHTrackV(this,i));
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
 fRichRingBV.clear();
 if(gAMSDisplay->DrawObject(krichrings)){
  for(int i=0;i<nRichRing();i++){
   if( (!gAMSDisplay->DrawUsedOnly() || ((pRichRing(i)->Status)/32)%2)){
    fRichRingV.push_back( RichRingV(this,i,false));
    if(gAMSDisplay->DrawRichRingsFromPlex())fRichRingV.push_back( RichRingV(this,i,true));
   }
    for(int i=0;i<NRichRingB();i++){
      if( (!gAMSDisplay->DrawUsedOnly() && (pRichRingB(i)->Status)%10==2 )){
	//if( !gAMSDisplay->DrawUsedOnly()){
	fRichRingBV.push_back( RichRingBV(this,i,false));
        if(gAMSDisplay->DrawRichRingsFromPlex())fRichRingBV.push_back( RichRingBV(this,i,true));
      }
    }
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

if(type==kall ||  type==kmcinfo || type==kusedonly){
 fMCEventgV.clear();
 if(gAMSDisplay->DrawObject(kmcinfo)){
  for(int i=0;i<NMCEventg();i++){
   if(gAMSDisplay->DrawUsedOnly() && GetPrimaryMC()==pMCEventg(i)){
    fMCEventgV.push_back( MCEventgV(this,i));
   }
   else if(!gAMSDisplay->DrawUsedOnly()){
    fMCEventgV.push_back( MCEventgV(this,i));
   }
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

for(int i=0;i<fDaqV.size();i++){
   fDaqV[i].AppendPad();
  }


 for(int i=0;i<fTrigger1V.size();i++){
   fTrigger1V[i].AppendPad();
  }
 for(int i=0;i<fTrigger3V.size();i++){
   fTrigger3V[i].AppendPad();
  }

 for(int i=0;i<fHeaderV.size();i++){
   fHeaderV[i].AppendPad();
  }




 for(int i=0;i<fTrTrackV.size();i++){
   fTrTrackV[i].AppendPad();
  }

 for(int i=0;i<fTrdTrackV.size();i++){
   fTrdTrackV[i].AppendPad();
  }

 for(int i=0;i<fTrdHTrackV.size();i++){
   fTrdHTrackV[i].AppendPad();
  }

 for(int i=0;i<fParticleV.size();i++){
  fParticleV[i].AppendPad();
 }

 for(int i=0;i<fEcalClusterV.size();i++){
   fEcalClusterV[i].AppendPad();
  }     
 for(int i=0;i<fRichRingV.size();i++){
   fRichRingV[i].AppendPad();
  }

 for(int i=0;i<fRichRingBV.size();i++){
   fRichRingBV[i].AppendPad();
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
 if(!run)run=Run();
// if(Run()==run   && Event() >=event){
//   gAMSDisplay->getchain()->Rewind();
// }



long long ent=gAMSDisplay->getchain()->GetEntryNo(run,event);
if(ent>=0){
 gAMSDisplay->getchain()->ReadOneEvent(ent);
 if(Run()==run && Event()==event)return true;
}

entry=gAMSDisplay->getchain()->Entry();

//Add Binary seach 
//gAMSDisplay->getchain()->ReadOneEvent(entry);
unsigned int fevent=Event();
unsigned int frun=Run();
int starti,endi;
int startevt,endevt;
//Find given run
int i1;
for(i1=0;i1<gAMSDisplay->m_chain_Runs.size();i1++){
    if(run==gAMSDisplay->m_chain_Runs[i1]){
        break;
    }
}
if(i1==gAMSDisplay->m_chain_Runs.size())
    return false;
starti=gAMSDisplay->m_chain_EntryIndex[i1];
//Find same runs, assume the same run's root file are continuous
//one run may have two root files
i1++;
for(;i1<gAMSDisplay->m_chain_Runs.size();i1++){
//    cout<<i1<<endl;
    if(run!=gAMSDisplay->m_chain_Runs[i1])
        break;
}
endi=gAMSDisplay->m_chain_EntryIndex[i1]-1;
//check binary's maximum search time
int maxn=int(ceil(log2(endi-starti)))+2;
//Update root setup if search a different run
//if(frun!=run){
AMSEventR::ProcessSetup=1; 
UpdateSetup(run);
//}
//if the event is very close to current entry, we just try one by one
if(maxn>fabs(event-fevent)){
    int inc=-1;
    if(fevent<event)
        inc=1;
    while(gAMSDisplay->getchain()->ReadOneEvent(entry)!=-1&&Run()==run&&entry>=0&&entry<gAMSDisplay->m_chain_Entries){
        if(inc*Event()>=inc*event)return true;
        entry+=inc;
    }
    return true;
}
else{   
    gAMSDisplay->getchain()->ReadOneEvent(starti);
    startevt=Event();
    if(Event()>=event){
        return true;
    }
    int endi2=starti+(event-startevt);
    if(endi2<endi)
        endi=endi2;
    gAMSDisplay->getchain()->ReadOneEvent(endi);
    endevt=Event();
    if(Event()<=event){
        return true;
    }
    
    //cout<<"start event= "<<
    while(endi>starti){
        int middle=int(ceil(0.5*(starti+endi)));
        gAMSDisplay->getchain()->ReadOneEvent(middle);
        //cout<<"starti= "<<starti<<", middle= "<<middle<<" endi= "<<endi<<" ev= "<<Event()<<" search= "<<event<<endl;
        if(Event()<event){
            starti=middle;
	    startevt=Event();
	}
        else if(Event()>event){
            endi=middle;
	    endevt=Event();
	}
        else
            return true;      
    	if(starti==endi-1){
		if(endevt!=event&&startevt!=event){
			if(fevent<event)
				gAMSDisplay->getchain()->ReadOneEvent(endi);	
			else
				gAMSDisplay->getchain()->ReadOneEvent(starti);
		}
		return true;
	}
    }
    return true;
}

//int prcs=AMSEventR::ProcessSetup;
//
//if(run!=Run())AMSEventR::ProcessSetup=1; 
//
//while(gAMSDisplay->getchain()->ReadOneEvent(entry++)!=-1){
//   if(Run()==run && AMSEventR::ProcessSetup!=prcs){
//       AMSEventR::ProcessSetup=prcs;
//       UpdateSetup(Run());
//   }
//   //if()
//   //cout<<"entry= "<<entry<<", event= "<<Event()<<", run= "<<Run()<<endl;
//  if(Run() == run && Event()>=event)return true;
// }
 return false;
 }


#include "TRotation.h"


RichRingV::RichRingV(AMSEventR *ev,int ref, bool drawplex):AMSDrawI(ev,ref),TPolyLine3D(){
 RichRingR *pcl=ev->pRichRing(ref);
//
// at the moment only rich rings ass with particles will be drawn
//
//  This should go to RichRingR  as soon as the latter will be updated
//  (added theta, radiator etc) by Carlos
//
static TNode *mirror=gAMSDisplay->GetGeometry()->GetNode("OMIR1");
static TNode *rich=gAMSDisplay->GetGeometry()->GetNode("RICH1");
           TCONE * pcone= (TCONE*)mirror->GetShape();
           double r1=pcone->GetRmin2();
           double r2=pcone->GetRmin();
           double dz=pcone->GetDz();      
           double xc=mirror->GetX();
           double yc=mirror->GetY();
           double zc=mirror->GetZ();
           double tc=(r1-r2)/2/dz;
           zc+=rich->GetZ();
           double  z2=zc-dz;
/*           cout <<" tc "<<tc<<" "<<z2<<endl;
             cout <<r1<<endl;           
             cout <<r2<<endl;           
             cout <<dz<<endl;           
             cout <<xc<<endl;           
             cout <<yc<<endl;           
             cout <<zc<<endl;           
             cout <<tc<<endl;           
*/
  const int npointm=360/5+1;
  int npoint=npointm;
  float array[3*npointm];
  double rad_thick;
  const double rad_length=-3;
  double n_aero;
  if(ev->Version()>0 && ev->Version()<130){
    n_aero=1.02998;
  }
  else n_aero=1.0529;
  const double n_naf=1.33;
  const double n_naf_Spread=0.01; 
//cout <<"   version "<<ev->Version()<<endl;
   if(ev->Version()>0 && ev->Version() <89){
  double refi;
   double rad_posz;
for(int i=0;i<ev->nParticle();i++){
 if( ev->pParticle(i)->iRichRing() == ref){
  if(fabs(ev->pParticle(i)->RichCoo[0][0])<11.3*3/2 && fabs(ev->pParticle(i)->RichCoo[0][1])<11.3*3/2){
    refi=n_naf;
    rad_posz=-2.5;
    rad_thick=-0.5;
  }
  else {
   refi=n_aero;
   rad_posz=0;
   rad_thick=-3;
  }
  double    cc=1./ev->pRichRing(ref)->Beta/refi;
//   cout <<" refi "<<refi<<" "<<cc<<" "<<ev->pParticle(i)->RichCoo[0][0]<<" "<<ev->pParticle(i)->RichCoo[0][1]<<" "<<ev->pParticle(i)->RichCoo[0][2]<<" "<<ev->pParticle(i)->RichCoo[1][2]<<" "<<ev->pRichRing(ref)->Beta<<endl;
  if(cc<1){
   double theta=acos(cc);
    TVector3 z(ev->pParticle(i)->RichCoo[1][0]-ev->pParticle(i)->RichCoo[0][0],ev->pParticle(i)->RichCoo[1][1]-ev->pParticle(i)->RichCoo[0][1],ev->pParticle(i)->RichCoo[1][2]-ev->pParticle(i)->RichCoo[0][2]);
    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
   double posz=rad_posz+rad_thick*float(rand())/RAND_MAX;
   double thick=rad_length-rad_posz;  
    rcoo[0]=ev->pParticle(i)->RichCoo[0][0]+z.X()/z.Z()*posz;
    rcoo[1]=ev->pParticle(i)->RichCoo[0][1]+z.Y()/z.Z()*posz;
    rcoo[2]=ev->pParticle(i)->RichCoo[0][2]+posz;
    phi+=dphi;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    double plex_thick =0.1;
    const double n_plex=1.49;
    {
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
      k=k-1;
      if(!(npoint--))break;
      continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     double zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
//    cout <<" after "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<" "<<ray.Phi()<<" "<<ray.Theta()<<endl;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    array[k*3+2]=ev->pParticle(i)->RichCoo[1][2]+3.2;
    double rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    while(rp>rc   && ray.Z()<0){
 
      zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
      }           
      //cout <<"  got iteration "<<rcoo[0]<< " "<<rcoo[1]<<" "<<rcoo[2]<< " "<<rp2 <<" "<<rc2<<endl;
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
//     cout <<"  normal to cone "<<cu<<" "<<cv<<" "<<cw<<endl;
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     array[k*3+0]=rcoo[0]+ru/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
      k--;
      if(!(npoint--))break;
//      cout <<"removing point point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
      continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
      k--;
      if(!(npoint--))break;
      continue;
     }
    //cout <<" ray "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<endl;
    //cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   

   SetPolyLine(npoint,array);
   SetLineColor(6);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   return;
   }
   }
   }
   return;
   }
   else{
    const double plex_thick =0.1;
    const double n_plex=1.49;
    double shift=0;
   double refi=pcl->Beta*cos(pcl->Theta);
    refi=1/refi;
  if(refi>1.1){
    rad_thick=-0.5;
  }
  else {
  if(ev->Version()>0 && ev->Version()<130){
   rad_thick=-3;
  }
  else rad_thick=-2.5;
  }
  if(drawplex){
      refi=n_plex;
      shift=-rad_thick/2; 
      rad_thick=-0.1;
      shift+=-rad_thick/2; 
  }    
   //cout << "  rad_thick "<<rad_thick<<" "<<refi<<pcl->TrPMTPos[2]-pcl->TrRadPos[2]<<endl;
   double theta=pcl->Theta;

    TVector3 z(pcl->TrPMTPos[0]-pcl->TrRadPos[0],pcl->TrPMTPos[1]-pcl->TrRadPos[1],pcl->TrPMTPos[2]-pcl->TrRadPos[2]);
    //cout << "pcl->TrRadPos[0] = " << pcl->TrRadPos[0] << " pcl->TrRadPos[1] = " << pcl->TrRadPos[1] << " pcl->TrRadPos[2] = " << pcl->TrRadPos[2] << endl;
    //cout << "pcl->TrPMTPos[0] = " << pcl->TrPMTPos[0] << " pcl->TrPMTPos[1] = " << pcl->TrPMTPos[1] << " pcl->TrPMTPos[2] = " << pcl->TrPMTPos[2] << endl;
    //cout << pcl->TrRadPos[2]<<"  "<<pcl->TrPMTPos[2]<<endl;
    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
    //cout << "  k npoint "<<k<<" "<<npoint<<endl;
   double posz=rad_thick*(float(rand())/RAND_MAX-0.5);
   double thick=rad_thick*0.5-posz;  
    rcoo[0]=pcl->TrRadPos[0]+z.X()/z.Z()*posz;
    rcoo[1]=pcl->TrRadPos[1]+z.Y()/z.Z()*posz;
    rcoo[2]=pcl->TrRadPos[2]+posz;
    phi+=dphi;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    if( !drawplex){
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
            k--;
      if(!(npoint--))break;
      else continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     double addon=4;
     if(ev->Version()<0 || ev->Version()>160){
        addon=0;
     }
    double rp=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
    if(rp>rc){
      k--;
      if(!(npoint--))break;
      else continue;
    }
    double zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    array[k*3+2]=pcl->TrPMTPos[2]+addon;
    rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    const int miter=1000;
    int iter=0;
    while(rp>rc  && ray.Z()<0 && iter++<miter){
      zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
      //cout <<"  mirror found "<<k<<" "<<rp<<" "<<rc<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<zl<<endl;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
        
      }           
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     array[k*3+0]=rcoo[0]+ru/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
            k--;
      if(!(npoint--))break;
      else continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
            k--;
      if(!(npoint--))break;
      else continue;
     }
     //cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   
}
   SetPolyLine(npoint,array);
   SetLineColor(6);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   if(drawplex)SetLineStyle(2);
   return;
}


RichRingBV::RichRingBV(AMSEventR *ev,int ref, bool drawplex):AMSDrawI(ev,ref),TPolyLine3D(){

RichRingBR *pcl=ev->pRichRingB(ref);
//
// at the moment only rich rings ass with particles will be drawn
//
//  This should go to RichRingR  as soon as the latter will be updated
//  (added theta, radiator etc) by Carlos
//

static TNode *mirror=gAMSDisplay->GetGeometry()->GetNode("OMIR1");
static TNode *rich=gAMSDisplay->GetGeometry()->GetNode("RICH1");

           TCONE * pcone= (TCONE*)mirror->GetShape();
           double r1=pcone->GetRmin2();
           double r2=pcone->GetRmin();
           double dz=pcone->GetDz();      
           double xc=mirror->GetX();
           double yc=mirror->GetY();
           double zc=mirror->GetZ();
           double tc=(r1-r2)/2/dz;
           zc+=rich->GetZ();
           double  z2=zc-dz;
//            cout <<" tc "<<tc<<" "<<z2<<endl;
//              cout <<r1<<endl;           
//              cout <<r2<<endl;           
//              cout <<dz<<endl;           
//              cout <<xc<<endl;           
//              cout <<yc<<endl;           
//              cout <<zc<<endl;           
//              cout <<tc<<endl;           
//
  const int npointm=360/5+1;
  int npoint=npointm;
  float array[3*npointm];
  double rad_thick;
  const double rad_length=-3;
  double n_aero;
  if(ev->Version()>0 && ev->Version()<130){
    n_aero=1.02998;
  }
  else n_aero=1.0529;
  const double n_naf=1.33;
  const double n_naf_Spread=0.01; 

   if(ev->Version()>0 && ev->Version() <89){
   double refi;
   double rad_posz;
   //   //for(int i=0;i<ev->nParticle();i++){
   //  // if( ev->pParticle(i)->iRichRing() == ref){

   
      for(int i=0;i<ev->nParticle();i++){
	if(ev->pParticle(i)->pTrTrack()!=NULL){
	  for (int nLIP=0; nLIP< (ev->nRichRingB()); nLIP++) {
	    RichRingBR ringLIP = ev->RichRingB(nLIP);
	    if( ((ringLIP.Status)%10) ==2 && (ringLIP.iTrTrack())==ref){
	      //if( (ringLIP.iTrTrack())==ref){
	 
      
 if(fabs(ev->pParticle(i)->RichCoo[0][0])<11.3*3/2 && fabs(ev->pParticle(i)->RichCoo[0][1])<11.3*3/2){

    refi=n_naf;
    rad_posz=-2.5;
    rad_thick=-0.5;
 }
  else {
   refi=n_aero;
   rad_posz=0;
   rad_thick=-3;
  }
  double    cc=1./ev->pRichRingB(ref)->Beta/refi;
//   cout <<" refi "<<refi<<" "<<cc<<" "<<ev->pParticle(i)->RichCoo[0][0]<<" "<<ev->pParticle(i)->RichCoo[0][1]<<" "<<ev->pParticle(i)->RichCoo[0][2]<<" "<<ev->pParticle(i)->RichCoo[1][2]<<" "<<ev->pRichRing(ref)->Beta<<endl;
  if(cc<1){
   double theta=acos(cc);
   TVector3 z(ev->pParticle(i)->RichCoo[1][0]-ev->pParticle(i)->RichCoo[0][0],ev->pParticle(i)->RichCoo[1][1]-ev->pParticle(i)->RichCoo[0][1],ev->pParticle(i)->RichCoo[1][2]-ev->pParticle(i)->RichCoo[0][2]);


    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
   double posz=rad_posz+rad_thick*float(rand())/RAND_MAX;
   double thick=rad_length-rad_posz;  
    rcoo[0]=ev->pParticle(i)->RichCoo[0][0]+z.X()/z.Z()*posz;
    rcoo[1]=ev->pParticle(i)->RichCoo[0][1]+z.Y()/z.Z()*posz;
    rcoo[2]=ev->pParticle(i)->RichCoo[0][2]+posz;
    phi+=dphi;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    double plex_thick =0.1;
    const double n_plex=1.49;
    {
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
      k=k-1;
      if(!(npoint--))break;
      continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     double zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
//    cout <<" after "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<" "<<ray.Phi()<<" "<<ray.Theta()<<endl;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    array[k*3+2]=ev->pParticle(i)->RichCoo[1][2]+3.2;
    double rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    while(rp>rc   && ray.Z()<0){
 
      zl=ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
      }           
      cout <<"  got iteration "<<rcoo[0]<< " "<<rcoo[1]<<" "<<rcoo[2]<< " "<<rp2 <<" "<<rc2<<endl;
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
//     cout <<"  normal to cone "<<cu<<" "<<cv<<" "<<cw<<endl;
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     array[k*3+0]=rcoo[0]+ru/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
      k--;
      if(!(npoint--))break;
//      cout <<"removing point point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
      continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
      k--;
      if(!(npoint--))break;
      continue;
     }
    //cout <<" ray "<<ray.X()<<" "<<ray.Y()<<" "<<ray.Z()<<endl;
    //cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   

   SetPolyLine(npoint,array);
   SetLineColor(1);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   return;
  }
	    }
	  }
	}
      }
   return;
   }
   else{
    const double plex_thick =0.1;
    const double n_plex=1.49;
    double shift=0;
    double refi=pcl->Beta*cos(pcl->AngleRec);
    refi=1/refi;
    float fracemission;
  if(refi>1.1){
    rad_thick=-0.5;
    fracemission = 0.5;
  }
  else {
  if(ev->Version()>0 && ev->Version()<130){
   rad_thick=-3;
    fracemission = 0.6;
  }
  else{
    rad_thick=-2.5;
    fracemission = 0.6;
  }
  }
  if(drawplex){
      refi=n_plex;
      shift=-rad_thick/2; 
      rad_thick=-0.1;
      shift+=-rad_thick/2; 
  }    

  double theta=(double)(pcl->AngleRec);
  //cout << " theta ck = " << pcl->AngleRec << "  " << refi << endl;

  float HRAD      =  2.5; //cm
  //float ZTMIRGAP  =  0.1;
  //float HMIR      = 46.32;
  //float ZBMIRGAP  =  0.5;
  float HEXPANSION = 47.02;
  //   float ZLGSIGNAL =  1.8;
  float ZTOPRAD = -72.37;

  float ximp,yimp,zimp;
  float xemission,yemission, zemission;
  float deltaz = - (HRAD + rad_thick) + fracemission*rad_thick;

  if(pcl->TrackRec[4]==0){
    ximp = pcl->TrackRec[0];
    yimp = pcl->TrackRec[2];
    zimp = ZTOPRAD - pcl->TrackRec[4];
    xemission = ximp - deltaz*sin(pcl->TrackRec[6])*cos(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
    yemission = yimp - deltaz*sin(pcl->TrackRec[6])*sin(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
    zemission = ZTOPRAD + deltaz;
  }else{
    ximp = pcl->TrackRec[0]-pcl->TrackRec[4]*sin(pcl->TrackRec[6])*cos(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
    yimp = pcl->TrackRec[2]-pcl->TrackRec[4]*sin(pcl->TrackRec[6])*sin(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
    zimp = ZTOPRAD ;	
    xemission = pcl->TrackRec[0];
    yemission = pcl->TrackRec[2];
    zemission = ZTOPRAD - pcl->TrackRec[4];
  }
   float zdet = ZTOPRAD-(HRAD + HEXPANSION);
   //   float ximp = pcl->TrackRec[0];
   //   float yimp = pcl->TrackRec[2];
   //   float zimp = ZTOPRAD - pcl->TrackRec[4];
   //float zimp = pcl->TrackRec[4];

   ////float xemission = ximp + 0.6*(HRAD-zimp)*sin(pcl->TrackRec[6])*cos(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   ////float yemission = yimp + 0.6*(HRAD-zimp)*sin(pcl->TrackRec[6])*sin(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   //   float deltaz = - (HRAD + rad_thick) + fracemission*rad_thick;
   //   float xemission = ximp - deltaz*sin(pcl->TrackRec[6])*cos(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   //   float yemission = yimp - deltaz*sin(pcl->TrackRec[6])*sin(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   //   float zemission = ZTOPRAD + deltaz;
   //cout << "xemission = " << xemission << " yemission = " << yemission << " zemission = " << zemission << endl;

   float xdet = ximp - (zdet-zimp)*sin(pcl->TrackRec[6])*cos(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   float ydet = yimp - (zdet-zimp)*sin(pcl->TrackRec[6])*sin(pcl->TrackRec[8])/cos(pcl->TrackRec[6]);
   //cout << "xdet = " << xdet << " ydet = " << ydet << " zdet = " << zdet << endl;
   //   TVector3 z(pcl->TrPMTPos[0]-pcl->TrRadPos[0],pcl->TrPMTPos[1]-pcl->TrRadPos[1],pcl->TrPMTPos[2]-pcl->TrRadPos[2]);

   TVector3 z(xdet-xemission,ydet-yemission,zdet-zemission);

    //cout << pcl->TrRadPos[2]<<"  "<<pcl->TrPMTPos[2]<<endl;
    double rcoo[3];
    TRotation r;
    r.SetZAxis(z);
   double dphi=2*3.1415926/(npoint-1);
   double phi=-dphi;
   for( int k=0;k<npoint;k++){
   //cout << "  k npoint "<<k<<" "<<npoint<<endl;
   //double posz=rad_thick*(float(rand())/RAND_MAX-0.5);
   double posz=0.0;
   double thick=rad_thick*0.5 - posz;  
   rcoo[0]=xemission+z.X()/z.Z()*posz;
   rcoo[1]=yemission+z.Y()/z.Z()*posz;
   rcoo[2]=zemission+posz;
   //rcoo[0]=pcl->TrRadPos[0]+z.X()/z.Z()*posz;
   //rcoo[1]=pcl->TrRadPos[1]+z.Y()/z.Z()*posz;
   //rcoo[2]=pcl->TrRadPos[2]+posz;
   phi+=dphi;
   double u=sin(theta)*cos(phi);
   double v=sin(theta)*sin(phi);
   double w=cos(theta);
   TVector3 ray(u,v,w);
   ray.Transform(r);

    rcoo[0]+=ray.X()/ray.Z()*thick;
    rcoo[1]+=ray.Y()/ray.Z()*thick;
    rcoo[2]+=ray.Z()/ray.Z()*thick;
//  Now refraction to the plex
    if( !drawplex){
     double st=refi*sin(ray.Theta())/n_plex;
     double u1=st*cos(ray.Phi());
     double v1=st*sin(ray.Phi());
     double w1=-sqrt(1-st*st);
     ray=TVector3(u1,v1,w1);
     // propagate
     rcoo[0]+=ray.X()/ray.Z()*plex_thick;
     rcoo[1]+=ray.Y()/ray.Z()*plex_thick;
     rcoo[2]+=ray.Z()/ray.Z()*plex_thick;
    }
//  Now refraction
    double st=n_plex*sin(ray.Theta());
    if(st>=1){
//      cerr<< "full refl "<<st<<endl;
            k--;
      if(!(npoint--))break;
      else continue;
    }
     
    double u1=st*cos(ray.Phi());
    double v1=st*sin(ray.Phi());
    double w1=-sqrt(1-st*st);
    ray=TVector3(u1,v1,w1);
    double addon=4.;
    if(ev->Version()<0 || ev->Version()>160){
      addon=0;
    }
    double rp=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
    if(rp>rc){
      k--;
      if(!(npoint--))break;
      else continue;
    }
    //    double zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
    double zl=zdet-rcoo[2]+addon;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*zl;
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*zl;
    //    array[k*3+2]=pcl->TrPMTPos[2]+addon;
    array[k*3+2]=zdet+addon;
    rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    const int miter=1000;
    int iter=0;
    while(rp>rc  && ray.Z()<0 && iter++<miter){
      //zl=pcl->TrPMTPos[2]-rcoo[2]+addon;
      zl=zdet-rcoo[2]+addon;
      //cout <<"  mirror found "<<k<<" "<<rp<<" "<<rc<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<zl<<endl;
      // take iterations
       double rp2=rp;
       double rc2=rc;
       double eps=1.e-2;
       while(fabs(rp2-rc2)>eps && fabs(zl)>eps){       
        zl=zl/2;
        rcoo[0]+=ray.X()/ray.Z()*zl;
        rcoo[1]+=ray.Y()/ray.Z()*zl;
        rcoo[2]+=zl;
        rp2=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
        rc2=sqrt(xc*xc+yc*yc)+r2+(rcoo[2]-z2)*tc;
        if(rp2>rc2 && zl<0)zl=-zl;
        else if(rp2<rc2 && zl>0)zl=-zl;
        
      }           
     // get norm vector to cone
     double cw=sin(atan(-tc));
     double phin=atan2(rcoo[1],rcoo[0]);
     double cu=cos(atan(-tc))*cos(phin);
     double cv=cos(atan(-tc))*sin(phin);
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
     ray=TVector3(ru,rv,rw);
     //     array[k*3+0]=rcoo[0]+ru/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     //     array[k*3+1]=rcoo[1]+rv/rw*(pcl->TrPMTPos[2]+addon-rcoo[2]);
     array[k*3+0]=rcoo[0]+ru/rw*(zdet+addon-rcoo[2]);
     array[k*3+1]=rcoo[1]+rv/rw*(zdet+addon-rcoo[2]);
     rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    }
     if(ray.Z()>0){
            k--;
      if(!(npoint--))break;
      else continue;
     }
     const double hole=32;
     if(fabs(array[k*3+0])<hole && fabs(array[k*3+1])<hole){
       // in the hole
            k--;
      if(!(npoint--))break;
      else continue;
     }
//    cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
   }   
}
   SetPolyLine(npoint,array);
   SetLineColor(4);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   if(drawplex)SetLineStyle(2);



   return;
}


#ifdef _PGTRACK_
TrRecHitV::~TrRecHitV(){
  //  cout << "TrRecHit destructor"<<endl;
}
TrRecHitV::TrRecHitV(AMSEventR *ev,int ref,int mult):AMSDrawI(ev,ref),TMarker3DCl(){
  try{
    //for (int ii=0;ii<1000;ii++) dummy_debug[ii]=ii*0.345;
    TrRecHitR *pcl=ev->pTrRecHit(ref);
    int size=gAMSDisplay->Focus()==0?2:1;
    //   cerr<<"--->Preparing TrRecHit "<<pcl->GetGlobalCoordinate(mult)<< endl;
    if(pcl){
//      pcl->BuildCoordinates();
      float sizex=pcl->GetECoord()[0]<0.5?pcl->GetECoord()[0]*100:pcl->GetECoord()[0];
      float sizey=pcl->GetECoord()[1]*100;
      float sizez=(sqrt(pcl-> GetTotSignal())/10.<8)?sqrt(pcl-> GetTotSignal()/10.):8.;
    
      SetSize(sizex,sizey,sizez);
      SetPosition(pcl->GetCoord(mult)[0],pcl->GetCoord(mult)[1],pcl->GetCoord(mult)[2]+fDz);
      
      SetDirection(0,0);
      if(gAMSDisplay->ShowTrClProfile()){
	float x[100];
	if(pcl->GetYCluster()){
	  int kmax=pcl->GetYCluster()->GetLength();
	  if(kmax>sizeof(x)/sizeof(x[0]))kmax=sizeof(x)/sizeof(x[0]);
	  for (int k=0;k<kmax;k++)x[k]=pcl->GetYCluster()->GetSignal(k)/(pcl->GetYCluster()->GetTotSignal()+1e-20);
	  //	    (pcl->Sum()+1.e-20);
	  SetProfileY(kmax,x);
	  SetShowProfileY(true);
	}
	
	if(pcl->GetXCluster()){
	  int kmax=pcl->GetXCluster()->GetLength();
	  if(kmax>sizeof(x)/sizeof(x[0]))kmax=sizeof(x)/sizeof(x[0]);
	  for (int k=0;k<kmax;k++)x[k]=pcl->GetXCluster()->GetSignal(k)/(pcl->GetXCluster()->GetTotSignal()+1.e-20);
	  SetProfileX(kmax,x);
	  SetShowProfileX(true);
	}
      }

      SetLineWidth(size);
      if(pcl->GetYCluster()&&pcl->GetXCluster()){
	SetLineColor(4);             // blue
	SetFillColor(4);
      }
      else if(pcl->GetYCluster()){
	SetLineColor(3);             // green
	SetFillColor(3);
      }else{
	SetLineColor(2);             // red
	SetFillColor(2);
      }
    }
  }
  catch (...){
    cerr<<" TrRecHitV-E-exception catched "<<endl;
    throw;
 }
  SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)
  SetFillStyle(1001);          // solid filling (not working now....)
  
}

#else
TrRecHitV::TrRecHitV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
  TrRecHitR *pcl=ev->pTrRecHit(ref);
  int size=gAMSDisplay->Focus()==0?2:1;
  if(pcl){
    SetSize(pcl->EHit[0]<0.5?pcl->EHit[0]*100:pcl->EHit[0],pcl->EHit[1]*100,sqrt(pcl->Sum/10)<6?sqrt(pcl->Sum/10
												     ):8);
    SetPosition(pcl->Hit[0],pcl->Hit[1],pcl->Hit[2]+fDz);
    SetDirection(0,0);
    if(gAMSDisplay->ShowTrClProfile()){
      float x[100];
      if(pcl->pTrCluster('y')){
	int kmax=pcl->pTrCluster('y')->Amplitude.size();
	if(kmax>sizeof(x)/sizeof(x[0]))kmax=sizeof(x)/sizeof(x[0]);
	for (int k=0;k<kmax;k++)x[k]=pcl->pTrCluster('y')->Amplitude[k]/(pcl->pTrCluster('y')->Sum+1.e-20);
	SetProfileY(pcl->pTrCluster('y')->Amplitude.size(),x);
	SetShowProfileY(true);
      }
      if(pcl->pTrCluster('x')){
	int kmax=pcl->pTrCluster('x')->Amplitude.size();
	if(kmax>sizeof(x)/sizeof(x[0]))kmax=sizeof(x)/sizeof(x[0]);
	for (int k=0;k<kmax;k++)x[k]=pcl->pTrCluster('x')->Amplitude[k]/(pcl->pTrCluster('x')->Sum+1.e-20);
	SetProfileX(pcl->pTrCluster('x')->Amplitude.size(),x);
	SetShowProfileX(true);
      }
    }
  }
  SetLineWidth(size);
  SetLineColor(4);             // blue
  SetFillColor(4);
  SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)
  SetFillStyle(1001);          // solid filling (not working now....)

}
#endif   
