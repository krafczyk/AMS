//  $Id: AMSNtupleV.h,v 1.55 2013/05/10 21:37:33 mduranti Exp $
#ifndef __AMSNtupleV__
#define __AMSNtupleV__
#include <TChain.h>
#include <TNamed.h>
#include <TH2.h>
#include <TH1.h>
#include <TFile.h>
#include <TPad.h>
#include <TView.h>
#include <TSystem.h>
#include <time.h>
#include <TMarker3DBox.h>
#include <TPolyLine3D.h>
#include <THelix.h>
#include "TPaveLabel.h"
#include "AMSDisplay.h"
#include "TMarker3DCl.h"
#include "../include/point.h"
class AMSDrawI{
public:
  int fRef;   ///<  Reference to corr in element in stlvector
  AMSEventR * fEv;

  AMSDrawI(AMSEventR *ev=0,int ref=0):fRef(ref),fEv(ev){}
};

class AMS3DMarker: public TMarker3DBox{
public:
  AMS3DMarker():TMarker3DBox(){};
  Int_t DistancetoPrimitive(Int_t px, Int_t py){
   
    const Int_t numPoints = 8;
    Int_t dist = 9999;
    Double_t points[3*numPoints];
    //   Float_t  points[3*numPoints];

    TView *view = gPad->GetView();
    if (!view) return dist;
    SetPoints(points);
    double  sum[3]={0,0,0};
    Double_t xndc[3];
    //   Float_t xndc[3];
    for (int i = 0; i < 8; i++) {
      view->WCtoNDC(points+i*3, xndc);
      for(int k=0;k<3;k++)sum[k]+=xndc[k];
    }
    for(int k=0;k<3;k++)sum[k]/=8;
    double x=gPad->XtoAbsPixel(sum[0]);        
    double y=gPad->YtoAbsPixel(sum[1]);        
    dist=sqrt((x-px)*(x-px)+(y-py)*(y-py));
    if (dist < 5) {
      gPad->SetCursor(kCross);
    }
    return dist;
  }
};


class  HeaderV: public   TPaveLabel, public AMSDrawI{
public:
  HeaderV():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Header","TR"){};
  HeaderV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.79,0.98,-0.71,"Header","TR"){};
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->fHeader.Info(fEv->fStatus):0;}


};

class Trigger1V: public   TPaveLabel, public AMSDrawI{
public:
  Trigger1V():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Level1","TR"){};
  Trigger1V(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.89,0.98,-0.81,"Level1","TR"){};
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pLevel1(fRef)->Info(fEv->Event()):0;}


};


class DaqV: public   TPaveLabel, public AMSDrawI{
public:
  DaqV():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"DAQ","TR"){};
  DaqV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.69,0.98,-0.61,"DAQ","TR"){};
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pDaqEvent(fRef)->Info(fRef):0;}


};



class Trigger3V: public   TPaveLabel, public AMSDrawI{
public:
  Trigger3V():AMSDrawI(NULL,-1),TPaveLabel(0.85+0.1*0.15,0.34*0.15,0.85+0.9*0.15,0.65*0.15,"Level1","TR"){};
  Trigger3V(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPaveLabel(0.8,-0.99,0.98,-0.91,"Level3","TR"){};
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pLevel3(fRef)->Info(fRef):0;}


};

class TofClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
  TofClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
  TofClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    TofClusterR *pcl=ev->pTofCluster(ref);
    if(pcl){
      SetSize(pcl->ErrorCoo[0],pcl->ErrorCoo[1],sqrt(pcl->Edep/2)<8?sqrt(pcl->Edep/2):8);
      SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);
      SetDirection(0,0);
    }
    SetLineWidth(3);
    SetLineColor(3);             // light green
    SetFillColor(3);
    SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)


  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTofCluster(fRef)->Info(fRef):0;}


};

class TrRecHitV: public TMarker3DCl, public AMSDrawI{
protected:

public:
  TrRecHitV():AMSDrawI(NULL,-1),TMarker3DCl(){};
#ifdef _PGTRACK_
  // float dummy_debug[1000];
  TrRecHitV(AMSEventR *ev,int ref,int mult=0);
  virtual ~TrRecHitV();
#else
  TrRecHitV(AMSEventR *ev,int ref);
#endif
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?const_cast<char*>(fEv->pTrRecHit(fRef)->Info(fRef)):0;}
  
};


class TrdClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
  TrdClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
  TrdClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    TrdClusterR *pcl=ev->pTrdCluster(ref);
    if(pcl){
      SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);

      float ercoo[3];
      ercoo[pcl->Direction]=sqrt(pcl->EDep/2);
      ercoo[1-pcl->Direction]=2*pcl->ClSizeR*pcl->Multip;
      ercoo[2]=2*pcl->ClSizeR;
      SetSize(ercoo[0],ercoo[1],ercoo[2]);
 

      SetDirection(0,0);
    }
    SetLineWidth(3);
    SetLineColor(46);             // dark red
    SetFillColor(46);
    SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrdCluster(fRef)->Info(fRef):0;}

};



class AntiClusterV: public TMarker3DCl, public AMSDrawI{
 protected:
 public:
 AntiClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
 AntiClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    AntiClusterR *pcl=ev->pAntiCluster(ref);
    int icharge=0;
    if(pcl){
      double dr=180./3.1415926;
#ifndef __USEANTICLUSTERPG__
      double x=pcl->Coo[0]*cos(pcl->Coo[1]/dr);
      double y=pcl->Coo[0]*sin(pcl->Coo[1]/dr);
      SetPosition(x,y,pcl->Coo[2]);
      SetSize(fabs(y)*pcl->ErrorCoo[1]/dr,fabs(x)*pcl->ErrorCoo[1]/dr,pcl->ErrorCoo[2]);
      SetDirection(0,0);
#else
      double rr=54.95;
      double phi = (pcl->Sector)*45.-22.5;
      pcl->ReBuildMe();
      double x=pcl->AntiCoo.x();
      double y=pcl->AntiCoo.y();
      double z=pcl->AntiCoo.z();
      double ex=fabs(y)*22.5/dr;
      double ey=fabs(x)*22.5/dr;
      double ez=8.;
      if ((pcl->Npairs)<1 || pcl->chi>30) ez = 39.9;
      SetPosition(x,y,z);
      SetSize(rr*22.5/dr,0.4,ez);
      icharge = (pcl->rawq)+0.5;
      SetDirection(0,phi+90.);
#endif
    }
#ifndef __USEANTICLUSTERPG__
    SetLineWidth(3);
#else
    if (icharge>4) icharge = 4;
    SetLineWidth(1+icharge);
#endif
    SetLineColor(30);             // purple
    SetFillColor(30);
    SetFillStyle(0);          // solid filling (not working now....)

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pAntiCluster(fRef)->Info(fRef):0;}

};

class RichHitV: public TMarker3DCl, public AMSDrawI{
protected:
public:
  RichHitV():AMSDrawI(NULL,-1),TMarker3DCl(){};
  RichHitV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    RichHitR *pcl=ev->pRichHit(ref);
    int size=gAMSDisplay->Focus()==0?2:1;
    if(pcl){
      float error[3];
      error[0]=error[1]=0.85/2*size;  //tmp
      error[2]=sqrt(fabs(pcl->Npe));
      SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]+error[2]);
      SetSize(error[0],error[1],error[2]);
//        cout <<"  rich hit "<<pcl->Coo[0]<<" "<<pcl->Coo[1]<<" "<<pcl->Coo[2]<<endl;
//        cout <<error[0]<<" "<<error[1]<<" "<<error[2]<<" "<<endl;
      SetDirection(0,0);
    }
    SetLineWidth(size);
    SetLineColor(9);             // purple
    SetFillColor(9);
    SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pRichHit(fRef)->Info(fRef):0;}

};

class EcalClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
  EcalClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
  EcalClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    EcalClusterR *pcl=ev->pEcalCluster(ref);
    int color=7;
    if(pcl){
      SetPosition(pcl->Coo[0],pcl->Coo[1],pcl->Coo[2]);
      float error[3];
      const float cellsize=0.9;
      error[pcl->Proj]=cellsize*pcl->NEcalHit();  //tmp
      error[2]=cellsize/2;         //tmp
      error[1-pcl->Proj]=log(1.+pcl->Edep<0?-pcl->Edep:pcl->Edep);
      if(error[1-pcl->Proj]<cellsize)error[1-pcl->Proj]=cellsize;
      //  cout <<pcl->Plane<<" "<<pcl->Coo[0]<<" "<<pcl->Coo[1]<<" "<<pcl->Coo[2]<<endl;
      //  cout <<pcl->Plane<<" "<<error[0]<<" "<<error[1]<<" "<<error[2]<<" "<<endl;
      if(pcl->Edep<0){
             color=8;
      }
      SetSize(error[0],error[1],error[2]);
      SetDirection(0,0);
    }
    SetLineWidth(gAMSDisplay->Focus()==0?1:2);
    SetLineColor(color);             // purple
    SetFillColor(color);
    SetFillStyle(gAMSDisplay->UseSolidStyle()?1001:0);          // solid filling (not working now....)

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pEcalCluster(fRef)->Info(fRef):0;}

};



class TrMCClusterV: public TMarker3DCl, public AMSDrawI{
protected:
public:
  TrMCClusterV():AMSDrawI(NULL,-1),TMarker3DCl(){};
  TrMCClusterV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TMarker3DCl(){
    TrMCClusterR *pcl=ev->pTrMCCluster(ref);
    if(pcl){
#ifdef _PGTRACK_
      SetPosition(pcl->GetXgl()[0],pcl->GetXgl()[1],pcl->GetXgl()[2]);
      SetSize(0.2,0.2,sqrt(pcl->Sum()*1000)*6);
#else
      SetPosition(pcl->Xgl[0],pcl->Xgl[1],pcl->Xgl[2]);
      SetSize(0.2,0.2,sqrt(pcl->Sum*1000)*6);
#endif
      SetDirection(0,0);
    }
    SetLineWidth(1);
    SetLineColor(5);   // yellow
    SetFillColor(5);
    SetFillStyle(0);          // solid filling (not working now....)

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?const_cast<char*>(fEv->pTrMCCluster(fRef)->Info(fRef)):0;}

};



class ParticleV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  ParticleV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  ParticleV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    ParticleR *pcl=ev->pParticle(ref);
      bool normal=pcl->TrCoo[0][2]>pcl->TrCoo[7][2];
      int trup=normal?0:7;
    if(pcl){
     if(pcl->TRDCoo[0][2]>pcl->TrCoo[trup][2]){
       //ams02 setup
      const int npoint=2+2+8+2+2+3;
      float array[3*npoint];
      int old=0;
      if(pcl->TRDCoo[1][2]>90 && pcl->TRDCoo[1][2]<150)old=1; 
//      cout <<pcl->TRDCoo[0][2]<<" "<<pcl->TRDCoo[1][2]<<" "<<endl;
      for(int k=0;k<3;k++)array[k]=pcl->TRDCoo[old][k];
      for(int k=0;k<3;k++)array[3+k]=pcl->TRDCoo[0][k];
      if(pcl->pTrdTrack()){
        for(int k=0;k<3;k++)array[3+k]+=pcl->pTrdTrack()->Coo[k];
        for(int k=0;k<3;k++)array[3+k]/=2;
        AMSDir dir(pcl->pTrdTrack()->Theta,pcl->pTrdTrack()->Phi);
        for(int k=0;k<3;k++)array[k]+=pcl->pTrdTrack()->Coo[k]+dir[k]/dir[2]*(pcl->TRDCoo[1][2]-pcl->pTrdTrack()->Coo[2]);
        for(int k=0;k<3;k++)array[k]/=2;
      }
      for(int k=0;k<3;k++)array[3+3+k]=pcl->TOFCoo[0][k];
      for(int k=0;k<3;k++)array[3+3*2+k]=pcl->TOFCoo[1][k];
      for(int i=0;i<8;i++){
	for(int k=0;k<3;k++)array[3+3*3+3*i+k]=pcl->TrCoo[i][k];
      } 
      for(int k=0;k<3;k++)array[3+3*11+k]=pcl->TOFCoo[2][k];
      for(int k=0;k<3;k++)array[3+3*12+k]=pcl->TOFCoo[3][k];
      for(int k=0;k<3;k++)array[3+3*13+k]=pcl->RichCoo[0][k];
      for(int k=0;k<3;k++)array[3+3*14+k]=pcl->RichCoo[1][k];
      for(int i=0;i<3;i++){
	for(int k=0;k<3;k++)array[3+3*15+3*i+k]=pcl->EcalCoo[i][k];
      } 
      for(int i=0;i<npoint;i++){
	//    cout <<" i "<<i<<array[3*i]<<" "<<array[3*i+1]<<" "<<array[3*i+2]<<" "<<endl;
      }
      SetPolyLine(npoint,array);
      SetLineColor(2);
      SetLineWidth(1);
      SetLineStyle(1);
      if(pcl->Charge==0)SetLineStyle(2);
    }
    else{
       //ams02p setup
      //       cout <<" ams02p setup "<<endl;
      const int npoint=2+2+9+2+2+3;
      float array[3*npoint];
      int old=0;
      if(pcl->TRDCoo[1][2]>90 && pcl->TRDCoo[1][2]<150)old=1;
//      cout <<pcl->TRDCoo[0][2]<<" "<<pcl->TRDCoo[1][2]<<" "<<endl;
//     tracker 012345678 schem determination
      bool normal=pcl->TrCoo[0][2]>pcl->TrCoo[7][2];
      int trup=normal?0:7;
      for(int k=0;k<3;k++)array[k]=pcl->TrCoo[trup][k];
      for(int k=0;k<3;k++)array[3+k]=pcl->TRDCoo[old][k];
      for(int k=0;k<3;k++)array[3+3+k]=pcl->TRDCoo[0][k];
      if(pcl->pTrdTrack()){
        for(int k=0;k<3;k++)array[3+3+k]+=pcl->pTrdTrack()->Coo[k];
        for(int k=0;k<3;k++)array[3+3+k]/=2;
        AMSDir dir(pcl->pTrdTrack()->Theta,pcl->pTrdTrack()->Phi);
        for(int k=0;k<3;k++)array[3+k]+=pcl->pTrdTrack()->Coo[k]+dir[k]/dir[2]*(pcl->TRDCoo[1][2]-pcl->pTrdTrack()->Coo[2]);
        for(int k=0;k<3;k++)array[3+k]/=2;
      }
      for(int k=0;k<3;k++)array[3+3+3+k]=pcl->TOFCoo[0][k];
      for(int k=0;k<3;k++)array[3+3+3*2+k]=pcl->TOFCoo[1][k];
      int tb=normal?1:0;
      for(int i=tb;i<7+tb;i++){
        for(int k=0;k<3;k++)array[3+3*4+3*(i-tb)+k]=pcl->TrCoo[i][k];
      }
      for(int k=0;k<3;k++)array[3+3*11+k]=pcl->TOFCoo[2][k];
      for(int k=0;k<3;k++)array[3+3*12+k]=pcl->TOFCoo[3][k];
      for(int k=0;k<3;k++)array[3+3*13+k]=pcl->RichCoo[0][k];
      for(int k=0;k<3;k++)array[3+3*14+k]=pcl->RichCoo[1][k];
      for(int k=0;k<3;k++)array[3+3*15+k]=pcl->TrCoo[8][k];
      for(int i=0;i<3;i++){
        for(int k=0;k<3;k++)array[3+3+3*15+3*i+k]=pcl->EcalCoo[i][k];
      }
//      cout <<" normal "<<normal<<endl;
      for(int i=0;i<npoint;i++){
//            cout <<" i "<<i<<array[3*i]<<" "<<array[3*i+1]<<" "<<array[3*i+2]<<" "<<endl;
      }
      SetPolyLine(npoint,array);
      SetLineColor(2);
      SetLineWidth(1);
      SetLineStyle(1);
      if(pcl->Charge==0)SetLineStyle(2);

    }
  }
}


  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pParticle(fRef)->Info(fRef,fEv):0;}


};

class TrdTrackV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  TrdTrackV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  TrdTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    TrdTrackR *pcl=ev->pTrdTrack(ref);
    if(pcl){
      const int npoint=2;
      float array[3*npoint];
      double u[3];
      u[0]=sin(pcl->Theta)*cos(pcl->Phi);
      u[1]=sin(pcl->Theta)*sin(pcl->Phi);
      u[2]=cos(pcl->Theta);
      float z1=65;   //tmp
      float x1=pcl->Coo[0]+u[0]/u[2]*(z1-pcl->Coo[2]);
      float y1=pcl->Coo[1]+u[1]/u[2]*(z1-pcl->Coo[2]);
      float z2=150; 
      float x2=pcl->Coo[0]+u[0]/u[2]*(z2-pcl->Coo[2]);
      float y2=pcl->Coo[1]+u[1]/u[2]*(z2-pcl->Coo[2]);
      array[0]=x1;
      array[1]=y1;
      array[2]=z1;
      array[3]=x2;
      array[4]=y2;
      array[5]=z2;
      SetPolyLine(npoint,array);
    }
    SetLineColor(28);
    SetLineWidth(1);
    SetLineStyle(1);

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrdTrack(fRef)->Info(fRef):0;}


};

class TrdHTrackV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  TrdHTrackV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  TrdHTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    TrdHTrackR *pcl=ev->pTrdHTrack(ref);
    if(pcl){
      const int npoint=2;
      float array[3*npoint];
      double u[3];
      double theta=pcl->Theta();
      double phi=pcl->Phi();
      u[0]=sin(theta)*cos(phi);
      u[1]=sin(theta)*sin(phi);
      u[2]=cos(theta);
      float z1=65;   //tmp
      float x1=pcl->Coo[0]+u[0]/u[2]*(z1-pcl->Coo[2]);
      float y1=pcl->Coo[1]+u[1]/u[2]*(z1-pcl->Coo[2]);
      float z2=150; 
      float x2=pcl->Coo[0]+u[0]/u[2]*(z2-pcl->Coo[2]);
      float y2=pcl->Coo[1]+u[1]/u[2]*(z2-pcl->Coo[2]);
      array[0]=x1;
      array[1]=y1;
      array[2]=z1;
      array[3]=x2;
      array[4]=y2;
      array[5]=z2;
      SetPolyLine(npoint,array);
    }
    SetLineColor(14);
    //    SetLineColor(28);
    SetLineWidth(1);
    SetLineStyle(1);
  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?const_cast<char*>(fEv->pTrdHTrack(fRef)->Info(fRef)):0;}
};


class MCEventgV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  MCEventgV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  MCEventgV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    MCEventgR *pcl=ev->pMCEventg(ref);
    if(pcl){
      const int npoint=2;
      float array[3*npoint];
      array[0]=pcl->Coo[0];
      array[1]=pcl->Coo[1];
      array[2]=pcl->Coo[2];
      MCEventgR *pcln=ev->pMCEventg(ref+1);
      if(pcln && pcl->Particle>0)array[5]=pcln->Coo[2];
      else array[5]=array[2]+pcl->Dir[2]*50;
      array[3]=pcl->Coo[0]+pcl->Dir[0]/pcl->Dir[2]*(array[5]-pcl->Coo[2]);
      array[4]=pcl->Coo[1]+pcl->Dir[1]/pcl->Dir[2]*(array[5]-pcl->Coo[2]);
      SetPolyLine(npoint,array);

      SetLineColor(5);
      SetLineWidth(1);
      SetLineStyle(1);
      if(pcl->Charge==0)SetLineStyle(2);
      else SetLineWidth(fabs(pcl->Charge));
    }
  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pMCEventg(fRef)->Info(fRef):0;}


};



#ifdef _PGTRACK_

class TrTrackV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  TrTrackV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  TrTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    if (ev==0) return;
    TrTrackR *pcl=ev->pTrTrack(ref);
    if(pcl){
      Double_t Bfield = -0.4;	// in minus-x direction of AMS
      Double_t P0[3];
      Double_t V0[3];
      Double_t Axis[3]={-1,0,0};
      Double_t Range[2]={-60,60};
      for(int i=0;i<3;i++)P0[i]=pcl->GetP0()[i];

      //       V0[0]=pcl->GetRigidity() * sin(pcl->GetTheta()) * cos(pcl->GetPhi());
      //       V0[1]=pcl->GetRigidity() * sin(pcl->GetTheta()) * sin(pcl->GetPhi());
      //       V0[2]=pcl->GetRigidity() * cos(pcl->GetTheta());
      int nh=pcl->GetNhits();
      int bit=pcl->GetBitPattern();
      SetPolyLine(-1);
      int ind=0;
      if((bit & (1<<7))>0){
	if(TrRecHitR* hh=pcl->GetHitLO(8)){
	  AMSPoint aa=hh->GetCoord();
	  if(hh->OnlyY () && hh->GetDummyX()==-1){
	    hh->SetDummyX(0);
//	    hh->BuildCoordinates();
	  }
	  SetPoint(ind++,aa[0],aa[1],aa[2]);
	}
      }
      for (int ii=0;ii<7;ii++){
	if((bit & (1<<ii))>0){
	  if(TrRecHitR* hh=pcl->GetHitLO(ii+1)){
	    AMSPoint aa=hh->GetCoord();
	    
	    if (!(hh->OnlyY () && hh->GetDummyX()==-1))
	      SetPoint(ind++,aa[0],aa[1],aa[2]);
	  }
	}
      }
      if((bit & (1<<8))>0){
	if(TrRecHitR* hh=pcl->GetHitLO(9)){
	  AMSPoint aa=hh->GetCoord();
	    if (!(hh->OnlyY () && hh->GetDummyX()==-1))
	  SetPoint(ind++,aa[0],aa[1],aa[2]);

	}
       }
       

      V0[0]= -1*pcl->GetDir()[0]*pcl->GetRigidity() ;
      V0[1]= -1*pcl->GetDir()[1]*pcl->GetRigidity() ;
      V0[2]= -1*pcl->GetDir()[2]*pcl->GetRigidity() ;

      //THelix::SetHelix(P0,V040.3*Bfield/100,Range,kHelixX,Axis);

      //      printf("=============> %f %f %f   %f %f %f\n",P0[0],P0[1],P0[2],V0[0],V0[1],V0[2]);
    }
    SetLineColor(14);
    SetLineWidth(1);
    SetLineStyle(1);

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?const_cast<char*>(fEv->pTrTrack(fRef)->Info(fRef)):0;}


};
#else

class TrTrackV: public THelix, public AMSDrawI{
protected:
public:
  TrTrackV():AMSDrawI(NULL,-1),THelix(){};
  TrTrackV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),THelix(){

    TrTrackR *pcl=ev->pTrTrack(ref);
    if(pcl){
      Double_t Bfield = -0.1;	// in minus-x direction of AMS
      Double_t P0[3];
      Double_t V0[3];
      Double_t Axis[3]={-1,0,0};
//      Double_t Range[2]={-75,75};
      for(int i=0;i<3;i++)P0[i]=pcl->P0[i];
      double span=pcl->Hit[0][2]-pcl->Hit[pcl->NTrRecHit()-1][2];
      Double_t Range[2]={pcl->Hit[pcl->NTrRecHit()-1][2],pcl->Hit[0][2]};
     
      V0[0]=pcl->Rigidity * sin(pcl->Theta) * cos(pcl->Phi);
      V0[1]=pcl->Rigidity * sin(pcl->Theta) * sin(pcl->Phi);
      V0[2]=pcl->Rigidity * cos(pcl->Theta);
      THelix::SetHelix(P0,V0,0.3*Bfield/100,Range,kHelixX,Axis);
    }
    SetLineColor(14);
    SetLineWidth(1);
    SetLineStyle(1);

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pTrTrack(fRef)->Info(fRef):0;}


};

#endif


class EcalShowerV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  EcalShowerV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  EcalShowerV(AMSEventR *ev,int ref):AMSDrawI(ev,ref),TPolyLine3D(){
    EcalShowerR *pcl=ev->pEcalShower(ref);
    if(pcl){
      const int npoint=2;
      float array[3*npoint];
      array[0]=pcl->Entry[0];
      array[1]=pcl->Entry[1];
      array[2]=pcl->Entry[2];
      array[3]=pcl->Exit[0];
      array[4]=pcl->Exit[1];
      array[5]=pcl->Exit[2];
      SetPolyLine(npoint,array);
      int lw=log(pcl->EnergyC);
      if(lw<1)lw=1;
      SetLineWidth(lw);
    }
    SetLineColor(12);
    SetLineStyle(1);

  }
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pEcalShower(fRef)->Info(fRef):0;}


};




class RichRingV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  RichRingV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  RichRingV(AMSEventR *ev,int ref, bool drawplex=false);
  char * GetObjectInfo(Int_t px, Int_t py) const{return fRef>=0?fEv->pRichRing(fRef)->Info(fRef):0;}


};


class RichRingBV: public TPolyLine3D, public AMSDrawI{
protected:
public:
  RichRingBV():AMSDrawI(NULL,-1),TPolyLine3D(){};
  RichRingBV(AMSEventR *ev,int ref, bool drawplex=false);
  char * GetObjectInfo(Int_t px, Int_t py) const;


};



class AMSNtupleV : public AMSEventR{


public:
  friend class AMSTOFHist;
  friend class AMSAntiHist;
  friend class AMSTrackerHist;
  friend class AMSLVL1Hist;
  friend class AMSLVL3Hist;
  friend class AMSAxAMSHist;
  friend class AMSGenHist;
  friend class AMSNtupleHelper;


protected:
  int TkMult;
  int TkDispY;
  // Drawable things 
  vector<AntiClusterV> fAntiClusterV;
  vector<TofClusterV> fTofClusterV;
  vector<TrdClusterV> fTrdClusterV;
  vector<RichHitV> fRichHitV;
  vector<TrMCClusterV> fTrMCClusterV;
  vector<EcalClusterV> fEcalClusterV;
  vector<TrRecHitV> fTrRecHitV;
  vector<TrTrackV> fTrTrackV;
  vector<TrdTrackV> fTrdTrackV;
  vector<TrdHTrackV> fTrdHTrackV;
  vector<EcalShowerV> fEcalShowerV;
  vector<RichRingV> fRichRingV;
  vector<RichRingBV> fRichRingBV;
  vector<ParticleV> fParticleV;
  vector<MCEventgV> fMCEventgV;
  vector<Trigger1V> fTrigger1V;
  vector<DaqV>      fDaqV;
  vector<Trigger3V> fTrigger3V;
  vector<HeaderV> fHeaderV;


public:
  AMSNtupleV():AMSEventR(){TkMult=1; TkDispY=1;};
  void Draw(EAMSType type=kall);
  void Prepare(EAMSType type=kall);
  bool GetEvent(unsigned int run, unsigned int event);
  char * GetObjInfo(int px,int py);
  void SetTkMult(int aa){TkMult=aa;}
  int GetTkMult(){return TkMult;}
  void SetTkDispY(int aa){TkDispY=aa;}
  int  GetTkDispY(){return TkDispY;}
  ClassDef(AMSNtupleV,1);           // Ntuple Drawable
};
#endif
