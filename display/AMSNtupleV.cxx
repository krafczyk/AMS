#include "AMSNtupleV.h"
#include "TCONE.h"
#include "TNode.h"
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
   if( (!gAMSDisplay->DrawUsedOnly() || (pRichRing(i)->Status)/32)%2)fRichRingV.push_back( RichRingV(this,i));
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


#include "TRotation.h"


RichRingV::RichRingV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
 RichRingR *pcl=ev->pRichRing(ref);
//
// at the moment only rich rings ass with particles will be drawn
//
//  This should go to RichRingR  as soon as the latter will be updated
//  (added theta, radiator etc) by Carlos
//
static TNode *mirror=gAMSDisplay->GetGeometry()->GetNode("OMIR1");
static TNode *rich=gAMSDisplay->GetGeometry()->GetNode("RICH1");
for(int i=0;i<ev->nParticle();i++){
 if( ev->pParticle(i)->iRichRing() == ref){
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
           
  const int npoint=360/5;
  float array[3*npoint];
  const double n_aero=1.02998;
//  const double n_naf=1.68;
  const double n_naf=1.336;
  double refi;
   double rad_posz;
  if(fabs(ev->pParticle(i)->RichCoo[0][0])<11.3*3/2 && fabs(ev->pParticle(i)->RichCoo[0][1])<11.3*3/2){
    refi=n_naf;
    rad_posz=-2.75;
  }
  else {
   refi=n_aero;
   rad_posz=-1.5;
  }
  double    cc=1./ev->pRichRing(ref)->Beta/refi;
   //cout <<" refi "<<refi<<" "<<cc<<" "<<ev->pParticle(i)->RichCoo[0][0]<<" "<<ev->pParticle(i)->RichCoo[0][1]<<" "<<ev->pParticle(i)->RichCoo[0][2]<<" "<<ev->pParticle(i)->RichCoo[1][2]<<" "<<ev->pRichRing(ref)->UsedM<<endl;
  if(cc<1){
   double theta=acos(cc);
    TVector3 z(ev->pParticle(i)->RichCoo[1][0]-ev->pParticle(i)->RichCoo[0][0],ev->pParticle(i)->RichCoo[1][1]-ev->pParticle(i)->RichCoo[0][1],ev->pParticle(i)->RichCoo[1][2]-ev->pParticle(i)->RichCoo[0][2]);
    double rcoo[3];
    rcoo[0]=ev->pParticle(i)->RichCoo[0][0]+z.X()/z.Z()*rad_posz;
    rcoo[1]=ev->pParticle(i)->RichCoo[0][1]+z.Y()/z.Z()*rad_posz;
    rcoo[2]=ev->pParticle(i)->RichCoo[0][2]+rad_posz;
//    cout << rcoo[2]<<endl;
    TRotation r;
    r.SetZAxis(z);
   for( int k=0;k<npoint;k++){
    double phi=k*2*3.1415926/npoint;
    double u=sin(theta)*cos(phi);
    double v=sin(theta)*sin(phi);
    double w=cos(theta);
    TVector3 ray(u,v,w);
    ray.Transform(r);
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*(ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2);
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*(ev->pParticle(i)->RichCoo[1][2]-rcoo[2]+3.2);
    array[k*3+2]=ev->pParticle(i)->RichCoo[1][2]+3.2;
    double rp=sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1]);
    double rc=sqrt(xc*xc+yc*yc)+r2+(array[k*3+2]-z2)*tc;
    if(rp>rc){
      //cout <<"  mirror found "<<k<<" "<<rp<<" "<<rc<<" "<<array[k*3+2]<<" "<<ev->pRichRing(ref)->UsedM<<endl;
      double R1=sqrt(rcoo[0]*rcoo[0]+rcoo[1]*rcoo[1]);
      double zi=(R1-rcoo[2]*tan(ray.Theta())-r2+z2*tc)/(tc-tan(ray.Theta()));
      //cout <<" zi "<<zi<<endl;
     // get norm vector to cone
     double cw=sin(atan(tc));
     double phin=atan2(rcoo[1]+ray.Y()/ray.Z()*(zi-rcoo[2]),rcoo[0]+ray.X()/ray.Z()*(zi-rcoo[2]));
     double cu=cos(atan(tc))*cos(phin);
     double cv=cos(atan(tc))*sin(phin);
     //  reflect
     double cc=ray.X()*cu+ray.Y()*cv+ray.Z()*cw;
     double ru=ray.X()-2*cc*cu;
     double rv=ray.Y()-2*cc*cv;
     double rw=ray.Z()-2*cc*cw;
    array[k*3+0]=rcoo[0]+ray.X()/ray.Z()*(zi-rcoo[2])+ru/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-zi);
    array[k*3+1]=rcoo[1]+ray.Y()/ray.Z()*(zi-rcoo[2])+rv/rw*(ev->pParticle(i)->RichCoo[1][2]+3.2-zi);

    //cout <<"point "<<k<<" "<<theta<<" "<<phi<<" "<<array[k*3+0]<<" "<<array[k*3+1]<<" "<<array[k*3+2]<<" "<<sqrt(array[k*3+0]*array[k*3+0]+array[k*3+1]*array[k*3+1])<<endl;
    }
   }   
   SetPolyLine(npoint,array);
   SetLineColor(6);
   int size=gAMSDisplay->Focus()==0?2:1;
   SetLineWidth(size*2);
   SetLineStyle(1);
   return;
  }
  else{
   cerr<<"RichRingV-E-ProblemWithRefIndex "<<refi<<" "<<ev->pRichRing(ref)->Beta<<" "<<cc<<endl;
  }
  break;
 }
}
}
