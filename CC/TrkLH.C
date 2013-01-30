// routine for tracker based likelihood to reduce charge confusion             
// F.N. 05/12/2012: created 

#ifndef __TRKLK_CXX__
#define __TRKLK_CXX__

#include <stdlib.h>
#include <signal.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <TString.h>
#include <TMath.h>
#include <TFile.h>
#include <TStyle.h>
#include <TH3D.h>
#include <TH2D.h>
#include <TH1F.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TLegend.h>
#include <TChain.h>
#include <time.h>
#include <cstdlib>
#include <TLine.h>
#include <TArrow.h>

#ifndef _NOGBATCH_
#include "TrTrack.h"
#include "root.h"
#include "root_setup.h"
#include "TofTrack.h"
#include "TrTrackSelection.h"
#endif

#include "TrkLH.h"

TrkLH* TrkLH::head=0;

using namespace std;

ClassImp(TrkLH)

TrkLH::TrkLH(){

  NVER=-9999;
  VARN=-9999;
  PATN=-9999;

  varv=0;

  varmask=0;
  varlist=0;
  varnorm=0;
  
  hlist0=0;
  hlist1=0;
  low=0;
  upp=0;

  int nm = TRLKINIT();
  if (nm!=0) cerr << "TrkLH::TRLKINIT-E-Error in TrkLH inizialization: " << nm << endl;

};

TrkLH::~TrkLH(){
  clear();
  head=0;
};

void TrkLH::clear(){

  if (varv) delete varv;

  if (varmask) delete[] varmask;
  if (varlist) delete[] varlist;
  if (varnorm) delete[] varnorm;
  
  for (int ii=0; ii<VARN; ii++) {
    if (hlist0[ii]) delete[] hlist0[ii];
    if (hlist1[ii]) delete[] hlist1[ii];
    if (low[ii]) delete[] low[ii];
    if (upp[ii]) delete[] upp[ii];
  }
  if (hlist0) delete[] hlist0;
  if (hlist1) delete[] hlist1;
  if (low) delete[] low;
  if (upp) delete[] upp;

  NVER=-9999;
  VARN=-9999;
  PATN=-9999;

  varmask=0;
  varlist=0;
  varnorm=0;
  
  hlist0=0;
  hlist1=0;
  low=0;
  upp=0;

  return;
}

TrkLH* TrkLH::gethead() {
  if (!head) {
    printf("TrkLH::gethead()-M-CreatingObjectTrkLHAsSingleton\n", head);
    head = new TrkLH();//this also initialize at the default version
  }
  return head;
}

void TrkLH::renewhead() {
  
    printf("TrkLH::renewhead()-M-ForcingTheRenewalOfTheHead\n");
    head = new TrkLH();//this also initialize at the default version
    
    return;
}

int TrkLH::TRLKINIT(int _NVER){

#define NVERMIN 1
#define NVERMAX 1

  if (_NVER<NVERMIN)
    cerr << "TrkLH::TRLKINIT-E-This version " << _NVER << " is no more supported. Ask for, at least, version " << NVERMIN << endl;  
  else if (_NVER>NVERMAX)
    cerr << "TrkLH::TRLKINIT-E-This version " << _NVER << " is not yet supported. Ask for, at most, version " << NVERMAX << endl;
  else {
    if (_NVER==NVER) {
      cerr << "TrkLH::TRLKINIT-W-This version " << _NVER << " is the same already inited..." << endl;
      return 0;
    }
    else {//Now yes!
      //Let's clean the previous instance...
      clear();
      
      NVER=_NVER;
      
      if (NVER==1) return TRLKINIT_v1();
    }
  }

  return -1;
}

int TrkLH::TRLKINIT_v1(){
  // load data for normalization of variables
  // load the PDF ow wrong and good reconstructed
  // fix the varmask  (0 = variable not used in TrkLH)

  //  printf("NVER = %d\n", NVER);//only for debug
  VARN=23;
  PATN=6;
  //  printf("VARN = %d, PATN = %d\n", VARN, PATN);//only for debug

  varv = new TrkLHVar();

  varmask = new int[VARN];
  varlist = new double[VARN];
  varnorm = new double[VARN];

  hlist0 = new TH2D**[VARN];
  hlist1 = new TH2D**[VARN];
  low = new TGraph**[VARN];
  upp = new TGraph**[VARN];
  for (int ii=0; ii<VARN; ii++) {
    hlist0[ii] = new TH2D*[PATN];
    hlist1[ii] = new TH2D*[PATN];
    low[ii] = new TGraph*[PATN];
    upp[ii] = new TGraph*[PATN];
  }

  //-----------------------------------------------

  TDirectory* curr = gDirectory;

  const char* dbPath;
  const char* dbdir;
  dbPath = getenv("TrkLHDataDir");
  if (dbPath==NULL) {
    dbdir = getenv("AMSDataDir");
    if (dbdir!=NULL) {
      dbPath = Form("%s/v5.00/TrkLH", dbdir);
    }
    //    else dbPath=".";
    else dbPath="/afs/cern.ch/exp/ams/Offline/AMSDataDir/v5.00/TrkLH";
  }

  char nfile[100];

  sprintf(nfile, "%s/normfile_%d.root", dbPath, NVER);
  TFile* infile = new TFile(nfile);
  if (!infile || infile->IsZombie()) {
    cerr << "TrkLH::TRKLKINIT-E-" << dbPath << "/normfile_" << NVER << ".root not found!" << endl;
    return 1;
  }
  cout << "TrkLH::TRKLKINIT-M-Loaded " << nfile << endl; 

  sprintf(nfile, "%s/pdffile_%d.root", dbPath, NVER);
  TFile* pdffile = new TFile(nfile);  
  if (!pdffile || pdffile->IsZombie()) {
    cerr << "TrkLH::TRKLKINIT-E-" << dbPath << "/pdffile_" << NVER << ".root not found!" << endl;
    return 2;
  }
  cout << "TrkLH::TRKLKINIT-M-Loaded " << nfile << endl; 

  for (int ij = 0;ij<VARN;ij++){
    for (int ip = 0;ip<PATN;ip++){ // 0 to be implemented
      int ipp = ip;
      //      if (ipp ==0) ipp=1; test old pdf
      sprintf(nfile,"l_%d_%d",ij,ipp);
      //      printf("TRLKINIT: Loaded %s", nfile);
      low[ij][ip]= (TGraph*) infile->Get(nfile); // low threshold
      sprintf(nfile,"u_%d_%d",ij,ipp);
      //      printf(" %s", nfile);
      upp[ij][ip]= (TGraph*) infile->Get(nfile); // upper threshold
      sprintf(nfile,"enh0_%d_%d",ij,ipp);
      //      printf(" %s", nfile);
      hlist0[ij][ip]= (TH2D*) pdffile->Get(nfile); // wrong R PDF
      sprintf(nfile,"enh1_%d_%d",ij,ipp);
      //      printf(" %s\n", nfile);
      hlist1[ij][ip]= (TH2D*) pdffile->Get(nfile); // good R PDF
      //      curazeri(hlist0[ij][ip]);
      //      curazeri(hlist1[ij][ip]);
      varmask[ij]=1;
    }
  }
  varmask[22]=0; // E/P not used in TrkLH
  cout << "TrkLH::TRKLKINIT-M-LoadedInfosForAll" << VARN << "VariablesForAll" << PATN << "TrackerPatterns" << endl; 
  
  if (curr) curr->cd();
  
  return 0; 
}

//-----------------------------------------------------------------------

double TrkLH::GetLikelihoodRatioElectronWG(int ipart){
  
  int ret = PopulateTrkLHVar(ipart);

  if (ret<0) return ret;
  else return dotrklk((*varv).energy, (*varv).pat);
}

int TrkLH::PopulateTrkLHVar(int ipart){
  
  int nm = filltrklhvarfromgbatch(*varv, AMSEventR::Head(), ipart);
  if (nm!=1) {
    cerr << "TrkLH:GetLikelihoodRatioElectronWG:-E-TrkLHVarLoading: " << nm << endl;
    return -9;
  }
  //   cout << " var loaded " << endl;
  
  nm = fillvar(*varv);
  if (nm!=0) {
    cerr << "TrkLH:GetLikelihoodRatioElectronWG-E-TrkLHVarFilling: " << nm << endl;
    return -8;
  }
  //   cout << " varv filled " << endl;
  
  return 0;
}

TrkLHVar TrkLH::GetTrkLHVar(int ipart){
  
  int ret = PopulateTrkLHVar(ipart);

  return (*varv);
}

//-----------------------------------------------------------------------

int TrkLH::fillvar(TrkLHVar & var1){
  // fill the TrkLH data structure and normalize the variables
  // starting from the pre-filled human readable data structure TrkLHVar

  var1.ly1 = var1.btp%2;
  var1.ly9 = 0;
  if (var1.btp > 255) var1.ly9 = 1;
  var1.ly2 = 0;
  if(var1.btp & (1 << 1)) var1.ly2=1;

  var1.pat = -1;
  if (var1.ly1==0 && var1.ly9==0 && var1.ly2==0) var1.pat=0;  // select 000
  if (var1.ly1==0 && var1.ly9==0 && var1.ly2==1) var1.pat=1;  // select 001
  if (var1.ly1==0 && var1.ly9==1 && var1.ly2==0) var1.pat=2;  // select 010
  if (var1.ly1==0 && var1.ly9==1 && var1.ly2==1) var1.pat=3;  // select 011
  if (var1.ly1==1 && var1.ly9==0               ) var1.pat=4;  // select 10x
  if (var1.ly1==1 && var1.ly9==1               ) var1.pat=5;  // select 11x
  if (var1.pat == -1) return var1.pat;
  //  cout << var1.btp << " " << var1.ly1 << var1.ly9 << var1.ly2 << endl;

  double vrinmx = 1.e-19;
  if (var1.pat == 5){
    if ((var1.rigidity_in+var1.rigidity_ch)!=0) {vrinmx = fabs((var1.rigidity_in-var1.rigidity_ch)/(var1.rigidity_in+var1.rigidity_ch));}
    else vrinmx = 1.e19;}

  double vrbt = 1.e-19;
  if (var1.pat == 5){
    if ((var1.rigidity_bt+var1.rigidity_tp)!=0){vrbt = fabs((var1.rigidity_bt-var1.rigidity_tp)/(var1.rigidity_bt+var1.rigidity_tp));}
    else vrbt = 1.e19;}

  double vribt = 1.e-19;
  if ((var1.rigidity_ibt+var1.rigidity_itp)!=0){vribt = fabs((var1.rigidity_ibt-var1.rigidity_itp)/(var1.rigidity_ibt+var1.rigidity_itp));}
    else vribt = 1.e19;

  double vrchal = 1.e-19;
  if ((var1.rigidity_ch+var1.rigidity_al)!=0) {vrchal = fabs((var1.rigidity_ch-var1.rigidity_al)/(var1.rigidity_ch+var1.rigidity_al));}
  else vrchal = 1.e19;

  double dxte = fabs(var1.ecal_ecogx-var1.trk_ecogx);
  double dyte = fabs(var1.ecal_ecogy-var1.trk_ecogy);

  varlist[0]=fabs(var1.chiy_ch);
  (varlist[0]!=0) ? varlist[0] = 4.*log10(varlist[0]) : -19;
  varlist[1]=fabs(var1.chix_ch);
  (varlist[1]!=0) ? varlist[1] = 2.*log10(varlist[1]) : -19;
  varlist[2]=fabs(var1.err_ch);
  (varlist[2]!=0) ? varlist[2] = 45.+15.*log10(varlist[2]) : -19;
  varlist[3]=fabs(vrinmx);
  (varlist[3]!=0) ? varlist[3] = 2.*log10(varlist[3]) : -19;
  varlist[4]=fabs(vribt);
  (varlist[4]!=0) ? varlist[4] = 2.*log10(varlist[4]) : -19;
  varlist[5]=fabs(vrbt);
  (varlist[5]!=0) ? varlist[5] = 2.*log10(varlist[5]) : -19;
  varlist[6]=fabs(vrchal);
  (varlist[6]!=0) ? varlist[6] = 2.*log10(varlist[6]) : -19;
  varlist[7]=15.*var1.trkqin-19.;
  varlist[8]=40.*var1.trkqrms-19.;
  varlist[9]=fabs(var1.minoise1);
  (varlist[9]!=0) ? varlist[9] = 3.+3.*log10(varlist[9]) : -19;
  varlist[10]=fabs(var1.minoise2);
  (varlist[10]!=0) ? varlist[10] = 3.+3.*log10(varlist[10]) : -19;
  varlist[11]=fabs(var1.minoise3);
  (varlist[11]!=0) ? varlist[11] = 3.+3.*log10(varlist[11]) : -19;
  varlist[12]=fabs(var1.minoise4);
  (varlist[12]!=0) ? varlist[12] = 3.+3.*log10(varlist[12]) : -19;
  varlist[13]=var1.nlayer-6.;
  varlist[14]=var1.ntrk-3.;
  varlist[15]=var1.nanti-4;
  varlist[16]=fabs(1.-(var1.cosy)*(var1.cosy));
  (varlist[16]>0) ? varlist[16] = 15.+300.*log10(varlist[16]) : -19;
  varlist[17]=6.*var1.tof_qd-19.;
  varlist[18]=6.*var1.tof_qu-19.;
  varlist[19]=fabs(var1.sinq_trk_ecal);
  (varlist[19]!=0) ? varlist[19] = 15.+3.*log10(varlist[19]) : -19;
  varlist[20]=fabs(dxte);
  (varlist[20]!=0) ? varlist[20] = 15.+3.*log10(varlist[20]) : -19;
  varlist[21]=fabs(dyte);
  (varlist[21]!=0) ? varlist[21] = 15.+3.*log10(varlist[21]) : -19;
  if (var1.rigidity_ch != 0) varlist[22]=var1.energy/fabs(var1.rigidity_ch);
  else varlist[22]= 0;
  (varlist[22]!=0) ? varlist[22] = 8.*log10(varlist[22]) : -19;

  for (int iv = 0; iv<VARN;iv++){ // do normalization                                             
    varnorm[iv] = normvar(low[iv][var1.pat],upp[iv][var1.pat],varlist[iv],fabs(var1.rigidity_ch));
    if (iv >9 && iv<16) varnorm[iv] = varlist[iv];  // have problems in normalizations           
  }

  for (int iv = 0; iv<VARN;iv++){   // do under-overflows                                                
    if (varnorm[iv]<=-19) {varnorm[iv] = -19.;}
    if (varnorm[iv]>= 19) {varnorm[iv] =  19.;}
  }

   return 0; 
}

void TrkLH::curazeri(TH2D* h){
  // if PDF bin is empty (value is 0)
  // the nonzero error of the bin will be used
  double min = 1;
  for (int ix = 0; ix<h->GetNbinsX()+1;ix++){
    for (int iy = 0; iy<h->GetNbinsY()+1;iy++){
      double val = h->GetBinContent(ix,iy);
      double err = h->GetBinError(ix,iy);
      if (val <= 0 && err>0) {
	h->SetBinContent(ix,iy,err);
      }
      if (val <= 0 && err<=0) {
	cout << val << " ERROR CURAZERI " << err << endl;
      }
    }
  }
}

double TrkLH::bilinear(TH2D* h, double y, double x, int ilin){
  // from a TH2D interpolate/exptrapolate value at (x,y)             
  // ilin = 0 no interpolation                              
  // ilin = 1 bilinear interpolation                                                                 
  int binx1 = 0;
  int binx2 = 0;
  int biny1 = 0;
  int biny2 = 0;
  int bing1 = 0;
  int bing2 = 0;
  bing1 = h->FindBin(x,y);
  double vret = h->GetBinContent(bing1); // NO INTERPLATION                                          
  int extrapx = 0;
  int extrapy = 0;
  if(ilin != 0 ) {
    // WEIGHTED BILIEAR INTERPOLATION in the following                                                 
    // but it is not suitable for integer var                                                          
    h->GetBinXYZ(bing1,binx1,biny1,bing2);
    double x1 = h->GetXaxis()->GetBinCenter(binx1);
    if (x1 > x) {binx1=binx1-1;}
    if (binx1<1) extrapx = 1;
    if (binx1<1) binx1 = binx1+1;
    if (binx1>h->GetNbinsX()) extrapx=h->GetNbinsX();
    if (binx1>h->GetNbinsX()) binx1 = binx1-1;
    binx2 = binx1+1;
    x1 = h->GetXaxis()->GetBinCenter(binx1);
    double x2  = h->GetXaxis()->GetBinCenter(binx2);
    double y1 = h->GetYaxis()->GetBinCenter(biny1);
    if (y1 > y) {biny1=biny1-1;}
    if (biny1<1) extrapy = 1;
    if (biny1<1) biny1 = biny1+1;
    if (biny1>h->GetNbinsY()) extrapy = h->GetNbinsY();
    if (biny1>h->GetNbinsY()) biny1 = biny1-1;
    biny2 = biny1+1;
    y1 = h->GetYaxis()->GetBinCenter(biny1);
    if (y1 > y) {biny1=biny1-1;}
    if (biny1<1) biny1 = biny1+1;
    if (biny1>h->GetNbinsY()) biny1 = biny1-1;
    biny2 = biny1+1;
    y1 = h->GetYaxis()->GetBinCenter(biny1);
    double y2  = h->GetYaxis()->GetBinCenter(biny2);
    double z11 = h->GetBinContent(binx1,biny1);
    double pz11 = h->GetBinError(binx1,biny1);
    double z12 = h->GetBinContent(binx1,biny2);
    double pz12 = h->GetBinError(binx1,biny2);
    double z22 = h->GetBinContent(binx2,biny2);
    double pz22 = h->GetBinError(binx2,biny2);
    double z21 = h->GetBinContent(binx2,biny1);
    double pz21 = h->GetBinError(binx2,biny1);
    (pz11!=0) ? pz11 = 1./(pz11*pz11) : 0;
    (pz12!=0) ? pz12 = 1./(pz12*pz12) : 0;
    (pz21!=0) ? pz21 = 1./(pz21*pz21) : 0;
    (pz22!=0) ? pz22 = 1./(pz22*pz22) : 0;
    double bil = z11*pz11*(x2-x)*(y2-y);
    bil  = bil - z21*pz21*(x1-x)*(y2-y);
    bil  = bil - z12*pz12*(x2-x)*(y1-y);
    bil  = bil + z22*pz22*(x1-x)*(y1-y);
    double pes = pz11*(x2-x)*(y2-y);
    pes  = pes - pz21*(x1-x)*(y2-y);
    pes  = pes - pz12*(x2-x)*(y1-y);
    pes  = pes + pz22*(x1-x)*(y1-y);
    if (pes == 0) {
      cout << " ERROR BILINEAR INTERPOLATION " << bil <<endl;
      cout << z11 << " " << z12 << " " << z21 << " " << z22 << endl;
      cout << pz11 << " " << pz12 << " " << pz21 << " " << pz22 << endl;
      cout << x1 << " " << x << " " << x2 << endl;
      cout << y1 << " " << y << " " << y2 << endl;}
    else bil = bil/pes;
    vret = bil;
  }
  if (extrapx != 0) vret = h->GetBinContent(extrapx,biny1);
  if (extrapy != 0) vret = h->GetBinContent(binx1,extrapy);
  if (extrapx != 0 && extrapy != 0) vret = h->GetBinContent(extrapx,extrapy);

  if (vret > 1. || vret < 0) cout << extrapx << extrapy << " vret = " << vret << " binx1= " << binx1 << " ilin= " << ilin << " biny1= " << biny1 << endl;

  if (vret == 0) vret = -1.*fabs(h->GetBinError(bing1)); // upper limit is negative                                                                                     

  return vret;
}

double TrkLH::dotrklk(double ene, int ipat){
  // evaluate tracker likelihood for energy and pattern                                                                                                                 
  double tpr0 = 1;
  double tpr1 = 1;
  Double_t pr0 = 1;
  Double_t pr1 = 1;
  ntailc1 = 0;
  ntailc0 = 0;
  ntail1 = 0;
  ntail0 = 0;
  ntail2 = 0;
  for (int ivar = 0; ivar<VARN; ivar++){
    if (varmask[ivar]==0) continue;
    //    cout << varlist[ivar] << endl;                                                                                                                                
    if ((ivar >=9 && ivar<=15)||(ipat!=5 && (ivar == 3 || ivar == 5))){
      pr0 = bilinear(hlist0[ivar][ipat], varnorm[ivar],ene,0);  // bkg                                         
      pr1 = bilinear(hlist1[ivar][ipat], varnorm[ivar],ene,0);  // signal                                
    }  else {  // now forced to never interpolate
      pr0 = bilinear(hlist0[ivar][ipat], varnorm[ivar],ene,0);  // bkg                                       
      pr1 = bilinear(hlist1[ivar][ipat], varnorm[ivar],ene,0);  // signal                                                                                                 
    }
    if (pr0 < 0 && pr1< 0) { ntail2++; }  // both in tail                                  
    if (pr0 < 0 && pr1>=fabs(pr0)) {pr0 = fabs(pr0); ntailc0++; }  // limit pr0                                   
    if (pr0 < 0 && pr1<fabs(pr0)) {pr0 = 1; pr1 = 1; ntail0++; }  // not known                                
    if (pr1 < 0 && pr0>=fabs(pr1)) {pr1 = fabs(pr1); ntailc1++; }  // limit pr1                                  
    if (pr1 < 0 && pr0<fabs(pr1)) {pr0 = 1; pr1 = 1; ntail1++; }  // not known                                

    tpr0 = tpr0*pr0;
    tpr1 = tpr1*pr1;
    if (pr0 > 1.01) cout << ipat << " " << ivar << " error pdf0 " << varlist[ivar] << " ene = " << ene << " " << pr0 << endl;
    if (pr1 > 1.01) cout << ipat << " " << ivar << " error pdf1 " << varlist[ivar] << " ene = " << ene << " " << pr1 << endl;
    if (pr0 < 0) cout << ipat << " " << ivar << " error pdf0 " << varlist[ivar] << " ene = " << ene << " " << pr0 << endl;
    if (pr1 < 0) cout << ipat << " " << ivar << " error pdf1 " << varlist[ivar] << " ene = " << ene << " " << pr1 << endl;
  }
  if(tpr0 <= 0 && tpr1>0) return 1.;  // signal                                                                                                                         
  if(tpr1 <= 0 && tpr0>0) return 0.;  // bkg                                                                                                                            
  if(tpr1 <= 0 && tpr0<=0) return 0.5;  // unknown                                                                                                                      
  double lktrk = log10(tpr0)/(log10(tpr0)+log10(tpr1));
  return lktrk;
}



double TrkLH::normvar(TGraph* _low, TGraph* _upp, double var , double rigd){
  // low and upp are 1 sigma tail thresholds
  // the normalized variable is centered and 
  // the width is in number of sigma
  double norm;
  if (var <= -19.) return -19.; // underflow                                                                             
  if (var >= 19.) return 19.; // overflow                                                                                
  double lv = _low->Eval(rigd);
  double uv = _upp->Eval(rigd);
  double cnt = (lv+uv)/2.;
  double dlt = fabs(lv-uv)/2.;  // the sigma width                                                                       
  if (dlt == 0) dlt = 1;
  norm = (var-cnt)/dlt;  // number of sigma from center                                                                  
  return norm;
}

int TrkLH::replacemask(int *newmask){
  // user defined variable mask  (0 = variable not used)
  // to avoid use of some variables in trklk
  // referring to fillvar() for the meaning of variables
  for (int ivar = 0; ivar<VARN; ivar++){
    if (newmask[ivar]!=0 && newmask[ivar]!=1) return newmask[ivar];
    varmask[ivar]=newmask[ivar];
  }
  return 0; // correct return
}

#ifndef _NOGBATCH_
int TrkLH::filltrklhvarfromgbatch(TrkLHVar& var1, AMSEventR* evt, int nptc){
  // fill the TrkLHVar data structure from gbatch  
  // nptc is the pre-selected particle number  
  // return -2 = event problem
  // return -1 = particle problem
  // return 1 = ok                             
  // return 0 = trk problem                    
  // return 2*n = ecal problem                 
  // return 3*n = tof problem            
  int iret = 1;
  if (!evt) return -2;
  var1.ntrk = 1.*(evt->NTrTrack());
  var1.nanti = 1.*(evt->nAntiCluster());
  ParticleR *part = evt->pParticle(nptc);
  if (!part) return -1;
  //  printf("%p\n", part);//only for debug
  TrTrackR *trk = part->pTrTrack();
  if (!trk) return 0;
  var1.btp = trk->GetBitPatternJ();

  int idd = trk->iTrTrackPar(1, 0, 1);
  if( idd < 0) return 0;
  var1.rigidity_ch = trk->GetRigidity(idd);
  var1.chix_ch   = trk->GetNormChisqX(idd);
  var1.chiy_ch   = trk->GetNormChisqY(idd);
  var1.err_ch = 0.;
  int  iddc = trk->iTrTrackPar(21,0,1); // error same weight                                                                                                               
  if (iddc>=0) var1.err_ch = trk->GetErrRinv(iddc);
  var1.rigidity_al = 0;
  iddc = trk->iTrTrackPar(2,0,1);
  if (iddc>=0) var1.rigidity_al = trk->GetRigidity(iddc);
  var1.rigidity_tp = 0;
  iddc = trk->iTrTrackPar(1,5,1);
  if (iddc>=0) var1.rigidity_tp = trk->GetRigidity(iddc);
  var1.rigidity_bt = 0;
  iddc = trk->iTrTrackPar(1,6,1);
  if (iddc>=0) var1.rigidity_bt = trk->GetRigidity(iddc);
  var1.rigidity_itp = 0;
  iddc = trk->iTrTrackPar(1,1,1);
  if (iddc>=0) var1.rigidity_itp = trk->GetRigidity(iddc);
  var1.rigidity_ibt = 0;
  iddc = trk->iTrTrackPar(1,2,1);
  if (iddc>=0) var1.rigidity_ibt = trk->GetRigidity(iddc);
  var1.rigidity_in = 0;
  iddc = trk->iTrTrackPar(1,3,1);
  if (iddc>=0) var1.rigidity_in = trk->GetRigidity(iddc);

  // search nearest noise hit for each layer
  int trk_layer_pattern[9] = {0,0,0,0,0,0,0,0,0};
  Double_t noise_y[9] = {1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9};
  int laypa = 1;
  for (int layer=1; layer<=9; layer++) {
    int bitexam = int(pow(2.,layer-1));
    bitexam = (bitexam & var1.btp);
    if (bitexam>0)    trk_layer_pattern[layer-1]  = 1;  // flags hitted layers
    AMSPoint pnoise = TrTrackSelection::GetMinDist(trk, laypa, 1);
    noise_y[layer-1]=fabs(pnoise.y());
    laypa=laypa*10;
  }
  var1.nlayer = 0.;
  var1.minoise1 = 7.;
  int lminoise1 = -1;
  for (int layer=1; layer<=9; layer++) {
    if(trk_layer_pattern[layer-1] ==1) var1.nlayer=var1.nlayer+1.;  // found number hitted layer
    if (noise_y[layer-1]<var1.minoise1 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1)
      {var1.minoise1 = noise_y[layer-1]; lminoise1=layer;}}
  var1.minoise2 = 7.;
  int lminoise2 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<var1.minoise2 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1)
      {var1.minoise2 = noise_y[layer-1]; lminoise2=layer;}}
  var1.minoise3=7.;
  int lminoise3 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<var1.minoise3 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2)
      {var1.minoise3 = noise_y[layer-1]; lminoise3=layer;}}
  var1.minoise4=7.;
  int lminoise4 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<var1.minoise4 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2 && layer != lminoise3)
      {var1.minoise4 = noise_y[layer-1]; lminoise4=layer;}}

  // ECAL RELATED

  EcalShowerR *ecal = part->pEcalShower();
  if (!ecal) {iret = iret*2;}
  else {
    var1.energy = ecal->EnergyE;
    AMSPoint ecalp9;  // top of ecal

    AMSPoint ecalp0; // bottom of ecal but after is used for cofg 

    // put in correct way top-bottom (sometime is flipped) 
    Double_t dzn = (ecal->Entry[2]);
    Double_t dzx = (ecal->Exit[2]);
    if (dzn*dzn < dzx*dzx)
      {// standard direction
	ecalp9.setp(ecal->Entry[0], ecal->Entry[1],ecal->Entry[2]);
        ecalp0.setp(ecal->Exit[0], ecal->Exit[1],ecal->Exit[2]);
      } else
      {// contrary direction
        ecalp0.setp(ecal->Entry[0], ecal->Entry[1],ecal->Entry[2]);
        ecalp9.setp(ecal->Exit[0], ecal->Exit[1],ecal->Exit[2]);}
    ecalp0.setp(ecal->CofG[0], ecal->CofG[1],ecal->CofG[2]); //cofg 
    // Ecal prediction from trk fit
    AMSPoint Pec9;
    AMSPoint Pec0;
    AMSDir dird;
    trk->Interpolate(ecalp0.z(), Pec0, dird, idd);
    trk->Interpolate(ecalp9.z(), Pec9, dird, idd);
    var1.ecal_ecogx = ecalp0.x();
    var1.ecal_ecogy = ecalp0.y();
    var1.trk_ecogx = Pec0.x();
    var1.trk_ecogy = Pec0.y();
    var1.cosy = fabs(dird.y());      // not really ecal related
    double ecp9x = ecalp9.x();
    double ecp9y = ecalp9.y();
    double ecp9z = ecalp9.z();
    double ecp0x = ecalp0.x();
    double ecp0y = ecalp0.y();
    double ecp0z = ecalp0.z();
    double tecdx = dird.x();
    double tecdy = dird.y();
    double tecdz = dird.z();

    double norm1 = (ecp9x-ecp0x)*(ecp9x-ecp0x)+(ecp9y-ecp0y)*(ecp9y-ecp0y)+(ecp9z-ecp0z)*(ecp9z-ecp0z);
    norm1 = sqrt(norm1);
    double norm2 = sqrt(tecdx*tecdx+tecdy*tecdy+tecdz*tecdz);
    double coset = (ecp9x-ecp0x)*tecdx+(ecp9y-ecp0y)*tecdy+(ecp9z-ecp0z)*tecdz;
    coset = fabs(coset)/(norm1*norm2);
    coset = fabs(1.-coset*coset);    // is sinsquare trk-ecal
    var1.sinq_trk_ecal = coset;
  }

  var1.sinq_trk_trd = 0;   // to be done and understood MC reliability of TRD track

  double beta = 1;
  BetaR* tof = part->pBeta();
  if (!tof) {iret = iret*3;}
  else {
    beta = tof->Beta;
    TofTrack* tof_track = new TofTrack(tof,trk,idd);
    var1.tof_qu = tof_track->GetChargePlane(TofTrack::kUpper);
    var1.tof_qd = tof_track->GetChargePlane(TofTrack::kLower);
  }

  var1.trkqin = trk->GetInnerQ(beta,1);
  var1.trkqrms = trk->GetQ_RMS(beta,1);

  return iret;
}
#endif

void TrkLH::SetDefaultMask(){
  varmask[0]=1;
  varmask[1]=1;
  varmask[2]=0;
  varmask[3]=1;
  varmask[4]=1;
  varmask[5]=1;
  varmask[6]=1;
  varmask[7]=1;
  varmask[8]=1;
  varmask[9]=1;
  varmask[10]=1;
  varmask[11]=1;
  varmask[12]=1;
  varmask[13]=0;
  varmask[14]=1;
  varmask[15]=1;
  varmask[16]=0;
  varmask[17]=1;
  varmask[18]=1;
  varmask[19]=1;
  varmask[20]=1;
  varmask[21]=1;
  varmask[22]=0;
}

#endif

//----------------------------------------------------

#ifndef __TRKLHVAR_CXX__
#define __TRKLHVAR_CXX__

#include <stdlib.h>

#include "TrkLH.h"

using namespace std;

ClassImp(TrkLHVar)

TrkLHVar::TrkLHVar(){
  init();
}

TrkLHVar::~TrkLHVar(){
}

void TrkLHVar::init(){
  btp=-99999999;   
  ly1=-99999999;   
  ly2=-99999999;   
  ly9=-99999999;   
  pat=-99999999;   
  energy=-99999999;     
  rigidity_ch=-99999999;
  rigidity_al=-99999999;
  chix_ch=-99999999;   
  chiy_ch=-99999999;   
  err_ch=-99999999;   
  rigidity_in=-99999999;
  rigidity_bt=-99999999;
  rigidity_tp=-99999999;
  rigidity_ibt=-99999999;
  rigidity_itp=-99999999;
  trkqin=-99999999;  
  trkqrms=-99999999; 
  minoise1=-99999999;
  minoise2=-99999999;
  minoise3=-99999999;  
  minoise4=-99999999;  
  cosy=-99999999; 
  nlayer=-99999999; 
  tof_qd=-99999999; 
  tof_qu=-99999999; 
  sinq_trk_ecal=-99999999;
  sinq_trk_trd=-99999999;
  ecal_ecogx=-99999999; 
  ecal_ecogy=-99999999; 
  trk_ecogx=-99999999; 
  trk_ecogy=-99999999; 
  ntrk=-99999999; 
  nanti=-99999999;    
}

#endif
