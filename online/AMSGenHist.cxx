//  $Id: AMSGenHist.cxx,v 1.3 2003/07/01 06:42:20 choutko Exp $
// By V. Choutko & D. Casadei
// Last changes: 27 Feb 1998 by D.C.
#include <iostream.h>
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


