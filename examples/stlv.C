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



//create here pointers to histos and/or array of histos

    TH1F * acc[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 




void stlv::Begin(TTree *tree){
    cout << " Begin called "<<endl;
   // User Function called before starting the event loop.
   // Initialize the tree branches.
   // Book Histos
   Init(tree);

   TString option = GetOption();

   // open file if...
   
   if(option.Length()>1){
    pOutFile=new TFile(option,"RECREATE");
    cout <<" write file opened"<<endl;
   }

   //here create histograms

   float al1=log(0.5);
   float al2=log(200.);
   int nch=12;
    
    for (int j = 0; j < sizeof(acc)/sizeof(acc[0]); j++) {
      char AccName[255];
      sprintf(AccName,"acc%03d",j);
      if(acc[j])delete acc[j];
      acc[j] = new TH1F(AccName,"acceptance",nch,al1,al2);
//
//    another way of def histos
//
      sprintf(AccName,"accv%03d",j);
      h1A.push_back(new TH1F(AccName,"acceptance",nch,al1,al2));         
   } 
   
     
   
        _pw =new TStopwatch();
        _pw->Start(); 
   
   
   
   

}

Bool_t stlv::ProcessCut(Int_t entry)
{
   // Selection function.
   // Entry is the entry number in the current tree.
   // Read only the header to select entries.
   // May return kFALSE as soon as a bad entry is detected.
   // Should Not be modified by (Non)Advanced User

  ev.ReadHeader(entry);

   return kTRUE;
 }

void stlv::ProcessFill(Int_t entry)
{
   // User Function called for selected entries only.
   // Entry is the entry number in the current tree.
   // Fills histograms.
   
    
    Float_t xm=0;
    if(ev.nMCEventg()>0){		
     MCEventgR mc_ev=ev.MCEventg(0);
      xm = log(mc_ev.Momentum);
      acc[0]->Fill(xm,1);
      h1A[0]-> Fill(xm,1);
     if(ev.nParticle()>0){
       int ptrack = ev.Particle(0).TrTrack();
       int ptrd = ev.Particle(0).TrdTrack();
       if(ev.NParticle()== 1 && ptrack>=0 && ptrd>=0){ //final if
         acc[1]->Fill(xm,1);
         h1A[1]-> Fill(xm,1);
     
        int pbeta = ev.Particle(0).iBeta();   // here iBeta, not Beta
        BetaR *pb =  ev.Particle(0).pBeta();   // another way 
        if(pbeta>=0){			//check beta
	  BetaR Beta=ev.Beta(pbeta);
          if(fabs(Beta.Beta) < 2 && pb->Chi2S < 5 && Beta.Pattern < 4){
	    if(ev.nTrdTrack()<2){
	    Int_t Layer1 =0;
            Int_t Layer2 =0;  
	    
             TrTrackR tr_tr=ev.TrTrack(ptrack);
		 int  ptrh=tr_tr.TrRecHit(0);			//pht1
   	          Layer1=ev.TrRecHit(ptrh).Layer;
		    
		 
		  ptrh=tr_tr.TrRecHit(tr_tr.NTrRecHit()-1);			//pht2
   	           Layer2=ev.TrRecHit(ptrh).Layer;

 // alt method

                  TrRecHitR* pph=tr_tr.pTrRecHit(tr_tr.NTrRecHit()-1);
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
   if(pOutFile){
     pOutFile->Write();
     pOutFile->Close();
   }
}
