///   \example stlv.C
///
#define stlv_cxx
#define _PGTRACK_
#include "/afs/cern.ch/ams/Offline/KSC/AMS/include/root_RVSP.h"
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
static unsigned long long comb[10000];
static unsigned long combn[10000];
static int ncomb=0;
static int nruns=-1;
class daqePG : public AMSEventR {
   public :

   daqePG(TTree *tree=0){ };


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
   virtual bool    UProcessStatus(unsigned long long status);

   /// User Function called to analyze each event.
   /// Called for all entries.
   /// Fills histograms.
   virtual void    UProcessFill();

   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();
};


//create here pointers to histos and/or array of histos

ofstream ftxt;
ofstream ftxt1;


void daqePG::UBegin(){
    cout << " Begin calling"<<endl;
    ftxt.open("selectaiee.txt");
    ftxt1.open("eventsaiee.txt");
    for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++)runs[i]=0;
   for(int i=0;i<sizeof(comb)/sizeof(comb[0]);i++)comb[i]=0;
  for(int i=0;i<sizeof(combn)/sizeof(combn[0]);i++)combn[i]=0;


   //here create histograms
    hbook1(-1201,"xcl si",10,-0.5,9.5);
    hbook1(-1202,"ycl si",10,-0.5,9.5);
   for (int k=0;k<10;k++){
     hbook1s(-10000-k-1,"  slot",50,-0.5,49.5);
    hbook1(410096+k*100," ",10,0,1);
    hbook1(510096+k*100," ",10,0,1);
    hbook1(401096+k*100," ",10,0,1);
    hbook1(501096+k*100," ",10,0,1);
    hbook1(1410096+k*100," ",10,0,1);
    hbook1(1510096+k*100," ",10,0,1);
    hbook1(96+k*100," ",10,0,1);
    hbook1(399096+k*100," ",10,0,1);
    hbook1(4399096+k*100," ",10,0,1);
    hbook1(1399096+k*100," ",10,0,1);
    hbook1(14399096+k*100," ",10,0,1);
    hbook1(410098+k*100," ",10,0,1);
    hbook1(510098+k*100," ",10,0,1);
    hbook1(401098+k*100," ",10,0,1);
    hbook1(501098+k*100," ",10,0,1);
    hbook1(400098+k*100," ",10,0,1);
    hbook1(500098+k*100," ",10,0,1);
    hbook1(1410098+k*100," ",10,0,1);
    hbook1(1510098+k*100," ",10,0,1);
    hbook1(98+k*100," ",10,0,1);
    hbook1(399098+k*100," ",10,0,1);
    hbook1(4399098+k*100," ",10,0,1);
    hbook1(1399098+k*100," ",10,0,1);
    hbook1(14399098+k*100," ",10,0,1);
   }
    for( int k=0;k<2000;k++){
     hbook1s(10000000+k," ",100,0.,2.);
   }
  for(int kk=0;kk<3;kk++){
   for(int k=0;k<2;k++){

  hbook1(6+k*100+kk*1000,"resol",200,-1.,3.);
   hbook1(7+k*100+kk*1000,"resol",200,-1.,3.);
   hbook1(8+k*100+kk*1000,"resol",200,-1.,3.);
   }
  }
   for(int kk=0;kk<3;kk++){
   for(int k=0;k<6;k++){
   hbook1(-6-k*100-kk*1000,"resol",200,-1.,3.);
   hbook1(-7-k*100-kk*1000,"resol",200,-1.,3.);
   hbook1(-8-k*100-kk*1000,"resol",200,-1.,3.);
   }
   }
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
    hbook1(3215,"chi2 space",200,0,100);
    hbook1(3216,"chi2 b",200,0,100);
    hbook1(3299,"mass",2000,0.,2.5);
    hbook1(-3299,"mass",2000,0.,2.5);
    hbook1(3298,"mass",2000,0.,2.5);
    hbook1(-3298,"mass",2000,0.,2.5);
    hbook1(3297,"mass",2000,0.,2.5);
    hbook1(-3297,"mass",2000,0.,2.5);
    hbook1(13299,"mass",2000,0.,2.5);
    hbook1(-13299,"mass",2000,0.,2.5);
    hbook1(13298,"mass",2000,0.,2.5);
    hbook1(-13298,"mass",2000,0.,2.5);
    hbook1(13297,"mass",2000,0.,2.5);
    hbook1(-13297,"mass",2000,0.,2.5);
    hbook1(23297,"mass",2000,0.,2.5);
    hbook1(-23297,"mass",2000,0.,2.5);
    hbook1(3300,"mass",2000,0.,2.5);
    hbook1(3311,"mass",2000,0.,2.5);
    hbook1(-3311,"mass",2000,0.,2.5);
    hbook1(3301,"mass",2000,0.,2.5);
    hbook1(3302,"mass rich",2000,0.,2.);
    hbook1(-3302,"mass rich",2000,0.,2.);
    hbook1(3303,"mass rich",2000,0.,2.);
    hbook1(3333,"mass rich",2000,0.,2.);
    hbook1(3304,"mass rich",2000,0.,2.);
    hbook1(-3303,"mass rich",2000,0.,2.);
    hbook1(-3333,"mass rich",2000,0.,2.);
    hbook1(-3304,"mass rich",2000,0.,2.);
    hbook1(3305,"chi2",2000,0.,200.);


    hbook1(3401,"trd hit",200,0.,200.);
    hbook1(3402,"trd cl ",200,0.,10.);
    hbook1(3403,"trd tr av",200,0.,10.);

    hbook1(3501,"ecal hit",2000,0.,200.);
    hbook1(3502,"ecal cl ",2000,0.,200.);
    hbook1(3503,"ecal  2d sh ",2000,0.,5000.);
    hbook1(3504,"ecal  sh ",2000,0.,10000.);
    hbook2(3505,"ecal vs tr",200,0.,50.,200,0.,50.);
    hbook2(3506,"ecal vs tr",200,0.,50.,200,0.,50.);
    hbook1(3507,"ecal hit a",2000,0.,2000.);
    hbook1(3601,"resid x vs 1/mom ",2000,0.,2.);
    hbook1(3602,"resid y vs 1/mom ",2000,0.,2.);
    hbook1(13601,"resid x vs 1/mom ",2000,0.,2.);
    hbook1(13602,"resid y vs 1/mom ",2000,0.,2.);
    hbook1(3605,"resid x vs 1/mom ",2000,0.,2.);
    hbook1(3606,"resid y vs 1/mom ",2000,0.,2.);
    hbook1(13605,"resid x vs 1/mom ",2000,0.,2.);
    hbook1(13606,"resid y vs 1/mom ",2000,0.,2.);
    hbook1(3603,"mom",2000,-1000,1000);  
    hbook1(13603,"mom",2000,-500,500);  
    hbook1(113604,"mom",2000,-500,500);  
    hbook1(13604,"mom",2000,-500,500);  
    hbook2(3604,"x vs y ",100,0,0.02,100,0.,0.02);
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
    hbook1(7011," beta tof", 400,0.5,1.5);
    hbook1(17011," beta tof", 1000,-2,2);
    hbook1(-7011," beta tof", 400,-1.5,-0.5);
    hbook1(7001," beta tof", 400,0.5,1.5);
    hbook1(17001," beta tof", 1000,-2,2);
    hbook1(-7001," beta tof", 400,-1.5,-0.5);
    hbook1(7002," beta rich", 400,0.75,1.05);
    hbook1(7003," charge rich", 400,0.5,2);
    float ax=0.02;
    hbook1(500011,"resid y (cm)",200,-ax,ax);
    hbook1(500012,"resid y (cm)",200,-ax,ax);
    hbook1(500013,"resid y (cm)",200,-ax,ax);
    hbook1(500014,"resid y (cm)",200,-ax,ax);
    hbook1(500015,"resid y (cm)",200,-ax,ax);
  for(int ilad=0;ilad<20;ilad++){
   for(int ih=0;ih<2;ih++){ 
    hbook1(400010+ih*10000+ilad*100,"resid x (cm)",200,-ax,ax);
    hbook1(500010+ih*10000+ilad*100,"resid y (cm)",200,-ax,ax);

    hbook1(1400010+ih*10000+ilad*100,"resid x (cm)",200,-ax/5,ax/5);
    hbook1(1500010+ih*10000+ilad*100,"resid y (cm)",200,-ax/5,ax/5);
   for(int i=0;i<9;i++){

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
 for(int i=0;i<9;i++){
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
///*
bool daqePG::UProcessStatus(unsigned long long status){
   if( !(status & 3) ||  !((status>>4)&1))return false;
  else return true;
}
//*/
// UProcessCut() is not mandatory
bool daqePG::UProcessCut(){
/*
      static char f[1024]="";
      const char * fname=_Tree->GetCurrentFile()->GetName();

     if(strcmp(f,fname)){
        cout <<" getfile "<<_Tree->GetCurrentFile()->GetName()<<endl;
        strcpy(f,fname); 
     }
*/
     if (nParticle()==0  || nTrTrack()==0   ) return false;
     return true;
}

void daqePG::UProcessFill()
{
try{

    bool ecm=false;
    bool mc=false;
    float tofcoo[4][6];
    float clcoo[20][3];
     const int nbadl=3;
     int blay[3]={4,6,8};
     int blad[3]={7,12,7};
     int bhalf[3]={1,1,0};
   // User Function called for all entries .
   // Entry is the entry number in the current tree.
   // Fills histograms.
     //cout<<Run()<<" "<<Event()<<endl; 
     static int runo=0;
//#pragma omp threadprivate (runo)
     if(Run()!=runo){
        cout <<"  tracks found run "<<Run()<<endl;
        runs[++nruns]=Run();
        events[nruns]=0;
        runo=Run();
     }   
     events[nruns]++;
     //if(events[nruns]>10000)return;
     static int written=0;
     const int maxw=200;
     bool cuts[23];
      for(int k=0;k<sizeof(cuts)/sizeof(cuts[0]);k++)cuts[k]=true;
    ParticleR part=Particle(0);
    if(part.iTrTrack()<0 )return;
    BetaR beta=Beta(part.iBeta());
        double o=3.45e-2;
        double s=0.98;
        double b1=beta.Beta/s-o;
//        if(beta.Beta>0)b1=beta.Beta*1.005;
//        else b1=beta.Beta*1.035;
     if(fabs(part.Momentum)<5 && fabs(b1)<0.91 && b1>0.3) {
        bool edge=false;
        for(int i=0;i<beta.NTofCluster();i++){
         TofClusterR tofc=TofCluster(beta.iTofCluster(i));
         if(tofc.Bar==1 || tofc.Bar==10){
           edge=true;
           break;
         }
        }
        double pm=part.Momentum/part.Mass;
        pm=pm*pm;
//        double b1=sqrt(pm/(1+pm));
//        b1=b1*1.007;
        double o=3.45e-2;
        double s=0.98;
        b1=beta.Beta/s-o;
        if(beta.Beta>0)b1=beta.Beta*1.005;
        else b1=beta.Beta*1.035;
        double g=1/sqrt(1-b1*b1);
        double m4=fabs(part.Momentum)/fabs(b1)/g;
        if(part.Momentum>0)hf1(3297,m4,1.);
        else hf1(-3297,m4,1.);
        if(part.Momentum>0)hf1(3299,part.Mass,1.);
        else hf1(-3299,part.Mass,1.);
        double gamma=1/sqrt(1-beta.Beta*beta.Beta);
        double m1=part.Momentum/fabs(beta.Beta)/gamma; 
        if(m1>0)hf1(3298,m1,1.);
        else hf1(-3298,-m1,1.);
        if(beta.Pattern==0 ){
        hf1(3215,beta.Chi2S,1);
        hf1(3216,beta.Chi2,1);
        }
       if(beta.Pattern==0 && beta.Chi2<16 &&  beta.Chi2S<8 ){
              if(part.Momentum>0)hf1(13297,m4,1.);
        else hf1(-13297,m4,1.);
        if(part.Momentum>0)hf1(13299,part.Mass,1.);
        else hf1(-13299,part.Mass,1.);
        double gamma=1/sqrt(1-beta.Beta*beta.Beta);
        double m1=part.Momentum/fabs(beta.Beta)/gamma;
        if(m1>0)hf1(13298,m1,1.);
        else hf1(-13298,-m1,1.);
        if(!edge){
              if(part.Momentum>0)hf1(23297,m4,1.);
        else hf1(-23297,m4,1.);
        }
        }
       }

    if(part.iRichRing()>=0){
       RichRingR rich=RichRing(part.iRichRing());
       if(rich.Used>3){
        hf1(7002,rich.Beta,1);
        float charge=sqrt(rich.NpCol/rich.NpExp);
        hf1(7003,charge,1);
       }
       if(fabs(part.Momentum)<2){
        hf1(3300,fabs(part.Mass),1.);
       }
       if(rich.Beta<0.97 && fabs(part.Momentum)<4 &&beta.Beta>0){
        double gamma=1/sqrt(1-rich.Beta*rich.Beta);
        double m1=part.Momentum/rich.Beta/gamma; 
         if(part.Momentum>0)hf1(3311,m1,1);
         else hf1(-3311,fabs(m1),1);
       }
       if(rich.Beta<0.997 && fabs(part.Momentum)<10){
        hf1(3301,fabs(part.Mass),1.);
        double gamma=1/sqrt(1-rich.Beta*rich.Beta);
        double m1=part.Momentum/rich.Beta/gamma; 
        if(m1>0) hf1(3302,m1,1.);
        if(m1<0)hf1(-3302,fabs(m1),1);
        TrTrackR trr=TrTrack(part.iTrTrack());
        double m2=trr.GetRigidity(TrTrackR::kAlcaraz)/rich.Beta/gamma; 
        hf1(3303,m2,1.);
        double m3=trr.GetRigidity(TrTrackR::kChikanian)/rich.Beta/gamma; 
       double b1=rich.Beta*1.0005;
        double g=1/sqrt(1-b1*b1);
        double m5=trr.GetRigidity(TrTrackR::kAlcaraz)/b1/g; 
        hf1(3304,m3,1.);
        hf1(3333,m5,1.);        
        hf1(-3304,-m3,1.);
        hf1(-3333,-m5,1.);        
        hf1(3305,trr.Chi2FastFitf(),1.);        
     }
    }
     TrTrackR tr=TrTrack(part.iTrTrack());
     int bars[4]={6,5,3,2};
     if(part.iBeta()>=0){
      BetaR beta=Beta(part.iBeta());
        bool nah=false;
        for(int i=0;i<beta.NTofCluster();i++){
         TofClusterR tofc=TofCluster(beta.iTofCluster(i));
         if(tofc.Bar!=bars[tofc.Layer-1] &&tofc.Layer-1<2){
           nah=true;
           break;
         }
        }
        
      if(beta.Pattern==0 && !nah){
        //cout <<" beta"<<beta.Beta<<endl;
        double o=3.45e-2;
        double s=0.98;
        double b1=beta.Beta/s-o;
        if(beta.Beta>0)b1=beta.Beta*1.005;
        else b1=beta.Beta*1.035;
        hf1(17001,beta.Beta,1);
        hf1(7001,beta.Beta,1);
        hf1(-7001,beta.Beta,1);
        hf1(17011,b1,1);
        hf1(7011,b1,1);
        hf1(-7011,b1,1);
      }
     }
       
    //cout<< "trhit "<<tr.NTrRecHit()<<endl;
    hf1(210,tr.NTrRecHit(),1);
    for(int i=0;i<tr.NTrRecHit();i++){
     //cout <<tr.iTrRecHit(i)<<endl;
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
     TrClusterR y=TrCluster(rh.iTrCluster('y'));
     int lay=rh.lay();
     int lad=rh.GetTkId();
      int half=lad>0?1:0;
      lad=rh.GetSlot();
     hf1(1200+lay,lad,1);
     //hf1(-1202,y.Amplitude.size(),1);
    }
    Level1R lvl1=Level1(0);
    hf1(113,lvl1.TrigTime[4]/1000.,1);
//     double nx1=sin(tr.Theta)*cos(tr.Phi);
//     double ny1=sin(tr.Theta)*sin(tr.Phi);
//     double nz1=cos(tr.Theta);
     double nx1=-tr.GetDir()[0];
     double ny1=-tr.GetDir()[1];
     double nz1=-tr.GetDir()[2];
     cuts[6]=part.iTrdTrack()>=0 && part.iTrTrack()>=0;

     int nlay=0;
     for (int i=0;i<nTofCluster();i++){
      nlay+=TofCluster(i).Layer;
     }   
     cuts[2]=nlay==10;
     //cout <<" q 0"<<endl;
     if(part.iEcalShower()>=0 ){
      //cout <<" q ecal"<<" "<<NEcalShower()<<" "<<part.iEcalShower()<<endl;
       float az=nx1/nz1;
      float  bz=ny1/nz1;
      EcalShowerR ecal=EcalShower(part.iEcalShower());
      hf1(6021,ecal.EnergyC,1);
      //cout <<" qwa "<<endl;
      double esum=0;
      for (int k=0;k<ecal.NEcal2DCluster();k++){
           //cout <<k<<" "<<ecal.iEcal2DCluster(k)<<endl;
              Ecal2DClusterR ec2d=Ecal2DCluster(ecal.iEcal2DCluster(k));
           hf1(3503,ec2d.Edep,1.);
           esum+=ec2d.Edep;
           for(int l=0;l<ec2d.NEcalCluster();l++){
              EcalClusterR ecl=EcalCluster(ec2d.iEcalCluster(l));
              hf1(3502,ecl.Edep,1.);
              for(int m=0;m<ecl.NEcalHit();m++){
               EcalHitR echt=EcalHit(ecl.iEcalHit(m));
                hf1(3501,echt.Edep,1.);
                hf1(3507,echt.ADC[0],1.);
              }
           }
        }
        hf1(3504,esum,1.);
        if(part.iTrTrack()>=0){
          hf2(3505,fabs(part.Momentum),esum/1000.,1.);
          hf2(3506,fabs(part.Momentum),ecal.EnergyC,1);
          if(fabs(part.Momentum)>10 && esum/1000.>10)ecm=true; 
        }

       //
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

      //cout <<" q 1"<<endl;
     if(part.iTrdTrack()>=0){
     TrdTrackR trd=TrdTrack(part.iTrdTrack());
     double sum=0;
     double nsum=0;
     for(int k=0;k<trd.NTrdSegment();k++){
        TrdSegmentR tseg=TrdSegment(trd.iTrdSegment(k));
        for (int l=0;l<tseg.NTrdCluster();l++){
          TrdClusterR trdcl=TrdCluster(tseg.iTrdCluster(l));
          sum+=trdcl.EDep;
          nsum++;
          hf1(3402,trdcl.EDep,1.);
          TrdRawHitR trdht=TrdRawHit(trdcl.iTrdRawHit());
          hf1(3401,trdht.Amp,1.);
        }
       }
       if(nsum){
        hf1(3403,sum/nsum,1.);
      }

     cuts[0]=NTrdTrack()<2 && trd.NTrdSegment()>=3;
     double nx=sin(trd.Theta)*cos(trd.Phi);
     double ny=sin(trd.Theta)*sin(trd.Phi);
     double nz=cos(trd.Theta);
     hf1(10211,nz1,1);
      //cout <<" q 2 "<<endl;
     double c=nx*nx1+ny*ny1+nz*nz1;
     c=acos(c);
     hf1(211,c,1);
   hf1(3212,tr.Chi2FastFitf(),1);
     //hf1(3214,tr.DBase[1],1);
     //if((tr.Status/8192)%2==1)hf1(3213,tr.Chi2FastFit,1);
     //   cout <<" c "<<c<<endl;
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


//cout <<" q 3"<<endl;
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
      //cout <<" q 4 "<<endl;
    cuts[1]=nTofCluster()<6;
//    cuts[2]=nlay==3;
    cuts[3]=nTrTrack()>0;
//    cuts[5]=(tr.Status/8192)%2==0 ;
       cuts[5]=false;
    if(!cuts[5]){
    bool hfound=false;
    bool ahfound=false;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
       //number dxt=rh.CofgX+1.1e-3-part.TrCoo[rh.lay()-1][0];
       //number dzt=rh.Hit[2]-part.TrCoo[rh.lay()-1][2];
       //number dyt=rh.CofgY-part.TrCoo[rh.lay()-1][1]-2e-4;
        AMSPoint Hit=rh.GetCoord();
       number dx=Hit[0]-part.TrCoo[rh.lay()-1][0];
       number dy=Hit[1]-part.TrCoo[rh.lay()-1][1];
       number dz=Hit[2]-part.TrCoo[rh.lay()-1][2];
    }
//    cuts[5]=cuts[5] && (tr.Status/16384)%2==0 ;
    cuts[7]=fabs(part.Momentum)>0;   
      cuts[5]=fabs(tr.Rigidityf())>10;
    Level1R lvl1=Level1(0);
     int membpat=lvl1.JMembPatt;
       int b15=(membpat>>15)&1;
       int b14=(membpat>>14)&1;
          cuts[7]=b15 && b14;

    bool cut=true;
    for(int i=0;i<8;i++){
      cut=cut && cuts[i];
    }
 //   for(int i=0;i<7;i++)cout<<i<<" "<<cuts[i]<<" ";
//    cout<<endl;    
    cut=true;
    if(cut){

//
//   calculate parameters
//
//


   
      hf1(3603,part.Momentum,1.);
//      cout <<part.Beta<<" "<<prt.Momentum<<endl;
      if(part.Beta>0)hf1(13603,part.Momentum,1.);
      else {
      hf1(13604,-part.Momentum,1.);
      hf1(113604,part.Momentum,1.);
      }
     float mcmom=400;
        float mccharge=1;
    if(nMCEventg()>0){		
     mc=true;
      MCEventgR mc_ev=MCEventg(0);
      mcmom=mc_ev.Momentum;
      mccharge=mc_ev.Charge;
    }
     bool h6=tr.NTrRecHit()>=6;
     bool h7=true;
     //fabs(cos(tr.Theta))>fabs(cos(20./180.*3.141592));
     bool h1=false;
     bool h9=false;
     bool h8=false;
     bool v394=Version()>393;
     unsigned long long ic=0;
      int icc=0;
   for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
     if(rh.lay()==8)h1=true;
     else if(rh.lay()==9)h9=true; 
     else if(rh.lay()==1)h8=true;
      unsigned long long p=1;
       for(int  k=1;k<rh.lay();k++)p*=100;
     if(rh.lay()==9 || rh.lay()==8)  ic+=p*(rh.GetTkId()%100+15);
     if(rh.lay()==8)icc+=rh.GetTkId()%100+14;
     if(rh.lay()==9)icc+=29*(rh.GetTkId()%100+8);


      if(abs(rh.GetTkId()%100)>15)cerr<<"   getslot problem"<<rh.GetSlot()<<endl; 
   }  
      int icomb=-1;    
if(h6 && h1 &&h9){
    for(int i=0;i<ncomb;i++){
               if(comb[i]==ic){
                  icomb=i;
                  break;
               }
     }
     if(icomb<0){
       comb[ncomb++]=ic;
        //cout <<"  comb found "<<ncomb<<endl;;
//                  for(int  k=1;k<10;k++){
     unsigned long long p=1;
     //  for(int  kk=1;kk<k;kk++)p*=100;
     //              cout <<"  "<<(ic/p)%100;
     //             }
 //cout <<endl;


        if(ncomb>sizeof(comb)/sizeof(comb[0]))ncomb=sizeof(comb)/sizeof(comb[0]);
       icomb=ncomb-1;
      } 
          for(int  k=1;k<10;k++){

     unsigned long long p=1;
       for(int  kk=1;kk<k;kk++)p*=100;
                  hf1s(-10000-k,(ic/p)%100,cuts,sizeof(cuts)/sizeof(cuts[0]),6);
                  }
        
}
   //cout <<" qwa "<<endl;
double x1=mcmom/fabs(mccharge)/(tr.Rigidityf());
      double x3=mcmom/fabs(mccharge)/(tr.GetRigidity(TrTrackR::kChikanian));
      double x2=mcmom/fabs(mccharge)/(tr.GetRigidity(TrTrackR::kAlcaraz));
      double xx[3][2];
   //cout <<" q1 "<<endl;
      for(int k=0;k<3;k++){
       for(int kk=0;kk<2;kk++){
          //xx[k][kk]=mcmom/fabs(mc_ev.Charge)/tr.RigidityIE[k][kk];
       }
       }
      hf1(6,x1,1.);
      hf1(7,x2,1.);
      hf1(8,x3,1.);
      if(h6){
      hf1(106,x1,1.);
      hf1(107,x2,1.);
      hf1(108,x3,1.);
   for(int k=0;k<3;k++){
        if(v394)for(int kk=0;kk<2;kk++)hf1(106+k+(kk+1)*1000,xx[k][kk]);
      }

      if(h1 ){
       hf1(-6,x1,1.);
       hf1(-7,x2,1.);
       hf1(-8,x3,1.);
    for(int k=0;k<3;k++){
        if(v394)for(int kk=0;kk<2;kk++)hf1(-6-k-(kk+1)*1000,xx[k][kk]);
      }

      }
      if(h9 ){
       hf1(-106,x1,1.);
       hf1(-107,x2,1.);
       hf1(-108,x3,1.);
    for(int k=0;k<3;k++){
        if(v394)for(int kk=0;kk<2;kk++)hf1(-106-k-(kk+1)*1000,xx[k][kk]);
      }
      }
      if(h9 && h1){
       hf1(-206,x1,1.);
       hf1(-207,x2,1.);
       hf1(-208,x3,1.);
        combn[icc+1]++;
       hf1s(10000000+icc+1,x3,cuts,sizeof(cuts)/sizeof(cuts[0]),2);
       hf1s(10000000,x3,cuts,sizeof(cuts)/sizeof(cuts[0]),2);

    for(int k=0;k<3;k++){
        if(v394)for(int kk=0;kk<2;kk++)hf1(-206-k-(kk+1)*1000,xx[k][kk]);
      }
      }
      if(!h9 && !h1){
       hf1(-406,x1,1.);
       hf1(-407,x2,1.);
       hf1(-408,x3,1.);
    for(int k=0;k<3;k++){
        if(v394)for(int kk=0;kk<2;kk++)hf1(-406-k-(kk+1)*1000,xx[k][kk]);
      }

      }
       if(h1 && h8){
       hf1(-306,x1,1.);
       hf1(-307,x2,1.);
       hf1(-308,x3,1.);
        if(h7){
       hf1(-506,x1,1.);
       hf1(-507,x2,1.);
       hf1(-508,x3,1.);

        }
    for(int k=0;k<3;k++){
        for(int kk=0;kk<2;kk++)hf1(-306-k-(kk+1)*1000,xx[k][kk]);

      }
       }
      }


     hf1(212,tr.Chi2FastFitf(),1);
     //hf1(214,tr.DBase[1],1);
     //if((tr.Status/8192)%2==1)hf1(213,tr.Chi2FastFit,1);
      int ntrrh=tr.NTrRecHit();
      for(int i=0;i<20;i++){
         for(int j=0;j<3;j++){
          //ftxt1.write((char *)&(clcoo[i][j]),sizeof(clcoo[0][0]));
       }
       }
      for(int i=0;i<4;i++){
         for(int j=0;j<6;j++){
          //ftxt1.write((char *)&(tofcoo[i][j]),sizeof(tofcoo[0][0]));
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

         if(0 && cut && h8 && h1 && h9 && h6){
            cout<<"Run to Fill"<<Run()<<" "<<Event()<<" "<<part.Momentum<<" "<<TrTrack(part.iTrTrack()).Rigidityf()<<" "<<TrTrack(part.iTrTrack()).GetRigidity(TrTrackR::kChikanian)<<endl;
              GetAllContents();
              Fill();

         }

//     float ztrd[2]={0.05,0.05};
//     float xytrd[2]={-0.03,-0.11};
float ztrd[2]={0,0};
float xytrd[2]={0,0};
     for(int i=0;i<20;i++){
      if(clcoo[i][0]==0){
       float ddx=xytrd[1];
       float ddz=ztrd[1]; 
       hf1(4000,clcoo[i][1]+ddx-part.TRDCoo[0][1]-bz*(clcoo[i][2]+ddz-part.TRDCoo[0][2]),1);
       hf1(4001+i,clcoo[i][1]+ddx-part.TRDCoo[0][1]-bz*(clcoo[i][2]+ddz-part.TRDCoo[0][2]),1);
       hf1(14000,bz,clcoo[i][1]+ddx-part.TRDCoo[0][1]-bz*(clcoo[i][2]+ddz-part.TRDCoo[0][2]));
       hf1(14001+i,bz,clcoo[i][1]+ddx-part.TRDCoo[0][1]-bz*(clcoo[i][2]+ddz-part.TRDCoo[0][2]));
       hf1(114000,bz,1);
       hf1(114001+i,bz,1);
       
      }
      else if(clcoo[i][0]==1){
     float ddx=xytrd[0];
       float ddz=ztrd[0];

         hf1(5000,clcoo[i][1]+ddx-part.TRDCoo[0][0]-az*(clcoo[i][2]+ddz-part.TRDCoo[0][2]),1);
         hf1(5001+i,clcoo[i][1]+ddx-part.TRDCoo[0][0]-az*(clcoo[i][2]+ddz-part.TRDCoo[0][2]),1);
         hf1(15000,az,clcoo[i][1]+ddx-part.TRDCoo[0][0]-az*(clcoo[i][2]+ddz-part.TRDCoo[0][2]));
         hf1(15001+i,az,clcoo[i][1]+ddx-part.TRDCoo[0][0]-az*(clcoo[i][2]+ddz-part.TRDCoo[0][2]));
         hf1(115000,az,1);
         hf1(115001+i,az,1);
      }
     }
          //ftxt1.write((char *)&(az),sizeof(az));
          //ftxt1.write((char *)&(bz),sizeof(az));
//      //ftxt1<<tr.NTrRecHit()<<" "<<tr.Chi2FastFit<<" "<<tr.Pattern<<" "<<tr.Address<<" "<<tr.AddressA<<" "<<tr.Rigidity<<" "<<mcmom<<endl;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
            AMSPoint Hit=rh.GetCoord();
         //cout <<rh.lay()<<" "<<Hit[2]<<" " <<part.TrCoo[rh.lay()-1][2]<<endl; 
       number dx=Hit[0]-part.TrCoo[rh.lay()-1][0];
       number dy=Hit[1]-part.TrCoo[rh.lay()-1][1];
       number dz=Hit[2]-part.TrCoo[rh.lay()-1][2];
       hf1(400010,dx,1);
       hf1(500010,dy,1);
       if(fabs(dx)<0.006)hf1(500011,dy,1);
       if(fabs(dx)<0.005)hf1(500012,dy,1);
       if(fabs(dx)<0.004)hf1(500013,dy,1);
       if(fabs(dx)<0.003)hf1(500014,dy,1);
       float rinv=1./(fabs(part.Momentum)+1.e-10);
       hf1(3601,rinv,dx*dx);
       hf1(3602,rinv,dy*dy);
       hf2(3604,fabs(dx),fabs(dy),1);
       hf1(13601,rinv,1);
       hf1(13602,rinv,1);
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

       int half=rh.GetTkId()>0?1:0;
       int lad=rh.GetSlot();
       hf2(1600000+rh.lay()+half*10000+lad*100,dx,y,1);
       hf2(1700000+rh.lay()+half*10000+lad*100,dy,x,1);
       hf2(2600000+rh.lay()+half*10000+lad*100,dx,x*nx1/nz1,1);
       hf2(2700000+rh.lay()+half*10000+lad*100,dy,y*ny1/nz1,1);
       hf2(3600000+rh.lay()+half*10000+lad*100,dx,y*nx1/nz1,1);
       hf2(3700000+rh.lay(),dy+half*10000+lad*100,x*ny1/nz1,1);



       int lay=rh.lay();

       hf2(700000+rh.lay(),dy,(ny1)/nz1,1.);
       hf1(600020+lay,nx1/nz1,dx);
       hf1(700020+lay,ny1/nz1,dy);
      hf1(600010+lay,nx1/nz1,1);
       hf1(700010+lay,ny1/nz1,1);

       hf1(500000+rh.lay(),dy,1);
/*
       hf1(1400010,dxt-dx,1);
       hf1(1500010,dyt-dy,1);
       if(dyt-dy<-1e-3){
         hf1(500+rh.lay(),rh.lad()*(rh.half()>0?1:-1));
       }       if(fabs(dxt-dx)>1e-3){
         hf1(400+rh.lay(),rh.lad()*(rh.half()>0?1:-1));
       }

       hf1(1400000+rh.lay(),dxt-dx,1);
       hf1(1500000+rh.lay(),dyt-dy,1);

*/
       hf1(400000+rh.lay()+half*10000+lad*100,dx,1);
       hf1(500000+rh.lay()+half*10000+lad*100,dy,1);
//       hf1(1400000+rh.lay()+half*10000+lad*100,dxt-dx,1);
//       hf1(1500000+rh.lay()+half*10000+lad*100,dyt-dy,1);

      int ilay=rh.lay();
//      int ilad=rh.lad();
//      int ihalf=rh.half();
//      int isen=rh.sen();
     
//     //ftxt1 <<" "<<rh.Hit[0]<<" "<<rh.Hit[1]<<" "<<rh.Hit[2]<<" "<<rh.EHit[0]<<" "<<rh.EHit[1]<<" "<<rh.EHit[2]<<" "<<rh.lay()<<" "<<rh.lad()<<" "<<rh.half()<<" "<<rh.sen()<<endl;
    }
      for(int i=0;i<2;i++){
         for(int j=0;j<3;j++){
          //ftxt1.write((char*)&(part.TRDCoo[i][j]),sizeof(part.TRDCoo[0][0]));
         }
      }
      for(int i=0;i<4;i++){
         for(int j=0;j<3;j++){
          //ftxt1.write((char*)&(part.TOFCoo[i][j]),sizeof(part.TOFCoo[0][0]));
         }
      }
      for(int i=0;i<3;i++){
         for(int j=0;j<3;j++){
          //ftxt1.write((char*)&(part.EcalCoo[i][j]),sizeof(part.EcalCoo[0][0]));
         }
      }
     
}
}
}
  catch (...)
    {
     static int i=0;
     if(i++<100);cout <<"  daqe exception catched "<<Run()<< " " <<Event()<<endl;
      //abort();
    return ;
    }

}


void daqePG::UTerminate()
{
ftxt.close();
ftxt1.close();
int evt=0;
for(int i=0;i<sizeof(combn)/sizeof(combn[0]);i++){
if(combn[i]>19){
cout <<"  comb "<<i<<" "<<combn[i]<<endl;
}
}

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

