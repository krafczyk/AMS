//  $Id: AMSGenHist.cxx,v 1.4 2004/02/22 15:39:39 choutko Exp $
#include <iostream>
#include "AMSDisplay.h"
#include <TGraphErrors.h>
#include "AMSNtuple.h"
#include <TProfile.h>
#include <TPaveText.h>
#include <TAttAxis.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "AMSGenHist.h"



void AMSGenHist::Book(){ 
}


void AMSGenHist::ShowSet(Int_t Set){
}



void AMSGenHist::Fill(AMSNtupleR *ntuple){ 
    Float_t xm=0;
//    if(ntuple->nMCEventg()>0){		
//     MCEventgR mc_ev=ntuple->MCEventg(0);
//      xm = log(mc_ev.Momentum);
//     } 
}


