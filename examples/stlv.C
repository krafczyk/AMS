#define stlv_cxx
#include "stlv.h"
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
#include <iostream.h>
#include <stdlib.h>
#include "TStopwatch.h"
static    TStopwatch  * _pw;
/** \example stlv.C 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */




//create here pointers to histos and array of histos
    TH1F * acc[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 



void stlv::Begin(TTree *tree){
   // Function called before starting the event loop.
   // Initialize the tree branches. 
   // Book Histos
   Init(tree);

   TString option = GetOption();

   //here create histograms

   Float_t al1=log(0.5);
   Float_t al2=log(200.);
   Int_t nch=12;
   Float_t step=(al2-al1)/nch; 
   
    for (Int_t j = 0; j < 20; j++) {
      char AccName[80];
      sprintf(AccName,"acc%d",(j+1));
      if(acc[j])delete acc[j];
      acc[j] = new TH1F(AccName,"acceptance",nch,al1,al2);
   
   } 
   
     
   
          _pw =new TStopwatch();
        _pw->Start(); 
   
   
   
   

}

Bool_t stlv::ProcessCut(Int_t entry)
{
   // Selection function.
   // Entry is the entry number in the current tree.
   // Read only the necessary branches to select entries.
   // Return kFALSE as soon as a bad entry is detected.
   // To read complete event, call fChain->GetTree()->GetEntry(entry).

   ev.ReadHeader(entry);

   return kTRUE;
}

void stlv::ProcessFill(Int_t entry)
{
   // Function called for selected entries only.
   // Entry is the entry number in the current tree.
   // Read branches not processed in ProcessCut() and fill histograms.
   // To read complete event, call fChain->GetTree()->GetEntry(entry).
   

    
       
    
    Float_t xm=0;
    if(ev.nMCEventg()>0){		
     MCEventgR mc_ev=ev.MCEventg(0);
      xm = log(mc_ev.Momentum);
      acc[0]->Fill(xm,1);
     if(ev.nParticle()>0){
       
       int ptrack = ev.Particle(0).TrTrack();
       int ptrd = ev.Particle(0).TrdTrack();
       
       
       if(ev.NParticle()== 1 && ptrack>=0 && ptrd>=0){ //final if
         acc[1]->Fill(xm,1);
     
        int pbeta = ev.Particle(0).iBeta();   // here iBeta, not Beta
       
        if(pbeta>=0){			//check beta
	  BetaR Beta=ev.Beta(pbeta);
          if(fabs(Beta.Beta) < 2 && Beta.Chi2S < 5 && Beta.Pattern < 4){
	    if(ev.nTrdTrack()<2){
	    Int_t Layer1 =0;
            Int_t Layer2 =0;  
	    
             TrTrackR tr_tr=ev.TrTrack(ptrack);
		 int  ptrh=tr_tr.TrRecHit(0);			//pht1
   	          Layer1=ev.TrRecHit(ptrh).Layer;
		    
		 
		  ptrh=tr_tr.TrRecHit(tr_tr.NHits-1);			//pht2
   	           Layer2=ev.TrRecHit(ptrh).Layer;

 // alt method

                  TrRecHitR* pph=tr_tr.pTrRecHit(tr_tr.NHits-1);
                  int l2=pph->Layer;
         
}
}
}
}
}
}
}

void stlv::Terminate()
{
   // Function called at the end of the event loop.

  _pw->Stop();
   cout <<_pw->CpuTime()<<endl;

}
