#define stlv_cxx
#include "/offline/vdev/include/root_RVS.h"
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
/** \example stlv.C 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */



///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the ROOT class TSelector. \n
 The following members functions are called by the TTree::Process() functions: \n
    UBegin():       called everytime a loop on the tree starts, \n
                    a convenient place to create your histograms. \n
    ProcessFill():  called in the entry loop for all entries \n
    UTerminate():   called at the end of a loop on the tree,
                    a convenient place to draw/fit your histograms. \n

   To use this file, try the following session on your Tree T \n

 Root > T->Process("stlv.C")  \n
 Root > T->Process("stlv.C","some options") \n
  Root > T->Process("stlv.C+") \n
 Root > T->Process("stlv.C++") \n

*/
class stlv : public AMSEventR {
   public :

    vector<TH1F*>   h1A;   ///< An (alt) way of having array of histos
    
   stlv(TTree *tree=0){ };

   ~stlv() { 
    for(unsigned int i=0;i<h1A.size();i++)delete h1A[i];
        h1A.clear(); 
   }


   /// User Function called before starting the event loop.
   /// Book Histos
   virtual void    UBegin();
   /// User Function called for selected entries only.
   /// Entry is the entry number in the current tree.
   /// Fills histograms.
   void    ProcessFill(Int_t entry);

   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();
};


//create here pointers to histos and/or array of histos

    TH1F * acc[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 




void stlv::UBegin(){
    cout << " Begin called "<<endl;

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
   
   

}


void stlv::ProcessFill(Int_t entry)
{
   // User Function called for selected entries only.
   // Entry is the entry number in the current tree.
   // Fills histograms.
   
    
    Float_t xm=0;
    if(nMCEventg()>0){		
     MCEventgR mc_ev=MCEventg(0);
      xm = log(mc_ev.Momentum);
      acc[0]->Fill(xm,1);
      h1A[0]-> Fill(xm,1);
     if(nParticle()>0){
       int ptrack = Particle(0).TrTrack();
       int ptrd = Particle(0).TrdTrack();
       if(NParticle()== 1 && ptrack>=0 && ptrd>=0){ //final if
         acc[1]->Fill(xm,1);
         h1A[1]-> Fill(xm,1);
     
        int pbeta = Particle(0).iBeta();   // here iBeta, not Beta
        BetaR *pb =  Particle(0).pBeta();   // another way 
        if(pbeta>=0){			//check beta
	  BetaR BetaI=Beta(pbeta);
          if(fabs(BetaI.Beta) < 2 && pb->Chi2S < 5 && BetaI.Pattern < 4){
	    if(nTrdTrack()<2){
	    Int_t Layer1 =0;
            Int_t Layer2 =0;  
             TrTrackR tr_tr=TrTrack(ptrack);
		 int  ptrh=tr_tr.TrRecHit(0);			//pht1
   	          Layer1=TrRecHit(ptrh).Layer;
		    
		 
		  ptrh=tr_tr.TrRecHit(tr_tr.NTrRecHit()-1);			//pht2
   	           Layer2=TrRecHit(ptrh).Layer;

 // alt method

                  TrRecHitR* pph=tr_tr.pTrRecHit(tr_tr.NTrRecHit()-1);
                  int l2=pph->Layer;
                  cout <<"l "<<Layer2<<l2<<endl;                           
}
}
}
}
}
}
}

void stlv::UTerminate()
{
  
}

