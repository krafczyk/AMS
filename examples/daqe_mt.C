#include <root.h>
///   \example daqe_mt.C
///
#define stlv_cxx
#include "../include/root_RVS.h"
#include "../include/amschain.h"
#include "TF1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TEventList.h"
#include "TSelector.h"
#include "TChain.h"
#include <TROOT.h>
#include <TTree.h>
#include <fstream.h>
#include <omp.h>
#include <getopt.h>
/** 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */



///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the AMSEventR class. \n

 The following member functions must be called by the user: \n
    void UBegin():       called everytime a loop on the tree starts,
                         a convenient place to create your histograms. \n
    void UProcessFill(): called in the entry loop for all entries \n
    void UTerminate():   called at the end of a loop on the tree,
                         a convenient place to draw/fit your histograms. \n
 
 The following member function is optional: \n
    bool UProcessCut(): called in the entry loop to accept/reject an event \n

   To use this file, try the following session on your Tree T: \n
 Root > T->Process("stlv.C+") -> compile library if not up to date \n
 Root > T->Process("stlv.C++") -> force always compilation \n
 Root > T->Process("stlv.C")  -> non-compiled (interpreted) mode \n
*/
static int runs[1000];
static int events[1000];
static int nruns=-1;
class daqe : public AMSEventR {
   public :

   daqe(TTree *tree=0){ };


/// This is necessary if you want to run without compilation as:
///     chain.Process("stlv.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

   /// User Function called before starting the event loop.
   /// Book Histos
   virtual void    UBegin();

   /// User Function called to preselect events (NOT MANDATORY).
   /// Called for all entries.
   virtual bool    UProcessCut();
//   virtual bool    UProcessStatus(unsigned long long status);

   /// User Function called to analyze each event.
   /// Called for all entries.
   /// Fills histograms.
   virtual void    UProcessFill();
   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();

   //virtual AMSEventR* init(unsigned int thr){return new daqe();}

};


//create here pointers to histos and/or array of histos

ofstream ftxt;
ofstream ftxt1;


void daqe::UBegin(){
    cout << " Begin calling"<<endl;
    ftxt.open("selectaiee.txt");
    ftxt1.open("eventsaiee.txt");
    for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++)runs[i]=0;
   //here create histograms
   hbook1(6,"resol",200,-1.,3.);
   hbook1(7,"resol",200,-1.,3.);
   hbook1(8,"resol",200,-1.,3.);
   hbook1(10,"diff tof/trd double",400,-20.,20.);
   hbook1(11,"diff tof/trd double lay #1",400,-20.,20.);
   hbook1(12,"diff tof/trd double lay #2",400,-20.,20.);
   hbook1(110,"diff tof/trd ",200,-20.,20.);
   hbook1(111,"diff tof/trd lay #1",200,-20.,20.);
   hbook1(112,"diff tof/trd lay #2",200,-20.,20.);
   hbook1(113,"timediff",2000,0,100);
   hbook1(211,"trdtr ",200,0,1);
   hbook1(3211,"ecaltr ",200,0,1);
   hbook1(4211,"ecaltr ",200,0,1);
   hbook1(10211,"nz",200,-1,1);
   hbook1(212,"chi2",200,0,1000);
   hbook1(213,"chi2 fx",200,0,1000);
    hbook1(214,"chi2 noa",200,0,1000);
   hbook1(3212,"chi2",200,0,1000);
   hbook1(3213,"chi2 fx",200,0,1000);
    hbook1(3214,"chi2 noa",200,0,1000);

    for(int i=0;i<4;i++){
      hbook1(2001+i,"tof perp",200,-10.,10.);
     hbook1(3001+i,"tof par",200,-10,10);
    }
    for(int i=0;i<21;i++){
       hbook1(4000+i,"TRD y ",200,-1,1.);
       hbook1(5000+i,"TRD x",200,-1,1.);
       hbook1(14000+i,"TRD y ",200,-1.,1.);
       hbook1(15000+i,"TRD x",200,-1,1.);
       hbook1(114000+i,"TRD y ",200,-1.,1.);
       hbook1(115000+i,"TRD x",200,-1,1.);
    }
    for(int i=0;i<21;i++){
      hbook1(6000+i,"ecal ",200,-2.,2.);
    }
      hbook1(16018,"ecal ",200,-1.,1.);
      hbook1(116018,"ecal ",200,-1.,1.);
      hbook1(16019,"ecal ",200,-1.,1.);
      hbook1(116019,"ecal ",200,-1.,1.);
    hbook1(6021," ecal sum",400,0.,100.);
    hbook1(7001," beta tof", 400,0.5,1.5);
    hbook1(7002," beta rich", 400,0.75,1.05);
    hbook1(7003," charge rich", 400,0.5,2);
    float ax=0.02;
  for(int ilad=0;ilad<20;ilad++){
   for(int ih=0;ih<2;ih++){ 
    hbook1(400010+ih*10000+ilad*100,"resid x (cm)",200,-ax,ax);
    hbook1(500010+ih*10000+ilad*100,"resid y (cm)",200,-ax,ax);

    hbook1(1400010+ih*10000+ilad*100,"resid x (cm)",200,-ax/5,ax/5);
    hbook1(1500010+ih*10000+ilad*100,"resid y (cm)",200,-ax/5,ax/5);
   for(int i=0;i<8;i++){

//  if(ih==0 && ilad==0){
   hbook2(600001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,-1.,1.);
    hbook2(700001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,-1.,1.);
    hbook1(600021+i+ih*10000+ilad*100,"resid x",120,-1.,1.);
    hbook1(700021+i+ih*10000+ilad*100,"resid x",120,-1.,1.);
    hbook1(600011+i+ih*10000+ilad*100,"resid x",120,-1.,1.);
    hbook1(700011+i+ih*10000+ilad*100,"resid x",120,-1.,1.);
    float a=-60.;
    float b=60;
    hbook2(1600001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook2(1700001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook1(1600021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(1700021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(1600011+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(1700011+i+ih*10000+ilad*100,"resid x",120,a,b);
   a=-40.;
     b=40;
    hbook2(2600001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook2(2700001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook1(2600021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(2700021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(2600011+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(2700011+i+ih*10000+ilad*100,"resid x",120,a,b);




     a=-40.;
     b=40;
    hbook2(3600001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook2(3700001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook1(3600021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(3700021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(3600011+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(3700011+i+ih*10000+ilad*100,"resid x",120,a,b);

  a=-40.;
     b=40;
    hbook2(4600001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook2(4700001+i+ih*10000+ilad*100,"resid x",200,-ax,ax,120,a,b);
    hbook1(4600021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(4700021+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(4600011+i+ih*10000+ilad*100,"resid x",120,a,b);
    hbook1(4700011+i+ih*10000+ilad*100,"resid x",120,a,b);



//   }

    hbook1(400001+i+ih*10000+ilad*100,"resid x (cm)",200,-ax,ax);
    hbook1(500001+i+ih*10000+ilad*100,"resid y (cm)",200,-ax,ax);
    hbook1(1400001+i+ih*10000+ilad*100,"resid x (cm)",200,-ax/2,ax/2);
    hbook1(1500001+i+ih*10000+ilad*100,"resid y (cm)",200,-ax/2,ax/2);
   }
}
}
 for(int i=0;i<8;i++){
    hbook1(1201+i,"ladder ",40,-0.5,39.5);
    hbook1(1301+i,"ladder ",40,-0.5,39.5);
       hbook1(400+1+i," ll",31,-15.5,15.5);
       hbook1(501+i," ll",31,-15.5,15.5);

   }
   hbook1(210,"nlay",10,-0.5,9.5);   
   for(int k=0;k<20;k++){
   hbook1(301+k,"count",20,-0.5,19.5);
   }
   for(int i=0;i<20;i++){
    for(int j=0;j<20;j++){
      int id=10000+i*100+j;
      hbook1(id,"mip",200,0,10);
    }
   }
      hbook1(201,"mip",200,0,10);
      hbook1(202,"mip adc",200,0,200);
    cout << " Begin called now"<<endl;
    
   

}
/*
bool daqe::UProcessStatus(unsigned long long status){
   if( !(status & 3) || !(status & (3<<8)) || !(status & (3<<13))  || !(status & (3<<17)))return false;
  else return true;
}
*/
// UProcessCut() is not mandatory
bool daqe::UProcessCut(){
/*
      static char f[1024]="";
      const char * fname=_Tree->GetCurrentFile()->GetName();

     if(strcmp(f,fname)){
        cout <<" getfile "<<_Tree->GetCurrentFile()->GetName()<<endl;
        strcpy(f,fname); 
     }
*/
     if (nParticle()==0  || nTrTrack()==0  || nTrdTrack()==0 ) return false;
     return true;
}

void daqe::UProcessFill()
{
    float tofcoo[4][6];
    float clcoo[20][3];
     const int nbadl=3;
     int blay[3]={4,6,8};
     int blad[3]={7,12,7};
     int bhalf[3]={1,1,0};
   // User Function called for all entries .
   // Entry is the entry number in the current tree.
   // Fills histograms.
//     ftxt<<Run()<<" "<<Event()<<endl; 
     static int runo=0;
#pragma omp threadprivate (runo)
     if(Run()!=runo){
        cout <<"  tracks found run "<<Run()<<endl;
//        runs[++nruns]=Run();
//        events[nruns]=0;
        runo=Run();
     }   
//     events[nruns]++;
     static int written=0;
     const int maxw=200;
     bool cuts[23];
    ParticleR part=Particle(0);
    if(part.iTrTrack()<0 )return;
    if(part.iRichRing()>=0){
       RichRingR rich=RichRing(part.iRichRing());
       if(rich.Used>3){
        hf1(7002,rich.Beta,1);
        float charge=sqrt(rich.NpCol/rich.NpExp);
        hf1(7003,charge,1);
       }
     }
     TrTrackR tr=TrTrack(part.iTrTrack());
     if(part.iBeta()>=0){
      BetaR beta=Beta(part.iBeta());
      if(beta.Pattern==0){
        hf1(7001,beta.Beta,1);
      }
       
     }
    //cout<< tr.NTrRecHit()<<endl;
    hf1(210,tr.NTrRecHit(),1);
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
     TrClusterR y=TrCluster(rh.iTrCluster('y'));
     int id=y.Idsoft;
     int lay=id%10;
     int lad=(id/10)%100;
     int half=(id/1000)%10-2;
     hf1(1200+lay,float(lad+20*half),1);
    }
    Level1R lvl1=Level1(0);
    hf1(113,lvl1.TrigTime[4]/1000.,1);
     double nx1=sin(tr.Theta)*cos(tr.Phi);
     double ny1=sin(tr.Theta)*sin(tr.Phi);
     double nz1=cos(tr.Theta);
     cuts[6]=part.iTrdTrack()>=0 && part.iTrTrack()>=0;
     int nlay=0;
     for (int i=0;i<nTofCluster();i++){
      nlay+=TofCluster(i).Layer;
     }   
     cuts[2]=nlay==10;
     if(part.iEcalShower()>=0 ){
       float az=nx1/nz1;
      float  bz=ny1/nz1;
      EcalShowerR ecal=EcalShower(part.iEcalShower());
      hf1(6021,ecal.EnergyC,1);
      double nx=ecal.Dir[0];
      double ny=ecal.Dir[1];
      double nz=ecal.Dir[2];
      double c=nx*nx1+ny*ny1+nz*nz1;
      c=acos(c);
      hf1(3211,c,1);
       nx=ecal.EMDir[0];
       ny=ecal.EMDir[1];
       nz=ecal.EMDir[2];
       c=nx*nx1+ny*ny1+nz*nz1;
      c=acos(c);
      hf1(4211,c,1);

      for(int i=0;i<ecal.NEcal2DCluster();i++){
       Ecal2DClusterR ecal2d=Ecal2DCluster(ecal.iEcal2DCluster(i));
//       cout << "ecal "<<i<<" "<<ecal.NEcal2DCluster()<<endl;
       for(int j=0;j<ecal2d.NEcalCluster();j++){
//          cout <<" 2d "<<j<<" "<<endl;
        EcalClusterR ecl=EcalCluster(ecal2d.iEcalCluster(j));
          float diff=0;
          if(ecl.Proj==0){
             diff=ecl.Coo[0]-part.EcalCoo[0][0]-az*(ecl.Coo[2]-part.EcalCoo[0][2]);
            hf1(6018,diff,1);
            hf1(16018,az,diff);
            hf1(116018,az,1);
           }
          else{
             diff=ecl.Coo[1]-part.EcalCoo[0][1]-bz*(ecl.Coo[2]-part.EcalCoo[0][2]);
            hf1(6019,diff,1);
            hf1(16019,bz,diff);
            hf1(116019,bz,1);
          }
          hf1(6000+ecl.Plane,diff,1);        
            hf1(6020,diff,1);
        }

      }

     }

     if(part.iTrdTrack()>=0){
     TrdTrackR trd=TrdTrack(part.iTrdTrack());
     cuts[0]=NTrdTrack()<2 && trd.NTrdSegment()>=3;
     double nx=sin(trd.Theta)*cos(trd.Phi);
     double ny=sin(trd.Theta)*sin(trd.Phi);
     double nz=cos(trd.Theta);
     hf1(10211,nz1,1);
     double c=nx*nx1+ny*ny1+nz*nz1;
     c=acos(c);
     hf1(211,c,1);
   hf1(3212,tr.Chi2FastFit,1);
     hf1(3214,tr.DBase[1],1);
     if((tr.Status/8192)%2==1)hf1(3213,tr.Chi2FastFit,1);

     cuts[4]=c<0.05;
 
   int count[20];
   for(int i=0;i<20;i++){
    for(int j=0;j<3;j++){
       clcoo[i][j]=-1;
    }
   }
  int ntrdcl=0;
   for(int k=0;k<20;k++)count[k]=0;
   for(int i=0;i<trd.NTrdSegment();i++){
     TrdSegmentR s=TrdSegment(trd.iTrdSegment(i));
     for (int j=0;j<s.NTrdCluster();j++){
      TrdClusterR c=TrdCluster(s.iTrdCluster(j));
      clcoo[c.Layer][0]=c.Direction;
      clcoo[c.Layer][1]=c.Direction==0?c.Coo[1]:c.Coo[0];  
      clcoo[c.Layer][2]=c.Coo[2];
      ntrdcl++;
      TrdRawHitR h=TrdRawHit(c.iTrdRawHit());
      
      int id=10000+h.Layer*100+h.Ladder;
      for(int k=0;k<20;k++){
       float thr=2+float(k)/10.;
       if(c.EDep>thr){
        count[k]++;
       }
      }
      hf1(id,c.EDep,1);
      hf1(201,c.EDep,1);
      hf1(202,h.Amp,1);
      }
     }
     for(int k=0;k<20;k++){
     hf1(301+k,count[k],1);
     }
    nlay=0;
    int nmemb=0;
    for(int i=0;i<4;i++){
     for(int j=0;j<6;j++){
         tofcoo[i][j]=-1;
     }
    }
    for (int i=0;i<nTofCluster();i++){
      tofcoo[TofCluster(i).Layer-1][0]=TofCluster(i).Coo[0];
      tofcoo[TofCluster(i).Layer-1][1]=TofCluster(i).Coo[1];
      tofcoo[TofCluster(i).Layer-1][2]=TofCluster(i).Coo[2];
      tofcoo[TofCluster(i).Layer-1][3]=TofCluster(i).Bar;
      tofcoo[TofCluster(i).Layer-1][4]=TofCluster(i).ErrorCoo[0];
      tofcoo[TofCluster(i).Layer-1][5]=TofCluster(i).ErrorCoo[1];
     if(TofCluster(i).Layer<3){
     nlay+=TofCluster(i).Layer;
      if(TofCluster(i).NTofRawCluster()==2)nmemb=TofCluster(i).Layer;
     }
    }     
    for (int i=0;i<nTofCluster();i++){
     if(nmemb== TofCluster(i).Layer){
     if(TofCluster(i).Layer==1 ){
      number yc=TofCluster(i).Coo[1];
      number zc=TofCluster(i).Coo[2];
      number cross=trd.Coo[1]+(zc-trd.Coo[2])*ny/nz;
      hf1(10,cross-yc,1);
      hf1(11,cross-yc,1);
     }
     else if(TofCluster(i).Layer==2){
      number xc=TofCluster(i).Coo[0];
      number zc=TofCluster(i).Coo[2];
      number cross=trd.Coo[0]+(zc-trd.Coo[2])*nx/nz;
      hf1(10,cross-xc,1);
      hf1(12,cross-xc,1);
     }
      }
     if(TofCluster(i).Layer==1 ){
      number yc=TofCluster(i).Coo[1];
      number zc=TofCluster(i).Coo[2];
      number cross=trd.Coo[1]+(zc-trd.Coo[2])*ny/nz;
      hf1(110,cross-yc,1);
      hf1(111,cross-yc,1);
     }
     else if(TofCluster(i).Layer==2){
      number xc=TofCluster(i).Coo[0];
      number zc=TofCluster(i).Coo[2];
      number cross=trd.Coo[0]+(zc-trd.Coo[2])*nx/nz;
      hf1(110,cross-xc,1);
      hf1(112,cross-xc,1);
     }

    }     



   }
    else{
     nlay=0;
     int nmemb=0;
     for (int i=0;i<nTofCluster();i++){
      nlay+=TofCluster(i).Layer;
      if(TofCluster(i).NTofRawCluster()==2)nmemb=TofCluster(i).Layer;
    }    
     //cuts[0]=NTrdTrack()<2;
     cuts[2]=nlay==10;
     //cout <<"nlay "<<nlay<<endl;
    }
     nlay=0;
    int nmemb=0;
    for (int i=0;i<nTofCluster();i++){
     if(TofCluster(i).Layer<3){
     nlay+=TofCluster(i).Layer;
      if(TofCluster(i).NTofRawCluster()==2)nmemb=TofCluster(i).Layer;
     }
    }     
    cuts[1]=nTofCluster()<5;
//    cuts[2]=nlay==3;
    cuts[3]=nTrTrack()>0;
    cuts[5]=(tr.Status/8192)%2==0 ;
    if(!cuts[5]){
    bool hfound=false;
    bool ahfound=false;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
       number dxt=rh.CofgX+1.1e-3-part.TrCoo[rh.lay()-1][0];
       number dzt=rh.Hit[2]-part.TrCoo[rh.lay()-1][2];
       number dyt=rh.CofgY-part.TrCoo[rh.lay()-1][1]-2e-4;
       number dx=tr.Hit[i][0]-part.TrCoo[rh.lay()-1][0];
       number dy=tr.Hit[i][1]-part.TrCoo[rh.lay()-1][1];
       number dz=tr.Hit[i][2]-part.TrCoo[rh.lay()-1][2];
       int lay,lad,half,stripx,stripy;
       rh.Geom(lay,lad,half,stripx,stripy);
        ahfound=(rh.Status/8192)%2!=0;
        if(ahfound){
        for(int k=0;k<nbadl;k++){
        if(lay==blay[k] && lad==blad[k] && half==bhalf[k]){
          ahfound=false;
          break;
        }
        }
       }
       if(ahfound)break;
       for(int k=0;k<nbadl;k++){
        if(lay==blay[k] && lad==blad[k] && half==bhalf[k]){
           hfound=true;
           break; 
        }
       }
     }
     cuts[5]=hfound && !ahfound;
    }
    cuts[5]=cuts[5] && (tr.Status/16384)%2==0 ;
        
    bool cut=true;
    for(int i=0;i<7;i++){
      cut=cut && cuts[i];
    }
//    for(int i=0;i<7;i++)cout<<i<<" "<<cuts[i]<<" ";
//    cout<<endl;    
    if(cut){

//
//   calculate parameters
//
//

//     cout <<" ee "<<Run()<<endl;

     float mcmom=0;
    if(nMCEventg()>0){		
     MCEventgR mc_ev=MCEventg(0);
      double x1=mc_ev.Momentum/(tr.Rigidity);
      double x2=mc_ev.Momentum/(tr.GRigidity);
      double x3=mc_ev.Momentum/(tr.PiRigidity);
      hf1(6,x1,1.);
      hf1(7,x2,1.);
      hf1(8,x3,1.);
      mcmom=mc_ev.Momentum;
    }

     hf1(212,tr.Chi2FastFit,1);
     hf1(214,tr.DBase[1],1);
     if((tr.Status/8192)%2==1)hf1(213,tr.Chi2FastFit,1);
      int ntrrh=tr.NTrRecHit();
      for(int i=0;i<20;i++){
         for(int j=0;j<3;j++){
//          ftxt1.write((char *)&(clcoo[i][j]),sizeof(clcoo[0][0]));
       }
       }
      for(int i=0;i<4;i++){
         for(int j=0;j<6;j++){
//          ftxt1.write((char *)&(tofcoo[i][j]),sizeof(tofcoo[0][0]));
       }
       }
       float az=nx1/nz1;
       float  bz=ny1/nz1;
     for(int i=0;i<4;i++){
         if(i==0 || i==3){
          hf1(2001+i,tofcoo[i][1]-part.TOFCoo[i][1],1);
          hf1(3001+i,tofcoo[i][0]-part.TOFCoo[i][0],1);
         }
         else{
          hf1(2001+i,tofcoo[i][0]-part.TOFCoo[i][0],1);
          hf1(3001+i,tofcoo[i][1]-part.TOFCoo[i][1],1);
         }
     }
         if(fabs(az)>0.88){
            cout<<"Run "<<Run()<<" "<<Event()<<endl;
              GetAllContents();
              Fill();

         }

     for(int i=0;i<20;i++){
      if(clcoo[i][0]==0){
       hf1(4000,clcoo[i][1]-part.TRDCoo[0][1]-bz*(clcoo[i][2]-part.TRDCoo[0][2]),1);
       hf1(4001+i,clcoo[i][1]-part.TRDCoo[0][1]-bz*(clcoo[i][2]-part.TRDCoo[0][2]),1);
       hf1(14000,bz,clcoo[i][1]-part.TRDCoo[0][1]-bz*(clcoo[i][2]-part.TRDCoo[0][2]));
       hf1(14001+i,bz,clcoo[i][1]-part.TRDCoo[0][1]-bz*(clcoo[i][2]-part.TRDCoo[0][2]));
       hf1(114000,bz,1);
       hf1(114001+i,bz,1);
       
      }
      else if(clcoo[i][0]==1){
         hf1(5000,clcoo[i][1]-part.TRDCoo[0][0]-az*(clcoo[i][2]-part.TRDCoo[0][2]),1);
         hf1(5001+i,clcoo[i][1]-part.TRDCoo[0][0]-az*(clcoo[i][2]-part.TRDCoo[0][2]),1);
         hf1(15000,az,clcoo[i][1]-part.TRDCoo[0][0]-az*(clcoo[i][2]-part.TRDCoo[0][2]));
         hf1(15001+i,az,clcoo[i][1]-part.TRDCoo[0][0]-az*(clcoo[i][2]-part.TRDCoo[0][2]));
         hf1(115000,az,1);
         hf1(115001+i,az,1);
      }
     }
//          ftxt1.write((char *)&(az),sizeof(az));
//          ftxt1.write((char *)&(bz),sizeof(az));
//      ftxt1<<tr.NTrRecHit()<<" "<<tr.Chi2FastFit<<" "<<tr.Pattern<<" "<<tr.Address<<" "<<tr.AddressA<<" "<<tr.Rigidity<<" "<<mcmom<<endl;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
       number dxt=rh.CofgX+1.6e-3-part.TrCoo[rh.lay()-1][0];
       number dzt=rh.Hit[2]-part.TrCoo[rh.lay()-1][2];
       number dyt=rh.CofgY-part.TrCoo[rh.lay()-1][1]-2e-4;
       number dx=tr.Hit[i][0]-part.TrCoo[rh.lay()-1][0];
       number dy=tr.Hit[i][1]-part.TrCoo[rh.lay()-1][1];
       number dz=tr.Hit[i][2]-part.TrCoo[rh.lay()-1][2];
       int lay,lad,half,stripx,stripy;
       rh.Geom(lay,lad,half,stripx,stripy);
//              cout <<lay<<rh.lay()<<" "<<lad<<rh.lad()<<" "<<half<<rh.half()<<endl;
       hf1(400010,dx,1);
       hf1(500010,dy,1);
       float x=part.TrCoo[rh.lay()-1][0];
       float y=part.TrCoo[rh.lay()-1][1];
       hf1(400000+rh.lay(),dx,1);
       hf2(600000+rh.lay(),dx,(nx1)/nz1,1.);
       hf2(1600000+rh.lay(),dx,y,1);
       hf2(1700000+rh.lay(),dy,x,1);
       hf2(2600000+rh.lay(),dx,x*nx1/nz1,1);
       hf2(2700000+rh.lay(),dy,y*ny1/nz1,1);
       hf2(3600000+rh.lay(),dx,y*nx1/nz1,1);
       hf2(3700000+rh.lay(),dy,x*ny1/nz1,1);


       hf2(1600000+rh.lay()+half*10000+lad*100,dx,y,1);
       hf2(1700000+rh.lay()+half*10000+lad*100,dy,x,1);
       hf2(2600000+rh.lay()+half*10000+lad*100,dx,x*nx1/nz1,1);
       hf2(2700000+rh.lay()+half*10000+lad*100,dy,y*ny1/nz1,1);
       hf2(3600000+rh.lay()+half*10000+lad*100,dx,y*nx1/nz1,1);
       hf2(3700000+rh.lay(),dy+half*10000+lad*100,x*ny1/nz1,1);





       hf2(700000+rh.lay(),dy,(ny1)/nz1,1.);
       hf1(600020+lay,nx1/nz1,dx);
       hf1(700020+lay,ny1/nz1,dy);
      hf1(600010+lay,nx1/nz1,1);
       hf1(700010+lay,ny1/nz1,1);

       hf1(500000+rh.lay(),dy,1);
       hf1(1400010,dxt-dx,1);
       hf1(1500010,dyt-dy,1);
       if(dyt-dy<-1e-3){
         hf1(500+rh.lay(),rh.lad()*(rh.half()>0?1:-1));
       }       if(fabs(dxt-dx)>1e-3){
         hf1(400+rh.lay(),rh.lad()*(rh.half()>0?1:-1));
       }

       hf1(1400000+rh.lay(),dxt-dx,1);
       hf1(1500000+rh.lay(),dyt-dy,1);
       hf1(400000+rh.lay()+half*10000+lad*100,dx,1);
       hf1(500000+rh.lay()+half*10000+lad*100,dy,1);
       hf1(1400000+rh.lay()+half*10000+lad*100,dxt-dx,1);
       hf1(1500000+rh.lay()+half*10000+lad*100,dyt-dy,1);

      int ilay=rh.lay();
      int ilad=rh.lad();
      int ihalf=rh.half();
      int isen=rh.sen();
     
//     ftxt1 <<" "<<rh.Hit[0]<<" "<<rh.Hit[1]<<" "<<rh.Hit[2]<<" "<<rh.EHit[0]<<" "<<rh.EHit[1]<<" "<<rh.EHit[2]<<" "<<rh.lay()<<" "<<rh.lad()<<" "<<rh.half()<<" "<<rh.sen()<<endl;
    }
      for(int i=0;i<2;i++){
         for(int j=0;j<3;j++){
//          ftxt1.write((char*)&(part.TRDCoo[i][j]),sizeof(part.TRDCoo[0][0]));
         }
      }
      for(int i=0;i<4;i++){
         for(int j=0;j<3;j++){
//          ftxt1.write((char*)&(part.TOFCoo[i][j]),sizeof(part.TOFCoo[0][0]));
         }
      }
      for(int i=0;i<3;i++){
         for(int j=0;j<3;j++){
//          ftxt1.write((char*)&(part.EcalCoo[i][j]),sizeof(part.EcalCoo[0][0]));
         }
      }
     
}
}


void daqe::UTerminate()
{
ftxt.close();
ftxt1.close();
int evt=0;
for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
if(runs[i]){
 evt+=events[i];
 cout <<runs[i]<<" "<<events[i]<<" "<<evt<<endl;
}
else break;  
}
for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
if(runs[i]){
 cout <<runs[i]<<",";
}
else break;
}
cout <<endl;
}




int main(int argc, char *argv[]){
    int option_index = 0;
    char fout[]="daqe521p.all.4p.root                                  ";
    static struct option long_options[] = {
        {"events", 1, 0, 'e'},
        {"help",    0, 0, 'h'},
        {"thread",  1, 0, 't'},
     {"file",  1, 0, 'f'},
    {"thickmemory",  1, 0, 'm'},
        {0, 0, 0, 0}
    };
     int events=1000000000;
     int thread=omp_get_num_procs();
      int mem=0;
    while (1) {
        int c = getopt_long (argc, argv, "e:t:hH?f:m", long_options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 'm':
              mem=1;
              break;
            case 'e':             /* display */
             events=atoi(optarg);
             break;
            case 'f':             /* display */
             strcpy(fout,optarg);
             break;
            case 't':             
             int t=atoi(optarg);
             if(t>0&& t<thread)thread=t;
             break;
            case 'h':
            case 'H':
            case '?':
            default:            /* help */
                cout<<"daqe_mt  -events[e] -thread[t] --file[f] --thickmemory[m]"<<endl;
                return 0;
                break;


        }
    }



      AMSEventR::fgThickMemory=mem;
      AMSChain chain("AMSRoot",thread,sizeof(daqe));
chain.Add("/s0dat0/Data/AMS02/2009A/data2009/cosmics.magof/*.root");
chain.Add("/r0fc00/Data/AMS02/2009A/data2009/cosmics.magof/*.root"); 
//chain.Add("/s0fc00/Data/AMS02/2006A/data2008/cern.cosmics.sci.4p/120791*.root");
chain.Process(new daqe[thread],fout,events);
return 0;
}

