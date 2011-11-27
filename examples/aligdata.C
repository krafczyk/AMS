		///   \example stlv.C
		///
		#define stlv_cxx
		#include "root_RVS.h"
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
	static int nruns=-1;
extern "C" void geocoor_alig_(float & glat, float &glong, float & galtt, float &cutoff, float &blat, float &cm_min, float& cm_max);
	class daqec : public AMSEventR {
	   public :
   static float mcmomentum;
	   daqec(TTree *tree=0){ };


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




float daqec::mcmomentum=400;

	void daqec::UBegin(){
	    cout << " Begin calling"<<endl;
	    for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++)runs[i]=0;
	   //here create histograms
	    hbook1(-10,"cuts",25,-0.5,24.5);
            hbook1s(11,"charge",20,-0.5,19.5);
            hbook1s(12,"beta.pattern",10,-0.5,9.5);
            hbook1s(112,"beta",200,-1.1,1.1);
            hbook1s(212,"partmass",200,0,10);
            hbook1s(312,"richmass",200,0,10);
            hbook1s(13,"trdtr",10,-0.5,9.5);
            hbook1s(14,"thetam",200,-1.1,1.1);
            hbook1s(15,"cm_min",200,0,20);
            hbook1s(16,"trstatus",2,-0.5,1.5);
            hbook1s(17,"rigidityf/cm_min",200,0,5);
            hbook1s(117,"rigidityf",200,-200,200);
            hbook1s(217,"rigidityf",200,-1,3);
            hbook1s(219,"rigidityf",2000,-1,3);
            hbook1s(221,"rigidityf",2000,-1,3);
            hbook1s(222,"rigidityf",2000,-1,3);
            hbook1s(310,"rigidityf",2000,-1,3);
            hbook1s(311,"rigidityf",2000,-1,3);
            hbook1s(313,"rigidityf",2000,-1,3);
            hbook1s(18,"ecal",200,0,20);
            hbook1s(19,"fchi2ms",200,0,2000);
            hbook1s(119,"fchi2fast",200,0,2000);
            hbook1s(20,"ntof+nanti",10,-0.5,9.5);
            hbook1s(120,"ntof",10,-0.5,9.5);
            hbook1s(220,"nanti",10,-0.5,9.5);
            hbook1s(21,"ntrrechit",10,-0.5,9.5);
            hbook1s(22,"ntrtr",10,-0.5,9.5);
            for(int k=0;k<10;k++){
             for(int l=0;l<20;l++){
               for(int s=0;s<2;s++){
                  hbook1(1000+k+(l+20*s)*10," resid x",400,-0.02,0.02);
                  hbook1(2000+k+(l+20*s)*10," resid y",400,-0.02,0.02);
                 for(int charge=0;charge<5;charge++){
                  hbook1(10000+k+(l+20*s)*10+charge*1000," resid x",400,-0.02,0.02);
                  hbook1(90000+k+(l+20*s)*10+charge*1000," resid x",400,-0.5,399.5);
                  hbook1(20000+k+(l+20*s)*10+charge*1000," resid x",400,-0.02,0.02);
                  hbook1(30000+k+(l+20*s)*10+charge*1000," resid y",400,-0.02,0.02);
                  hbook1(40000+k+(l+20*s)*10+charge*1000," resid x",400,-0.02,0.02);
                  hbook1(50000+k+(l+20*s)*10+charge*1000," resid y ",400,-0.02,0.02);
                  hbook1(60000+k+(l+20*s)*10+charge*1000," diff x ",400,-0.02,0.02);
                  hbook1(70000+k+(l+20*s)*10+charge*1000," diff y ",400,-0.02,0.02);
                  hbook1(80000+k+(l+20*s)*10+charge*1000," diff z ",400,-0.02,0.02);
               }
     }
   }
}
}

///*
bool daqec::UProcessStatus(unsigned long long status){
   return true;
}
//*/
// UProcessCut() is not mandatory
bool daqec::UProcessCut(){
/*
      static char f[1024]="";
      const char * fname=_Tree->GetCurrentFile()->GetName();

     if(strcmp(f,fname)){
        cout <<" getfile "<<_Tree->GetCurrentFile()->GetName()<<endl;
        strcpy(f,fname); 
     }
*/
     if (NParticle()==0  ||  nTrTrack()==0  ) return false;
     return true;
}

void daqec::UProcessFill(){
try{
     static int runo=0;
     const int nbadl=2;
     int blay[3]={3,5,0};
     int blad[3]={2,3,0};
     int bhalf[3]={0,1,-1};
#pragma omp threadprivate (runo)
{
}


     bool cuts[23];
    ParticleR part=Particle(0);
    if(part.iTrTrack()<0 )return;
    if(part.iTrdTrack()<0)return;
    if(part.iBeta()<0)return;
    for(int k=0;k<sizeof(cuts)/sizeof(cuts[0]);k++)cuts[k]=true;
    TrdTrackR trd=TrdTrack(part.iTrdTrack());
    TrTrackR tr=TrTrack(part.iTrTrack());
    double richb=1;
    double tofbeta=1;
     BetaR beta=Beta(part.iBeta());
     tofbeta=beta.Beta;
     double mass2=0;
    if(part.iRichRing()>=0){
      RichRingR rich=RichRing(part.iRichRing());
      richb=rich.Beta;
      if(richb>1)richb=1;
      double igamma=sqrt(1/richb/richb-1);
      mass2=part.Momentum*igamma;
    }
     float mcmom=10000;
     bool mc=false;
    if(nMCEventg()>0){		
     mc=true;
     MCEventgR mc_ev=MCEventg(0);
      mcmom=mc_ev.Momentum/mc_ev.Charge;
    }
    float rmc=1;
    
     float ecale=0;

    if(nEcalShower()>0){
       EcalShowerR ecal= EcalShower(0);
       ecale=ecal.EnergyC*(1-ecal.RearLeak);
    }	
     

     
       TrTrackFitR ftot(0,0,1,1);
//     internal tracker only aligned
       TrTrackFitR fi(-3,0,1,1);
//     no ms internal tracker only object aligned
//
       TrTrackFitR fii(-3,0,1,0);
//     internal tracker only ext align
//       TrTrackFitR::SetAligDB("TrAligglDBP_lay",nMCEventg()>0?0:1);
//      TrTrackFitR::SetAligDB_TDVR("TrAligglDBP",nMCEventg()>0?0:1,true);
//        TrTrackFitR::SetAligDB("TrAligglDBP_lad",nMCEventg()>0?0:1);
       TrTrackFitR::SetAligDB("TrAligglDBP_senc",nMCEventg()>0?0:1);
//       TrTrackFitR::SetAligDB("TrAligglDBP",nMCEventg()>0?0:1);

       TrTrackFitR fiii(-3,0,3,0);
       TrTrackFitR fms(-3,2,1,0);
// noalignd  internal tracker only int residuals  VC fit  
       TrTrackFitR fnoalig(-3,0,0,0);
//  noaligned internal tracker only ext residuals VC fit
       TrTrackFitR noaligext(-3,0,0,100);
       TrTrackFitR fiiiext(-3,0,3,100);
       TrTrackFitR noaligc(-3,2,0,1);
       TrTrackFitR fia(-3,1,1,1);
       TrTrackFitR fic(-3,2,1,1);
       TrTrackFitR f1(-1,2,1,1);
       TrTrackFitR f2(-2,2,1,1);
       int ifms=tr.iTrTrackFit(fms);
       int iftot=tr.iTrTrackFit(ftot);
       int ifi=tr.iTrTrackFit(fi);
       cout <<" Rig0 "<<fi.Rigidity<<" "<<fi.NHits()<<" "<<Event()<<endl;
  //forced refit of fii object
       int ifi1=tr.iTrTrackFit(fii,2);
       //cout <<" Rig1 "<<fii.Rigidity<<" "<<fii.NHits()<<" "<<Event()<<endl;
  //refit of fiii object
       int ifi3=tr.iTrTrackFit(fiii,1);
       int ifi3ext=tr.iTrTrackFit(fiiiext,1);
       //cout <<" Rig3 "<<fiii.Rigidity<<" "<<fiii.NHits()<<" "<<Event()<<endl;
       int ifia=tr.iTrTrackFit(fia);
       //cout <<" rig a "<<fia.Rigidity<<endl;
       int ific=tr.iTrTrackFit(fic);
       //cout <<" rig c "<<fic.Rigidity<<endl;
       int if1=tr.iTrTrackFit(f1);
       int if2=tr.iTrTrackFit(f2);
       int inoalig=tr.iTrTrackFit(fnoalig);
       //cout <<" Rig0n "<<fnoalig.Rigidity<<" "<<fnoalig.NHits()<<" "<<Event()<<endl;
       int inoaligi=tr.iTrTrackFit(fnoalig,2);
       //cout <<" Rig1n "<<fnoalig.Rigidity<<" "<<fnoalig.NHits()<<" "<<Event()<<endl;
       int inoaligext=tr.iTrTrackFit(noaligext,1);
       //int i2=tr.iTrTrackFit(noaligext,2);
       int inoaligc=tr.iTrTrackFit(noaligc);
    int l19=0;
    int l1=0;
    int l9=0;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
       int lay,lad,half,stripx,stripy;
       rh.Geom(lay,lad,half,stripx,stripy);
       if(lay==1 || lay==9)l19++;
       if(lay==1)l1=1;
       if(lay==9)l9=1;
     }
//     if(l19==0)return;
     cuts[0]=part.Charge>0 and part.Charge<40;
      float cutoff,blat,cm_min,cm_max;
      geocoor_alig_(fHeader.ThetaS,fHeader.PhiS,fHeader.RadS,cutoff,blat,cm_min,cm_max);
      cuts[1]=beta.Pattern<5;
      cuts[2]=tofbeta>0.9; 
     cuts[3]=part.Momentum/part.Charge>10;
     cuts[3]=true;
      int ntrrrh=tr.NTrRecHit();
    cuts[5]=(tr.Status/8192)%2==0 ;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
       int lay,lad,half,stripx,stripy;
       rh.Geom(lay,lad,half,stripx,stripy);
//       if(lay==1 || lay==9)ntrrrh--;
    }
   

    if(!cuts[5]){
    bool hfound=false;
    bool ahfound=false;
    for(int i=0;i<tr.NTrRecHit();i++){
     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
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
    cuts[5]=cuts[5] && ((tr.Status/16384)%2==0) ;

     cuts[6]=tr.Rigidityf()>10;
     cuts[6]=true;
       cuts[7]=ecale<2;
       cuts[8]=tr.FChi2MSf()<1000;
       cuts[9]=NTofCluster()+NAntiCluster()<7;
       cuts[10]=ntrrrh>=5;
       cuts[11]=NTrTrack()<4;
    //cuts[12]=NTrdTrack()<2;
    hf1s(11,part.Charge,cuts,sizeof(cuts)/sizeof(cuts[0]),1);
    hf1s(12,beta.Pattern,cuts,sizeof(cuts)/sizeof(cuts[0]),2);
    hf1s(112,tofbeta,cuts,sizeof(cuts)/sizeof(cuts[0]),3);
    hf1s(212,part.Mass,cuts,sizeof(cuts)/sizeof(cuts[0]),2);
    hf1s(312,mass2,cuts,sizeof(cuts)/sizeof(cuts[0]),2);
    hf1s(13,NTrdTrack(),cuts,sizeof(cuts)/sizeof(cuts[0]),13);
    hf1s(14,fHeader.ThetaM,cuts,sizeof(cuts)/sizeof(cuts[0]),4);
    hf1s(15,cm_min,cuts,sizeof(cuts)/sizeof(cuts[0]),5);
    hf1s(16,(tr.Status/8192)%2,cuts,sizeof(cuts)/sizeof(cuts[0]),6);
    hf1s(17,tr.Rigidityf()/cm_min,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(117,tr.Rigidityf(),cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(217,mcmom/fnoalig.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(219,mcmom/ftot.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(221,mcmom/fi.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(222,mcmom/fii.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    if(l1==1)hf1s(310,mcmom/ftot.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    if(l9==1)hf1s(311,mcmom/ftot.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    if(l19==2)hf1s(313,mcmom/ftot.Rigidity,cuts,sizeof(cuts)/sizeof(cuts[0]),7);
    hf1s(18,ecale,cuts,sizeof(cuts)/sizeof(cuts[0]),8);
    hf1s(19,tr.FChi2MS,cuts,sizeof(cuts)/sizeof(cuts[0]),9);
    hf1s(119,tr.Chi2FastFit,cuts,sizeof(cuts)/sizeof(cuts[0]),9);
    hf1s(20,NTofCluster()+NAntiCluster(),cuts,sizeof(cuts)/sizeof(cuts[0]),10);
    hf1s(120,NTofCluster(),cuts,sizeof(cuts)/sizeof(cuts[0]),10);
    hf1s(220,NAntiCluster(),cuts,sizeof(cuts)/sizeof(cuts[0]),10);
    hf1s(21,ntrrrh,cuts,sizeof(cuts)/sizeof(cuts[0]),11);
    hf1s(22,NTrTrack(),cuts,sizeof(cuts)/sizeof(cuts[0]),12);
    bool cut=true;
    for(int k=0;k<sizeof(cuts)/sizeof(cuts[0])-1;k++){
    if(cut)hf1(-10,k);
     cut=cut && cuts[k];
    }

    if(cut){
           
//             GetAllContents();
//              Fill();
       static int print=0;
       if(print++<10){
           //cout <<" "<<ifms<<" "<<ifi<<" "<<ifia<<" "<<ific<<" "<<if1<<" "<<if2<<" "<<inoalig<<" "<<inoaligc<<endl;
        }
                  for(int i=0;i<tr.NTrRecHit();i++){
                     TrRecHitR rh=TrRecHit(tr.iTrRecHit(i));
     //     dx,dy,dz  hits differenr due to the alignment (non alignd == test beam geometry)
                     double dx=tr.Hit[i][0]-rh.Hit[0];  
                     double dy=tr.Hit[i][1]-rh.Hit[1];
                     double dz=tr.Hit[i][2]-rh.Hit[2];
       int lay,lad,half,stripx,stripy;
       rh.Geom(lay,lad,half,stripx,stripy);
                  hf1(60000,dx,1);                       
                  hf1(70000,dy,1);                       
                  hf1(80000,dz,1);                       
                  hf1(60000+lay,dx,1);
                  hf1(70000+lay,dy,1);
                  hf1(80000+lay,dz,1);
                  hf1(60000+lay+(lad+half*20)*10,dx,1);
                  hf1(70000+lay+(lad+half*20)*10,dy,1);
                  hf1(80000+lay+(lad+half*20)*10,dz,1);
                  hf1(60000+part.Charge*1000,dx,1);                       
                  hf1(70000+part.Charge*1000,dy,1);                       
                  hf1(80000+part.Charge*1000,dz,1);                       
                  hf1(60000+lay+part.Charge*1000,dx,1);
                  hf1(70000+lay+part.Charge*1000,dy,1);
                  hf1(80000+lay+part.Charge*1000,dz,1);
                  hf1(60000+(lad+half*20)*10+lay+part.Charge*1000,dx,1);
                  hf1(70000+(lad+half*20)*10+lay+part.Charge*1000,dy,1);
                  hf1(80000+(lad+half*20)*10+lay+part.Charge*1000,dz,1);
//  ext residuals for ext aligned case  
                  AMSPoint hit;
                  int ret=fiiiext.getHit(lay,hit);
                  if(ret>1){
                      cerr<< "  error getting hit "<<lay<<" "<<ret;
                  } 
                  double dxt=hit[0]-fiiiext.fTrSCoo[lay-1].Coo[0];
                  double dyt=hit[1]-fiiiext.fTrSCoo[lay-1].Coo[1];
                  hf1(40000+part.Charge*1000,dxt,1);                   
                  hf1(50000+part.Charge*1000,dyt,1);                   
                  hf1(40000+lay+part.Charge*1000,dxt,1);                   
                  hf1(50000+lay+part.Charge*1000,dyt,1);                   
                  hf1(40000+(lad+half*20)*10+lay+part.Charge*1000,dxt,1);                   
                  hf1(50000+(lad+half*20)*10+lay+part.Charge*1000,dyt,1);                   
                  hf1(40000,dxt,1);                   
                  hf1(50000,dyt,1);                   
                  hf1(40000+lay,dxt,1);                   
                  hf1(50000+lay,dyt,1);                   
                  hf1(40000+(lad+half*20)*10+lay,dxt,1);                   
                  hf1(50000+(lad+half*20)*10+lay,dyt,1);                   
                 if(ifi3>0){
                   ret=fiii.getHit(lay,hit);
                   if(!ret && part.Charge==2){
//                        if(lay==2){
//                           ret=fiii.getHit(lay,hit);
//                           ifi3=tr.iTrTrackFit(fiii,2);
//                        }
                        dxt=hit[0]-fiii.fTrSCoo[lay-1].Coo[0];
                        dyt=hit[1]-fiii.fTrSCoo[lay-1].Coo[1];
                        hf1(1000+lay,dxt,1);                   
                        hf1(2000+lay,dyt,1);                   
                        hf1(1000,dxt,1);                   
                        hf1(2000,dyt,1);                   
                   }
                  }
                 if(inoaligext>0){
    //          external residuals for nonaligned == testbeam case
    //          change noaligext -> noalig   if internal residuals
                  ret=noaligext.getHit(lay,hit);
                        dxt=hit[0]-noaligext.fTrSCoo[lay-1].Coo[0];
                  dyt=hit[1]-noaligext.fTrSCoo[lay-1].Coo[1];
                  hf1(20000+part.Charge*1000,dxt,1);                   
                  hf1(10000+part.Charge*1000,dxt,rh.stripx());                 
                  hf1(30000+part.Charge*1000,dyt,1);                   
                  hf1(20000+lay+part.Charge*1000,dxt,1);                   
                  hf1(10000+lay+part.Charge*1000,dxt,rh.stripx());                   
                  hf1(30000+lay+part.Charge*1000,dyt,1);                   
                  //hf1(20000+(lad+half*20)*10+lay+part.Charge*1000,dxt,1);                   
                  hf1(10000+(lad+half*20)*10+lay+part.Charge*1000,dxt,rh.stripx());                   
                  hf1(30000+(lad+half*20)*10+lay+part.Charge*1000,dyt,1);                   
                  hf1(20000,dxt,1);                   
                  hf1(10000,dxt,rh.stripx());                   
                  hf1(30000,dyt,1);                   
                  hf1(20000+lay,dxt,1);                   
                  hf1(10000+lay,dxt,rh.stripx());                   
                  hf1(30000+lay,dyt,1);                   
                  hf1(20000+(lad+half*20)*10+lay,dxt,1);                   
                  hf1(21000+(half*20)*10+lay,dxt,1);                   
                  //if(stripx<64)hf1(22000+(half*20)*10+lay,dxt,1);                   
                   //else if(stripx<160)hf1(23000+(half*20)*10+lay,dxt,1);                   
                   //else  hf1(24000+(half*20)*10+lay,dxt,1);                   
                    hf1(90000+(lad+half*20)*10+lay,rh.stripx(),dxt);                   
                  hf1(91000+(lad+half*20)*10+lay,rh.stripx(),1);                   
                  hf1(30000+(lad+half*20)*10+lay,dyt,1);                   
                   }
                  }
         
    }
}


 
 catch (...)
    {
     cout <<"  daqec exception catched "<<" "<<Event()<<endl;
    return ;
    }

}
void daqec::UTerminate()
{
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


#include "../include/amschain.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>
void (handler)(int);
AMSChain *pchain=0;

int main(int argc, char *argv[]){

   *signal(SIGTERM, handler);
     *signal(SIGINT, handler);
     *signal(SIGUSR1, handler);

    int option_index = 0;
    char fout[]="                                           ISS.B538.root";
    static struct option long_options[] = {
        {"events", 1, 0, 'e'},
        {"help",    0, 0, 'h'},
        {"thread",  1, 0, 't'},
     {"file",  1, 0, 'f'},
    {"thickmemory",  1, 0, 'm'},
    {"order",1,0,'o'},
        {0, 0, 0, 0}
    };
     int events=1000000000;
#ifdef _OPENMP
     int thread=omp_get_num_procs()-1;
#else
int thread=1;
#endif
     long long order=0;
      int t=0;
      int mem=0;
    while (1) {
        int c = getopt_long (argc, argv, "e:t:hH?f:mo:", long_options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 'o':
            order=atoll(optarg);
              break;
            case 'm':
              mem=1;
              break;
            case 'e':             /* display */
             events=atol(optarg);
             break;
            case 'f':             /* display */
             strcpy(fout,optarg);
             break;
            case 't':
              t=atoi(optarg);
             if(t>0&& t<thread)thread=t;
             break;
            case 'h':
            case 'H':
            case '?':
            default:            /* help */
                cout<<"daqe_mt  -events[e] -thread[t] --file[f] --thickmemory[m] --order[o]" <<endl;
                return 0;
                break;


        }
    }

  daqec::mcmomentum=180;
     AMSEventR::fgThickMemory=mem;


unsigned long long pp=1;
if(order==0 || order/pp%10==1){
      AMSChain chain("AMSRoot",thread,sizeof(daqec));
chain.Add("/afs/cern.ch/ams//Offline/DataSetsDir/Data/AMS02/2011A/ISS.B538.v4/pass2_v5c_lay/*.root");
string fileoutput(fout);
fileoutput+=".v4";
cout <<" fileoutpot "<<fileoutput.c_str()<<endl;
pchain=&chain;
chain.Process(new daqec[thread],fileoutput.c_str(),events);
}



return 0;
}




void (handler)(int sig){
  switch(sig){
  case SIGTERM: case SIGINT:
#pragma omp master
{
    cerr <<" SIGTERM intercepted"<<endl;
    if(pchain){cerr<<" resetting "<<endl;pchain->nentries=-1;pchain->ntree=0;} 
}
    break;
  case SIGUSR1:
#pragma omp master
{
   cerr<<"   SIGUSR1 intercepted gracefully terminating"<<endl;
if(pchain){pchain->ntree=0;}
}
}
}
