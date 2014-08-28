//  $Id$

#include "TROOT.h"
#include "TRegexp.h"
#include "root.h"
#include "ntuple.h"
#include <TRandom.h>
#include <TRandom3.h>
#include "TSystem.h"
#include <TChainElement.h>
#include "TFile.h"
#include "TH3.h"
#include "TGraph.h"
#include "TMinuit.h"
#include "TEnv.h"
#ifdef _OPENMP
#include <omp.h>
#endif
#ifdef _PGTRACK_
#include "TrExtAlignDB.h"
#include "TrInnerDzDB.h"
#endif
#include "timeid.h"
#include "commonsi.h"
#include "RichCharge.h"
#include "RichBeta.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "antirec02.h"
#include "beta.h"
#include "ecalrec.h"
#include "mceventg.h"
#ifdef _PGTRACK_
//#include "TrRecon.h"
#else
#include "trmccluster.h"
#include "trrawcluster.h"
#endif
#include "mccluster.h"
#include "particle.h"
#include "richrec.h"
#include "tofdbc02.h"
#include "tofsim02.h"
#include "tofrec02.h"
#include "ecaldbc.h"
#include "antirec02.h"
#include "trdrec.h"
#include "trdhrec.h"
#include "trigger102.h"
#include "trigger302.h"
#include "trrec.h"
#include "charge.h"
#include "tofhit.h"
#ifndef _PGTRACK_
#include "vtx.h"
#else 
#endif
#include "astro.h"
#include "amsdbc.h"
#ifdef __DARWIN__
#include <limits.h>
#else
#include <values.h>
#include <float.h>
#endif
#endif
#include "Sunposition.h"
#include "FrameTrans.h"
#include "GeoMagField.h"
#include "GeoMagTrace.h"
#include "HistoMan.h"
#include "Tofrec02_ihep.h"
#include "GM_SubLibrary.h"
#include "BackTracingMIB.h"
#include "MagField.h"
#include "tkdcards.h"



using namespace root;
#ifdef __WRITEROOT__
/*
  ClassImp(HeaderR)
  ClassImp(EcalHitR)
  ClassImp(EcalClusterR)
  ClassImp(Ecal2DClusterR)
  ClassImp(EcalShowerR)
  ClassImp(RichHitR)
  ClassImp(RichRingR)
  ClassImp(RichRingBSegmentR)
  ClassImp(RichRingBR)
  ClassImp(TofRawClusterR)
  ClassImp(TofRawSideR)
  ClassImp(TofClusterR)
  ClassImp(TofClusterHR)
  ClassImp(AntiRawSideR)
  ClassImp(AntiClusterR)
  ClassImp(TrRawClusterR)
  ClassImp(TrClusterR)
  ClassImp(TrRecHitR)
  ClassImp(TrTrackR)
  ClassImp(TrdRawHitR)
  ClassImp(TrdClusterR)
  ClassImp(TrdSegmentR)
  ClassImp(TrdHSegmentR)
  ClassImp(TrdTrackR)
  ClassImp(TrdHTrackR)
  ClassImp(Level1R)
  ClassImp(Level3R)
  ClassImp(BetaR)
  ClassImp(BetaHR)
  ClassImp(ChargeR)
  ClassImp(ParticleR)
  ClassImp(AntiMCClusterR)
  ClassImp(TrMCClusterR)
  ClassImp(TofMCClusterR)
  ClassImp(TofMCPmtHitR)
  ClassImp(EcalMCHitR)
  ClassImp(TrdMCClusterR)
  ClassImp(RichMCClusterR)
  ClassImp(MCTrackR)
  ClassImp(MCEventgR)
  ClassImp(DaqEventR)
*/
ClassImp(AMSEventR)


//------------------- constructors -----------------------

AMSEventR::Service::hb1_d AMSEventR::Service::hb1;
AMSEventR::Service::hb2_d AMSEventR::Service::hb2;
AMSEventR::Service::hbp_d AMSEventR::Service::hbp;
//char AMSEventR::Service::Dir[]="";

TGraph *SSAedge = NULL;

bool AMSEventR::fgThickMemory=false;
TFile* AMSEventR::fgOutSep[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
TDirectory* AMSEventR::fgOutSepDir[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
#ifndef __ROOTSHAREDLIBRARY__
int AMSEventR::fgSeparateOutputFile=0;
#else
int AMSEventR::fgSeparateOutputFile=2;
#endif
int AMSEventR::fgThreads=1;
TString AMSEventR::Dir="";
TString AMSEventR::gWDir="";
void AMSEventR::hcopy(char dir[],int id1,int id2){
  char save[1024];
  strcpy(save,Dir.Data());
  hcdir(dir);
  hcopy(id1,id2); 
}

void AMSEventR::hcopy(int id1,int id2){
  TH1D *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
     float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
if(h1p->GetXaxis()->GetXbins()->GetArray()){
 hbook1(id2,title,n,(double*)h1p->GetXaxis()->GetXbins()->GetArray());
}
else{
    hbook1(id2,title,n,a,b);
}
    TH1D *h2p = h1(id2);
    if(h2p){
      for (int i=0;i<n+2;i++){
	h2p->SetBinContent(i,h1p->GetBinContent(i));
	h2p->SetBinError(i,h1p->GetBinError(i));
      }
      h2p->SetEntries(h1p->GetEntries());
      return ;
    }
  }
  else{
    TH2D *h2p = h2(id1);
    if(h2p){
    float ax=h2p->GetXaxis()->GetBinLowEdge(1); 
    float ay=h2p->GetYaxis()->GetBinLowEdge(1); 
    int nx=h2p->GetNbinsX();
    int ny=h2p->GetNbinsY();
    float bx=h2p->GetXaxis()->GetBinLowEdge(nx)+h2p->GetXaxis()->GetBinWidth(nx); 
    float by=h2p->GetYaxis()->GetBinLowEdge(ny)+h2p->GetYaxis()->GetBinWidth(ny); 
    const char *title=h2p->GetTitle();
    hbook2(id2,title,nx,ax,bx,ny,ay,by);
    TH2D *h3p = h2(id2);
    if(h3p){
      for (int i=0;i<nx+2;i++){
       for (int j=0;j<ny+2;j++){
	h3p->SetBinContent(i,j,h2p->GetBinContent(i,j));
	h3p->SetBinError(i,j,h2p->GetBinError(i,j));
       }       
      }
      h3p->SetEntries(h2p->GetEntries());
      return ;
    }

    }
    else{
      TProfile *hpp=hp(id1);
      if(hpp){
      }
    }
  }
}
void AMSEventR::hdivide(int id1,int id2,int id3){
{

  TH1D *h2p = h1(id2);
  if(h2p){
    //   h2p->Sumw2();
    if(id1!=id3){
       if(id2!=id3)hcopy(id1,id3);
       else{
        hcopy(id1,INT_MAX-1);
        id3=INT_MAX-1;
       }
    }
    TH1D *h1p = h1(id3);
    if(h1p){
      //    h1p->Sumw2();
      h1p->Divide(h2p);
      if(id3==INT_MAX-1){
        hcopy(id3,id2);
        hdelete(id3); 
      }
      return; 
   }   
  }
}


{
  TH2D *h2p = h2(id2);
  if(h2p){
    //   h2p->Sumw2();
    if(id1!=id3){
       if(id2!=id3)hcopy(id1,id3);
       else{
        hcopy(id1,INT_MAX-1);
        id3=INT_MAX-1;
       }
    }
    TH2D *h1p = h2(id3);
    if(h1p){
      //    h1p->Sumw2();
      h1p->Divide(h2p);
      if(id3==INT_MAX-1){
        hcopy(id3,id2);
        hdelete(id3); 
      }
      return; 
   }   
  }
}
}

bool AMSEventR::IsInSAA(unsigned int time){
/*
 *
 *  Created by William GILLARD on 03/03/2014.
 *  Determine either the AMS was inside the SAA boundary at a given time. The default value for the time corresponds to the curent event's date.
 *  Retrun true if in the SAA boundary, false otherwize.
 */

	AMSSetupR::RTI a;

	if(time == 0)
//		GetRTI(a, fHeader.Time[0]);
                GetRTI(a);
	else
		GetRTI(a, time);
        
	return a.IsInSAA();
}

void AMSEventR::hscale(int id1,double fac,bool calcsumw2){
{
  TH1D *h2p = h1(id1);
  if(h2p){
    if(calcsumw2)h2p->Sumw2();
    h2p->Scale(fac);
    return;
  }
}
{
  TH2D *h2p = h2(id1);
  if(h2p){
    if(calcsumw2)h2p->Sumw2();
    h2p->Scale(fac);
    return;
  }
}
}
void AMSEventR::hsub(int id1,int id2,int id3){
    if(id1!=id3){
      if(id2!=id3)hcopy(id2,id3);
{
  TH1D *h2p = h1(id1);
  if(h2p){
    h2p->Sumw2();
    TH1D *h1p = h1(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
      return;
    }   
  }
}
{
  TH2D *h2p = h2(id1);
  if(h2p){
    h2p->Sumw2();
    TH2D *h1p = h2(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
      return;
    }   
  }
}
}
else{

{
  TH1D *h2p = h1(id2);
  if(h2p){
    h2p->Sumw2();
    TH1D *h1p = h1(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
      return;
    }   
  }
}
{
  TH2D *h2p = h2(id1);
  if(h2p){
    h2p->Sumw2();
    TH2D *h1p = h2(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
      return;
    }   
  }
}



}
}
void AMSEventR::hadd(int id1,int id2,int id3){
    if(id1!=id3){
      if(id2!=id3)hcopy(id2,id3);
{
  TH1D *h2p = h1(id1);
  if(h2p){
//    h2p->Sumw2();
    TH1D *h1p = h1(id3);
    if(h1p){
//      h1p->Sumw2();
      h1p->Add(h2p,1);
      return;
    }   
  }
}
{
  TH2D *h2p = h2(id1);
  if(h2p){
//    h2p->Sumw2();
    TH2D *h1p = h2(id3);
    if(h1p){
//      h1p->Sumw2();
      h1p->Add(h2p,1);
      return;
    }   
  }
}
}
else{

{
  TH1D *h2p = h1(id2);
  if(h2p){
//    h2p->Sumw2();
    TH1D *h1p = h1(id3);
    if(h1p){
//      h1p->Sumw2();
      h1p->Add(h2p,1);
      return;
    }   
  }
}
{
  TH2D *h2p = h2(id1);
  if(h2p){
 //   h2p->Sumw2();
    TH2D *h1p = h2(id3);
    if(h1p){
 //     h1p->Sumw2();
      h1p->Add(h2p,1);
      return;
    }   
  }
}



}
}
void AMSEventR::hbook1s(int id,const char title[], int ncha, float  a, float b,int howmany,int shift){
  for (int i=0;i<howmany;i++){
    hbook1(id+shift*i,title,ncha,a,b);
  }
}
void AMSEventR::hbook1s(int id,const char title[], int ncha, double xbin[],int howmany,int shift){
  for (int i=0;i<howmany;i++){
    hbook1(id+shift*i,title,ncha,xbin);
  }
}

void AMSEventR::hbook1(int idd,const char title[], int ncha, float  a, float b){ 
  AMSID id(idd,Dir);
  if(Service::hb1.find(id) != Service::hb1.end()){
    delete Service::hb1.find(id)->second;
    Service::hb1.erase((Service::hb1.find(id)));
    cerr<<"  AMSEventR::hbook1-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
  }
  char hid[1025];
  sprintf(hid,"hb1_%d_%s_%s",idd,title,Dir.Data()); 
  TH1D * p= new TH1D(hid,title,ncha,a,b);
  (Service::hb1).insert(make_pair(id,p));
}

void AMSEventR::hbook1(int idd,const char title[], int ncha, double xbin[]){ 
  AMSID id(idd,Dir);
  if(Service::hb1.find(id) != Service::hb1.end()){
    delete Service::hb1.find(id)->second;
    Service::hb1.erase((Service::hb1.find(id)));
    cerr<<"  AMSEventR::hbook1-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
  }
  char hid[1025];
  sprintf(hid,"hb1_%d_%s_%s",idd,title,Dir.Data()); 
  TH1D * p= new TH1D(hid,title,ncha,xbin);
  (Service::hb1).insert(make_pair(id,p));
}

void AMSEventR::hbookp(int idd,const char title[], int ncha, float  a, float b){
  AMSID id(idd,Dir);
  if(Service::hbp.find(id) == Service::hbp.end()){
    char hid[1025];
    sprintf(hid,"hbp_%d_%s_%s",idd,title,Dir.Data()); 
    TProfile * p= new TProfile(hid,title,ncha,a,b);
    Service::hbp.insert(make_pair(id,p));
  }
  else{
    cerr<<"  AMSEventR::hbookp-S-Histogram "<<id<<" AlreadyExistsResetting "<<endl;
    Service::hbp.find(id)->second->Reset();
  }
}

void AMSEventR::hbook2s(int id, const char title[], int ncha, float  a, float b, int nchaa, float aa, float ba,int howmany,int shift){
  for (int i=0;i<howmany;i++){
    hbook2(id+shift*i,title,ncha,a,b,nchaa,aa,ba);
  }
}
void AMSEventR::hbook2s(int id, const char title[], int ncha, float  a, float b, int nchaa, double *ba,int howmany,int shift){
  for (int i=0;i<howmany;i++){
    hbook2(id+shift*i,title,ncha,a,b,nchaa,ba);
  }
}

void AMSEventR::hbook2(int idd,const char title[], int ncha, float  a, float b, int nchaa, float aa, float ba){
  AMSID id(idd,Dir);
#pragma omp critical (hf2)
  if(Service::hb2.find(id) != Service::hb2.end()){
    delete Service::hb2.find(id)->second;
    Service::hb2.erase((Service::hb2.find(id)));
    cerr<<"  AMSEventR::hbook2-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
  }
  char hid[1025];
  sprintf(hid,"hb2_%d_%s_%s",idd,title,Dir.Data()); 
  TH2D * p= new TH2D(hid,title,ncha,a,b,nchaa,aa,ba);
#pragma omp critical (hf2)
  Service::hb2.insert(make_pair(id,p));
}


void AMSEventR::hbook2(int idd,const char title[], int ncha, float  a, float b, int nchaa, double ybin[]){
  AMSID id(idd,Dir);
#pragma omp critical (hf2)
  if(Service::hb2.find(id) != Service::hb2.end()){
    delete Service::hb2.find(id)->second;
    Service::hb2.erase((Service::hb2.find(id)));
    cerr<<"  AMSEventR::hbook2-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
  }
  char hid[1025];
  sprintf(hid,"hb2_%d_%s_%s",idd,title,Dir.Data()); 
  TH2D * p= new TH2D(hid,title,ncha,a,b,nchaa,ybin);
#pragma omp critical (hf2)
  Service::hb2.insert(make_pair(id,p));
}



void AMSEventR::hfill(int idd, float a, float b=0, float w=1){
  AMSID id(idd,Dir);
  Service::hb1i i1=Service::hb1.find(id);
  if(i1 != Service::hb1.end()){
    hf1(idd,a,w);
  }
  else{
    Service::hb2i i2=Service::hb2.find(id);
    if(i2 != Service::hb2.end()){
      hf2(idd,a,b,w);
    }
    else{
      Service::hbpi ip=Service::hbp.find(id);
      if(ip != Service::hbp.end()){
	hfp(idd,a,w);
      }
    }
  }
}

void AMSEventR::hfit1(int id, char func[],double xmin, double xmax){
  AMSID idd(id,Dir);
  char fit[255];
  Service::hb1i i1=Service::hb1.find(idd);
  if(i1 != Service::hb1.end()){
    if(func[0] == 'g')strcpy(fit,"gaus");
    else if(func[0] =='e')strcpy(fit,"expo");
    else if(func[0]=='p'){
      strcpy(fit,"pol");
      strcat(fit,func+1);
    }
    else strcpy(fit,func);
    i1->second->Fit(fit,"" ,"",xmin,xmax);
  }
}

void AMSEventR::hprint(int idd, const char opt[]){
  AMSID id(idd,Dir);
  Service::hb1i i1=Service::hb1.find(id);
  if(i1 != Service::hb1.end()){
    i1->second->Draw(opt);
  }
  else{
    Service::hb2i i2=Service::hb2.find(id);
    if(i2 != Service::hb2.end()){
      i2->second->Draw(opt);
    }
    else{
      Service::hbpi ip=Service::hbp.find(id);
      if(ip != Service::hbp.end()){
	ip->second->Draw(opt);
      }
    }
  }
}

bool AMSEventR::Status(unsigned int group, unsigned int bitgroup){
  return true;
}
bool AMSEventR::Status(unsigned int bit){
  if(bit<64)return (fStatus & (1<<bit));
  else return false;
}
void AMSEventR::hlist(const char ptit[]){
  for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
    if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" 1D "<<i->first<<" "<<i->second->GetTitle()<<endl;
  }
  for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
    if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" 2D "<<i->first<<" "<<i->second->GetTitle()<<endl;
  }
  for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
    if(strlen(ptit)<1 || strstr(i->second->GetTitle(),ptit))cout<<" Profile "<<i->first<<" "<<i->second->GetTitle()<<endl;
  }
}


void AMSEventR::hdelete(int idd){
  if(idd ==0){
#pragma omp critical (hf1)
    for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
      delete i->second;
    }
    Service::hb1.clear();
#pragma omp critical (hf2)
    for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
      delete i->second;
    }
    Service::hb2.clear();
    for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
      delete i->second;
    }
    Service::hbp.clear();

  }
  else{
    AMSID id(idd,Dir);
    Service::hb1i i1=Service::hb1.find(id);
    if(i1 != Service::hb1.end()){
      delete i1->second;
      Service::hb1.erase(i1);
    }
    else{
      Service::hb2i i2=Service::hb2.find(id);
      if(i2 != Service::hb2.end()){
	delete i2->second;
	Service::hb2.erase(i2);

      }
      else{
	Service::hbpi ip=Service::hbp.find(id);
	if(ip != Service::hbp.end()){
	  delete ip->second;
	  Service::hbp.erase(ip);

	}
      }
    }

  }
}
void AMSEventR::chdir(const char dir[]){
  //strcpy(Dir,dir);
  Dir=dir;
}

void AMSEventR::hcdir(const char dir[]){
  //strcpy(Dir,dir);
  Dir=dir;
}

void AMSEventR::hfetch( const char file[],int id){
  TFile *f= new TFile(file);
  hfetch(*f,file,id,NULL);
  hcdir(file);
  // f->Close();
  // delete f;
}
void AMSEventR::hfetch( const char file[],const char pat[]){
  TFile *f= new TFile(file);
  hfetch(*f,file,0,pat);
  hcdir(file);
  // f->Close();
  // delete f;
}

#include <TKey.h>


void AMSEventR::hfetch(TFile &f, const char dir[],int idh, const char pat[]){
  int fetch1=0;
  int fetch2=0;
  int fetchp=0;
  TIter nextkey(f.GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextkey())) {
    if(pat &&!strstr(key->GetName(),pat)){
       continue;
    }
    TObject *to=(key->ReadObj());
    TH1D * f1 = dynamic_cast<TH1D*>(to);
    TProfile * f1p = dynamic_cast<TProfile*>(to);
    if(f1 &&!f1p){
      TString t(f1->GetName());
      if(t.BeginsWith("hb1")){
        for(unsigned int i=4;i<strlen(f1->GetName());i++){
          if(i>4 && f1->GetName()[i] =='_'){
	    TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
              int idd=st.Atoi(); 
              AMSID id(idd,dir);
              if(!idh || idh==idd){
              fetch1++;
              if(Service::hb1.find(id) == Service::hb1.end()){
              }
              else{
		cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		Service::hb1.erase((Service::hb1.find(id)));
              }
              (Service::hb1).insert(make_pair(id,f1));
              }
              break;
	    }
	    else cerr<<"TH1D "<<t<<" IdNotDigitalSkipped"<<endl;
	  }
        }
      }
      else if(t!=TString(""))cerr<<"TH1D "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
    }
    else{
      TH2D * f1 = dynamic_cast<TH2D*>(to);
      if(f1){
	TString t(f1->GetName());
	if(t.BeginsWith("hb2")){
	  for(unsigned int i=4;i<strlen(f1->GetName());i++){
	    if(i>4 && f1->GetName()[i] =='_'){
	      TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
		int idd=st.Atoi(); 
		AMSID id(idd,dir);
		if(!idh || idh==idd){
                fetch2++;
		if(Service::hb2.find(id) == Service::hb2.end()){
		}
		else{
		  cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		  Service::hb2.erase((Service::hb2.find(id)));
		}
	(Service::hb2).insert(make_pair(id,f1));
               }
		break;
	      }
	      else cerr<<"TH2D "<<t<<" IdNotDigitalSkipped"<<endl;
	    }
	  }
	}
	else if(t!=TString(""))cerr<<"TH2D "<<t<<" NotCreatedByHBOOK2Skipped"<<endl;
      }
      else{
	TProfile * f1 = dynamic_cast<TProfile*>(to);
	if(f1){
	  TString t(f1->GetName());
	  if(t.BeginsWith("hbp")){
	    for(unsigned int i=4;i<strlen(f1->GetName());i++){
	      if(i>4 && f1->GetName()[i] =='_'){
		TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
		  int idd=st.Atoi(); 
		  AMSID id(idd,dir);
		  if(!idh || idh==idd){
                  fetchp++;
		  if(Service::hbp.find(id) == Service::hbp.end()){
		  }
		  else{
		    cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		    Service::hbp.erase((Service::hbp.find(id)));
		  }
		  if(!idh || idh==idd)(Service::hbp).insert(make_pair(id,f1));
                 }
		  break;
		}
		else cerr<<"TProfile "<<t<<" IdNotDigitalSkipped"<<endl;
	      }
	    }
	  }
	  else  if(t!=TString(""))cerr<<"TProfile "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
	}


      }


    }
  } 
  if(fetch1>0)cout <<fetch1<<" 1D Histos fetched"<<endl;
  if(fetch2>0)cout <<fetch2<<" 2D Histos fetched"<<endl;
  if(fetchp>0)cout <<fetchp<<" Profile Histos fetched"<<endl;
  //    chdir(dir);
}
TH1D * AMSEventR::h1(int idd){
  //
  // must be used with care as may be changed after map update operation
  //
  AMSID id(idd,Dir);
  Service::hb1i i1=Service::hb1.find(id);
  if(i1 != Service::hb1.end())return i1->second;
  else return 0; 
}

TH2D * AMSEventR::h2(int idd){
  AMSID id(idd,Dir);
  Service::hb2i i1=Service::hb2.find(id);
  if(i1 != Service::hb2.end())return i1->second;
  else return 0; 
}


TProfile * AMSEventR::hp(int idd){
  AMSID id(idd,Dir);
  Service::hbpi i1=Service::hbp.find(id);
  if(i1 != Service::hbp.end())return i1->second;
  else return 0; 
}

void AMSEventR::hreset(int idd){
  if(idd ==0){
    for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++)i->second->Reset();
    for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++)i->second->Reset();
    for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++)i->second->Reset();
  }
  else{
    AMSID id(idd,Dir);
    Service::hb1i i1=Service::hb1.find(id);
    if(i1 != Service::hb1.end()){
      i1->second->Reset();
    }
    else{
      Service::hb2i i2=Service::hb2.find(id);
      if(i2 != Service::hb2.end()){
	i2->second->Reset();
      }
      else{
	Service::hbpi ip=Service::hbp.find(id);
	if(ip != Service::hbp.end()){
	  ip->second->Reset();
	}
      }
    }
  }
}

void AMSEventR::hf2s(int id, float a, float b,bool cuts[], int ncuts, int icut,int shift,float w){
  //  Fill the series of histos 
  //  1st before cuts
  //  before cut icut-1
  //  before icut-1 as last cut
  //  after icut-1 as last cut
  //  after last cut as first cut : changed  after !icut-1 as last cut
  //  after cut icut-1 as first cut
  hf2(id,a,b,w);
  bool cut=true;
  if(icut-1>0)for(int k=0;k<icut-1;k++)cut=cut && cuts[k];
  if(cut)hf2(id+shift,a,b,w);
  for(int k=icut;k<ncuts;k++)cut=cut && cuts[k];
  if(cut)hf2(id+shift+shift,a,b,w);
  if(cut && cuts[icut-1])hf2(id+shift+shift+shift,a,b,w);             
  if(cut && !cuts[icut-1])hf2(id+shift+shift+shift+shift,a,b,w);             
//  if(cuts[ncuts-1])hf1(id+shift+shift+shift+shift,a,w);             
  if(icut-1>=0 && cuts[icut-1])hf2(id+shift+shift+shift+shift+shift,a,b,w);             
}
void AMSEventR::hf1s(int id, float a, bool cuts[], int ncuts, int icut,int shift,float w){
  //  Fill the series of histos 
  //  1st before cuts
  //  before cut icut-1
  //  before icut-1 as last cut
  //  after icut-1 as last cut
  //  after last cut as first cut : changed  after !icut-1 as last cut
  //  after cut icut-1 
  hf1(id,a,w);
  bool cut=true;
  if(icut-1>0)for(int k=0;k<icut-1;k++)cut=cut && cuts[k];
  if(cut)hf1(id+shift,a,w);
  if(cut && icut-1>=0 && cuts[icut-1])hf1(id+shift+shift+shift+shift+shift,a,w);
  for(int k=icut;k<ncuts;k++)cut=cut && cuts[k];
  if(cut)hf1(id+shift+shift,a,w);
  if(cut && cuts[icut-1])hf1(id+shift+shift+shift,a,w);             
  if(cut && !cuts[icut-1])hf1(id+shift+shift+shift+shift,a,w);             
//  if(cuts[ncuts-1])hf1(id+shift+shift+shift+shift,a,w);             
//  if(icut-1>=0 && cuts[icut-1])hf1(id+shift+shift+shift+shift+shift,a,w);             
}

void AMSEventR::hf1(int idd, float a, float w){
   
  AMSID id(idd,Dir);
  Service::hb1i i1=Service::hb1.find(id);
  if(i1 != Service::hb1.end()){
    if(fgThickMemory)i1->second->Fill(a,w);
    else{

      /*
      // complex method

      int thr=0;
      int nthr=1;
      #ifdef _OPENMP
      thr=omp_get_thread_num();
      nthr=omp_get_num_threads();
      #endif
      #pragma omp critical (hf1)
      {
      for(int i=0;i<nthr;i++){
      while(idd && sem[i]==idd){}
      }
      sem[thr]=idd;
      }
      i1->second->Fill(a,w);
      sem[thr]=0;


      */
      ///*
      // simple method

//#pragma omp critical (hf1)
      i1->second->Fill(a,w);
      //*/
    }
  }
  else{
#pragma omp critical (cout)
{ 
static int ic=0;
if(ic++<10)cout << "id not found "<<id<<endl;
}
}
}

void AMSEventR::hfp(int idd, float a, float w=1){
  AMSID id(idd,Dir);
  Service::hbpi i1=Service::hbp.find(id);
  if(i1 != Service::hbp.end()){
    if(fgThickMemory)i1->second->Fill(a,w);
    else{
//#pragma omp critical (hfp)
      i1->second->Fill(a,w);
    }
  }
}


void AMSEventR::hf2(int idd, float a, float b, float w=1){
  AMSID id(idd,Dir);
  Service::hb2i i1=Service::hb2.find(id);
  if(i1 != Service::hb2.end()){
    if(fgThickMemory)i1->second->Fill(a,b,w);
    else{
      /*
      // complex method

      int thr=0;
      int nthr=1;
      #ifdef _OPENMP
      thr=omp_get_thread_num();
      nthr=omp_get_num_threads();
      #endif
      #pragma omp critical (hf2)
      {
      for(int i=0;i<nthr;i++){
      while(idd && sem[i]==idd){}
      }
      sem[thr]=idd;
      }
      i1->second->Fill(a,b,w);
      sem[thr]=0;

      */

      ///*
      // simple method
#pragma omp critical (hf2)
      i1->second->Fill(a,b,w);

      //*/

    }
  }
}

void AMSEventR::hjoin(){
  int joined=0;
  for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
    int id=i->first.getid();	
    TH1D *h1p = i->second;
    if(h1p){
      h1p->Sumw2();
      for(Service::hb1i j=i;j!=Service::hb1.end();j++){
	if(i!=j && id==j->first.getid()){
	  TH1D* h2p=  j->second;
	  if(h2p){
	    h1p->Add(h2p,1);
	    j->second=0;
	  }
	  joined++;
	}
      }
    }
  }

  cout <<"AMSEventR::hjoin-"<<joined<<" 1d Histograms had been joined "<<endl;
  joined=0;


  for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
    int id=i->first.getid();
    TH2D *h1p = i->second;
    if(h1p){
      //   h1p->Sumw2();
      for(Service::hb2i j=i;j!=Service::hb2.end();j++){
	if(i!=j && id==j->first.getid()){
	  TH2D* h2p=  j->second;
	  if(h2p){
	    h1p->Add(h2p,1);
	    joined++;
	    j->second=0;
	  }
	}
      }
    }
  }



  cout <<"AMSEventR::hjoin-"<<joined<<" 2d Histograms had been joined "<<endl;
  joined=0;

  for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
    int id=i->first.getid();
    TProfile *h1p = i->second;
    h1p->Sumw2();
    for(Service::hbpi j=Service::hbp.begin();j!=Service::hbp.end();j++){
      if(i!=j && id==j->first.getid()){
	TProfile* h2p=  j->second;
	if(h2p)h2p->Sumw2();
	if(h2p)h1p->Add(h2p,1);
	delete h2p;
	joined++;
	Service::hbp.erase(j);

      }
    }
  }


  cout<<"AMSEventR::hjoin-"<<joined<<" Profile Histograms had been joined "<<endl;
  joined=0;


}
//------------------------------- Other public functions for root files analysis:
bool AMSEventR::GetEcalTriggerFlags(float Thr_factor[],float angle_factor[],int fastalgo,bool MCflag,bool debug,int flagtype){
// *************************************************************************
// Authors: S. Di Falco, S. Rosier
// Date:    March 14, 2011
//
// Description: function to evaluate the ECAL trigger flags
//
// Inputs: pev          Pointer to the AMSEventR object in the root file  
//         Thr_factor[6]   Multiplicative factors w.r.t. default thresholds 
//                         in mV of the 6 ECAL trigger superlayers: 
//                         ThresmV[6]={26.7,60.,46.7,20.,33.,33.}
//         angle_factor[2] Multiplicative factor w.r.t. default max distance 
//                         in PMT*64 of the PMT average position in adiacent 
//                         superlayers: if the number of hits in the last 2 
//                         superlayers of the view (X or Y) is less than 5, 
//                         anglecut[0] applies; if not, anglecut[1] is applied.
//                         Defaults: anglecut[2]={74,138}
//                         N.B.: angle_factor*anglecut is approximated to the 
//                               closest integer
//         fastalgo     flag for the fast trigger logic in each view:  
//                      0=2/3 superlayers with at least 1 PMT over threshold
//                      1=1/3     "        "               "
//                      2=3/3     "        "               "
//                      Default: fastalgo=0
//         MCflag       if true Montecarlo adc2mV conversion is used
//                      Default: true
//         debug        prints out the trigger bit pattern of each event
//                      (not intended for efficiency evaluation on large number //                      of events)
//                      Default: false
//
//         flagtype     define the the flag type
//                      0: fastOR:    OR  of ECAL Fast Trigger in X and Y view
//                      1: fastAND:   AND    "      "    "       "      "
//                      2: Level1OR:  OR  of ECAL Level 1 Trigger in X and Y view
//                      3: Level1AND: AND    "      "    "       "      "
//
// for each event:	
//
// bool ELECTRON = GetTofTrigFlags(HT_factor,SHT_factor,"HT",3,0) && 
//                 GetEcalTriggerFlags(Thr_factor,angle_factor,0,MCflag,0,1)
//
// bool PHOTON   = GetEcalTriggerFlags(Thr_factor,angle_factor,0,MCflag,0,3)
// 
// **************************************************************************
  EcalHitR *hit;
  int nhits;
  const int nPMTs=36;
  const int nsuplayrs=6; // ECAL trigger uses superlayers X1,X3,X5 and Y2,Y4,Y6

  float ThresmV[nsuplayrs]={26.7,60.,46.7,20.,33.,33.};
  float anglecut[2]={74,138};

  float MvOnPMT[nPMTs][nsuplayrs];
  for (int i = 0; i < nPMTs; ++i) {
    for (int j = 0; j < nsuplayrs; ++j)
      MvOnPMT[i][j]=0;
  }

  // Front end electronics parameters  
  const float GainTrigger=10.; 
  const float GainAnode=33.5; 
  float ADC2mV=3000./4096.; 
  if (MCflag) ADC2mV*=1.4; // temporary patch: ADC2MeV to be fixed in MC       
  // threshold function parameter	
  Double_t par[3], par0;
  par0=1.4  ;  // factor to get the effective threshold from ADC in mV where efficiency=50% 
  par[1]=3. ;  //rising step (mV)
  par[2]=1. ;  // maximal efficiency 
  float f, test, atrig;	
  //
  int layer,cell;
  int PMT, trig_Superlayer;
  
  nhits=nEcalHit();
  if (debug){
    cout<<"---> Nhits="<<nhits<<endl;
  }
  
  for (int ihit=0;ihit<nhits;ihit++){
    hit=pEcalHit(ihit);
    layer=hit->Plane;
    // consider only hits in superlayers 1 to 6
    if (layer>1 && layer<14){
      cell=hit->Cell;
      // add energy to its PMT
      PMT=cell/2;
      trig_Superlayer=(layer-2)/2; 
      MvOnPMT[PMT][trig_Superlayer] += hit->ADC[0]*ADC2mV*GainTrigger/GainAnode;
    }
  }
  
  
  // Find number of PMTs over threshold and fill the bit pattern mask
  int NumhitPMT[nsuplayrs]={0};
  int hitPMT[nsuplayrs][nPMTs];
  for (int j = 0; j < nsuplayrs; ++j) {
    for (int i = 0; i < nPMTs; ++i)
	  hitPMT[j][i] = 0;
  }

  for (int isuplayr=0; isuplayr<nsuplayrs;isuplayr++){
    // loop on PMTs
    for (int ipmt=0;ipmt<nPMTs;ipmt++){	  
      hitPMT[isuplayr][ipmt]=0; // trigger but mask
      // include the threshold function, deduced from data (average value per layer from  KSC data
      par[0]=ThresmV[isuplayr]*Thr_factor[isuplayr]*par0;
      atrig=MvOnPMT[ipmt][isuplayr];
      f=par[2]/(1+TMath::Exp(2*(-atrig+par[0])/par[1]));
      test= gRandom->Uniform(0,1);
      if (test<f) {
	hitPMT[isuplayr][ipmt] = 1;
	NumhitPMT[isuplayr]   += 1;
      }
    }
  }


  //
  // FAST TRIGGER
  bool fastX=false;
  bool fastY=false;
  int sl_bit[nsuplayrs]={0};
  // control of superlayer multiplicity  
  for (int isuplayr=0; isuplayr<nsuplayrs;isuplayr++){
    if (NumhitPMT[isuplayr] > 0) sl_bit[isuplayr]=1;
  }
  // FAST TRIGGER X
  switch (fastalgo){
  case 0:
    if ( (sl_bit[0] == 1 && sl_bit[2] == 1) || 
	 (sl_bit[0] == 1 && sl_bit[4] == 1) || 
	 (sl_bit[2] == 1 && sl_bit[4] == 1) ) fastX= true ;
    // FAST TRIGGER Y
    if ( (sl_bit[1] == 1 && sl_bit[3] == 1) || 
	 (sl_bit[1] == 1 && sl_bit[5] == 1) || 
	 (sl_bit[3] == 1 && sl_bit[5] == 1) ) fastY= true ;
    break;
  case 1:
    if ( sl_bit[0] == 1 || sl_bit[2] == 1 || sl_bit[4] == 1) fastX= true ;
    if ( sl_bit[1] == 1 || sl_bit[3] == 1 || sl_bit[5] == 1) fastY= true ;
    break;
  case 2:
    if ( sl_bit[0] == 1 && sl_bit[2] == 1 && sl_bit[4] == 1) fastX= true ;
    if ( sl_bit[1] == 1 && sl_bit[3] == 1 && sl_bit[5] == 1) fastY= true ;
    break;
  }
  
  // OR or AND logic
  bool fastOR;
  fastOR= fastX || fastY;
  bool fastAND;
  fastAND=fastX && fastY;

  if (debug && flagtype<2){
    cout << "X view" << endl;
    cout << "SL";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){
      cout << " " << ipmt%10 ;
    }
    cout << endl;
    for (int isuplayr=0; isuplayr<nsuplayrs;isuplayr+=2){  
      cout << " " << isuplayr+1 << " ";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){	 
      cout << hitPMT[isuplayr][ipmt] << " " ;
    }
    cout << "bit="<< sl_bit[isuplayr] << endl;
    }
    cout << "Y view" << endl;   
    cout << "SL";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){
      cout << " " << ipmt%10 ;
    }
    cout << endl;
    for (int isuplayr=1; isuplayr<nsuplayrs;isuplayr+=2){  
      cout << " " << isuplayr+1 << " ";
      for (int ipmt=0;ipmt<nPMTs;ipmt++){	 
	cout << hitPMT[isuplayr][ipmt] << " " ;
      }
      cout << "bit="<< sl_bit[isuplayr] << endl;
    }
    cout << "fastOR=" << fastOR << " fastAND=" << fastAND << endl;
  }
  if (flagtype==0) return fastOR;
  if (flagtype==1) return fastAND;
  //
  // proceed only if fast trigger is there
  bool Level1OR=false;
  bool Level1AND=false;
  float bari[nsuplayrs]={0};
  float bitsum[nsuplayrs]={0};
  if ((flagtype==2 && fastOR) ||
      (flagtype==3 && fastAND)){
    // ANGULAR TRIGGER
    bool Level1X=false;
    bool Level1Y=false;
    int nhit;
    //
    // find average position
    for (int isuplayr=0; isuplayr<nsuplayrs; isuplayr++){ 
      for (int ipmt = 0; ipmt< nPMTs ; ipmt++){
	bari[isuplayr] += hitPMT[isuplayr][ipmt]*ipmt;
	bitsum[isuplayr] += hitPMT[isuplayr][ipmt];
      }
      if (bitsum[isuplayr] > 0){
	bari[isuplayr] /=  bitsum[isuplayr];
      }
      else{
	bari[isuplayr]= -1.;
      }
    }    
    // centroids distance for far superlayers  
    float dfar,dclose1,dclose2;
    float dcut,dcut1,dcut2;
    dcut1= (int)(anglecut[0]*angle_factor[0]+0.5)/64.;
    dcut2= (int)(anglecut[1]*angle_factor[0]+0.5)/64.;
    //
    int suplayr1,suplayr2,suplayr3;
    // X view
    if (fastX){
      suplayr1=0;
      suplayr2=2;
      suplayr3=4;
      // try maximum superlayer distance
      if (bari[suplayr1]>=0 && bari[suplayr3]>=0){
	dfar=fabs(bari[suplayr1]-bari[suplayr3])/2.;
	nhit= NumhitPMT[suplayr1]+NumhitPMT[suplayr3];
	dcut=nhit<5?dcut1:dcut2;
	if (dfar < dcut) Level1X=true;
      }
      // if not try adiacent superlayers
      else{
	dclose1=0.;
	dclose2=0.;
	if (bari[suplayr1]>=0 && bari[suplayr2]>=0) dclose1=fabs(bari[suplayr1]-bari[suplayr2]);
	if (bari[suplayr2]>=0 && bari[suplayr3]>=0) dclose2=fabs(bari[suplayr2]-bari[suplayr3]);
	if (dclose1>dclose2){
	  nhit= NumhitPMT[suplayr1]+NumhitPMT[suplayr2];
	  dcut=nhit<5?dcut1:dcut2;
	  if (dclose1< dcut) Level1X=true;
	}
	else{      
	  nhit= NumhitPMT[suplayr2]+NumhitPMT[suplayr3];
	  dcut=nhit<5?dcut1:dcut2;
	  if (dclose2< dcut) Level1X=true;
	}
      }
    }
    // Y view
    if (fastY){
      suplayr1=1;
      suplayr2=3;
      suplayr3=5;
      // try maximum superlayer distance
      if (bari[suplayr1]>=0 && bari[suplayr3]>=0){
	dfar=fabs(bari[suplayr1]-bari[suplayr3])/2.;
	nhit= NumhitPMT[suplayr1]+NumhitPMT[suplayr3];
	dcut=nhit<5?dcut1:dcut2;
	if (dfar < dcut) Level1Y=true;
      }
      // if not try adiacent superlayers
      else{
	dclose1=0.;
	dclose2=0.;
	if (bari[suplayr1]>=0 && bari[suplayr2]>=0) dclose1=fabs(bari[suplayr1]-bari[suplayr2]);
	if (bari[suplayr2]>=0 && bari[suplayr3]>=0) dclose2=fabs(bari[suplayr2]-bari[suplayr3]);
	if (dclose1>dclose2){
	  nhit= NumhitPMT[suplayr1]+NumhitPMT[suplayr2];
	  dcut=nhit<5?dcut1:dcut2;
	  if (dclose1< dcut) Level1Y=true;
	}
	else{      
	  nhit= NumhitPMT[suplayr2]+NumhitPMT[suplayr3];
	  dcut=nhit<5?dcut1:dcut2;
	  if (dclose2< dcut) Level1Y=true;
	} 
      }
    }
    //
    Level1OR = Level1X || Level1Y;
    Level1AND= Level1X && Level1Y;
  }
  if (debug){
    cout << "X view" << endl;
    cout << "SL";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){
      cout << " " << ipmt%10 ;
    }
    cout << endl;
    for (int isuplayr=0; isuplayr<nsuplayrs;isuplayr+=2){  
      cout << " " << isuplayr+1 << " ";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){	 
      cout << hitPMT[isuplayr][ipmt] << " " ;
    }
    cout << "bit="<< sl_bit[isuplayr] << " " << bari[isuplayr] << endl;
    }
    cout << "Y view" << endl;   
    cout << "SL";
    for (int ipmt=0;ipmt<nPMTs;ipmt++){
      cout << " " << ipmt%10 ;
    }
    cout << endl;
    for (int isuplayr=1; isuplayr<nsuplayrs;isuplayr+=2){  
      cout << " " << isuplayr+1 << " ";
      for (int ipmt=0;ipmt<nPMTs;ipmt++){	 
	cout << hitPMT[isuplayr][ipmt] << " " ;
      }
      cout << "bit="<< sl_bit[isuplayr] << " " << bari[isuplayr] << endl;
    }
    cout << "fastOR=" << fastOR << " fastAND=" << fastAND << endl;
    cout << "Level1OR=" << Level1OR << " Level1AND=" << Level1AND << endl;
  }
  if (flagtype==2) return Level1OR;
  if (flagtype==3) return Level1AND;

  return false;
}

//-----------------------------
bool AMSEventR::GetTofTrigFlags(float HT_factor, float SHT_factor,string TOF_type, int TOF_numb, int ACC_max){
/*
*  
*  Created by Andrea Contin on 06/03/2011.
*
* GetTofTrigFlags(float HT_factor, float SHT_factor,string TOF_type, int TOF_numb, int ACC_max)
*
* 
*
* HT_factor = multiplicative factor w.r.t. real data HT thrershold
*
* HT_factor = multiplicative factor w.r.t. real data HT thrershold
*
* TOF_type = HT  --> TOF High Threshold (Z>=1)
*            SHT --> TOF Super High Threshold (Z>1)
*            FTZ --> slow TOF Super High Threshold (Z>1)
*
* TOF_numb = 3   --> at least 3 out of 4 layers
*          = 4   --> 4 out of 4 layers
*          for FTZ: = 0 -> (1&2)&(3&4), = 1 -> (1&2)&(3|4), = 2 -> (1|2)&(3&4), = 3 -> (1|2)&(3|4)
*
* ACC_max  = n   --> at most n ACC counters
*          = -1  --> ACC not in the trigger
*
* for each event:	bool FTC=GetTofTrigFlags(HT_factor,SHT_factor,"HT",3,0);
*               	bool BZ=GetTofTrigFlags(HT_factor,SHT_factor,"SHT",3,-1);
*               	bool FTZ=GetTofTrigFlags(HT_factor,SHT_factor,"FTZ",0,-1);
*              	if(FTC || BZ || FTZ) accept event ....
*/
	
	// values constant with time
	
	int counterside[68]={
		1011,1012,1021,1022,1031,1032,1041,1042,1051,1052,
		1061,1062,1071,1072,1081,1082,2011,2012,2021,2022,
		2031,2032,2041,2042,2051,2052,2061,2062,2071,2072,
		2081,2082,3011,3012,3021,3022,3031,3032,3041,3042,
		3051,3052,3061,3062,3071,3072,3081,3082,3091,3092,
		3101,3102,4011,4012,4021,4022,4031,4032,4041,4042,
		4051,4052,4061,4062,4071,4072,4081,4082
	};       // LBBS (L=layer 1-4, BB=bar 1-8(10), S=side 1-2)
	int DAC[68]={
		1,17,2,18,1,17,2,18,1,17,2,18,3,19,4,20,5,21,6,22,
		5,21,6,22,5,21,6,22,7,23,8,24,9,25,10,26,9,25,10,26,
		9,25,10,26,11,27,12,28,11,27,12,28,13,29,14,30,13,29,14,30,
		13,29,14,30,15,31,16,32
	};               // DAC number for each counter side
	float peak_mc[68]={
		51.96,51.92,55.26,55.47,55.05,55.04,55.13,54.59,54.63,54.83,
		55.09,55.02,55.62,55.53,52.53,52.38,58.2,58.32,51.64,51.57,
		51.56,51.29,51.61,51.4,51.57,51.35,51.7,51.7,51.95,51.44,
		58.41,58.49,51.00,50.96,49.61,49.67,46.54,46.63,49.33,49.14,
		49.22,49.16,49.16,48.91,49.29,49.12,46.44,46.59,49.17,49.26,
		50.78,50.97,53.04,53.01,53.2,53.11,53.08,52.98,53.49,53.42,
		53.08,53.22,53.27,53.37,54.1,53.91,52.39,52.39
	};         // m.i.p. peak in mc (ADC ch.)
	float a_ht[68]={
		1.6,1.61,1.4,1.39,1.28,1.37,1.34,1.27,1.32,1.3,
		1.28,1.31,1.3,1.37,1.63,1.69,1.41,1.39,1.33,1.24,
		1.32,1.39,1.39,1.36,1.44,1.3,1.38,1.43,1.22,1.3,
		1.53,1.58,1.41,1.4,1.33,1.45,1.28,1.32,1.26,1.06,
		1.3,1.28,1.31,1.33,1.26,1.21,1.22,1.33,1.41,1.26,
		1.52,1.52,1.77,1.75,1.29,1.43,1.25,1.25,1.35,1.35,
		1.31,1.32,1.34,1.32,1.33,1.1,1.83,1.83
	};   // HT threshold (ADC ch.)=a_ht*DAC+b_ht
	float b_ht[68]={
		6.62,3.68,2.22,1.43,3.87,2.72,1.9,1.02,5.06,2.26,
		3.4,1.17,2.48,2.49,2.53,2.2,3.18,2.04,2.75,3.77,
		2.43,-0.62,3,2.51,0.55,2.29,5.74,0.85,3.45,1.99,
		3.45,1.83,3.68,2.4,2.35,2.73,3.74,1.68,2.02,13.35,
		3.87,0.62,2.24,2.86,1.95,3.11,3.09,3.22,1.07,2.84,
		2.38,3.45,3.65,3.45,2.51,3.04,4.29,3.07,1.28,3.01,
		2.9,1.11,2.2,4.78,3.58,8.58,4.04,4.83
	};   // HT threshold (ADC ch.)=a_ht*DAC+b_ht
	float a_sht[68]={
		1.4,1.42,1.18,1.24,1.13,1.22,1.24,1.13,1.07,1.11,
		1.09,1.13,1.13,1.19,1.49,1.52,1.18,1.11,1.06,1.01,
		1.14,1.03,1.17,1.12,1.08,1.15,1.17,1.22,1.15,1.02,
		1.37,1.27,1.18,1.38,1.14,1.4,1.15,1.15,1.1,1.21,
		1.13,1.3,1.1,1.22,1.09,1.26,1.09,1.28,1.34,1.3,
		1.32,1.43,1.42,1.71,1.07,1.41,1.08,1.25,1.12,1.29,
		1.07,1.36,1.16,1.31,1.04,1.07,1.53,1.74
	}; // SHT threshold (ADC ch.)=a_sht*DAC+b_sht
	float b_sht[68]={
		17.69,14.07,17.35,16.85,12.33,9.19,10.04,14.58,13.31,8.66,
		15.52,17.48,12.41,7.46,13.71,18.26,10.68,18.65,13.55,14.34,
		7.44,14.01,9.94,14.29,8.12,13.37,11.81,12.61,8.79,15.14,
		8.24,12.76,10.36,1.47,10.58,4.39,5.74,7.38,9.12,5.3,
		7.48,-2.37,11.28,6.61,7.61,0.6,12.59,3.16,1.91,0.86,
		15.23,4.46,20.31,4.86,15.01,0.3,13.66,3.12,15.66,2.63,
		13.77,-0.51,14.25,4.03,16.59,9.45,20.36,6.71
	}; // SHT threshold (ADC ch.)=a_sht*DAC+b_sht
	int c_max[32]={
		1011,1021,1071,1081,2011,2061,2071,2081,3051,3021,
		3071,3101,4011,4021,4071,4081,1012,1062,1072,1082,
		2032,2042,2072,2082,3012,3062,3092,3082,4012,4062,
		4072,4082
	};                         // LBBS of the counter side which has the maximum threshold in one DAC
	
	// values dependent on time and/or setting of HV and thresholds
	
	float ht_data[68]={
		0.69,0.71,0.6,0.5,0.46,0.37,0.34,0.6,0.73,0.57,
		0.57,0.64,0.66,0.65,0.64,0.61,0.7,0.65,0.37,0.61,
		0.39,0.69,0.53,0.7,0.57,0.45,0.77,0.34,0.6,0.58,
		0.66,0.71,0.47,0.67,0.65,0.44,0.54,0.53,0.59,0.7,
		0.68,0.36,0.44,0.68,0.65,0.56,0.53,0.71,0.43,0.62,
		0.66,0.55,0.72,0.62,0.77,0.55,0.63,0.48,0.49,0.63,
		0.67,0.32,0.55,0.63,0.58,0.64,0.73,0.73
	};         // relative HT threshold (ADC ch.)
	float sht_data[68]={
		4.63,4.72,4.48,3.42,3.21,2.35,2.24,4.32,4.5,3.33,
		3.87,4.46,4.18,4.0,4.56,4.76,4.36,4.5,2.31,4.03,
		2.52,4.66,3.16,4.63,3.87,2.85,4.62,2.44,4.41,3.28,
		4.13,4.46,3.25,4.01,4.56,2.64,3.82,3.24,4.12,4.37,
		4.6,2.11,3.05,4.4,4.57,3.35,3.79,4.43,3.02,4.27,
		4.92,3.67,4.63,4.02,4.63,3.05,4.19,3.06,2.97,3.73,
		4.52,2.11,3.21,3.79,3.07,4.19,4.66,4.09
	};        // relative SHT threshold (ADC ch.)
	float peak_data[68]={
		36.39,38.11,41.56,58.47,41.61,62.19,78.34,41.72,28.91,40.36,
		44.00,42.49,47.04,50.92,42.19,47.6,36.17,35.56,61.53,44.17,
		58.69,30.61,46.88,41.11,37.19,54.98,32.57,85.96,38.69,80.5,
		59.14,32.24,69.47,46.1,47.65,75.73,53.99,54.21,49.55,41.53,
		45.76,79.82,70.26,42.76,41.53,44.65,43.38,41.15,71.46,37.08,
		41.1,56.09,56.56,50.17,37.71,56.47,46.34,47.79,59.35,46.12,
		43.28,71.33,56.05,46.05,93.96,48.35,50.71,50.84
	};       // m.i.p. peak in data (ADC ch.)
	
	Level1R *plvl1;      // pointer to Level1 trigger
	TofRawSideR *ptofrs; // pointer to TofRawSide
	
	float DAC_value_HT[32];    // DAC values corresponding to the modified HT thresolds
	float DAC_value_SHT[32];   // DAC values corresponding to the modified SHT thresolds
	float ht_data_new[68];     // modified relative HT threshold (ADC ch.)
	float sht_data_new[68];    // modified relative SHT threshold (ADC ch.)
	
	// check input variables
	
	if(HT_factor<0) {
		printf("GetTofTrigFlags ERROR --> wrong HT factor: %f\n",HT_factor);
		return false;
	}
	if(SHT_factor<0) {
		printf("GetTofTrigFlags ERROR --> wrong SHT factor: %f\n",SHT_factor);
		return false;
	}
	if(TOF_type!="HT" && TOF_type!="SHT" && TOF_type!="FTZ") {
		printf("GetTofTrigFlags ERROR --> wrong TOF_type: %s\n",TOF_type.c_str());
		return false;
	}
	if((TOF_type=="HT" || TOF_type=="SHT") && TOF_numb!=3 && TOF_numb!=4) {
		printf("GetTofTrigFlags ERROR --> TOF_type=%s, wrong TOF_numb: %d\n",TOF_type.c_str(),TOF_numb);
		return false;
	}
	if(TOF_type=="FTZ" && (TOF_numb<0 || TOF_numb>3)) {
		printf("GetTofTrigFlags ERROR --> TOF_type=FTZ, wrong TOF_numb: %d\n",TOF_numb);
		return false;
	}
	if(ACC_max<-1 || ACC_max>8) {
		printf("GetTofTrigFlags ERROR --> wrong ACC_max: %d\n",ACC_max);
		return false;
	}
	
	// find DAC values for new HT and SHT	
	
	float s;
	for (int i_dac=0;i_dac<32;i_dac++){
		for (int is=0;is<68;is++){
			if(c_max[i_dac]==counterside[is]) {
				s=HT_factor*peak_data[is]*ht_data[is];
				DAC_value_HT[i_dac]=(s-b_ht[is])/a_ht[is];
				if(DAC_value_HT[i_dac]>255) {
					DAC_value_HT[i_dac]=255;
				}else if(DAC_value_HT[i_dac]<0) {
					DAC_value_HT[i_dac]=5;
				}
				s=SHT_factor*peak_data[is]*sht_data[is];
				DAC_value_SHT[i_dac]=(s-b_sht[is])/a_sht[is];
				if(DAC_value_SHT[i_dac]>255) {
					DAC_value_SHT[i_dac]=255;
				}else if(DAC_value_SHT[i_dac]<0) {
					DAC_value_SHT[i_dac]=5;
				}
			}
		}
	}
	
	// find new SHT
	
	for (int i_dac=0;i_dac<32;i_dac++){
		for (int is=0;is<68;is++){
			if(DAC[is]==i_dac+1){
				ht_data_new[is]=(DAC_value_HT[i_dac]*a_ht[is]+b_ht[is])/peak_data[is];
				sht_data_new[is]=(DAC_value_SHT[i_dac]*a_sht[is]+b_sht[is])/peak_data[is];
			}
		}
	}
	
	// event analysis	
	
	// compute the number of ACC counters hit and set ACC_OK
	
	bool ACC_OK;
	plvl1 = pLevel1(0);
	if(ACC_max>-1){
		if(plvl1){
			int nanti = 0;
			unsigned int antipatt = plvl1->AntiPatt;
			for(int i=0;i<8;i++){ 
				if((((antipatt>>i)&1)==1) || (((antipatt>>(i+8))&1)==1)) nanti++;
			}
			ACC_OK=(nanti<=ACC_max);
			if(!ACC_OK) return false; // more than the allowed number of ACC counters hit
		}else{
			return false;
		}
	}else{
		ACC_OK=true; // ACC not in the trigger
	}
	
	// look for any side in each TOF plane
	
	bool TOF_OK = false;
	bool layer_ok[4]={false,false,false,false};
	for(unsigned int irc=0; irc<NTofRawSide(); irc++) {
		ptofrs = pTofRawSide(irc);
		if (ptofrs->stat==0) { // check row cluster OK
			int layer=ptofrs->swid/1000; 
			float adc=ptofrs->adca;
			for (int is=0;is<68;is++){
				if(ptofrs->swid==counterside[is]){
					if(TOF_type=="HT" && adc>ht_data_new[is]*peak_mc[is]) layer_ok[layer-1]=true;
					if(TOF_type=="SHT" && adc>sht_data_new[is]*peak_mc[is]) layer_ok[layer-1]=true; 
				}
			}
		}
	}

	if(TOF_type=="HT" || TOF_type=="SHT"){
		if(TOF_numb==3){
			TOF_OK=((layer_ok[0]&&layer_ok[1]&&layer_ok[2]) ||
					(layer_ok[0]&&layer_ok[1]&&layer_ok[3]) ||
					(layer_ok[0]&&layer_ok[2]&&layer_ok[3]) ||
					(layer_ok[1]&&layer_ok[2]&&layer_ok[3]));
		}else{
			TOF_OK=(layer_ok[0]&&layer_ok[1]&&layer_ok[2]&&layer_ok[3]);
		}
	}

	if(TOF_type=="FTZ"){
		if(TOF_numb==0){
			TOF_OK=(layer_ok[0]&&layer_ok[1]&&layer_ok[2]&&layer_ok[3]);
		}else if(TOF_numb==1){
			TOF_OK=((layer_ok[0]&&layer_ok[1])&&(layer_ok[2]||layer_ok[3]));
		}else if(TOF_numb==2){
			TOF_OK=((layer_ok[0]||layer_ok[1])&&(layer_ok[2]&&layer_ok[3]));
		}else if(TOF_numb==3){
			TOF_OK=((layer_ok[0]||layer_ok[1])&&(layer_ok[2]||layer_ok[3]));
		}
	}
	
	return TOF_OK && ACC_OK;
}
//-----------------------------
TBranch* AMSEventR::bStatus;
TBranch* AMSEventR::bAll;
TBranch* AMSEventR::bHeader;
TBranch* AMSEventR::bEcalHit;
TBranch* AMSEventR::bEcalCluster;
TBranch* AMSEventR::bEcal2DCluster;
TBranch* AMSEventR::bEcalShower;
TBranch* AMSEventR::bRichHit;
TBranch* AMSEventR::bRichRing;
TBranch* AMSEventR::bRichRingB;
TBranch* AMSEventR::bTofRawCluster;
TBranch* AMSEventR::bTofRawSide;
TBranch* AMSEventR::bTofCluster;
TBranch* AMSEventR::bTofClusterH;
TBranch* AMSEventR::bAntiRawSide;
TBranch* AMSEventR::bAntiCluster;
TBranch* AMSEventR::bTrRawCluster;
TBranch* AMSEventR::bTrCluster;
TBranch* AMSEventR::bTrRecHit;
TBranch* AMSEventR::bTrTrack;
TBranch* AMSEventR::bTrdRawHit;
TBranch* AMSEventR::bTrdCluster;
TBranch* AMSEventR::bTrdSegment;
TBranch* AMSEventR::bTrdTrack;
TBranch* AMSEventR::bTrdHSegment;
TBranch* AMSEventR::bTrdHTrack;
TBranch* AMSEventR::bLevel1;
TBranch* AMSEventR::bLevel3;
TBranch* AMSEventR::bBeta;
TBranch* AMSEventR::bBetaB;
TBranch* AMSEventR::bBetaH;
TBranch* AMSEventR::bEcalH;
TBranch* AMSEventR::bVertex;
TBranch* AMSEventR::bCharge;
TBranch* AMSEventR::bParticle;
TBranch* AMSEventR::bAntiMCCluster;
TBranch* AMSEventR::bTrMCCluster;
TBranch* AMSEventR::bTofMCCluster;
TBranch* AMSEventR::bTofMCPmtHit;
TBranch* AMSEventR::bEcalMCHit;
TBranch* AMSEventR::bTrdMCCluster;
TBranch* AMSEventR::bRichMCCluster;
TBranch* AMSEventR::bMCTrack;
TBranch* AMSEventR::bMCEventg;
TBranch* AMSEventR::bDaqEvent;
TBranch* AMSEventR::bAux;

void* AMSEventR::vStatus=0;
void* AMSEventR::vHeader=0;
void* AMSEventR::vEcalHit=0;
void* AMSEventR::vEcalCluster=0;
void* AMSEventR::vEcal2DCluster=0;
void* AMSEventR::vEcalShower=0;
void* AMSEventR::vRichHit=0;
void* AMSEventR::vRichRing=0;
void* AMSEventR::vRichRingB=0;
void* AMSEventR::vTofRawCluster=0;
void* AMSEventR::vTofRawSide=0;
void* AMSEventR::vTofCluster=0;
void* AMSEventR::vTofClusterH=0;
void* AMSEventR::vAntiRawSide=0;
void* AMSEventR::vAntiCluster=0;
void* AMSEventR::vTrRawCluster=0;
void* AMSEventR::vTrCluster=0;
void* AMSEventR::vTrRecHit=0;
void* AMSEventR::vTrTrack=0;
void* AMSEventR::vTrdRawHit=0;
void* AMSEventR::vTrdCluster=0;
void* AMSEventR::vTrdSegment=0;
void* AMSEventR::vTrdTrack=0;
void* AMSEventR::vTrdHTrack=0;
void* AMSEventR::vTrdHSegment=0;
void* AMSEventR::vLevel1=0;
void* AMSEventR::vLevel3=0;
void* AMSEventR::vBeta=0;
void* AMSEventR::vBetaB=0;
void* AMSEventR::vBetaH=0;
void* AMSEventR::vEcalH=0;
void* AMSEventR::vVertex=0;
void* AMSEventR::vCharge=0;
void* AMSEventR::vParticle=0;
void* AMSEventR::vAntiMCCluster=0;
void* AMSEventR::vTrMCCluster=0;
void* AMSEventR::vTofMCCluster=0;
void* AMSEventR::vTofMCPmtHit=0;
void* AMSEventR::vEcalMCHit=0;
void* AMSEventR::vTrdMCCluster=0;
void* AMSEventR::vRichMCCluster=0;
void* AMSEventR::vMCTrack=0;
void* AMSEventR::vMCEventg=0;
void* AMSEventR::vDaqEvent=0;
void* AMSEventR::vAux=0;

char   DaqEventR::_Info[255];
char  TofClusterR::_Info[255];
char  TofClusterHR::_Info[255];
char  ParticleR::_Info[255];
#ifndef _PGTRACK_
//PZ FIXME
char TrRecHitR::_Info[255];
char TrTrackR::_Info[255];
char TrMCClusterR::_Info[255];
#endif
float TrdTrackR::ChargePDF[10030];
char TrdTrackR::_Info[255];
char TrdClusterR::_Info[255];
char EcalClusterR::_Info[255];
char EcalShowerR::_Info[255];
char RichHitR::_Info[255];
char RichRingR::_Info[255];
char RichRingBR::_Info[255];
char MCEventgR::_Info[255];
char Level1R::_Info[255];
char Level3R::_Info[512];
char HeaderR::_Info[1024];


TTree*     AMSEventR::_Tree=0;
TTree*     AMSEventR::_TreeSetup=0;
TTree*     AMSEventR::_ClonedTree[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
TTree*     AMSEventR::_ClonedTreeSetup[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AMSEventR* AMSEventR::_Head=0;
AMSEventR::Service*    AMSEventR::pService=0;
int AMSEventR::_Count=0;
int AMSEventR::_NFiles=-1;
TTree* AMSEventR::_pFiles=0;
char AMSEventR::filename[1024]="";
vector<string> AMSEventR::fRequested;
int AMSEventR::_Entry=-1;
unsigned long long  AMSEventR::_Lock=0;
int AMSEventR::_EntrySetup=-1;
AMSEventR::MY_TLS_ITEM  AMSEventR::_RunSetup;
const char* AMSEventR::_Name="ev.";   

// Calibration correction modes
int RichRingR::shouldLoadCorrection=RichRingR::tileCorrection;
bool RichRingR::loadChargeUniformityCorrection=true;

// Dynamic calibration
bool RichRingR::_updateDynamicCalibration=false;
bool RichRingR::_isCalibrationEvent=false;
double RichRingR::_pThreshold=50;
int    RichRingR::_tileCalEvents=50;
int    RichRingR::_tileCalWindow=6;
double RichRingR::_tileLearningFactor=0.25;
TH1F RichRingR::indexHistos[122];
double RichRingR::indexCorrection[122]={1,1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1,
					1,1,1,1,1,1,1,1,1,1,1};

int RichRingR::_lastUpdate[122]={1,1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1,
				 1,1,1,1,1,1,1,1,1,1,1};

int RichRingR::_numberUpdates[122]={0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0};


int RichRingR::_totalIndex[122]={0,0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0,
				 0,0,0,0,0,0,0,0,0,0,0};


double RichRingR::_sumIndex[122]={0,0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0,
				  0,0,0,0,0,0,0,0,0,0,0};

// Rich Charge Corrections Settings & Flags
bool RichRingR::loadPmtCorrections=true;
int RichRingR::pmtCorrectionsFailed = -1;
TString RichRingR::correctionsDir="";  // Directory
bool RichRingR::useRichRunTag = false;           // good Rich runs 
bool RichRingR::usePmtStat = true;               // good PMT status
bool RichRingR::useSignalMean = false;           // Equalize PMT Gains to mean (median)
bool RichRingR::useGainCorrections = true;       // PMT Gain equalization
bool RichRingR::useEfficiencyCorrections = true; // PMT Efficiency equalization
bool RichRingR::useBiasCorrections = true;       // PMT Efficiency bias corrections
bool RichRingR::useTemperatureCorrections = true;// PMT Temperature corrections
bool RichRingR::useEffectiveTemperatureCorrection = false;// PMT Additional Temperature correction
bool RichRingR::useExternalFiles = false;        // read external files
bool RichRingR::reloadTemperatures = true;       // force load Temperatures from AMSSetup
bool RichRingR::reloadRunTag = false;            // force load Config & Status from ext. files
bool RichRingR::reloadPMTs = true;               // force load PMT info from ext. files

void AMSEventR::GetBranch(TTree *fChain){
  char tmp[255];
  bAll=fChain->GetBranch(_Name);
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fStatus");
    bStatus = fChain->GetBranch(tmp);
  }
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fHeader");
    bHeader = fChain->GetBranch(tmp);
  }
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalHit");
    bEcalHit=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalCluster");
    bEcalCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcal2DCluster");
    bEcal2DCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalShower");
    bEcalShower=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichHit");
    bRichHit=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichRing");
    bRichRing=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichRingB");
    bRichRingB=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofRawCluster");
    bTofRawCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofRawSide");
    bTofRawSide=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofCluster");
    bTofCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofClusterH");
    bTofClusterH=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiRawSide");
    bAntiRawSide=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiCluster");
    bAntiCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrRawCluster");
    bTrRawCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrCluster");
    bTrCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrRecHit");
    bTrRecHit=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrTrack");
    bTrTrack=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdRawHit");
    bTrdRawHit=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdCluster");
    bTrdCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdSegment");
    bTrdSegment=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdTrack");
    bTrdTrack=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdHSegment");
    bTrdHSegment=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdHTrack");
    bTrdHTrack=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fLevel1");
    bLevel1=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fLevel3");
    bLevel3=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBeta");
    bBeta=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBetaB");
    bBetaB=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBetaH");
    bBetaH=fChain->GetBranch(tmp);
   }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalH");
    bEcalH=fChain->GetBranch(tmp);
   }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fCharge");
    bCharge=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fVertex");
    bVertex=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fParticle");
    bParticle=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiMCCluster");
    bAntiMCCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrMCCluster");
    bTrMCCluster=fChain->GetBranch(tmp);
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofMCCluster");
    bTofMCCluster=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofMCPmtHit");
    bTofMCPmtHit=fChain->GetBranch(tmp);
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalMCHit");
    bEcalMCHit=fChain->GetBranch(tmp);
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdMCCluster");
    bTrdMCCluster=fChain->GetBranch(tmp);
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichMCCluster");
    bRichMCCluster=fChain->GetBranch(tmp);
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fMCTrack");
    bMCTrack=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fMCEventg");
    bMCEventg=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fDaqEvent");
    bDaqEvent=fChain->GetBranch(tmp);
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAux");
    bAux=fChain->GetBranch(tmp);
  }
}



void AMSEventR::GetBranchA(TTree *fChain){
  char tmp[255];

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fHeader");
    vHeader = fChain->GetBranch(tmp)->GetAddress();
  }
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fStatus");
    vStatus = fChain->GetBranch(tmp)->GetAddress();
  }
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalHit");
    vEcalHit=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalCluster");
    vEcalCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcal2DCluster");
    vEcal2DCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalShower");
    vEcalShower=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichHit");
    vRichHit=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichRing");
    vRichRing=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichRingB");
    vRichRingB=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofRawCluster");
    vTofRawCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofRawSide");
    vTofRawSide=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofCluster");
    vTofCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofClusterH");
    vTofClusterH=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiRawSide");
    vAntiRawSide=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiCluster");
    vAntiCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrRawCluster");
    vTrRawCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrCluster");
    vTrCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrRecHit");
    vTrRecHit=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrTrack");
    vTrTrack=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdRawHit");
    vTrdRawHit=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdCluster");
    vTrdCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdSegment");
    vTrdSegment=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdTrack");
    vTrdTrack=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdHSegment");
    vTrdHSegment=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdHTrack");
    vTrdHTrack=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fLevel1");
    vLevel1=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fLevel3");
    vLevel3=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBeta");
    vBeta=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBetaB");
    vBetaB=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fBetaH");
    vBetaH=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalH");
    vEcalH=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fCharge");
    vCharge=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fVertex");
    vVertex=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fParticle");
    vParticle=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fAntiMCCluster");
    vAntiMCCluster=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrMCCluster");
    vTrMCCluster=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofMCCluster");
    vTofMCCluster=fChain->GetBranch(tmp)->GetAddress();
  }

   {
    strcpy(tmp,_Name);
    strcat(tmp,"fTofMCPmtHit");
    vTofMCPmtHit=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fEcalMCHit");
    vEcalMCHit=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fTrdMCCluster");
    vTrdMCCluster=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fRichMCCluster");
    vRichMCCluster=fChain->GetBranch(tmp)->GetAddress();
  }


  {
    strcpy(tmp,_Name);
    strcat(tmp,"fMCTrack");
    vMCTrack=fChain->GetBranch(tmp)->GetAddress();
  }

  {
    strcpy(tmp,_Name);
    strcat(tmp,"fMCEventg");
    vMCEventg=fChain->GetBranch(tmp)->GetAddress();
  }
  {
    strcpy(tmp,_Name);
    strcat(tmp,"fDaqEvent");
    vDaqEvent=fChain->GetBranch(tmp)->GetAddress();
  }

}



void AMSEventR::SetCont(){
  // Set container sizes;
  fHeader.EcalHits=fEcalHit.size();
  fHeader.EcalClusters=fEcalCluster.size();
  fHeader.Ecal2DClusters=fEcal2DCluster.size();
  fHeader.EcalShowers=fEcalShower.size();
  fHeader.RichHits=fRichHit.size();
  fHeader.RichRings=fRichRing.size();
  fHeader.RichRingBs=fRichRingB.size();
  fHeader.TofRawClusters=fTofRawCluster.size();
  fHeader.TofRawSides=fTofRawSide.size();
  fHeader.TofClusters=fTofCluster.size();
  fHeader.TofClusterHs=fTofClusterH.size();
  fHeader.AntiRawSides=fAntiRawSide.size();
  fHeader.AntiClusters=fAntiCluster.size();
  fHeader.TrRawClusters=fTrRawCluster.size();
  fHeader.TrClusters=fTrCluster.size();
  fHeader.TrRecHits=fTrRecHit.size();
  fHeader.TrTracks=fTrTrack.size();
  fHeader.TrdRawHits=fTrdRawHit.size();
  fHeader.TrdClusters=fTrdCluster.size();
  fHeader.TrdSegments=fTrdSegment.size();
  fHeader.TrdTracks=fTrdTrack.size();
  fHeader.TrdHSegments=fTrdHSegment.size();
  fHeader.TrdHTracks=fTrdHTrack.size();
  fHeader.Level1s=fLevel1.size();
  fHeader.Level3s=fLevel3.size();
  fHeader.Betas=fBeta.size();
  fHeader.BetaBs=fBetaB.size();
  fHeader.BetaHs=fBetaH.size();
  fHeader.EcalHs=fEcalH.size();
  fHeader.Vertexs=fVertex.size();
  fHeader.Charges=fCharge.size();
  fHeader.Particles=fParticle.size();
  fHeader.AntiMCClusters=fAntiMCCluster.size();
  fHeader.TrMCClusters=fTrMCCluster.size();
  fHeader.TofMCClusters=fTofMCCluster.size();
  fHeader.TofMCPmtHits=fTofMCPmtHit.size();
  fHeader.EcalMCHits=fEcalMCHit.size();
  fHeader.TrdMCClusters=fTrdMCCluster.size();
  fHeader.RichMCClusters=fRichMCCluster.size();
  fHeader.MCTracks=fMCTrack.size();
  fHeader.MCEventgs=fMCEventg.size();
  fHeader.DaqEvents=fDaqEvent.size();
}

bool AMSEventR::ReadHeader(int entry){
  bool badrun=false;
  static unsigned int runo=0;
  static unsigned int evento=0;
  static const int probe=100;
  static double dif=0;
  static double difm=0;
  static double dif2=0;
#pragma omp threadprivate(runo,evento,dif,difm,dif2)
  fStatus=0;
  if(bStatus){
    int j=0;
    {
      j=bStatus->GetEntry(entry);
    }
    if(j>0){
      _Entry=entry;
      if(_Entry!=0 && !UProcessStatus(fStatus)){
#pragma omp critical (rd) 
	(*pService).TotalTrig++;
	return false;
      }
    }
  }
  else{
    static int nold=0;
    if(!nold++){
      cerr<<"AMSEvent::ReadHeader-E-OldVersionUprocessStatusNotProcessed "<<endl;
    }
  }
  //    if(this!=_Head){
  //       cerr<<" AMSEventR::ReadHeader-S-WrongHeadPointer"<<endl;
  //      _Entry=entry;
  //       clear();
  //       return false;
  //     }
  _Entry=entry;
  memset((char*)(&fHeader)+sizeof(void*),0,sizeof(fHeader)-sizeof(void*));
  int i;
  {
    i=bHeader->GetEntry(entry);
  }
  badrun=isBadRun(Run());      
  bool rts=RunTypeSelected(fHeader.RunType);

  if(badrun || !rts)return false;
  clear();
  static TFile* local_pfile=0;
  static string local_pfile_name="";
#pragma omp threadprivate (local_pfile)
#pragma omp threadprivate (local_pfile_name)
  if(i>0){
    if( local_pfile!=_Tree->GetCurrentFile() || entry==0 || strcmp(local_pfile_name.c_str(),_Tree->GetCurrentFile()->GetName())){
      string f((const char*)_Tree->GetCurrentFile()->GetName());
      if(pService)(*pService).fProcessed.push_back(f);
      int thr=0;
      int nthr=1;
#ifdef _OPENMP
      thr=omp_get_thread_num();
      nthr=omp_get_num_threads();
#endif

      //#pragma omp atomic 
      //_Lock-=(1<<thr);
      //cout <<" LockBefore "<<_Lock<<" "<<thr<<endl;
#pragma omp critical (readsetup)
      {
	//#pragma omp atomic 
	//_Lock+=0x100000000;
	//while(!(_Lock&0xFFFFFFFF)){
	//}
	static int initdone[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      
	local_pfile=_Tree->GetCurrentFile();
	local_pfile_name=_Tree->GetCurrentFile()->GetName();
	// workaround root static bug
	if(!gDirectory ||  !dynamic_cast<TDirectoryFile*>(gDirectory)){
          cout <<" Open "<<_Tree->GetCurrentFile()->GetName()<<endl;
          TFile::Open(_Tree->GetCurrentFile()->GetName());
          cout <<"AMSEventR::ReadHeader-I-SettinggDirectory "<<endl;
	}
	if(!InitSetup(local_pfile,"AMSRootSetup",Run())){
	  cout <<"AMSEventR::ReadHeader-I-Version/OS "<<Version()<<"/"<<OS()<<" "<<_Tree->GetCurrentFile()->GetName()<<endl;
	}
	else{
	  cout <<"AMSSetupR::ReadHeader-I-Version/OS/BuildTime "<<getsetup()->fHeader.BuildNo<<"/"<<getsetup()->fHeader.OS<<" "<<getsetup()->BuildTime()<<" Run "<<getsetup()->fHeader.Run<<" "<<_Tree->GetCurrentFile()->GetName()<<endl;
	  cout <<"AMSSetupR::ReadHeader-I-"<<getsetup()->fScalers.size()<<" ScalersEntriesFound "<<endl;
	  cout<<"AMSSetupR::ReadHeader-I-"<<getsetup()->getAllTDV(UTime())<<" TDVNamesFound"<<endl;
	  for(AMSSetupR::GPS_i i=getsetup()->fGPS.begin();i!=getsetup()->fGPS.end();i++){
	    //cout << " GPS "<<i->first<<" "<<i->second.Run<<endl;
	  }
	  badrun=isBadRun(getsetup()->fHeader.Run);
	  //getsetup()->printAllTDV_Time();
	  //getsetup()->fSlowControl.print();
	  for(AMSSetupR::Scalers_i i=getsetup()->fScalers.begin();i!=getsetup()->fScalers.end();i++){
        
	    //cout<< "FT " <<i->first<<" "<<i->second.FTtrig(0)<<endl; 
	  }
	}     
	//#pragma omp atomic 
	//_Lock-=0x100000000;
	//#pragma omp atomic 
	//_Lock+=(1<<thr);
	//cout <<" Lock "<<_Lock<<" "<<omp_get_thread_num()<<endl;
	if(!initdone[thr] || nthr==1){
	  InitDB(local_pfile);
	  initdone[thr]=1;
	  cout <<"  InitDB Init done "<<thr<<endl;
	}

      }
      //cout <<" LockAfter "<<_Lock<<" "<<thr<<endl;

#ifndef _PGTRACK_
      TrTrackFitR::InitMF(UTime());
#endif

    }
#ifdef _PGTRACK_
    if(!AMSEventR::Head()->nMCEventgC()){ // not active for MC events!!
      // update the tracker Databases
      if(TrExtAlignDB::GetHead()){
	TrExtAlignDB::GetHead()->UpdateTkDBc(UTime());
	TrExtAlignDB::GetHead()->UpdateTkDBcDyn(fHeader.Run,UTime(),3);
      }
      if(TrInnerDzDB::GetHead()) TrInnerDzDB::GetHead()->UpdateTkDBc(UTime());
      if(TRFITFFKEY.magtemp && Version()>=800 ) MagField::GetPtr()->UpdateMagTemp(UTime());
    }
#endif
#ifdef _PGTRACK_
    // Fix trdefaultfit
    if(Version()>700 && Version()<714){
      int fix=0;
      for(unsigned int i=0;i<NTrTrack();i++){
	TrTrackR *trk=pTrTrack(i);
	if (trk->Gettrdefaultfit()==7) { trk->Resettrdefaultfit(); fix=1; }
      }
      if (fix) {
	TofRecH::ReBuild();
	fHeader.BetaHs = NBetaH();
      }
    }
#endif
    if(Version()<160){
      // Fix rich rings
      NRichHit();
      for(unsigned int i=0;i<NRichRing();i++){
	RichRingR *ring=pRichRing(i);
	ring->FillRichHits(i);
      }
    }

    // Build corrections for each PMT
    if (RichRingR::loadPmtCorrections && AMSEventR::Head()->nMCEventgC()) {
      cout << "RICH PMT Corrections disabled for MC." << endl;
      RichRingR::loadPmtCorrections=false;
    }
    if (RichRingR::loadPmtCorrections) {
      RichPMTCalib::useRichRunTag=RichRingR::useRichRunTag;
      RichPMTCalib::usePmtStat=RichRingR::usePmtStat;
      RichPMTCalib::useSignalMean=RichRingR::useSignalMean;
      RichPMTCalib::useGainCorrections=RichRingR::useGainCorrections;
      RichPMTCalib::useEfficiencyCorrections=RichRingR::useEfficiencyCorrections;
      RichPMTCalib::useBiasCorrections=RichRingR::useBiasCorrections;
      RichPMTCalib::useTemperatureCorrections=RichRingR::useTemperatureCorrections;

      RichConfigManager::useExternalFiles=RichRingR::useExternalFiles;
      RichConfigManager::reloadTemperatures=RichRingR::reloadTemperatures;
      RichConfigManager::reloadRunTag=RichRingR::reloadRunTag;
      RichConfigManager::reloadPMTs=RichRingR::reloadPMTs;

      // Initialize if it has not yet done and the directory name is the new one
      if(!RichPMTCalib::getHead() && RichRingR::useExternalFiles && RichRingR::correctionsDir!=RichPMTCalib::currentDir){
	RichPMTCalib::Init(RichRingR::correctionsDir);
      }
      RichPMTCalib::currentDir=RichRingR::correctionsDir;
      if (!RichPMTCalib::buildCorrections()) {
	cout<<"*********************** Failed to build RICH PMT corrections. Skiping."<<endl; 
	RichRingR::loadPmtCorrections=false;
      }
    }
    // Set corrections flag
    RichPMTCalib::loadPmtCorrections = RichRingR::loadPmtCorrections;
    // Set default flag value
    RichRingR::pmtCorrectionsFailed = -1;
    // Rich Uniformity Beta Correction Loading. Only once per run
#pragma omp critical(rd)
    if(fHeader.Run!=runo && nMCEventgC()){
#ifdef _PGTRACK_
      if(RichRingR::shouldLoadCorrection!=-1){
	TString filename;
	if(Version()<658) filename=Form("%s/v5.00/RichDefaultTileCorrectionMC.root",getenv("AMSDataDir"));
	else filename=Form("%s/v5.00/RichDefaultTileCorrectionMC_v2.root",getenv("AMSDataDir"));
	
	if(!RichRingTables::Load(filename)) cout<<"Problem loading "<<filename<<endl;
	else cout<<"Rich Default Refractive Index correction loaded"<<endl; 
      }
#endif

      // Disable corrections for MC
      RichRingR::shouldLoadCorrection=-1;
      RichRingR::loadChargeUniformityCorrection=false;      

    }

    // Charge corrections
#pragma omp critical(rd)
    if(RichRingR::loadChargeUniformityCorrection && !RichChargeUniformityCorrection::getHead()){
      if(!RichChargeUniformityCorrection::Init()) cout<<"*********************** Failed to load RICH charge uniformity corrections. Skiping."<<endl; 
      RichRingR::loadChargeUniformityCorrection=false;
    }
    
    // Rich Default Beta Correction Loading. Only once per run
    if(fHeader.Run!=runo && !nMCEventgC()){

      ///////////// BEGIN FIX
      AMSSetupR::TDVR repo;
      bool should_fix=false;
      getsetup()->getTDV("RichRadTilesParameters",UTime(),repo);
      if(repo.FilePath.Contains(".RichRadTilesParameters") || !repo.FilePath.Contains("RichRadTilesParameters")) should_fix=true;

      // Fix in the case the RichRadtileParameters was not present during production
#pragma omp critical(rd)
      if(should_fix){
	cout<<"################## RICH forcing tile corrections for run "<<fHeader.Run<<endl;
	TString filename=Form("%s/v5.00/RichRadTileParametersFix.root",getenv("AMSDataDir"));
	if(!RichRingTables::Load(filename)) cout<<"Problem loading "<<filename<<endl;

	if(RichBetaUniformityCorrection::getHead()){
	  RichBetaUniformityCorrection *p=RichBetaUniformityCorrection::getHead();
	  if(p->_agl) delete p->_agl;
	  if(p->_naf) delete p->_naf;
	  RichBetaUniformityCorrection::_head=0; // This leaks a bit so should not be used too much
	  RichRingR::shouldLoadCorrection=RichRingR::fullUniformityCorrection;
	}else{
	  if(RichRingR::shouldLoadCorrection<0)
	    RichRingR::shouldLoadCorrection=RichRingR::tileCorrection; // Default for other runs
	}
	/////////////// END FIX

      }else{

	if(RichRingR::shouldLoadCorrection==RichRingR::fullUniformityCorrection && 
	   !RichBetaUniformityCorrection::getHead()){
	  if(!RichBetaUniformityCorrection::Init()) cout<<"*********************** Failed to load RICH velocity uniformity corrections. Skiping."<<endl; 
	  if(RichRingR::isCalibrating()) 
	    cout<<"RICH Uniformity Corrections disable RICH dynamic calibration. If the latter is required, "<<endl
		<<"consider setting RichRingR::shouldLoadCorrection=RichRingR::tileCorrection before starting"<<endl
		<<"the event loop"<<endl; 
	  RichRingR::shouldLoadCorrection=RichRingR::tileCorrection;
	}

	if(RichRingR::shouldLoadCorrection==RichRingR::tileCorrection){
	  RichRingR::shouldLoadCorrection=-1; // Done
#ifndef _PGTRACK_
	  TString filename=Form("%s/v4.00/RichDefaultTileCorrection.root",getenv("AMSDataDir"));
#else
	  TString filename=Form("%s/v5.00/RichDefaultTileCorrection.root",getenv("AMSDataDir"));
#endif
	  if(!RichRingTables::Load(filename)) cout<<"Problem loading "<<filename<<endl;
	  else cout<<"Rich Default Refractive Index correction loaded"<<endl; 
	}
      }
    }
    // Rich Dynamic Calibration
    RichRingR::_isCalibrationEvent=false;
    if(!RichBetaUniformityCorrection::getHead())
      if(RichRingR::isCalibrating() && RichRingR::calSelect(*this)){
#pragma omp critical (rd)
	RichRingR::updateCalibration(*this); 
      }

if(TofRecH::RebuildBetaHInReadHeader){
    RebuildBetaH();
}
if(MCEventgR::Rebuild){
    RebuildMCEventg();
}
     
    if(fHeader.Run!=runo){
      cout <<"AMSEventR::ReadHeader-I-NewRun "<<fHeader.Run<<endl;
      runo=fHeader.Run;
      if(!UpdateSetup(fHeader.Run)){
	cerr<<"AMSEventR::UpdateSetup-E-UnabletofindSetupEntryfor "<<fHeader.Run<<endl;
      }
//
// tle bug
//
      if(runo>1366721606 && runo<1371032409 && getsetup() && Version()<653 ){
       getsetup()->fISSData.clear();
      }
      
      if(evento>0){
#pragma omp critical (rd) 
	if(pService)(*pService).TotalTrig+=(int)dif/2; 
      }
      dif=0;
      difm=0;
      dif2=0;
    }
    else if(_Entry<probe){
      if(difm==0){
#pragma omp critical (rd) 
	if(pService)(*pService).TotalTrig+=Event()-evento;
      }
      if(Event()-evento>difm)difm=Event()-evento;
      dif+=Event()-evento;
      dif2+=(Event()-evento)*(Event()-evento);
#pragma omp critical (rd)
      if(pService)(*pService).TotalTrig+=Event()-evento;
    }
    else if(_Entry==probe){
      dif/=probe-1;
      dif2/=probe-1;
      dif2=sqrt(dif2-dif*dif);
      dif2=difm+20*dif2+1;
#pragma omp critical (rd) 
      if(pService)(*pService).TotalTrig+=Event()-evento;
    }
    else{
      if(Event()-evento<dif2 || dif2!=dif2){
	if(pService)(*pService).TotalTrig+=Event()-evento;
      }
      else{
	static int ntotm=0;
	if(Event()-evento>2 && ntotm++<50)cerr<<"HeaderR-W-EventSeqSeemsToBeBroken "<<Event()<<" "<<evento<<" "<<dif2<<endl;
#pragma omp critical (rd) 
	if(pService)(*pService).TotalTrig++;
      }
    }

    if(_Entry==0 && bStatus &&  !UProcessStatus(fStatus))return false;
    if(!AMSEventR::Head()->nMCEventgC()){ // not for MC events
     fHeader.getISSTLE();
     fHeader.getISSAtt();
    }

    evento=Event();
  }
  else{
#pragma omp critical (rd) 
    if(pService)(*pService).BadEv++;    
  }
  if(pService){
#pragma omp critical (rd) 
    (*pService).TotalEv++;
  }
  return i>0;
}

#ifdef __root__new
#include <TStreamerInfo.h>
#include <TMatrix.h>
#include <TCanvas.h>
#endif
AMSEventR::AMSEventR():TSelector(){
  if(_Count++)cerr<<"AMSEventR::ctor-W-OnlyOneSingletonAllowed "<<this<<" "<<_Count<<endl;
  else{ // cout<<"AMSEventR::ctor-I-SingletonInitialized "<<this<<endl;
#ifdef _OPENMP
    cout <<"  thread "<<omp_get_thread_num()<<endl;
#endif
#ifdef __root__new
    if(gROOT==0)
    {
      ROOT::GetROOT();
    }
    TStreamerInfo::SetFactory(new TStreamerInfo()); 
#endif
     if(gEnv){
         gEnv->SetValue("TFile.Recover", 0);
         gEnv->SetValue("XNet.RootdFallback", 0);
     }
     else cerr<<"AMSEventR::ctor-E-gEnvNotDefined"<<endl;
  }
  fEcalHit.reserve(MAXECHITS);
  fEcalCluster.reserve(MAXECCLUST);
  fEcal2DCluster.reserve(MAXEC2DCLUST);
  fEcalShower.reserve(MAXECSHOW);

  fRichHit.reserve(MAXRICHITS);
  fRichRing.reserve(MAXRICHRIN);
  fRichRingB.reserve(MAXRICHRINLIP);

  fTofRawCluster.reserve(MAXTOFRAW);
  fTofRawSide.reserve(MAXTOFRAWS);
  fTofCluster.reserve(MAXTOF);
  fTofClusterH.reserve(MAXTOFH);
  fAntiRawSide.reserve(MAXANTIRS);
  fAntiCluster.reserve(MAXANTICL);

  fTrRawCluster.reserve(MAXTRRAW);
  fTrCluster.reserve(MAXTRCL);
  fTrRecHit.reserve(MAXTRRH02);
  fTrTrack.reserve(MAXTRTR02);

  fTrdRawHit.reserve(MAXTRDRHT );
  fTrdCluster.reserve(MAXTRDCL );
  fTrdSegment.reserve(MAXTRDSEG);
  fTrdTrack.reserve(MAXTRDTRK);
  fTrdHSegment.reserve(10);
  fTrdHTrack.reserve(4);

  fLevel1.reserve(MAXLVL1);
  fLevel3.reserve(MAXLVL3);

  fBeta.reserve(MAXBETA02);
  fBetaB.reserve(MAXBETA02);
  fBetaH.reserve(MAXBETAH);
  fEcalH.reserve(MAXECALH);
  fTofChargeH.reserve(MAXBETAH);
  fCharge.reserve(MAXCHARGE02);
  fVertex.reserve(2);
  fParticle.reserve(MAXPART02);

  fAntiMCCluster.reserve(MAXANTIMC);
  fTofMCCluster.reserve(MAXTOFMC);
  fTofMCPmtHit.reserve(MAXTOFMCPMT);
  fEcalMCHit.reserve(MAXECALMC);
  fTrMCCluster.reserve(MAXTRCLMC);
  fTrdMCCluster.reserve(MAXTRDCLMC);
  fRichMCCluster.reserve(MAXRICMC);

  fMCTrack.reserve(MAXMCVOL);
  fMCEventg.reserve(MAXMCG02);
}

void AMSEventR::clear(){
  fEcalHit.clear();
  fEcalCluster.clear();
  fEcal2DCluster.clear();
  fEcalShower.clear();

  fRichHit.clear();
  fRichRing.clear();
  fRichRingB.clear();

  fTofRawCluster.clear();
  fTofRawSide.clear();
  fTofCluster.clear();
  fTofClusterH.clear();
  fAntiRawSide.clear();
  fAntiCluster.clear();

  fTrRawCluster.clear();
  fTrCluster.clear();
  fTrRecHit.clear();
  fTrTrack.clear();

  fTrdRawHit.clear();
  fTrdCluster.clear();
  fTrdSegment.clear();
  fTrdTrack.clear();
  fTrdHSegment.clear();
  fTrdHTrack.clear();

  fLevel1.clear();
  fLevel3.clear();

  fBeta.clear();
  fBetaB.clear();
  fBetaH.clear();
  fEcalH.clear();
  fTofChargeH.clear();
  fCharge.clear();
  fVertex.clear();
  fParticle.clear();

  fAntiMCCluster.clear();
  fTofMCCluster.clear();
  fTofMCPmtHit.clear();
  fEcalMCHit.clear();
  fTrMCCluster.clear();
  fTrdMCCluster.clear();
  fRichMCCluster.clear();

  fMCTrack.clear();
  fMCEventg.clear();
  fDaqEvent.clear();

}



//------------- AddAMSObject 
#ifndef __ROOTSHAREDLIBRARY__

void AMSEventR::AddAMSObject(DAQEvent *ptr){
  if(ptr)fDaqEvent.push_back(DaqEventR(ptr));
}


void AMSEventR::AddAMSObject(AMSEcalHit *ptr)
{
  fEcalHit.push_back(EcalHitR(ptr));
  ptr->SetClonePointer(fEcalHit.size()-1);
}

void AMSEventR::AddAMSObject(Ecal1DCluster *ptr) 
{
  if (ptr) {
    fEcalCluster.push_back(EcalClusterR(ptr));
    ptr->SetClonePointer(fEcalCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- Ecal1DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSEcal2DCluster *ptr) {
  if (ptr) {
    fEcal2DCluster.push_back(Ecal2DClusterR(ptr));
    ptr->SetClonePointer(fEcal2DCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- (Ecal2DCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSEcalShower *ptr) 
{
  if (ptr) {
    fEcalShower.push_back(EcalShowerR(ptr));
    ptr->SetClonePointer(fEcalShower.size()-1);
  }  else {
    cout<<"AddAMSObject -E- EcalShower ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRawEvent *ptr, float x, float y, float z)
{
  if (ptr) {
    fRichHit.push_back(RichHitR(ptr,x,y,z));
    ptr->SetClonePointer(fRichHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSRichRing *ptr)
{
  fRichRing.push_back(RichRingR(ptr,(fRichHit.size())));
  ptr->SetClonePointer(fRichRing.size()-1);
}

void AMSEventR::AddAMSObject(AMSRichRingNew *ptr)
{
  fRichRingB.push_back(RichRingBR(ptr));
  ptr->SetClonePointer(fRichRingB.size()-1);
}


void AMSEventR::AddAMSObject(AMSVtx *ptr){
  if(ptr){
#ifdef _PGTRACK_
    fVertex.push_back(VertexR(*ptr));
#else
    fVertex.push_back(VertexR(ptr));
#endif
    ptr->SetClonePointer(fVertex.size()-1);
  }else {
    cout<<"AddAMSObject -E- AMSVtx ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTOFCluster *ptr){
  if (ptr) {
    fTofCluster.push_back(TofClusterR(ptr));
    ptr->SetClonePointer(fTofCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTofCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTOFClusterH *ptr){
  if (ptr) {
    fTofClusterH.push_back(TofClusterHR(ptr));
    ptr->SetClonePointer(fTofClusterH.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTofClusterH ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TOF2RawCluster *ptr)
{
  if (ptr) {
    fTofRawCluster.push_back(TofRawClusterR(ptr));
    ptr->SetClonePointer(fTofRawCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTOF2RawCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TOF2RawSide *ptr)
{
  if (ptr) {
    fTofRawSide.push_back(TofRawSideR(ptr));
    ptr->SetClonePointer(fTofRawSide.size()-1);
  }  else {
    cout<<"AddAMSObject -E- TOF2RawSide ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(Anti2RawEvent *ptr)
{
  if (ptr) {
    fAntiRawSide.push_back(AntiRawSideR(ptr));
    ptr->SetClonePointer(fAntiRawSide.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- Anti2RawEvent ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiCluster *ptr)
{
  if (ptr) {
    fAntiCluster.push_back(AntiClusterR(ptr));
    ptr->SetClonePointer(fAntiCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrRawCluster *ptr)
{
  if (ptr) {
#ifdef _PGTRACK_
    //    fTrRawCluster.push_back(*ptr);
    fTrRawCluster.push_back(TrRawClusterR(*ptr));
#else
    fTrRawCluster.push_back(TrRawClusterR(ptr));
#endif
    ptr->SetClonePointer( fTrRawCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- TrRawCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrCluster *ptr){
  if (ptr) {
#ifdef _PGTRACK_
    fTrCluster.push_back(TrClusterR(*ptr));
#else
    fTrCluster.push_back(TrClusterR(ptr));
#endif
    ptr->SetClonePointer(fTrCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTrCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTrRecHit *ptr)
{
  if (ptr) {
#ifdef _PGTRACK_
    // Bug-Fix AO: all TrRecHits for PGTRACK, enjoy!
    fTrRecHit.push_back(TrRecHitR(*ptr));
#else
    if(fTrRecHit.size()>root::MAXTRRH02*2 && !ptr->checkstatus(AMSDBc::USED))return;
    fTrRecHit.push_back(TrRecHitR(ptr));
#endif
    ptr->SetClonePointer(fTrRecHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTrRecHit ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTrTrack *ptr)
{
  if (ptr) {
#ifdef _PGTRACK_
    fTrTrack.push_back(TrTrackR(*ptr));
#else
    fTrTrack.push_back(TrTrackR(ptr));
#endif
    ptr->SetClonePointer( fTrTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTrTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDRawHit *ptr)
{
  if (ptr) {
    fTrdRawHit.push_back(TrdRawHitR(ptr));
    ptr->SetClonePointer( fTrdRawHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDRawHit ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDCluster *ptr)
{
  if (ptr) {
    fTrdCluster.push_back(TrdClusterR(ptr));
    ptr->SetClonePointer( fTrdCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDCluster ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDSegment *ptr)
{
  if (ptr) {
    fTrdSegment.push_back(TrdSegmentR(ptr));
    ptr->SetClonePointer(fTrdSegment.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDSegment ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDTrack *ptr)
{
  if (ptr && (!ptr->checkstatus(AMSDBc::DELETED) || ptr->checkstatus(AMSDBc::USED))) {
    fTrdTrack.push_back(TrdTrackR(ptr));
    ptr->SetClonePointer(fTrdTrack.size()-1);
  }  else if(!ptr){
    cout<<"AddAMSObject -E- AMSTRDTrack ptr is NULL "<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDHSegment *ptr)
{
  if (ptr) {
    fTrdHSegment.push_back(TrdHSegmentR(ptr));
    ptr->SetClonePointer(fTrdHSegment.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDHSegment ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSTRDHTrack *ptr)
{
  if (ptr) {
    fTrdHTrack.push_back(TrdHTrackR(ptr));
    ptr->SetClonePointer(fTrdHTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDHTrack ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(Trigger2LVL1 *ptr)
{
  if (ptr) {
    fLevel1.push_back(Level1R(ptr));
    ptr->SetClonePointer(fLevel1.size()-1);
  }  else {
    cout<<"AddAMSObject -E- Trigger2LVL1 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(TriggerLVL302 *ptr)
{
  if (ptr) {
    fLevel3.push_back(Level3R(ptr));
    ptr->SetClonePointer(fLevel3.size()-1);
  }  else {
    cout<<"AddAMSObject -E- TriggerLVL302 ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSBeta *ptr)
{
  if (ptr) {
    fBeta.push_back(BetaR(ptr));
    ptr->SetClonePointer(fBeta.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSBeta ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObjectB(AMSBeta *ptr)
{
  if (ptr) {
    fBetaB.push_back(BetaR(ptr));
    ptr->SetClonePointer(fBetaB.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSBeta ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSBetaH *ptr)
{
  if (ptr) {
    fBetaH.push_back(BetaHR(ptr));
    ptr->SetClonePointer(fBetaH.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSBetaH ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSEcalH *ptr)
{
  if (ptr) {
    fEcalH.push_back(EcalHR(ptr));
    ptr->SetClonePointer(fEcalH.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSEcalH ptr is NULL"<<endl;
  }
}

void AMSEventR::AddAMSObject(AMSCharge *ptr){
  if(!ptr){
    cout<<"AddAMSObject -E- AMSCharge ptr is NULL"<<endl;
    return;
  }
  fCharge.push_back(ChargeR(ptr));
  ptr->SetClonePointer(fCharge.size()-1);
}

void AMSEventR::AddAMSObject(AMSParticle *ptr, float phi, float phigl)
{
  if (ptr) {
    fParticle.push_back(ParticleR(ptr, phi, phigl));
    ptr->SetClonePointer(fParticle.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSParticle ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSAntiMCCluster *ptr)
{
  if (ptr) {
    if( fAntiMCCluster.size()>root::MAXANTIMC*2)return; 
    fAntiMCCluster.push_back(AntiMCClusterR(ptr));
    ptr->SetClonePointer(fAntiMCCluster.size()-1);  
  }  else {
    cout<<"AddAMSObject -E- AMSAntiMCCluster ptr is NULL"<<endl;
  }
}






void AMSEventR::AddAMSObject(AMSRichMCHit *ptr, int _numgen)
{
  fRichMCCluster.push_back(RichMCClusterR(ptr,_numgen));
  ptr->SetClonePointer(fRichMCCluster.size()-1);
}





void AMSEventR::AddAMSObject(AMSTOFMCCluster *ptr)
{
  if (ptr) {
    fTofMCCluster.push_back(TofMCClusterR(ptr));
    ptr->SetClonePointer(fTofMCCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTofMCCluster ptr is NULL"<<endl;
  }
}

void  AMSEventR::AddAMSObject(AMSTOFMCPmtHit *ptr)
{
  if(ptr) {
    if(int(fTofMCPmtHit.size())<=MAXTOFMCPMT){
      fTofMCPmtHit.push_back(TofMCPmtHitR(ptr));
      ptr->SetClonePointer(fTofMCPmtHit.size()-1);
      if(fTofMCPmtHit.size()==MAXTOFMCPMT-1) {
    //   cout<<"AddAMSObject -E- TofMCPmtHit Size Too Large Abandon"<<endl;
      }
    }
   } else {
    cout<<"AddAMSObject -E- AMSTofMCPmtHit ptr is NULL"<<endl;
  }
}



void AMSEventR::AddAMSObject(AMSEcalMCHit *ptr)
{
  if (ptr) {
    fEcalMCHit.push_back(EcalMCHitR(ptr));
    ptr->SetClonePointer(fEcalMCHit.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSEcalMCHit ptr is NULL"<<endl;
  }
}




void AMSEventR::AddAMSObject(AMSTrMCCluster *ptr)
{
  if (ptr) {
    if( fTrMCCluster.size() > root::MAXTRCLMC * 2)return;
#ifdef _PGTRACK_
    fTrMCCluster.push_back(TrMCClusterR(*ptr));
#else
    fTrMCCluster.push_back(TrMCClusterR(ptr));
#endif
    ptr->SetClonePointer( fTrMCCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTrMCCluster ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSTRDMCCluster *ptr)
{
  if (ptr) {
    fTrdMCCluster.push_back(TrdMCClusterR(ptr));
    ptr->SetClonePointer(fTrdMCCluster.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDMCCluster ptr is NULL"<<endl;
  }
}



void AMSEventR::AddAMSObject(AMSmceventg *ptr)
{
  if (ptr) {
    if(fMCEventg.size()>root::MAXMCG02 * 2)return;
    fMCEventg.push_back(MCEventgR(ptr));
    ptr->SetClonePointer( fMCEventg.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmceventg ptr is NULL"<<endl;
  }
}


void AMSEventR::AddAMSObject(AMSmctrack *ptr)
{
  if (ptr) {
    fMCTrack.push_back(MCTrackR(ptr));
    ptr->SetClonePointer(fMCTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSmctrack ptr is NULL"<<endl;
  }
}

#endif

//---------------------------------------------------------------------

#ifdef __USEANTICLUSTERPG__

int AMSEventR::RebuildAntiClusters(int sect, float sect_zguess, float err_sect_zguess){
  
  AntiRecoPG* Acci = AntiRecoPG::gethead();
  Acci->AMSEventRHead()=this;
  int nacc = Acci->BuildAllClusters(sect,sect_zguess,err_sect_zguess);
//    printf("Built %d AntiClusterR\n", nacc);

//   printf("Before: fAntiCluster.size()=%d\n", (int)fAntiCluster.size());
//   printf("Before: nAntiCluster()=%d\n", (int)nAntiCluster());
//   printf("Before: NAntiCluster()=%d\n", (int)NAntiCluster());
  fAntiCluster.clear();
  fHeader.AntiClusters = 0;

  for (int ii=0; ii<nacc; ii++) {
    AntiClusterR* clus = Acci->pAntiClusterPG(ii);
    //    printf("%d) Found one AntiClusterR...\n", ii);
    if (clus) {
      AntiClusterR clusderef = *clus;
      fAntiCluster.push_back(clusderef);
      //      printf("Valid (%p, %d)", clus, clus->sector);
      //      printf(" (%d)\n", clusderef.sector);
    }
  }
  // printf("After: fAntiCluster.size()=%d\n", (int)fAntiCluster.size());
  // printf("After: nAntiCluster()=%d\n", (int)nAntiCluster());
  // printf("After: NAntiCluster()=%d\n", (int)NAntiCluster());
  fHeader.AntiClusters = nacc;
//   printf("After2: fAntiCluster.size()=%d\n", (int)fAntiCluster.size());
//   printf("After2: nAntiCluster()=%d\n", (int)nAntiCluster());
//   printf("After2: NAntiCluster()=%d\n", (int)NAntiCluster());

  return nacc;
}
#endif

//---------------------------------------------------------------------

#ifndef __ROOTSHAREDLIBRARY__
void HeaderR::Set(EventNtuple02* ptr){

  Run=       ptr->Run;
  RunType=   ptr->RunType;
  Event=     ptr->Eventno;
  Raw=       ptr->RawWords%(1<<18);
  Version=   (ptr->RawWords)>>18;
  Time[0]=   ptr->Time[0];
  Time[1]=   ptr->Time[1];
  RNDMSeed[0]= ptr->RNDMSeed[0];
  RNDMSeed[1]= ptr->RNDMSeed[1];
  RadS=      ptr->RadS;
  ThetaS=    ptr->ThetaS;
  PhiS=      ptr->PhiS;
//  ISSEqAsc=        ptr->ISSEqAsc;
//  ISSEqDec=       ptr->ISSEqDec;
//  ISSGalLat=      ptr->ISSGalLat;
//  ISSGalLong=     ptr->ISSGalLong;
//  AMSEqAsc=     ptr->AMSEqAsc;
//  AMSEqDec=    ptr->AMSEqDec;
//  AMSGalLat=   ptr->AMSGalLat;
//  AMSGalLong=  ptr->AMSGalLong;
  Yaw=       ptr->Yaw;
  Pitch=     ptr->Pitch;
  Roll=      ptr->Roll;
  GPSTime.clear();
  for(unsigned int k=0;k<ptr->GPSL;k++)GPSTime.push_back(ptr->GPS[k]);
  VelocityS= ptr->VelocityS;
  VelTheta=  ptr->VelTheta;
  VelPhi=    ptr->VelPhi;
  ThetaM=    ptr->ThetaM;
  PhiM =     ptr->PhiM;
      TrStat=ptr->TrStat;
}
#endif





AntiRawSideR::AntiRawSideR(Anti2RawEvent *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  swid = ptr->_idsoft;
  stat = ptr->_status;
  temp = ptr->_temp;
  adca = ptr->_adca;
  nftdc = ptr->_nftdc;
  for(int i=0;i<nftdc;i++)ftdc[i] = ptr->_ftdc[i];
  ntdct = ptr->_ntdct<int(sizeof(tdct)/sizeof(tdct[0]))?ptr->_ntdct:sizeof(tdct)/sizeof(tdct[0]);
  for(int i=0;i<ntdct;i++)tdct[i] = ptr->_tdct[i];
#endif
}

AntiMCClusterR::AntiMCClusterR(AMSAntiMCCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->_idsoft;
  GtrkID = ptr->_gtrkid;
  for (int i=0; i<3; i++) Coo[i]=ptr->_xcoo[i];
  TOF    = ptr->_tof;
  Edep   = ptr->_edep;
#endif
}


double BetaR::GetTRDBetaCorr(int datamc){
if(datamc!=0)datamc=1;
              static double p[2][3]={{0.522677,-0.16927,0.676221},
                                     {0.522677,-0.16927,0.676221}};
              double betamc1=0;
              for(int k=0;k<int(sizeof(p)/sizeof(p[0][0])/2);k++)betamc1+=p[datamc][k]*pow(fabs(Beta),k);
              double x=(betamc1+fabs(Beta))/2;
              if(x>0.96)x=0.96;
              double norm=1.8377e-1;
              double corr=norm/x/x*(log(x*x/(1-x*x))-x*x+(8*log(0.511e6*2/10/54)+2*log(0.511e6*2/10/6)+4*log(0.511e6*2/10/8))/14.);
              return corr;

}

BetaR::BetaR(AMSBeta *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status   = ptr->_status;
  Pattern  = ptr->getpattern();
  Beta     = ptr->_Beta;
  BetaC    = ptr->_BetaC;
  Error    = ptr->_InvErrBeta;
  ErrorC   = ptr->_InvErrBetaC;
  Chi2     = ptr->_Chi2;
  Chi2S    = ptr->_Chi2Space;
  for(int ii=0;ii<4;ii++)
  Len[ii]  = ptr->_sleng[ii];
  fTrTrack    = -1;
#endif
}

BetaHR::BetaHR(AMSBetaH *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  BetaPar=ptr->getbetapar();
#endif
}

BetaHR::BetaHR(TofClusterHR *phith[4],TrTrackR* ptrack,TrdTrackR *trdtrack,EcalShowerR *show,TofBetaPar betapar){
    BetaPar=betapar;
//--TofClusterHR assamble
    AMSEventR *ev=AMSEventR::Head();
   for(int il=0;il<4;il++){
     fLayer[il]=-1;
     if(phith[il]){
        for(unsigned int ii=0;ii<ev->NTofClusterH();ii++){if(ev->pTofClusterH(ii)==phith[il]){fLayer[il]=ii;break;};}
        phith[il]->Status|=TOFDBcN::USED; phith[il]->Pattern+=1000;
        fTofClusterH.push_back(fLayer[il]);
       }
   }
//--Track assamble
   double mass,emass,rigidity,charge,evrig;
   fTrTrack=-1;fTrdTrack=-1,fEcalShower=-1;
   if(ptrack){
      for(unsigned int ii=0;ii<ev->NTrTrack();ii++)   {if(ev->pTrTrack(ii)==ptrack){fTrTrack=ii;break;}}
      for(unsigned int ii=0;ii<ev->NParticle();ii++)  {
        if(ev->pParticle(ii)->pTrTrack()==ptrack){
          fTrdTrack=ev->pParticle(ii)->iTrdTrack();
          fEcalShower=ev->pParticle(ii)->iEcalShower();
          break;
        }
      }
#ifdef _PGTRACK_
      for(unsigned int ii=0;ii<ev->NCharge();ii++)    {
         if(ev->pCharge(ii)->pBeta()->pTrTrack()==ptrack){
          rigidity=ptrack->GetRigidity();
          evrig=   ptrack->GetErrRinv();
          charge=double(ev->pCharge(ii)->Charge());
          MassReFit(mass,emass,rigidity,charge,evrig,0,1);break;
        }
      }
#endif
    }
   if(trdtrack){//trd betah
      for(unsigned int ii=0;ii<ev->NTrdTrack();ii++)  {if(ev->pTrdTrack(ii)==trdtrack){fTrdTrack=ii;break;}}//Find Trd Index No Tk Index
   }
   if(show){ // ecal betah
      for(unsigned int ii=0;ii<ev->NEcalShower();ii++)  {if(ev->pEcalShower(ii)==show){fEcalShower=ii;break;}}
   }
   fTofChargeH=-1;
 
}


ChargeSubDR::ChargeSubDR(AMSChargeSubD *ptr){

#ifndef __ROOTSHAREDLIBRARY__
#define CP(x) x=ptr->_##x
  CP(ID);
  Status=ptr->getstatus();
  CP(ChargeI);
  CP(Lkhd);
  CP(Prob);
  CP(Q);
#undef CP

  ptr->_addAttr(Attr);
#endif
}

ChargeR::ChargeR(AMSCharge *ptr){
#ifndef __ROOTSHAREDLIBRARY__
#define CP(x) x=ptr->_##x
  Status=ptr->getstatus();
  CP(ChargeI);
  CP(Lkhd);
  CP(Prob);
#undef CP

  // Copy subdetector information
  for(map <TString,AMSChargeSubD*>::iterator i=ptr->_charges.begin();i!=ptr->_charges.end();++i){
    Charges[i->first]=ChargeSubDR(i->second);
  }

  // Add further information here
#endif
}

float ChargeSubDR::getAttr(TString key){
  map<TString,float>::iterator v=Attr.find(key);
  if(v==Attr.end()) {
    cout<<"ChargeSubDR::getAttr-E-Attribute "<<key<<" not found for "<<ID<<endl;
    return INFINITY;} 
  else return v->second;
}

ChargeSubDR* ChargeR::getSubD(TString ID){
  map<TString,ChargeSubDR>::iterator v=Charges.find(ID);
  if(v==Charges.end()){
    //cout<<"ChargeR::getSubD-E-ChargeSubD "<<ID<<" not found"<<endl;
    return 0;} 
  else return &v->second;
}


Ecal2DClusterR::Ecal2DClusterR(AMSEcal2DCluster *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->getstatus();
  Proj   = ptr->_proj;
  Edep   = ptr->_EnergyC;
  Coo    = ptr->_Coo;
  Tan    = ptr->_Tan;
  Chi2   = ptr->_Chi2;
#endif
}

EcalClusterR::EcalClusterR(Ecal1DCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status  = ptr->_status;
  Proj    = ptr->_proj;
  Plane   = ptr->_plane;
  Left    = ptr->_Left;
  Center  = ptr->_MaxCell;
  Right   = ptr->_Right;
  Edep    = ptr->_EnergyC;
  SideLeak = ptr->_SideLeak;
  DeadLeak = ptr->_DeadLeak;
  for (int i=0; i<3; i++)Coo[i] = ptr->_Coo[i];
#endif
}

EcalShowerR::EcalShowerR(AMSEcalShower *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status      = ptr->_status;
  for (int i=0; i<3; i++) {
    Dir[i]   = ptr->_Dir[i];
    EMDir[i] = ptr->_EMDir[i];
    Entry[i] = ptr->_EntryPoint[i];
    Exit[i]  = ptr->_ExitPoint[i];
    CofG[i]  = ptr->_CofG[i];
  }
  ErDir   = ptr->_Angle3DError;
  ErDirE  = ptr->_Angle3DErrorPI;
  Chi2Dir   = ptr->_AngleTrue3DChi2;
  FirstLayerEdep = ptr->_FrontEnergyDep;
  EnergyD   =   ptr->_Energy;
  EnergyC   =   ptr->_EnergyC;
  EnergyE   =   ptr->_EnergyPIC;
  EnergyE2014 =   ptr->_EnergyH;
  for (int il=0;il<18;il++){
    Elayer_corr[il]=ptr->_elayer_corr[il];
  }
  for (int i=0;i<3;i++){
    CofGCR[i]=ptr->_CooNew[i];
  }
  if ( ptr->_KCR[0]==0 ||  ptr->_KCR[0]==-99999. ||
       ptr->_KCR[1]==0 ||  ptr->_KCR[1]==-99999.){
    for (int i=0;i<3;i++){
      DirCR[i]=-999.;
    }
  }
  else{
    DirCR[0]=1./sqrt(1.+1./ptr->_KCR[0]/ptr->_KCR[0]+ptr->_KCR[1]*ptr->_KCR[1]/ptr->_KCR[0]/ptr->_KCR[0]);
    DirCR[1]=1./sqrt(1.+1./ptr->_KCR[1]/ptr->_KCR[1]+ptr->_KCR[0]*ptr->_KCR[0]/ptr->_KCR[1]/ptr->_KCR[1]);
    DirCR[2]=-1./sqrt(1.+ptr->_KCR[0]*ptr->_KCR[0]+ptr->_KCR[1]*ptr->_KCR[1]);
    if (ptr->_KCR[0]>0) DirCR[0]=-DirCR[0];
    if (ptr->_KCR[1]>0) DirCR[1]=-DirCR[1];
  }

  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  ErEnergyE   = ptr->_ErrEnergyPIC;
  ErEnergyE2014 = ptr->_ErrEnergyH;
  DifoSum     = ptr->_DifoSum;
  SideLeak    = ptr->_SideLeak;
  RearLeak    = ptr->_RearLeak;
  S13Leak     = ptr->_S13Leak;
  SideLeakPI  = ptr->_SideLeakPI;
  RearLeakPI  = ptr->_RearLeakPI;
  EnergyA   =   ptr->_EnergyA; //LAPP
  Energy0A[0]   =   ptr->_Energy0A[0]; //LAPP 
  Energy0A[1]   =   ptr->_Energy0A[1]; //LAPP 
  ErEnergyA = ptr->_ErrEnergyA;//LAPP
  SideLeakA    = ptr->_SideLeakA;//LAPP
  RearLeakA    = ptr->_RearLeakA;//LAPP
  S13LeakXA    = ptr->_S13LeakXA;//LAPP
  S13LeakYA    = ptr->_S13LeakYA;//LAPP
  S13LeakXA0    = ptr->_S13LeakXA0;//LAPP March 21 2012
  S13LeakYA0    = ptr->_S13LeakYA0;//LAPP Apr 26 2011
  S35LeakXA    = ptr->_S35LeakXA;//LAPP Apr 27 2012
  S35LeakYA    = ptr->_S35LeakYA;//LAPP Apr 27 2012
  VarLeakXA    = ptr->_VarLeakXA;//LAPP Apr 27 2012
  VarLeakYA    = ptr->_VarLeakYA;//LAPP Apr 27 2012

   NLinLeakA   =ptr->_NLinLeakA;//LAPP

  S13Ra[0]    = ptr->_S13Ra[0];//LAPP
  S13Ra[1]    = ptr->_S13Ra[1];//LAPP
  S35Ra[0]    = ptr->_S35Ra[0];//LAPP
  S35Ra[1]    = ptr->_S35Ra[1];//LAPP
  DeadLeakA    = ptr->_DeadLeakA;//LAPP
  AttLeakA     = ptr->_AttLeakA;//LAPP
  OrpLeakA     = ptr->_OrpLeakA;//LAPP

  S13LeakXPI    = ptr->_S13LeakXPI;
  S13LeakYPI    = ptr->_S13LeakYPI;
  NLinLeak      =ptr->_NLinLeak;
  NLinLeakPI    =ptr->_NLinLeakPI;
  S13R    = ptr->_S13R;
  S13Rpi[0]    = ptr->_S13Rpi[0];
  S13Rpi[1]    = ptr->_S13Rpi[1];
  DeadLeak    = ptr->_DeadLeak;
  AttLeak     = ptr->_AttLeak;
  OrpLeak     = ptr->_OrpLeak;
  DeadLeakPI  = ptr->_DeadLeakPI;
  AttLeakPI   = ptr->_AttLeakPI;
  OrpLeakPI   = ptr->_OrpLeakPI;
  Orp2DEnergy = ptr->_Orp2DEnergy;

  Chi2Profile = ptr->_ProfilePar[4+ptr->_Direction*5];
  Chi2ProfileBack=ptr->_ProfilePar[4+(1-ptr->_Direction)*5];
  for (int i=0; i<4; i++) ParProfile[i] = fabs(ptr->_ProfilePar[i+ptr->_Direction*5])>FLT_MAX?FLT_MAX:ptr->_ProfilePar[i+ptr->_Direction*5];
  Chi2Trans = ptr->_TransFitChi2;
  for (int i=0; i<3; i++) SphericityEV[i] = ptr->_SphericityEV[i];
  Nhits = ptr->_Nhits;


  // LAPP
  S1tot[0] = ptr->_S1tot;		  
  S1tot[1] = ptr->_S1totx;		  
  S1tot[2] = ptr->_S1toty;		  
  S3tot[0] = ptr->_S3tot;		  
  S3tot[1] = ptr->_S3totx;		  
  S3tot[2] = ptr->_S3toty;		  
  S5tot[0] = ptr->_S5tot;		  
  S5tot[1] = ptr->_S5totx;		  
  S5tot[2] = ptr->_S5toty;		  
  ShowerLatDisp[0] = ptr->_ShowerLatDisp;	  
  ShowerLatDisp[1] = ptr->_ShowerLatDispx;	  
  ShowerLatDisp[2] = ptr->_ShowerLatDispy;	  
  ShowerLongDisp = ptr->_ShowerLongDisp;  
  ShowerDepth = ptr->_ShowerDepth;	  
  ShowerFootprint[0] = ptr->_ShowerFootprint; 
  ShowerFootprint[1] = ptr->_ShowerFootprintx; 
  ShowerFootprint[2] = ptr->_ShowerFootprinty; 
  NbLayerX = ptr->_NbLayerX;
  NbLayerY = ptr->_NbLayerY;	
  ZprofileChi2 = ptr->_ZprofileChi2;
  Zprofile[0] = ptr->_Zprofile[0];
  Zprofile[1] = ptr->_Zprofile[1];
  Zprofile[2] = ptr->_Zprofile[2];
  Zprofile[3] = ptr->_Zprofile[3];
  ZprofileChi2v2 = ptr->_ZprofileChi2v2;
  Zprofilev2[0] = ptr->_Zprofilev2[0];
  Zprofilev2[1] = ptr->_Zprofilev2[1];
  Zprofilev2[2] = ptr->_Zprofilev2[2];
  Zprofilev2[3] = ptr->_Zprofilev2[3];
  EnergyF = ptr->_EnergyF;
#endif
}

/// Second pass corrections on shower energy
float  EcalShowerR::GetCorrectedEnergy(int partid,int method){

  //variables
  float energy;
  float alpha;
  float EnergyLayer[18]={0};
  float energyfractionlast2layers = 0; // corrected for anode efficiency
  float depositedenergy = 0; //corrected for andode efficiency
  float edep_xy[2] ={0}; //energy in each side: 0 - x side, 1 - yside

  int n2DCLUSTERs = NEcal2DCluster();
  float kink_lowenergy;
  float kink_highenergy;
  float correction_factor =1;

  if (method == 0 ) return EnergyP(partid); // no correction implemented yet

  if (method == 1 ) return EnergyA; // no correction implemented yet

  if (method == 2){
    //Computation of last 2 layer energy fraction, corrected for anode efficiency
    //Loop on cluster2d and cluster 1d
    for (int i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)
      {
	Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
	//reject cluster NOT USED
	if (!cluster2d || !(cluster2d->Status & 32)) continue;
      
	int nCLUSTERs = cluster2d->NEcalCluster();
	for (int icluster = 0; icluster < nCLUSTERs; ++icluster)
	  {
	    EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);
	    //reject cluster NOT USED
	    if (!cluster || !(cluster->Status & 32)) continue;
	    EnergyLayer[cluster->Plane] += cluster->Edep;
	    edep_xy[cluster->Proj] += cluster->Edep;
	    
	  }//end loop on 1d clusters
      }//end loop on 2d clusters
    
 
    // Deposited energy corrected for anode efficiency (in GeV)
    depositedenergy = (edep_xy[0]/(1+S13LeakXPI) + edep_xy[1]/(1+S13LeakYPI))/1000;
    //depositedenergy is in GeV,  EnergyLayer are in MeV, S13LeakYPI is the correction factor for Y-side
    energyfractionlast2layers = (EnergyLayer[16] +  EnergyLayer[17])/1000/(1+S13LeakYPI)/depositedenergy;

    if (partid==2 ) {
      // electron hypothesis
      //define kink
      kink_highenergy = 850;
      kink_lowenergy = 3;

      if(EnergyE<kink_lowenergy)
	alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -EnergyE)));
      else
	if(EnergyE<kink_highenergy)
	  alpha = 1.037 - 0.0743/EnergyE-0.159/TMath::Power(EnergyE,2)-1.9186e-9*TMath::Power(EnergyE,2.189);  
	else
	  alpha = 0.7628+460/EnergyE-1.988e5/TMath::Power(EnergyE,2)-6.1e-7*TMath::Power(EnergyE,-2.537);
      
      correction_factor = alpha -  0.752 * energyfractionlast2layers -  5.633 * TMath::Power(energyfractionlast2layers,2);
      
      if(correction_factor>0)
	energy = depositedenergy/correction_factor;
      else
	energy = EnergyE;
      
      //second iteration
      if(energy<kink_lowenergy)
	alpha = 1/(1+TMath::Exp(1.581*(-6.131e-1 -energy)));
      else
	if(energy<kink_highenergy)
	  alpha = 1.037 - 0.0743/energy-0.159/TMath::Power(energy,2)-1.9186e-9*TMath::Power(energy,2.189);  
	else
	  alpha = 0.7628+460/energy-1.988e5/TMath::Power(energy,2)-6.1e-7*TMath::Power(energy,-2.537);
      
      correction_factor = alpha -  0.752 * energyfractionlast2layers -  5.633 * TMath::Power(energyfractionlast2layers,2);
      //}//ed else for fraction
    
      //corrected value  
      if(correction_factor>0)
	energy = depositedenergy/correction_factor;
      else
	energy = depositedenergy;
      
      //reconstructed energy never less than deposited energy corrected for anode efficiency
      if(energy<depositedenergy)
	energy =  depositedenergy;
   
      //////
      //It is not an electron, the fraction is too high (checked by MC, 3 sigmas)
      //   if(energyfractionlast2layers>0.2)
      //	energy = -1 * energy;
      //////

      return energy;
    }//end if on partid of electrons
  
  
    if(partid==1){
      // photon hypothesis
      if(EnergyE>700.) alpha = 1.022 + EnergyE * 1.943*TMath::Power(10, -4) - EnergyE*EnergyE* 1.947 * TMath::Power(10, -7) + EnergyE*EnergyE* EnergyE* 4.25 * TMath::Power(10, -11);
      else if(EnergyE>100.) alpha = 1.0755 + EnergyE * 1.2*TMath::Power(10, -5) - EnergyE*EnergyE* 1.5 * TMath::Power(10, -8);
      else alpha = 0.03881*(1-TMath::Exp(-EnergyE/18.74))+1.034;
      
      energy = depositedenergy/(alpha - 1.8*energyfractionlast2layers + 0.5*energyfractionlast2layers*energyfractionlast2layers);
      energy = energy - 0.2*energy/100;
      
      //reconstructed energy never less than deposited energy corrected for anode efficiency
      if(energy<depositedenergy)
	energy =  depositedenergy;
      return energy;
    }//end if on partid of photons
  }//end if of method 2

   if (method == 3) return EnergyE2014; // no correction implemented yet




  cerr << "EcalShowerR::GetCorrectedEnergy>>UNKNOWN PARTICLE ID OR METHOD FOR ENERGY CORRECTION" <<endl;
  return -1.;
}


float EcalShowerR::EnergyP(int partid){
float ec_ec=EnergyC;
float ec_rl=RearLeak;
float phi=atan2(Dir[1],Dir[0]);
float theta=acos(Dir[2]);
bool mcc=false;
#ifdef __ROOTSHAREDLIBRARY__
if(AMSEventR::Head())mcc=AMSEventR::Head()->nMCEventg()>0;
#else
if(AMSJob::gethead())mcc=AMSJob::gethead()->isRealData()?false:true;
#endif
//           ec_ec energyC in gev
//           ec _rl energyC rear leak 
//           phi shower phi angle (rad)
//            theta shower theta angle (rad)
//           mcc true if mc events
             double c_ec_ec=1;
             if(mcc)c_ec_ec=1.04;
             else c_ec_ec=0.985;
             double ecd=ec_ec;
             ec_ec=ec_ec>1650?ec_ec/(0.225+(1-0.225)*ec_ec/1650):ec_ec;
             ec_ec*=c_ec_ec;
             double fe=(2.65+0.25*log10(ecd/(1+ec_rl)))/3.1;
             double c2=1/(1+ec_rl)/(1-fe*ec_rl/(1+ec_rl));
             ec_ec*=c2;        
             double ea=0.0806;
             double eb=812;
             double c3=1;
             double sq=1-4*ea*(1-ea)*ec_ec/eb;
             if(sq>0 && ec_ec>0){
             c3=eb/2/ea/ec_ec*(1-sqrt(sq));
             if(c3<1)c3=1;
             }
              ec_ec*=c3;
              if(cos(theta)>0){
                double nx=cos(phi)*sin(theta); 
                double ny=sin(phi)*sin(theta); 
                double nz=cos(theta);
                phi=atan2(-ny,-nx);
                theta=acos(-nz);
              }
              if(phi>3.1415926)phi=phi-2*3.1415926;
              double cphi=(1+5.45e-3*cos(4*fabs(phi)));
              ec_ec*=cphi;
              double ctheta=(1-1.6e-1*(1+cos(theta)));
              ec_ec*=ctheta;
              return ec_ec;

}


/// Normalise variable as a function of the energy
void EcalShowerR::NormaliseVariableLAPP(){
  float log10E = TMath::Log10(EnergyC);
  
  Ecal2DClusterR *tc2D;
  EcalClusterR *tc1D;
  EcalHitR *thit;
  float etot=0.;

  for(int a=0;a<18;a++)
        EnergyFractionLayer[a]=0.;
  for(int a=0;a<NEcal2DCluster();a++){
    tc2D = pEcal2DCluster(a);
    for(int b=0;b<tc2D->NEcalCluster();b++){
      tc1D = tc2D->pEcalCluster(b);
      for(int c=0;c<tc1D->NEcalHit();c++){
        thit = tc1D->pEcalHit(c);
	if(!isnan(thit->Edep)){
        	etot += thit->Edep;
        	EnergyFractionLayer[thit->Plane] += thit->Edep;
	}
      }
    }
  }


 for(int a=0;a<18;a++){
    if(etot!=0.)
      EnergyFractionLayer[a] = EnergyFractionLayer[a] / etot;
    else
      EnergyFractionLayer[a]=0.;
  }

  //Normalisation not defined outside the energy range ~4.5->~316 GeV
  if(log10E<0.65||log10E>2.5){
    for(int a=0;a<3;a++){
	NZprofile[a]=-20.;
    	NShowerFootprint[a]=-20.;
	NShowerLatDisp[a]=-20.;
	NS3S5[a]=-20.;
	NS1S3[a]=-20.;
	NS1tot[a]=-20.;
    }
    NZprofile[3]=-20.;	
    NZprofileChi2=-20.; 
    NShowerLongDisp=-20.;
    NEnergy3C3=-20.;
    NEnergy3C2=-20.;	
    NS13R=-20.;
    for(int a=0;a<18;a++)
	NEnergyFractionLayer[a]=-20.;
    return;
  }

  if(Zprofile[3]!=0.)
  	NZProfileMaxRatio = (Zprofile[2]/Zprofile[3] - (0.893418+log10E*-0.185014+pow(log10E,2)*0.173813+pow(log10E,3)*-0.0377678))/(0.119254+log10E*0.100735+pow(log10E,2)*-0.178089+pow(log10E,3)*0.102479+pow(log10E,4)*-0.0189757);
  else NZProfileMaxRatio = -20.; 
  NZprofile[3] = (Zprofile[3]-(4.6038+log10E*2.18661+pow(log10E,2)*0.232865+pow(log10E,3)*-0.0756714))/(-0.666721+log10E*2.29757+pow(log10E,2)*-2.6375+pow(log10E,3)*1.25471+pow(log10E,4)*-0.201141);
  NZprofile[2] = (Zprofile[2]-(2.58378+log10E*5.6755+pow(log10E,2)*-3.43793+pow(log10E,3)*1.2026))/(0.088173+log10E*4.29587+pow(log10E,2)*-5.2607+pow(log10E,3)*2.59343+pow(log10E,4)*-0.325017);
  NZprofile[1] = (Zprofile[1]-(0.595077+log10E*-0.33609+pow(log10E,2)*0.274155+pow(log10E,3)*-0.0638039))/(0.155689+log10E*0.118769+pow(log10E,2)*-0.25295+pow(log10E,3)*0.141587+pow(log10E,4)*-0.020465);
  NZprofile[0] = (Zprofile[0]-(0.938884+log10E*0.0879708+pow(log10E,2)*-0.0410125+pow(log10E,3)*0.0104611))/(0.263762+log10E*-0.56278+pow(log10E,2)*0.674824+pow(log10E,3)*-0.345726+pow(log10E,4)*0.0651346);
  NZprofileChi2 = (ZprofileChi2-(6.93363+log10E*-8.81057+pow(log10E,2)*4.10661+pow(log10E,3)*-0.655118))/(5.95343+log10E*-12.4466+pow(log10E,2)*11.8791+pow(log10E,3)*-4.87835+pow(log10E,4)*0.707919);
  NShowerFootprint[2] = (ShowerFootprint[2]-(4.56276+log10E*2.79923+pow(log10E,2)*-3.49012+pow(log10E,3)*1.17603))/(-3.83273+log10E*18.894+pow(log10E,2)*-23.9165+pow(log10E,3)*11.8707+pow(log10E,4)*-1.96426);
  NShowerFootprint[1] = (ShowerFootprint[1]-(4.02813+log10E*0.924191+pow(log10E,2)*-1.26475+pow(log10E,3)*0.479156))/(-0.242628+log10E*5.36033+pow(log10E,2)*-7.88255+pow(log10E,3)*4.13856+pow(log10E,4)*-0.684636);
  NShowerFootprint[0] = (ShowerFootprint[0]-(8.48389+log10E*4.37138+pow(log10E,2)*-5.5595+pow(log10E,3)*1.94879))/(-6.88814+log10E*32.6109+pow(log10E,2)*-42.0799+pow(log10E,3)*21.5081+pow(log10E,4)*-3.65795);
  NShowerLatDisp[2] = (ShowerLatDisp[2]-(-16.96+log10E*124.363+pow(log10E,2)*-106.846+pow(log10E,3)*28.303))/(-39.2136+log10E*181.489+pow(log10E,2)*-210.614+pow(log10E,3)*96.5514+pow(log10E,4)*-14.7641);
  NShowerLatDisp[1] = (ShowerLatDisp[1]-(2.24942+log10E*55.3929+pow(log10E,2)*-57.9721+pow(log10E,3)*17.6346))/(-31.3107+log10E*155.292+pow(log10E,2)*-204.306+pow(log10E,3)*105.489+pow(log10E,4)*-17.9817);
  NShowerLatDisp[0] = (ShowerLatDisp[0]-(-10.6173+log10E*168.02+pow(log10E,2)*-152.836+pow(log10E,3)*41.8257))/(-57.6265+log10E*260.525+pow(log10E,2)*-305.665+pow(log10E,3)*141.884+pow(log10E,4)*-21.8149);
  if(S5tot[2]!=0.) NS3S5[2] = (S3tot[2]/S5tot[2]-(0.900994+log10E*-0.0351425+pow(log10E,2)*0.067597+pow(log10E,3)*-0.0228938))/(-0.0775086+log10E*0.41959+pow(log10E,2)*-0.527143+pow(log10E,3)*0.257775+pow(log10E,4)*-0.0428106);
  else NS3S5[2] = -1.;
  if(S5tot[1]!=0.) NS3S5[1] = (S3tot[1]/S5tot[1]-(0.941223+log10E*-0.0518037+pow(log10E,2)*0.056474+pow(log10E,3)*-0.0177348))/(-0.0378472+log10E*0.254035+pow(log10E,2)*-0.35294+pow(log10E,3)*0.188781+pow(log10E,4)*-0.0335423);
  else NS3S5[1] = -1.;
  if(S5tot[0]!=0.) NS3S5[0] = (S3tot[0]/S5tot[0]-(0.902154+log10E*-0.0214723+pow(log10E,2)*0.056905+pow(log10E,3)*-0.0214067))/(-0.204989+log10E*0.854424+pow(log10E,2)*-1.07686+pow(log10E,3)*0.549502+pow(log10E,4)*-0.0964843);
  else NS3S5[0] = -1.;
  if(S3tot[0]!=0.)  NS1S3[0] = (S1tot[0]/S3tot[0]-(0.767901+log10E*-0.489178+pow(log10E,2)*0.442909+pow(log10E,3)*-0.115515))/(-0.488682+log10E*1.8756+pow(log10E,2)*-2.1674+pow(log10E,3)*1.02595+pow(log10E,4)*-0.169443);
  else NS1S3[0] = -1.;	
  // Not done yet
  NS1S3[1]=-1.;
  NS1S3[2]=-1.;
  NShowerLongDisp = (ShowerLongDisp-(11.1025+log10E*3.25752+pow(log10E,2)*-0.997847+pow(log10E,3)*-0.0267775))/(0.919455+log10E*6.83936+pow(log10E,2)*-9.99839+pow(log10E,3)*4.92373+pow(log10E,4)*-0.706398);
  NS1tot[0] = (S1tot[0]-(0.598873+log10E*-0.342752+pow(log10E,2)*0.372974+pow(log10E,3)*-0.108247))/(-0.360685+log10E*1.74558+pow(log10E,2)*-2.22315+pow(log10E,3)*1.12403+pow(log10E,4)*-0.194263);
  // Not done yet
  NS1tot[1]=-1.;
  NS1tot[2]=-1.;
  NEnergy3C3 = (Energy3C[2]-(0.848462+log10E*-0.184872+pow(log10E,2)*0.205634+pow(log10E,3)*-0.0636298))/(-0.452453+log10E*1.76401+pow(log10E,2)*-2.15624+pow(log10E,3)*1.0807+pow(log10E,4)*-0.187112);
  NEnergy3C2 = (Energy3C[1]-(0.986097+log10E*0.00567924+pow(log10E,2)*0.00639086+pow(log10E,3)*-0.00467772))/(-0.0102002+log10E*0.0664985+pow(log10E,2)*-0.0939351+pow(log10E,3)*0.0506715+pow(log10E,4)*-0.00880054);
  NS13R = (S13R-(0.896068+log10E*-0.519294+pow(log10E,2)*0.362811+pow(log10E,3)*-0.0822381))/(-0.0499018+log10E*0.286575+pow(log10E,2)*-0.306258+pow(log10E,3)*0.132597+pow(log10E,4)*-0.0191218);
  NEnergyFractionLayer[9] = (EnergyFractionLayer[9]-(0.0616903+log10E*-0.0174509+pow(log10E,2)*0.0471086+pow(log10E,3)*-0.0156655))/(0.013873+log10E*0.0383099+pow(log10E,2)*-0.0583929+pow(log10E,3)*0.0279095+pow(log10E,4)*-0.00400579);
  NEnergyFractionLayer[8] = (EnergyFractionLayer[8]-(0.0667993+log10E*0.00561868+pow(log10E,2)*0.0282832+pow(log10E,3)*-0.0124659))/(0.0118501+log10E*0.0572122+pow(log10E,2)*-0.0917456+pow(log10E,3)*0.0487935+pow(log10E,4)*-0.00819485);
  NEnergyFractionLayer[7] = (EnergyFractionLayer[7]-(.0707861+log10E*0.0309512+pow(log10E,2)*0.00591091+pow(log10E,3)*-0.00937915))/(0.00685963+log10E*0.0823392+pow(log10E,2)*-0.128769+pow(log10E,3)*0.072947+pow(log10E,4)*-0.0131891);
  NEnergyFractionLayer[2] = (EnergyFractionLayer[2]-(0.113831+log10E*-0.0664229+pow(log10E,2)*0.0039547+pow(log10E,3)*0.00239836))/(0.0206638+log10E*0.049813+pow(log10E,2)*-0.0841076+pow(log10E,3)*0.0420049+pow(log10E,4)*-0.00681568);
  NEnergyFractionLayer[1] = (EnergyFractionLayer[1]-(0.0815388+log10E*-0.0761681+pow(log10E,2)*0.0241524+pow(log10E,3)*-0.00232803))/(0.0226358+log10E*0.0232661+pow(log10E,2)*-0.0623346+pow(log10E,3)*0.0364453+pow(log10E,4)*-0.00653768);
 

  NEnergyFractionLayer[0] = (EnergyFractionLayer[0]-(0.0263059+log10E*-0.0222291+pow(log10E,2)*0.00459452+pow(log10E,3)*0.00028022))/(-0.0087581+log10E*0.0604421+pow(log10E,2)*-0.0776135+pow(log10E,3)*0.0372794+pow(log10E,4)*-0.00605276);
  NEnergyFractionLayer[17] = (EnergyFractionLayer[17] - (-0.00697372+log10E*0.0399547+pow(log10E,2)*-0.0354845+pow(log10E,3)*0.0133338))/( -0.0225656+log10E*0.107272+pow(log10E,2)*-0.131483+pow(log10E,3)*0.0655059+pow(log10E,4)*-0.00993156);
  NEnergyFractionLayer[16] = (EnergyFractionLayer[16] - (-0.00701472+log10E*0.045764+pow(log10E,2)*-0.0387198+pow(log10E,3)*0.0144802))/( -0.0200684+log10E*0.10307+pow(log10E,2)*-0.126546+pow(log10E,3)*0.0630652+pow(log10E,4)*-0.00952961);
  NEnergyFractionLayer[15] = (EnergyFractionLayer[15] - (-0.00513213+log10E*0.0474593+pow(log10E,2)*-0.037675+pow(log10E,3)*0.0141781))/( -0.0220574+log10E*0.117021+pow(log10E,2)*-0.146629+pow(log10E,3)*0.0747321+pow(log10E,4)*-0.0119933);
  NEnergyFractionLayer[14] = (EnergyFractionLayer[14] - (0.0022702+log10E*0.0359843+pow(log10E,2)*-0.0243196+pow(log10E,3)*0.0101739))/( -0.0102708+log10E*0.0853348+pow(log10E,2)*-0.112035+pow(log10E,3)*0.0595469+pow(log10E,4)*-0.0100892);
  NEnergyFractionLayer[13] = (EnergyFractionLayer[13] - (0.0113565+log10E*0.0244452+pow(log10E,2)*-0.00808283+pow(log10E,3)*0.00494669))/( -0.00151284+log10E*0.0673545+pow(log10E,2)*-0.0931804+pow(log10E,3)*0.0512705+pow(log10E,4)*-0.00922647);
  NEnergyFractionLayer[12] = (EnergyFractionLayer[12] - (0.0268613+log10E*-0.00148632+pow(log10E,2)*0.0187658+pow(log10E,3)*-0.00324979))/( 0.0212245+log10E*-0.00422532+pow(log10E,2)*-0.00604992+pow(log10E,3)*0.00592189+pow(log10E,4)*-0.00107765);
  NEnergyFractionLayer[11] = (EnergyFractionLayer[11] - (0.0400763+log10E*-0.0195611+pow(log10E,2)*0.0407237+pow(log10E,3)*-0.0108408))/( 0.0374668+log10E*-0.052617+pow(log10E,2)*0.0516337+pow(log10E,3)*-0.0247812+pow(log10E,4)*0.00475269);
  NEnergyFractionLayer[10] = (EnergyFractionLayer[10] - (0.0527833+log10E*-0.0297875+pow(log10E,2)*0.0543798+pow(log10E,3)*-0.0161649))/( 0.0285186+log10E*-0.0164225+pow(log10E,2)*0.00842615+pow(log10E,3)*-0.00529222+pow(log10E,4)*0.00193954);
  NEnergyFractionLayer[6] = (EnergyFractionLayer[6] - (0.0838441+log10E*0.0444102+pow(log10E,2)*-0.0192247+pow(log10E,3)*-0.00272435))/( 0.0170528+log10E*0.0608813+pow(log10E,2)*-0.103429+pow(log10E,3)*0.0600095+pow(log10E,4)*-0.0109843);
  NEnergyFractionLayer[5] = (EnergyFractionLayer[5] - (0.099613+log10E*0.052699+pow(log10E,2)*-0.0499309+pow(log10E,3)*0.00688721))/( 0.033449+log10E*0.00958178+pow(log10E,2)*-0.0421113+pow(log10E,3)*0.0292516+pow(log10E,4)*-0.00573698);
  NEnergyFractionLayer[4] = (EnergyFractionLayer[4] - (0.114548+log10E*0.0231941+pow(log10E,2)*-0.0484808+pow(log10E,3)*0.00970349))/( 0.0363273+log10E*0.00567759+pow(log10E,2)*-0.0318358+pow(log10E,3)*0.019821+pow(log10E,4)*-0.00357058);
  NEnergyFractionLayer[3] = (EnergyFractionLayer[3] - (0.131139+log10E*-0.0419316+pow(log10E,2)*-0.015678+pow(log10E,3)*0.00552745))/( 0.0429826+log10E*-0.00488818+pow(log10E,2)*-0.0246421+pow(log10E,3)*0.0153291+pow(log10E,4)*-0.00257683);

  return;
}



TMVA::Reader *ecallappreader;
TMVA::Reader *NESEreaderv2;
TMVA::Reader *NESEreaderv3;


float varBDT[23];
float cEnergyC;
float cMomentum;

float ESENLayerSigma[18],ESENS1tot[3],ESENS3tot[3],ESENS5tot[3],ESENShowerLatDisp[3],ESENShowerLongDisp;
float ESENShowerFootprint[3],ESENZprofile[4],ESENZprofileChi2,ESENZProfileMaxRatio,ESENEnergyFractionLayer[18];
float ESENS1S3[3],ESENS3S5[3],ESENS13R,ESENEnergy3C2,ESENEnergy3C3,ESENShowerMeanNorm,ESENShowerSigmaNorm;
float ESENShowerSkewnessNorm,ESENShowerKurtosisNorm,ESENF2LEneDepNorm,ESENL2LFracNorm,ESENF2LFracNorm,ESENR3cmFracNorm;
float ESENR5cmFracNorm,ESENDifoSumNorm,ESENS3totxNorm,ESENS3totyNorm,ESENS5totxNorm,ESENS5totyNorm;
float ESENEcalHitsNorm,ESENShowerFootprintXNorm,ESENShowerFootprintYNorm;

float nEnergyA=0;
float nEnergyC=0;
float nEnergyD=0;
float nEnergyE=0;


// Declaration of global variables used for ESEv3

float N5EnergyFractionLayer[18]={0};
float N5LayerSigma[18]={0};
float N5ShowerLongDisp=0;
float N5S3S5[3]={0};
float N5ShowerLatDisp[3]={0};
float N5ShowerFootprint[3]={0};
float N5ZProfileMaxRatio=0;
float N5Zprofile=0;
float ED_Nhits=0;
float zprofile[5]={0};
float hcount=0;
float Rear1=0;
float Ed_sqpixl=0;
float Ed_sqpix=0;
float Ed_pixl=0;
float ESE3RearLeak=0;
float ESEv3=0;
float ghostESEv3=-20;




namespace EcalShowerR_ZProf {
  static TMinuit *fMinuit = 0;
#pragma omp threadprivate(fMinuit)
};

float zv1[18],xv1[18],errorzv1[18];
Int_t nbinsv1;
//______________________________________________________________________________
Double_t funcv1(float x,Double_t *par)
{        
  Double_t value= par[0]*par[1]*TMath::Exp(-par[1]*x)*pow(par[1]*x,par[1]*par[2])/TMath::Gamma(par[1]*par[2]+1.);;
  return value;
} 
//______________________________________________________________________________
void fcnv1(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  //  const Int_t nbins = 18;
  Int_t i;
  //calculate chisquare
  Double_t chisq = 0.;
  Double_t delta;
  for (i=0;i<nbinsv1; i++) {
    delta = (zv1[i]-funcv1(xv1[i],par))/errorzv1[i];
    chisq += delta*delta;       
  } 
  f = chisq;
}








//______________________________________________________________________________ ESEv3





Double_t fcnv3(Double_t *x, Double_t *par){ 
	return par[0]*par[1]*TMath::Exp(-par[1]*(x[0]-par[3]))*pow(par[1]*(x[0]-par[3]),par[1]*par[2])/TMath::Gamma(par[1]*par[2]+1.);
}


float EcalShowerR::EcalStandaloneEstimatorV3(){

	
	nEnergyA=EnergyA;
	nEnergyC=EnergyC;
	nEnergyD=EnergyD;
	nEnergyE=EnergyE;

	// Local variables;



	float ShowerFootprint[3]={0};
	float LayerSigma[18];
	 
	float _ShowerLongDisp={0};  

	float bcell_lat[18]={0};
	float bcell2_lat[18]={0};
	float s_cell_w[18]={0};
	float s_cell2_w[18]={0};
	float edep_cell[18][72]={{0}};
	float edep_layer[18]={0};
	int nhitcell[18]={0};
	float bplane;
	float bplane2;
	int cell,plane;

	float EcalEdepFrac[18];

	int npix,npixl; 

	float S1tot[3]={0};
	float S3tot[3]={0};
	float S5tot[3]={0};


	float ShowerLatDisp[3]={0};

	float EnergyDh;
	float S3S5=0,S3S5x=0,S3S5y=0;
	
	float hedepl[18];
	float hedepc[18][72];
	int   hcmax[18];
	float adc_hi, adc_low;

	float edep_h;
	float s1,s3,s5;

	float zv3[18];


	// Variables initialisation
		
		// FractionLayer, LayerSigma
		for (int jj=0; jj<18; jj++){
			zv3[jj]=0.;	 
			EcalEdepFrac[jj]=0.;  
			LayerSigma [jj]=0.;
			bcell_lat[jj]=0.;
			bcell2_lat[jj]=0.;
			s_cell_w[jj]=0.;
			s_cell2_w[jj]=0.;
			
			edep_layer[jj]=0.;
			nhitcell[jj]=0;
			 hedepl[jj]=0;
				
			for (int ii=0; ii<72; ii++) {
				edep_cell[jj][ii]=0.;
				hedepc[jj][ii]=0.;
			}
		}

		
		

		float bcell_latmx=0., bcell_latmy=0.;
		float edep_tot=0., edep_totx=0., edep_toty=0.;
		int contained_flag=1;
		
		float S1=0.;
		float S3=0.;
		float S5=0.; 
		
		int bcell_i=0;
		int nbcellx=0,nbcelly=0;
		int nblayer = 0;

		
		EnergyDh=0.;
		npix=0; npixl=0;
		//	
		// loop on the hits

		//

	// Effective computing
	// ___________________________________________________________
	

			int nhits=AMSEventR::Head()->NEcalHit();
			if (nhits>0) {
			for (int ihit=0; ihit<nhits; ihit++){
				EcalHitR hit=AMSEventR::Head()->EcalHit(ihit);
				cell=hit.Cell;
				plane=hit.Plane;
				//
				adc_hi=hit.ADC[0];
				adc_low=hit.ADC[1];						
				//
				
				edep_layer[plane]      += hit.Edep;
				edep_cell[plane][cell]  =  hit.Edep;
				s_cell_w[plane]        += (cell+1)*hit.Edep;
				s_cell2_w[plane]       += pow((cell+1),2.)*hit.Edep;
				//
				nhitcell[plane]++;			
				//
				if (adc_low>4) npixl+=1;
				if ( (adc_hi>4&&adc_low<=4) || adc_low>4 ) npix+=1; 
				
				EnergyDh		+= hit.Edep;
				
				edep_h=0.;
				if (adc_hi>4)  {
					edep_h=hit.Edep;
					hedepl[plane]		+=	edep_h;
					hedepc[plane][cell]=	edep_h;
				}


				ED_Nhits=EnergyDh/nhits;
				
			}
			//
			//

			
			if (npixl>0) Ed_pixl    =EnergyDh/npixl; 			else  Ed_pixl=1000000;
			if (npixl>0) Ed_sqpixl  =EnergyDh/sqrt(npixl);	else  Ed_sqpixl=1000000;
			if (npix>0)  Ed_sqpix   =EnergyDh/sqrt(npix); 	else  Ed_sqpix=1000000;

			// 
			//position of the cell with the maximum deposit energy
			//
			hcount=0;
			
			for (int il=0; il<18; il++) {
				float emax=0.1;
				for (int ic=0; ic<72; ic++) if (hedepc[il][ic]>emax) {
					hcmax[il]= (ic);
					emax = hedepc[il][ic];
				}
				
				if (hcmax[il]>0 && hcmax[il]<71 && emax>0.1) {
					s1=hedepc[il][hcmax[il]];
					s3=hedepc[il][hcmax[il]-1]+hedepc[il][hcmax[il]]+hedepc[il][hcmax[il]+1];
					s5 = s3;
					if (hcmax[il]>1&&hcmax[il]<70) s5+=hedepc[il][hcmax[il]+2]+hedepc[il][hcmax[il]-2];
					if (s1>4 && s5>4 && s3>4 && (s1/s3>=.99 || s3/s5>=.99))	{
						hcount+=1;				
					}
				}
			}
			
			
			
			// Variance, Footprint, S3/Sy from the hits 

			ShowerLatDisp[2]=0.; 
			ShowerLatDisp[1]=0.;
			bplane2=0.;
			bplane=0.; 
			bcell_latmy=0.;
			bcell_latmx=0.;
			edep_toty=0.; 	
			edep_totx=0.; 
			float shower_depth_op=0.;
			
				for (int jj=0; jj<18; jj++){
					EcalEdepFrac[jj] = edep_layer[jj]/EnergyDh;
					S1=0;S3=0;S5=0; 
					if (edep_layer[jj]!=0.){				
						// Calculate shower longitudinal dispersion
						bplane2+=pow((jj+1),2.)*edep_layer[jj];  
						bplane+=(jj+1)*edep_layer[jj]; 
						// Calculate shower lateral dispersion  
						bcell_lat[jj]=s_cell_w[jj]/edep_layer[jj];
						bcell2_lat[jj]=s_cell2_w[jj]/edep_layer[jj];
						// Calculate shower depth term  
						shower_depth_op+=(jj+1)*(1./pow(edep_layer[jj],2));
						if (((jj/2)%2)==0){
							ShowerLatDisp[2]+=(bcell2_lat[jj]-pow((bcell_lat[jj]),2));
							bcell_latmy+=bcell_lat[jj]*edep_layer[jj];
							edep_toty+=edep_layer[jj];
							nbcelly++;
						}
						else{
							ShowerLatDisp[1]+=bcell2_lat[jj]-pow((bcell_lat[jj]),2);
							bcell_latmx+=bcell_lat[jj]*edep_layer[jj];
							edep_totx+=edep_layer[jj];
							nbcellx++;      
						}				
						bcell_i=(int) (bcell_lat[jj]-1.);
						if ((bcell_lat[jj]-1-bcell_i)>0.5)  bcell_i++;
						S1=edep_cell[jj][bcell_i];
						S3=S1; 
						if (((bcell_i-1)>=0)&&((bcell_i+1)<=71)) S3=edep_cell[jj][bcell_i-1]+edep_cell[jj][bcell_i+1]+S1;	
						S5=S3; 
						if (((bcell_i-2)>=0)&&((bcell_i+2)<=71)) S5=edep_cell[jj][bcell_i-2]+edep_cell[jj][bcell_i+2]+S3;				
						//
						if (((jj/2)%2)==0){
							S1tot[2]+=S1;
							S3tot[2]+=S3;
							S5tot[2]+=S5;               
						}
						else{
							S1tot[1]+=S1;
							S3tot[1]+=S3;
							S5tot[1]+=S5;
						}				
					}
				}
		
		
			_ShowerLongDisp =bplane2/EnergyDh-pow((bplane/EnergyDh),2);
			
			if (contained_flag==1){
				S1tot[0]=S1tot[1]+S1tot[2];
				S3tot[0]=S3tot[1]+S3tot[2];
				S5tot[0]=S5tot[1]+S5tot[2];			
			}
			edep_tot=edep_totx+edep_toty;
			
			//Calculate shower shower Cofg_z
			bplane=bplane/edep_tot;
				//Calculate shower CofG x,y 
				if (edep_totx>0.){
					bcell_latmx=bcell_latmx/edep_totx;
					S1tot[1]=S1tot[1]/edep_totx;
					S3tot[1]=S3tot[1]/edep_totx; 
					S5tot[1]=S5tot[1]/edep_totx;
				}
				if (edep_toty>0.){
					bcell_latmy=bcell_latmy/edep_toty;
					S1tot[2]=S1tot[2]/edep_toty;
					S3tot[2]=S3tot[2]/edep_toty; 
					S5tot[2]=S5tot[2]/edep_toty;
				}
				if (edep_tot>0.){
					S1tot[0]=S1tot[0]/EnergyDh;
					S3tot[0]=S3tot[0]/EnergyDh; 
					S5tot[0]=S5tot[0]/EnergyDh;
				}
		
			//Calculate shower footprint
			float sigmax2=0., sigmaxz=0., sigmay2=0., sigmayz=0., sigmaz2_x=0., sigmaz2_y=0.;
			for (int jj=0; jj<18; jj++){
				if (edep_layer[jj]!=0){
					if (((jj/2)%2)==0){
						for (int cc=0; cc<72; cc++){
							if (edep_cell[jj][cc]>0){
								sigmay2+=pow(((cc+1)-bcell_latmy),2)*edep_cell[jj][cc];
								LayerSigma[jj] += pow(((cc+1)-bcell_latmy),2)*edep_cell[jj][cc];
								sigmayz+=((cc+1)-bcell_latmy)*((jj+1)-bplane)*edep_cell[jj][cc];
								sigmaz2_y+=pow(((jj+1)-bplane),2)*edep_cell[jj][cc];
							}		     
						}
					}
					else{
						for (int cc=0; cc<72; cc++){
							if (edep_cell[jj][cc]>0){
								sigmax2+=pow(((cc+1)-bcell_latmx),2)*edep_cell[jj][cc];
								LayerSigma[jj] += pow(((cc+1)-bcell_latmx),2)*edep_cell[jj][cc];
								sigmaxz+=((cc+1)-bcell_latmx)*((jj+1)-bplane)*edep_cell[jj][cc];
								sigmaz2_x+=pow(((jj+1)-bplane),2)*edep_cell[jj][cc];	
							}
						}
					}
				}
			}
				for (int jj=0; jj<18; jj++){
				if (edep_layer[jj]!=0){
					LayerSigma[jj] =LayerSigma[jj]/(edep_layer[jj]);
				 }
			}	
			if (edep_totx>0.){
				sigmax2=sigmax2/edep_totx;
				sigmaxz=sigmaxz/edep_totx;
				sigmaz2_x=sigmaz2_x/edep_totx;
			}	
			if (edep_toty>0.){
				sigmay2=sigmay2/edep_toty;
				sigmayz=sigmayz/edep_toty;
				sigmaz2_y=sigmaz2_y/edep_toty;
			}	

				
			//	
			//FootPrint
			//	
			ShowerFootprint[1]=TMath::Sqrt(TMath::Abs(sigmax2*sigmaz2_x-pow(sigmaxz,2)));
			ShowerFootprint[2]=TMath::Sqrt(TMath::Abs(sigmay2*sigmaz2_y-pow(sigmayz,2)));
			ShowerFootprint[0]= ShowerFootprint[1] + ShowerFootprint[2];	 
			
			S3S5  = (S5tot[0]==0.)?0.:(S3tot[0]/S5tot[0]); 
			S3S5x = (S5tot[1]==0.)?0.:(S3tot[1]/S5tot[1]); 
			S3S5y = (S5tot[2]==0.)?0.:(S3tot[2]/S5tot[2]); 

			zprofile[0]=1;
			zprofile[4]=0;

			}
		
		// estimate impact correction in X and Y 
		float ratio=1.; 
		float cora,corx=1.,cory=1.; 
		if (S3tot[1]>0.1) ratio=S1tot[1]/S3tot[1];
		if (ratio<=0.5) ratio=0.5; 
		if (ratio>=0.5&&ratio<0.75) corx=1./(0.379*ratio+0.717);
		
		if (S3tot[2]>0.1) ratio=S1tot[2]/S3tot[2];
		if (ratio<0.5) ratio=0.5;
		if (ratio>=0.5&&ratio<0.75) cory=1./(0.3749*ratio+0.7187);
		//
		float log10E = TMath::Log10(EnergyDh/1000.);
		float exprearl= 0.3*log10E+0.4338*log10E*log10E;
		float frac,err ;
		//


		Double_t par0, par1,par2, par3; 
		par0=1.0;
		par1=0.48;
		par2=1.05*(log(EnergyDh/(8.)));
		par3=1.0;
		TH1F *hfitecal = new TH1F("hfitecal","",18,0.,18.);
		if (EnergyDh/1000>100) par0=0.95;
		float zmax=-1;  
		float az=0; 

		for(int a=0;a<18;a++){
			
			cora=1.;
			if (((a/2)%2)==1&&corx>0.7) cora=corx; 
			if (((a/2)%2)==0&&cory>0.7) cora=cory;   

			frac = EcalEdepFrac[a]*cora*(.975);//(shower->EnergyD);
			
			err=frac*0.07;	
			//
			zv3[a]=frac; 
			if (zv3[a]>zmax) {
				zmax=zv3[a];
			}

			az+=zv3[a]; 
			
			//
			if (EnergyDh/1000<45.) err=frac*0.10;
			if (err<.009&&EnergyDh<10000.) err=0.009;
			if (err<.004&&EnergyDh>10000.) err=0.004;
			if(frac) nblayer++;
			hfitecal->SetBinContent(a+1,frac) ;
			hfitecal->SetBinError(a+1,err);
		}

		if(nblayer>5){

			// new bounds for the
			float xmin=3.5;
			float xmax=18.;  
			if (EnergyDh/1000.<=150) xmin=3;
			if (EnergyDh/1000.<=100) xmin=2.8;
			if (EnergyDh/1000.<=80) xmin=2.7;
			if (EnergyDh/1000.<=50) xmin=2.5;
			if (EnergyDh/1000.<=20) xmin=2.;
			if (EnergyDh/1000.<=2) xmin=1.5;

			TF1 *fitf = new TF1("fitf",fcnv3,0.,18.,4);
			fitf->SetParameters(par0,par1,par2,par3);
			fitf->SetParLimits(0,0.999*par0,2.*par0);
			fitf->SetParLimits(1,par1-0.05,par1+0.05);
			fitf->SetParLimits(2,par2-2,par2+2);
			fitf->SetParLimits(3,-1-par3,par3+1);

			hfitecal->Fit("fitf","rQNM","",xmin,xmax);
			zprofile[0]  =fitf->GetParameter(0);
			zprofile[1]  =fitf->GetParameter(1);
			zprofile[2]  =fitf->GetParameter(2);
			zprofile[3]  =fitf->GetParameter(3);
			zprofile[4] = fitf->GetChisquare()/fitf->GetNDF();


						
		}

		delete hfitecal;	
		
		if (exprearl<0.) exprearl=0.; 
		Rear1= zprofile[0]-1-(exprearl/100);
		ESE3RearLeak     = EnergyE/(EnergyD);


	//
	// Monte carlo reweighting 
	//
	float FP[3]={0.};
	float LaD[3]={0.};
	float scor[5]={0.} ; 
	//
	
	if(AMSEventR::Head()->nMCEventgC()){

		TRandom3 R;
		// footprint
		FP[0]=1.5; 
		FP[1]=1.5;
		FP[2]=1.5;
		// Lat disp and long 
		LaD[0]= 0.5; //long
		LaD[1]= 1.; // 
		LaD[2]= 1.5; //
		// s3S5
		scor[0]=R.Gaus(-0.005,0.005);
		scor[1]=R.Gaus(-0.0025,0.0065); 
		scor[2]=R.Gaus(-0.0025,0.007);
		scor[3]=0.8;
		scor[4]=0.8;
		
		// Ed_sqpixl 
		float rndm1= 84.8333  -6.2193*EnergyE      + 0.0314263*EnergyE*EnergyE -0.000109152*EnergyE*EnergyE*EnergyE;
		float rndm2= 6.83278  +1.12944*EnergyE   + 0.0134335*EnergyE*EnergyE -5.76413e-05*EnergyE*EnergyE*EnergyE; 
		if (rndm2<0.) rndm2=0.;
		Ed_sqpixl+=R.Gaus(rndm1,rndm2);
		//Ed_sqpix 
		rndm1= 38.7858  -3.4022*EnergyE   +0.00940488*EnergyE*EnergyE   -3.94327e-05*EnergyE*EnergyE*EnergyE  ;
		rndm2=0.; 
		if (EnergyE<=20) rndm2=40.; 
		Ed_sqpix+=R.Gaus(rndm1,rndm2);
		//Ed_pixl; 
		rndm1 =     -1.44988  -0.472731*EnergyE   +0.00107027*EnergyE*EnergyE  -6.41228e-06*EnergyE*EnergyE*EnergyE  ;
		rndm2 =     -14.5638  +0.934797*EnergyE  -0.0047743*EnergyE*EnergyE   + 6.10559e-06*EnergyE*EnergyE*EnergyE  ;
		if (rndm2<0.) rndm2=0.;
		Ed_pixl+=R.Gaus(rndm1,rndm2);
		//ED_Nhits
		rndm1=      0.21914  -0.44617*EnergyE   + 0.00197538*EnergyE*EnergyE  -4.96836e-06*EnergyE*EnergyE*EnergyE   ; 
		rndm2=1.; 
		ED_Nhits+=R.Gaus(rndm1,rndm2);
	}	
	




	//Normalization
	// ___________________________________________________________
	
	N5ShowerLongDisp = 	LaD[0]+(_ShowerLongDisp- (11.8081+log10E*-0.936491+pow(log10E,2)*7.55407+pow(log10E,3)*-7.29086+pow(log10E,4)*2.23501+pow(log10E,5)*0.238421+pow(log10E,6)*-0.266485+pow(log10E,7)*0.0399966))/( 3.47107+log10E*-0.502491+pow(log10E,2)*-7.74648+pow(log10E,3)*17.2689+pow(log10E,4)*-18.3605+pow(log10E,5)*10.8724+pow(log10E,6)*-3.65372+pow(log10E,7)*0.652625+pow(log10E,8)*-0.0481742);
	N5S3S5[0] = scor[0]+(S3S5 -  0.933152 );
	N5S3S5[1] = scor[1]+(S3S5x - 0.932582 );
	N5S3S5[2] = scor[2]+(S3S5y - 0.925783 );
	N5ShowerLatDisp[1] = LaD[1]+(ShowerLatDisp[1] - (12.5426+log10E*-4.74202+pow(log10E,2)*90.3105+pow(log10E,3)*-174.116+pow(log10E,4)*137.685+pow(log10E,5)*-53.8503+pow(log10E,6)*10.308+pow(log10E,7)*-0.765736))/( 9.5461+log10E*4.01062+pow(log10E,2)*3.40718+pow(log10E,3)*-27.8418+pow(log10E,4)*28.3826+pow(log10E,5)*-12.6456+pow(log10E,6)*2.8295+pow(log10E,7)*-0.307295+pow(log10E,8)*0.0131647);
	N5ShowerLatDisp[2] = LaD[2]+(ShowerLatDisp[2]- (27.5235+log10E*-11.918+pow(log10E,2)*84.0223+pow(log10E,3)*-123.902+pow(log10E,4)*68.2564+pow(log10E,5)*-13.3742+pow(log10E,6)*-0.425371+pow(log10E,7)*0.303984))/( 22.3648+log10E*8.75913+pow(log10E,2)*-84.2654+pow(log10E,3)*190.862+pow(log10E,4)*-225.388+pow(log10E,5)*145.746+pow(log10E,6)*-51.8595+pow(log10E,7)*9.54455+pow(log10E,8)*-0.71059);
	N5ShowerFootprint[0]=FP[0]+(ShowerFootprint[0] - (11.7701+log10E*-2.18588+pow(log10E,2)*0.715416+pow(log10E,3)*-2.20044+pow(log10E,4)*1.69941+pow(log10E,5)*-0.0880537+pow(log10E,6)*-0.207168+pow(log10E,7)*0.042239))/( 5.21709+log10E*-6.77066+pow(log10E,2)*10.1759+pow(log10E,3)*-14.564+pow(log10E,4)*11.7963+pow(log10E,5)*-5.13228+pow(log10E,6)*1.20079+pow(log10E,7)*-0.140656+pow(log10E,8)*0.00633411);
	N5ShowerFootprint[1]=FP[1]+(ShowerFootprint[1] - (4.0103+log10E*0.728578+pow(log10E,2)*0.215779+pow(log10E,3)*-1.86346+pow(log10E,4)*1.23615+pow(log10E,5)*-0.0529401+pow(log10E,6)*-0.136613+pow(log10E,7)*0.026924))/( 1.60146+log10E*-0.912229+pow(log10E,2)*0.130424+pow(log10E,3)*-0.990985+pow(log10E,4)*1.52236+pow(log10E,5)*-0.925456+pow(log10E,6)*0.287532+pow(log10E,7)*-0.046657+pow(log10E,8)*0.0032173);
	N5ShowerFootprint[2]=FP[2]+(ShowerFootprint[2] - (6.57051+log10E*-0.757404+pow(log10E,2)*0.0631734+pow(log10E,3)*-1.7685+pow(log10E,4)*1.37394+pow(log10E,5)*-0.0626767+pow(log10E,6)*-0.161355+pow(log10E,7)*0.0318432))/( 3.13721+log10E*-3.59197+pow(log10E,2)*6.88893+pow(log10E,3)*-10.1701+pow(log10E,4)*6.90775+pow(log10E,5)*-1.9908+pow(log10E,6)*0.0841649+pow(log10E,7)*0.0661749+pow(log10E,8)*-0.00931509);
	N5ZProfileMaxRatio = ( (Zprofile[2]/Zprofile[3]) - (1.0147+log10E*-1.24533+pow(log10E,2)*2.77027+pow(log10E,3)*-2.87956+pow(log10E,4)*1.63222+pow(log10E,5)*-0.506559+pow(log10E,6)*0.0785756+pow(log10E,7)*-0.00452505))/( 0.0523153+log10E*1.21012+pow(log10E,2)*-3.82707+pow(log10E,3)*5.68673+pow(log10E,4)*-4.71951+pow(log10E,5)*2.33578+pow(log10E,6)*-0.693652+pow(log10E,7)*0.11523+pow(log10E,8)*-0.00828294);
	N5Zprofile = (Zprofile[0] - (1.03032+log10E*-0.448632+pow(log10E,2)*1.00227+pow(log10E,3)*-0.835511+pow(log10E,4)*0.250214+pow(log10E,5)*0.0340714+pow(log10E,6)*-0.034056+pow(log10E,7)*0.00507154))/(0.106574+log10E*0.196734+pow(log10E,2)*-1.12027+pow(log10E,3)*2.13524+pow(log10E,4)*-1.88636+pow(log10E,5)*0.818319+pow(log10E,6)*-0.154296+pow(log10E,7)*0.00297922+pow(log10E,8)*0.00178974 );




	// Creating reader if needed
	// ___________________________________________________________

	if (!NESEreaderv3) {
		NESEreaderv3 = new TMVA::Reader( "!Color:!Silent"	);
		NESEreaderv3->AddVariable("N5ShowerLongDisp"							, & N5ShowerLongDisp);
		NESEreaderv3->AddVariable("N5S3S5[0]"									, & N5S3S5[0]);
		NESEreaderv3->AddVariable("N5S3S5[1]"									, & N5S3S5[1]);
		NESEreaderv3->AddVariable("N5S3S5[2]"									, & N5S3S5[2]);
		NESEreaderv3->AddVariable("N5ShowerLatDisp[1]"						, & N5ShowerLatDisp[1]);
		NESEreaderv3->AddVariable("N5ShowerLatDisp[2]"						, & N5ShowerLatDisp[2]);
		NESEreaderv3->AddVariable("N5ShowerFootprint[0]"					, & N5ShowerFootprint[0]);
		NESEreaderv3->AddVariable("N5ShowerFootprint[1]"					, & N5ShowerFootprint[1]);
		NESEreaderv3->AddVariable("N5ShowerFootprint[2]"					, & N5ShowerFootprint[2]);
		NESEreaderv3->AddVariable("N5ZProfileMaxRatio"						, & N5ZProfileMaxRatio);
		NESEreaderv3->AddVariable("N5Zprofile"									, & N5Zprofile);
		NESEreaderv3->AddVariable("ED_Nhits"									, & ED_Nhits);
		NESEreaderv3->AddVariable("zprofile[4]"								, & zprofile[4]);
		NESEreaderv3->AddVariable("hcount"										, & hcount		);
		NESEreaderv3->AddVariable("Rear1"										, & Rear1		);
		NESEreaderv3->AddVariable("Ed_sqpixl"									, & Ed_sqpixl	);
		NESEreaderv3->AddVariable("Ed_sqpix"									, & Ed_sqpix	);
		NESEreaderv3->AddVariable("Ed_pixl"										, & Ed_pixl		);
		NESEreaderv3->AddVariable("RearLeak"									, & ESE3RearLeak);
		NESEreaderv3->AddSpectator("EnergyA"									, & nEnergyA);
		NESEreaderv3->AddSpectator("EnergyC"									, & nEnergyC);
		NESEreaderv3->AddSpectator("EnergyD"									, & nEnergyD);
		NESEreaderv3->AddSpectator("EnergyE"									, & nEnergyE);
		NESEreaderv3->AddSpectator("ESEv3"										, & ghostESEv3);
		
		char name[801]="";
      sprintf(name,"%s/%s/TMVA_ePrej_V3b_BCat.vc.weights.xml",getenv("AMSDataDir"),AMSCommonsI::getversion());
      NESEreaderv3->BookMVA("BCat method", name);

	}


	ESEv3=NESEreaderv3->EvaluateMVA("BCat method");


	// Retrieving ESEv3 value
	// ___________________________________________________________


	return ESEv3;

}






float EcalShowerR::GetESEv3Efficiency(){
	float ESEv3=EcalStandaloneEstimatorV3();
	if (ESEv3<-1) return 1;
	if (ESEv3>1) return 0;
	static const int nbins=1000;
	int bin=int(((nbins-1)*(ESEv3+1)/2));
	float efficiency;
	if(EnergyD<10000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.99999, 0.99999, 0.99999, 0.99999, 0.99999, 0.99999, 0.999969, 0.999969, 0.999969, 0.999959, 0.999949, 0.999949, 0.999938, 0.999938, 0.999928, 0.999918, 0.999918, 0.999918, 0.999918, 0.999908, 0.999898, 0.999887, 0.999877, 0.999846, 0.999816, 0.999816, 0.999816, 0.999816, 0.999816, 0.999795, 0.999734, 0.999693, 0.999652, 0.99957, 0.999467, 0.999354, 0.999303, 0.999201, 0.999006, 0.998842, 0.998657, 0.998493, 0.998165, 0.998001, 0.997704, 0.997264, 0.996833, 0.996433, 0.996105, 0.995603, 0.994988, 0.994435, 0.993625, 0.99301, 0.992149, 0.991288, 0.990274, 0.989146, 0.987804, 0.986297, 0.984985, 0.983305, 0.981439, 0.979267, 0.977442, 0.974819, 0.972472, 0.970073, 0.967101, 0.963801, 0.960388, 0.956914, 0.953173, 0.949094, 0.944789, 0.940096, 0.935227, 0.929744, 0.924169, 0.918665, 0.912331, 0.905147, 0.898024, 0.890799, 0.882661, 0.874585, 0.866345, 0.856957, 0.846852, 0.837402, 0.826938, 0.816003, 0.804821, 0.793445, 0.781915, 0.770282, 0.757389, 0.744086, 0.731286, 0.716958, 0.702035, 0.687349, 0.671894, 0.657422, 0.641403, 0.625179, 0.60872, 0.59308, 0.577246, 0.560745, 0.544429, 0.527713, 0.511919, 0.495327, 0.478488, 0.462008, 0.446019, 0.430144, 0.414719, 0.399028, 0.383184, 0.368169, 0.352775, 0.338693, 0.324704, 0.310704, 0.297493, 0.284067, 0.271574, 0.260351, 0.249498, 0.238675, 0.227955, 0.217644, 0.208287, 0.198756, 0.189255, 0.181066, 0.173216, 0.166103, 0.159001, 0.152513, 0.146005, 0.139702, 0.13425, 0.1289, 0.12397, 0.119194, 0.114428, 0.110175, 0.106137, 0.102406, 0.0989833, 0.0957959, 0.092629, 0.0898414, 0.0867974, 0.0841635, 0.081468, 0.0789468, 0.0764769, 0.0745706, 0.0722338, 0.0698971, 0.0677858, 0.0659103, 0.0639118, 0.061862, 0.0601095, 0.0585824, 0.0569323, 0.055354, 0.0535912, 0.0520744, 0.0505575, 0.048969, 0.0475239, 0.0462325, 0.0449002, 0.0434141, 0.0420408, 0.0405752, 0.039417, 0.0382077, 0.0368651, 0.0356455, 0.0343644, 0.0331755, 0.0322121, 0.0309822, 0.0298856, 0.0286968, 0.0278154, 0.0266982, 0.0255914, 0.0246075, 0.0237876, 0.0228754, 0.0220145, 0.0211844, 0.0202517, 0.0194318, 0.0185402, 0.0178432, 0.0171053, 0.0164289, 0.015691, 0.0149633, 0.0144201, 0.0138154, 0.0131185, 0.0127086, 0.0121244, 0.0114787, 0.0110175, 0.010628, 0.0102078, 0.0096954, 0.00935719, 0.00892674, 0.00854753, 0.00821957, 0.00783012, 0.00738942, 0.00702046, 0.00669249, 0.00643627, 0.00615955, 0.00585209, 0.00562661, 0.0053294, 0.00509367, 0.0048887, 0.00468372, 0.00441725, 0.00428402, 0.00416103, 0.0039868, 0.00385356, 0.00368958, 0.0035051, 0.00341286, 0.00328988, 0.00315664, 0.00298241, 0.00285943, 0.00271594, 0.0026237, 0.00252122, 0.00240848, 0.00228549, 0.00214201, 0.00202927, 0.00193703, 0.00186529, 0.00175255, 0.00165006, 0.00161932, 0.00153733, 0.00143484, 0.00135285, 0.00127086, 0.00120936, 0.00116837, 0.00109663, 0.00106588, 0.00102488, 0.000983889, 0.000932645, 0.000891649, 0.000850654, 0.000809658, 0.000778912, 0.000727668, 0.000655926, 0.000655926, 0.000625179, 0.000573935, 0.000522691, 0.000502193, 0.000491944, 0.000481696, 0.000471447, 0.000450949, 0.000420203, 0.000379207, 0.000348461, 0.000327963, 0.000286968, 0.000245972, 0.000245972, 0.000225475, 0.000225475, 0.000215226, 0.000204977, 0.000204977, 0.000194728, 0.000184479, 0.000143484, 0.000122986, 0.000112737, 0.000112737, 0.000102488, 9.22396e-05, 9.22396e-05, 8.19907e-05, 8.19907e-05, 7.17419e-05, 6.14931e-05, 6.14931e-05, 6.14931e-05, 6.14931e-05, 5.12442e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 4.09954e-05, 3.07465e-05, 2.04977e-05, 2.04977e-05, 2.04977e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 1.02488e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
		efficiency=Eff[bin];
	} else if (EnergyD<20000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999962, 0.999942, 0.999942, 0.999942, 0.999923, 0.999923, 0.999923, 0.999923, 0.999923, 0.999923, 0.999904, 0.999904, 0.999904, 0.999904, 0.999904, 0.999885, 0.999885, 0.999885, 0.999885, 0.999865, 0.999865, 0.999865, 0.999865, 0.999827, 0.999827, 0.999808, 0.999808, 0.999808, 0.999769, 0.99975, 0.99975, 0.999712, 0.999692, 0.999635, 0.999577, 0.9995, 0.9995, 0.999404, 0.999366, 0.999289, 0.999192, 0.999039, 0.998962, 0.998827, 0.998712, 0.998635, 0.998423, 0.99827, 0.998116, 0.997866, 0.997635, 0.997289, 0.996904, 0.996635, 0.996308, 0.99602, 0.995559, 0.995078, 0.994482, 0.994059, 0.993444, 0.992655, 0.991906, 0.991079, 0.990117, 0.989406, 0.988502, 0.987503, 0.986272, 0.985042, 0.9836, 0.982062, 0.980177, 0.978216, 0.976236, 0.974102, 0.971756, 0.968507, 0.965603, 0.962642, 0.959566, 0.956163, 0.952452, 0.948049, 0.944108, 0.939474, 0.934322, 0.929207, 0.924362, 0.918152, 0.911442, 0.904712, 0.897791, 0.890235, 0.882063, 0.873238, 0.864221, 0.854569, 0.844225, 0.833804, 0.822884, 0.811732, 0.799869, 0.787603, 0.775259, 0.761108, 0.747323, 0.732864, 0.718194, 0.703024, 0.687874, 0.672588, 0.655919, 0.638865, 0.622426, 0.606391, 0.58951, 0.572129, 0.554767, 0.53904, 0.52189, 0.504893, 0.48757, 0.470477, 0.453596, 0.437561, 0.421892, 0.406222, 0.389918, 0.374575, 0.359039, 0.3446, 0.32943, 0.31476, 0.301052, 0.286978, 0.273173, 0.260522, 0.24912, 0.236335, 0.225298, 0.213339, 0.200861, 0.190037, 0.179289, 0.168964, 0.158928, 0.149488, 0.140067, 0.131491, 0.123685, 0.116129, 0.108419, 0.100248, 0.093461, 0.0874815, 0.0813674, 0.0757917, 0.0699852, 0.0646594, 0.059622, 0.0555844, 0.0512007, 0.0477591, 0.0442599, 0.0406452, 0.0371652, 0.0340505, 0.030782, 0.0280902, 0.0255331, 0.023322, 0.0212071, 0.0193036, 0.0175348, 0.0158236, 0.0142662, 0.0127858, 0.0117667, 0.0106516, 0.00994021, 0.00861356, 0.00778681, 0.00730615, 0.00661399, 0.00596028, 0.00536425, 0.00501817, 0.00459518, 0.00415297, 0.00365307, 0.00334545, 0.00311473, 0.00273019, 0.00240334, 0.00219184, 0.00205726, 0.00182654, 0.00169195, 0.00155736, 0.00146123, 0.00130742, 0.00123051, 0.00103824, 0.000922882, 0.000845975, 0.000749841, 0.000711388, 0.000653708, 0.000576801, 0.000519121, 0.000499894, 0.000442214, 0.000384534, 0.000365307, 0.000288401, 0.000249947, 0.000249947, 0.000192267, 0.000153814, 0.00011536, 0.00011536, 9.61335e-05, 5.76801e-05, 3.84534e-05, 3.84534e-05, 3.84534e-05, 3.84534e-05, 1.92267e-05, 1.92267e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<28000) {	
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.99997, 0.99997, 0.99997, 0.99997, 0.99997, 0.99997, 0.99997, 0.99997, 0.99994, 0.99994, 0.999909, 0.999909, 0.999909, 0.999879, 0.999879, 0.999819, 0.999788, 0.999788, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999758, 0.999728, 0.999728, 0.999698, 0.999577, 0.999516, 0.999486, 0.999426, 0.999426, 0.999395, 0.999395, 0.999335, 0.999244, 0.999184, 0.999154, 0.999123, 0.999033, 0.998972, 0.998851, 0.99867, 0.998579, 0.998458, 0.998337, 0.998277, 0.998035, 0.997551, 0.99731, 0.996947, 0.996584, 0.996161, 0.995556, 0.994982, 0.994196, 0.993652, 0.992745, 0.991596, 0.990387, 0.98939, 0.988483, 0.987123, 0.985792, 0.984069, 0.982588, 0.980533, 0.978235, 0.975877, 0.972885, 0.970376, 0.967444, 0.963937, 0.96031, 0.956078, 0.951513, 0.947341, 0.942354, 0.937064, 0.931018, 0.924881, 0.918201, 0.911006, 0.903358, 0.895831, 0.885735, 0.877331, 0.86793, 0.858166, 0.847163, 0.836432, 0.825852, 0.814214, 0.802334, 0.789456, 0.776458, 0.76219, 0.748557, 0.733986, 0.719356, 0.705087, 0.689913, 0.674919, 0.659714, 0.643028, 0.627279, 0.610985, 0.594631, 0.578852, 0.561773, 0.5456, 0.528158, 0.510716, 0.495934, 0.479429, 0.462773, 0.446571, 0.431607, 0.415737, 0.397963, 0.381699, 0.367008, 0.351471, 0.336386, 0.320909, 0.305765, 0.291194, 0.278135, 0.264381, 0.251323, 0.238778, 0.226535, 0.214353, 0.201657, 0.189807, 0.178501, 0.168103, 0.156857, 0.147396, 0.138267, 0.128473, 0.120129, 0.112058, 0.104441, 0.0965811, 0.0896889, 0.0830386, 0.0771742, 0.0712494, 0.0656873, 0.060367, 0.0554699, 0.0504217, 0.0466129, 0.0426529, 0.039237, 0.0354282, 0.0324355, 0.0294127, 0.0268432, 0.0242738, 0.0217648, 0.0195278, 0.0177141, 0.0158097, 0.0141773, 0.0126961, 0.0114265, 0.0102476, 0.00925002, 0.0084943, 0.00755721, 0.00680149, 0.00622714, 0.0053505, 0.00468547, 0.00420181, 0.00383906, 0.00353677, 0.00299265, 0.00266014, 0.00235785, 0.00229739, 0.0019951, 0.00175327, 0.00169281, 0.00148121, 0.00133007, 0.00120915, 0.0011487, 0.000967323, 0.000846407, 0.000755721, 0.000695263, 0.000665034, 0.000665034, 0.000604577, 0.000544119, 0.000483661, 0.000392975, 0.000362746, 0.000362746, 0.000332517, 0.000211602, 0.000181373, 0.000151144, 6.04577e-05, 6.04577e-05, 6.04577e-05, 6.04577e-05, 3.02288e-05, 3.02288e-05, 3.02288e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<42000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999985, 0.999985, 0.99997, 0.999955, 0.999955, 0.999955, 0.99994, 0.999925, 0.999911, 0.999911, 0.999881, 0.999881, 0.999881, 0.999881, 0.999866, 0.999851, 0.999836, 0.999821, 0.999791, 0.999747, 0.999717, 0.999657, 0.999657, 0.999627, 0.999613, 0.999598, 0.999568, 0.999523, 0.999478, 0.999434, 0.999434, 0.999374, 0.999285, 0.99924, 0.999121, 0.998942, 0.998867, 0.998733, 0.998569, 0.998376, 0.998063, 0.997809, 0.997675, 0.997362, 0.99705, 0.996528, 0.996036, 0.995574, 0.995068, 0.994516, 0.993801, 0.99316, 0.992281, 0.991476, 0.990523, 0.989599, 0.988571, 0.987542, 0.986171, 0.984622, 0.983251, 0.981582, 0.979644, 0.977484, 0.97504, 0.97267, 0.969586, 0.966755, 0.963551, 0.959885, 0.956145, 0.952255, 0.948113, 0.943568, 0.938665, 0.933703, 0.92807, 0.921886, 0.91521, 0.908579, 0.901381, 0.893618, 0.885616, 0.877166, 0.868732, 0.859135, 0.848511, 0.838288, 0.827797, 0.816621, 0.805207, 0.792883, 0.780664, 0.767327, 0.753617, 0.739506, 0.725349, 0.710716, 0.696634, 0.681076, 0.665638, 0.650722, 0.635329, 0.619518, 0.604006, 0.586899, 0.570566, 0.555486, 0.539571, 0.523671, 0.507741, 0.491707, 0.475837, 0.460414, 0.445959, 0.430745, 0.416439, 0.401597, 0.388126, 0.374149, 0.360663, 0.346491, 0.333408, 0.320056, 0.308373, 0.295856, 0.283279, 0.2717, 0.260286, 0.249184, 0.238321, 0.227622, 0.217295, 0.207266, 0.197878, 0.188639, 0.179057, 0.170206, 0.161056, 0.15216, 0.144232, 0.135545, 0.128079, 0.120598, 0.112894, 0.105354, 0.098261, 0.0921364, 0.0855201, 0.0799171, 0.0741055, 0.0685771, 0.0630635, 0.0576393, 0.0525728, 0.0474317, 0.0432593, 0.039206, 0.0354956, 0.0320086, 0.0287004, 0.025735, 0.0227845, 0.0203854, 0.0180905, 0.015751, 0.0137542, 0.0121895, 0.0107887, 0.0094774, 0.00828528, 0.00722726, 0.00625866, 0.00540927, 0.0047089, 0.00409793, 0.00365089, 0.00314423, 0.00269719, 0.00248856, 0.00227994, 0.00202661, 0.00175839, 0.00160937, 0.00144545, 0.00132624, 0.00111762, 0.000953701, 0.000864291, 0.000789783, 0.000745079, 0.000700374, 0.000685472, 0.000640768, 0.00056626, 0.00056626, 0.000536457, 0.000521555, 0.000461949, 0.000417244, 0.000372539, 0.000342736, 0.000312933, 0.000298031, 0.00028313, 0.00028313, 0.000268228, 0.000253327, 0.000238425, 0.00019372, 0.000178819, 0.000178819, 0.000149016, 8.94094e-05, 7.45079e-05, 5.96063e-05, 5.96063e-05, 5.96063e-05, 5.96063e-05, 5.96063e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 2.98032e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 1.49016e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<65000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999991, 0.999981, 0.999981, 0.999981, 0.999972, 0.999972, 0.999972, 0.999972, 0.999962, 0.999953, 0.999934, 0.999924, 0.999924, 0.999924, 0.999924, 0.999906, 0.999896, 0.999896, 0.999868, 0.99984, 0.99983, 0.999821, 0.999802, 0.999736, 0.999717, 0.999689, 0.99967, 0.999632, 0.999604, 0.999585, 0.999538, 0.999481, 0.999387, 0.999292, 0.999151, 0.999047, 0.998943, 0.99882, 0.998679, 0.998443, 0.998226, 0.997952, 0.997725, 0.997461, 0.997074, 0.996791, 0.996385, 0.995885, 0.995366, 0.994696, 0.994016, 0.99328, 0.992506, 0.991836, 0.991006, 0.989902, 0.98893, 0.987778, 0.986429, 0.984862, 0.983324, 0.981578, 0.979794, 0.97751, 0.975453, 0.972923, 0.970479, 0.96777, 0.964694, 0.961409, 0.957615, 0.953944, 0.949773, 0.945242, 0.94008, 0.934616, 0.929415, 0.923262, 0.916967, 0.910332, 0.90282, 0.895053, 0.886719, 0.878121, 0.868712, 0.85952, 0.849591, 0.839465, 0.828725, 0.817437, 0.805697, 0.793937, 0.781026, 0.768182, 0.754459, 0.740878, 0.726212, 0.711556, 0.697003, 0.681742, 0.666368, 0.651107, 0.63612, 0.620699, 0.605646, 0.589281, 0.572661, 0.556636, 0.540139, 0.524453, 0.508013, 0.492138, 0.475651, 0.459371, 0.443204, 0.428066, 0.412522, 0.397205, 0.38234, 0.367061, 0.352621, 0.338927, 0.325544, 0.31151, 0.298364, 0.285245, 0.272655, 0.260952, 0.249335, 0.237707, 0.226826, 0.215689, 0.205657, 0.19554, 0.185989, 0.17674, 0.16801, 0.158525, 0.150541, 0.142509, 0.135044, 0.127834, 0.12067, 0.113941, 0.107307, 0.101002, 0.0951226, 0.0894977, 0.0839767, 0.0789369, 0.0739821, 0.0690934, 0.064535, 0.0603164, 0.0560977, 0.0521999, 0.0487835, 0.0451405, 0.0417524, 0.038704, 0.0360048, 0.033334, 0.0308235, 0.0283414, 0.0259159, 0.0237075, 0.0217633, 0.0200362, 0.0182714, 0.0167519, 0.0151947, 0.0139961, 0.0127881, 0.0114102, 0.0102399, 0.00929614, 0.00839012, 0.00764454, 0.0068329, 0.00614394, 0.00553049, 0.00501142, 0.00436966, 0.00396383, 0.0035297, 0.00311444, 0.00269918, 0.00249155, 0.00218955, 0.00193473, 0.00164216, 0.00146284, 0.0013024, 0.00118915, 0.00104758, 0.00100983, 0.000858831, 0.00069839, 0.000632326, 0.0005757, 0.000471885, 0.000415259, 0.000339757, 0.000273693, 0.00024538, 0.000188754, 0.000160441, 0.000113252, 9.4377e-05, 7.55016e-05, 7.55016e-05, 6.60639e-05, 6.60639e-05, 6.60639e-05, 5.66262e-05, 4.71885e-05, 4.71885e-05, 4.71885e-05, 4.71885e-05, 2.83131e-05, 2.83131e-05, 1.88754e-05, 1.88754e-05, 1.88754e-05, 1.88754e-05, 1.88754e-05, 1.88754e-05, 1.88754e-05, 9.4377e-06, 9.4377e-06, 9.4377e-06, 9.4377e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<100000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999994, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.999987, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.99998, 0.999974, 0.999967, 0.999967, 0.999961, 0.999954, 0.999954, 0.999948, 0.999935, 0.999935, 0.999928, 0.999909, 0.999896, 0.999889, 0.999876, 0.999837, 0.999824, 0.999798, 0.999759, 0.999713, 0.999687, 0.999661, 0.999615, 0.999511, 0.999381, 0.999277, 0.999185, 0.99899, 0.998886, 0.998703, 0.998566, 0.998338, 0.998149, 0.997862, 0.997602, 0.997302, 0.996944, 0.9965, 0.996064, 0.995588, 0.995008, 0.994395, 0.993757, 0.993085, 0.992362, 0.991521, 0.990694, 0.989703, 0.988654, 0.987422, 0.986099, 0.984835, 0.983238, 0.981479, 0.979641, 0.977653, 0.975496, 0.973111, 0.970524, 0.967806, 0.964802, 0.961406, 0.957855, 0.953827, 0.949976, 0.945674, 0.941047, 0.936205, 0.930848, 0.92527, 0.919385, 0.913005, 0.906247, 0.899352, 0.891994, 0.884402, 0.876659, 0.868096, 0.858972, 0.849607, 0.84004, 0.830017, 0.819173, 0.80842, 0.797152, 0.785571, 0.774004, 0.762136, 0.749963, 0.737339, 0.72407, 0.711252, 0.697077, 0.68315, 0.669126, 0.654326, 0.639812, 0.624941, 0.609828, 0.594845, 0.579628, 0.564359, 0.549415, 0.53468, 0.519372, 0.504806, 0.48942, 0.474131, 0.458686, 0.443729, 0.429059, 0.414057, 0.399224, 0.384881, 0.37083, 0.356792, 0.343028, 0.329708, 0.31683, 0.304018, 0.290821, 0.278491, 0.266323, 0.253922, 0.241774, 0.23033, 0.219531, 0.209, 0.197915, 0.187846, 0.178038, 0.168907, 0.159217, 0.150015, 0.141165, 0.132751, 0.124572, 0.117391, 0.11015, 0.103451, 0.096849, 0.0905275, 0.0842387, 0.0782104, 0.072886, 0.0677637, 0.0627326, 0.0581446, 0.0536414, 0.049477, 0.0457363, 0.0418782, 0.038548, 0.0353025, 0.0325784, 0.0297175, 0.0271693, 0.0248493, 0.022627, 0.0206132, 0.0187168, 0.0169181, 0.0152498, 0.0138356, 0.0125778, 0.01132, 0.0101013, 0.00903907, 0.00813972, 0.00729903, 0.00651048, 0.00580012, 0.00512887, 0.00454234, 0.00398188, 0.00355828, 0.00324546, 0.00289354, 0.00254814, 0.00222229, 0.00188341, 0.0016488, 0.00138812, 0.00119261, 0.00104924, 0.000899345, 0.000808107, 0.000710352, 0.000566978, 0.000443156, 0.00039102, 0.000299782, 0.000254163, 0.000241129, 0.000208544, 0.000169442, 0.000143374, 0.000104272, 8.47209e-05, 5.21359e-05, 5.21359e-05, 4.5619e-05, 4.5619e-05, 3.9102e-05, 2.6068e-05, 1.3034e-05, 1.3034e-05, 1.3034e-05, 1.3034e-05, 1.3034e-05, 1.3034e-05, 6.51699e-06, 6.51699e-06, 6.51699e-06, 6.51699e-06, 6.51699e-06, 6.51699e-06, 6.51699e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<150000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999995, 0.999995, 0.999995, 0.999995, 0.999995, 0.999995, 0.999995, 0.999995, 0.999995, 0.999991, 0.999991, 0.999991, 0.999991, 0.999991, 0.999991, 0.999991, 0.999991, 0.999991, 0.999986, 0.999986, 0.999986, 0.999986, 0.999986, 0.999986, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999977, 0.999972, 0.999972, 0.999972, 0.999967, 0.999963, 0.999958, 0.999944, 0.999944, 0.999935, 0.999921, 0.999912, 0.999907, 0.999893, 0.999884, 0.999874, 0.99986, 0.999842, 0.999823, 0.999809, 0.999791, 0.999772, 0.999767, 0.999744, 0.999702, 0.999656, 0.999609, 0.999581, 0.999521, 0.999451, 0.99939, 0.999335, 0.999209, 0.999093, 0.998972, 0.998851, 0.998716, 0.998539, 0.99832, 0.998115, 0.997864, 0.997585, 0.997203, 0.996775, 0.99631, 0.995858, 0.995319, 0.99477, 0.994104, 0.993308, 0.99228, 0.991405, 0.990312, 0.989186, 0.987734, 0.986105, 0.9845, 0.982727, 0.980703, 0.97818, 0.975491, 0.972745, 0.969437, 0.96617, 0.962522, 0.958487, 0.954127, 0.949953, 0.945016, 0.939748, 0.934081, 0.928013, 0.922052, 0.915425, 0.908394, 0.900907, 0.893271, 0.884778, 0.876012, 0.866598, 0.857245, 0.846849, 0.836491, 0.825462, 0.814117, 0.80247, 0.790711, 0.778259, 0.76583, 0.752684, 0.740138, 0.726993, 0.714056, 0.70011, 0.68616, 0.672488, 0.658104, 0.64427, 0.629738, 0.615717, 0.601543, 0.587276, 0.572734, 0.558793, 0.544707, 0.530398, 0.516196, 0.502431, 0.48889, 0.474986, 0.461431, 0.448192, 0.434911, 0.421766, 0.408778, 0.396019, 0.383427, 0.370649, 0.358513, 0.346288, 0.334539, 0.323124, 0.311919, 0.300704, 0.289224, 0.277991, 0.267526, 0.256907, 0.246586, 0.236451, 0.226511, 0.216804, 0.207381, 0.198163, 0.188959, 0.180336, 0.171834, 0.163281, 0.155454, 0.147507, 0.139922, 0.132662, 0.125277, 0.118014, 0.111159, 0.104649, 0.0985719, 0.0925412, 0.0867477, 0.0811125, 0.0759101, 0.070945, 0.0659193, 0.0611962, 0.0571478, 0.0529644, 0.0490649, 0.0454306, 0.0418941, 0.0387159, 0.0357656, 0.0331551, 0.0305772, 0.0281109, 0.0256074, 0.0234575, 0.0214892, 0.0195766, 0.0177293, 0.016068, 0.0147139, 0.0132481, 0.0120847, 0.0108423, 0.00981391, 0.00884602, 0.00796653, 0.00721269, 0.00642627, 0.00575619, 0.0052071, 0.00466731, 0.00409029, 0.003611, 0.003262, 0.00293626, 0.00265241, 0.00232667, 0.00209401, 0.00182411, 0.00165194, 0.0014658, 0.00130294, 0.0011866, 0.00105166, 0.000939976, 0.00086087, 0.000721269, 0.000623549, 0.000581669, 0.000502562, 0.000437415, 0.000362961, 0.000316428, 0.000274548, 0.000237321, 0.000190787, 0.000162867, 0.000134947, 0.000130294, 0.000102374, 9.77203e-05, 7.44536e-05, 6.98002e-05, 6.51469e-05, 6.04935e-05, 4.18801e-05, 3.72268e-05, 2.79201e-05, 2.32667e-05, 1.86134e-05, 1.86134e-05, 1.86134e-05, 1.396e-05, 9.3067e-06, 4.65335e-06, 4.65335e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else if (EnergyD<200000) {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999995, 0.99999, 0.99999, 0.999986, 0.999986, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999981, 0.999976, 0.999976, 0.999976, 0.999976, 0.999976, 0.999966, 0.999952, 0.999932, 0.999928, 0.999918, 0.999899, 0.999894, 0.999865, 0.999846, 0.999802, 0.999773, 0.999749, 0.999706, 0.999628, 0.999546, 0.99944, 0.999373, 0.999262, 0.999146, 0.998977, 0.998808, 0.998678, 0.998471, 0.998287, 0.998051, 0.997728, 0.997404, 0.996941, 0.996367, 0.99573, 0.99519, 0.9945, 0.993708, 0.992825, 0.99187, 0.990813, 0.989588, 0.988165, 0.9865, 0.984932, 0.983132, 0.981067, 0.978713, 0.976006, 0.973328, 0.97025, 0.967012, 0.963384, 0.959457, 0.955409, 0.950859, 0.945966, 0.940847, 0.93526, 0.929485, 0.923198, 0.916187, 0.908902, 0.901486, 0.89367, 0.885135, 0.876001, 0.867365, 0.857604, 0.847448, 0.836929, 0.826392, 0.815275, 0.803541, 0.791918, 0.779871, 0.767254, 0.754444, 0.741822, 0.728771, 0.715295, 0.701447, 0.68761, 0.673869, 0.659708, 0.645831, 0.631535, 0.617355, 0.602943, 0.588618, 0.574303, 0.559577, 0.545412, 0.531289, 0.51762, 0.503049, 0.489298, 0.475094, 0.461203, 0.447568, 0.434025, 0.420636, 0.407112, 0.394504, 0.381641, 0.368711, 0.356195, 0.343747, 0.331371, 0.319473, 0.307739, 0.296179, 0.284527, 0.272981, 0.261835, 0.251086, 0.240355, 0.230271, 0.220004, 0.209997, 0.200738, 0.191624, 0.182249, 0.173299, 0.164938, 0.156924, 0.148823, 0.141262, 0.133673, 0.126387, 0.119295, 0.112603, 0.105915, 0.0999518, 0.0939786, 0.0884975, 0.0828573, 0.0778008, 0.0726624, 0.0677555, 0.0632056, 0.0587861, 0.054849, 0.0513268, 0.0476358, 0.0441426, 0.0407266, 0.0377352, 0.0348499, 0.0322349, 0.0297308, 0.0273473, 0.0253257, 0.0234006, 0.0213934, 0.019613, 0.0180112, 0.0164286, 0.0149908, 0.0136157, 0.0123661, 0.011266, 0.0103059, 0.00938917, 0.00850622, 0.00774872, 0.0069092, 0.00620959, 0.00549069, 0.00493583, 0.00439545, 0.00390813, 0.00353662, 0.003223, 0.00291421, 0.0026585, 0.00234006, 0.00213741, 0.00196854, 0.00170317, 0.00150053, 0.00133649, 0.00115797, 0.00100357, 0.000858825, 0.000767152, 0.000689955, 0.000593457, 0.000535559, 0.000487311, 0.000424587, 0.000371514, 0.000323265, 0.000275017, 0.000241243, 0.000207469, 0.000183345, 0.00016887, 0.000144746, 0.000110972, 8.20226e-05, 5.30734e-05, 3.85989e-05, 3.85989e-05, 2.89491e-05, 2.89491e-05, 2.41243e-05, 2.41243e-05, 1.92994e-05, 1.92994e-05, 1.92994e-05, 1.44746e-05, 1.44746e-05, 1.44746e-05, 1.44746e-05, 1.44746e-05, 9.64972e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	} else {
		float Eff[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999999, 0.999997, 0.999997, 0.999996, 0.999995, 0.999995, 0.999995, 0.999995, 0.999993, 0.999993, 0.999993, 0.999993, 0.999992, 0.99999, 0.99999, 0.99999, 0.99999, 0.99999, 0.99999, 0.999988, 0.999985, 0.999985, 0.999985, 0.999985, 0.999981, 0.999981, 0.999977, 0.999971, 0.99997, 0.999968, 0.999967, 0.999964, 0.999964, 0.999963, 0.999961, 0.999956, 0.999952, 0.999949, 0.999939, 0.999934, 0.999928, 0.99992, 0.999913, 0.999905, 0.999893, 0.999875, 0.999866, 0.999855, 0.99985, 0.99984, 0.999825, 0.999813, 0.999784, 0.99976, 0.999742, 0.99972, 0.999698, 0.999672, 0.999642, 0.999603, 0.999577, 0.999541, 0.999503, 0.999464, 0.999421, 0.99938, 0.999332, 0.999292, 0.999237, 0.999191, 0.999129, 0.999051, 0.99896, 0.998858, 0.998764, 0.998662, 0.998548, 0.998442, 0.998296, 0.998183, 0.998023, 0.99787, 0.997722, 0.997525, 0.997334, 0.997146, 0.996949, 0.996725, 0.996474, 0.996185, 0.995929, 0.995619, 0.99531, 0.994979, 0.994591, 0.994208, 0.993822, 0.993404, 0.992992, 0.992518, 0.991987, 0.991451, 0.990893, 0.990227, 0.989553, 0.988875, 0.988192, 0.98747, 0.986679, 0.985917, 0.98511, 0.98418, 0.983269, 0.982291, 0.98126, 0.980199, 0.978985, 0.977794, 0.976607, 0.975315, 0.974011, 0.972606, 0.971141, 0.9696, 0.968032, 0.966245, 0.964521, 0.96263, 0.960598, 0.958527, 0.956527, 0.954419, 0.952305, 0.949985, 0.947551, 0.945098, 0.942565, 0.939887, 0.93708, 0.934217, 0.931318, 0.928435, 0.925348, 0.922202, 0.918906, 0.915655, 0.912238, 0.908754, 0.905123, 0.901456, 0.897721, 0.893831, 0.889935, 0.885809, 0.88167, 0.877413, 0.873173, 0.868697, 0.864202, 0.859683, 0.854986, 0.850012, 0.84505, 0.840057, 0.835003, 0.829846, 0.824652, 0.819317, 0.813687, 0.80814, 0.802307, 0.796492, 0.790535, 0.784474, 0.778354, 0.772061, 0.765849, 0.75959, 0.75312, 0.746556, 0.740188, 0.733493, 0.726728, 0.719932, 0.71306, 0.706238, 0.69927, 0.692219, 0.685236, 0.678025, 0.670961, 0.663615, 0.656268, 0.649101, 0.641899, 0.634664, 0.627325, 0.620063, 0.612623, 0.605265, 0.597895, 0.59047, 0.583064, 0.575511, 0.568357, 0.560973, 0.553478, 0.545908, 0.538615, 0.531127, 0.523956, 0.516741, 0.509544, 0.502188, 0.49508, 0.487896, 0.480664, 0.473528, 0.466392, 0.459219, 0.452175, 0.445111, 0.438164, 0.43114, 0.424156, 0.417279, 0.410566, 0.403787, 0.396934, 0.39013, 0.38361, 0.376953, 0.370482, 0.363883, 0.35737, 0.350866, 0.344455, 0.338234, 0.331856, 0.325502, 0.319289, 0.313276, 0.307263, 0.301215, 0.295374, 0.2894, 0.283489, 0.277625, 0.272027, 0.266449, 0.260927, 0.255263, 0.249629, 0.244095, 0.238744, 0.233339, 0.228055, 0.222676, 0.217626, 0.212656, 0.207565, 0.202544, 0.19757, 0.192788, 0.187887, 0.183035, 0.178322, 0.173776, 0.16929, 0.164842, 0.16049, 0.156212, 0.151975, 0.147774, 0.14356, 0.139471, 0.135439, 0.131353, 0.127477, 0.123476, 0.119647, 0.116005, 0.112216, 0.108639, 0.105039, 0.101702, 0.0982739, 0.0950056, 0.0917595, 0.0885849, 0.0855634, 0.082488, 0.0793962, 0.076533, 0.0736947, 0.0708853, 0.0682027, 0.0655366, 0.0629228, 0.0603917, 0.0579103, 0.0555184, 0.0532243, 0.0510886, 0.0488455, 0.0468173, 0.0448222, 0.0428685, 0.0410208, 0.0391566, 0.0373172, 0.0355233, 0.0338726, 0.0322495, 0.0306526, 0.0291866, 0.027773, 0.0264199, 0.0251041, 0.0237593, 0.0225661, 0.0213219, 0.0201701, 0.0190403, 0.0180234, 0.0170231, 0.0160779, 0.0151176, 0.014193, 0.0133374, 0.01253, 0.0117612, 0.0111012, 0.0104371, 0.00978123, 0.00913089, 0.00853842, 0.00799418, 0.00742514, 0.00692498, 0.00643723, 0.00598392, 0.00556506, 0.0051462, 0.00478934, 0.0044132, 0.00407563, 0.00373944, 0.00344596, 0.0031759, 0.00291549, 0.00266748, 0.00244841, 0.00224862, 0.00203644, 0.00187523, 0.00173469, 0.00157762, 0.00143019, 0.00131859, 0.00120423, 0.00108573, 0.000972749, 0.000890079, 0.00080052, 0.00073714, 0.000646203, 0.000586956, 0.000515309, 0.000456062, 0.000410594, 0.000358236, 0.000315523, 0.000282456, 0.000249388, 0.000220453, 0.000192896, 0.000174985, 0.000150184, 0.000135028, 0.000117116, 8.81812e-05, 7.57808e-05, 7.02694e-05, 6.06246e-05, 4.68463e-05, 3.72015e-05, 3.16901e-05, 2.20453e-05, 2.06675e-05, 1.79118e-05, 1.51561e-05, 1.37783e-05, 1.10227e-05, 1.10227e-05, 6.88916e-06, 2.75566e-06, 2.75566e-06, 2.75566e-06, 2.75566e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		efficiency=Eff[bin];
	}
	return efficiency;
}

float EcalShowerR::GetESEv3Rejection(){
	float ESEv3=EcalStandaloneEstimatorV3();
	if (ESEv3<-1) return 1.;
	if (ESEv3>1) return 1e31;
	static const int nbins=1000;
	int bin=int(((nbins-1)*(ESEv3+1)/2));
	float rejection;
	if(EnergyD<10000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999999, 0.999999, 0.999999, 0.999997, 0.999995, 0.99999, 0.999985, 0.999975, 0.99995, 0.999927, 0.999892, 0.999833, 0.99976, 0.999651, 0.999477, 0.999299, 0.999036, 0.998696, 0.998301, 0.997787, 0.997214, 0.996523, 0.995729, 0.994847, 0.993889, 0.992772, 0.991641, 0.990471, 0.989204, 0.987745, 0.986472, 0.985074, 0.983694, 0.98237, 0.980997, 0.979491, 0.977854, 0.976228, 0.974272, 0.972286, 0.970063, 0.967385, 0.964926, 0.961812, 0.958511, 0.955345, 0.951579, 0.947874, 0.944087, 0.939618, 0.93562, 0.931184, 0.926574, 0.921467, 0.915623, 0.909366, 0.902517, 0.894907, 0.887568, 0.878617, 0.869966, 0.861439, 0.85166, 0.841969, 0.833286, 0.823695, 0.815391, 0.807522, 0.799824, 0.792299, 0.785038, 0.777977, 0.771542, 0.765439, 0.759822, 0.754649, 0.749481, 0.744322, 0.739371, 0.73443, 0.729641, 0.724796, 0.720035, 0.715311, 0.710454, 0.70575, 0.700956, 0.696225, 0.691551, 0.686791, 0.681976, 0.677171, 0.672455, 0.667824, 0.663144, 0.65853, 0.653932, 0.649374, 0.644778, 0.640255, 0.635685, 0.6313, 0.626864, 0.622455, 0.618103, 0.613671, 0.609291, 0.604977, 0.60073, 0.596505, 0.592281, 0.588188, 0.584041, 0.579849, 0.575753, 0.571726, 0.567671, 0.563647, 0.559646, 0.555779, 0.551941, 0.548079, 0.544333, 0.540579, 0.536909, 0.533334, 0.529747, 0.526154, 0.522663, 0.519137, 0.51567, 0.512218, 0.508759, 0.505263, 0.501796, 0.498323, 0.494879, 0.49138, 0.48801, 0.484587, 0.481219, 0.477839, 0.474382, 0.47101, 0.46761, 0.46427, 0.460885, 0.457474, 0.454068, 0.450658, 0.447231, 0.443794, 0.440407, 0.436919, 0.433403, 0.42988, 0.426462, 0.422904, 0.419337, 0.415801, 0.412124, 0.408476, 0.404895, 0.40123, 0.39757, 0.393716, 0.390022, 0.386198, 0.382429, 0.378539, 0.374672, 0.370844, 0.36695, 0.363037, 0.3591, 0.355074, 0.351076, 0.347081, 0.343045, 0.339008, 0.334907, 0.330816, 0.326769, 0.322621, 0.318432, 0.314165, 0.309912, 0.305616, 0.301296, 0.296979, 0.292606, 0.288186, 0.283678, 0.279194, 0.274718, 0.270237, 0.265724, 0.261153, 0.256616, 0.252019, 0.24733, 0.242647, 0.238057, 0.233416, 0.228758, 0.224091, 0.219378, 0.214637, 0.209973, 0.205179, 0.20038, 0.195646, 0.190829, 0.18611, 0.181483, 0.17679, 0.172095, 0.167413, 0.162808, 0.158227, 0.153637, 0.14914, 0.144681, 0.140232, 0.13576, 0.131417, 0.127092, 0.122847, 0.118617, 0.114507, 0.110497, 0.106423, 0.102456, 0.0984918, 0.0947056, 0.0910678, 0.0874069, 0.0838163, 0.0803306, 0.0769257, 0.0735345, 0.0703558, 0.0671241, 0.0640795, 0.0610876, 0.0582497, 0.0554328, 0.0526778, 0.0500369, 0.0474564, 0.0449881, 0.0425885, 0.0402582, 0.0380511, 0.0359202, 0.0338745, 0.0319322, 0.030006, 0.0281588, 0.0264315, 0.0247549, 0.0231724, 0.0216753, 0.0202809, 0.0188803, 0.0175971, 0.0163427, 0.01518, 0.0140943, 0.0130724, 0.0121018, 0.0112106, 0.0103428, 0.00953155, 0.00875444, 0.00807435, 0.00745666, 0.00683312, 0.00625296, 0.0056811, 0.00513654, 0.00466315, 0.0042395, 0.00383485, 0.00346629, 0.00314988, 0.00285201, 0.00257022, 0.00233036, 0.00209635, 0.00189549, 0.00169804, 0.001524, 0.00136263, 0.00121198, 0.00108376, 0.000965782, 0.000857552, 0.000770773, 0.000687894, 0.000606966, 0.000524574, 0.000465584, 0.000415857, 0.000359304, 0.00031689, 0.000276425, 0.000243274, 0.000207197, 0.000177458, 0.000150644, 0.000127243, 0.000111155, 9.60419e-05, 7.70286e-05, 6.87407e-05, 5.89902e-05, 5.07024e-05, 4.09519e-05, 3.41266e-05, 2.82763e-05, 2.29136e-05, 2.1451e-05, 1.95009e-05, 1.75508e-05, 1.41382e-05, 1.21881e-05, 1.07255e-05, 9.26293e-06, 7.80036e-06, 7.31284e-06, 6.82532e-06, 6.33779e-06, 5.36275e-06, 5.36275e-06, 3.90018e-06, 2.92514e-06, 1.95009e-06, 1.95009e-06, 9.75045e-07, 9.75045e-07, 9.75045e-07, 9.75045e-07, 9.75045e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 4.87523e-07, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<20000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999997, 0.999997, 0.999997, 0.999997, 0.999992, 0.99999, 0.99998, 0.999975, 0.999965, 0.999958, 0.999943, 0.999938, 0.999928, 0.999923, 0.999918, 0.999908, 0.99989, 0.999873, 0.999865, 0.99984, 0.999828, 0.999795, 0.999758, 0.999746, 0.999716, 0.999683, 0.999646, 0.999611, 0.999541, 0.999484, 0.999439, 0.999374, 0.999319, 0.999259, 0.999157, 0.999087, 0.99902, 0.998918, 0.998796, 0.998693, 0.998579, 0.998441, 0.998279, 0.998127, 0.997958, 0.997766, 0.997613, 0.997399, 0.997175, 0.996915, 0.996676, 0.996359, 0.996065, 0.995691, 0.995287, 0.99483, 0.994377, 0.993843, 0.993272, 0.992646, 0.991968, 0.991304, 0.990626, 0.989843, 0.98886, 0.987845, 0.98684, 0.985773, 0.984581, 0.983304, 0.982012, 0.980536, 0.979042, 0.977486, 0.975915, 0.974147, 0.972304, 0.970234, 0.968072, 0.965898, 0.963733, 0.961384, 0.958773, 0.95596, 0.953199, 0.950391, 0.947366, 0.944192, 0.940795, 0.937429, 0.933723, 0.929982, 0.926184, 0.922129, 0.917913, 0.913439, 0.90899, 0.904232, 0.899506, 0.894376, 0.889257, 0.883962, 0.878419, 0.87272, 0.867222, 0.861261, 0.855199, 0.849199, 0.843027, 0.836665, 0.830207, 0.823431, 0.816648, 0.80991, 0.802862, 0.795907, 0.788593, 0.781236, 0.773413, 0.765705, 0.758079, 0.750311, 0.742421, 0.734473, 0.726279, 0.718062, 0.70989, 0.701575, 0.693171, 0.684775, 0.676176, 0.667658, 0.658757, 0.650301, 0.641745, 0.63271, 0.624089, 0.615398, 0.606498, 0.597615, 0.588879, 0.580114, 0.571368, 0.56254, 0.55367, 0.544849, 0.536163, 0.527433, 0.518712, 0.510044, 0.50154, 0.492956, 0.484507, 0.475909, 0.467662, 0.459243, 0.451031, 0.442774, 0.43462, 0.426664, 0.418659, 0.410497, 0.402517, 0.394754, 0.387081, 0.379537, 0.372338, 0.364639, 0.35717, 0.349881, 0.342487, 0.335402, 0.32836, 0.321335, 0.314455, 0.307689, 0.301138, 0.294592, 0.28814, 0.281669, 0.275213, 0.268781, 0.262315, 0.256405, 0.250459, 0.244529, 0.238721, 0.233195, 0.227664, 0.222198, 0.216679, 0.21137, 0.206153, 0.201118, 0.196282, 0.19134, 0.186861, 0.182135, 0.177689, 0.173384, 0.169055, 0.164841, 0.160729, 0.156552, 0.152452, 0.148454, 0.144502, 0.140706, 0.137147, 0.133566, 0.130105, 0.126681, 0.12332, 0.120015, 0.116763, 0.113718, 0.110726, 0.107674, 0.104816, 0.101915, 0.0991997, 0.0966262, 0.0941524, 0.0915663, 0.0891748, 0.0867209, 0.0842271, 0.0820177, 0.0795837, 0.0773543, 0.0751573, 0.0730002, 0.0708805, 0.0688605, 0.0669378, 0.0649278, 0.063075, 0.0611273, 0.0592445, 0.0573642, 0.0555363, 0.0537757, 0.0519877, 0.0503567, 0.0487981, 0.0472944, 0.0458031, 0.0442969, 0.0427782, 0.0414016, 0.0400525, 0.0386535, 0.0373392, 0.0359502, 0.0347532, 0.0334839, 0.0322544, 0.0310599, 0.0298654, 0.0286784, 0.0276036, 0.0265736, 0.0254739, 0.0245287, 0.0235113, 0.0225163, 0.021631, 0.0206983, 0.0197956, 0.0189477, 0.0180424, 0.0172444, 0.0164464, 0.0156858, 0.0149551, 0.0142245, 0.0135162, 0.0129252, 0.0122968, 0.0116833, 0.0110374, 0.0104788, 0.00994766, 0.00936411, 0.00887284, 0.008409, 0.00795264, 0.00749129, 0.00706236, 0.00665089, 0.006207, 0.00575812, 0.00528929, 0.00491522, 0.00461847, 0.00432669, 0.00401996, 0.00373318, 0.00344141, 0.00322943, 0.00300499, 0.00273816, 0.00250125, 0.0023217, 0.00212968, 0.00198005, 0.00180549, 0.00166335, 0.00148629, 0.00133916, 0.00121696, 0.00109975, 0.000997509, 0.000887783, 0.000783044, 0.000688281, 0.00060848, 0.00054863, 0.000466335, 0.000401497, 0.000351622, 0.000319203, 0.000286784, 0.000241896, 0.000214464, 0.00019202, 0.000167083, 0.000137157, 0.000104738, 9.97509e-05, 8.97758e-05, 8.47882e-05, 7.23194e-05, 5.23692e-05, 3.74066e-05, 3.74066e-05, 2.99253e-05, 2.99253e-05, 2.99253e-05, 2.49377e-05, 2.49377e-05, 2.24439e-05, 1.74564e-05, 1.49626e-05, 9.97509e-06, 9.97509e-06, 4.98754e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 2.49377e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<28000) {	
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999989, 0.999989, 0.999989, 0.999989, 0.999978, 0.999978, 0.999978, 0.999978, 0.999978, 0.999978, 0.999966, 0.999955, 0.999955, 0.999933, 0.999933, 0.999933, 0.999933, 0.999921, 0.999899, 0.999899, 0.999899, 0.999877, 0.999865, 0.999854, 0.999832, 0.999798, 0.999764, 0.999753, 0.999719, 0.999663, 0.999607, 0.999585, 0.999551, 0.999495, 0.999461, 0.999349, 0.999271, 0.999192, 0.99908, 0.998945, 0.998799, 0.998665, 0.99844, 0.99816, 0.99798, 0.9978, 0.997497, 0.997352, 0.997004, 0.996667, 0.996431, 0.996061, 0.995724, 0.995275, 0.994972, 0.994591, 0.994075, 0.993603, 0.993065, 0.992358, 0.991853, 0.99119, 0.990439, 0.989631, 0.988778, 0.987723, 0.986735, 0.985781, 0.984872, 0.983761, 0.982751, 0.981528, 0.980181, 0.978902, 0.977544, 0.976085, 0.974537, 0.972842, 0.971304, 0.969453, 0.96768, 0.965727, 0.963718, 0.961945, 0.959745, 0.957299, 0.9552, 0.952933, 0.950027, 0.947491, 0.944809, 0.942126, 0.939141, 0.93592, 0.932509, 0.929153, 0.925775, 0.922308, 0.918761, 0.915047, 0.911411, 0.907449, 0.903118, 0.899134, 0.894589, 0.889864, 0.884634, 0.879551, 0.87495, 0.869922, 0.864221, 0.858554, 0.852325, 0.846265, 0.840317, 0.833438, 0.825885, 0.818434, 0.81078, 0.803048, 0.795518, 0.78645, 0.777562, 0.769224, 0.759763, 0.750247, 0.740416, 0.730035, 0.720149, 0.710149, 0.700061, 0.690095, 0.67949, 0.669222, 0.658818, 0.648595, 0.637833, 0.627418, 0.616566, 0.605232, 0.594784, 0.584235, 0.573742, 0.563204, 0.552262, 0.541848, 0.531658, 0.521794, 0.511593, 0.502278, 0.492739, 0.482617, 0.473515, 0.464594, 0.45556, 0.446402, 0.437447, 0.428693, 0.420456, 0.412331, 0.404408, 0.395599, 0.387204, 0.37927, 0.371673, 0.363581, 0.355804, 0.348869, 0.341653, 0.334403, 0.327243, 0.319848, 0.313114, 0.306235, 0.299154, 0.292768, 0.286349, 0.279582, 0.273275, 0.267226, 0.260953, 0.254792, 0.248923, 0.242593, 0.236679, 0.230855, 0.224739, 0.218364, 0.212933, 0.20758, 0.20253, 0.197244, 0.192183, 0.18656, 0.181454, 0.17628, 0.171376, 0.166775, 0.161882, 0.157461, 0.153443, 0.14901, 0.144207, 0.139931, 0.135689, 0.13175, 0.127452, 0.123648, 0.119675, 0.115893, 0.112582, 0.109294, 0.10604, 0.10283, 0.0992952, 0.0961979, 0.093022, 0.0901715, 0.0871976, 0.0844705, 0.0818557, 0.0793083, 0.0767271, 0.0743143, 0.0717556, 0.0695897, 0.0673004, 0.0650334, 0.0628788, 0.06087, 0.0586592, 0.0569197, 0.0550905, 0.0533285, 0.0517013, 0.0499618, 0.0482673, 0.0466961, 0.0450913, 0.0435539, 0.0418705, 0.0405912, 0.039065, 0.0378193, 0.0363716, 0.0347556, 0.0333865, 0.032051, 0.0306594, 0.0296831, 0.0285945, 0.0273264, 0.0262042, 0.0252278, 0.0243974, 0.023118, 0.0221866, 0.020997, 0.0199758, 0.0189545, 0.0180343, 0.0172936, 0.0164632, 0.0156439, 0.0149594, 0.0141626, 0.0135903, 0.0128496, 0.0120528, 0.0114805, 0.010751, 0.0101001, 0.00957265, 0.00916865, 0.00860753, 0.00806886, 0.00739552, 0.00687929, 0.00637429, 0.00576828, 0.00543161, 0.00509494, 0.00469094, 0.00426449, 0.00389415, 0.00359115, 0.00327692, 0.00294025, 0.00258114, 0.00244647, 0.00215469, 0.00202002, 0.0018068, 0.00160479, 0.00147013, 0.00127935, 0.00121201, 0.00112223, 0.000976343, 0.000909009, 0.000852898, 0.000751897, 0.000662118, 0.000628451, 0.000617229, 0.000561117, 0.000448893, 0.000392782, 0.000347892, 0.00033667, 0.000303003, 0.000291781, 0.000269336, 0.000235669, 0.000202002, 0.00019078, 0.000168335, 0.000168335, 0.000157113, 0.000123446, 0.000101001, 0.000101001, 8.97787e-05, 7.85564e-05, 6.7334e-05, 4.48893e-05, 4.48893e-05, 4.48893e-05, 3.3667e-05, 2.24447e-05, 2.24447e-05, 2.24447e-05, 2.24447e-05, 2.24447e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 1.12223e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<42000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999987, 0.999987, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.999975, 0.99995, 0.999924, 0.999899, 0.999886, 0.999886, 0.999874, 0.999874, 0.999874, 0.999836, 0.999836, 0.999811, 0.999773, 0.999697, 0.999659, 0.999621, 0.999583, 0.999508, 0.999482, 0.999444, 0.999344, 0.999217, 0.999179, 0.999091, 0.99899, 0.998889, 0.998826, 0.998649, 0.998573, 0.998384, 0.998119, 0.998018, 0.997841, 0.997664, 0.997526, 0.997311, 0.997109, 0.996806, 0.99649, 0.996276, 0.995947, 0.995581, 0.995013, 0.994596, 0.994066, 0.993586, 0.993056, 0.992501, 0.991731, 0.99115, 0.990594, 0.989862, 0.989117, 0.988335, 0.987236, 0.986315, 0.985355, 0.98437, 0.983095, 0.982009, 0.980684, 0.979459, 0.977957, 0.976252, 0.974333, 0.972478, 0.970849, 0.968741, 0.966695, 0.96489, 0.962655, 0.96061, 0.958199, 0.955686, 0.95306, 0.950068, 0.947215, 0.943932, 0.940726, 0.937468, 0.934262, 0.930739, 0.926826, 0.92295, 0.918518, 0.914314, 0.909807, 0.90477, 0.899707, 0.894859, 0.889519, 0.884305, 0.878434, 0.872664, 0.866428, 0.86009, 0.85331, 0.846291, 0.838893, 0.831116, 0.82378, 0.815827, 0.80829, 0.799528, 0.791208, 0.783747, 0.774947, 0.766412, 0.758282, 0.74952, 0.741049, 0.733247, 0.724157, 0.715976, 0.70734, 0.699677, 0.691597, 0.683151, 0.674831, 0.665716, 0.657257, 0.648836, 0.640769, 0.632209, 0.623864, 0.615986, 0.607312, 0.599674, 0.592503, 0.585143, 0.577303, 0.569854, 0.562961, 0.555386, 0.547811, 0.541031, 0.533759, 0.526273, 0.519177, 0.512928, 0.505896, 0.499091, 0.492463, 0.485191, 0.478588, 0.472288, 0.465849, 0.459082, 0.452025, 0.445498, 0.438781, 0.432393, 0.425715, 0.419617, 0.413443, 0.407017, 0.400351, 0.394089, 0.387789, 0.381679, 0.375682, 0.369786, 0.364079, 0.358146, 0.351922, 0.345584, 0.339903, 0.333451, 0.327631, 0.322316, 0.316609, 0.310272, 0.304818, 0.298922, 0.292925, 0.28761, 0.28203, 0.276525, 0.270642, 0.264986, 0.259431, 0.253333, 0.248359, 0.242892, 0.237173, 0.231631, 0.225646, 0.220394, 0.214852, 0.209436, 0.20407, 0.198477, 0.193251, 0.188428, 0.183479, 0.178959, 0.173985, 0.169352, 0.165286, 0.160703, 0.155994, 0.151424, 0.14722, 0.142612, 0.138079, 0.134115, 0.130694, 0.12697, 0.123169, 0.119748, 0.116099, 0.112766, 0.109572, 0.106302, 0.103197, 0.10028, 0.0974144, 0.0945738, 0.0917458, 0.0887284, 0.0862539, 0.0832113, 0.080661, 0.0783002, 0.0760529, 0.0738941, 0.0716468, 0.0697026, 0.0675942, 0.0654606, 0.0634784, 0.0612438, 0.0593879, 0.0575573, 0.0555499, 0.0538961, 0.0523306, 0.0509544, 0.0494521, 0.0477477, 0.0464978, 0.0451975, 0.043834, 0.0424957, 0.0411575, 0.0397182, 0.0385315, 0.0373826, 0.0363221, 0.035047, 0.0340748, 0.0331153, 0.032219, 0.0313605, 0.0304262, 0.0295425, 0.0285577, 0.0275856, 0.0265377, 0.0256792, 0.0248965, 0.0241769, 0.0234699, 0.0225609, 0.021715, 0.0209954, 0.0202379, 0.0192405, 0.0184704, 0.0176876, 0.017069, 0.0164882, 0.0157055, 0.014948, 0.0143041, 0.0137107, 0.0130037, 0.0123725, 0.0117665, 0.01111, 0.0105924, 0.0100747, 0.00955712, 0.00897637, 0.00842087, 0.00796637, 0.00749924, 0.00705737, 0.00666599, 0.00611049, 0.00585799, 0.00550449, 0.00522674, 0.00470912, 0.00441875, 0.0041915, 0.00386325, 0.00359812, 0.00334562, 0.00309312, 0.00284062, 0.002626, 0.00249975, 0.00220937, 0.00204525, 0.00195687, 0.00185587, 0.00170437, 0.00159075, 0.00150237, 0.00140137, 0.00128775, 0.00119937, 0.00109837, 0.000959499, 0.000871124, 0.000757499, 0.000706999, 0.000656499, 0.000618624, 0.000542874, 0.000517624, 0.000492375, 0.0004545, 0.000441875, 0.000366125, 0.0003535, 0.00032825, 0.000303, 0.000265125, 0.000214625, 0.000214625, 0.000214625, 0.000202, 0.000189375, 0.000189375, 0.000138875, 0.000113625, 0.000101, 0.000101, 0.000101, 6.31249e-05, 5.04999e-05, 5.04999e-05, 5.04999e-05, 3.7875e-05, 3.7875e-05, 3.7875e-05, 2.525e-05, 2.525e-05, 2.525e-05, 2.525e-05, 2.525e-05, 2.525e-05, 1.2625e-05, 1.2625e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<65000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999988, 0.999988, 0.999988, 0.999988, 0.999988, 0.999988, 0.999975, 0.999963, 0.999963, 0.999963, 0.99995, 0.999938, 0.999938, 0.999938, 0.999926, 0.999913, 0.999901, 0.999876, 0.999864, 0.999851, 0.999827, 0.999827, 0.999814, 0.999802, 0.99974, 0.999715, 0.999665, 0.999628, 0.999591, 0.999566, 0.999504, 0.999492, 0.999393, 0.999244, 0.999207, 0.999096, 0.998996, 0.998873, 0.998798, 0.998612, 0.998365, 0.998216, 0.99803, 0.997832, 0.997609, 0.997398, 0.997163, 0.996704, 0.996333, 0.995874, 0.995255, 0.994958, 0.994388, 0.993842, 0.993334, 0.992851, 0.992232, 0.991352, 0.990534, 0.989742, 0.988986, 0.988019, 0.987251, 0.986161, 0.985281, 0.984278, 0.983175, 0.98211, 0.981119, 0.980003, 0.978913, 0.977575, 0.976101, 0.974577, 0.973276, 0.971901, 0.970253, 0.968518, 0.966945, 0.965037, 0.963241, 0.961543, 0.959871, 0.957628, 0.955906, 0.953787, 0.951458, 0.949278, 0.947048, 0.94478, 0.942612, 0.940134, 0.937446, 0.93472, 0.931871, 0.92917, 0.926283, 0.923508, 0.920373, 0.917065, 0.914067, 0.910474, 0.907104, 0.903982, 0.900129, 0.896239, 0.89241, 0.888532, 0.884506, 0.880182, 0.875734, 0.871162, 0.866653, 0.862031, 0.856977, 0.852194, 0.847028, 0.841651, 0.836398, 0.830711, 0.824553, 0.818656, 0.813044, 0.807134, 0.800803, 0.794001, 0.787261, 0.780497, 0.773571, 0.766273, 0.759249, 0.752534, 0.745013, 0.737257, 0.729427, 0.721907, 0.713903, 0.705231, 0.697141, 0.688926, 0.680985, 0.672758, 0.66437, 0.65597, 0.646765, 0.638043, 0.630039, 0.621181, 0.61231, 0.60365, 0.594271, 0.584967, 0.576282, 0.567386, 0.55911, 0.550499, 0.542099, 0.53365, 0.524903, 0.516602, 0.508066, 0.499566, 0.491191, 0.482791, 0.473573, 0.465124, 0.456798, 0.448101, 0.439403, 0.43104, 0.422851, 0.414315, 0.405989, 0.397738, 0.38966, 0.38121, 0.373318, 0.365203, 0.357286, 0.34958, 0.342047, 0.334824, 0.327428, 0.32018, 0.312548, 0.305474, 0.298164, 0.290805, 0.284325, 0.277548, 0.271081, 0.264167, 0.257737, 0.251803, 0.245026, 0.238918, 0.233231, 0.227024, 0.221449, 0.21596, 0.210112, 0.204599, 0.198999, 0.193771, 0.188542, 0.18319, 0.177863, 0.173142, 0.168682, 0.163739, 0.159378, 0.155202, 0.151027, 0.146703, 0.142107, 0.138018, 0.134623, 0.131018, 0.127301, 0.124291, 0.120983, 0.117526, 0.114342, 0.110972, 0.107726, 0.105149, 0.102052, 0.0989296, 0.0962287, 0.0933048, 0.0905419, 0.0879649, 0.0852021, 0.0827366, 0.0804445, 0.0781029, 0.0760339, 0.073779, 0.0717967, 0.0697648, 0.0677702, 0.0657135, 0.063694, 0.0619223, 0.0601259, 0.0584781, 0.0569046, 0.0554055, 0.0538321, 0.0522338, 0.0507471, 0.0495081, 0.0482196, 0.0468692, 0.0456303, 0.0441931, 0.0429789, 0.0419754, 0.0408232, 0.039832, 0.0385559, 0.0373913, 0.0361523, 0.0350373, 0.0338851, 0.0329435, 0.0318284, 0.0307506, 0.0297718, 0.0286815, 0.0276036, 0.026662, 0.02577, 0.0247045, 0.0238868, 0.0231187, 0.022462, 0.0217187, 0.0208886, 0.0200089, 0.0194018, 0.0185346, 0.0177664, 0.0169611, 0.016131, 0.0154992, 0.0148425, 0.0141239, 0.0134921, 0.0129098, 0.0122036, 0.0115098, 0.0107912, 0.0101717, 0.00971331, 0.00915578, 0.00863543, 0.00810268, 0.0076195, 0.00717348, 0.00662834, 0.00614515, 0.00587259, 0.00561241, 0.00525312, 0.00483188, 0.00448497, 0.00412568, 0.00372922, 0.00343187, 0.00313452, 0.00286196, 0.00265134, 0.0023416, 0.00220532, 0.00203187, 0.00177169, 0.00161063, 0.00146195, 0.00130089, 0.00122655, 0.00113983, 0.00102832, 0.000953986, 0.000904428, 0.000817702, 0.000755755, 0.000730976, 0.000669029, 0.000619471, 0.000545135, 0.000495577, 0.000470798, 0.000446019, 0.00042124, 0.000408851, 0.000396462, 0.000346904, 0.000309736, 0.000284957, 0.000272567, 0.000247788, 0.000235399, 0.00022301, 0.000198231, 0.000161063, 0.000148673, 0.000148673, 0.000136284, 0.000111505, 9.91154e-05, 9.91154e-05, 7.43365e-05, 7.43365e-05, 6.19471e-05, 6.19471e-05, 6.19471e-05, 6.19471e-05, 4.95577e-05, 4.95577e-05, 4.95577e-05, 3.71683e-05, 2.47788e-05, 2.47788e-05, 1.23894e-05, 1.23894e-05, 1.23894e-05, 1.23894e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<100000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999989, 0.999989, 0.999989, 0.999989, 0.999989, 0.999989, 0.999989, 0.999989, 0.999978, 0.999978, 0.999978, 0.999967, 0.999967, 0.999956, 0.999945, 0.999934, 0.999934, 0.999934, 0.999934, 0.999923, 0.999923, 0.999911, 0.999845, 0.999845, 0.999845, 0.999823, 0.999801, 0.999801, 0.999779, 0.999756, 0.999723, 0.999701, 0.99969, 0.999668, 0.999635, 0.99959, 0.999579, 0.999535, 0.999502, 0.999413, 0.999325, 0.999269, 0.999181, 0.999037, 0.998948, 0.998838, 0.998672, 0.998561, 0.998472, 0.998251, 0.998074, 0.99783, 0.997686, 0.997421, 0.997211, 0.997033, 0.996746, 0.99648, 0.996192, 0.995816, 0.995428, 0.994997, 0.994631, 0.994177, 0.993657, 0.993037, 0.99244, 0.991897, 0.991133, 0.990347, 0.989406, 0.988665, 0.98758, 0.986606, 0.985565, 0.984425, 0.98334, 0.982211, 0.980916, 0.979654, 0.978248, 0.976942, 0.975581, 0.973632, 0.972061, 0.970168, 0.968419, 0.966481, 0.964411, 0.961998, 0.95974, 0.957283, 0.955002, 0.952312, 0.949634, 0.946822, 0.943911, 0.940811, 0.937723, 0.934778, 0.931767, 0.928513, 0.924893, 0.921284, 0.917875, 0.914455, 0.910724, 0.906695, 0.902832, 0.898503, 0.894386, 0.889792, 0.885098, 0.88005, 0.875567, 0.870664, 0.865262, 0.860192, 0.854502, 0.849011, 0.842348, 0.835839, 0.829064, 0.82229, 0.81588, 0.80884, 0.800903, 0.793797, 0.786015, 0.777381, 0.769444, 0.760832, 0.751367, 0.742135, 0.73277, 0.723505, 0.713598, 0.703934, 0.694337, 0.684518, 0.674412, 0.664471, 0.654165, 0.644258, 0.63434, 0.624078, 0.613883, 0.603854, 0.593626, 0.583486, 0.573635, 0.564115, 0.554495, 0.544798, 0.535002, 0.525405, 0.515841, 0.506675, 0.498041, 0.489617, 0.480473, 0.472614, 0.464279, 0.456297, 0.448538, 0.440867, 0.432587, 0.42465, 0.41679, 0.409363, 0.402101, 0.394795, 0.387556, 0.380593, 0.373608, 0.36587, 0.358753, 0.352587, 0.34517, 0.338529, 0.331522, 0.325245, 0.318083, 0.311464, 0.304966, 0.29766, 0.291782, 0.286048, 0.280104, 0.273849, 0.267673, 0.261817, 0.25616, 0.250028, 0.244393, 0.238692, 0.233545, 0.228276, 0.223007, 0.218026, 0.212712, 0.207155, 0.202185, 0.197503, 0.192721, 0.188392, 0.184009, 0.179515, 0.174865, 0.17046, 0.166353, 0.16239, 0.158394, 0.154664, 0.151121, 0.147026, 0.143694, 0.139853, 0.136056, 0.132679, 0.129348, 0.125805, 0.122518, 0.119717, 0.116573, 0.113906, 0.110994, 0.108072, 0.105393, 0.102692, 0.10039, 0.0979765, 0.0955744, 0.0930395, 0.0906042, 0.0882242, 0.0861764, 0.084051, 0.081981, 0.0797671, 0.0774646, 0.0753836, 0.0735017, 0.0716421, 0.0697381, 0.067978, 0.0662401, 0.0644247, 0.0627754, 0.0612035, 0.0594877, 0.0578605, 0.0564989, 0.0549381, 0.0532113, 0.0515841, 0.0502889, 0.0491155, 0.0475879, 0.0460271, 0.0446324, 0.0435365, 0.0423742, 0.0410901, 0.0396068, 0.038367, 0.0371383, 0.0358764, 0.0347473, 0.033596, 0.0325002, 0.0312936, 0.0303416, 0.029456, 0.0285483, 0.0275521, 0.0267329, 0.0257367, 0.0249397, 0.0240873, 0.0232682, 0.0223051, 0.0212535, 0.0204786, 0.0196706, 0.0187629, 0.017988, 0.0171467, 0.0164383, 0.0157409, 0.0150324, 0.01449, 0.0138148, 0.013217, 0.0126193, 0.0118998, 0.0113573, 0.0107928, 0.0103057, 0.00969692, 0.00909916, 0.0086121, 0.00822467, 0.00768226, 0.00723948, 0.00678563, 0.00624322, 0.00580044, 0.00540193, 0.00498129, 0.00463814, 0.00429498, 0.00395183, 0.00366402, 0.00326551, 0.0029445, 0.00267883, 0.00251279, 0.0023246, 0.00212535, 0.00197038, 0.00180433, 0.00160508, 0.00151653, 0.00140583, 0.00131728, 0.00123979, 0.00112909, 0.00111802, 0.000996258, 0.000896633, 0.000797007, 0.000752729, 0.000697381, 0.000642033, 0.000597755, 0.000542407, 0.000509199, 0.000420642, 0.000398503, 0.000343156, 0.000321017, 0.000287808, 0.000276738, 0.000254599, 0.000199252, 0.000166043, 0.000132834, 9.96259e-05, 9.96259e-05, 8.85563e-05, 7.74868e-05, 7.74868e-05, 6.64172e-05, 6.64172e-05, 6.64172e-05, 5.53477e-05, 5.53477e-05, 4.42782e-05, 3.32086e-05, 2.21391e-05, 2.21391e-05, 2.21391e-05, 2.21391e-05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<150000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999997, 0.999994, 0.999994, 0.999987, 0.999987, 0.999984, 0.999981, 0.999974, 0.999974, 0.999958, 0.999945, 0.999939, 0.999932, 0.999926, 0.99992, 0.9999, 0.999891, 0.999881, 0.999878, 0.999852, 0.99983, 0.999794, 0.999775, 0.999759, 0.999733, 0.999704, 0.999672, 0.99965, 0.999621, 0.999595, 0.999569, 0.999537, 0.999492, 0.99945, 0.999399, 0.999347, 0.999286, 0.999206, 0.999142, 0.999084, 0.998981, 0.9989, 0.998791, 0.998701, 0.998563, 0.998412, 0.99827, 0.998061, 0.99782, 0.997605, 0.997347, 0.997116, 0.996907, 0.996579, 0.996264, 0.995881, 0.995486, 0.994984, 0.994541, 0.994078, 0.993605, 0.993145, 0.992544, 0.992007, 0.991416, 0.990859, 0.990123, 0.989441, 0.988718, 0.988088, 0.987374, 0.986586, 0.985786, 0.984821, 0.983892, 0.982963, 0.982034, 0.981108, 0.980166, 0.979163, 0.978066, 0.976854, 0.975668, 0.974501, 0.973134, 0.971678, 0.970266, 0.968549, 0.967035, 0.96536, 0.963633, 0.96181, 0.9599, 0.957833, 0.955698, 0.953509, 0.951088, 0.948522, 0.945956, 0.943365, 0.940674, 0.937738, 0.934604, 0.931363, 0.927816, 0.924058, 0.920196, 0.916242, 0.911978, 0.907458, 0.90295, 0.897967, 0.892678, 0.887109, 0.881688, 0.876023, 0.869696, 0.863111, 0.856552, 0.849164, 0.841901, 0.83412, 0.825989, 0.817912, 0.809559, 0.800515, 0.791307, 0.782394, 0.773344, 0.763888, 0.753995, 0.743745, 0.73346, 0.7232, 0.712252, 0.701662, 0.690688, 0.679368, 0.668198, 0.656865, 0.645943, 0.634712, 0.623424, 0.612328, 0.601175, 0.590292, 0.579305, 0.568084, 0.556735, 0.545016, 0.533386, 0.521648, 0.510137, 0.498505, 0.486557, 0.474636, 0.463363, 0.452152, 0.441349, 0.430961, 0.42101, 0.411261, 0.401841, 0.392488, 0.383836, 0.375072, 0.366725, 0.358584, 0.350697, 0.343126, 0.335599, 0.328583, 0.321603, 0.314794, 0.30836, 0.301865, 0.295515, 0.289214, 0.283243, 0.277491, 0.272019, 0.266563, 0.261075, 0.255972, 0.250786, 0.245735, 0.240835, 0.23618, 0.23146, 0.227113, 0.222679, 0.218194, 0.214149, 0.210005, 0.206025, 0.202099, 0.198157, 0.194402, 0.190872, 0.187309, 0.183695, 0.180233, 0.176898, 0.173522, 0.170182, 0.16696, 0.163858, 0.161044, 0.15799, 0.155042, 0.152245, 0.149454, 0.146547, 0.143982, 0.141155, 0.138403, 0.136059, 0.13367, 0.13124, 0.12888, 0.126549, 0.124363, 0.122048, 0.119739, 0.117556, 0.115505, 0.113428, 0.111267, 0.109174, 0.107084, 0.1052, 0.10339, 0.101593, 0.0998116, 0.0979597, 0.0961945, 0.0944969, 0.0928379, 0.0912657, 0.0896645, 0.088102, 0.0866037, 0.085099, 0.083575, 0.0819771, 0.0805688, 0.0791091, 0.0777138, 0.0764084, 0.0751641, 0.0738813, 0.0725759, 0.0713509, 0.0700777, 0.0688013, 0.067586, 0.066361, 0.0650878, 0.0638821, 0.0627022, 0.061654, 0.0605126, 0.0594195, 0.0584067, 0.0572942, 0.0562461, 0.055198, 0.0541691, 0.0530052, 0.0519796, 0.0510086, 0.0500955, 0.0492949, 0.0483046, 0.0473047, 0.0463595, 0.045501, 0.0445943, 0.0438002, 0.0428967, 0.0420962, 0.0412634, 0.0404693, 0.0396076, 0.0387717, 0.037984, 0.0371094, 0.0363892, 0.0357012, 0.0349424, 0.0342029, 0.0334602, 0.032695, 0.0319105, 0.0312193, 0.0305827, 0.0299203, 0.0292934, 0.0286085, 0.0278562, 0.0272164, 0.0265155, 0.0258531, 0.0252455, 0.0245799, 0.0239466, 0.0232296, 0.0225833, 0.021921, 0.0211815, 0.0205481, 0.0199501, 0.0192074, 0.0185804, 0.0180306, 0.0174358, 0.0168925, 0.0162494, 0.0155646, 0.0149698, 0.0144007, 0.0137641, 0.0131822, 0.0126227, 0.0120119, 0.0114331, 0.0108158, 0.0102339, 0.00966479, 0.00911499, 0.0086295, 0.00806042, 0.00756207, 0.00707979, 0.00659752, 0.00608631, 0.00566191, 0.00514426, 0.00473272, 0.00433404, 0.00401253, 0.00365886, 0.00333091, 0.00304476, 0.00274575, 0.00245317, 0.0020995, 0.00189052, 0.00170725, 0.00154006, 0.00136323, 0.00123462, 0.00107708, 0.000938828, 0.000803791, 0.000716982, 0.000636603, 0.000540148, 0.000450123, 0.000389035, 0.000347238, 0.000295795, 0.000263644, 0.000231492, 0.000196125, 0.000154328, 0.000122176, 0.000109316, 9.32398e-05, 8.68095e-05, 7.07336e-05, 5.46578e-05, 4.50123e-05, 4.50123e-05, 4.17971e-05, 2.89365e-05, 1.9291e-05, 1.28607e-05, 9.6455e-06, 9.6455e-06, 9.6455e-06, 9.6455e-06, 9.6455e-06, 9.6455e-06, 9.6455e-06, 9.6455e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 3.21517e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else if (EnergyD<200000) {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999992, 0.999992, 0.999992, 0.999992, 0.999984, 0.999984, 0.999977, 0.999977, 0.999977, 0.999977, 0.999969, 0.999961, 0.999961, 0.999945, 0.999945, 0.999937, 0.999922, 0.999906, 0.999898, 0.99989, 0.99989, 0.999883, 0.999851, 0.999836, 0.999804, 0.999749, 0.99971, 0.999663, 0.999632, 0.999601, 0.999561, 0.999522, 0.999452, 0.999405, 0.999326, 0.99924, 0.999178, 0.999115, 0.999099, 0.999021, 0.99895, 0.998872, 0.998778, 0.998653, 0.998575, 0.998457, 0.998355, 0.998214, 0.998097, 0.997901, 0.997791, 0.997619, 0.997486, 0.997243, 0.997016, 0.99675, 0.996468, 0.996194, 0.995974, 0.995637, 0.995379, 0.994925, 0.994478, 0.994008, 0.993679, 0.993108, 0.992622, 0.992058, 0.991408, 0.990805, 0.990069, 0.989419, 0.988706, 0.987884, 0.986991, 0.986012, 0.985017, 0.984124, 0.982988, 0.981853, 0.980874, 0.979652, 0.978297, 0.976605, 0.974866, 0.972994, 0.971209, 0.969157, 0.966948, 0.964716, 0.962601, 0.960236, 0.957549, 0.954831, 0.951839, 0.948605, 0.94544, 0.94183, 0.937647, 0.933512, 0.929008, 0.924442, 0.919477, 0.914542, 0.908973, 0.903225, 0.896951, 0.890419, 0.883793, 0.87654, 0.869436, 0.861338, 0.853357, 0.845039, 0.836447, 0.827275, 0.817187, 0.807271, 0.796894, 0.786109, 0.774345, 0.76244, 0.749266, 0.735865, 0.722409, 0.708554, 0.693915, 0.679645, 0.665539, 0.649553, 0.634829, 0.619791, 0.604345, 0.589237, 0.574403, 0.560422, 0.547154, 0.534082, 0.520924, 0.508463, 0.495837, 0.483768, 0.472286, 0.461038, 0.450159, 0.439382, 0.42848, 0.418392, 0.40904, 0.398889, 0.388801, 0.378815, 0.369283, 0.359571, 0.350173, 0.34126, 0.332715, 0.324217, 0.315875, 0.307581, 0.299561, 0.29201, 0.284703, 0.277748, 0.270432, 0.263697, 0.257407, 0.251353, 0.245494, 0.23937, 0.233738, 0.22795, 0.222319, 0.217024, 0.211957, 0.206787, 0.201963, 0.197021, 0.192282, 0.188068, 0.184066, 0.17997, 0.176046, 0.171848, 0.167932, 0.163914, 0.160272, 0.156716, 0.153536, 0.150458, 0.147482, 0.144458, 0.141482, 0.138545, 0.135757, 0.132898, 0.130251, 0.127588, 0.12512, 0.122645, 0.120217, 0.11793, 0.115722, 0.113497, 0.111234, 0.109229, 0.1072, 0.105179, 0.103292, 0.101459, 0.0993601, 0.0978093, 0.0960236, 0.0942926, 0.092499, 0.0906976, 0.0890998, 0.0875804, 0.0859199, 0.084377, 0.0826304, 0.0812519, 0.0797403, 0.0783931, 0.077093, 0.075644, 0.0744378, 0.0729967, 0.07172, 0.0704199, 0.0693077, 0.0679449, 0.0668092, 0.0656892, 0.064577, 0.0632534, 0.0620315, 0.0610212, 0.0598307, 0.0587028, 0.057528, 0.056455, 0.0553506, 0.0541131, 0.0532437, 0.0523274, 0.051411, 0.0503771, 0.0492493, 0.0480823, 0.0469388, 0.0458031, 0.0448632, 0.0439468, 0.0430148, 0.0420671, 0.0409706, 0.0399602, 0.0391378, 0.0381588, 0.0372268, 0.0363104, 0.0353862, 0.034462, 0.0337179, 0.0329033, 0.0319791, 0.0312742, 0.0304518, 0.0296686, 0.0287601, 0.027828, 0.0269665, 0.0260893, 0.0250946, 0.0241704, 0.023254, 0.0225256, 0.0216954, 0.0209121, 0.0200898, 0.0192674, 0.0185076, 0.0176383, 0.0168472, 0.0161345, 0.0153121, 0.014474, 0.0137378, 0.0131347, 0.0124768, 0.0117562, 0.0110043, 0.0103543, 0.00979816, 0.00916375, 0.00845101, 0.00791059, 0.0073075, 0.00676708, 0.00632847, 0.00570972, 0.00527111, 0.00488733, 0.00445656, 0.00411977, 0.00373599, 0.00333654, 0.00299976, 0.00262381, 0.00234185, 0.00208338, 0.00183275, 0.00166827, 0.00147247, 0.00134715, 0.00112785, 0.000986865, 0.000885046, 0.00075973, 0.000634413, 0.000571755, 0.0004856, 0.000422942, 0.000391613, 0.000313291, 0.000274129, 0.000250632, 0.0002428, 0.000219303, 0.000211471, 0.000164478, 0.000133148, 0.000117484, 9.39872e-05, 7.04904e-05, 5.48259e-05, 4.69936e-05, 3.13291e-05, 2.34968e-05, 2.34968e-05, 1.56645e-05, 1.56645e-05, 1.56645e-05, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 7.83226e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	} else {
		float Rej[nbins]={1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0.999993, 0.999993, 0.999986, 0.999979, 0.999979, 0.999972, 0.999965, 0.999965, 0.999965, 0.999958, 0.999832, 0.999825, 0.999818, 0.99979, 0.999755, 0.999748, 0.999727, 0.99972, 0.999671, 0.999657, 0.99958, 0.999552, 0.999426, 0.999405, 0.99937, 0.999245, 0.999182, 0.999035, 0.998944, 0.998762, 0.998636, 0.998454, 0.998188, 0.998034, 0.997846, 0.997727, 0.997552, 0.997342, 0.997181, 0.996873, 0.996656, 0.996335, 0.996132, 0.995733, 0.995285, 0.994978, 0.994635, 0.994278, 0.9939, 0.993348, 0.992942, 0.99248, 0.992005, 0.991417, 0.990802, 0.990109, 0.989591, 0.989109, 0.988682, 0.98822, 0.987794, 0.987381, 0.986814, 0.986276, 0.985793, 0.985045, 0.98452, 0.984037, 0.983366, 0.982673, 0.982079, 0.981449, 0.980799, 0.980169, 0.979498, 0.978861, 0.978127, 0.977497, 0.976749, 0.976042, 0.975231, 0.974461, 0.973671, 0.972866, 0.972069, 0.971181, 0.970173, 0.969285, 0.968453, 0.967368, 0.966312, 0.965025, 0.964018, 0.963115, 0.961905, 0.960744, 0.959485, 0.957848, 0.956631, 0.955309, 0.953924, 0.952455, 0.951056, 0.949559, 0.948146, 0.946635, 0.945152, 0.943543, 0.941753, 0.94013, 0.938311, 0.93652, 0.934597, 0.93205, 0.930414, 0.928028, 0.92586, 0.923733, 0.921768, 0.919264, 0.916962, 0.9145, 0.912024, 0.909338, 0.906512, 0.903826, 0.901021, 0.898258, 0.895355, 0.892291, 0.889451, 0.886485, 0.883267, 0.879413, 0.876615, 0.873229, 0.868186, 0.864038, 0.86038, 0.856441, 0.852328, 0.848488, 0.843955, 0.839758, 0.834932, 0.830441, 0.825397, 0.820774, 0.816388, 0.811806, 0.805839, 0.801006, 0.795095, 0.787939, 0.781119, 0.775915, 0.770403, 0.76452, 0.75821, 0.751845, 0.744997, 0.738387, 0.731685, 0.725509, 0.719451, 0.713267, 0.7063, 0.70025, 0.692856, 0.685994, 0.678866, 0.671927, 0.66531, 0.658154, 0.650375, 0.643583, 0.636924, 0.629747, 0.622654, 0.615008, 0.607468, 0.600599, 0.592932, 0.585021, 0.578187, 0.570898, 0.563448, 0.555698, 0.548486, 0.541652, 0.534048, 0.526375, 0.518547, 0.510552, 0.503417, 0.495457, 0.488259, 0.481348, 0.473562, 0.466008, 0.457278, 0.449255, 0.440686, 0.433208, 0.42494, 0.416889, 0.409187, 0.40129, 0.391959, 0.384782, 0.376129, 0.36763, 0.36067, 0.35315, 0.344819, 0.33753, 0.329185, 0.322414, 0.314706, 0.307962, 0.295707, 0.289062, 0.282689, 0.2741, 0.267566, 0.261054, 0.254611, 0.248778, 0.242328, 0.237026, 0.230542, 0.225624, 0.220182, 0.214432, 0.210151, 0.206269, 0.202191, 0.197826, 0.193545, 0.189516, 0.185955, 0.182647, 0.177708, 0.174267, 0.170888, 0.167964, 0.164572, 0.160661, 0.157738, 0.155226, 0.152645, 0.150309, 0.147406, 0.144286, 0.14209, 0.139676, 0.137361, 0.135095, 0.133017, 0.129771, 0.127883, 0.126134, 0.124553, 0.122231, 0.120678, 0.119069, 0.117537, 0.116012, 0.114571, 0.112865, 0.111305, 0.110081, 0.108346, 0.10701, 0.105436, 0.104156, 0.102974, 0.101819, 0.100546, 0.0992313, 0.0978882, 0.0966711, 0.095419, 0.0942858, 0.0931386, 0.0920124, 0.0906624, 0.0895222, 0.088347, 0.0872908, 0.0861646, 0.0851853, 0.0840591, 0.0828699, 0.0818976, 0.0808693, 0.0798621, 0.0788058, 0.0777356, 0.0767843, 0.0758119, 0.0747697, 0.0739093, 0.072895, 0.0720626, 0.0712582, 0.0702439, 0.0694185, 0.0684882, 0.0676488, 0.0667534, 0.065893, 0.0650047, 0.0642072, 0.0633748, 0.0625564, 0.061703, 0.0608986, 0.0600592, 0.0593667, 0.0586532, 0.0577788, 0.0570303, 0.0563238, 0.0556243, 0.0548619, 0.0540505, 0.053316, 0.0526794, 0.0519799, 0.0513364, 0.0505809, 0.0499024, 0.0491819, 0.0485524, 0.0478669, 0.0473492, 0.0467547, 0.046244, 0.0456564, 0.0451668, 0.0446282, 0.0440826, 0.0435649, 0.0430473, 0.0424597, 0.0418931, 0.0413335, 0.040718, 0.0401654, 0.0396967, 0.0391301, 0.0387244, 0.0380948, 0.0376332, 0.0371925, 0.0367868, 0.0362971, 0.0357935, 0.0353248, 0.0349191, 0.0345134, 0.0341217, 0.033674, 0.0332053, 0.0328136, 0.0323589, 0.0319602, 0.0315615, 0.0311838, 0.0307221, 0.0302394, 0.0298687, 0.0293581, 0.0289244, 0.0283648, 0.028001, 0.0276583, 0.0272386, 0.0268399, 0.0264691, 0.0260005, 0.0255528, 0.0251401, 0.0248183, 0.0243916, 0.0239999, 0.0236361, 0.0233214, 0.0229157, 0.0225449, 0.0222022, 0.0218174, 0.0214677, 0.021055, 0.0206843, 0.0202366, 0.0199078, 0.019565, 0.0192713, 0.0189285, 0.0186067, 0.018173, 0.0177533, 0.0173406, 0.0170189, 0.0166831, 0.0164033, 0.0160046, 0.0156898, 0.015396, 0.0150882, 0.0147735, 0.0144797, 0.0141089, 0.0138361, 0.0135353, 0.0131926, 0.0128498, 0.01257, 0.0122623, 0.0119825, 0.0116187, 0.0113599, 0.0109962, 0.0107513, 0.0105765, 0.0102897, 0.0100029, 0.0096741, 0.00938731, 0.00920544, 0.00893263, 0.00863185, 0.00838002, 0.0081282, 0.00783441, 0.00760358, 0.00733077, 0.00707196, 0.00682014, 0.00663127, 0.00640044, 0.00621857, 0.00605768, 0.00586882, 0.00568695, 0.0054771, 0.00528823, 0.00513434, 0.00491749, 0.00470065, 0.00446981, 0.00430893, 0.00411307, 0.0039312, 0.00381228, 0.00368637, 0.00349751, 0.00334362, 0.00313377, 0.00297288, 0.00283298, 0.0026651, 0.00255318, 0.00242727, 0.00228737, 0.00214747, 0.00203555, 0.00193762, 0.00183969, 0.00172077, 0.00162984, 0.00153191, 0.00148994, 0.00137802, 0.00131506, 0.00125211, 0.00116817, 0.00109822, 0.00102827, 0.000944327, 0.000916347, 0.000839402, 0.000762456, 0.000692506, 0.000629551, 0.000594576, 0.000566596, 0.000531621, 0.000461671, 0.000433691, 0.000384726, 0.000370736, 0.000356746, 0.000335761, 0.000307781, 0.000293791, 0.000272805, 0.000230835, 0.00019586, 0.00018187, 0.000160885, 0.000146895, 0.000132905, 0.00012591, 0.00011192, 0.000104925, 9.79302e-05, 8.39401e-05, 7.69451e-05, 6.99501e-05, 6.99501e-05, 5.59601e-05, 5.59601e-05, 4.89651e-05, 4.89651e-05, 4.89651e-05, 4.89651e-05, 4.89651e-05, 4.19701e-05, 4.19701e-05, 4.19701e-05, 3.49751e-05, 3.49751e-05, 3.49751e-05, 3.49751e-05, 2.798e-05, 2.0985e-05, 1.399e-05, 6.99501e-06, 6.99501e-06, 6.99501e-06, 6.99501e-06, 6.99501e-06, 6.99501e-06, 6.99501e-06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		rejection=Rej[bin];
	}
	return 1/rejection;
}
	




float EcalShowerR::EcalStandaloneEstimatorV2(){

        float nEnergyC;
        float nEcalStandaloneEstimator,necalBDT,nMomentum,nEnergyE,nRigInn;

        NormaliseVariableLAPP();

   	float S1totL[18]={-20.};
   	float S3totL[18]={-20.}; 
   	float S5totL[18]={-20.}; 

	// Case Processing version <B584
	if(AMSEventR::Head()->Version()<584)
        	nEnergyA = EnergyA;
	// Case Processing version >=B584
	if(AMSEventR::Head()->Version()>=584)
		nEnergyA = EnergyA*1000.;


        float log10E = TMath::Log10(nEnergyA/1000.);
        if(log10E<0.4 || log10E>3.6){
                return -30.;
        }

    //=== Calcul Layer Sigma
    float MapEneDep[18][72];
    float s_cell_w[18];

    for (Int_t ilayer = 0; ilayer < 18; ++ilayer){
        s_cell_w[ilayer] = 0;
        for (Int_t icell = 0; icell < 72; ++icell)
                MapEneDep[ilayer][icell] = 0.;
    }

    for (Int_t i2dcluster = 0; i2dcluster < NEcal2DCluster(); ++i2dcluster){
        for (Int_t icluster = 0; icluster < pEcal2DCluster(i2dcluster)->NEcalCluster(); ++icluster){
            for (Int_t ihit = 0; ihit < pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->NEcalHit(); ++ihit){
                EcalHitR *hit = pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->pEcalHit(ihit);
                if (hit->ADC[0] > 4.){
                        MapEneDep[hit->Plane][hit->Cell] = hit->Edep;
                        s_cell_w[hit->Plane]        += (hit->Cell+1)*hit->Edep;
                }
             }
        }
    }
    float LayerMean[18];
    float LayerEneDep[18];
    for (Int_t ilayer = 0; ilayer < 18; ++ilayer){
        LayerEneDep[ilayer] = 0.;
        LayerMean[ilayer]   = 0.;
        for (Int_t icell = 0; icell < 72; ++icell){
            MapEneDep[ilayer][icell] /= 1000.;
            LayerMean[ilayer]   += icell*MapEneDep[ilayer][icell];
            LayerEneDep[ilayer] += MapEneDep[ilayer][icell];
          }
        if (LayerEneDep[ilayer] > 0.) LayerMean[ilayer] /= LayerEneDep[ilayer];
        else LayerMean[ilayer] = -1.;
      } 
        
    float LayerSigma[18];
    int bcell_i;
    float bcell_lat[18];
    for (Int_t ilayer = 0; ilayer < 18; ++ilayer){
        LayerSigma[ilayer] = 0.;
        S1totL[ilayer] = 0.;
        S3totL[ilayer] = 0.;
        S5totL[ilayer] = 0.;
        for (Int_t icell = 0; icell < 72; ++icell)
          LayerSigma[ilayer] += TMath::Power(icell-LayerMean[ilayer], 2)*MapEneDep[ilayer][icell];
        if (LayerEneDep[ilayer] > 0.){
                LayerSigma[ilayer] /= LayerEneDep[ilayer];
                LayerSigma[ilayer]  = TMath::Sqrt(LayerSigma[ilayer]);
                bcell_lat[ilayer]=s_cell_w[ilayer]/LayerEneDep[ilayer];
                bcell_i=(int) (bcell_lat[ilayer]-1.);
                if ((bcell_lat[ilayer]-1-bcell_i)>0.5)  bcell_i++;
                if (((bcell_i-2)>=0)&&((bcell_i+2)<=71)){
                        S1totL[ilayer]=(MapEneDep[ilayer][bcell_i])/LayerEneDep[ilayer];
                        S3totL[ilayer]=(MapEneDep[ilayer][bcell_i-1]+MapEneDep[ilayer][bcell_i]+MapEneDep[ilayer][bcell_i+1])/LayerEneDep[ilayer];
                        S5totL[ilayer]=(MapEneDep[ilayer][bcell_i-2]+MapEneDep[ilayer][bcell_i-1]+MapEneDep[ilayer][bcell_i]+MapEneDep[ilayer][bcell_i+1]+MapEneDep[ilayer][bcell_i+2])/LayerEneDep[ilayer];     
                }
        } 
    }
	(void) S1totL;
	(void) S3totL;
	(void) S5totL;


   // FIT ZPROFILE
    float ZprofileChi2v1=-1.;

  float etot=0.;
  float err;
  float par0,par1,par2;
  float frac[18]={0.};
  float ECalEdepFrac[18]={0.};

   for (Int_t i2dcluster = 0; i2dcluster < NEcal2DCluster(); ++i2dcluster){
        for (Int_t icluster = 0; icluster < pEcal2DCluster(i2dcluster)->NEcalCluster(); ++icluster){
            for (Int_t ihit = 0; ihit < pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->NEcalHit(); ++ihit){
                EcalHitR *hit = pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->pEcalHit(ihit);
                if (!isnan(hit->Edep)){
                        ECalEdepFrac[hit->Plane] += hit->Edep;
			etot += hit->Edep;
        	}
      	    }
	}	
    }
  nbinsv1 = 0;
  if(etot>=0.){
    par2= 1.05*(log(EnergyE*1000./(8.))-0.5);
    for(int a=0;a<18;a++){
      frac[a] = ECalEdepFrac[a]/(EnergyE*1000.);
      err=(ECalEdepFrac[a]/(EnergyE*1000.))*0.10;
      if (err<.009&&EnergyD<1000.) err=0.009;
      if (err<.004&&EnergyD>1000.) err=0.004;
      if(!isnan(frac[a]) && !isnan(err) && frac[a]>0. && frac[a]<1. && err>0. && err<1.){
        if((par2>8&&a>4)||(par2<8&&a<15)){
          zv1[nbinsv1] = frac[a];
          errorzv1[nbinsv1] = err;
          xv1[nbinsv1]=(float) a + 0.5;
          nbinsv1++;
        }
      }
    }
     Double_t arglist[10];
      // The z values   
      Double_t zprof[3],errprof[3];
      par0=1.;
      par1=0.5; 
      Int_t ierflg = 0;
    if(nbinsv1>3){
         
      // par2==zmax if the shower is an electromagnetic shower 
      // Minuit
      if (!EcalShowerR_ZProf::fMinuit) {
#pragma omp critical (tminuit) 
        EcalShowerR_ZProf::fMinuit = new TMinuit(3);
      }         
      TMinuit *minuit = EcalShowerR_ZProf::fMinuit;
        
      //Set Minuit print Options
      minuit->SetPrintLevel(-1);
      minuit->SetFCN(fcnv1);
      arglist[0]   = 1;
      minuit->mnexcm("SET ERR", arglist ,1,ierflg);
      // Set starting values and step sizes for parameters
      Double_t vstart[3] = {par0,par1,par2};
      Double_t step[3]   = {0.0001 , 0.0001 , .0001};
      minuit->mnparm(0, "a1", vstart[0], step[0],0.2*par0,3.*par0,ierflg);
      minuit->mnparm(1, "a2", vstart[1], step[1],0.,1.,ierflg);
      minuit->mnparm(2, "a3", vstart[2], step[2],0.5*par2,3.*par2,ierflg);
      // Now ready for minimization step
      arglist[0] = 1000;
      arglist[1] = 1;
      minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
      // Print results
      minuit->GetParameter(0,zprof[0],errprof[0]);
      minuit->GetParameter(1,zprof[1],errprof[1]);
      minuit->GetParameter(2,zprof[2],errprof[2]);
      Double_t amin,edm,errdef;
      Int_t nvpar,nparx,icstat;
      minuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
      ZprofileChi2v1 = amin/(nbinsv1-3);
    }
  
}

/*
cout << "chi2 " << ZprofileChi2v1 << endl;
        
                
        
for(int a=0;a<18;a++){cout << LayerSigma[a] << " "; }
cout<<endl;
for(int a=0;a<18;a++){cout << EnergyFractionLayer[a] << " ";} cout <<endl;
for(int a=0;a<3 ;a++){cout<< S1tot[a] << " " << S3tot[a] << " " << S5tot[a] << " ";}
cout << ShowerLatDisp[0] << " "<< (ShowerLatDisp[1]* (AMSEventR::Head()->Version()<584?1.:(1.+VarLeakXA))) << " "<< (ShowerLatDisp[2]* (AMSEventR::Head()->Version()<584?1.:(1.+VarLeakYA))) << " ";
for(int a=0;a<3 ;a++){cout<< ShowerFootprint[a] << " "<< (S1tot[0]/S3tot[0]) << " " << (S3tot[0]/S5tot[0]) << " ";}
for(int a=0;a<4;a++)cout << Zprofile[a] << endl ;
cout << ShowerLongDisp << " ";
cout << ZprofileChi2 << " " << (Zprofile[2]/Zprofile[3]) << " ";
cout << S13R << " " << Energy3C[1] << " " << Energy3C[2] << " ";
cout <<  nEnergyA << " ";
*/              
    

  ESENEnergyFractionLayer[0] = (EnergyFractionLayer[0] - (0.0293281+log10E*-0.0209319+pow(log10E,2)*-0.00743863+pow(log10E,3)*0.0118679+pow(log10E,4)*-0.00407401+pow(log10E,5)*0.000455905))/( 0.0142375+log10E*0.00526443+pow(log10E,2)*-0.0359919+pow(log10E,3)*0.0329761+pow(log10E,4)*-0.0133547+pow(log10E,5)*0.00258324+pow(log10E,6)*-0.000194743);
  ESENEnergyFractionLayer[1] = (EnergyFractionLayer[1] - (0.0589512+log10E*-0.0105698+pow(log10E,2)*-0.0538665+pow(log10E,3)*0.0424683+pow(log10E,4)*-0.0121717+pow(log10E,5)*0.00123329))/( 0.0227113+log10E*0.0268896+pow(log10E,2)*-0.0854598+pow(log10E,3)*0.0720737+pow(log10E,4)*-0.028848+pow(log10E,5)*0.00569079+pow(log10E,6)*-0.000445795);
  ESENEnergyFractionLayer[2] = (EnergyFractionLayer[2] - (0.113296+log10E*-0.0435857+pow(log10E,2)*-0.0496812+pow(log10E,3)*0.0439499+pow(log10E,4)*-0.0127508+pow(log10E,5)*0.0013001))/( 0.0453848+log10E*-0.00636716+pow(log10E,2)*-0.0536752+pow(log10E,3)*0.055918+pow(log10E,4)*-0.025203+pow(log10E,5)*0.00547468+pow(log10E,6)*-0.000465204);
  ESENEnergyFractionLayer[3] = (EnergyFractionLayer[3] - (0.120157+log10E*0.0127458+pow(log10E,2)*-0.109836+pow(log10E,3)*0.0701623+pow(log10E,4)*-0.0183609+pow(log10E,5)*0.00178725))/( 0.0439958+log10E*-0.00170606+pow(log10E,2)*-0.0424325+pow(log10E,3)*0.038754+pow(log10E,4)*-0.0159361+pow(log10E,5)*0.00318657+pow(log10E,6)*-0.00024982);
  ESENEnergyFractionLayer[4] = (EnergyFractionLayer[4] - (0.116545+log10E*0.0210839+pow(log10E,2)*-0.0576944+pow(log10E,3)*0.0172123+pow(log10E,4)*-0.000713965+pow(log10E,5)*-0.00020084))/( 0.0419422+log10E*-0.0152135+pow(log10E,2)*-0.00849916+pow(log10E,3)*0.0121256+pow(log10E,4)*-0.00583174+pow(log10E,5)*0.00125058+pow(log10E,6)*-9.8688e-05);
  ESENEnergyFractionLayer[5] = (EnergyFractionLayer[5] - (0.107401+log10E*0.0294138+pow(log10E,2)*-0.0292393+pow(log10E,3)*-0.00644733+pow(log10E,4)*0.00594653+pow(log10E,5)*-0.000857954))/( 0.0416871+log10E*-0.0194578+pow(log10E,2)*-0.0165457+pow(log10E,3)*0.03311+pow(log10E,4)*-0.0196423+pow(log10E,5)*0.00501175+pow(log10E,6)*-0.000473562);
  ESENEnergyFractionLayer[6] = (EnergyFractionLayer[6] - (0.0868872+log10E*0.0411799+pow(log10E,2)*-0.0191194+pow(log10E,3)*-0.00996514+pow(log10E,4)*0.00570376+pow(log10E,5)*-0.000760575))/( 0.0400084+log10E*-0.00434858+pow(log10E,2)*-0.0651632+pow(log10E,3)*0.0896122+pow(log10E,4)*-0.0499331+pow(log10E,5)*0.0127892+pow(log10E,6)*-0.00124799);
  ESENEnergyFractionLayer[7] = (EnergyFractionLayer[7] - (0.0725795+log10E*0.0292511+pow(log10E,2)*0.0224447+pow(log10E,3)*-0.0389068+pow(log10E,4)*0.0148578+pow(log10E,5)*-0.00186933))/( 0.0390845+log10E*-0.0102248+pow(log10E,2)*-0.0469026+pow(log10E,3)*0.0635955+pow(log10E,4)*-0.0331956+pow(log10E,5)*0.00794739+pow(log10E,6)*-0.000730641);
  ESENEnergyFractionLayer[8] = (EnergyFractionLayer[8] - (0.0571514+log10E*0.0224879+pow(log10E,2)*0.0399348+pow(log10E,3)*-0.0450408+pow(log10E,4)*0.0162448+pow(log10E,5)*-0.00209165))/( 0.0346756+log10E*-0.00821707+pow(log10E,2)*-0.034565+pow(log10E,3)*0.042904+pow(log10E,4)*-0.0214691+pow(log10E,5)*0.00509811+pow(log10E,6)*-0.000474483);
  ESENEnergyFractionLayer[9] = (EnergyFractionLayer[9] - (0.0457281+log10E*-0.00065603+pow(log10E,2)*0.080106+pow(log10E,3)*-0.0679522+pow(log10E,4)*0.0224519+pow(log10E,5)*-0.00273937))/( 0.0331102+log10E*-0.0175524+pow(log10E,2)*-0.00350307+pow(log10E,3)*0.0106265+pow(log10E,4)*-0.0068344+pow(log10E,5)*0.0020232+pow(log10E,6)*-0.000224017);
  ESENEnergyFractionLayer[10] = (EnergyFractionLayer[10] - (0.034797+log10E*0.000818035+pow(log10E,2)*0.0619266+pow(log10E,3)*-0.0500295+pow(log10E,4)*0.0174051+pow(log10E,5)*-0.00232639))/( 0.029857+log10E*-0.0147422+pow(log10E,2)*-0.00656914+pow(log10E,3)*0.0192983+pow(log10E,4)*-0.0147657+pow(log10E,5)*0.00475027+pow(log10E,6)*-0.000544009);
  ESENEnergyFractionLayer[11] = (EnergyFractionLayer[11] - (0.0265528+log10E*0.0115914+pow(log10E,2)*0.0302168+pow(log10E,3)*-0.0237653+pow(log10E,4)*0.00888894+pow(log10E,5)*-0.00131228))/( 0.0254383+log10E*-0.00799443+pow(log10E,2)*-0.00911492+pow(log10E,3)*0.0146029+pow(log10E,4)*-0.00848828+pow(log10E,5)*0.00216088+pow(log10E,6)*-0.000196073);
  ESENEnergyFractionLayer[12] = (EnergyFractionLayer[12] - (0.0212208+log10E*0.00603145+pow(log10E,2)*0.0340895+pow(log10E,3)*-0.0285554+pow(log10E,4)*0.0115654+pow(log10E,5)*-0.00171917))/( 0.0209154+log10E*0.00334983+pow(log10E,2)*-0.0323003+pow(log10E,3)*0.0396929+pow(log10E,4)*-0.0209712+pow(log10E,5)*0.00501654+pow(log10E,6)*-0.000444993);
  ESENEnergyFractionLayer[13] = (EnergyFractionLayer[13] - (0.0161308+log10E*0.00534484+pow(log10E,2)*0.0218018+pow(log10E,3)*-0.0123567+pow(log10E,4)*0.00387449+pow(log10E,5)*-0.000474708))/( 0.0187966+log10E*-0.00206869+pow(log10E,2)*-0.0121325+pow(log10E,3)*0.015188+pow(log10E,4)*-0.00672142+pow(log10E,5)*0.00111737+pow(log10E,6)*-4.2658e-05);
  ESENEnergyFractionLayer[14] = (EnergyFractionLayer[14] - (0.0118662+log10E*0.00390857+pow(log10E,2)*0.014345+pow(log10E,3)*-0.00556176+pow(log10E,4)*0.00152988+pow(log10E,5)*-0.000172793))/( 0.0160184+log10E*-0.00732929+pow(log10E,2)*0.00556074+pow(log10E,3)*-0.00533376+pow(log10E,4)*0.00497049+pow(log10E,5)*-0.00202253+pow(log10E,6)*0.000274761);
  ESENEnergyFractionLayer[15] = (EnergyFractionLayer[15] - (0.00894285+log10E*0.00723843+pow(log10E,2)*-0.00199131+pow(log10E,3)*0.0107959+pow(log10E,4)*-0.00532545+pow(log10E,5)*0.000851564))/( 0.0126819+log10E*-0.00427508+pow(log10E,2)*0.0116124+pow(log10E,3)*-0.021258+pow(log10E,4)*0.0168832+pow(log10E,5)*-0.00562134+pow(log10E,6)*0.00065933);
  ESENEnergyFractionLayer[16] = (EnergyFractionLayer[16] - (0.00714805+log10E*0.00688464+pow(log10E,2)*-0.005231+pow(log10E,3)*0.0115373+pow(log10E,4)*-0.00518898+pow(log10E,5)*0.000820743))/( 0.0101054+log10E*0.00360477+pow(log10E,2)*-0.0113191+pow(log10E,3)*0.00960592+pow(log10E,4)*-0.0024255+pow(log10E,5)*-8.29438e-06+pow(log10E,6)*4.66736e-05);
  ESENEnergyFractionLayer[17] = (EnergyFractionLayer[17] - (0.00601519+log10E*0.00429147+pow(log10E,2)*-0.00397828+pow(log10E,3)*0.00906065+pow(log10E,4)*-0.0039474+pow(log10E,5)*0.000635338))/( 0.0108678+log10E*-0.0116407+pow(log10E,2)*0.0257701+pow(log10E,3)*-0.032626+pow(log10E,4)*0.0218976+pow(log10E,5)*-0.00685427+pow(log10E,6)*0.00079466);
  ESENLayerSigma[0] = (LayerSigma[0] - (1.1995+log10E*0.67065+pow(log10E,2)*-2.30011+pow(log10E,3)*2.43718+pow(log10E,4)*-0.907874+pow(log10E,5)*0.117796))/( 1.53866+log10E*-0.962565+pow(log10E,2)*0.91356+pow(log10E,3)*-0.259805+pow(log10E,4)*-0.0896804+pow(log10E,5)*0.0630317+pow(log10E,6)*-0.00954929);
  ESENLayerSigma[1] = (LayerSigma[1] - (1.2387+log10E*0.824597+pow(log10E,2)*-2.86718+pow(log10E,3)*2.66005+pow(log10E,4)*-0.998105+pow(log10E,5)*0.137347))/( 1.37315+log10E*-0.868922+pow(log10E,2)*-0.285264+pow(log10E,3)*1.2822+pow(log10E,4)*-0.962437+pow(log10E,5)*0.298924+pow(log10E,6)*-0.033814);
  ESENLayerSigma[2] = (LayerSigma[2] - (0.704584+log10E*0.379729+pow(log10E,2)*-0.779305+pow(log10E,3)*0.557004+pow(log10E,4)*-0.17599+pow(log10E,5)*0.0249407))/( 0.697992+log10E*-0.654891+pow(log10E,2)*1.815+pow(log10E,3)*-2.60611+pow(log10E,4)*1.67659+pow(log10E,5)*-0.487044+pow(log10E,6)*0.0525982);
  ESENLayerSigma[3] = (LayerSigma[3] - (0.785647+log10E*0.357952+pow(log10E,2)*-1.11919+pow(log10E,3)*1.01188+pow(log10E,4)*-0.374269+pow(log10E,5)*0.0517661))/( 0.682365+log10E*-0.128638+pow(log10E,2)*-0.420734+pow(log10E,3)*0.400229+pow(log10E,4)*-0.13364+pow(log10E,5)*0.0166614+pow(log10E,6)*-0.000149023);
  ESENLayerSigma[4] = (LayerSigma[4] - (1.18462+log10E*1.7215+pow(log10E,2)*-4.07251+pow(log10E,3)*2.92371+pow(log10E,4)*-0.870798+pow(log10E,5)*0.0953579))/( 0.998131+log10E*1.03335+pow(log10E,2)*-4.1852+pow(log10E,3)*4.26369+pow(log10E,4)*-2.01173+pow(log10E,5)*0.458226+pow(log10E,6)*-0.0406137);
  ESENLayerSigma[5] = (LayerSigma[5] - (1.21569+log10E*1.91084+pow(log10E,2)*-4.56144+pow(log10E,3)*3.3692+pow(log10E,4)*-1.03287+pow(log10E,5)*0.115188))/( 0.98569+log10E*1.08057+pow(log10E,2)*-4.34727+pow(log10E,3)*4.42545+pow(log10E,4)*-2.08761+pow(log10E,5)*0.476034+pow(log10E,6)*-0.0423323);
  ESENLayerSigma[6] = (LayerSigma[6] - (1.08661+log10E*0.61245+pow(log10E,2)*-1.74063+pow(log10E,3)*1.3976+pow(log10E,4)*-0.452953+pow(log10E,5)*0.0528941))/( 0.796743+log10E*0.36108+pow(log10E,2)*-2.17755+pow(log10E,3)*2.28174+pow(log10E,4)*-1.06068+pow(log10E,5)*0.234624+pow(log10E,6)*-0.0201406);
  ESENLayerSigma[7] = (LayerSigma[7] - (1.11653+log10E*1.06421+pow(log10E,2)*-2.5831+pow(log10E,3)*2.0031+pow(log10E,4)*-0.64257+pow(log10E,5)*0.0743222))/( 0.902575+log10E*-0.240009+pow(log10E,2)*-0.799582+pow(log10E,3)*0.810748+pow(log10E,4)*-0.295105+pow(log10E,5)*0.0438198+pow(log10E,6)*-0.00187547);
  ESENLayerSigma[8] = (LayerSigma[8] - (1.23703+log10E*2.97847+pow(log10E,2)*-6.13093+pow(log10E,3)*4.41739+pow(log10E,4)*-1.36346+pow(log10E,5)*0.153791))/( 1.09765+log10E*0.616413+pow(log10E,2)*-3.2584+pow(log10E,3)*3.13943+pow(log10E,4)*-1.35608+pow(log10E,5)*0.280426+pow(log10E,6)*-0.0225884);
  ESENLayerSigma[9] = (LayerSigma[9] - (1.05628+log10E*4.09418+pow(log10E,2)*-7.74194+pow(log10E,3)*5.4826+pow(log10E,4)*-1.69055+pow(log10E,5)*0.1912))/( 1.08859+log10E*1.02871+pow(log10E,2)*-4.2975+pow(log10E,3)*4.17418+pow(log10E,4)*-1.85598+pow(log10E,5)*0.396456+pow(log10E,6)*-0.0329291);
  ESENLayerSigma[10] = (LayerSigma[10] - (0.878451+log10E*3.27025+pow(log10E,2)*-5.45002+pow(log10E,3)*3.73804+pow(log10E,4)*-1.15754+pow(log10E,5)*0.133677))/( 1.09715+log10E*-0.0871678+pow(log10E,2)*-1.13885+pow(log10E,3)*0.869454+pow(log10E,4)*-0.2249+pow(log10E,5)*0.0125079+pow(log10E,6)*0.00181621);
  ESENLayerSigma[11] = (LayerSigma[11] - (0.7636+log10E*3.70248+pow(log10E,2)*-5.52066+pow(log10E,3)*3.52622+pow(log10E,4)*-1.04084+pow(log10E,5)*0.116271))/( 1.11495+log10E*0.356329+pow(log10E,2)*-2.01241+pow(log10E,3)*1.55344+pow(log10E,4)*-0.487573+pow(log10E,5)*0.0601787+pow(log10E,6)*-0.00132094);
  ESENLayerSigma[12] = (LayerSigma[12] - (0.471052+log10E*5.66989+pow(log10E,2)*-7.99996+pow(log10E,3)*4.90874+pow(log10E,4)*-1.40626+pow(log10E,5)*0.153472))/( 1.0284+log10E*2.2878+pow(log10E,2)*-6.17669+pow(log10E,3)*5.30301+pow(log10E,4)*-2.17963+pow(log10E,5)*0.437728+pow(log10E,6)*-0.0344545);
  ESENLayerSigma[13] = (LayerSigma[13] - (0.422251+log10E*4.55314+pow(log10E,2)*-5.14365+pow(log10E,3)*2.68038+pow(log10E,4)*-0.697187+pow(log10E,5)*0.072939))/( 1.09738+log10E*2.23776+pow(log10E,2)*-6.01791+pow(log10E,3)*5.16537+pow(log10E,4)*-2.154+pow(log10E,5)*0.445421+pow(log10E,6)*-0.0366036);
  ESENLayerSigma[14] = (LayerSigma[14] - (0.332788+log10E*2.47855+pow(log10E,2)*-0.348791+pow(log10E,3)*-0.829789+pow(log10E,4)*0.363045+pow(log10E,5)*-0.0416549))/( 0.848708+log10E*2.61553+pow(log10E,2)*-5.56182+pow(log10E,3)*4.14436+pow(log10E,4)*-1.50977+pow(log10E,5)*0.272793+pow(log10E,6)*-0.0196357);
  ESENLayerSigma[15] = (LayerSigma[15] - (0.25737+log10E*1.56722+pow(log10E,2)*2.11807+pow(log10E,3)*-2.75949+pow(log10E,4)*0.977555+pow(log10E,5)*-0.111681))/( 0.652372+log10E*3.38822+pow(log10E,2)*-5.89548+pow(log10E,3)*3.67127+pow(log10E,4)*-1.03417+pow(log10E,5)*0.119647+pow(log10E,6)*-0.00272773);  ESENLayerSigma[16] = (LayerSigma[16] - (0.234222+log10E*0.636947+pow(log10E,2)*4.52065+pow(log10E,3)*-4.5979+pow(log10E,4)*1.54902+pow(log10E,5)*-0.174913))/( 0.586191+log10E*3.46126+pow(log10E,2)*-4.58645+pow(log10E,3)*1.57458+pow(log10E,4)*0.191528+pow(log10E,5)*-0.194067+pow(log10E,6)*0.0269499);  ESENLayerSigma[17] = (LayerSigma[17] - (0.159336+log10E*-0.0104728+pow(log10E,2)*5.62076+pow(log10E,3)*-5.25354+pow(log10E,4)*1.73196+pow(log10E,5)*-0.19523))/( 0.444376+log10E*3.58308+pow(log10E,2)*-3.7803+pow(log10E,3)*0.494137+pow(log10E,4)*0.702942+pow(log10E,5)*-0.297369+pow(log10E,6)*0.0343403);
  if(Zprofile[3]!=0.) ESENZProfileMaxRatio = (Zprofile[2]/Zprofile[3]  - (1.0129+log10E*-0.763376+pow(log10E,2)*0.988021+pow(log10E,3)*-0.458768+pow(log10E,4)*0.0717106))/( 0.00100992+log10E*0.785363+pow(log10E,2)*-1.44717+pow(log10E,3)*1.12741+pow(log10E,4)*-0.393829+pow(log10E,5)*0.0504198); else ESENZProfileMaxRatio = -10.;
ESENZprofile[3] = (Zprofile[3] - (4.50263+log10E*2.34572+pow(log10E,2)*0.143905+pow(log10E,3)*-0.0833355+pow(log10E,4)*0.0150589))/( 0.0434271+log10E*0.0464243+pow(log10E,2)*-0.0974044+pow(log10E,3)*0.0767125+pow(log10E,4)*-0.0251018+pow(log10E,5)*0.00290821);
ESENZprofile[2] = (Zprofile[2] - (3.67016+log10E*2.50798+pow(log10E,2)*-0.107036+pow(log10E,3)*0.18317+pow(log10E,4)*-0.0449247))/( 1.44785+log10E*0.131643+pow(log10E,2)*-1.50198+pow(log10E,3)*2.22714+pow(log10E,4)*-1.11549+pow(log10E,5)*0.179176);
ESENZprofile[1] = (Zprofile[1] - (0.326949+log10E*0.528463+pow(log10E,2)*-0.670308+pow(log10E,3)*0.339608+pow(log10E,4)*-0.0569746))/( 0.189716+log10E*0.0343689+pow(log10E,2)*-0.251494+pow(log10E,3)*0.255097+pow(log10E,4)*-0.102414+pow(log10E,5)*0.014367);
ESENZprofile[0] = (Zprofile[0] - (0.976082+log10E*-0.0705222+pow(log10E,2)*0.175072+pow(log10E,3)*-0.0914683+pow(log10E,4)*0.0137933))/( 0.165222+log10E*-0.249009+pow(log10E,2)*0.373086+pow(log10E,3)*-0.254006+pow(log10E,4)*0.0733925+pow(log10E,5)*-0.00739734);
ESENZprofileChi2 = (ZprofileChi2v1 - (8.50299+log10E*-13.0275+pow(log10E,2)*8.10907+pow(log10E,3)*-2.32256+pow(log10E,4)*0.254447))/( 3.19023+log10E*-3.17808+pow(log10E,2)*-0.446047+pow(log10E,3)*1.90832+pow(log10E,4)*-0.87448+pow(log10E,5)*0.123363);
ESENShowerFootprint[2] = (ShowerFootprint[2] - (6.30702+log10E*2.64898+pow(log10E,2)*-6.91719+pow(log10E,3)*4.00913+pow(log10E,4)*-0.690694))/( 2.57879+log10E*1.61286+pow(log10E,2)*-6.8939+pow(log10E,3)*5.43924+pow(log10E,4)*-1.7182+pow(log10E,5)*0.195773);
ESENShowerFootprint[1] = (ShowerFootprint[1] - (4.02898+log10E*2.04488+pow(log10E,2)*-3.08223+pow(log10E,3)*1.57129+pow(log10E,4)*-0.256888))/( 1.58523+log10E*-0.295658+pow(log10E,2)*-1.52979+pow(log10E,3)*1.33371+pow(log10E,4)*-0.407086+pow(log10E,5)*0.043129);
ESENShowerFootprint[0] = (ShowerFootprint[0] - (10.7414+log10E*4.30687+pow(log10E,2)*-10.1444+pow(log10E,3)*5.82378+pow(log10E,4)*-1.00416))/( 3.85793+log10E*0.288424+pow(log10E,2)*-6.63226+pow(log10E,3)*5.81433+pow(log10E,4)*-1.91808+pow(log10E,5)*0.224113);
ESENShowerLatDisp[2] = (ShowerLatDisp[2]* (AMSEventR::Head()->Version()<584?1.:(1.+VarLeakYA)) - (19.0553+log10E*54.0248+pow(log10E,2)*-69.1774+pow(log10E,3)*29.9681+pow(log10E,4)*-4.15628))/( 15.5826+log10E*32.9272+pow(log10E,2)*-68.8799+pow(log10E,3)*45.2201+pow(log10E,4)*-12.5163+pow(log10E,5)*1.26381);
ESENShowerLatDisp[1] = (ShowerLatDisp[1] * (AMSEventR::Head()->Version()<584?1.:(1.+VarLeakXA)) - (10.5906+log10E*31.8959+pow(log10E,2)*-35.8796+pow(log10E,3)*13.6127+pow(log10E,4)*-1.66184))/( 8.70862+log10E*17.5474+pow(log10E,2)*-34.899+pow(log10E,3)*22.0739+pow(log10E,4)*-6.09356+pow(log10E,5)*0.634846);
// Remove additionnal correction 
ESENShowerLatDisp[0] = (ShowerLatDisp[0] - (29.8002+log10E*88.9076+pow(log10E,2)*-110.919+pow(log10E,3)*46.8499+pow(log10E,4)*-6.38335))/( 19.7325+log10E*41.8142+pow(log10E,2)*-87.4022+pow(log10E,3)*58.9758+pow(log10E,4)*-17.2272+pow(log10E,5)*1.87766);if(S5tot[2]) ESENS3S5[2] = (S3tot[2]/S5tot[2] - (0.925137+log10E*-0.100376+pow(log10E,2)*0.125556+pow(log10E,3)*-0.0499635+pow(log10E,4)*0.0065578))/( 0.0537135+log10E*-0.00338936+pow(log10E,2)*-0.0120451+pow(log10E,3)*-0.0114605+pow(log10E,4)*0.0100869+pow(log10E,5)*-0.00179196);else ESENS3S5[2] = -20.;
if(S5tot[1]) ESENS3S5[1] = (S3tot[1]/S5tot[1] - (0.931109+log10E*-0.0265477+pow(log10E,2)*0.0315243+pow(log10E,3)*-0.0115648+pow(log10E,4)*0.00144984))/( 0.0478292+log10E*-0.012534+pow(log10E,2)*-0.0458203+pow(log10E,3)*0.044711+pow(log10E,4)*-0.015465+pow(log10E,5)*0.00188226);
else ESENS3S5[1] = -20.;if(S5tot[0]) ESENS3S5[0] = (S3tot[0]/S5tot[0] - (0.921565+log10E*-0.175071+pow(log10E,2)*0.272039+pow(log10E,3)*-0.133165+pow(log10E,4)*0.0210554))/( 0.0428441+log10E*0.157715+pow(log10E,2)*-0.333628+pow(log10E,3)*0.22896+pow(log10E,4)*-0.067359+pow(log10E,5)*0.00729265); else ESENS3S5[0] = -20.;
if(S3tot[0]) ESENS1S3[0] = (S1tot[0]/S3tot[0] - (0.622985+log10E*-0.243001+pow(log10E,2)*0.31769+pow(log10E,3)*-0.122334+pow(log10E,4)*0.0150953))/( 0.0946042+log10E*0.0751587+pow(log10E,2)*-0.0922105+pow(log10E,3)*-0.0159299+pow(log10E,4)*0.0304762+pow(log10E,5)*-0.00617571);
else ESENS1S3[0] = -20.;
ESENShowerLongDisp = (ShowerLongDisp - (11.6608+log10E*2.5556+pow(log10E,2)*-0.379147+pow(log10E,3)*-0.356563+pow(log10E,4)*0.0694614))/( 3.59225+log10E*-1.85411+pow(log10E,2)*-0.283144+pow(log10E,3)*0.856242+pow(log10E,4)*-0.398109+pow(log10E,5)*0.0618494);
ESENS1tot[0] = (S1tot[0] - (0.507233+log10E*-0.357514+pow(log10E,2)*0.593993+pow(log10E,3)*-0.286615+pow(log10E,4)*0.0443787))/( 0.153756+log10E*0.186214+pow(log10E,2)*-0.47659+pow(log10E,3)*0.313955+pow(log10E,4)*-0.0838449+pow(log10E,5)*0.00795356);
ESENEnergy3C3 = (Energy3C[2] - (0.830358+log10E*-0.14142+pow(log10E,2)*0.143214+pow(log10E,3)*-0.0440383+pow(log10E,4)*0.00393432))/( 0.0602811+log10E*0.0689515+pow(log10E,2)*-0.0959921+pow(log10E,3)*0.0137288+pow(log10E,4)*0.0125294+pow(log10E,5)*-0.00318491);
ESENS13R = (S13R - (0.726479+log10E*-0.0412848+pow(log10E,2)*-0.0995643+pow(log10E,3)*0.0911045+pow(log10E,4)*-0.0186589))/( 0.0440288+log10E*-0.00203543+pow(log10E,2)*0.0317023+pow(log10E,3)*-0.0455753+pow(log10E,4)*0.0201134+pow(log10E,5)*-0.00284211);
ESENEnergy3C2 = (Energy3C[1] - (0.977856+log10E*0.0272133+pow(log10E,2)*-0.015759+pow(log10E,3)*0.00327821+pow(log10E,4)*-0.000232916+pow(log10E,5)*0))/( 0.0152631+log10E*-0.0194549+pow(log10E,2)*0.0118655+pow(log10E,3)*-0.00409424+pow(log10E,4)*0.000934227+pow(log10E,5)*-0.000109527+pow(log10E,6)*0);



   const int nLAYERs = 18;
   const int nCELLs  = 72;

   float EneDep = 0.; // Total energy deposited [GeV] (sum of every cell of every layer)

   float F2LEneDep = 0.; // Energy deposit [GeV] in the first 2 layer

   float NEcalHits = Nhits; 
   float L2LFrac = 0.; // Energy fraction of last 2 layer wrt energy deposit
   float F2LFrac = 0.; // Energy fraction of first 2 layer wrt energy deposit
   float ShowerFootprintX = ShowerFootprint[1]; 
   float ShowerFootprintY = ShowerFootprint[2];
   float S3totx = S3tot[1];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S3toty = S3tot[2];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5totx = S5tot[1];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5toty = S5tot[2];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit
   float R3cmFrac = Energy3C[0]; // Energy fraction in a circle +- 3 cm around maximum wrt to total energy
   float R5cmFrac = Energy3C[1]; // Energy fraction in a circle +- 5 cm around maximum wrt to total energy
   float LayerClusterEnergy[nLAYERs];      // Corrected energy deposit [GeV] in each layer (sum of clusters' energy for each layer)
   float ShowerMean  = 0.; // Longitudinal mean [layer]: (sum_i i*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerSigma = 0.; // Longitudinal sigma [layer]: TMath::Sqrt((sum_i (i-ShowerMean)^2*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i]))
   float ShowerSkewness = 0.; // Longitudinal skewness [layer^3]: (sum_i (i-ShowerMean)^3*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerKurtosis = 0.; // Longitudinal kurtosis [layer^4]: (sum_i (i-ShowerMean)^4*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float LayerEnergy = 0.; // sum_i LayerClusterEnergy[i]

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {      LayerClusterEnergy[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell) MapEneDep[ilayer][icell] = 0.;
   }
   Int_t n2DCLUSTERs = NEcal2DCluster();
   for (Int_t i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)   {
      Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);
      Int_t nCLUSTERs = cluster2d->NEcalCluster();      for (Int_t icluster = 0; icluster < nCLUSTERs; ++icluster)
      {         EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);
         LayerClusterEnergy[cluster->Plane] += cluster->Edep;

         Int_t nclHITs = cluster->NEcalHit();
         for (Int_t ihit = 0; ihit < nclHITs; ++ihit)
         {
            EcalHitR *hit = cluster->pEcalHit(ihit);
            if (hit->ADC[0] > 4) MapEneDep[hit->Plane][hit->Cell] = hit->Edep;
         }
      }
   }
   
   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
      for (Int_t icell = 0; icell < nCELLs; ++icell) EneDep += MapEneDep[ilayer][icell];

   EneDep /= 1000.;
   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {      LayerEneDep[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell)      {
         MapEneDep[ilayer][icell] /= 1000.;
         LayerEneDep[ilayer] += MapEneDep[ilayer][icell];
         if (ilayer == 16 || ilayer == 17) L2LFrac += MapEneDep[ilayer][icell];         if (ilayer ==  0 || ilayer ==  1) F2LFrac += MapEneDep[ilayer][icell];
      }

      LayerClusterEnergy[ilayer] /= 1000.;
      LayerEnergy += LayerClusterEnergy[ilayer];
      ShowerMean  += ilayer*LayerClusterEnergy[ilayer];
   }

   F2LEneDep   = F2LFrac;
   L2LFrac     = L2LFrac/EneDep;
   F2LFrac     = F2LFrac/EneDep;
   ShowerMean /= LayerEnergy;

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      ShowerSigma    += TMath::Power(ilayer-ShowerMean, 2)*LayerClusterEnergy[ilayer];
      ShowerSkewness += TMath::Power(ilayer-ShowerMean, 3)*LayerClusterEnergy[ilayer];
      ShowerKurtosis += TMath::Power(ilayer-ShowerMean, 4)*LayerClusterEnergy[ilayer];
   }
   ShowerSigma    /= LayerEnergy;
   ShowerSigma     = TMath::Sqrt(ShowerSigma);
   ShowerSkewness /= LayerEnergy;
   ShowerKurtosis /= LayerEnergy;

   float mean;
   float sigma;


   mean = 4.6675+(0.979051*log(EnergyE+1.18325));
   sigma = 0.852135+(-0.0431518*log(EnergyE));
   ESENShowerMeanNorm = (sigma!=0.)?(ShowerMean - mean)/sigma:-10.;

   mean = 3.53852+(0.0296595*log(EnergyE-4.87199));
   sigma = 0.287891+(-0.0394277*log(EnergyE-4.37052));
   ESENShowerSigmaNorm = (sigma!=0.)?(ShowerSigma - mean)/sigma:-10.;

   mean = 50.7688+(-8.72425*log(EnergyE+10.0236));
   sigma = 8.88718+(-0.533865*log(EnergyE-4.9));
   ESENShowerSkewnessNorm = (sigma!=0.)?(ShowerSkewness - mean)/sigma:-10.;

   mean = (523.084+(-239.06/EnergyE))+(-59.4085*atan(0.0467487*EnergyE));
   sigma = 141.4+(-61.9827*atan(0.132307*EnergyE));
   ESENShowerKurtosisNorm = (sigma!=0.)?(ShowerKurtosis - mean)/sigma:-10.;

   mean = 0.00632314+(0.123049*log(EnergyE+0.353085));
   sigma = -0.110641+(0.0866421*log(EnergyE+7.60773));
   ESENF2LEneDepNorm = (sigma!=0.)?(F2LEneDep - mean)/sigma:-10.;
   
   mean = 0.00981856+(0.00470305*sqrt(EnergyE-3.35461));
  sigma = 0.0129705+(0.00166802*sqrt(EnergyE+8.22741));
   ESENL2LFracNorm = (sigma!=0.)?(L2LFrac - mean)/sigma:-10.;
         
   mean = -0.00453365+(0.122221*(pow(EnergyE,(float)-0.513752)));
   sigma = -0.00315033+(0.107401*(pow(EnergyE,(float)-0.543919)));
   ESENF2LFracNorm = (sigma!=0.)?(F2LFrac - mean)/sigma:-10.;
         
   mean = 0.972864+(-0.00499681*atan((EnergyE*0.0362795)-0.459303));
   sigma = 0.0104377+(-0.00140507*log(EnergyE-3.97738));
   ESENR3cmFracNorm = (sigma!=0.)?(R3cmFrac - mean)/sigma:-10.;
      
   mean = (1.01319+(-0.00461599*log(EnergyE)))+(-0.113443/(EnergyE+4.50707));
   sigma = 0.0043004+(-0.000536274*log(EnergyE-4.9));
   ESENR5cmFracNorm = (sigma!=0.)?(R5cmFrac - mean)/sigma:-10.;

   mean = 0.0180086+(-0.0100441*log(EnergyE+9.07422));
   sigma = 0.505472+(-0.311353*atan((0.518772*EnergyE)+3.74849));
   ESENDifoSumNorm = (sigma!=0.)?(DifoSum - mean)/sigma:-10.;
   
   mean = 0.91946+(-0.022189*atan((2.52632*EnergyE)-7.67717));
   sigma = 0.00789299+(0.0720622*(pow(EnergyE,(float)-0.733793)));
   ESENS3totxNorm = (sigma!=0.)?(S3totx - mean)/sigma:-10.;
      
   mean = (-0.0136692*exp(-0.128828*EnergyE))+0.874554; 
   sigma = 0.501058+(-0.314568*atan((1.2783*EnergyE)+2.90362));
   ESENS3totyNorm = (sigma!=0.)?(S3toty - mean)/sigma:-10.;
   
   mean = 0.960591+(-0.00355234*log(EnergyE));
   sigma = 0.0217616+(-0.0107094*atan(0.079457*EnergyE));
   ESENS5totxNorm = (sigma!=0.)?(S5totx - mean)/sigma:-10.;

   mean = 0.948521+(-0.00193183*log(EnergyE));
   sigma = 0.0227471+(-0.00393239*log(EnergyE-4.78839));
   ESENS5totyNorm = (sigma!=0.)?(S5toty - mean)/sigma:-10.;

   mean = (55.4579+(21.667*log(EnergyE)))+(4.34059*(log(EnergyE)*log(EnergyE)));
   sigma = (6.88496+(0.316054*log(EnergyE)))+(0.349772*(log(EnergyE)*log(EnergyE)));
   ESENEcalHitsNorm = (sigma!=0.)?(NEcalHits - mean)/sigma:-10.;
   
   mean = 3.94369+(0.00837397*(pow(EnergyE,(float)0.888997)));
   sigma = 2.04609+(-1.15052*atan((0.278696*EnergyE)+0.639821));
   ESENShowerFootprintXNorm = (sigma!=0.)?(ShowerFootprintX - mean)/sigma:-10.;
   
   mean = (5.23776+(-0.210765*log(EnergyE)))+(0.00721687*EnergyE);
   sigma = 1.54466+(-0.845935*atan(EnergyE*0.116966));
   ESENShowerFootprintYNorm = (sigma!=0.)?(ShowerFootprintY - mean)/sigma:-10.;

  if(isnan(ESENShowerMeanNorm)) ESENShowerMeanNorm = -10.;
   if(isnan(ESENShowerSigmaNorm)) ESENShowerSigmaNorm = -10.;
  if(isnan(ESENShowerSkewnessNorm)) ESENShowerSkewnessNorm = -10.;
   if(isnan(ESENShowerKurtosisNorm)) ESENShowerKurtosisNorm = -10.;
   if(isnan(ESENF2LEneDepNorm)) ESENF2LEneDepNorm = -10.;
   if(isnan(ESENL2LFracNorm)) ESENL2LFracNorm = -10.;
   if(isnan(ESENF2LFracNorm)) ESENF2LFracNorm = -10.;
   if(isnan(ESENR3cmFracNorm)) ESENR3cmFracNorm = -10.;
   if(isnan(ESENR5cmFracNorm)) ESENR5cmFracNorm = -10.;
   if(isnan(ESENDifoSumNorm)) ESENDifoSumNorm = -10.;
   if(isnan(ESENS3totxNorm)) ESENS3totxNorm = -10.;
   if(isnan(ESENS3totyNorm)) ESENS3totyNorm = -10.;
   if(isnan(ESENS5totxNorm)) ESENS5totxNorm = -10.;
   if(isnan(ESENS5totyNorm)) ESENS5totyNorm = -10.;
   if(isnan(ESENEcalHitsNorm)) ESENEcalHitsNorm = -10.;
   if(isnan(ESENShowerFootprintXNorm)) ESENShowerFootprintXNorm = -10.;
   if(isnan(ESENShowerFootprintYNorm)) ESENShowerFootprintYNorm = -10.;

   TString vname,varset0;
   if(!NESEreaderv2){
   	NESEreaderv2 = new TMVA::Reader( "!Color:!Silent" );
	for(int aa=0;aa<18;aa++){
		vname = "N3LayerSigma[";
                vname += aa;
                               vname += "]";
                                NESEreaderv2->AddVariable(vname.Data(),&ESENLayerSigma[aa]);
                        }

                        for(int aa=0;aa<18;aa++){
                                vname = "N3EnergyFractionLayer[";
                                vname += aa;
                                vname += "]";
                                NESEreaderv2->AddVariable(vname.Data(),&ESENEnergyFractionLayer[aa]);
                        }


                        NESEreaderv2->AddVariable("N2Energy3C2",&ESENEnergy3C2);
                        NESEreaderv2->AddVariable("N2S13R",&ESENS13R);
                        NESEreaderv2->AddVariable("N2Energy3C3",&ESENEnergy3C3);
                        NESEreaderv2->AddVariable("N2ShowerLongDisp",&ESENShowerLongDisp);
                        NESEreaderv2->AddVariable("N2S1S3",&ESENS1S3[0]);
                        NESEreaderv2->AddVariable("N2S3S5",&ESENS3S5[0]);
                        NESEreaderv2->AddVariable("N2S3S5x",&ESENS3S5[1]);
                        NESEreaderv2->AddVariable("N2S3S5y",&ESENS3S5[2]);
                        NESEreaderv2->AddVariable("N2ShowerLatDispx",&ESENShowerLatDisp[1]);
                        NESEreaderv2->AddVariable("N2ShowerLatDispy",&ESENShowerLatDisp[2]);
                        NESEreaderv2->AddVariable("N2ShowerFootprint",&ESENShowerFootprint[0]);
                        NESEreaderv2->AddVariable("N2ShowerFootprintx",&ESENShowerFootprint[1]);
                        NESEreaderv2->AddVariable("N2ShowerFootprinty",&ESENShowerFootprint[2]);
                        NESEreaderv2->AddVariable("N2ZProfileMaxRatio",&ESENZProfileMaxRatio);
                        NESEreaderv2->AddVariable("N2Zprofile0",&ESENZprofile[0]);

                        NESEreaderv2->AddVariable("ShowerMeanNorm",&ESENShowerMeanNorm);
                        NESEreaderv2->AddVariable("ShowerSigmaNorm",&ESENShowerSigmaNorm);
                        NESEreaderv2->AddVariable("ShowerSkewnessNorm",&ESENShowerSkewnessNorm);
                        NESEreaderv2->AddVariable("ShowerKurtosisNorm",&ESENShowerKurtosisNorm);
                        NESEreaderv2->AddVariable("F2LEneDepNorm",&ESENF2LEneDepNorm);
                        NESEreaderv2->AddVariable("L2LFracNorm",&ESENL2LFracNorm);
                        NESEreaderv2->AddVariable("F2LFracNorm",&ESENF2LFracNorm);
                        NESEreaderv2->AddVariable("R3cmFracNorm",&ESENR3cmFracNorm);
                        NESEreaderv2->AddVariable("R5cmFracNorm",&ESENR5cmFracNorm);
                        NESEreaderv2->AddVariable("DifoSumNorm",&ESENDifoSumNorm);
                        NESEreaderv2->AddVariable("S3totxNorm",&ESENS3totxNorm);
                        NESEreaderv2->AddVariable("S3totyNorm",&ESENS3totyNorm);
                        NESEreaderv2->AddVariable("S5totxNorm",&ESENS5totxNorm);
                        NESEreaderv2->AddVariable("S5totyNorm",&ESENS5totyNorm);
                        NESEreaderv2->AddVariable("NEcalHitsNorm",&ESENEcalHitsNorm);
                        NESEreaderv2->AddVariable("ShowerFootprintXNorm",&ESENShowerFootprintXNorm);
                        NESEreaderv2->AddVariable("ShowerFootprintYNorm",&ESENShowerFootprintYNorm);
   
   
                        NESEreaderv2->AddSpectator("EnergyC",&nEnergyC);
                        NESEreaderv2->AddSpectator("EcalStandaloneEstimator",&nEcalStandaloneEstimator);
                        NESEreaderv2->AddSpectator("ecalBDT",&necalBDT);
                        NESEreaderv2->AddSpectator("Momentum",&nMomentum);
                        NESEreaderv2->AddSpectator("EnergyA",&nEnergyA);
                        NESEreaderv2->AddSpectator("EnergyE",&nEnergyE);
                        NESEreaderv2->AddSpectator("RigInn",&nRigInn);
                        
                                
                        char name[801]="";
                        sprintf(name,"%s/%s/TMVA_ePrej_V3_12_BCat.vc.weights.xml",getenv("AMSDataDir"),AMSCommonsI::getversion());
                        NESEreaderv2->BookMVA("BCat method" , name);
                }       

                        
        float vMean[11] = {0.,0.,0.0806513,0.0840544,0.101147,0.0780171,0.107208,0.164722,0.229185,0.230659,0.17618};
        float vRMS[11] = {1.,1.,0.0374293,0.0319869,0.0365631,0.0359361,0.0458087,0.0721709,0.0926243,0.116207,0.0721707};
        
        if(log10E>=2.5){        
                log10E = 2.4;
                nEnergyA = 250000.;
        }
        float output = NESEreaderv2->EvaluateMVA("BCat method");
        if(log10E>2.){  
                return (output-vMean[10])/vRMS[10];
        }               
        for(int a=0;a<10;a++){
                if(log10E>(a*0.2) && log10E<=((a+1)*0.2)){
                        return (output-vMean[a])/vRMS[a];
                }       
        }               
        return 0.;      
}                       




float EcalShowerR::EcalStandaloneEstimator(){
	// Normalise variables first	
     	NormaliseVariableLAPP();

	double estimator = -20.;
	float log10E = TMath::Log10(EnergyC);
	if(log10E<=0.65||log10E>=2.5)
		return estimator;


	cEnergyC = EnergyC;
	
	varBDT[0] = NEnergyFractionLayer[0];         
	varBDT[1] = NEnergyFractionLayer[1];        
	varBDT[2] = NEnergyFractionLayer[2];        
	varBDT[3] = NEnergyFractionLayer[7];        
	varBDT[4] = NEnergyFractionLayer[8];        
	varBDT[5] = NEnergyFractionLayer[9];        
	varBDT[6] = NEnergy3C2;          
	varBDT[7] = NS13R;           
	varBDT[8] = NEnergy3C3;           
	varBDT[9] = NS1tot[0];           
	varBDT[10] = NShowerLongDisp;       
	varBDT[11] = NS1S3[0];   
	varBDT[12] = NS3S5[0];                 
	varBDT[13] = NS3S5[1];                
	varBDT[14] = NS3S5[2];                
	varBDT[15] = NShowerLatDisp[0];        
	varBDT[16] = NShowerLatDisp[1];       
	varBDT[17] = NShowerLatDisp[2];       
	varBDT[18] = NShowerFootprint[0];     
	varBDT[19] = NShowerFootprint[1];    
	varBDT[20] = NShowerFootprint[2];
	varBDT[21] = NZProfileMaxRatio; 
	varBDT[22] = NZprofile[0];
	float Momentum = 1.;
	if(!ecallappreader){ 
		ecallappreader = new TMVA::Reader( "!Color:!Silent" );
		std::cout << "##############################################################" << std::endl;
		std::cout << "                   Create ECAL LAPP Reader  v02 " << std::endl;
		std::cout << "##############################################################" << std::endl;
		

                ecallappreader->AddVariable("NEnergyFractionLayer[0]",&varBDT[0]);
                ecallappreader->AddVariable("NEnergyFractionLayer[1]",&varBDT[1]);
                ecallappreader->AddVariable("NEnergyFractionLayer[2]",&varBDT[2]);
                ecallappreader->AddVariable("NEnergyFractionLayer[7]",&varBDT[3]);
                ecallappreader->AddVariable("NEnergyFractionLayer[8]",&varBDT[4]);
                ecallappreader->AddVariable("NEnergyFractionLayer[9]",&varBDT[5]);
                ecallappreader->AddVariable("NEnergy3C2",&varBDT[6]);
                ecallappreader->AddVariable("NS13R",&varBDT[7]);
                ecallappreader->AddVariable("NEnergy3C3",&varBDT[8]);
                ecallappreader->AddVariable("NS1tot[0]",&varBDT[9]);
                ecallappreader->AddVariable("NShowerLongDisp",&varBDT[10]);
                ecallappreader->AddVariable("NS1S3",&varBDT[11]);
                ecallappreader->AddVariable("NS3S5",&varBDT[12]);
                ecallappreader->AddVariable("NS3S5x",&varBDT[13]);
                ecallappreader->AddVariable("NS3S5y",&varBDT[14]);
                ecallappreader->AddVariable("NShowerLatDispx",&varBDT[16]);
                ecallappreader->AddVariable("NShowerLatDispy",&varBDT[17]);
                ecallappreader->AddVariable("NShowerFootprint",&varBDT[18]);
                ecallappreader->AddVariable("NShowerFootprintx",&varBDT[19]);
                ecallappreader->AddVariable("NShowerFootprinty",&varBDT[20]);
                ecallappreader->AddVariable("NZProfileMaxRatio",&varBDT[21]);
                ecallappreader->AddVariable("NZprofile0",&varBDT[22]);
        	ecallappreader->AddSpectator("EnergyC",&cEnergyC);
        	ecallappreader->AddSpectator("Momentum",&Momentum);


		char name[801]="";
        	sprintf(name,"%s/%s/ECAL_LAPP_BDT_v02.weights.xml",getenv("AMSDataDir"),AMSCommonsI::getversion());
		ecallappreader->BookMVA("BCat method" , name);
	}

	estimator = ecallappreader->EvaluateMVA("BCat method");

	// Normalise output in energy bins
	if(log10E>0.65 && log10E<0.8) estimator = (estimator - 0.0887863) / 0.0332431; 
	if(log10E>=0.8 && log10E<1.) estimator = (estimator - 0.0929854) / 0.0319299; 
	if(log10E>=1. && log10E<1.2) estimator = (estimator - 0.0977055) / 0.0333698; 
	if(log10E>=1.2 && log10E<1.4) estimator = (estimator - 0.157461) / 0.0599707; 
	if(log10E>=1.4 && log10E<1.6) estimator = (estimator - 0.197333) / 0.0701744; 
	if(log10E>=1.6 && log10E<1.8) estimator = (estimator - 0.287696) / 0.0939257; 
	if(log10E>=1.8 && log10E<2.) estimator = (estimator - 0.444025) / 0.119273; 
	if(log10E>=2. && log10E<2.5) estimator = (estimator - 0.529917) / 0.120451; 


	return estimator;
}


float EcalShowerR::EcalChargeEstimator() {


	AMSEventR *ev=AMSEventR::Head();


//	if (ev->NEcalShower() != 1 ) return 0;



	// ----------  Number of layers at MIP


	// Filling the Map

	float hMap[18][72]={{0}};
	float aE[18]={0}; // energy 	for each layer
	float aX[18]={0}; // PL 		for each layer

	for (Int_t i2dcluster = 0; i2dcluster < NEcal2DCluster(); ++i2dcluster){
		for (Int_t icluster = 0; icluster < pEcal2DCluster(i2dcluster)->NEcalCluster(); ++icluster){
			for (Int_t ihit = 0; ihit < pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->NEcalHit(); ++ihit){
				EcalHitR *hit = pEcal2DCluster(i2dcluster)->pEcalCluster(icluster)->pEcalHit(ihit);
				hMap[hit->Plane][hit->Cell]=hit->ADC[0];
				aE[hit->Plane] += hit->Edep;
			}
		}	
	}

	hMap[6][50]=0; // Noisy cell

	// Tests on each layer


	

	int nLay=0;

		
	for (nLay=0; nLay<18; nLay++) {
		float max =	3;
		int cellmax = -1;
		bool isAdj=false; // not 3 adjacent cells

		for (int cell=0;cell<72;cell++) {
			if (hMap[nLay][cell]>max) {
				max=hMap[nLay][cell];
				cellmax=cell;
			}
		}
		if (cellmax==-1) break;

		float S3=hMap[nLay][cellmax];

		if (cellmax>0) { // not first cell
			S3+=hMap[nLay][cellmax-1];
			for (int cell=0;cell<cellmax-2;cell++ )
				if (hMap[nLay][cell]>3) break;
			if (hMap[nLay][cellmax-1]) isAdj=true;
		}

		if (cellmax<71) { // not last cell
			S3+=hMap[nLay][cellmax+1];
			for (int cell=cellmax+2;cell<72;cell++ ) 
				if (hMap[nLay][cell]>3) break;
			if (hMap[nLay][cellmax+1] && isAdj) break;
		}

		float S5=S3;
		if (cellmax>1)  S5+=hMap[nLay][cellmax-2];
		if (cellmax<70) S5+=hMap[nLay][cellmax+2];
		if (S3/S5<0.98) break;

	} // End loop on planes



	

	// Got MIP ?

	if (nLay==0) return 0;


	// ----------  We have nLay layers at MIP
	
	// Identifying particle pointer

	ParticleR* part;
	int particleid=-1;

  	for (int iparticle = 0; iparticle < ev->nParticle(); ++iparticle)
	{
		part = ev->pParticle(iparticle);
		if (part->pEcalShower() == this)
		{
			particleid = iparticle;
			break;
		}
	}
	if(particleid<0) return 0;


	// Path length


		

	// Particle coordinates
	float x0=part->EcalCoo[0][0];
	float y0=part->EcalCoo[0][1];
	float z0=part->EcalCoo[0][2];
	float x1=part->EcalCoo[1][0];
	float y1=part->EcalCoo[1][1];
	float z1=part->EcalCoo[1][2];
	float xe, ye, ze, xf, yf, zf;

	float EMV=0;


	int cnt=0;
	
	for (int il=0; il<nLay; il++) {
		xe=(x1-x0)*il/18.; xf=(x1-x0)*(il+1)/18.;
		ye=(y1-y0)*il/18.; yf=(y1-y0)*(il+1)/18.;
		ze=(z1-z0)*il/18.; zf=(z1-z0)*(il+1)/18.;
		aX[il]=sqrt( ((xe-xf)*(xe-xf)) +((ye-yf)*(ye-yf)) +((ze-zf)*(ze-zf)) )*0.3923 ;
		if (aE[il]==0) break;
		if (aX[il]==0 || isnan(aX[il])) continue;
		cnt++;
		EMV+=log10(aE[il]/aX[il]);
	}

	if (cnt==0) 	return 0;

	
	EMV/=float(cnt);

	EMV=pow(10, (EMV-1.46)/2)/1.43;


	if (EMV > 7) {

		static const int nb=13;
		float xk[nb]={ 1,				2,					3,				4,					5,					6, 			7, 			7.66, 				9.26, 		10.72, 			12.10, 		13.55, 		20.00};
		float yk[nb]={ 1,				2,					3,				4,					5,					6, 			7, 			8, 				10,				12, 				14, 			16, 			26};
		float b[nb]= {1.001827e+00, 9.990863e-01, 1.001827e+00, 9.936044e-01, 1.023755e+00, 9.113757e-01, 1.330742e+00, 1.505884e+00, 1.230221e+00, 1.451748e+00, 1.421522e+00, 1.350610e+00, 2.017511e+00};
		float c[nb]={-2.740957e-03, 0.000000e+00, 2.740957e-03, -1.096383e-02, 4.111435e-02, -1.534936e-01, 5.728600e-01, -3.074936e-01, 1.352044e-01, 1.652662e-02, -3.842955e-02, -1.047562e-02, 6.450000e+00};
		float d[nb]={9.136522e-04, 9.136522e-04, -4.568261e-03, 1.735939e-02, -6.486931e-02, 2.421178e-01, -4.446231e-01, 9.222871e-02, -2.709537e-02, -1.327444e-02, 6.426191e-03, 6.426191e-03, 6.380541e-01,};

		int iquench=0;
		if (EMV>xk[nb-1]) iquench=nb-1;
		else while (EMV > xk[iquench+1]) iquench++;
		float dx=EMV-xk[iquench];
		
		EMV=yk[iquench]+dx*(b[iquench]+dx*(c[iquench]+dx*(d[iquench])));
			
	}

	return EMV;

}













Level1R::Level1R(Trigger2LVL1 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  PhysBPatt = ptr->_PhysBPatt;
  JMembPatt = ptr->_JMembPatt;
  AuxTrigPatt = ptr->_AuxTrigPatt;
  TofFlag1 = ptr->_tofflag1;
  TofFlag2 = ptr->_tofflag2;
  for (int i=0; i<4; i++) {
    TofPatt1[i] = ptr->_tofpatt1[i];
    TofPatt2[i] = ptr->_tofpatt2[i];
  }
  AntiPatt = ptr->_antipatt;
  EcalFlag = ptr->_ecalflag;
  for(int i=0;i<6;i++){
    for(int j=0;j<3;j++)EcalPatt[i][j] = ptr->_ectrpatt[i][j];
  }
  EcalTrSum= ptr->_ectrsum;
  LiveTime   = ptr->_LiveTime;
  for(int i=0; i<19; i++)TrigRates[i]  = ptr->_TrigRates[i];
  for(unsigned int i=0; i<sizeof(TrigTime)/sizeof(TrigTime[0]); i++){
    TrigTime[i]  = ptr->_TrigTime[i];
  }
#endif
}



Level3R::Level3R(TriggerLVL302 *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  TOFTr     = ptr->_TOFTrigger;
  TRDTr     = ptr->_TRDTrigger;
  TrackerTr = ptr->_TrackerTrigger;
  MainTr    = ptr->_MainTrigger;
  Direction = ptr->_TOFDirection;
  NTrHits   = ptr->_NTrHits;
  NPatFound = ptr->_NPatFound;
  for (int i=0; i<2; i++) {Pattern[i] = ptr->_Pattern[i];}
  for (int i=0; i<2; i++) {Residual[i]= ptr->_Residual[i];}
  Time      = ptr->_Time;
  ELoss     = ptr->_TrEnergyLoss;
  TRDHits   = ptr->TRDAux._NHits[0]+ptr->TRDAux._NHits[1];;
  HMult     = ptr->TRDAux._HMult;;
  for (int i=0; i<2; i++) {TRDPar[i] = ptr->TRDAux._Par[i][0];}
  ECemag    = ptr->_ECemag;
  ECmatc    = ptr->_ECmatc;
  for (int i=0; i<4; i++) {ECTOFcr[i] = ptr->_ECtofcr[i];}

#endif
}

MCEventgR::MCEventgR(AMSmceventg *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Nskip    = ptr->_nskip;
  Particle = ptr->_ipart;
  trkID    = ptr->_trkid;
  parentID = ptr->_parentid;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) Dir[i] = ptr->_dir[i];
  Momentum = ptr->_mom;
  Mass     = ptr->_mass;
  Charge   = ptr->_charge;
  TBl   = ptr->_tbline;
  PartInfo   = ptr->_parinfo;
#endif
}           

MCTrackR::MCTrackR(AMSmctrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  RadL = ptr->_radl;
  AbsL = ptr->_absl;
  StepL = ptr->_stlen;
  EneTot = ptr->_enetot;
  EneIon = ptr->_eneion;
  fEl=ptr->_fEl;
  trkID = ptr->_tid;
  for (int i=0; i<3; i++) Pos[i]   = ptr->_pos[i];
  for (int i=0; i<4; i++) VolName[i] = ptr->_vname[i];
  VolName[4]='\0';
#endif
}

#ifndef _PGTRACK_
VertexR::VertexR(AMSVtx *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Momentum=ptr->getmom();
  ErrMomentum=ptr->geterrmom();
  Theta=ptr->gettheta();
  Phi=ptr->getphi();
  Mass=ptr->getmass();
  Status=ptr->getstatus();
  Charge=ptr->getcharge();
  Chi2=ptr->getchi2();
  Ndof=ptr->getndof();
  for(int i=0;i<3;i++)Vertex[i]=ptr->getvert()[i];
#endif
}
#endif

ParticleR::ParticleR(AMSParticle *ptr, float phi, float phigl)
{
#ifndef __ROOTSHAREDLIBRARY__
  Phi      = phi;
  PhiGl    = phigl;
  fBeta   = -1;
  fBetaH  = -1;
  fCharge = -1;
  fTrTrack  = -1;
  fTrdTrack    = -1;
  fTrdHTrack   = -1;
  fRichRing   = -1;
  fEcalShower = -1;
  fVertex = -1;
  fRichRingB = -1;
  Particle     = ptr->_gpart[0];
  ParticleVice = ptr->_gpart[1];
  for (int i=0; i<2; i++) {Prob[i] = (float)ptr->_prob[i];}
  ErrMomentum = (float) ptr->_ErrMomentum;
  Mass     =(float) ptr->_Mass;
  FitMom   = (float) ptr->_fittedmom[0];
  ErrMass  = (float)ptr->_ErrMass;
  Momentum = (float)ptr->_Momentum;
  Beta     = (float)ptr->_Beta;
  ErrBeta  = (float)ptr->_ErrBeta;
  Charge   = ptr->_Charge;
  Theta    = ptr->_Theta;
  ThetaGl  = ptr->_ThetaGl;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_Coo[i];}
  Cutoff   = ptr->_CutoffMomentum;
  for (int i=0; i<4; i++) {
    TOFTLength[i]=ptr->_TOFTLength[i];
    for (int j=0; j<3; j++) {
      TOFCoo[i][j] = ptr->_TOFCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      if(j<3)AntiCoo[i][j] = ptr->_AntiCoo[i][j];
      else AntiCoo[i][j] = ptr->_AntiCrAngle[i][j-3];
    }
  }
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      EcalCoo[i][j] = ptr->_EcalSCoo[i][j];
    }
  }
#ifdef _PGTRACK_
  for (int i=0; i<TkDBc::Head->nlay(); i++) 
#else
  for (int i=0; i<TKDBc::nlay(); i++) 
#endif
    {
    for (int j=0; j<3; j++) {
      TrCoo[i][j] = ptr->_TrCoo[i][j];
    }
  }
#ifdef _PGTRACK_
  for (int i=0; i<TkDBc::Head->nlay(); i++) 
#else
  for (int i=0; i<TKDBc::nlay(); i++) 
#endif
    Local[i] = ptr->_Local[i];

  for (int i=0; i<3; i++) {TRDCoo[0][i] = ptr->_TRDCoo[0][i];}
  for (int i=0; i<3; i++) {TRDCoo[1][i] = ptr->_TRDCoo[1][i];}
  for (int i=0; i<2; i++) {
    for (int j=0; j<3; j++) {
      RichCoo[i][j] = ptr->_RichCoo[i][j];
    }
  }
  for (int i=0; i<2; i++) {RichPath[i] = ptr->_RichPath[i];
    RichPathBeta[i] = ptr->_RichPathBeta[i];}
  RichLength = ptr->_RichLength;
  RichParticles= ptr->_RichParticles;

  TRDHLikelihood = ptr->_TRDHLikelihood;
  TRDHElik = ptr->_TRDHElik;
  TRDHPlik = ptr->_TRDHPlik;
  TRDCCnhit = ptr->_TRDCCnhit;

  TrdSH_E2P_Likelihood  = ptr->_TrdSH_E2P_lik;
  TrdSH_He2P_Likelihood = ptr->_TrdSH_He2P_lik;
  TrdSH_E2He_Likelihood = ptr->_TrdSH_E2He_lik;

  CutoffS = GetGeoCutoff(NULL);  /// Stoermer Geomagnetic Cutoff in GeV/c

  BT_result = BT_status = -1;
  BT_glong = BT_glat = BT_RPTO[0] = BT_RPTO[1] = BT_RPTO[2] = BT_time = 0;

  if (BACKTRACEFFKEY.enable) {

#ifdef _PGTRACK_
    bool istrack = (ptr->_ptrack && !ptr->_ptrack->IsFake());
#else
    bool istrack =  ptr->_ptrack;
#endif
    bool isshwr  = (ptr->_pShower &&
		    ptr->_pShower->getEnergy()*1e-3 > BACKTRACEFFKEY.thengd);
    bool isvtx   = ptr->_pvert;

    BT_result = 0;
    enum { bENGY = 8 };

    double msave = Momentum;
    if (!istrack && isshwr) {
      Momentum = (Momentum > 0) ?  ptr->_pShower->getEnergyPIC()
	                        : -ptr->_pShower->getEnergyPIC();
      BT_result |= (1<<bENGY);
    }
    int ret = -1;
    if (BACKTRACEFFKEY.enable != 1 || istrack || isshwr || isvtx) 
      ret = DoBacktracing(BACKTRACEFFKEY.out_type);
    if (BT_status < 0) BT_result = -1;

#ifdef _PGTRACK_
    if (isvtx && ret == 0 && BT_status == 4 &&
	ptr->_pvert &&
	ptr->_pvert->IsPhotonVertex() &&
	ptr->_pvert->checkstatus(AMSDBc::GOOD)) {
      cout << "ParticleR::ParticleR-I-phvtx candidate @"
	   << AMSEvent::gethead()->getEvent() << " "
	   << AMSEvent::gethead()->getrun() << " coo= "
	   << BT_glong << " " << BT_glat << endl;

      hman.Fill("GmBTph", -BT_glong, BT_glat);
    }
#endif

    Momentum = msave;
  }
#endif
}

int ParticleR::DoBacktracing(int out_type)
{
#ifndef __ROOTSHAREDLIBRARY__
  EventNtuple02 *ptr = AMSJob::gethead()->getntuple()->Get_event02();
  if (!ptr) return -1;

  enum { bSTK  = 0, bINTL = 1, bTLE  = 2, bGTOD = 3, bCTRS = 4, bGPSW = 5,
	 bGPST = 6, bGPSC = 7 };

  AMSDir dir(Theta, Phi);
  int    ichg = Charge;
  double beta = Beta;
  double momt = Momentum;

  if (beta < 0) { dir  = dir*(-1); beta = -beta; }
  if (momt < 0) { momt = -momt;    ichg = -ichg; }
  if (momt > 1e5) ichg = 0;

  // Force as photons
  if (BACKTRACEFFKEY.enable == 3) ichg = 0;

  GeoMagTrace::DEBUG    = BACKTRACEFFKEY.debug;
  GeoMagTrace::StepCv   = BACKTRACEFFKEY.stepdv;
  GeoMagTrace::MinStep  = BACKTRACEFFKEY.minstep*GeoMagTrace::Re;
  GeoMagTrace::MaxStep  = BACKTRACEFFKEY.maxstep*GeoMagTrace::Re;
  GeoMagTrace::NmaxStep = BACKTRACEFFKEY.nmax;

  if (GeoMagField::GetInitStat() == 0) GeoMagField::GetHead();
  if (GeoMagField::GetInitStat() <  0) return -2;

  static bool first = true;
#pragma omp critical (DoBacktracing)
  if (first) {
    cout << "ParticleR::DoBacktracing-I-Parameters= "
	 << BACKTRACEFFKEY.stepdv  << " "
	 << BACKTRACEFFKEY.minstep << " " << BACKTRACEFFKEY.maxstep << " "
	 << BACKTRACEFFKEY.nmax << endl;
    first = false;
  }

  AMSgObj::BookTimer.start("ReAxDoBacktr");

  int    utime  =        ptr->Time[0];
  double tfrac  = double(ptr->Time[1])/1000000;
  double xtime  = double(utime)+tfrac-AMSEventR::gpsdiff(utime);

  double YPR[3] = { ptr->Yaw,       ptr->Pitch,  ptr->Roll };
  double RPT[3] = { ptr->RadS,      ptr->PhiS,   ptr->ThetaS };
  double VPT[3] = { ptr->VelocityS, ptr->VelPhi, ptr->VelTheta };

  if (AMSEventR::getsetup()) {
    double tcorr = 0, terr = 0;
    AMSEventR::getsetup()->GetJMDCGPSCorr(tcorr, terr, utime);
    xtime += tcorr;

    double dt = 0.5;
    float RTP[3], VTP[3];
    if (AMSEventR::getsetup()->getISSTLE(RTP, VTP, xtime+dt) == 0) {
      RPT[0] = RTP[0];
      RPT[1] = RTP[2];
      RPT[2] = RTP[1];
      VPT[0] = VTP[0];
      VPT[1] = VTP[2];
      VPT[2] = VTP[1];
    }

    float roll = 0, pitch = 0, yaw = 0;
    if (AMSEventR::getsetup()->getISSAtt(roll, pitch, yaw, xtime) == 0) {
      YPR[0] = yaw;
      YPR[1] = pitch;
      YPR[2] = roll;
    }
  }

  if (ichg == 0) {
    AMSDir dir(Theta, Phi);
    if (dir.z() < 0) dir = dir*(-1);

    double x = dir.x(), y = dir.y(), z = dir.z();
    double glon = 0, glat = 0;
    if (out_type == 1)
      get_ams_l_b_fromGTOD   (x, y, z, glon, glat, RPT, VPT, YPR, xtime);
    else if (out_type == 2)
      get_ams_ra_dec_fromGTOD(x, y, z, glon, glat, RPT, VPT, YPR, xtime);
    else if (out_type == 3)
      get_ams_gtod_fromGTOD  (x, y, z, glon, glat, RPT, VPT, YPR, xtime);
    else return -1;

    BT_glong  = glon;
    BT_glat   = glat;
    BT_status = 4;

    AMSgObj::BookTimer.stop("ReAxDoBacktr");
    return 0;
  }

  BT_result |= (1<<bTLE);

  double rgt = momt/ichg;
  GeoMagTrace gp(RPT, VPT, YPR, xtime,
		 dir.gettheta(), dir.getphi(), rgt, ichg, beta);

  int stat = gp.Propagate(gp.NmaxStep);
  BT_status = 3;
  if (stat == GeoMagTrace::SPACE) BT_status = 1;
  if (stat == GeoMagTrace::ATMOS) BT_status = 2;

  BT_RPTO[0] = gp.GetRadi()*1e5;
  BT_RPTO[1] = gp.GetLong (false);
  BT_RPTO[2] = gp.GetLati (false);
  BT_time    = gp.GetTof();

  if (rgt != 0&& ichg != 0) {
    double phg = RPT[1]*180./M_PI;
    double thg = RPT[2]*180./M_PI;
    double phm = ptr->  PhiM*180./M_PI;
    double thm = ptr->ThetaM*180./M_PI;
    double trm = ptr->ThetaM;
    double arg = abs(rgt);
    double lrg = log10(arg);
    if (phg > 180) phg -= 360;
    if (phm > 180) phm -= 360;
#ifdef _PGTRACK_
    hman.Fill("GgIss",  phg, thg);
    hman.Fill("GmIss",  phm, thm);
    hman.Fill("GgCutD", phg, thg, Cutoff /ichg);
    hman.Fill("GmCutD", phm, thm, Cutoff /ichg);
    hman.Fill("GgCutS", phg, thg, CutoffS/ichg);
    hman.Fill("GmCutS", phm, thm, CutoffS/ichg);
    if (BT_status == 1) { hman.Fill("GgBTS",  phg, thg, lrg);
                          hman.Fill("GmBTS",  phm, thm, lrg);
			  hman.Fill("GmBTrS", trm, arg);
			  hman.Fill("GmBTmS", arg, BT_time); }
    if (BT_status == 2) { hman.Fill("GgBTA",  phg, thg, lrg);
                          hman.Fill("GmBTA",  phm, thm, lrg);
			  hman.Fill("GmBTrA", trm, arg);
			  hman.Fill("GmBTmA", arg, BT_time); }
    if (BT_status == 3) { hman.Fill("GgBTT",  phg, thg, lrg);
                          hman.Fill("GmBTT",  phm, thm, lrg);
			  hman.Fill("GmBTrT", trm, arg);
			  hman.Fill("GmBTmT", arg, BT_time); }
#endif
  }

  if (BACKTRACEFFKEY.out_type == 3) {
    BT_glong = gp.GetDlong(true);
    BT_glat  = gp.GetDlati(true);
  }
  else {
    double x = gp.GetDx(), y = gp.GetDy(), z = gp.GetDz(), r = 1;

    double glon = 0, glat = 0;
    FT_GTOD2Equat  (x, y, z, xtime-BT_time);
    FT_Cart2Angular(x, y, z, r, glat, glon);
    if (BACKTRACEFFKEY.out_type == 1) FT_Equat2Gal(glon, glat);
    BT_glong = glon*180./M_PI;
    BT_glat  = glat*180./M_PI;
  }

  AMSgObj::BookTimer.stop("ReAxDoBacktr");
  return 0;
#else
  GeoMagTrace::StepCv   = 0.1;                  //BACKTRACEFFKEY.stepdv;
  GeoMagTrace::MinStep  = 0.05*GeoMagTrace::Re; //BACKTRACEFFKEY.minstep
  GeoMagTrace::MaxStep  = 2.0 *GeoMagTrace::Re; //BACKTRACEFFKEY.maxstep
  GeoMagTrace::NmaxStep = 200;                  //BACKTRACEFFKEY.nmax;

  AMSDir dir(Theta, Phi);
  int    ichg = Charge;
  double beta = Beta;
  double momt = Momentum;

  if (beta < 0) { dir  = dir*(-1); beta = -beta; }
  if (momt < 0) { momt = -momt;    ichg = -ichg; }
  if (momt > 1e5) ichg = 0;

  if (ichg == 0) {
    AMSDir dir(Theta, Phi);
    if (dir.z() > 0) dir = dir*(-1);

    double glon = 0, glat = 0;
    int ret = AMSEventR::Head()->GetGalCoo(BT_result, glon, glat,
		  dir.gettheta(), dir.getphi(), 1, 4, 3, 0, out_type);
    BT_glong  = glon;
    BT_glat   = glat;
    BT_status = 4;

    return ret;
  }

  double glon = 0, glat = 0, RPTO[3] = { 0, 0, 0 }, time;
  int ret = AMSEventR::Head()
    ->DoBacktracing(BT_result, BT_status, glon, glat, RPTO,
		    time, dir.gettheta(), dir.getphi(), momt, beta, ichg);
  BT_glong   = glon;
  BT_glat    = glat;
  BT_RPTO[0] = RPTO[0];
  BT_RPTO[1] = RPTO[1];
  BT_RPTO[2] = RPTO[2];
  BT_time    = time;

  return ret;
#endif
}

int ParticleR::Loc2Gl(AMSEventR *pev){
         if(!pev)return 2;
	// Define transformation matrices
	////////////
	// AMS->ISS
	number alpha = 12 * TMath::DegToRad();
	number mAMS2ISS[3][3];
	mAMS2ISS[0][0] = 0;
	mAMS2ISS[0][1] = -1;
	mAMS2ISS[0][2] = 0;
	mAMS2ISS[1][0] = -cos(alpha);
	mAMS2ISS[1][1] = 0;
	mAMS2ISS[1][2] = -sin(alpha);
	mAMS2ISS[2][0] = sin(alpha);
	mAMS2ISS[2][1] = 0;
	mAMS2ISS[2][2] = -cos(alpha);
	AMSRotMat TrAMS2ISS(mAMS2ISS);

	////////////
	// ISS->LVLH
        float pitch,roll,yaw;
        int k=pev->fHeader.getISSAtt(roll,pitch, yaw); 
         if(k){
          static int mprint=0;
             if(mprint++<10)cerr<<"AMSParticleR-E-UnableToLoadRPY "<<k<<endl;
             yaw=pev->fHeader.Yaw; 
             roll=pev->fHeader.Roll; 
             pitch=pev->fHeader.Pitch; 
             if(yaw==0 && pitch==0 && roll==0)return 1;
         }         
	double ca = cos(yaw);
	double sa = sin(yaw);
	double cb = cos(pitch);
	double sb = sin(pitch);
	double cg = cos(roll);
	double sg = sin(roll);
	double mISS2LVLH[3][3];
	mISS2LVLH[0][0] = ca*cb;           
	mISS2LVLH[0][1] = ca*sb*sg-sa*cg;  
	mISS2LVLH[0][2] = ca*sb*cg+sa*sg;  
	mISS2LVLH[1][0] = sa*cb;           
	mISS2LVLH[1][1] = sa*sb*sg+ca*cg;  
	mISS2LVLH[1][2] = sa*sb*cg-ca*sg;  
	mISS2LVLH[2][0] = -sb;             
	mISS2LVLH[2][1] = cb*sg;           
	mISS2LVLH[2][2] = cb*cg;           
	AMSRotMat TrISS2LVLH(mISS2LVLH);

	////////////
	// LVLH->GEO
	 float StationRad,theta,phi,v,vtheta,vphi;
         double pi=3.1415926;
         static int err=0;         
         k=pev->fHeader.getISSCTRS(StationRad,theta, phi, v, vtheta, vphi);
         if(k){
             if(err++<10)cerr<<"AMSParticleR-E-UnableToLoadCTRS "<<k<<endl;
             //return 2;
             StationRad=pev->fHeader.RadS; 
             theta=pev->fHeader.ThetaS;
             phi=pev->fHeader.PhiS;
             v=pev->fHeader.VelocityS;
             vtheta=pev->fHeader.VelTheta;
             vphi=pev->fHeader.VelPhi;
         }         
	AMSDir amszg(pi/2+theta,phi+pi);  // z points toward the Earth's center 
	AMSDir amsxg(pi/2-vtheta,vphi); // x points along the orbital velocity
	AMSDir amsyg=amszg.cross(amsxg);
	double prod=amsxg.prod(amszg);
	if(fabs(prod)>0.03 ){
		cerr<<"AMSParticleR::Loc2Gl-E-AMSGlobalCoosystemIllDefined "<<prod<<" "<<amsxg<<" "<<amsyg<<" "<<amszg<<endl;
	}
    double mLVLH2GEO[3][3];
	for(int i=0 ; i < 3 ; i++) {
		mLVLH2GEO[i][0] = amsxg[i];
		mLVLH2GEO[i][1] = amsyg[i];
		mLVLH2GEO[i][2] = amszg[i];
	}
	AMSRotMat TrLVLH2GEO(mLVLH2GEO);


	////////////
	AMSDir _dir(Theta,Phi);
	AMSDir global = TrLVLH2GEO * (TrISS2LVLH * (TrAMS2ISS * _dir));

	ThetaGl=global.gettheta();
	PhiGl=global.getphi();
        return 0;
}

#ifdef _PGTRACK_
double ParticleR::GetRadiationLength(double z1, double z2, int type)
{
  AMSPoint pnt;
  AMSDir   dir;
  double   rgt = 0;

  if (type == 1 && pTrTrack())    { pnt = pTrTrack()->GetP0();
                                    dir = pTrTrack()->GetDir();
				    rgt = pTrTrack()->GetRigidity(); }
  if (type == 2 && pTrdTrack())   { pnt = AMSPoint(pTrdTrack()->Coo);
                                    dir = AMSDir  (pTrdTrack()->Theta,
				   	           pTrdTrack()->Phi); }
  if (type == 3 && pEcalShower()) { pnt = AMSPoint(pEcalShower()->Entry);
                                    dir = AMSDir  (pEcalShower()->Dir); }

  if (dir.norm() == 0) return -1;

  return AMSEventR::GetRadiationLength(pnt, dir, rgt, z1, z2);
}

int ParticleR::GetElementAbundance(double z1, double z2, int type,
				   double elem[9])
{
  AMSPoint pnt;
  AMSDir   dir;
  double   rgt = 0;

  if (type == 1 && pTrTrack())    { pnt = pTrTrack()->GetP0();
                                    dir = pTrTrack()->GetDir();
				    rgt = pTrTrack()->GetRigidity(); }
  if (type == 2 && pTrdTrack())   { pnt = AMSPoint(pTrdTrack()->Coo);
                                    dir = AMSDir  (pTrdTrack()->Theta,
				   	           pTrdTrack()->Phi); }
  if (type == 3 && pEcalShower()) { pnt = AMSPoint(pEcalShower()->Entry);
                                    dir = AMSDir  (pEcalShower()->Dir); }

  if (dir.norm() == 0) return -1;

  return AMSEventR::GetElementAbundance(pnt, dir, rgt, z1, z2, elem);
}

double ParticleR::GetRelInteractionLength(double z1, double z2, int zp,
					  int zt, int model, int norm)
{
  if (!pTrTrack()) return -1;

  AMSPoint pnt = pTrTrack()->GetP0();
  AMSDir   dir = pTrTrack()->GetDir();
  double   rgt = pTrTrack()->GetRigidity();
  if (dir.norm() == 0) return -1;

  return AMSEventR::GetRelInteractionLength(pnt, dir, rgt, z1, z2,
					    zp, zt, model, norm);
}
#endif

int  ParticleR::IsPassTOF(int ilay, const AMSPoint &pnt, const AMSDir &dir, AMSPoint &tofpnt, float &disedge){
   
   double tofz=TOFGeom::GetMeanZ(ilay);
   float coo[3],dis;
   bool isinbar=0;
   coo[0]=(tofz-pnt[2])*dir[0]/dir[2]+pnt[0];
   coo[1]=(tofz-pnt[2])*dir[1]/dir[2]+pnt[1];     
   int barid=TOFGeom::FindNearBar(ilay,coo[0],coo[1],dis,isinbar);
//----To TOF_Geometry Again
   tofz=TOFGeom::Sci_pz[ilay][barid];
   coo[0]=(tofz-pnt[2])*dir[0]/dir[2]+pnt[0];
   coo[1]=(tofz-pnt[2])*dir[1]/dir[2]+pnt[1];
   barid=TOFGeom::FindNearBar(ilay,coo[0],coo[1],dis,isinbar);
   tofpnt[0]=coo[0];tofpnt[1]=coo[1];tofpnt[2]=tofz;

//---Inside
   disedge=-1;
//---Lay Edge
  if(isinbar){
      disedge=9999999;
      float bdcoo[3][2];
      TOFGeom::GetLayEdge(ilay,bdcoo);
      for(int ipr=0;ipr<2;ipr++){     
        for(int ilr=0;ilr<2;ilr++){
          if(fabs(bdcoo[ipr][ilr]-coo[ipr])<disedge){disedge=fabs(bdcoo[ipr][ilr]-coo[ipr]);}
      }
     }
//----Bar Edge
     TOFGeom::GetBarEdge(ilay,barid,bdcoo);
     for(int ilr=0;ilr<2;ilr++){
       if(fabs(bdcoo[TOFGeom::Proj[ilay]][ilr]-coo[TOFGeom::Proj[ilay]])<disedge){disedge=fabs(bdcoo[TOFGeom::Proj[ilay]][ilr]-coo[TOFGeom::Proj[ilay]]);}
     } 

//---Additional  Trapdis
     float trapdis;
     TOFGeom::IsInSideBar(ilay,0,coo[0],coo[1],trapdis);
     if(trapdis<disedge){disedge=trapdis;}
     TOFGeom::IsInSideBar(ilay,TOFGeom::Nbar[ilay]-1,coo[0],coo[1],trapdis);
     if(trapdis<disedge){disedge=trapdis;}

//---Neighbor dis     
   if(!TOFGeom::IsTrapezoid(ilay,barid)){
      float bcoo[3][3][2];
      TOFGeom::GetBarEdge(ilay,barid,  bcoo[0]);
      TOFGeom::GetBarEdge(ilay,barid-1,bcoo[1]);
      TOFGeom::GetBarEdge(ilay,barid+1,bcoo[2]);
//--left
      for(int lr=0;lr<2;lr++){
//-----lrid
        int lrid=(lr==0)?barid-1:barid+1;
        if(TOFGeom::Sci_l[ilay][lrid]>=TOFGeom::Sci_l[ilay][barid])continue;
//---Only Len<Now Len
        float dt=fabs(bcoo[0][1-TOFGeom::Proj[ilay]][lr]-coo[1-TOFGeom::Proj[ilay]]);//T-Dis
        float dl=fabs(coo[TOFGeom::Proj[ilay]])-TOFGeom::Sci_l[ilay][lrid]/2;
        if(dl>0){
           if(dt<disedge)disedge=dt;
        }
        else { 
           float ds=sqrt(dt*dt+dl*dl);
           if(ds<disedge)disedge=ds;
         }
      }
    }
//----
  }//End inside
  int ubarid=0;
  if(isinbar)ubarid=barid;
  else       ubarid=-1;
  return ubarid;
//---
}


bool ParticleR::IsInsideTRD()
{
  // check if particle passing inside acceptance of the 20 layers TRD
  int nTrdCenter = 9;
  float AccepCenterX[] = {-80.0, -47.0, 47.0, 80.0,  80.0,  47.0, -47.0, -80.0, -80.0};
  float AccepCenterY[] = { 43.5,  75.5, 75.5, 43.5, -43.5, -75.5, -75.5, -43.5,  43.5};

  int nTrdTop = 37;
  float AccepTopX[] = {-99.0,-89.0,-89.0,-78.7,-78.7,-67.8,-67.8,-57.7,-57.7, 57.7, 57.7, 67.8, 67.8, 78.7, 78.7, 89.0, 89.0, 99.0, 
		 99.0, 89.0, 89.0, 78.7, 78.7, 67.8, 67.8, 57.7, 57.7,-57.7,-57.7,-67.8,-67.8,-78.7,-78.7,-89.0,-89.0,-99.0,-99.0};
  float AccepTopY[] = { 54.5, 54.5, 62.5, 62.5, 74.0, 74.0, 84.0, 84.0, 95.3, 95.3, 84.0, 84.0, 74.0, 74.0, 62.5, 62.5, 54.5, 54.5,
		-51.7,-51.7,-62.2,-62.2,-72.0,-72.0,-82.5,-82.5,-92.5,-92.5,-82.5,-82.5,-72.0,-72.0,-62.2,-62.2,-51.7,-51.7, 54.5};

  bool passTrdCenter = TMath::IsInside(TRDCoo[0][0], TRDCoo[0][1], nTrdCenter, AccepCenterX, AccepCenterY);
  bool passTrdTop    = TMath::IsInside(TRDCoo[1][0], TRDCoo[1][1], nTrdTop,    AccepTopX,    AccepTopY);
  
  return (passTrdCenter && passTrdTop);

}
//---------------------------------------------------------------------------------
double ParticleR::GetGeoCutoff(AMSEventR *pev){

        double deg2rad = TMath::DegToRad();
        double Re =  6371.2; //km Earth radius
#ifndef __ROOTSHAREDLIBRARY__
	EventNtuple02 *ptr = AMSJob::gethead()->getntuple()->Get_event02();
	if (!ptr) return 0;
        time_t Utime = ptr->Time[0];
        //...km
        double Altitude = ptr->RadS/1.e5-Re;
        //...ISS rad
	double ThetaISS=  ptr->ThetaS;
	double PhiISS=    ptr->PhiS;
#else
        time_t Utime =pev->UTCTime() ;
        //...km
        double Altitude = pev->fHeader.RadS/1.e5-Re;
        //...ISS rad
        double ThetaISS=  pev->fHeader.ThetaS;
        double PhiISS=    pev->fHeader.PhiS;
#endif
       //...ISS deg 
        double thetaISS =ThetaISS/deg2rad ;
        double phiISS = PhiISS/deg2rad;
        //----to be centered in 0!!
                //............................ZeroCentered
                if(phiISS > 180)  phiISS-=360.;



       //.....particle direction in GTOD:
        double dirTheta = ThetaGl;//----------COLATITUDINE!  lat = colat -90
        double dirPhi   = PhiGl ;//------------LONG!
      //from polar to cos dir.
        AMSDir dir(dirTheta ,dirPhi);
         dir[0] = - dir[0] ;
         dir[1] = - dir[1] ;
         dir[2] = - dir[2] ;

//      cout<<" ParticleR::GetGeoCutoff -dir "<< dir[0]<< "\t"<< dir[1]<< "\t" << dir[2]<< endl;        

        //....particle coordinates in GTOD deg  
        double theta = dir.gettheta()/deg2rad;
       //theta is colatitude --> I need Latitude:
        theta=90.- theta;
        double phi = dir.getphi()/deg2rad ;

//        cout<<" ParticleR::GetGeoCutoff theta= lat "<< theta << " phi=long "<< phi<< endl;

        int pos = -1;
        int sign = Momentum>0?1:-1;//sign
        if (sign ==-1) pos = 0 ;//...negative perticles         
        if (sign == 1) pos = 1 ;//...positive perticles         

        //...GeV/c
        double R =Charge * GeoMagCutoff( Utime,  Altitude , theta,  phi, thetaISS,  phiISS,  pos ) ;
        //Charge is part.Charge        

return R;
}
//----------------------------------------------------------------------------------------
int ParticleR::GetStoermerCutoff(double &Scut,int momOrR, int sign, AMSDir amsdir ){

                // set to zero
                Scut=0;

       if(momOrR<0 || momOrR>1){
        //cerr<< "ParticleR::GetStoermerCutoff momOrR must be 0 or 1 "<< endl;  
        return -4 ;
        }

        if(sign<-1 || sign>1 ){
        //cerr<< "ParticleR::GetStoermerCutoff wrong sign parameter"<<endl;
        return -3;
        }

        //-------------------check AMSEventR::Head()
        if(!AMSEventR::Head()){
        //cerr<< "ParticleR::GetStoermerCutoff No Header"<< endl;
        return -2;
        }

        double deg2rad = TMath::DegToRad();
        double Re =  6371.2; //km Earth radius
        time_t Utime = (int)AMSEventR::Head()->UTCTime() ;
        //...km
        double Altitude = AMSEventR::Head()->fHeader.RadS/1.e5-Re;
        //...ISS rad
        double ThetaISS=  AMSEventR::Head()->fHeader.ThetaS;
        double PhiISS=    AMSEventR::Head()->fHeader.PhiS;
       //...ISS deg 
        double thetaISS =ThetaISS/deg2rad ;
        double phiISS = PhiISS/deg2rad;
        //----to be centered in 0!!
               //............................ZeroCentered
                if(phiISS > 180)  phiISS-=360.;

        // From AMS to GTOD
        //... out:
        int result ;
        //...in AMS:--->      
        double  thetaAMS = amsdir.gettheta();//colatitude in AMS reference frame (theta-->pi==out-going; theta-->0==up-going) 
        double  phiAMS =   amsdir.getphi();  //longiture  in AMS reference frame 

        //...out GTOD:---> 
        double  theta_deg ; // latitude [deg]
        double  phi_deg ;   // longitude[deg] 
        // Parameters:  
        //use_att = 1 (LVLH)
        //use_coo = 4,(Use AMS-GPS)
        //Use_time= 3,(AMSGPS Time Corrected)
        //dt      = 0,  time jitter (sec)
        //out_type= 3 (GTOD)

        int gtodT =  AMSEventR::Head()->GetGalCoo(result, phi_deg , theta_deg, thetaAMS, phiAMS, 1,4,3,0.,3 );
        if(gtodT<0){
        return gtodT;
        }

//        cout<<" ParticleR::GetStoermerCutoff theta= lat "<< theta_deg << " phi=long "<< phi_deg<< endl;

        //-------particle sign
        int pos = -1;
        if (sign ==-1) pos = 0 ;//...force sign negative perticles         
        if (sign == 1) pos = 1 ;//...force sign positive perticles         

        if (sign == 0 ){

        int  signP = Momentum>0?1:-1;// ParticleR sign
        if (signP ==-1) pos = 0 ;//...negative perticles         
        if (signP == 1) pos = 1 ;//...positive perticles 
        }

        //---------------------[GV]

        double Rcut =GeoMagCutoff( Utime,  Altitude , theta_deg,  phi_deg, thetaISS,  phiISS,  pos ) ;
//        cout<<" ParticleR::GetStoermerCutoff Old " <<RcutOld << " New "<< Rcut <<" ratio New/Old "<< Rcut/RcutOld<<endl;   
        if(momOrR==0){
        //---------------------[GeV/c]
        Rcut *=Charge ;
        }
        //Charge is part.Charge!        

        //-----------------Stoermer
        Scut=Rcut;

return 0;
}

//----------------------------------------------------------------------------------------


double ParticleR::RichBetasAverage(){
  if(pRichRing() && pRichRingB()){
    double beta=pRichRing()->getBeta();
    double betaB=pRichRingB()->Beta;
    double factor=1;

    // Check if Dynamic Calibration is taking place
    // If so compute the calibration factor for RichRingB  
    if(RichRingR::isCalibrating()) factor=pRichRing()->betaCorrection();
    return 0.5*(beta+factor*betaB);
  }else return 0;
}

double ParticleR::RichBetasDiscrepancy(){
  if(pRichRing() && pRichRingB()){
    double betaB=pRichRingB()->Beta;
    double factor=1;
    
    // Check if Dynamic Calibration is taking place
    // If so compute the calibration factor for RichRingB  
    if(RichRingR::isCalibrating()) factor=pRichRing()->betaCorrection();
    return pRichRing()->getBeta()-factor*betaB;
  }else return HUGE_VAL;
}

TofClusterR::TofClusterR(AMSTOFCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  Edep   = ptr->_edep;
  Edepd  = ptr->_edepd;
  Time   = ptr->_time;
  ErrTime= ptr->_etime;
  for (int i=0; i<3; i++) {
    Coo[i] = ptr->_Coo[i];
    ErrorCoo[i] = ptr->_ErrorCoo[i];
  }
#endif
}

TofClusterHR::TofClusterHR(unsigned int sstatus[2],unsigned int status,int pattern,int idsoft,double adca[2],double adcd[2][3],
               double sdtm[2],double times[2],double timer,double etimer,AMSPoint coo,AMSPoint ecoo,double q2pa[2],double q2pd[2][3],double edepa,double edepd,TofRawSideR *tfraws[2]):
         Status(status),Pattern(pattern),Time(timer),ETime(etimer),AEdep(edepa),DEdep(edepd)
{
   Layer= idsoft/1000%10;
   Bar  = idsoft/100%10;
   int index[2]={-1};
   for(int is=0;is<2;is++){
    Aadc[is]= adca[is];
    Rtime[is]=sdtm[is];
    Stime[is]=times[is];
    SideBitPat[is]=sstatus[is];
    AQ2[is]=q2pa[is];
    for(int ipm=0;ipm<3;ipm++){Dadc[is][ipm]=adcd[is][ipm];DQ2[is][ipm]=q2pd[is][ipm];}
    if(tfraws[is]){
      for(unsigned int ii=0;ii<AMSEventR::Head()->NTofRawSide();ii++){if(AMSEventR::Head()->pTofRawSide(ii)==tfraws[is]){index[is]=ii;}}
    }
    fTofRawSide.push_back(index[is]);
  }
  for(int i=0;i<3;i++){
    Coo[i]= coo[i];
    ECoo[i]=ecoo[i];
  }
  QPar.clear();
}

TofClusterHR::TofClusterHR(AMSTOFClusterH *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->Status;
  Pattern= ptr->Pattern;
  Layer  = ptr->Layer;
  Bar    = ptr->Bar;
  for(int is=0;is<2;is++){
    Aadc[is]= ptr->Aadc[is];
    Rtime[is]=ptr->Rtime[is];
    Stime[is]=ptr->Stime[is];
    SideBitPat[is]=ptr->SideBitPat[is];
    AQ2[is]=ptr->AQ2[is];
    for(int ipm=0;ipm<3;ipm++){Dadc[is][ipm]=ptr->Dadc[is][ipm];DQ2[is][ipm]=ptr->DQ2[is][ipm];}
  }
  Time= ptr->Time;
  ETime=ptr-> ETime;
  for(int i=0;i<3;i++){
    Coo[i]= ptr->Coo[i];
    ECoo[i]=ptr->ECoo[i];
  }
  AEdep=ptr->AEdep;
  DEdep=ptr->DEdep;
  QPar=ptr->QPar;
#endif
}


TofMCClusterR::TofMCClusterR(AMSTOFMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft =  ptr->idsoft;
  Particle= ptr->particle;
  ParentNo= ptr->parentid;
  GtrkID = ptr->gtrkid;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Beta= ptr->beta;
  Edep= ptr->edep;
  EdepR=ptr->edepr;
  Step= ptr->step;
#endif
}

TofMCPmtHitR::TofMCPmtHitR(AMSTOFMCPmtHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->getpmtid();
  ParId  = ptr->getparentid();
  TimeG=   ptr->getphtime();
  TimeT=   ptr->getphtimel();
  Ekin=    ptr->getphekin();
  Length=  ptr->getphtral();
  //cout<<"photon pos"<<ptr->getphpos()[0]<<endl;
  for(int i=0;i<3;i++){Pos[i]=ptr->getphpos()[i];}
  for(int i=0;i<3;i++){Dir[i]=ptr->getphdir()[i];}
  TimeP=   ptr->gettimp();
  Amp=     ptr->getamp();
#endif
}

TofRawClusterR::TofRawClusterR(TOF2RawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_ntof;
  Bar    = ptr->_plane;
  for (int i=0; i<2; i++) adca[i]=ptr->_adca[i];
  for (int i=0; i<2; i++) adcd[i]=ptr->_adcd[i];
  for (int i=0; i<2; i++){
    for(int ip=0;ip<TOF2GC::PMTSMX;ip++){
      adcdr[i][ip]=ptr->_adcdr[i][ip];
    }
  }
  for (int i=0; i<2; i++) sdtm[i] =ptr->_sdtm[i];
  edepa  = ptr->_edepa;
  edepd  = ptr->_edepd;
  time   = ptr->_time;
  cool   = ptr->_timeD;
#endif
}

TofRawSideR::TofRawSideR(TOF2RawSide *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  fftdc.clear();
  fstdc.clear();
  fsumht.clear();
  fsumsht.clear();
  swid=ptr->_swid;
  hwidt=ptr->_hwidt;
  for(int i=0; i<4; i++)hwidq[i]=ptr->_hwidq[i];
  stat=ptr->_status;
  nftdc=ptr->_nftdc;
//  for(int i=0; i<nftdc && i<sizeof(ftdc)/sizeof(ftdc[0]); i++)ftdc[i]=ptr->_ftdc[i];
  for(int i=0; i<nftdc; i++)fftdc.push_back(ptr->_ftdc[i]);
  nstdc=ptr->_nstdc;
  for(int i=0; i<nstdc; i++)fstdc.push_back(ptr->_stdc[i]);
  nsumh=ptr->_nsumh;
  for(int i=0; i<nsumh; i++)fsumht.push_back(ptr->_sumht[i]);
  nsumsh=ptr->_nsumsh;
  for(int i=0; i<nsumsh; i++)fsumsht.push_back(ptr->_sumsht[i]);
  adca=ptr->_adca;
  nadcd=ptr->_nadcd;
  for(int ip=0;ip<TOF2GC::PMTSMX && ip<int(sizeof(adcd)/sizeof(adcd[0]));ip++)adcd[ip]=ptr->_adcd[ip];
  temp=ptr->_tempT;
  tempC=ptr->_tempC;
  tempP=ptr->_tempP;
#endif
}



TrdClusterR::TrdClusterR(AMSTRDCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Layer  = ptr->_layer;
  for (int i=0; i<3; i++) Coo[i]= ptr->_Coo[i];
  if(ptr->_CooDir[0]>0.9)Direction=0;
  else Direction=1;
  ClSizeR=ptr->_ClSizeR;
  ClSizeZ=ptr->_ClSizeZ;
  Multip = ptr->_Multiplicity;
  HMultip= ptr->_HighMultiplicity;
  EDep   = ptr->_Edep;
  //a=TrdRawHitR(ptr->_pmaxhit);
  fTrdRawHit =-1;
#endif
}

TrdMCClusterR::TrdMCClusterR(AMSTRDMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_idsoft.getlayer();
  Ladder = ptr->_idsoft.getladder();
  Tube   = ptr->_idsoft.gettube();
  ParticleNo= ptr->_ipart;
  GtrkID = ptr->_gtrkid;
  ProcID = ptr->_procid;
  if(ptr->_itra!=0)ParticleNo= -ParticleNo;
  //  TrackNo= ptr->_itra;
  Edep   = ptr->_edep;
  Ekin   = ptr->_ekin;
  for (int i=0; i<3; i++) {Xgl[i] = ptr->_xgl[i];}
  Step   = ptr->_step;
#endif
}

TrdRawHitR::TrdRawHitR(AMSTRDRawHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Layer  = ptr->_id.getlayer();
  Ladder = ptr->_id.getladder();
  Tube   = ptr->_id.gettube();
  Amp    = ptr->Amp();
  int hadr=   ptr->_id.gethaddr();
  Haddr=(hadr&63)+100*((hadr>>6)&7)+1000*((hadr>>9)&7)+10000*ptr->_id.getcrate();
#endif
}

unsigned int TrdRawHitR::getid(){
 const int maxtube=16;
 const int maxlad=18;
return  maxtube*maxlad*Layer+maxtube*Ladder+Tube;
}

float TrdRawHitR::getped(int & error){
 const string name("TRDPedestals");
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 return value.f;
}

float TrdRawHitR::getsig(int & error){
 const string name("TRDSigmas");
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 return value.f;
}
float TrdRawHitR::getgain(int & error){
 const string name("TRDGains");
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 return value.f;
}

float TrdRawHitR::getgain2(int & error){
// ugly function to get trdgains2
//  init==3  unable to get correct tdv from db
//  init==1  gains2 already in root file
//  init==2  gains2 correctly read from db and put into root file

  const string name("TRDGains2");
 static AMSTimeID *tdvdb=0; 
 static int init=0;
 static int *gain2=0;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate(init,tdvdb,gain2)
#endif
 if( !init){
   AMSEventR::if_t value;
  value.u=0;
  if(!AMSEventR::Head()){
   cerr<<"TrdRawHitR::getgain2-NoEventRHeadFound "<<endl;
   init =3;
  }
  else{
   error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
   if(error==1){
    cerr<<"TrdRawHitR::getgain2-NoTDVFound "<<endl;
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;
    const int maxtube=16;
    const int maxlad=18;
    const int maxlay=20;
    int size=maxtube*maxlad*maxlay+maxtube*maxlad+maxtube;
    int ssize=size*sizeof(int);
    gain2=new int[size];
{
    AMSCommonsI a;
   tdvdb=new AMSTimeID(AMSID(name.c_str(),1),
                        begin,end,ssize,
                        (void*)gain2,AMSTimeID::Standalone,true);
//    cout <<"  init passed "<<omp_get_thread_num()<<endl;
    time_t tmt=AMSEventR::Head()->UTime();
//     cout <<"  before validate passed "<<endl;
    int ret=tdvdb->validate(tmt);
//    cout <<"  validate passed "<<endl;
      if(ret){
//      if(AMSEventR::Head()->getsetup())AMSEventR::Head()->getsetup()->TDVRC_Add(tmt,tdvdb);
      init =2;
    }
    else {
         cerr<<"TrdRawHitR::getgain2-UnableToValidateTDV "<<endl;
         init=3;
    }
}
   }

   else init=1;
 }
 }
 else if(init==2){
    if(tdvdb){
      time_t tmt=AMSEventR::Head()->UTime();

       int ret=tdvdb->validate(tmt);
       if(ret){
//         if(AMSEventR::Head()->getsetup())AMSEventR::Head()->getsetup()->TDVRC_Add(tmt,tdvdb);
       }
       else init=3;
    
 }
 else init=3;
}
      
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 if(init==3)error=init;
 return value.f;
}


TrdSegmentR::TrdSegmentR(AMSTRDSegment *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status        = ptr->_status;
  Orientation   = ptr->_Orientation;
  for (int i=0; i<2; i++) {FitPar[i] = ptr->_FitPar[i];}
  Chi2          = ptr->_Chi2;
  Pattern       = ptr->_Pattern;
#endif
}

#include <fstream>
bool TrdTrackR::CreatePDF(const char *fnam){


ifstream file;
if(fnam && strlen(fnam)>0)file.open(fnam);
else if(getenv("TRDChargePDFFile")){
 file.open(getenv("TRDChargePDFFile"));
}
else if (AMSEventR::Head() && AMSEventR::Head()->getsetup()){
 AMSEventR::Head()->getsetup()->fTDV_Name.clear();
 AMSEventR::Head()->getsetup()->getAllTDV("TRDPDF");
 if(AMSEventR::Head()->getsetup()->fTDV_Name.size()){
  if(AMSEventR::Head()->getsetup()->fTDV_Name[0].Size == AMSEventR::Head()->getsetup()->fTDV_Name[0].Data.size()){ 
 cout <<    "Charge::ChargePDF-I-FoundTDVSize "<<AMSEventR::Head()->getsetup()->fTDV_Name[0].Data.size()<<endl;
 for(unsigned int k=0;k<AMSEventR::Head()->getsetup()->fTDV_Name[0].Size;k++){
/*
     union if_t{
     float f;
     unsigned int u;
     int i; 
     };
     if_t tmp;
     tmp.u=AMSEventR::Head()->getsetup()->fTDV_Name[0].Data[k];
     ChargePDF[k]=tmp.f; 
*/
    }
     if( AMSEventR::Head()->getsetup()->fTDV_Name[0].CopyOut(ChargePDF)){
     const int span=1003;
  int thr=0;
static int xdone=0;
#ifdef _OPENMP
xdone=omp_get_num_threads()==1?0:1;
#endif
for(int k=0;k<10;k++){
 if(!xdone) AMSEventR::hbook1(-50000-k-thr*100,"pdf func ",1000,0.,100.);
  for(int j=0;j<span-3;j++){
     AMSEventR::hf1(-50000-k-thr*100,j/10.+0.05,ChargePDF[span*k+j]);
}
}
xdone=1;

return true;
  }
}
  else cerr<<"Charge::ChargePDF-E- "<<AMSEventR::Head()->getsetup()->fTDV_Name[0].Name<<" RealSize "<<AMSEventR::Head()->getsetup()->fTDV_Name[0].Data.size()<< " Size "<<AMSEventR::Head()->getsetup()->fTDV_Name[0].Size<<endl;
  }
  else{
    cerr<<"Charge::ChargePDF-E-noTDV "<<"TRDPDF"<<endl;
  }
}
else{
 cerr<<"Charge::ChargePDF-E-FileNotDefined"<<endl;
  
return false;
}
if(!file){
 cerr<<"Charge::ChargePDF-E-UnableToOpen "<<fnam <<" "<<getenv("TRDChargePDFFile")<<endl;
return false;
}
for(unsigned int k=0;k<sizeof(ChargePDF)/sizeof(ChargePDF[0]);k++)ChargePDF[k]=0;
unsigned int ptr=0;
const unsigned int span=1003;
while(file.good() && !file.eof()){

file>>ChargePDF[ptr*span+span-3];  //e2c
file>>ChargePDF[ptr*span+span-2];  //id
file>>ChargePDF[ptr*span+span-1];  //ok
for(unsigned int k=0;k<span-3;k++)file>>ChargePDF[ptr*span+k];
double smax=0;
for(unsigned int k=0;k<span-3;k++){
  float a=ChargePDF[ptr*span+k];
  if(a<=0){
    float sum=0;
    for(unsigned int j=k;j<span-3;j++){
      if(ChargePDF[ptr*span+j]<10)sum+=ChargePDF[ptr*span+j];
    }
    for(unsigned int j=k;j<span-3;j++){
        if(ChargePDF[ptr*span+j]<10)ChargePDF[ptr*span+j]=(sum==0?1:sum)/(span-3-k);
      }
    }
  }
 
for(unsigned int k=0;k<span-3;k++){
smax+=ChargePDF[ptr*span+k];
}
for(unsigned int k=0;k<span-3;k++)ChargePDF[ptr*span+k]/=smax;
if(ChargePDF[ptr*span+span-1])ptr++;


}

cout <<"Charge::ChargePDF-I- "<<ptr<<" pdf loaded for ";
for(unsigned int k=0;k<ptr;k++)cout<<ChargePDF[k*span+span-2]<<" ";
cout <<endl;
if(ptr<3){
 cerr<<"Charge::ChargePDF-E-minimal 3 PDF Needed "<<endl;
 return false;
}
for(unsigned int i=ptr;i<sizeof(ChargePDF)/sizeof(ChargePDF[0])/span;i++){
  ChargePDF[i*span+span-2]=i;
}
  int thr=0;
#ifdef _OPENMP
//  thr=omp_get_thread_num();
#endif
static int done=0;
for(int k=0;k<10;k++){
 if(!done) AMSEventR::hbook1(-50000-k-thr*100,"pdf func ",1000,0.,100.);
  for(unsigned int j=0;j<span-3;j++){
     AMSEventR::hf1(-50000-k-thr*100,j/10.+0.05,ChargePDF[span*k+j]);
}
}
done=1;
return true;
}



void TrdTrackR::ComputeCharge(double betacorr){
            for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=-1;
            for(unsigned int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=10000;
            vector<float>edepc;
     for(int k=0;k<NTrdSegment();k++){
        TrdSegmentR tseg=*(pTrdSegment(k));
        for (int l=0;l<tseg.NTrdCluster();l++){
          TrdClusterR trdcl=*(tseg.pTrdCluster(l));
          if(trdcl.Multip<3){
            float range=trdcl.Range(Coo,Theta,Phi);
            double norm=TrdClusterR::RangeCorr(0.57,1.);        
            double e=trdcl.EDep/TrdClusterR::RangeCorr(range,norm);
                edepc.push_back(e);         
          }
         }
        }
        if(edepc.size()>6){

   sort(edepc.begin(),edepc.end());
   double medianc=0;            
   if(edepc.size()%2)for(unsigned int k=edepc.size()/2-2;k<edepc.size()/2+3;k++)medianc+=edepc[k]/5;
   else for(unsigned int k=edepc.size()/2-2;k<edepc.size()/2+2;k++)medianc+=edepc[k]/4;
   Q=sqrt(medianc/betacorr)*1/1.0925+0.115/1.0925;

    
   //lkhd
   const int span =sizeof(ChargePDF)/sizeof(ChargePDF[0])/(sizeof(Charge)/sizeof(Charge[0]));
   for (unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
    if(ChargePDF[k*span+span-1]){
    Charge[k]=ChargePDF[k*span+span-2];  
    ChargeP[k]=0;
    for(unsigned int i=0;i<edepc.size();i++){
      int ch=edepc[i]/betacorr/ChargePDF[k*span+span-3];
      if(ch<0)ch=0;
      if(ch>span-3)ch=span-3;
      ChargeP[k]+=-log(ChargePDF[k*span+ch]);
    }
    }
    else{
      for(int l=k;l>0;l--){
        if(ChargePDF[l*span+span-1]){
         Charge[k]=ChargePDF[k*span+span-2];  
         ChargeP[k]=0;
         float factor=Charge[l]/Charge[k];
          for(unsigned int i=0;i<edepc.size();i++){
            int ch=edepc[i]*factor*factor/betacorr/ChargePDF[l*span+span-3];
            if(ch<0)ch=0;
            if(ch>span-3)ch=span-3;
              ChargeP[k]+=-log(ChargePDF[l*span+ch]);
          }
         break;
        }
      }
    }
    
   }

   double prsum=0;
   for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     prsum+=exp(-double(ChargeP[k]));
   }
    
   for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     ChargeP[k]+=log(prsum);
   }
   multimap<float,short int> chmap;
   for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     chmap.insert(make_pair(ChargeP[k],Charge[k]));
   }
   int l=0;
   for(multimap<float,short int>::iterator k=chmap.begin();k!=chmap.end();k++){
        Charge[l]=k->second;
        ChargeP[l++]=k->first;
   }


 }


        }



//----TrdTrack Interpolation
double TrdTrackR::Interpolate(double zpl,AMSPoint &pnt,AMSDir &dir) const{
//----
   number theta=Theta,phi=Phi;
   AMSDir d1(theta,phi);
   AMSPoint p0,p1;
   p1[2]=zpl;
   p0[2]=0;
   for(int ixy=0;ixy<2;ixy++){
     p1[ixy]=Coo[ixy]+d1[ixy]/d1[2]*(p1[2]-Coo[2]);//zpl
     p0[ixy]=Coo[ixy]+d1[ixy]/d1[2]*(p0[2]-Coo[2]);
   }
   AMSPoint p2(p1-p0);
   number path=p2.norm();
   if(p1[2]>0)path*=-1;
//---
   dir=d1; pnt=p1;
   return path;
}
//---

TrdTrackR::TrdTrackR(const TrdTrackR &o){
if(&o!=this){
 for(int k=0;k<3;k++)Coo[k]=o.Coo[k];
 for(int k=0;k<3;k++)ErCoo[k]=o.ErCoo[k];
  Chi2=o.Chi2;
  Pattern=o.Pattern;
 Status=o.Status;
 Phi=o.Phi; 
 Theta=o.Theta; 
 Q=o.Q;
 for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=o.Charge[k];
 for(unsigned int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=o.ChargeP[k];
 for(unsigned int i=0;i<o.fTrdSegment.size();i++)fTrdSegment.push_back(o.fTrdSegment[i]);  
}
}

TrdTrackR::TrdTrackR(AMSTRDTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  for (int i=0; i<3; i++) {
    Coo[i]   = ptr->_StrLine._Coo[i];
    ErCoo[i] = ptr->_StrLine._ErCoo[i];
  }
  Phi   = ptr->_StrLine._Phi;
  Theta = ptr->_StrLine._Theta;
  Chi2  = ptr->_StrLine._Chi2;
  Pattern = ptr->_BaseS._Pattern;
  Q=ptr->_Charge.Q;
  for(unsigned int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=ptr->_Charge.Charge[k];
  for(unsigned int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=ptr->_Charge.ChargeP[k];
#endif
}

/*TrdHSegmentR::TrdHSegmentR(AMSTRDHSegment *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  d   = ptr->d;
  Nhits = ptr->Nhits;
  Chi2 = ptr->Chi2;
  m   = ptr->m;
  em  = ptr->em;
  r   = ptr->r;
  er  = ptr->er;
  w   = ptr->w;
  z   = ptr->z;
#endif
}

TrdHTrackR::TrdHTrackR(AMSTRDHTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  for (int i=0; i<3; i++) {
    Coo[i]   = ptr->Coo[i];
    Dir[i]   = ptr->Dir[i];
  }
  //  Phi   = ptr->Phi();
  //  Theta = ptr->Theta();
  Chi2  = ptr->Chi2;
  Nhits = ptr->Nhits;
  //  ETheta= ptr->ETheta();
  //  EPhi  = ptr->EPhi();
  status= ptr->status;
#endif
}*/

#ifndef _PGTRACK_
TrClusterR::TrClusterR(AMSTrCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr-> _Id.cmpt();
  Status = ptr->_status;
  NelemL = ptr->_NelemL;
  NelemR = ptr->_NelemR;
  StripM= ptr->_Id.getstrip();
  Sum    = ptr->_Sum;
  Sigma  = ptr->_Sigma;
  Mean   = ptr->_Mean;
  RMS    = ptr->_Rms;
  ErrorMean = ptr->_ErrorMean;
  for (int i=0; i<ptr->getnelem(); i++)Amplitude.push_back(ptr->_pValues[i]);
#endif
}

TrMCClusterR::TrMCClusterR(AMSTrMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft  = ptr->_idsoft;
  TrackNo = ptr->_itra;
  for (int i=0; i<2; i++) {
    Left[i]   = ptr->_left[i];
    Center[i] = ptr->_center[i];
    Right[i]  = ptr->_right[i];
  }
  for (int i=0; i<3; i++) {
    Xca[i] = ptr->_xca[i];
    Xcb[i] = ptr->_xcb[i];
    Xgl[i] = ptr->_xgl[i];
  }
  Sum = ptr->_sum;
  for (int i=0; i<2; i++) {
    for (int j=0; j<5; j++) {
      SS[i][j] = ptr->_ss[i][j];
    }
  }
#ifdef __PGTRACK__
  GtrkID = ptr->_gtrkid;
#endif
#endif
}

TrRawClusterR::TrRawClusterR(AMSTrRawCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  address = ptr->_address+ptr->_strip*10000;
  s2n     = ptr->_s2n;
  amp.clear(); 
  for(int k=0;k<ptr->_nelem;k++)amp.push_back(ptr->getamp(k));
#endif
}

TrRecHitR::TrRecHitR(AMSTrRecHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  fTrClusterX = -1;
  fTrClusterY = -1;
  Status= ptr->_status;
  Id = ptr->_Id.cmptr();
  for (int i=0; i<3; i++) Hit[i]  = ptr->_Hit[i];
  for (int i=0; i<3; i++) EHit[i] = ptr->_EHit[i];
  Sum     = ptr->_Sum;
  DifoSum = ptr->_DifoSum;
  CofgX   = ptr->_cofgx;
  CofgY   = ptr->_cofgy;
  for (int i=0; i<3; i++) Bfield[i] = ptr->_Bfield[i];
#endif
}

TrTrackR::TrTrackR(AMSTrTrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status    = ptr->_status;
  Pattern   = ptr->_Pattern;
  Addressl   = ptr->_Address;
  DBase[0]       = FLT_MAX;
  if(!isnan(ptr->_Dbase[0]))
  DBase[0]       = fabs(ptr->_Dbase[0])>FLT_MAX?FLT_MAX:ptr->_Dbase[0];
  DBase[1]       = FLT_MAX;
  if(!isnan(ptr->_Dbase[1]))
      DBase[1]=fabs(ptr->_Dbase[1])>FLT_MAX?FLT_MAX:ptr->_Dbase[1];
  GeaneFitDone    = ptr->_GeaneFitDone;
  AdvancedFitDone = ptr->_AdvancedFitDone;
  Chi2StrLine     = ptr->_Chi2StrLine;
  Chi2WithoutMS   = ptr->_Chi2WithoutMS;
  RigidityWithoutMS  = ptr->_RigidityWithoutMS;
  Chi2FastFit     = ptr->_Chi2FastFit;
  Rigidity        = ptr->_Ridgidity;
  ErrRigidity     = ptr->_ErrRidgidity;
  Theta           = ptr->_Theta;
  Phi             = ptr->_Phi;
  for (int i=0; i<3; i++) P0[i] = ptr->_P0[i];
  GChi2           = (float)ptr->_GChi2;
  GRigidity       = (float)ptr->_GRidgidity;;
  GErrRigidity    = (float)ptr->_GErrRidgidity;

#ifdef _PGTRACK_
  for(int i=0;i<TkDBc::Head->nlay();i++)
#else
  for(int i=0;i<TKDBc::nlay();i++)
#endif
    {
      for(int k=0;k<3;k++){
	Hit[i][k]=(float)ptr->_Hit[i][k];
	EHit[i][k]=(float)ptr->_EHit[i][k];
      }
    }
  if(AdvancedFitDone){
    for (int i=0; i<2; i++) {
      HChi2[i]        = (float)ptr->_HChi2[i];
      HRigidity[i]    = (float)ptr->_HRidgidity[i];
      HErrRigidity[i] = (float)ptr->_HErrRidgidity[i];
      HTheta[i]       = (float)ptr->_HTheta[i];
      HPhi[i]         = (float)ptr->_HPhi[i];
      for (int j=0; j<3; j++)  HP0[i][j] = (float)ptr->_HP0[i][j];
      for(int j=0;j<3;j++)RigidityIE[j][i]=(float)ptr->_RigidityIE[j][i]; 
  }
} 
  else{
    for (int i=0; i<2; i++) {
      HChi2[i]        = 0;
      HRigidity[i]    = 0;
      HErrRigidity[i] = 0;
      HTheta[i]       = 0;
      HPhi[i]         = 0;
      for (int j=0; j<3; j++)  HP0[i][j] = 0;
   for (int j=0; j<3; j++)  RigidityIE[j][i] = 0;

    }
  }
  FChi2MS         = ptr->_Chi2MS;
  PiErrRig        = ptr->_PIErrRigidity;
  RigidityMS      = ptr->_RidgidityMS;
  PiRigidity      = ptr->_PIRigidity;
       BitPattern=0;
       for(int k=0;k<ptr->_NHits;k++){
       int l=(ptr->_Pthit[k]->getid().cmptr())%10-1;
        BitPattern|=(1<<l);
       }
  fTrTrackFit.clear();
  for(int k=0;k<ptr->fTrTrackFit.size();k++){
    fTrTrackFit.push_back(ptr->fTrTrackFit[k]);
  }
//  char* info;
//  info=Info();
#endif
}
#endif

RichMCClusterR::RichMCClusterR(AMSRichMCHit *ptr, int _numgen){
#ifndef __ROOTSHAREDLIBRARY__
  Id        = ptr->_id;
  for (int i=0; i<3; i++) {
    Origin[i]    = ptr->_origin[i];
    Direction[i] = ptr->_direction[i];
  }
  Status       = ptr->_status;
  fRichHit = ptr->_hit;
  NumGen       = _numgen;
  GtrkID = ptr->_gtrkid;
  GparentID = ptr->_gparentid;
#endif
}

RichHitR::RichHitR(AMSRichRawEvent *ptr, float x, float y, float z){
#ifndef __ROOTSHAREDLIBRARY__
  if (ptr) {
    Channel = ptr->_channel;
    Counts  = ptr->_counts;
    Status  = ptr->_status;
    Npe     = ptr->getnpe();
    HwAddress = ptr -> getHwAddress();
    Coo[0]=x;
    Coo[1]=y;
    Coo[2]=z;
    int pmt,pixel;
    RichPMTsManager::UnpackGeom(Channel,pmt,pixel);
    SoftId=RichPMTsManager::GetPMTID(pmt)*16+RichPMTsManager::GetChannelID(pmt,pixel);
  } else {
    cout<<"RICEventR -E- AMSRichRawEvent ptr is NULL"<<endl;
  }
#endif
}

int RichHitR::PhotoElectrons(double sigmaOverQ){
  int org=int(floor(Npe));
  double maximum=-HUGE_VAL;
  int best=0;
  for(int n=max(org-3,1);n<=org+3;n++){
    double rms=n*sigmaOverQ*sigmaOverQ;
    double lkh=-0.5*(Npe-n)*(Npe-n)/rms-0.5*log(rms);
    if(lkh>maximum){
      maximum=lkh;
      best=n;
    }
  }
  return best;
}


float RichHitR::getCollectedPhotoElectrons(){
  AMSEventR *event=AMSEventR::Head();
  if(!event) return 0;
  float counter=0;
  for(int i=0;i<event->nRichHit();i++){
    RichHitR *hit=event->pRichHit(i);
    if(!hit) continue;
    if(hit->IsCrossed()) continue;
    counter+=hit->Npe;
  }
  return counter;
}


int RichHitR::getPMTs(bool countCrossed){
  AMSEventR *event=AMSEventR::Head();
  if(!event) return 0;
  bool counted[680];
  for(int i=0;i<680;counted[i++]=false); 
  int counter=0;
  for(int i=0;i<event->nRichHit();i++){
    RichHitR *hit=event->pRichHit(i);
    if(!hit) continue;
    if(!countCrossed && hit->IsCrossed()) continue;
    int pmt=hit->Channel/16;
    if(counted[pmt]) continue;
    counted[pmt]=true;
    counter++;
  }
  return counter;
}

float RichRingR::getBetaExpectedResolution(){
  if(RichBetaUniformityCorrection::getHead())     // Use the uniformity corrections, which should be better
    return RichBetaUniformityCorrection::getHead()->getWidth(this)*2;

  return -1;
}

float RichRingR::getBetaExpectedRms(){
  if(RichBetaUniformityCorrection::getHead())     // Use the uniformity corrections, which should be better
    return RichBetaUniformityCorrection::getHead()->getRms(this);

  return -1;
}


float RichRingR::getChargeExpectedResolution(){
  if(RichChargeUniformityCorrection::getHead())     // Use the uniformity corrections, which should be better
    return RichChargeUniformityCorrection::getHead()->getWidth(this);

  return -1;
}

float RichRingR::getChargeExpectedRms(){
  if(RichChargeUniformityCorrection::getHead())     // Use the uniformity corrections, which should be better
    return RichChargeUniformityCorrection::getHead()->getRms(this);

  return -1;
}


float RichRingR::getPMTChargeConsistency(){
  // Compute the charge using iterators
  if(NpExpPMT.size()<1) return -1;

  double totalCol=0,totalExp=0;
  typedef map<unsigned short,float>::iterator It;
  for(It i=NpColPMT.begin();i!=NpColPMT.end();i++) totalCol+=i->second;
  for(It i=NpExpPMT.begin();i!=NpExpPMT.end();i++) totalExp+=i->second;

  if(totalExp==0) return -1;

  double charge2=totalCol/totalExp;

  double acc=0;
  for(It i=NpExpPMT.begin();i!=NpExpPMT.end();i++){
    double sigma=charge2/i->second;
    if(NpColPMT.find(i->first)==NpColPMT.end()) continue;
    double lcharge=NpColPMT.find(i->first)->second/i->second;
    lcharge-=charge2;
    lcharge*=lcharge/sigma;
    acc+=lcharge;
  }

  acc/=NpExpPMT.size();
  return acc;
}

void RichRingR::calPush(double beta,double index,float x,float y){
  if(isCalibrating()){cerr<<"RichRingR::calPush -- should not be used if calibrating."<<endl;return;}


  int tile=getTileIndex(x,y);
  if(tile<0) return;                                // Reasonable tile
  if(indexHistos[tile].GetNbinsX()==1)
    indexHistos[tile].SetBins(200,1/1.33,1.015);



  // Add the current event
  indexHistos[tile].Fill(beta);
  
  // Keep track of the mean index of the tile
  _sumIndex[tile]+=index;
  _totalIndex[tile]++;

  // Check if we have to update the calibration constants
  if(indexHistos[tile].GetEntries()<_tileCalEvents) return;

  int peak_bin=indexHistos[tile].GetMaximumBin();

  double weight=0,sum=0;
  for(int i=-_tileCalWindow;i<=_tileCalWindow;i++){
    weight+=indexHistos[tile].GetBinContent(peak_bin+i);
    sum+=indexHistos[tile].GetBinContent(peak_bin+i)*indexHistos[tile].GetXaxis()->GetBinCenter(peak_bin+i);
  }
  
  if(weight<_tileCalEvents) return;
  sum/=weight;

  double mean_index=_sumIndex[tile]/_totalIndex[tile];

  //
  // We  got the value, store it
  //
  if(_lastUpdate[tile]==1)
    indexCorrection[tile]=sum*mean_index;
  else
    indexCorrection[tile]=sum*mean_index*_tileLearningFactor+indexCorrection[tile]*(1-_tileLearningFactor); 

  _lastUpdate[tile]=2;
  _numberUpdates[tile]++;
  indexHistos[tile].Reset();
  _sumIndex[tile]=0;
  _totalIndex[tile]=0;
}

bool RichRingR::calSelect(AMSEventR &event){
#define SELECT(_name,_condition) {if(!(_condition)) return false;}
  SELECT("1 Tracker, RICH particle",(event.fStatus&0x33)==0x31);
  SELECT("No antis",!(event.fStatus&(0x3<<21)));
  SELECT("At most 1 trd track",(event.fStatus&(0x3<<8))<=(0x1<<8));
  SELECT("At most 4 tof clusters",(event.fStatus&(0x7<<10))<=(0x4<<10));
  SELECT("At least 1 tr track",event.fStatus&(0x3<<13));
  SELECT("At least 1 rich ring",event.fStatus&(0x3<<15));
  SELECT("At most 1 crossing particle at rich",event.pParticle(0)->RichParticles<=1);

  RichRingR *ring=event.Particle(0).pRichRing();
  SELECT("PointersCheck",ring && ring->iTrTrack()>-1);
  SELECT("Charge is one",ring->Used/ring->NpExp<1.5*1.5);
  SELECT("Ring is clean",ring->IsClean());
  return true;
#undef SELECT
}


void RichRingR::switchDynCalibration(){
  _updateDynamicCalibration=1-_updateDynamicCalibration;
  if(_updateDynamicCalibration) cout<<"RichRingR Dynamic calibration activated"<<endl;
  else{
    cout<<"RichRingR Dynamic calibration de-activated"<<endl;
    for(int i=0;i<122;i++) {indexCorrection[i]=_lastUpdate[i]=1;indexHistos[i].Reset();_numberUpdates[i]=_totalIndex[i]=_sumIndex[i]=0;}
  }
}

void RichRingR::updateCalibration(AMSEventR &event){
  RichRingR *ring=event.Particle(0).pRichRing();
  if(!ring) return;
  int tile=ring->getTileIndex();
  if(tile<0) return;                                // Reasonable tile
  if(fabs(event.Particle(0).Momentum)<_pThreshold) return; // beta=1
  if(ring->IsNaF()) tile=121;
  RichRingR::_isCalibrationEvent=true;
  if(indexHistos[tile].GetNbinsX()==1){
    if(!ring->IsNaF()) indexHistos[tile].SetBins(100,0.95,1.005); 
    else indexHistos[tile].SetBins(200,1/1.33,1.015);
  }
  // Add the current event
  indexHistos[tile].Fill(ring->Beta);

  // Keep track of the mean index of the tile
  _sumIndex[tile]+=ring->getRawIndexUsed();
  _totalIndex[tile]++;
  
  // Check if we have to update the calibration constants
  if(indexHistos[tile].GetEntries()<_tileCalEvents) return;

  int peak_bin=indexHistos[tile].GetMaximumBin();

  double weight=0,sum=0;
  for(int i=-_tileCalWindow;i<=_tileCalWindow;i++){
    weight+=indexHistos[tile].GetBinContent(peak_bin+i);
    sum+=indexHistos[tile].GetBinContent(peak_bin+i)*indexHistos[tile].GetXaxis()->GetBinCenter(peak_bin+i);
  }
  
  if(weight<_tileCalEvents) return;
  sum/=weight;

  //
  // We  got the value, store it
  //
  int now=event.fHeader.Time[0];
  if(now<_lastUpdate[tile]-30*60 || _lastUpdate[tile]==1)
    //    indexCorrection[tile]=sum*mean_index;
    indexCorrection[tile]=sum;
  else
    //    indexCorrection[tile]=sum*mean_index*_tileLearningFactor+indexCorrection[tile]*(1-_tileLearningFactor); 
    indexCorrection[tile]=sum*_tileLearningFactor+indexCorrection[tile]*(1-_tileLearningFactor); 

  _lastUpdate[tile]=now;
  _numberUpdates[tile]++;
  indexHistos[tile].Reset();
  _sumIndex[tile]=0;
  _totalIndex[tile]=0;
}

int RichRingR::updates(){
  int t=getTileIndex();
  if(t<0) return 0;
  if(IsNaF()) t=121;
  return _numberUpdates[t];
}

int RichRingR::updates(float x,float y){
  int t=getTileIndex(x,y);
  if(t<0) return 0;
  return _numberUpdates[t];
}



bool RichRingR::buildChargeCorrections(){
  NpColCorr.clear();
  NpExpCorr.clear();

  pmtCorrectionsFailed = 1;

  // If it is MC skip use a trivial compuation
  if(AMSEventR::Head()->nMCEventgC()){
    for(map<unsigned short,float>::iterator it=NpColPMT.begin();
	it!=NpColPMT.end();it++) NpColCorr[it->first]=1;
    for(map<unsigned short,float>::iterator it=NpExpPMT.begin();
	it!=NpExpPMT.end();it++) NpExpCorr[it->first]=1;
    pmtCorrectionsFailed = 0;
    return true;
  }

  // If it is data, check that the required tools are OK
  RichPMTCalib::currentDir=correctionsDir;
  RichPMTCalib::useRichRunTag = useRichRunTag;
  RichPMTCalib::usePmtStat = usePmtStat;
  RichPMTCalib::useSignalMean = useSignalMean;
  RichPMTCalib::useGainCorrections = useGainCorrections;
  RichPMTCalib::useEfficiencyCorrections = useEfficiencyCorrections;
  RichPMTCalib::useBiasCorrections = useBiasCorrections;
  RichPMTCalib::useTemperatureCorrections = useTemperatureCorrections;
  RichPMTCalib::useEffectiveTemperatureCorrection = useEffectiveTemperatureCorrection;

  RichConfigManager::useExternalFiles = useExternalFiles;
  RichConfigManager::reloadTemperatures = reloadTemperatures;
  RichConfigManager::reloadRunTag = reloadRunTag;
  RichConfigManager::reloadPMTs = reloadPMTs;

  RichPMTCalib *corr=RichPMTCalib::Update();
  if(!corr) return false;

  // Correct the collected 
  for(map<unsigned short,float>::iterator it=NpColPMT.begin();
      it!=NpColPMT.end();it++){
    int pmt=it->first;
    if(find(corr->BadPMTs.begin(), corr->BadPMTs.end(), pmt)!=corr->BadPMTs.end()) NpColCorr[pmt]=0;
    else NpColCorr[pmt] = (corr->GainCorrection(pmt)>0 && corr->GainTemperatureCorrection(pmt)>0) ?
      1.0 / corr->GainCorrection(pmt) / corr->GainTemperatureCorrection(pmt) : 0;
  }

  // Correct the expected
  for(map<unsigned short,float>::iterator it=NpExpPMT.begin();
      it!=NpExpPMT.end();it++){
    int pmt=it->first;
    if(find(corr->BadPMTs.begin(), corr->BadPMTs.end(), pmt)!=corr->BadPMTs.end()) NpExpCorr[pmt]=0;
    else NpExpCorr[pmt]=corr->EfficiencyCorrection(pmt) * corr->EfficiencyTemperatureCorrection(pmt)
      * corr->EffectiveTemperatureCorrection();
  }

  pmtCorrectionsFailed = 0;
  return true;
}


int RichRingR::getUsedHits(bool corr) {
  if (!corr || !loadPmtCorrections) return Used;

  if(NpColPMT.size() && !NpColCorr.size())
    if(!buildChargeCorrections()) return Used;

  AMSEventR *event=AMSEventR::Head();
  if(!event) return 0;
  int sum=0;
  for(int i=0; i<Used; i++){
    RichHitR *hit = event->pRichHit(iRichHit(i));
    if (!hit) continue;
    int pmt=hit->Channel/16;
    if (NpColCorr.find(pmt) == NpColCorr.end()) continue;
    sum += (NpColCorr[pmt]>0);
  }
  return sum;
}


int RichRingR::getUsedHits(int pmt, bool corr) {
  AMSEventR *event=AMSEventR::Head();
  if(!event) return 0;
  int sum=0;
  for(int i=0; i<Used; i++){
    RichHitR *hit = event->pRichHit(iRichHit(i));
    if (!hit || hit->Channel/16!=pmt) continue;
    if ((!corr || !loadPmtCorrections) && ++sum) continue;
    if(NpColPMT.size() && !NpColCorr.size())
      if(!buildChargeCorrections() && ++sum) continue;
    if (NpColCorr.find(pmt) == NpColCorr.end()) continue;
    sum += (NpColCorr[pmt]>0);
  }
  return sum;
}


float RichRingR::getPhotoElectrons(bool corr){
  if (!corr || !loadPmtCorrections){
    float myNpCol=0;
    for(map<unsigned short,float>::iterator i=NpColPMT.begin();
	i!=NpColPMT.end();i++) myNpCol+=i->second;
      return myNpCol;
  }

  float sum=0;
  for(map<unsigned short,float>::iterator i=NpColPMT.begin();
      i!=NpColPMT.end();i++)
    sum+=getPhotoElectrons(i->first, corr);

  return sum;
}


float RichRingR::getPhotoElectrons(int pmt, bool corr){
  map<unsigned short,float>::iterator i=NpColPMT.find(pmt);
  if (i==NpColPMT.end()) return 0;

  if (!corr || !loadPmtCorrections) return i->second;

  if(NpColPMT.size() && !NpColCorr.size()) 
    if(!buildChargeCorrections()) return i->second;

  map<unsigned short,float>::iterator correction=NpColCorr.find(i->first);
  return i->second*(correction==NpColCorr.end()?0:correction->second);
}


float RichRingR::getExpectedPhotoElectrons(bool corr){
  if (!corr){
    float myNpExp=0;
    for(map<unsigned short,float>::iterator i=NpExpPMT.begin();
	i!=NpExpPMT.end();i++) myNpExp+=i->second;
    return myNpExp;
  }

  float sum=0;
  for(map<unsigned short,float>::iterator i=NpExpPMT.begin();
      i!=NpExpPMT.end();i++)
    sum+=getExpectedPhotoElectrons(i->first, corr);

  if(RichChargeUniformityCorrection::getHead())
    sum*=RichChargeUniformityCorrection::getHead()->getCorrection(this);


  return sum;
}


float RichRingR::getExpectedPhotoElectrons(int pmt, bool corr){
  map<unsigned short,float>::iterator i=NpExpPMT.find(pmt);
  if (i==NpExpPMT.end()) return 0;

  if (!corr || !loadPmtCorrections) return i->second;

  if(NpExpPMT.size() && !NpExpCorr.size())
    if(!buildChargeCorrections()) return i->second;

  map<unsigned short,float>::iterator correction=NpExpCorr.find(i->first);
  return i->second*(correction==NpExpCorr.end()?0:correction->second);
}


double RichRingR::betaCorrection(float index,float x,float y){
  int t=getTileIndex(x,y);
  if(t<0) return 1;
  if(_lastUpdate[t]==1) return 1; // defaults to no correction
  //  return index/indexCorrection[t];
  return 1.0/indexCorrection[t];
}

double RichRingR::betaCorrection(){
  if(RichBetaUniformityCorrection::getHead())     // Use the uniformity corrections, which should be better
    return RichBetaUniformityCorrection::getHead()->getCorrection(this);


  int t=getTileIndex();
  if(t<0) return 1;
  if(_lastUpdate[t]==1) return 1;
  if(IsNaF()) t=121;
  //  return getRawIndexUsed()/indexCorrection[t];
  return 1.0/indexCorrection[t];
}

int RichRingR::getTileIndex(float x,float y){
  const int grid_side_length=11;
  const int n_tiles=grid_side_length*grid_side_length;
  const double tile_width=0.1+11.5; 

  if(fabs(x)<tile_width*1.5 && fabs(y)<tile_width*1.5) return 121;

  int nx=int(x/tile_width+5.5);
  int ny=int(y/tile_width+5.5);
  int t=ny*grid_side_length+nx;
  return t>n_tiles?-1:t;
}



int RichRingR::getTileIndex(){
  const int grid_side_length=11;
  const int n_tiles=grid_side_length*grid_side_length;
  const double tile_width=0.1+11.5; 

  float &x=TrRadPos[0];
  float &y=TrRadPos[1];

  int nx=int(x/tile_width+5.5);
  int ny=int(y/tile_width+5.5);
  int t=ny*grid_side_length+nx;
  return t>n_tiles?-1:t;
}


void RichRingR::FillRichHits(int ring){
  fRichHit.clear();
  for(unsigned int i=0;i<AMSEventR::Head()->NRichHit();i++){
    RichHitR hit=AMSEventR::Head()->RichHit(i);
    if((hit.Status>>ring)%2){
      fRichHit.push_back(i);
    }
  }
  if(Used!=int(fRichHit.size()))cerr<<" problem hits for ring "<<ring<<" "<<Used<<" "<<fRichHit.size()<<endl;
}

float RichRingR::getBetaConsistency(){
  AMSEventR *event=AMSEventR::Head();
  if(event->nRichRingB()<=0) return -1;
  for(int i=0;i<event->nRichRingB();i++){
    RichRingBR *p=event->pRichRingB(i);
    if(!p) continue;
    if(pTrTrack()==p->pTrTrack() && (p->Status%10)==2){
      return fabs(getBeta()-p->Beta);
    }
  }
  return -1;
}

int RichRingR::getPMTs(){
  AMSEventR *event=AMSEventR::Head();
  if(!event) return 0;
  bool counted[680];
  for(int i=0;i<680;counted[i++]=false); 
  int counter=0;
  for(int i=0;i<Used;i++){
    RichHitR *hit=event->pRichHit(iRichHit(i));
    if(!hit) continue;
    if(hit->IsCrossed()) continue;
    int pmt=hit->Channel/16;
    if(counted[pmt]) continue;
    counted[pmt]=true;
    counter++;
  }
  return counter;
}


RichRingR::RichRingR(AMSRichRing *ptr, int nhits) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    Used  = ptr->_used;
    UsedM = ptr->_mused;
    Beta  = ptr->_beta;
    ErrorBeta = ptr->_errorbeta;
    BetaRefit = ptr->_wbeta;
    Status    = ptr->_status;
    NpCol= ptr->_collected_npe;
    NpColLkh= ptr->_collected_npe_lkh;
    NpExp     = ptr->_npexp;
    Prob    = ptr->_probkl;
    UDist = ptr->_unused_dist;
    Theta   =ptr->_theta;
    ErrorTheta  =ptr->_errortheta;
    for(int i=0;i<3;i++){
      TrRadPos[i]  = ptr->_radpos[i];
      TrPMTPos[i]  = ptr->_pmtpos[i];
    }
    for(int i=0;i<5;i++)
      AMSTrPars[i] = ptr->_crossingtrack[i];

    // Control variables
    int hits = (ptr->_hit_pointer).size();

    fBetaHit.clear();

    int write_mode=(RICRECFFKEY.recon[0]/1000)%10;
    
    if(write_mode==1)
      for(int i=0;i<nhits;i++){
	float value=0;
	for(int j=0;j<hits;j++){
	  if((ptr->_hit_pointer)[j]==i){
#ifdef __AMSDEBUG__
	    cout<<"FOUND "<<(ptr->_hit_pointer)[j]<<" HIT DOUBLE "<<i<<endl;
#endif	  
	    if((ptr->_hit_used)[j]==0)
	      value=(ptr->_beta_direct)[j];
	    if((ptr->_hit_used)[j]==1)
	      value=-(ptr->_beta_reflected)[j];
	    break;
	  }
	}
#ifdef __AMSDEBUG__
	cout<<"PUSHING "<<value<<endl;
#endif
	fBetaHit.push_back(value);
      }

    if(write_mode==2){ 	// Store all the betas for all the hits
      for(unsigned int i=0;i<(ptr->_beta_direct).size();i++)
	if((ptr->_beta_direct)[i]>0) fBetaHit.push_back((ptr->_beta_direct)[i]);
      for(unsigned int i=0;i<(ptr->_beta_reflected).size();i++)
	if((ptr->_beta_reflected)[i]>0) fBetaHit.push_back((ptr->_beta_reflected)[i]);
    }
  
    for(int i=0;i<10;i++){
      UsedWindow[i]=ptr->_collected_hits_window[i];
      NpColWindow[i]=ptr->_collected_pe_window[i];
    }

  } else {
    cout<<"RICRingR -E- AMSRichRing ptr is NULL"<<endl;
  }
  
  // Fill the information per PMT. 
  NpCol=0;
  NpExp=0;
  for(unsigned short pmtNb=0;pmtNb<680;pmtNb++){
    if(ptr->NpColPMT[pmtNb]>1e-6) {NpColPMT[pmtNb]=ptr->NpColPMT[pmtNb];NpCol+=ptr->NpColPMT[pmtNb];}
    if(ptr->NpExpPMT[pmtNb]>1e-6) {NpExpPMT[pmtNb]=ptr->NpExpPMT[pmtNb];NpExp+=ptr->NpExpPMT[pmtNb];}
  }
  
#endif
}

double RichRingR::RingWidth(bool usedInsteadNpCol){
  double weight=0,sum=0;

  for(int i=0;i<10;i++){
    double w;
    if(usedInsteadNpCol) w=UsedWindow[i]-(i>0?UsedWindow[i-1]:0);
    else w=NpColWindow[i]-(i>0?NpColWindow[i-1]:0);
    weight+=w;
    sum+=(0.5+i)*(0.5+i)*w;
  }
  if(weight) return sqrt(sum/weight); else return -1;
}


RichRingTables::RichRingTables(){
#define Do(_x) _x[i]=RichRingR::_x[i]
  for(int i=0;i<122;i++){
    Do(indexHistos);
    Do(_sumIndex); 
    Do(_totalIndex); 
    Do(indexCorrection);
    Do(_lastUpdate); 
    Do(_numberUpdates);
  }
#undef Do
}

void RichRingTables::Save(TString fileName){
#pragma omp critical
  {
  TFile file(fileName,"RECREATE");
  RichRingTables tables;
  tables.Write("RichRingTables");
  file.Close();
  }
}

bool RichRingTables::Load(TString fileName){
  bool fail=false;  
#pragma omp critical
  {
    fail=false;
    RichRingTables *tables=0;
    TFile *currentFile=0;
    if(gDirectory) currentFile=gDirectory->GetFile();
    TFile file(fileName);
    tables=(RichRingTables*)file.Get("RichRingTables");

    if(!tables) fail=true;
    
#define Do(_x) RichRingR::_x[i]=tables->_x[i]
    if(!fail)
      for(int i=0;i<122;i++){
	Do(indexHistos);
	Do(_sumIndex); 
	Do(_totalIndex); 
	Do(indexCorrection);
	Do(_lastUpdate); 
	Do(_numberUpdates);
      }
#undef Do
    if(tables) delete tables;
    file.Close();

    // Return to the previous root directory
    if(currentFile) currentFile->cd();
  }
  
  if(fail) return false;
  return true;
}



//void RichRingBR::FillRichHits(int ringnew){
//  fRichHit.clear();
//  for(int i=0;i<HitsStatus.size();i++) {
//    if(HitsStatus[i]>=0) {
//      cout << "Adding hit " << i << " to RichRingBR" << endl;
//      fRichHit.push_back(i);
//    }
//  }
//}

float RichRingBR::getBetaConsistency(){
  float signedval = getBetaConsistencySigned();
  if(signedval>-1.01 && signedval<-0.99) return -1;
  return fabs(signedval);
}

float RichRingBR::getBetaConsistencySigned(){
  AMSEventR *event=AMSEventR::Head();
  if(event->nRichRing()<=0) return -1;
  for(int i=0;i<event->nRichRing();i++){
    RichRingR *p=event->pRichRing(i);
    if(!p) continue;
    if(pTrTrack()==p->pTrTrack()){
      return p->getBeta()-Beta;
    }
  }
  return -1;
}

RichRingBR::RichRingBR(AMSRichRingNew *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    Status = ptr->_Status;
    Beta = ptr->_Beta;
    AngleRec = ptr->_AngleRec;
    AngleRecErr = ptr->_AngleRecErr;
    Likelihood = ptr->_Likelihood;
    Used = ptr->_Used;
    ProbKolm = ptr->_ProbKolm;
    for(int i=0;i<2;i++){
      Flatness[i] = (ptr->_Flatness)[i];
    }
    ChargeRec = ptr->_ChargeRec;
    for(int i=0;i<3;i++){
      ChargeProb[i] = (ptr->_ChargeProb)[i];
    }
    ChargeRecDir = ptr->_ChargeRecDir;
    ChargeRecMir = ptr->_ChargeRecMir;
    NpeRing = ptr->_NpeRing;
    NpeRingDir = ptr->_NpeRingDir;
    NpeRingRef = ptr->_NpeRingRef;
    for(int i=0;i<3;i++){
      RingAcc[i] = (ptr->_RingAcc)[i];
    }
    for(int i=0;i<6;i++){
      RingEff[i] = (ptr->_RingEff)[i];
    }
    for(int i=0;i<2;i++){
      RingAccRW[i] = (ptr->_RingAccRW)[i];
    }
    NMirSec = ptr->_NMirSec;
    for(int i=0;i<3;i++){
      RingAccMsec1R[i] = (ptr->_RingAccMsec1R)[i];
      RingAccMsec2R[i] = (ptr->_RingAccMsec2R)[i];
      RingEffMsec1R[i] = (ptr->_RingEffMsec1R)[i];
      RingEffMsec2R[i] = (ptr->_RingEffMsec2R)[i];
      RingAccRWMsec[i] = (ptr->_RingAccRWMsec)[i];
    }
    if(ptr->_UsePixelSegments==0) {  // use segment data by PMT
      for(int i=0;i<ptr->_Segments;i++) {
	RichRingBSegmentR seg;
	seg.PMT = (short int) (ptr->_SegPMT)[i];
	seg.Pixel = -1;
	seg.RefStatus = (short int) (ptr->_SegRefStatus)[i];
	seg.Theta = (ptr->_SegTheta)[i];
	seg.Phi = (ptr->_SegPhi)[i];
	seg.Acceptance = (ptr->_SegAcceptance)[i];
	seg.EffRad = (ptr->_SegEffRad)[i];
	seg.EffFull = (ptr->_SegEffFull)[i];
	Segments[(ptr->_SegPMT)[i]].push_back(seg);
      }
    }
    else {  // use segment data by pixel
      for(int i=0;i<ptr->_PixelSegments;i++) {
	RichRingBSegmentR seg;
	seg.PMT = (short int) ((ptr->_PixSegPixel)[i])/16;
	seg.Pixel = (short int) ((ptr->_PixSegPixel)[i])%16;
	seg.RefStatus = (short int) (ptr->_PixSegRefStatus)[i];
	seg.Theta = (ptr->_PixSegTheta)[i];
	seg.Phi = (ptr->_PixSegPhi)[i];
	seg.Acceptance = (ptr->_PixSegAcceptance)[i];
	seg.EffRad = (ptr->_PixSegEffRad)[i];
	seg.EffFull = (ptr->_PixSegEffFull)[i];
	Segments[((ptr->_PixSegPixel)[i])/16].push_back(seg);
      }
    }
    HitsResiduals = ptr->_HitsResiduals;
    HitsStatus = ptr->_HitsStatus;
    TrackRec = ptr->_TrackRec;

    fRichHit = ptr->_HitsAssoc;

  } else {
    cout<<"RICRingNewR -E- AMSRichRingNew ptr is NULL"<<endl;
  }
#endif
}

DaqEventR::DaqEventR(DAQEvent *ptr){
#ifndef __ROOTSHAREDLIBRARY__ 
  L3Error=ptr->lv3.pack();
  Length=ptr->getlength();
  Tdr=ptr->getsublength(0);
  Udr=ptr->getsublength(1);
  Sdr=ptr->getsublength(2);
  Rdr=ptr->getsublength(3);
  Edr=ptr->getsublength(4);
  L1dr=ptr->getsublength(5);
  L3dr=ptr->_lvl3[0] | (ptr->_lvl3[1]<<16);
  L3VEvent=ptr->_lvl3[3] | (ptr->_lvl3[2]<<16);;
  L3TimeD=ptr->_lvl3[4];
  for(unsigned int k=0;k<sizeof(JError)/sizeof(JError[0]);k++){
      JError[k]=ptr->getjerror(k);
  }
  for(int ii=0;ii<4;ii++)
  JINJStatus[ii]=ptr->getjstatus(ii);
  
#endif
}   


EcalHitR::EcalHitR(AMSEcalHit *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Idsoft = ptr->_idsoft;
  Proj   = ptr->_proj;
  Plane  = ptr->_plane;
  Cell   = ptr->_cell;
  Edep   = ptr->_edep;
  EdCorr = ptr->_edepc;
  if (Proj) {
    Coo[0]= ptr->_cool;
    Coo[1]= ptr->_coot;
  }
  else{     //<-- x-proj
    Coo[0]= ptr->_coot;
    Coo[1]= ptr->_cool;
  }
  Coo[2]  = ptr->_cooz;

  AttCor  = ptr->_attcor;
  AMSECIds ic(ptr->getid());
  ADC[0] = ptr->_adc[0];
  //  ADC[1] = ptr->_adc[1]*ic.gethi2lowr();
  //  ADC[2] = ptr->_adc[2]*ic.getan2dyr();
  ADC[1] = ptr->_adc[1];
  ADC[2] = ptr->_adc[2];

  Gain   = ic.getgain();

#endif
}


float EcalHitR::GetECALPed(int layer, int cell, int channel) {

	/* Pedestals are retrieved for each PM via
	 * AMSEventR::Head()->GetTDVEl(rmsped,iu,value);
	 * with rmsped="EcalPeds", iu = index and value being the return value
	 * Each PM stores 28 informations :
	 *	1-9  	-> initialisation ?
	 *	10-13 	-> for each pixel, HG ped
	 *	14-17 	-> for each pixel, LG ped
	 *	18-21 	-> for each pixel, HG rms
	 *	22-25 	-> for each pixel, LG rms
	 *	26		-> dynode ped
	 *	27		-> dynode RMS
	 *
	 *	Information for superlayer SL, photom. PM is thus stored btw
	 * 	28 * (36*SL + PM) <= idx < 28 * (36*SL + (PM+1))
	 *
	 *
	 * Indexes for a pixel in a PM are as follows :
	 *	0 1
	 * 	2 3
	 * 	0 and 1 have the same layer idx, 0 and 2 the same cell idx
	 */

	if (layer<0||layer>17||cell<0||cell>71||channel<0||channel>2) return -1;

	// SlowControlDB Initialisation :
	//AMSEventR* ev=chain->GetEvent(0);
	//SlowControlDB* scdb=SlowControlDB::GetPointer();
	AMSEventR::if_t value;
	value.u=0;

	// Variables initialisation
	int PM  = (int) cell/2;
	int SL  = (int) layer/2;
	int pxl = 2*(layer%2) + cell%2;
	int iPM = 28 * (36*SL + PM);
	int idx= iPM + 10 ;

	// Getting good idx
	if (channel==2) idx = iPM + 26; // dynode
	else 			idx = iPM + 10 + 4*channel + pxl;
	int err = AMSEventR::Head()->GetTDVEl("Ecalpeds",idx,value);
	if (err)return -2.;
	else 	return value.f;
}


float EcalHitR::GetECALRms(int layer, int cell, int channel) {
	// Format description : see EcalHitR::GetECALPed
	if (layer<0||layer>17||cell<0||cell>71||channel<0||channel>2) return -1;

	// SlowControlDB Initialisation :
	//AMSEventR* ev=chain->GetEvent(0);
	//SlowControlDB* scdb=SlowControlDB::GetPointer();
	AMSEventR::if_t value;
	value.u=0;

	// Variables initialisation
	int PM  = (int) cell/2;
	int SL  = (int) layer/2;
	int pxl = 2*(layer%2) + cell%2;
	int iPM = 28 * (36*SL + PM);
	int idx= 0 ;

	// Getting good idx
	if (channel==2) idx = iPM + 27; // dynode
	else 			idx = iPM + 18 + 4*channel + pxl;
	int err = AMSEventR::Head()->GetTDVEl("Ecalpeds",idx,value);
	if (err)return -2.;
	else 	return value.f;
}





EcalMCHitR::EcalMCHitR(AMSEcalMCHit *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->_idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->_xcoo[i];}
  TOF = ptr->_tof;
  Edep= ptr->_edep;
#endif
}



// Pointer related functions advanced users only
//


EcalHitR* EcalClusterR::pEcalHit(unsigned int i){
  return (AMSEventR::Head() && i<fEcalHit.size())?AMSEventR::Head()->pEcalHit(fEcalHit[i]):0;
}



EcalClusterR* Ecal2DClusterR::pEcalCluster(unsigned int i){
  return (AMSEventR::Head() && i<fEcalCluster.size())?AMSEventR::Head()->pEcalCluster(fEcalCluster[i]):0;
}

Ecal2DClusterR* EcalShowerR::pEcal2DCluster(unsigned int i){
  return (AMSEventR::Head() && i<fEcal2DCluster.size())?AMSEventR::Head()->pEcal2DCluster(fEcal2DCluster[i]):0;
}


TrTrackR* RichRingR::pTrTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
}


TrTrackR* RichRingBR::pTrTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
}


TofRawClusterR* TofClusterR::pTofRawCluster(unsigned int i){
  return (AMSEventR::Head() && i<fTofRawCluster.size())?AMSEventR::Head()->pTofRawCluster(fTofRawCluster[i]):0;
}


TofRawSideR* TofRawClusterR::pTofRawSide(unsigned int i){
  return (AMSEventR::Head() && i<fTofRawSide.size())?AMSEventR::Head()->pTofRawSide(fTofRawSide[i]):0;
}

int TofClusterHR::iTofRawSide(unsigned int i){
   vector<int>fraws;
   for(int is=0;is<2;is++)if(fTofRawSide[is]>=0)fraws.push_back(fTofRawSide[is]);
   return i<fraws.size()?fraws[i]:-1;
}

TofRawSideR* TofClusterHR::pTofRawSide(unsigned int i){
   vector<int>fraws;
   for(int is=0;is<2;is++)if(fTofRawSide[is]>=0)fraws.push_back(fTofRawSide[is]);
   return (AMSEventR::Head() && i<fraws.size())?AMSEventR::Head()->pTofRawSide(fraws[i]):0;
}

TofRawSideR* TofClusterHR::GetRawSideHS(int is){
  return (AMSEventR::Head() && is<int(fTofRawSide.size())&&fTofRawSide[is]>=0)?AMSEventR::Head()->pTofRawSide(fTofRawSide[is]):0;
}

int TofClusterHR::DefaultQOpt=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kBetaCor|TofRecH::kQ2Q);
int TofClusterHR::DefaultQ2Opt=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kBetaCor);
int TofClusterHR::DefaultQOptIon=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kBetaCor|TofRecH::kRigidityCor|TofRecH::kQ2Q);
int TofClusterHR::DefaultQOptIonW=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kDAWeight|TofRecH::kBetaCor|TofRecH::kRigidityCor|TofRecH::kQ2Q);

float TofClusterHR::GetEdepPM  (int pmtype, int is,int pm)  {

  double q2=pmtype==1?AQ2[is]:DQ2[is][pm];
  TofRecPar::IdCovert(Layer,Bar,is,pm);
  float edep=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,(TofRecH::kBirkCor|TofRecH::kQ2MeV),q2);
  return edep;
}


float TofClusterHR::GetQSignalPM(int pmtype,int is,int pm,int opt,float cosz,float beta){
   
   double q2=pmtype==1?AQ2[is]:DQ2[is][pm];
   TofRecPar::IdCovert(Layer,Bar,is,pm);
   float signal=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,opt,q2,0,double(cosz),double(beta));
   return signal;
}

float TofClusterHR::GetEdep(int pmtype,int pattern,int optw){

    if(pattern==111111&&optw==1){
       if     (pmtype==1)return AEdep;//Anode
       else if(pmtype==0)return DEdep;//Dynode
       else if((AEdep>0&&AEdep<6*6*TofCAlignPar::ProEdep)||DEdep<=0)return AEdep;//Anode Range
       else              return DEdep;
    }

///---rawqd
    double rawqd[2][TOFCSN::NPMTM]={{0}},rawqa[2]={0};
    for(int is=0;is<TOFCSN::NSIDE;is++){
       int patterns=pattern/int(pow(1000.,is));
       rawqa[is]=(patterns%10==0)?0:AQ2[is];
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
          int patternp=patterns/int(pow(10.,ipm));
          rawqd[is][ipm]=(patternp%10==0)?0:DQ2[is][ipm];
      }
    }

///---Anode Or Dynode Sum Information+BirkCor
    TofRecPar::IdCovert(Layer,Bar);
    float qa=TofRecH::SumSignalA(TofRecPar::Idsoft,rawqa,optw);
    float qd=TofRecH::SumSignalD(TofRecPar::Idsoft,rawqd,optw,1);
    float q2;
    if     (pmtype==1)q2=qa;//Anode
    else if(pmtype==0)q2=qd;//Dynode
    else  {
       if((qa>0&&qa<6*6)||qd<=0){q2=qa; pmtype=1;}//Anode  Range
       else                     {q2=qd; pmtype=0;}//Overflow or Q2>6*6 using Dynode
    }
    float signal=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,(TofRecH::kBirkCor|TofRecH::kQ2MeV),q2);
    return signal;
}

float TofClusterHR::GetQSignal(int pmtype,int opt,float cosz,float beta,float lcoo,int pattern,int optw){

//---Reconstruction
    if(pmtype==-1){
       if((AEdep>0&&AEdep<6*6*TofCAlignPar::ProEdep)||DEdep<=0){return  AEdep>=0?sqrt(AEdep/TofCAlignPar::ProEdep):AEdep;}//Anode Range
       else                                                    {return  DEdep>=0?sqrt(DEdep/TofCAlignPar::ProEdep):DEdep;}
    }

///--Default 
     if(opt==TofClusterHR::DefaultQOpt&&cosz==1&&beta==1&&lcoo==-1000){
       int qpat=pmtype*100000000+pattern*100+optw;
       map<int, float >::iterator it;
       it=QPar.find(qpat); 
       if(it!=QPar.end()){return QPar[qpat];}
    }
 

//---Attenuation by external Coo
     float lcoo1=Coo[GetDirection()];
     float rawqd1[2][TOFCSN::NPMTM]={{0}},rawqa1[2]={0};
     if(lcoo!=-1000){
       float aedep1,dedep1;
       TofRecH::EdepRecR(Layer,Bar,Aadc,Dadc,lcoo,rawqa1,rawqd1,aedep1,dedep1);
       lcoo1=lcoo;
     }
     else {
        for(int is=0;is<TOFCSN::NSIDE;is++){
          rawqa1[is]=AQ2[is];
          for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
            rawqd1[is][ipm]=DQ2[is][ipm]; 
         } 
       }
     }

//--Mask
    double rawqd[2][TOFCSN::NPMTM]={{0}},rawqa[2]={0};
    for(int is=0;is<TOFCSN::NSIDE;is++){
       int patterns=pattern/int(pow(1000.,is));
       rawqa[is]=(patterns%10==0)?0:rawqa1[is];
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
          int patternp=patterns/int(pow(10.,ipm));
          rawqd[is][ipm]=(patternp%10==0)?0:rawqd1[is][ipm];
      }
    }
  
///---Anode Or Dynode Sum Information+Theta// BirkCor+Beta Corr
    int npmtype=pmtype;   
    TofRecPar::IdCovert(Layer,Bar);
    float qa=TofRecH::SumSignalA(TofRecPar::Idsoft,rawqa,optw);
    float qd=TofRecH::SumSignalD(TofRecPar::Idsoft,rawqd,optw,1);
    float q2;
    if     (npmtype==1)q2=qa;//Anode
    else if(npmtype==0)q2=qd;//Dynode
    else  {
       if((qa>0&&qa<6*6)||qd<=0){q2=qa; npmtype=1;}//Anode  Range
       else                     {q2=qd; npmtype=0;}//Overflow or Q2>6*6 using Dynode
    }
    float signal=TofRecH::GetQSignal(TofRecPar::Idsoft,npmtype,opt,q2,lcoo1,double(cosz),double(beta));

//--Default 
     if(opt==TofClusterHR::DefaultQOpt&&cosz==1&&beta==1&&lcoo==-1000){
       int qpat=pmtype*100000000+pattern*100+optw;
       QPar[qpat]=signal;
       qpat=npmtype*100000000+pattern*100+optw;
       QPar[qpat]=signal;
    }
//---

    return signal;
}

int TofClusterHR::GetNPMTQ(int pmtype){
     int npmtd=0,npmta=0;
     for(int is=0;is<TOFCSN::NSIDE;is++){
       if(AQ2[is]>0)npmta++;
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
          if(DQ2[is][ipm]>0)npmtd++;
      }
    }
   return pmtype==1?npmta:npmtd; 
}

#ifndef _PGTRACK_
TrClusterR* TrRecHitR::pTrCluster(char xy){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrCluster(xy=='x'?fTrClusterX:fTrClusterY):0;
}


TrRecHitR* TrTrackR::pTrRecHit(unsigned int i){
  return (AMSEventR::Head() && i<fTrRecHit.size())?AMSEventR::Head()->pTrRecHit(fTrRecHit[i]):0;
}
#endif

TrdRawHitR* TrdClusterR::pTrdRawHit(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrdRawHit(fTrdRawHit):0;
}

double TrdClusterR::RangeCorr(double range, double norm){
  double p[3]={2.83105,-2.56767,+4.53374};
  double b[2]={0.3,0.75};
   double corr=1;
    double rng=range;
  if(rng<b[0])rng=b[0];
  if(rng>b[1])rng=b[1];
  
  for(unsigned int i=0;i<sizeof(p)/sizeof(p[0]);i++){
    corr+=p[i]*pow(rng,double(i));
  }
  return corr/norm;
}
double TrdClusterR::Range(float coo[], float theta,float phi){
double R=0.3;
AMSPoint  c1(Coo);
AMSPoint c2(coo);
double xdir=Direction==0?1:0;
double ydir=1-xdir;
AMSDir n(xdir,ydir,0);
AMSDir l(theta,phi);
AMSPoint delta=c1-c2;
double nl=n.prod(l);
double dn=delta.prod(n);
double dl=delta.prod(l);
double d2=delta.prod(delta);
double a=1-nl*nl;
double b=dn*nl-dl;
double c=d2-dn*dn-R*R;
double d=b*b-a*c;
double ret=d<0?0:2*sqrt(d)/a;
return ret; 
}

TrdClusterR* TrdSegmentR::pTrdCluster(unsigned int i){
  return (AMSEventR::Head() && i<fTrdCluster.size())?AMSEventR::Head()->pTrdCluster(fTrdCluster[i]):0;
}

//TrdRawHitR* TrdHSegmentR::pTrdRawHit(unsigned int i){
//  return (AMSEventR::Head() && i<fTrdRawHit.size())?AMSEventR::Head()->pTrdRawHit(fTrdRawHit[i]):0;
//}

TrdSegmentR* TrdTrackR::pTrdSegment(unsigned int i){
  return (AMSEventR::Head() && i<fTrdSegment.size())?AMSEventR::Head()->pTrdSegment(fTrdSegment[i]):0;
}

//TrdHSegmentR* TrdHTrackR::pTrdHSegment(unsigned int i){
//  return (AMSEventR::Head() && i<fTrdHSegment.size())?AMSEventR::Head()->pTrdHSegment(fTrdHSegment[i]):0;
//}


TofClusterR* BetaR::pTofCluster(unsigned int i){
  return (AMSEventR::Head() && i<fTofCluster.size())?AMSEventR::Head()->pTofCluster(fTofCluster[i]):0;
}

TrTrackR* BetaR::pTrTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
}

//---TofClusterH
int TofClusterHR::TRecover(float  tklcoo,int useside,float &tm,float &etm){
  int idsoft=Layer*1000+Bar*100;
  unsigned int status=Status;
  TofRecH::Init(); 
  return  TofRecH::TRecover(idsoft,tklcoo,Rtime,tm,etm,status,useside);
}

//-------TofBetaPar
void TofBetaPar::Init(){
  Status=SumHit=UseHit=0;
  for(int ilay=0;ilay<4;ilay++){
    Pattern[ilay]=0;
    Len[ilay]=0;
    Time[ilay]=0;
    ETime[ilay]=1000;
    TResidual[ilay]=1000;
    CosZ[ilay]=1;
    AEdepL[ilay]=0;
    DEdepL[ilay]=0;
    for(int is=0;is<2;is++){
      CResidual[ilay][is]=0;AQ2L[ilay][is]=0;
      for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){DQ2L[ilay][is][ipm]=0;}
     }
     QL[ilay].clear();//map
   }
   Beta=BetaC=InvErrBeta=InvErrBetaC=Chi2T=T0=Chi2C=0;
   Mass=EMass=0;
   QPar.clear();
};

const TofBetaPar& TofBetaPar::operator=(const TofBetaPar &right){
  Status=right.Status;
  SumHit=right.SumHit;
  UseHit=right.UseHit;
  for(int ilay=0;ilay<4;ilay++){
    Pattern[ilay]=right.Pattern[ilay];
    Len[ilay]=right.Len[ilay];
    Time[ilay]=right.Time[ilay];
    ETime[ilay]=right.ETime[ilay];
    TResidual[ilay]=right.TResidual[ilay];
    CosZ[ilay]=right.CosZ[ilay];
    AEdepL[ilay]=right.AEdepL[ilay];
    DEdepL[ilay]=right.DEdepL[ilay];
    for(int is=0;is<2;is++){
       CResidual[ilay][is]=right.CResidual[ilay][is];AQ2L[ilay][is]=right.AQ2L[ilay][is];
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){DQ2L[ilay][is][ipm]=right.DQ2L[ilay][is][ipm];}
      }
    QL[ilay]=right.QL[ilay];//map
  }
  T0=right.T0;
  Beta=right.Beta;
  BetaC=right.BetaC;
  InvErrBeta=right.InvErrBeta;
  InvErrBetaC=right.InvErrBetaC;
  Chi2T=right.Chi2T;
  Chi2C=right.Chi2C;
//---
  Mass=right.Mass;
  EMass=right.EMass;
  QPar=right.QPar;
  return *this;
}

void TofBetaPar::SetFitPar(float _Beta,float _BetaC,float _InvErrBeta,float _InvErrBetaC,float _Chi2T,float _T0){
    Beta=_Beta;BetaC= _BetaC;
    InvErrBeta=_InvErrBeta;InvErrBetaC=_InvErrBetaC;
    Chi2T=_Chi2T;T0=_T0;
}

void TofBetaPar::CalFitRes(){
  for(int ilay=0;ilay<4;ilay++){
    if((Pattern[ilay]%10>0)){
      TResidual[ilay]=Time[ilay]-(Len[ilay]/Beta/TofRecH::cvel+T0);
    }
  }
}

//-------BetaH
TrTrackR* BetaHR::pTrTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
}

TrdTrackR* BetaHR::pTrdTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrdTrack(fTrdTrack):0;
}

EcalShowerR* BetaHR::pEcalShower(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pEcalShower(fEcalShower):0;
}


TofClusterHR* BetaHR::pTofClusterH(unsigned int i){
  return (AMSEventR::Head() && i<fTofClusterH.size())?AMSEventR::Head()->pTofClusterH(fTofClusterH[i]):0;
}

//-------BetaH Function
TofClusterHR* BetaHR::GetClusterHL(int ilay){
  if(ilay<0||ilay>=4||fLayer[ilay]<0)return 0;
  return (AMSEventR::Head())?AMSEventR::Head()->pTofClusterH(fLayer[ilay]):0;
}


//------TofChargeH Function
int  BetaHR::iTofChargeH() {
  if(fTofChargeH<0){
     AMSEventR::Head()->TofChargeH().push_back(TofChargeHR(this));
     fTofChargeH=AMSEventR::Head()->TofChargeH().size()-1;
   }
   return fTofChargeH;
}


TofChargeHR *  BetaHR::pTofChargeH(){
  return (AMSEventR::Head())?AMSEventR::Head()->pTofChargeH(iTofChargeH()):0; 
}

int   BetaHR::GetBetaPattern(){
    
   int pattern=0;
   for(int ilay=0;ilay<4;ilay++){
     pattern+=(BetaPar.Pattern[ilay]%10)*int(pow(10.,3-ilay));
   }
   return pattern;
}

int  BetaHR::GetBuildType(){
    
   if     (fTrTrack>=0){
        if((GetStatus()&TOFDBcN::TKTRACK2)==0)return 1;
        else   return 11;
   }
   else if(fTrdTrack>=0){
        if((GetStatus()&TOFDBcN::TRDTRACK2)==0)return 2;
        else return 12; 
   }
   else if(fEcalShower>=0)   return 3;
   else if(BetaPar.SumHit==4)return 4;
   else                      return 5;
}

float BetaHR::GetBetaS(){

   double beta,ebetav;
   BetaReFit(beta,ebetav,-2);
   return float(beta);
}

float BetaHR::GetNormChi2T(){
    
   int nhit=0;
   double time[4],etime[4],len[4];
//----GetQ
   int nlay;float qrms;
   float tfq=GetQ(nlay,qrms,2,(TofRecH::kBirkCor|TofRecH::kQ2Q));
//---
   int ch1=int(tfq)>1?int(tfq):1;
   int ch2=ch1+1;
   double ww1=fabs(tfq-ch1),ww2=fabs(ch2-tfq);
//---
   for(int ilay=0;ilay<4;ilay++){
       if(!TestExistHL(ilay))continue;
       if(GetPattern(ilay)%10!=4&&!(((GetStatus()&(TOFDBcN::TKTRACK2|TOFDBcN::TRDTRACK2))>0)&&(GetPattern(ilay)%10>0)))continue;
       time [nhit]=GetTime(ilay);
       len  [nhit]=GetTkTFLen(ilay);
//---
       TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar);
       if((ch1<=1)||(ch1==tfq)){etime[nhit]=TofRecPar::GetTimeSigma(TofRecPar::Idsoft,ch1);}
       else  etime[nhit]=(ww2*TofRecPar::GetTimeSigma(TofRecPar::Idsoft,ch1)+ww1*TofRecPar::GetTimeSigma(TofRecPar::Idsoft,ch2))/(ww2+ww1);
       if(GetPattern(ilay)%10!=4)etime[nhit]=1.414*etime[nhit];
       nhit++;
    }
   TofBetaPar fitpar;
   TofRecH::BetaFitT(time,etime,len,nhit,fitpar);
   return fitpar.Chi2T;
}

float BetaHR::GetNormChi2C(){

   float chisl=0,ecoo,res; 
   int nhitl=0;
   int nlay;float qrms;
//----GetQ
   float tfq=GetQ(nlay,qrms,2,(TofRecH::kBirkCor|TofRecH::kQ2Q));
//---
   int ch1=int(tfq)>1?int(tfq):1;
   int ch2=ch1+1;
   double ww1=fabs(tfq-ch1),ww2=fabs(ch2-tfq);
//---
   for(int ilay=0;ilay<4;ilay++){
       if(!TestExistHL(ilay))continue;
       if(GetPattern(ilay)%10!=4)continue;
       res=GetCResidual(ilay,TOFGeom::Proj[ilay]);
//--
       TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar);
       if((ch1<=1)||(ch1==tfq))ecoo=TofRecPar::GetCooSigma(TofRecPar::Idsoft,ch1);
       else ecoo=(ww2*TofRecPar::GetCooSigma(TofRecPar::Idsoft,ch1)+ww1*TofRecPar::GetCooSigma(TofRecPar::Idsoft,ch2))/(ww2+ww1);
//---
       chisl+=res*res/(ecoo*ecoo);
       nhitl++;
    }
   return (nhitl==0)?0:chisl/nhitl; 
}



int BetaHR::MassReFit(double &mass, double &emass,double rig,double charge,double erigv,int isbetac,int update){
  TofRecH::betapar=BetaPar;
  TofRecH::MassRec(TofRecH::betapar,rig,charge,erigv,isbetac);
  mass=TofRecH::betapar.Mass;
  emass=TofRecH::betapar.EMass;
  if(update){BetaPar=TofRecH::betapar;}
  return 0;
}

int BetaHR::BetaReFit(double &beta,double &ebetav,int pattern,int mode,int update){

  TofBetaPar betapar;
  int stat=BetaReFit(betapar,pattern,mode,update);
  beta=betapar.Beta; 
  ebetav=betapar.InvErrBeta;
  return stat;
}

int BetaHR::BetaReFit(TofBetaPar &betapar,int pattern,int mode,int update){
  
  vector<int> layc;
  double maxtres=0;int maxltres=-1;
///--Candidate
  for(int ilay=0;ilay<4;ilay++){
     if(!TestExistHL(ilay))continue;
     bool pass1=((pattern/int(pow(10., 3-ilay))%10>=2)&&((GetClusterHL(ilay)->Status&TOFDBcN::RECOVERED)>0));
     bool pass2=((pattern/int(pow(10., 3-ilay))%10==1)&&((GetClusterHL(ilay)->Status&TOFDBcN::BADTIME)==0));
     if(pattern>0){if(!(pass1||pass2))continue;} 
     else if(GetPattern(ilay)%10!=4)continue;
//---Select Good,Using Recursion If Bad Time Beta
     if((pattern<0)&&(fabs(GetBeta())>1.3||fabs(GetBeta())<0.35)){
       TofBetaPar betaparT; int patT=1111-int (pow(10.,3-ilay));//Try To Remove this Layer To Fit
       BetaReFit(betaparT,patT);
       if(fabs(betaparT.Beta)>0.35&&fabs(betaparT.Beta)<1.3){//Find Windows Counter
         maxtres=FLT_MAX ;maxltres=ilay;
         continue;//not put this layer to candidate
       }
     }
//---Try ReFit
     if(fabs(GetTResidual(ilay))>maxtres){maxtres=fabs(GetTResidual(ilay));maxltres=ilay;}
     layc.push_back(ilay);
  }


 
//---Further Selction
  double time[4],etime[4],len[4],chisl=0,cres,ecoo; int nhit=0,layc1[4];
  for(unsigned int il=0;il<layc.size();il++){
     int ilay=layc.at(il);
     if((pattern==-1)&&(ilay==maxltres))continue;//Using TResidual
     else if(pattern==-2){//Optimize Using CResidual
         if(fabs(GetCResidual(ilay,TOFGeom::Proj[ilay]))>15)continue;//15cm out tracker 5sigma
         if(fabs(GetClusterHL(ilay)->Coo[TOFGeom::Proj[ilay]])>TOFGeom::Sci_l[ilay][GetClusterHL(ilay)->Bar]/2.+10)continue;//10cm out TOF
      }
      time [nhit]=GetTime(ilay);
      etime[nhit]=GetETime(ilay);
      len  [nhit]=GetTkTFLen(ilay);
      layc1 [nhit]=ilay;
      cres=fabs(GetCResidual(ilay,TOFGeom::Proj[ilay]));
      ecoo =GetClusterHL(ilay)->ECoo[TOFGeom::Proj[ilay]];
      chisl+=cres*cres/(ecoo*ecoo); 
      nhit++;     
  }

  betapar.Init();
  if(nhit<2)return -1;
  else if((nhit==2)&&(layc1[0]/2==layc1[1]/2)){return -1;}//Same Up||Same Down

//---Then Fill
  betapar=BetaPar;
  TofRecH::BetaFitT(time,etime,len,nhit,betapar,mode); 
  betapar.UseHit=nhit;
  betapar.Chi2C=chisl/nhit;
  if(update){BetaPar=betapar;}
  return 0;
}

double BetaHR::TInterpolate(double zpl,AMSPoint &pnt,AMSDir &dir,double &time,bool usetrtr,int tkopt){

  double path=0;
  pnt.setp(0,0,zpl);
  dir.setp(0,0,1);
#ifdef _PGTRACK_
  if(fTrTrack>=0 && usetrtr && (tkopt/10000%10>0) ){//TrTrack
     path=pTrTrack()->Interpolate(zpl,pnt,dir);
   }
  else if(fTrdTrack>=0 && usetrtr && (tkopt/1000%10>0) ){//TrdTrack
     path=pTrdTrack()->Interpolate(zpl,pnt,dir); 
  } 
  else if( !usetrtr || (tkopt/10%10>0) ){//TofTrack
#endif
///--Find Coo
     double coo[3][4]={{0}},ecoo[3][4]={{0}};//X Y Z 4Layer;
     int nhits=0;
     for(int ilay=0;ilay<4;ilay++){
       if(!TestExistHL(ilay))continue;
       if(GetPattern(ilay)%10!=4)continue;
       coo[0][nhits]= GetClusterHL(ilay)->Coo[0];
       coo[1][nhits]= GetClusterHL(ilay)->Coo[1];
       coo[2][nhits]= GetClusterHL(ilay)->Coo[2];
       ecoo[0][nhits]=GetClusterHL(ilay)->ECoo[0];
       ecoo[1][nhits]=GetClusterHL(ilay)->ECoo[1];
       ecoo[2][nhits]=GetClusterHL(ilay)->ECoo[2];
       nhits++;
     }
///--Space Line Fit x=az+b y=az+b
     if(nhits>=2){
       double ax,bx,ay,by;
       TofRecH::LineFit(nhits,coo[2],coo[0],ecoo[0],ax,bx);
       TofRecH::LineFit(nhits,coo[2],coo[1],ecoo[1],ay,by);
       AMSPoint p1(ax*zpl+bx,ay*zpl+by,zpl);
       AMSPoint p0(ax*0.+bx,ay*0.+by,0.);
       AMSPoint p2(p1-p0);
       AMSDir   d1(p2);
       path=p2.norm();  
       dir=d1; pnt=p1;
       if (zpl == 0) dir = AMSDir(ax, ay, 1);
     }
#ifdef _PGTRACK_
  }
#endif
  path=fabs(path);
  if(zpl>0)path*=-1;
  time =path/GetBeta()/TofRecH::cvel+GetT0();
  return path;
}

float BetaHR::GetEdepLPM(int ilay,int pmtype, int is,int pm){
  if(!TestExistHL(ilay))return 0;
  double q2=pmtype==1?BetaPar.AQ2L[ilay][is]:BetaPar.DQ2L[ilay][is][pm];
  TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar,is,pm);
  float edep=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,(TofRecH::kBirkCor|TofRecH::kQ2MeV),q2);
  return edep;
}


float BetaHR::GetQLPM(int ilay,int pmtype,int is,int pm,int opt){
   if(!TestExistHL(ilay))return 0;
    double q2=pmtype==1?BetaPar.AQ2L[ilay][is]:BetaPar.DQ2L[ilay][is][pm];
    TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar,is,pm);
    float signal=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,opt,q2,0,double(BetaPar.CosZ[ilay]),double(BetaPar.Beta));
    return signal;
}

float BetaHR::GetEdepL(int ilay,int pmtype,int pattern,int optw){
     if(!TestExistHL(ilay))return 0;
     if(pattern==111111&&optw==1){
        if     (pmtype==1)return BetaPar.AEdepL[ilay];//Anode
        else if(pmtype==0)return BetaPar.DEdepL[ilay];//Dynode
        else if((BetaPar.AEdepL[ilay]>0&&BetaPar.AEdepL[ilay]<6*6*TofCAlignPar::ProEdep)||BetaPar.DEdepL[ilay]<=0)return BetaPar.AEdepL[ilay];//Anode Range
        else              return BetaPar.DEdepL[ilay];//Dynode
     }
///---rawqd
    double rawqd[2][TOFCSN::NPMTM]={{0}},rawqa[2]={0};
    for(int is=0;is<TOFCSN::NSIDE;is++){
       int patterns=pattern/int(pow(1000.,is));
       rawqa[is]=(patterns%10==0)?0:BetaPar.AQ2L[ilay][is];
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
          int patternp=patterns/int(pow(10.,ipm));
          rawqd[is][ipm]=(patternp%10==0)?0:BetaPar.DQ2L[ilay][is][ipm];
      }
    }

///---Anode Or Dynode Sum Information+BirkCor
    TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar);
    float qa=TofRecH::SumSignalA(TofRecPar::Idsoft,rawqa,optw);
    float qd=TofRecH::SumSignalD(TofRecPar::Idsoft,rawqd,optw,1);
    float q2;
    if     (pmtype==1)q2=qa;//Anode
    else if(pmtype==0)q2=qd;//Dynode
    else  {
       if((qa>0&&qa<6*6)||qd<=0){q2=qa; pmtype=1;}//Anode  Range
       else                     {q2=qd; pmtype=0;}//Overflow or Q2>6*6 using Dynode
    }
    float signal=TofRecH::GetQSignal(TofRecPar::Idsoft,pmtype,(TofRecH::kBirkCor|TofRecH::kQ2MeV),q2);
    return signal;
}


float BetaHR::GetQL(int ilay,int pmtype,int opt,int pattern,float fbeta,float frig,int optw){
    if(!TestExistHL(ilay))return 0;

//---Get Default
    if((opt==TofClusterHR::DefaultQOpt)&&(fbeta==0)){
       int qpat=pmtype*100000000+pattern*100+optw;
       map<int, float >::iterator it;
       it=BetaPar.QL[ilay].find(qpat);
       if(it!=BetaPar.QL[ilay].end()){return (BetaPar.QL[ilay])[qpat];}
    }

    int npmtype=pmtype;   
///---rawqd
    double rawqd[2][TOFCSN::NPMTM]={{0}},rawqa[2]={0};
    for(int is=0;is<TOFCSN::NSIDE;is++){
       int patterns=pattern/int(pow(1000.,is));
       rawqa[is]=(patterns%10==0)?0:BetaPar.AQ2L[ilay][is];
       for(int ipm=0;ipm<TOFCSN::NPMTM;ipm++){
          int patternp=patterns/int(pow(10.,ipm));
          rawqd[is][ipm]=(patternp%10==0)?0:BetaPar.DQ2L[ilay][is][ipm];
      }
    }

///---Anode Or Dynode Sum Information+Theta// BirkCor+Beta Corr
    TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar);
    float qa=TofRecH::SumSignalA(TofRecPar::Idsoft,rawqa,optw);
    float qd=TofRecH::SumSignalD(TofRecPar::Idsoft,rawqd,optw,1);
    float q2;
    if     (npmtype==1)q2=qa;//Anode
    else if(npmtype==0)q2=qd;//Dynode
    else  {
       if((qa>0&&qa<6*6)||qd<=0){q2=qa; npmtype=1;}//Anode  Range
       else                     {q2=qd; npmtype=0;}//Overflow or Q2>6*6 using Dynode
    }
    double rig=0;
#ifdef _PGTRACK_
    if(fTrTrack>=0&&fbeta==0){rig=pTrTrack()->GetRigidity();}
#endif
    if(frig!=0)rig=frig;
    AMSPoint pnt;AMSDir dir; double time; 
    TInterpolate(GetClusterHL(ilay)->Coo[2],pnt,dir,time);   
 
    double ubeta=(fbeta==0)?BetaPar.Beta:fbeta;
    float signal=0;
    if(pmtype==2&&((opt&TofRecH::kDAWeight)!=0)){
//---Q2Opt
       int optq2=(opt&(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor));
       double qd2=TofRecH::GetQSignal(TofRecPar::Idsoft,0,optq2,double(qd),pnt[TOFGeom::Proj[ilay]],double(BetaPar.CosZ[ilay]));//Dynode
       double qa2=TofRecH::GetQSignal(TofRecPar::Idsoft,1,optq2,double(qa),pnt[TOFGeom::Proj[ilay]],double(BetaPar.CosZ[ilay]));//Anode
//---QOpt
       int optq=(opt&(TofRecH::kBetaCor|TofRecH::kRigidityCor|TofRecH::kQ2Q));
       qd=TofRecH::GetQSignal(TofRecPar::Idsoft,0,optq,qd2,0,1,ubeta,rig);
       qa=TofRecH::GetQSignal(TofRecPar::Idsoft,1,optq,qa2,0,1,ubeta,rig);
//---SUMDA
       double wd=TofRecH::GetWeightDA(TofRecPar::Idsoft,0,qd2);
       double wa=TofRecH::GetWeightDA(TofRecPar::Idsoft,1,qa2);
       if(qd2<0&&qa2<0)signal=-1;
       else            signal=(wa*qa+wd*qd)/(wa+wd); 
     }
     else {
        signal=TofRecH::GetQSignal(TofRecPar::Idsoft,npmtype,opt,double(q2),pnt[TOFGeom::Proj[ilay]],double(BetaPar.CosZ[ilay]),ubeta,rig);
     }
//---Default push_back
    if((opt==TofClusterHR::DefaultQOpt)&&(fbeta==0)){
       int qpat=pmtype*100000000+pattern*100+optw;
       (BetaPar.QL[ilay])[qpat]=signal;
       qpat=npmtype*100000000+pattern*100+optw;
       (BetaPar.QL[ilay])[qpat]=signal;
    }
//---

    return signal;
}


float BetaHR::GetQ(int &nlay,float &qrms,int pmtype,int opt,int pattern,float fbeta,float frig){

//---Get Default
    if((opt==TofClusterHR::DefaultQOpt)&&(fbeta==0)){
       int qpat=pmtype*100000000+pattern*100;
//---
       map<int,pair<int, pair<float,float> > >::iterator it;
       it=BetaPar.QPar.find(qpat);
       if(it!=BetaPar.QPar.end()){
          nlay=(BetaPar.QPar)[qpat].first; 
          qrms=(BetaPar.QPar)[qpat].second.second;
          return (BetaPar.QPar)[qpat].second.first;
       }
   }
//---

   vector<float >ql;
   vector<bool >qg;//Geometry Check
   float qs;
   int qbn=0;//Q-Bad Number
//----Fill Vector
   for(int ilay=0;ilay<4;ilay++){
      if(!TestExistHL(ilay))continue;
      qs=GetQL(ilay,pmtype,opt,111111,fbeta,frig);
      if(qs<=0)continue;
      if((pattern>0)&&((pattern/int(pow(10.,3-ilay)))%10==0))continue;
      ql.push_back(qs);
//-----Good Path Length Geometry Check
      AMSPoint pnt;AMSDir dir; double time;
      TInterpolate(GetClusterHL(ilay)->Coo[2],pnt,dir,time);
      bool qgl=TOFGeom::IsGoodQGeom(ilay,GetClusterHL(ilay)->Bar,pnt);
      qg.push_back(qgl);
      if(!qgl){qbn++;}
 //----
   }

//---Pool PathLength ReMove Candiadte  For Default-Q
     if(pattern<0&&ql.size()>2&&qbn>0&&fTrTrack>=0&&(pattern>=-10)){
//----
         for(unsigned int i=0; i<qg.size();i++){
            if(qg.at(i))continue;//Good Keep
            ql.erase(ql.begin()+i);
            qg.erase(qg.begin()+i);
            i--;//Go To Again
            if(ql.size()<=2)break;
          }
//-----
      }


//-----GetMean
    float mean=0,sig=0,qmax=0,qmean=0,qmin=99999999;
    for(unsigned int i=0; i<ql.size();i++){
       mean+=ql.at(i); sig+=ql.at(i)*ql.at(i);
       if(ql.at(i)>qmax){qmax=ql.at(i);}
       if(ql.at(i)<qmin){qmin=ql.at(i);}
    }

//----Fill Var
    if(ql.size()<=2||pattern>0||pattern==-10){
       nlay=ql.size();
       if(nlay==0){qrms=0; qmean=0;}
       else       {
         mean=mean/ql.size();sig=sig/ql.size();
         sig=sqrt(fabs(sig-mean*mean));
         qrms=sig;
         qmean=mean;
       }
     }
    else {
       
       float meanl=(mean-qmax)/(ql.size()-1); float sigl= (sig-qmax*qmax)/(ql.size()-1);
       sigl=sqrt(fabs(sigl-meanl*meanl)); 
       float dqh=fabs(qmax-meanl)/sigl;
//----
       float meanh=(mean-qmin)/(ql.size()-1); float sigh= (sig-qmin*qmin)/(ql.size()-1);
       sigh=sqrt(fabs(sigh-meanh*meanh));
       float dql=fabs(qmin-meanh)/sigh;
//----
       nlay=ql.size()-1;
//-----        
       if(pattern%10==-2){qrms=sigl;qmean=meanl;}
       else           {
         if(dqh>dql)  {qrms=sigl;qmean=meanl;}
         else         {qrms=sigh;qmean=meanh;}
       }
    }

//--Default
      if((opt==TofClusterHR::DefaultQOpt)&&(fbeta==0)){
         int qpat=pmtype*100000000+pattern*100;
         pair<float,float>qp(qmean,qrms);
         (BetaPar.QPar)[qpat]=make_pair(nlay,qp);
      }
//---
     return qmean; 

}

float BetaHR::GetQBetaL(int ilay,int charge,int pmtype){

   if(!TestExistHL(ilay))return 0;

   double q2=GetQL(ilay,pmtype,(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor));
   TofRecPar::IdCovert(ilay,GetClusterHL(ilay)->Bar);
   TofRecH::Init();
   float qbeta=TofRecH::GetBetaCalCh(TofRecPar::Idsoft,1,BetaPar.Beta,q2,charge,0.);

   return qbeta;
}

bool  BetaHR::IsGoodQPathL(int ilay){
  
    if(!TestExistHL(ilay)||fTrTrack<0)return 0;  

     AMSPoint pnt;AMSDir dir; double time;
     TInterpolate(GetClusterHL(ilay)->Coo[2],pnt,dir,time);
     bool qgl=TOFGeom::IsGoodQGeom(ilay,GetClusterHL(ilay)->Bar,pnt);

     return qgl;
}

//---end of BetaH

BetaR* ChargeR::pBeta(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
}

BetaHR* ChargeR::pBetaH(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBetaH(fBetaH):0;
}

RichRingR* ParticleR::pRichRing(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pRichRing(fRichRing):0;
}

RichRingBR* ParticleR::pRichRingB(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pRichRingB(fRichRingB):0;
}

BetaR* ParticleR::pBeta(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
}

BetaHR* ParticleR::pBetaH(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBetaH(fBetaH):0;
}

ChargeR* ParticleR::pCharge(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pCharge(fCharge):0;
}

#ifndef _PGTRACK_
TrTrackR* VertexR::pTrTrack(unsigned int i){
  return (AMSEventR::Head() && i<fTrTrack.size())?AMSEventR::Head()->pTrTrack(fTrTrack[i]):0;
}
#endif

TrTrackR* ParticleR::pTrTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrTrack(fTrTrack):0;
}

TrdTrackR* ParticleR::pTrdTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrdTrack(fTrdTrack):0;
}

TrdHTrackR* ParticleR::pTrdHTrack(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pTrdHTrack(fTrdHTrack):0;
}

EcalShowerR* ParticleR::pEcalShower(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pEcalShower(fEcalShower):0;
}


VertexR* ParticleR::pVertex(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pVertex(fVertex):0;
}
#ifdef __LZMA__
#include "Compression.h"
#endif
void AMSEventR::CreateBranch(TTree *tree, int branchSplit){
  if(tree){
    _Head=this;
    tree->Branch(BranchName(),"AMSEventR",&_Head,128000,branchSplit);
    TBranch * branch=tree->GetBranch(BranchName());
    int clevel=branch->GetCompressionLevel();
#ifdef __LZMA__
    if(clevel<6 && branch->GetCompressionAlgorithm()!=ROOT::kLZMA)clevel=6;
#else
    if(clevel<6)clevel=6;
#endif
branch->SetCompressionLevel(clevel);

    cout <<" CompressionLevel "<<branch->GetCompressionLevel()<<" "<<branch->GetSplitLevel()<<endl;
    tree->SetBranchStatus("ev.TSelector",false);
    //     tree->SetBranchStatus("ev.fService",false);
  }
}

void AMSEventR::SlaveBegin(TTree *tree){
  Begin(tree);
}
void AMSEventR::SlaveTerminate(){
  Terminate();
}
#ifdef _PGTRACK_
#include "tkdcards.h"
#endif
void AMSEventR::Begin(TTree *tree){
  // Function called before starting the event loop.
  // Initialize the tree branches.
  Init(tree);
  TString option = GetOption();
  // open file if...
  gWDir=gSystem->WorkingDirectory();
        cout <<"AMSEventR::Begin-I-WorkingDirWas "<<gWDir<<endl;

#pragma omp master
  if(!pService){
    (fService)._w.Start();
    _NFiles=-_NFiles;
    for(int thr=fgThickMemory?fgThreads-1:0;thr>=0;thr--){
      if(thr==0){
	if(option.Length()>1){
        if(strstr((const char*)gWDir,".proof") &&strstr((const char*)gWDir,"worker-0."  )){
          string fdir((const char*)gWDir);
          int pos=fdir.find("worker-0");
          string fupdir=fdir.substr(0,pos);
          string fname(option.Data());
          if(fname.find('/')==string::npos){
            string ffull=fupdir+fname;
            ffull+=fdir.substr(pos+8,fdir.length()-pos-7);
            option=ffull.c_str();
           }
          } 


	  (fService)._pOut=new TFile(option,"RECREATE");
	  (fService)._pDir=gDirectory;
          _pFiles = new TTree("FILES","FILES");
          _pFiles->Branch("file",(void*)filename,"file/C",1024);  
           
//	      cout <<" gdir "<<gDirectory<<" "<< " "<<gROOT<<" "<<gDirectory->GetFile()->GetName()<<endl;
	  cout <<"AMSEventR::Begin-I-WriteFileOpened "<<option<< endl;
	}
      }
      char dir[1024];
      sprintf(dir,"thread_%d",thr);
      if(fgThickMemory)Dir=dir;
      UBegin();
#ifdef _PGTRACK_
         TRFITFFKEY.init();
#endif
      pService=&fService; 
    }
  }
  while(!pService){
    usleep(1);
  }
  char dir[1024];
  int thr=0;
#ifdef _OPENMP
  if(fgThickMemory)thr=omp_get_thread_num();
#endif
  sprintf(dir,"thread_%d",thr);
  //strcpy(Dir,dir);
  if(fgThickMemory)Dir=dir;
}


void AMSEventR::Init(TTree *tree){
  //   Set branch addresses
  if (tree == 0) return;
  _Tree    = tree;
  //_Tree->SetMakeClass(1);
  //SetBranchA(_Tree);
  Head() = this;
  _Tree->SetBranchAddress("ev.",&Head());
   
}

void AMSEventR::ProcessFill(int entry){ 
  if (!UProcessCut()) return; 
  UProcessFill();
}
 


void AMSEventR::Terminate()
{
string frej=filename;
#pragma omp critical (cls)
  {
    if(--_NFiles==0 && pService){
      // Function called at the end of the event loop.
      //_Tree->SetMakeClass(0);
      if(pService){
      ofstream ofbrej;
      int nrej=0;
//      cout<<" filename "<<strlen(filename)<<" "<<filename<<(*pService).fProcessed.size()<<endl;
      for(unsigned int k=0;k<fRequested.size();k++){
       bool found=false;
       for(unsigned int l=0;l<(*pService).fProcessed.size();l++){
          if(strstr(fRequested[k].c_str(),(*pService).fProcessed[l].c_str())){
              found=true;
              break;
          }
       }
       if(!found){
          cerr<<"AMSEventR::Terminate-W-FileWasRequestedButNotProcessed "<<fRequested[k]<<" "<<k<<endl;
          int  ifound=fRequested[k].find("?svcClass=");
          string aname=fRequested[k];
         if(ifound>=0){
          aname=fRequested[k].substr(0,ifound);
        }
      if(nrej==0 && strlen(frej.c_str())>1){
          ofbrej.clear();
          ofbrej.open(frej.c_str());
      }
          if(ofbrej)ofbrej<<aname<<endl;
          else cerr<<"AMSEventR::Terminate-E-UnableTopenRejFile "<<frej<<" "<<nrej<<endl;
          nrej++;
     }
     else{
          if(fRequested[k].size()<sizeof(filename)/sizeof(filename[0]))strcpy(filename,fRequested[k].c_str());
          else strncpy(filename,fRequested[k].c_str(),sizeof(filename)/sizeof(filename[0])-1);
          if(_pFiles){
             _pFiles->Fill();
             cout<<"AMSEventR::Terminate-I-FileAddedtoFILESBranch"<<filename<<endl;
          }  
          else  cerr<<"  AMSEventR::Terminate-E-PFilesProblem "<<     filename<<endl;    
     }
    }
      if(ofbrej){
        ofbrej.close();
      }
           
    }
      UTerminate();
      (*pService)._w.Stop();
      if(fgThickMemory)hjoin();
      for(unsigned int k=0;k<sizeof(_ClonedTree)/sizeof(_ClonedTree[0]);k++)_ClonedTree[k]=0;
      for(unsigned int k=0;k<sizeof(_ClonedTreeSetup)/sizeof(_ClonedTreeSetup[0]);k++)_ClonedTreeSetup[k]=0;
      cout <<"AMSEventR::Terminate-I-CputimeSpent "<<(*pService)._w.CpuTime()<<" sec"<<endl;
      cout <<"AMSEventR::Terminate-I-Total/Bad "<<(*pService).TotalEv<<"/"<<(*pService).BadEv<<" events processed "<<endl;
      cout <<"AMSEventR::Terminate-I-ApproxTotal of "<<(*pService).TotalTrig<<" triggers processed "<<endl;
      if((*pService)._pOut){
	(*pService)._pDir->cd(); 
	(*pService)._pOut->Write();
	cout <<"AMSEventR::Terminate-I-WritedFile "<<GetOption()<<endl;
      for(unsigned int k=0;k<sizeof(fgOutSep)/sizeof(fgOutSep[0]);k++){
	if(fgOutSep[k]){
         fgOutSep[k]->Write();
         fgOutSep[k]->Close();
 	cout <<"AMSEventR::Terminate-I-ClosedFile "<<fgOutSep[k]->GetName()<<endl;
         delete fgOutSep[k];
         fgOutSep[k]=0;
      }        
      }
          hdelete(0);
	 (*pService)._pOut->Close();
	cout <<"AMSEventR::Terminate-I-ClosedFile "<<(*pService)._pOut->GetName()<<endl;
         cout <<"AMSEventR::Terminate-I-WorkingDirWas "<<gSystem->WorkingDirectory()<<endl;
	if(!(*pService).TotalEv   &&   !(*pService).TotalTrig){
       	    cout <<" AMSEventR::Terminate-E-ProofLiteOutputFileDeleted  "<< (*pService)._pOut->GetName()<<endl;
	  unlink(  (*pService)._pOut->GetName());
	}    
	else if(strstr((const char*)gWDir,".proof") &&strstr((const char*)gWDir,"worker-0."  )){
	  string fdir((const char*)gWDir);
	  int pos=fdir.find("worker-0");
	  string fupdir=fdir.substr(0,pos);
	  string fname((*pService)._pOut->GetName());
	  if(fname.find('/')==string::npos){
	    string ffull=fupdir+fname;
            string fsystem="mv "+fdir+"/" +fname+" ";
	    ffull+=fdir.substr(pos+8,fdir.length()-pos-7);
	    fsystem+=ffull;
	    system(fsystem.c_str());
	    cout <<" AMSEventR::Terminate-I-ProofLiteOutputFileMovedTo  "<<fsystem<<endl;
	  }
          else{
	    cout <<" AMSEventR::Terminate-E-ProofLiteProblemToMoveOutputfile  "<<fname<<endl;
          }
	}


  
         delete (*pService)._pOut;
	(*pService)._pOut=0;
	//for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
	//delete i->second;
	//}
	Service::hb1.clear();
	//for( Service::hb2i i=Service::hb2.begin();i!=Service::hb2.end();i++){
	//delete i->second;
	//}
	Service::hb2.clear();
	//for( Service::hbpi i=Service::hbp.begin();i!=Service::hbp.end();i++){
	//delete i->second;
	//}
	Service::hbp.clear();
        _NFiles=-1;
        pService=0;
	cout <<"AMSEventR::Terminate-I-WriteFileClosed "<<GetOption()<<endl;
      }
    }
    else {
     cerr<<"AMSEventR::Terminate-W-_NFilesNotZero "<<_NFiles<<endl;
     if(_NFiles ==0){
      ofstream ofbrej;
      int nrej=0;
      for(unsigned int k=0;k<fRequested.size();k++){
          cerr<<"AMSEventR::Terminate-W-FileWasRequestedButNotProcessed "<<fRequested[k]<<endl;
          int  ifound=fRequested[k].find("?svcClass=");
          string aname=fRequested[k];
         if(ifound>=0){
          aname=fRequested[k].substr(0,ifound);
        }
      if(nrej==0 && strlen(frej.c_str())>1){
          ofbrej.clear();
          ofbrej.open(frej.c_str());
      }
          if(ofbrej)ofbrej<<aname<<endl;
          else cerr<<"AMSEventR::Terminate-E-UnableTopenRejFile "<<frej<<" "<<nrej<<endl;
          nrej++;
     }
    
      if(ofbrej){
        ofbrej.close();
      }
           
    }
}

     }
    
}

Int_t AMSEventR::Fill()
{
  int i;
#pragma omp critical 
  {
    int thr=0;
#ifdef _OPENMP
    if(fgSeparateOutputFile)thr=omp_get_thread_num();
#endif
   
    if (_ClonedTree[thr]==NULL) {
      TFile * input=_Tree->GetCurrentFile();
      if(!input){cerr<<"AMSEventR::Fill-E-annot find input file "<<endl;}
      const int nobjs=8;
      char objlist[nobjs][40]={"TkDBc","TrCalDB","TrParDB","TrGainDB","TrReconPar","TrExtAlignDB","TrInnerDzDB","TrOccDB"};
      TObject* obj[nobjs]={0,0,0,0,0,0,0,0}; 
      TObjString* obj2=0;
      TObjString* obj3=0;
      if(!input){cerr<<"AMSEventR::Fill-E-cannot find input file "<<endl;}
      else{
	cout <<input->GetName()<<endl;
	for(int ii=(nobjs-1);ii>=0;ii--){
	  obj[ii]=input->Get(objlist[ii]);
	}
	obj2=(TObjString*)input->Get("AMS02Geometry");
	obj3=(TObjString*)input->Get("DataCards");
      }
      _ClonedTree[thr] = _Tree->GetTree()->CloneTree(0);
      //    cout <<"  cloned treeess "<<_ClonedTree<<endl;
      TDirectory *gdir=gDirectory;
      //  cout <<" ************8thr "<<thr<<" "<<fgOutSepDir[thr]<<endl;
      if(thr!=0){
	if(!fgOutSepDir[thr] ){
	  char dir[1024];
	  sprintf(dir,"_%d",thr);
	  if(fgSeparateOutputFile==2){
	    string fname(GetOption());
	    fname+=dir;
	    TFile *f=new TFile(fname.c_str(),"RECREATE");
	    if(!f){
	      cerr<<"AMSEventR::Fill-E-UnableToOpenFile "<<fname<<endl;
	      thr=0;
	    }  
	    else{
	      fgOutSep[thr]=f;
	      AMSEventR::_ClonedTree[thr]->SetDirectory(gDirectory);
	      fgOutSepDir[thr]=gDirectory;
	    }
	  }
	  else{
	    gDirectory=AMSEventR::OFD();
	    if(gDirectory){
	      gDirectory=gDirectory->mkdir(dir);
	      AMSEventR::_ClonedTree[thr]->SetDirectory(gDirectory);
	      fgOutSepDir[thr]=gDirectory;
	      //      cout <<" gdirectory "<<gDirectory->GetName()<<endl;
	    }
	  }
	}
	else{
	  AMSEventR::_ClonedTree[thr]->SetDirectory(fgOutSepDir[thr]);
	  gDirectory=fgOutSepDir[thr];
	}
      }
      if(thr==0){
	AMSEventR::_ClonedTree[thr]->SetDirectory(AMSEventR::OFD());
	gDirectory=AMSEventR::OFD();
      }
      
      cout <<" ofd "<<AMSEventR::OFD()<<endl;
      cout <<" gdir "<<gDirectory->GetFile()<<endl;
      //cout <<" obj2 "<<obj2<<" "<<(void*)obj3<<" "<<AMSEventR::OFD()->GetFile()->GetName()<<" "<<gDirectory->GetFile()->GetName()<<endl;
      if(obj2)obj2->Write("AMS02Geometry");
      if(obj3)obj3->Write("DataCards");
      for(int i=0;i<nobjs;i++)if(obj[i]){cout<<" write "<<objlist[i]<<endl;obj[i]->Write();};
      gDirectory=gdir;
      //    cout <<"  hopa "<<_ClonedTree<<" "<<_ClonedTree->GetCurrentFile()<<endl;
      //    cout <<"2nd "<< _ClonedTree->GetCurrentFile()->GetName()<<endl;
    }
    //    cout <<"  cloned treeess "<<_ClonedTree<<endl;
    
    {
      _ClonedTree[thr]->SetBranchAddress(_Name,&Head());
      i= _ClonedTree[thr]->Fill();
      //      cout <<"  i "<<i<<" thr "<<thr<<endl;
    }
    
    
    // save root setup here
    TDirectory *gdir=gDirectory;
    if (_ClonedTreeSetup[thr]==NULL) {
      if(_TreeSetup){
      }
      else{
	TFile * input=_Tree->GetCurrentFile();
	if(input){
	  _TreeSetup=dynamic_cast<TTree*>(input->Get("AMSRootSetup"));
	}
      }
      if(_TreeSetup){
	_ClonedTreeSetup[thr] = _TreeSetup->GetTree()->CloneTree(0);
	if(thr!=0){
	  if(!fgOutSepDir[thr]){
	    char dir[1024];
	    sprintf(dir,"_%d",thr);
	    if(fgSeparateOutputFile==2){
	      string fname(GetOption());
	      TFile *f=new TFile(fname.c_str(),"RECREATE");
	      if(!f){
		cerr<<"AMSEventR::Fill-E-UnableToOpenFile "<<fname<<endl;
		thr=0;
	      }  
	      else{
		fgOutSep[thr]=f;
		AMSEventR::_ClonedTreeSetup[thr]->SetDirectory(gDirectory);
		fgOutSepDir[thr]=gDirectory;
	      }
	    }
	    else{
	      gDirectory=AMSEventR::OFD();
	      if(!gDirectory->cd(dir)){
		gDirectory=gDirectory->mkdir(dir);
		AMSEventR::_ClonedTree[thr]->SetDirectory(gDirectory);
		fgOutSepDir[thr]=gDirectory;
		//      cout <<" gdirectory2 "<<gDirectory->GetName()<<endl;
	      }
	      else{
		//      cout <<" gdirectory3 "<<gDirectory->GetName()<<endl;
		AMSEventR::_ClonedTree[thr]->SetDirectory(gDirectory);
	      } 
 
	    }
	  }
	  else{
	    AMSEventR::_ClonedTreeSetup[thr]->SetDirectory(fgOutSepDir[thr]);
	    gDirectory=fgOutSepDir[thr];
	  }
	}
	if(thr==0){
	  AMSEventR::_ClonedTreeSetup[thr]->SetDirectory(AMSEventR::OFD());
	}
      }
    }
    gDirectory=gdir;
      
    if(_ClonedTreeSetup[thr] && _EntrySetup!=-1){
      _ClonedTreeSetup[thr]->SetBranchAddress("run.",&AMSSetupR::gethead());
      i= _ClonedTreeSetup[thr]->Fill();
      cout <<" Setup written for run "<<AMSSetupR::gethead()->fHeader.Run<<" "<<i<<endl;
      _EntrySetup=-1;
    }
  }
    

  
  return i;
}

void AMSEventR::UBegin(){
}
void AMSEventR::UProcessFill(){
}
void AMSEventR::UTerminate(){
}



void AMSEventR::GetAllContents() {
  clear();
  bStatus->GetEntry(_Entry);
  bHeader->GetEntry(_Entry);
   UpdateGPS();
  bEcalHit->GetEntry(_Entry);
  bEcalCluster->GetEntry(_Entry);
  bEcal2DCluster->GetEntry(_Entry);
  bEcalShower->GetEntry(_Entry);
  bRichHit->GetEntry(_Entry);
  bRichRing->GetEntry(_Entry);
  if(bRichRingB)bRichRingB->GetEntry(_Entry);
  bTofRawCluster->GetEntry(_Entry);
  NTofRawSide();
  bTofCluster->GetEntry(_Entry);
  if(bTofClusterH)bTofClusterH->GetEntry(_Entry);
  NAntiRawSide();
  bAntiCluster->GetEntry(_Entry);
  bTrRawCluster->GetEntry(_Entry);
  bTrCluster->GetEntry(_Entry);
  bTrRecHit->GetEntry(_Entry);
  bTrTrack->GetEntry(_Entry);
  bTrdRawHit->GetEntry(_Entry);
  bTrdCluster->GetEntry(_Entry);
  bTrdSegment->GetEntry(_Entry);
  bTrdTrack->GetEntry(_Entry);
  bTrdHSegment->GetEntry(_Entry);
  bTrdHTrack->GetEntry(_Entry);
  bLevel1->GetEntry(_Entry);
  bLevel3->GetEntry(_Entry);
  bBeta->GetEntry(_Entry);
  bBetaB->GetEntry(_Entry);
  if(bBetaH) bBetaH->GetEntry(_Entry);
  if(bEcalH) bEcalH->GetEntry(_Entry);
  bVertex->GetEntry(_Entry);
  bCharge->GetEntry(_Entry);
  bParticle->GetEntry(_Entry);
  bAntiMCCluster->GetEntry(_Entry);
  bTrMCCluster->GetEntry(_Entry);
  bTofMCCluster->GetEntry(_Entry);
  if(bTofMCPmtHit)bTofMCPmtHit->GetEntry(_Entry);
  bEcalMCHit->GetEntry(_Entry);
  bTrdMCCluster->GetEntry(_Entry);
  bRichMCCluster->GetEntry(_Entry);
  bMCTrack->GetEntry(_Entry);
  bMCEventg->GetEntry(_Entry);
  bDaqEvent->GetEntry(_Entry);
}


AMSEventR::AMSEventR(const AMSEventR &o):TSelector(),fStatus(o.fStatus),fHeader(o.fHeader),
fEcalHit(o.fEcalHit),fEcalCluster(o.fEcalCluster),fEcal2DCluster(o.fEcal2DCluster),
fEcalShower(o.fEcalShower),fEcalH(o.fEcalH),fRichHit(o.fRichHit),fRichRing(o.fRichRing),fRichRingB(o.fRichRingB),
fTofRawCluster(o.fTofRawCluster),fTofRawSide(o.fTofRawSide),fTofCluster(o.fTofCluster),fTofClusterH(o.fTofClusterH),
fAntiRawSide(o.fAntiRawSide),fAntiCluster(o.fAntiCluster),fTrRawCluster(o.fTrRawCluster),
fTrCluster(o.fTrCluster),fTrRecHit(o.fTrRecHit),fTrTrack(o.fTrTrack),fTrdRawHit(o.fTrdRawHit),
fTrdCluster(o.fTrdCluster),fTrdSegment(o.fTrdSegment),fTrdTrack(o.fTrdTrack),fTrdHSegment(o.fTrdHSegment),
fTrdHTrack(o.fTrdHTrack),fLevel1(o.fLevel1),fLevel3(o.fLevel3),fBeta(o.fBeta),fBetaB(o.fBetaB),fBetaH(o.fBetaH),fTofChargeH(o.fTofChargeH),fCharge(o.fCharge),
fVertex(o.fVertex),fParticle(o.fParticle),fAntiMCCluster(o.fAntiMCCluster),fTrMCCluster(o.fTrMCCluster),
fTofMCCluster(o.fTofMCCluster),fTofMCPmtHit(o.fTofMCPmtHit),fEcalMCHit(o.fEcalMCHit),fTrdMCCluster(o.fTrdMCCluster),
fRichMCCluster(o.fRichMCCluster),fMCTrack(o.fMCTrack),fMCEventg(o.fMCEventg),fDaqEvent(o.fDaqEvent),fAux(o.fAux)
{}
#endif

long long AMSEventR::Size(){
  long long size=sizeof(fHeader)+sizeof(fStatus)+sizeof(TSelector);
  size+=sizeof(EcalHitR)*fEcalHit.size();
  size+=sizeof(EcalClusterR)*fEcalCluster.size();
  size+=sizeof(Ecal2DClusterR)*fEcal2DCluster.size();
  size+=sizeof(EcalShowerR)*fEcalHit.size();
  size+=sizeof(EcalShowerR)*fEcalHit.size();
  size+=sizeof(RichHitR)*fRichHit.size();
  size+=sizeof(RichRingR)*fRichRing.size();
  size+=sizeof(RichRingBR)*fRichRingB.size();
  size+=sizeof(TofRawClusterR)*fTofRawCluster.size();
  size+=sizeof(TofRawSideR)*fTofRawSide.size();
  size+=sizeof(TofClusterR)*fTofCluster.size();
  size+=sizeof(TofClusterHR)*fTofClusterH.size();
  size+=sizeof(AntiRawSideR)*fAntiRawSide.size();
  size+=sizeof(AntiClusterR)*fAntiCluster.size();
  size+=sizeof(TrRawClusterR)*fTrRawCluster.size();
  size+=sizeof(TrClusterR)*fTrCluster.size();
  size+=sizeof(TrRecHitR)*fTrRecHit.size();
  size+=sizeof(TrTrackR)*fTrTrack.size();
  size+=sizeof(TrdRawHitR)*fTrdRawHit.size();
  size+=sizeof(TrdClusterR)*fTrdCluster.size();
  size+=sizeof(TrdSegmentR)*fTrdSegment.size();
  size+=sizeof(TrdTrackR)*fTrdTrack.size();
  size+=sizeof(TrdHSegmentR)*fTrdHSegment.size();
  size+=sizeof(TrdHTrackR)*fTrdHTrack.size();
  size+=sizeof(Level1R)*fLevel1.size();
  size+=sizeof(Level3R)*fLevel3.size();
  size+=sizeof(BetaR)*fBeta.size();
  size+=sizeof(BetaR)*fBetaB.size();
  size+=sizeof(BetaHR)*fBetaH.size();
  size+=sizeof(EcalHR)*fEcalH.size();
  size+=sizeof(TofChargeHR)*fTofChargeH.size();
  size+=sizeof(ChargeR)*fCharge.size();
  size+=sizeof(VertexR)*fVertex.size();
  size+=sizeof(ParticleR)*fParticle.size();
  size+=sizeof(DaqEventR)*fDaqEvent.size();
  return size;
}

//----------------------------------------------------------------------
int AMSEventR::isInShadow(AMSPoint&  ic,int ipart){
        if(ipart>=nParticle()){
                cerr<<"AMSEventR::isInShadow-E-NiSuchParticle "<<ipart<<endl;
                return -1;
        }


        ParticleR part=Particle(ipart);
        return isInShadow(ic,part);
}
int AMSEventR::isInShadow(AMSPoint&  ic,ParticleR & part){
// Says if particle ipart is in shadow and returns also  AMSPoints of interception of particles 
// with iss solar panel rotation plane (FLT_MAX if directed away);

        double deg2rad =3.141592654/180.;

        ic=0;
        //cout<<"\n----------------------New Event --------------------"<<endl;


        if(part.iBeta()>=0){
          BetaR &beta=Beta(part.iBeta());
          if(beta.Pattern<5 && beta.Beta<0)return -2;
        }

        // AMS Coo central point in SSACS [cm]
        double x_ams=-85.73;
        double y_ams=2136.4;
        double z_ams=-388.29;
        AMSPoint AMScoo( x_ams  ,y_ams , z_ams );

        // Solar array centers in cm (at alpha=0)
        // Solar Array 1A
        double x_1A=1982.51;
        double y_1A=3417.32 ;
        double z_1A=-23.77;
        // Solar Array 3A
        double x_3A=-1982.51;
        double y_3A=3418.02 ;
        double z_3A=110.11;

        AMSPoint s1A(x_1A ,y_1A ,z_1A);
        AMSPoint s3A(x_3A ,y_3A ,z_3A);

        //Dimensions: ...inchs
        double dx=1306. ;
       double dy=0.5 ;
        double dz=453.9 ;
        //.....cm
        dx*=2.54;
        dy*=2.54;
        dz*=2.54;
        //......half dim:
        dx/=2.;
        dy/=2.;
        dz/=2.;

//-------if alpha==0 && beta(1or3)==0 pannels are in this config:
//
//                     z+
//      s1-------s2                s1--------s2           
//x+    |    1A   |    y(out)      |    3A   |        x-
//      s3-------s4                s3--------s4
//
//                     z-
//---------------------------------------------------------------

        // AMS tilt angle 
        double tilt=12.*deg2rad;

        // add some error due to the mscattering
 	//... rigidity == momentum
        double Momentum = part.Momentum;
        //..... important only for low momentum particles:
        double addon=13.6e-3/fabs(Momentum)*part.Charge*sqrt(0.35)*5/sqrt(2.);


        //Direction of incident particle in AMS coo
        AMSDir dir(part.Theta,part.Phi);
        if(dir[2]>0){
         dir[0]=-dir[0];
         dir[1]=-dir[1];
         dir[2]=-dir[2];
        }                 
        dir[0]=-dir[0];
        dir[1]=-dir[1];
        dir[2]=-dir[2];

        //Direction of incident particle in SSACS coo
         AMSDir dv0;//..........1) rotation of titl angle
         AMSDir dv; //..........2) axis inversion (x'-> -y, y'->-x, z'->-z)


        //1) rotation  around y_iss in SSACS 
        dv0[0]= dir[0]*cos(tilt)+ dir[2]*sin(tilt);//  |  cos   0   sin   |   |x|           
        dv0[1]= dir[1];                            //= |   0    1    0    | * |y|
        dv0[2]=-dir[0]*sin(tilt)+ dir[2]*cos(tilt);//  | -sin   0   cos   |   |z|_particle
        //2) inversion
        dv[0]=-dv0[1];
        dv[1]=-dv0[0];
        dv[2]=-dv0[2];

        //................................Incident Particle
        //.....incident point of particle in AMS Coo
        AMSPoint coo0(part.Coo);

        AMSPoint coo1;//1) rotation of tilt angle
        AMSPoint coo;// 2) axis inversion + AMScoo ---> Coo in SSACS

        //1)
        coo1[0]= coo0[0]*cos(tilt)+ coo0[2]*sin(tilt);//  |  cos   0   sin   |   |x|           
        coo1[1]= coo0[1];                             //= |   0    1    0    | * |y|
        coo1[2]=-coo0[0]*sin(tilt)+ coo0[2]*cos(tilt);//  | -sin   0   cos   |   |z|_particle
        //2)
        coo[0]=-coo1[1] + AMScoo[0];
        coo[1]=-coo1[0] + AMScoo[1];
        coo[2]=-coo1[2] + AMScoo[2];


        //.........................SET angles ................................

        float alpha,beta1,beta3,  b1b,b3b;
        float aa,ab ,ba1,bb1, ba3,bb3;
        int s1=fHeader.getISSSA(aa,ba1,ba3,b1b,b3b,-5);
        int s2=fHeader.getISSSA(ab,bb1,bb3,b1b,b3b,5);
        int s3=fHeader.getISSSA(alpha,beta1,beta3,b1b,b3b);
        //....rad
        alpha*=deg2rad;
        aa*=deg2rad;
        ab*=deg2rad;
        ba1*=deg2rad;
        bb1*=deg2rad;
        ba3*=deg2rad;
        bb3*=deg2rad;

        beta1*=deg2rad;
        beta3*=deg2rad;

        //.......some error on angles
        double da = fabs(ab-aa)/2.;
        double db1 = fabs(ba1-bb1)/2.;
        double db3 = fabs(ba3-bb3)/2.;


     if(s3==2 || s1==2 || s2==2){
        //.....................if no values only d-alpha 
        //mean variation of alpha in 5 seconds during one orbit:
        da=360.*deg2rad/(90.*60)*5;
        db1=0;
        db3=0;
        }



        //.................Check intersection of particle direction and solar array plane.....................


        // Three points to define each plane
        double P[2][3][3];
        double xyz0[2][3][3];//zero condition (alpha=0 & beta=0)
        double xyzRb[2][3][3];//rotation of beta
        double xyzRba[2][3][3];//rotation of alpha
        double xyzC[2][3];//central poit of solar array

        //this parameter sets the rotation of beta (3A == clockwise; 1A==anticlockwise)
        double s=0;


        //..coefficient for intersections:
        double A[2];
        double B[2];
        double C[2];
        double D[2];
        //.................. r == distance of intersection from each plane:
        //coordinates of intersection in SSACS:
        AMSPoint Int[2];
        // x=r*cos*(dirX) + x0
        // y=r*cos*(dirY) + y0
        // z=r*cos*(dirZ) + z0
        //(x0,y0,z0)---> coortinates of incident point 
        double r[2];
      //......................uncertainties:
        double db=0; //in beta --> to be defined after
        double dt = 2*deg2rad; // 2 deg for tilt 12+/-1 [deg];
        double dr[2]; //distance due to multiple scattering;


         //...back to original position: 
        AMSPoint Int1[2];
        AMSPoint Int0[2];//...in SSACS  


        //...If there is intersection tell which Solar Array (wh==0 --> 1A ; wh==1 --> 3A; wh==-1 No intersection)
        int wh=-1;
        //..................sign of r if ==0 (>0) if ==1 (<0)

        //.......................................begin for k==0 1A ; k==1 3A 
        for(int k=0; k<2 ; k++){

        //...................................Plane construction
               if(k==0){//1A
                s=1.;//anticlockwise beta rotation
                xyzC[k][0]=s1A[0];
                xyzC[k][1]=s1A[1];
                xyzC[k][2]=s1A[2];
                db=db1;
                }

                if(k==1){//3A
                s=-1.;//clockwise beta rotation
                xyzC[k][0]=s3A[0];
                xyzC[k][1]=s3A[1];
                xyzC[k][2]=s3A[2];
                db=db3;
                }

        //We need 3 points to define the plane in local coo: 
         xyz0[k][0][0]= s*dx ;
         xyz0[k][0][1]= 0.  ;
         xyz0[k][0][2]= dz ;

         xyz0[k][1][0]= s*dx ;
        xyz0[k][1][1]= 0.  ;
         xyz0[k][1][2]= -dz;

         xyz0[k][2][0]=0.;
         xyz0[k][2][1]=0.;
         xyz0[k][2][2]=0.;

        // 1).............................rotation of beta:
        double beta=0;
        if(k==0){
        beta=beta1;
        }else{
        beta=beta3;
        }

       for(int i=0 ; i <3 ; i++){
        xyzRb[k][i][0]=  xyz0[k][i][0]                                       + xyzC[k][0];
        xyzRb[k][i][1]=  xyz0[k][i][1]*cos(beta) - s*xyz0[k][i][2]*sin(beta) + xyzC[k][1];
        xyzRb[k][i][2]=s*xyz0[k][i][1]*sin(beta) +   xyz0[k][i][2]*cos(beta) + xyzC[k][2];
        }

        // 2)..........................rotation of alpha
        for(int i=0 ; i <3 ; i++){
        xyzRba[k][i][0]=  xyzRb[k][i][0]*cos(alpha) + xyzRb[k][i][2]*sin(alpha)  ;
        xyzRba[k][i][1]=  xyzRb[k][i][1] ;
        xyzRba[k][i][2]= -xyzRb[k][i][0]*sin(alpha) + xyzRb[k][i][2]*cos(alpha) ;

        }


        //..............................3 points to define the plane:

        //if(k==0)cout<<" 1A........ "<<endl;
        //if(k==1)cout<<" 3A........ "<<endl;

        for(int i=0 ; i<3 ; i++){
                for(int j=0 ; j<3 ; j++){
                                P[k][i][j]=  xyzRba[k][i][j] ;
                }
        }

        //............................................................
         A[k]=   P[k][0][1]*( P[k][1][2] - P[k][2][2] ) -  P[k][0][2]*( P[k][1][1]- P[k][2][1]) +
                                                        ( P[k][1][1]* P[k][2][2] -  P[k][1][2]* P[k][2][1]);
         B[k]= -(P[k][0][0]*( P[k][1][2] - P[k][2][2] ) -  P[k][0][2]*( P[k][1][0]- P[k][2][0]) +
                                                        ( P[k][1][0]* P[k][2][2] -  P[k][1][2]* P[k][2][0]));
         C[k]=   P[k][0][0]*( P[k][1][1] - P[k][2][1] ) -  P[k][0][1]*( P[k][1][0]- P[k][2][0]) +
                                                        ( P[k][1][0]* P[k][2][1] -  P[k][1][1]* P[k][2][0]);
         D[k]=-(P[k][0][0]*(P[k][1][1]*P[k][2][2]-P[k][1][2]* P[k][2][1])-
                P[k][0][1]*(P[k][1][0]*P[k][2][2]-P[k][1][2]*P[k][2][0])+P[k][0][2]*(P[k][1][0]*P[k][2][1]-P[k][1][1]*P[k][2][0]));


        double num= A[k]*coo[0] + B[k]*coo[1] + C[k]*coo[2] +D[k]  ;
        double den= A[k]*dv[0] +  B[k]*dv[1] + C[k]*dv[2];



        //.................. r == distance of intersection:
        //coordinates of intersection:
        // x=r*cos*(dirX) + x0
        // y=r*cos*(dirY) + y0
        // z=r*cos*(dirZ) + z0
        //(x0,y0,z0)---> coortinates of incident point 


        if(den!=0){
        r[k]=-num/den;
        }
        else{
        //....No intersection because the plane and the particle directions are parallel.
        r[k]=0;
        }

        //....................intersection point in SSACS:
        Int[k][0]=r[k]*dv[0] + coo[0];
        Int[k][1]=r[k]*dv[1] + coo[1];
        Int[k][2]=r[k]*dv[2] + coo[2];

        //cout<<" Int[k] "<< Int[k] <<endl;


        //...........................Ckeck if intersection point is onside Solar Array:

        //...back to original position: 
        //1) -alpha rotation 
        Int1[k][0]=  Int[k][0]*cos(alpha) - Int[k][2]*sin(alpha)  ;
        Int1[k][1]=  Int[k][1] ;
        Int1[k][2]=  Int[k][0]*sin(alpha) + Int[k][2]*cos(alpha) ;

        //2) -shift for zero popsition
        Int1[k][0]-= xyzC[k][0];
        Int1[k][1]-= xyzC[k][1];
        Int1[k][2]-= xyzC[k][2];

        //3) -beta rotation + shift for zero posiotion in SSACS
        Int0[k][0]=   Int1[k][0]                                 + xyzC[k][0];
        Int0[k][1]=    Int1[k][1]*cos(beta)   + s*Int1[k][2]*sin(beta) + xyzC[k][1];
        Int0[k][2]= -s*Int1[k][1]*sin(beta)   +   Int1[k][2]*cos(beta) + xyzC[k][2];



        dr[k]=r[k]*tan(addon);//5 sigma radius

        //...........................................................
        //with propagation of errors calculate the uncertainty for each dimension : dx , dy, dz
        //they depend on : beta (db), alpha (da), tilt angle (dt) and multiple scattering (dr)

        // X partial derivates:
        double dxdb=0;
        double dxda=- Int[k][0]*sin(alpha) - Int[k][2]*cos(alpha);
        double dxdr=cos(alpha)*dv[0] - sin(alpha)*dv[2];
        double dxdt=-r[k]*sin(alpha)*cos(tilt)*dir[0] - r[k]*sin(alpha)*sin(tilt)*dir[2];
        // Y partial derivates:
        double dydb=-Int1[k][1]*sin(beta) +s* Int1[k][2]*cos(beta);
        double dyda=s*sin(beta)*(Int[k][0]*cos(alpha)-Int[k][2]*sin(alpha) );
        double dydr=cos(beta)*dv[1] +s*sin(beta)*(sin(alpha)*dv[0] + cos(alpha)*dv[2] );
        double dydt=r[k]*cos(beta)*(dir[0]*sin(tilt) - dir[2]*cos(tilt) ) + s*sin(beta)*cos(alpha)*r[k]*(dir[0]*cos(tilt) + dir[2]*sin(tilt)  ) ;
         // Z partial derivates:
        double dzdb=-s*Int1[k][1]*cos(beta) - Int1[k][2]*sin(beta);
        double dzda=cos(beta)*(Int[k][0]*cos(alpha) - Int[k][2]*sin(alpha));
        double dzdr=-s*sin(beta)*dv[1] + cos(beta)*( sin(alpha)*dv[0] + cos(alpha)*dv[2]);
        double dzdt= -s*sin(beta)*r[k]*(dir[0]*sin(tilt) - dir[2]*cos(tilt)) + cos(beta)*r[k]*(-dir[0]*cos(tilt) - dir[2]*sin(tilt));

        //squares:
        double dxdb2=dxdb*dxdb;
        double dxda2=dxda*dxda;
        double dxdr2=dxdr*dxdr;
        double dxdt2=dxdt*dxdt;

        double dydb2=dydb*dydb;
        double dyda2=dyda*dyda;
        double dydr2=dydr*dydr;
        double dydt2=dydt*dydt;

        double dzdb2=dzdb*dzdb;
        double dzda2=dzda*dzda;
        double dzdr2=dzdr*dzdr;
        double dzdt2=dzdt*dzdt;

        double db2=db*db;
        double da2=da*da;
        double dr2=dr[k]*dr[k];
        double dt2=dt*dt;

        //sigmas:
        double sigmax= sqrt( dxdb2*db2 + dxda2*da2 + dxdr2*dr2 +dxdt2*dt2  );
        double sigmay= sqrt( dydb2*db2 + dyda2*da2 + dydr2*dr2 +dydt2*dt2  );
        double sigmaz= sqrt( dzdb2*db2 + dzda2*da2 + dzdr2*dr2 +dzdt2*dt2  );


        if(r[k]>0 && Int[k][2]<0 ){

                //...............................all uncertainties together
                // distance of intersection point to SA center
                double dxc=abs(Int0[k][0]- xyzC[k][0]);
                double dyc=abs(Int0[k][1]- xyzC[k][1]);
                double dzc=abs(Int0[k][2]- xyzC[k][2]);

        if( dxc<= dx+ sigmax && dyc<= dy+ sigmay && dzc<= dz+ sigmaz  ){
                        // 1A , 3A or no ? 
                        wh=k;
                        }
                }
        }//.....................end for k ...........................
       //.............................................
        //............................................
        if(wh==-1){
        //cout<< " NOT cross Solar Array!"<<endl;
        ic=AMSPoint(FLT_MAX,FLT_MAX,FLT_MAX);
        return 0;
        }else if(wh==0){ // in 1A
                ic[0]=Int0[0][0];
                ic[1]=Int0[0][1];
                ic[2]=Int0[0][2];
        return 1;

        }else if(wh==1){ // in 3A
                ic[0]=Int0[1][0];
                ic[1]=Int0[1][1];
                ic[2]=Int0[1][2];
        return 1;

        }
        //cout<<"---------------------------------------\n "<<endl;




   return 0;
}
//----------------------------------------------------------------------
void AMSEventR::FromISStoAMS(double iss[3],double ams[3] ){

        double pi = 3.141592654;
        double deg2rad=pi/180.;


// AMS Coo central point in SSACS [cm]
        double x_ams=-85.73;
        double y_ams=2136.4;
        double z_ams=-388.29;

        double tilt =12.*deg2rad;
        double iss0[3];
        double isst[3];

        iss0[0]= iss[0]-x_ams;
        iss0[1]= iss[1]-y_ams;
        iss0[2]= iss[2]-z_ams;

        isst[0]= -iss0[1];
        isst[1]= -iss0[0];
        isst[2]= -iss0[2];


        double cost=cos(tilt);
        double sint=sin(tilt);


        ams[0]=isst[0]*cost - isst[2]*sint;
        ams[1]=isst[1];
        ams[2]=isst[0]*sint + isst[2]*cost;


}
//----------------------------------------------------------------------
double  AMSEventR::SolidAngleInShadow(double AMSfov0){

        float alpha, beta1,beta3,b1b, b3b;

        //....Set Alpha and beta angles
        int getAngles=fHeader.getISSSA(alpha,beta1,beta3,b1b,b3b);

        //.....no data:
        if(getAngles==2){ cerr<< " AMSEventR::SolidAngleInShadow --> No ISSSA data!"<<endl;
        return -1;
        }


	//cout<<"MA ALLORA!!!! :< "<<endl;

        //..from deg to rad:
        double pi = 3.141592654;
        double deg2rad=pi/180.;
        double AMSfov=AMSfov0*deg2rad;
        alpha*=deg2rad;
        beta1*=deg2rad;
        beta3*=deg2rad;

                //.............the quantity to be calculated:
                double solidAngle[2];

        //define Aolar Arrays coordinates and dimensions............
        double  x_A1=1982.51;//cm
        double  y_A1=3417.32 ; //cm
        double  z_A1=-23.77;//cm

        double  x_A3=-1982.51;//cm
        double  y_A3=3418.02 ; //cm
        double  z_A3=110.11;//cm

        double  dx=1306.*2.54;
        double  dy= 0.5*2.54;
        double  dz= 453.9*2.54;

        double xyzC[3];
         xyzC[0]=0;
         xyzC[1]=0;
         xyzC[2]=0;


        dx/=2.;
        dy/=2.;
        dz/=2.;

        //......................check if there is intersection:
        int check[2][4];// 2 Solar Arrays x 4 corners

        //...........................................do it for both SA 
        for(int p=0 ; p<2 ; p++){
        //...... 1) check if there is intersection or not!
        //int check[4];
        double AMSp[4][3];
        double P[4][3];

        double theta=alpha;

        double xyz0[4][3];
        double xyzRb[4][3];
        double xyzRba[4][3];
        double beta=0; //----> to be define according to the Solar Array
        double s;

        //..........1A
        if(p==0){
        beta=beta1;
        s=1.;
        xyzC[0]=x_A1;
        xyzC[1]=y_A1;
        xyzC[2]=z_A1;
        }
        //.........3A
        if(p==1){
        beta=beta3;
        s=-1.;
        xyzC[0]=x_A3;
        xyzC[1]=y_A3;
        xyzC[2]=z_A3;
        }

       //........corners positions in local SA 
        xyz0[0][0]= s*dx ;
        xyz0[0][1]= dy  ;
        xyz0[0][2]= dz ;

        xyz0[1][0]= s*dx ;
        xyz0[1][1]= dy  ;
        xyz0[1][2]= -dz;

        xyz0[2][0]=s*dx;
        xyz0[2][1]=-dy;
        xyz0[2][2]= dz ;

        xyz0[3][0]= s*dx ;
        xyz0[3][1]= -dy  ;
        xyz0[3][2]= -dz;


        //.............................transform to ISS coordinate system:
       //...beta rotation:
        for(int i=0 ; i <4 ; i++){
        xyzRb[i][0]=  xyz0[i][0]                                    + xyzC[0];
        xyzRb[i][1]=  xyz0[i][1]*cos(beta) - s*xyz0[i][2]*sin(beta) + xyzC[1];
        xyzRb[i][2]=s*xyz0[i][1]*sin(beta) +   xyz0[i][2]*cos(beta) + xyzC[2];

        }

        //---alpha rotation:
        for(int i=0 ; i <4 ; i++){

        xyzRba[i][0]=  xyzRb[i][0]*cos(theta) + xyzRb[i][2]*sin(theta)  ;
        xyzRba[i][1]=  xyzRb[i][1] ;
        xyzRba[i][2]= -xyzRb[i][0]*sin(theta) + xyzRb[i][2]*cos(theta) ;

        }
        //......................................Corners coordinates in ISS 
        for(int i=0 ; i<4 ; i++){
                for(int j=0 ; j<3 ; j++){
                                P[i][j]=  xyzRba[i][j] ;
                }
        }



        //............................form ISS to AMS
        for(int i=0; i<4;i++){
                check[p][i]=0;//----------------> set to zero check
                FromISStoAMS(P[i], AMSp[i] );//--->from ISS--to AMS coord.Sis.

       }


 	//------------------------------Check if there is intersection:
               for(int i=0; i<4;i++){
                //-------------------> if corners Z>0
                              if(AMSp[i][2]>=0 ){
                                if(AMSfov0!=90.){
                                double rconeP=AMSp[i][2]*tan(AMSfov);
                                double rp=std::sqrt( AMSp[i][0]*AMSp[i][0] + AMSp[i][1]*AMSp[i][1] );

                                        if(rp<=rconeP ){
                                        check[p][i]=1;
                                        }else check[p][i]=0;
                                }else if (AMSfov0==90)  check[p][i]=1;
                }else check[p][i]=0;

                        }//-------------------------end check




              //..............if there is no intersection:
                if(check[p][0]!=1 && check[p][1]!=1 && check[p][2]!=1 &&  check[p][3]!=1){

                //.......................Set to Zeto the solid angle! 
                         solidAngle[p]=0;
                //.............if there is intersection:
                }else if(check[p][0]==1 || check[p][1]==1 || check[p][2]==1 ||  check[p][3]==1 ){

                double phi[4];
                double t[4];
                        //-------------------calculate theta and phi of 4 corners
                        for(int i=0; i<4;i++){
                        double x0=AMSp[i][0];
                        double y0=AMSp[i][1];
                        double z0=AMSp[i][2];

                         phi[i]=atan(y0/x0);
                        double R0=sqrt(x0*x0 + y0*y0 + z0*z0);
                         t[i]=acos(z0/R0);
                        }


               //--------------the higher dphi and the lower theta
              double dphi[]={ fabs(phi[0] - phi[1] ),
                                fabs(phi[0] - phi[2] ),
                                fabs(phi[0] - phi[3] ),
                                fabs(phi[1] - phi[2] ),
                                fabs(phi[1] - phi[3] ),
                                fabs(phi[3] - phi[2] ),
                                   };
                //------dpdi.........
              double dphimax=dphi[0] ;//--> choose the first and then check
                for(int j=0 ; j<6 ; j++ ){
                        if(dphi[j]>dphimax ) dphimax=dphi[j];
                }

                //---the same for theta.....
                double tmin=t[0];//--> choose the first then check:
                for(int i=0; i<4;i++){
                        if(t[i] < tmin ) tmin=t[i];//---this is the min! 
                }
        //---------------- AMSfov in shadow ====>  ;
       double cosAMSfov2=cos(AMSfov)*cos(AMSfov);
        double costmin2=cos( tmin)*cos( tmin) ;


                        //...this is the solid angle!!
                        solidAngle[p]= - dphimax* ( cosAMSfov2 - costmin2 )/2.;
			//if  (solidAngle[p]!=0) cout<<"----AMSEventR::SolidAngleInShadow() "<<  solidAngle[p] <<endl;

                }//..........end if

        }//................................................................end for p

       //----------------------------Return the Intersection solid angle ;       


                if( solidAngle[0]==0 && solidAngle[1]==0 ){
                        return 0;
                }else if (solidAngle[0]!=0 ){

                        return solidAngle[0];

                }else if (solidAngle[1]!=0 ){
                        return solidAngle[1];

                }



 return 0;

}
//------------------------------------------------------------------------------
int AMSEventR::GetMaxGeoCutoff( double AMSfov ,double degbin , double cutoff[2]){

	if(degbin<=0){
	cerr<<" AMSEventR::GetMaxGeoCutoff degbin parameter must be >0 !"<<endl;
	return -1;
	}
       //...set to zero
        cutoff[0] =0;
        cutoff[1] =0;

        double Re =  6371.2; //km Earth radius 
        double deg2rad = TMath::DegToRad();
        double AMSfovrad = deg2rad * AMSfov;

        //..... 1x1 deg [theta, phi] 
     //   double degbin =1. ;

        double Thetamin =0;
        double Thetamax =AMSfovrad;
        double dTheta = degbin *deg2rad;
        double dimt = (Thetamax - Thetamin )/dTheta +1;
        int dimtheta = (int) dimt;

	if(dimtheta==0) return -1; 

        double Phimin =0;
        double Phimax= 360.*deg2rad;
        double dPhi =  dTheta;
        double dimp= (Phimax - Phimin )/dPhi +1;
        int dimphi = (int) dimp;


       // double Theta[dimtheta];//Colat==angle between ams zenith and direction!
       // double Phi[dimphi];

	//-----------------------------------
      	//---directions 
        vector <double>  xv;
        vector <double>  yv;
        vector <double>  zv;



        for (int i = 0 ; i < dimtheta ; i++){
         double      Theta = Thetamin + dTheta*i;
                for (int j = 0 ; j < dimphi ; j++){
         double      Phi = Phimin + dPhi*j;


                double x = sin( Theta  )* cos( Phi );
                double y = sin( Theta  )* sin( Phi );
                double z = cos( Theta  );

                double r0 = sqrt( x*x + y*y +z*z);
                x/=r0;
                y/=r0;
                z/=r0;
                if (Theta <= AMSfovrad ){

                        //select only directions in AMSfov 
                xv.push_back(x);
                yv.push_back(y);
                zv.push_back(z);


                }//---end if in AMSfov
        }//.........end for j
        }//.........end for i


        //-----------------vector dimension
        int dim = xv.size();
        for(int i =0 ; i < dim ; i++){//........only in AMSfov, select Max Rcutoff

       //...invert direction to simulate ParticleR dir in ams ref. system 
        AMSDir ams;
        ams[0] = -xv[i];
        ams[1] = -yv[i];
        ams[2] = -zv[i];


        // From AMS to GTOD
        //...out:
        int result ;

        //.............in deg!!!!
        double  theta_deg ; //GTOD
        double  phi_deg ;

        //..in :--->      
        double  theta = ams.gettheta();
        double  phi =  ams.getphi();


        //...This function inverts particle dir of ams system and returns particle dir in GTOD  deg!!!
	// Parameters:   use_att= 1, use_coo = 1, use_time= 1, dt= 0, out_type= 3
        int gtodT =  GetGalCoo(result, phi_deg , theta_deg, theta, phi, 1,1,1,0.,3 ); //correct SDT inverted phi_deg and theta_deg
	if (gtodT<0) return  -1;


        //...gtheta is colatitude --> I need Latitude:
        //theta_deg=90.- theta_deg; // SDT - Jan 2013 - GetGalCoo gives latitude instead of colatitude (prev Version)

        //....from deg to rad
        //gtheta*=deg2rad;
        //gphi*=deg2rad;        

        //cout<< " AMSEventR::GetMaxGeoCutoff -> gtheta "<< gtheta/deg2rad << " gphi "<< gphi/deg2rad << endl;
        //......Geographic coo.................................................
                double ThetaISS=    fHeader.ThetaS;
                double PhiISS=      fHeader.PhiS;
                //..from rad to deg:
                double ThetaISS_deg=ThetaISS/deg2rad;
                double PhiISS_deg=PhiISS/deg2rad;
                //----to be centered in 0!!
                //............................ZeroCentered
                if(PhiISS_deg > 180)  PhiISS_deg-=360.;
        //        cout<< " ISS pos theta "<< Theta_deg << " Phi "<< Phi_deg << endl;
                time_t Time = UTCTime();
                double Altitude = fHeader.RadS/1.e5-Re;//km
                // cout << "AMSEventR::GetMaxGeoCutoff Altitude =  ok in km! " << Altitude << endl;     
                // this function needs GTOD coo. in deg and Altitude in km
                //..........................................................................................pos part            
                double Rcutoff_pos =  GeoMagCutoff( Time, Altitude , theta_deg, phi_deg, ThetaISS_deg, PhiISS_deg, 1 );
                //..........................................................................................neg part            
                double Rcutoff_neg =  GeoMagCutoff( Time, Altitude , theta_deg, phi_deg, ThetaISS_deg, PhiISS_deg, 0 );

                //.....maximum Rcut off 
                if ( Rcutoff_pos > cutoff[1]  ) {
                        cutoff[1] = Rcutoff_pos ;
                        }
                if (fabs(Rcutoff_neg ) > fabs(cutoff[0] ) ) {
                        cutoff[0] =Rcutoff_neg ;
                }
/*
      cout<<Form("%7.2f %7.2f %6.2f %6.2f",
		 theta*TMath::RadToDeg(), phi*TMath::RadToDeg(),
		 Rcutoff_neg, Rcutoff_pos)<<endl;
*/
        }//....................................//end for i 



//cout << "AMSEventR::GetMaxGeoCutoff Negative cutoff[0] " << cutoff[0] << " Positive cutoff[1] "<< cutoff[1] << endl;  




return 0;
}


int AMSEventR::GetIGRFCutoff(double &Rcut, int sign, AMSDir dir)
{
  double YPR[3] = { fHeader.Yaw,       fHeader.Pitch,  fHeader.Roll };
  double RPT[3] = { fHeader.RadS,      fHeader.PhiS,   fHeader.ThetaS };
  double VPT[3] = { fHeader.VelocityS, fHeader.VelPhi, fHeader.VelTheta };
  double xtime  =   fHeader.UTCTime();

  return fHeader.GetIGRFCutoff(Rcut, sign, dir, RPT, VPT, YPR, xtime);
}

map<unsigned int,AMSEventR::IGRF> AMSEventR::fIGRF;

int AMSEventR::GetMaxIGRFCutoff(double fov, double degbin, double cutoff[2])
{
  if (degbin > 0) {
    double YPR[3] = { fHeader.Yaw,       fHeader.Pitch,  fHeader.Roll };
    double RPT[3] = { fHeader.RadS,      fHeader.PhiS,   fHeader.ThetaS };
    double VPT[3] = { fHeader.VelocityS, fHeader.VelPhi, fHeader.VelTheta };
    double xtime  =   fHeader.UTCTime();

    return fHeader.GetMaxIGRFCutoff(fov, degbin, cutoff, RPT, VPT, YPR, xtime);
  }
  
  unsigned int tm = UTime();
  if (degbin < -1300000000) tm = -degbin;
  return GetMaxIGRFCutoff(fov, cutoff, tm);
}

int AMSEventR::GetMaxIGRFCutoff(double fov, double cutoff[2], unsigned int xtime,const char *fdir){

  cutoff[0] = cutoff[1] = 0;

  static int stm = 0;
  if (stm < 0) return -1;

  unsigned int tm = xtime;

  const int    dt = 1000000;
  const int    ct = 3600;// compensate item
  int          tid= tm/dt*dt;//stemp
  unsigned int bt = tid-ct;
  unsigned int et = tid+dt-1+ct;
//---
  if (tid != stm) fIGRF.clear();

  if (fIGRF.empty()) {
    TString std = "/afs/cern.ch/ams/Offline/AMSDataDir";
    const char *ad = getenv("AMSDataDir");
    const char *ai = getenv("AMSISS");
    if (ad && strlen(ad)) std = ad; std += "/altec/";
    if (ai && strlen(ai)) std = ai; std += "RTI/";
    if(fdir!=0) std=fdir;
    TString sfn = Form(std+"MaxIGRFCutoff_%d.csv", int(tm/10000000));
    ifstream fin(sfn);
    if (!fin) {
      cerr << "AMSEventR::GetMaxIGRFCutoff-E-File not found: "
	   << sfn.Data() << endl;
      stm = -1;
      return -1;
    }
    else {
       cout << "AMSEventR::GetMaxIGRFCutoff File: " << sfn.Data() << endl;
    }
    while (fin.good() && !fin.eof()) {
      unsigned int nt; IGRF a;
      fin >> nt >> a.phi >> a.theta;
      for(int j = 0; j < 4; j++) fin >> a.cf[j][0] >> a.cf[j][1];
      if (!fin.good()) continue;
      if (nt<bt||nt>et)continue;//unnesasery erase
      fIGRF.insert(make_pair(nt, a));
    }
    fin.close();
    cout<< "AMSEventR::GetMaxIGRFCutoff-I- " << fIGRF.size()
	<< " Entries Loaded" << endl;
    stm = tid;
  }

  unsigned int t1, t2;
  IGRF a1, a2;
  map<unsigned int,IGRF>::iterator k = fIGRF.lower_bound(tm);
  if (k == fIGRF.end()) k--;
  t2 = k->first; a2 = k->second; k--;
  t1 = k->first; a1 = k->second;

  int fi = (fov-21)/5;
  if (fi < 0) fi = 0;
  if (fi > 3) fi = 3;

  cutoff[0] = a1.cf[fi][0]+(a2.cf[fi][0]-a1.cf[fi][0])*(tm-t1)/(t2-t1);
  cutoff[1] = a1.cf[fi][1]+(a2.cf[fi][1]-a1.cf[fi][1])*(tm-t1)/(t2-t1);

  return 0;
}


//----------------------------------------------------------------------
int AMSEventR::GetRTIStat(){

   AMSSetupR::RTI a;
   return getsetup()->getRTI(a,fHeader.Time[0]);
}

int AMSEventR::GetRTI(AMSSetupR::RTI & a){

   return getsetup()->getRTI(a,fHeader.Time[0]);
}

//--------
map<unsigned int,AMSSetupR::RunI> AMSEventR::fRunList;

int AMSEventR::RecordRTIRun(){

  static string pf="";
//----Find rootfile
  unsigned int nt=fHeader.Time[0];
  unsigned int nr=fHeader.Run;
  string nf=Tree()->GetCurrentFile()->GetName();
  if(nf==pf){
     if(nt<fRunList[nr].bt)fRunList[nr].bt=nt;
     if(nt>fRunList[nr].et)fRunList[nr].et=nt;
     return 2;//exist
  }

  pf=nf;
//----  
  map<unsigned int,AMSSetupR::RunI>::iterator it=fRunList.find(nr);
  if(it==fRunList.end()){//Find Run
     fRunList.insert(make_pair(nr,AMSSetupR::RunI(nr,nt,nt,nf)));
     return 0;//new run
  }
  else {//Exist Run Check File
    if(nt<fRunList[nr].bt)fRunList[nr].bt=nt;
    if(nt>fRunList[nr].et)fRunList[nr].et=nt;
    for(unsigned int ifn=0;ifn<fRunList[nr].fname.size();ifn++){
      if(fRunList[nr].fname.at(ifn)==nf)return 2;//exist
    }
    fRunList[nr].fname.push_back(nf); //new file
    return 1;//
  }

  return 3;

}

int AMSEventR::GetRTI(AMSSetupR::RTI & a, unsigned int  xtime){

#ifdef __ROOTSHAREDLIBRARY__
  static AMSSetupR setupu;
  static unsigned int stime[2]={1,1};
#pragma omp threadprivate (setupu)
#pragma omp threadprivate (stime)
static int vrti=-1;
#pragma omp threadprivate (vrti)
if(AMSSetupR::RTI::Version!=vrti){
  cout<<"<<----------RTI Version="<<AMSSetupR::RTI::Version<<"---------->>"<<endl;
  vrti=AMSSetupR::RTI::Version;
}
 const int pt=3600*3;
 const int dt=3600*24;
 if(stime[0]==1||xtime>stime[1]||xtime<stime[0]){
    stime[0]=(xtime<=pt)?1:xtime-pt;
    stime[1]=xtime+dt;
    setupu.fRTI.clear();
    setupu.LoadRTI(stime[0],stime[1]);
 }
//---Status
  AMSSetupR::RTI b;
  a=b;
  AMSSetupR::RTI_i k=setupu.fRTI.lower_bound(xtime);
  if (setupu.fRTI.size()==0)return 2;
  if(k==setupu.fRTI.end())return 1;

  if(xtime==k->first){//find
    a=(k->second);
    return 0;
  }
#endif
  return 2;
}


int AMSEventR::GetRTIRunTime(unsigned int runid,unsigned int time[2]){

  const unsigned int pt=1000;//Run window Pr
  const unsigned int dt=3600;//Run window Af 1~hour 20min
  unsigned int bt=(runid<=pt)?1:runid-pt;
  unsigned int et=runid+dt;
  time[0]=time[1]=runid;//Not Found

//----Find BT
  AMSSetupR::RTI a;
  for(unsigned int t=bt;t<=et;t++){      
     if(GetRTI(a,t)!=0)continue; 
     if(a.run==runid){time[0]=t;break;}
  }
//---Find ET
  for(unsigned int t=et;t>=bt;t--){
     if(GetRTI(a,t)!=0)continue;
     if(a.run==runid){time[1]=t;break;}
  }

//--Result
  if(time[0]==et||time[1]==bt){
     cerr<<"RTI-RunTime Size Too Big Run="<<runid<<" Time="<<time[0]<<"-"<<time[1]<<endl;
  }
  if(time[0]>time[1]){
     cerr<<"RTI-RunTime Abnormal Run="<<runid<<" Time="<<time[0]<<"-"<<time[1]<<endl;
     time[0]=time[1]=runid;
  }
  int ssize=time[1]-time[0]+1;
  return ssize;
}

int AMSEventR::GetRTIdL1L9(int extlay,AMSPoint &nxyz, AMSPoint &dxyz,unsigned int xtime,unsigned int dt){

  unsigned int bt= xtime/dt*dt;
  unsigned int et= bt+dt-1;
 
  static unsigned int rbt=0;
  static unsigned int ret=0;
  static unsigned int rst=0;//Eff  dt
  static AMSPoint rnxyz[2];
  static AMSPoint rdxyz[2]; 

//----New External Layer If
  if(!(bt>=rbt&&et<=ret)){ //First Time arrive
    rst=0;
    for(int iexl=0;iexl<2;iexl++){rnxyz[iexl].setp(0,0,0);rdxyz[iexl].setp(0,0,0);}
//----Sum BT
    AMSSetupR::RTI a;
    for(unsigned int t=bt;t<=et;t++){
       if(GetRTI(a,t)!=0)continue;
       for(int iexl=0;iexl<2;iexl++){
         for(int ico=0;ico<3;ico++){
           double nev=a.nl1l9[iexl][ico==0?0:1];
           rnxyz[iexl][ico]+=nev;//X Y Z Events number
           rdxyz[iexl][ico]+=a.dl1l9[iexl][ico]*nev;
         }
       }
       rst++;
    }
//---Calculate Mean
     for(int iexl=0;iexl<2;iexl++){
       for(int ico=0;ico<3;ico++){
         rdxyz[iexl][ico]=(rnxyz[iexl][ico]>0)?rdxyz[iexl][ico]/rnxyz[iexl][ico]:0;
       }
     }
  }
    
  nxyz=rnxyz[extlay]; dxyz=rdxyz[extlay];
  rbt=bt;     ret=et;
  return rst;
}


//----------------------------------------------------------------------
double HeaderR::Zenith(){

        if(Pitch==0 && Yaw==0 && Roll==0){
		  getISSAtt();
        }
        double  cb = cos(Pitch);
        double  cg = cos(Roll);
        double  sg = sin(Roll);

        const float angle=12./180*3.1415926;
        double crp=cos(angle);
        double srp=sin(angle);
        double cams=(sg*cb*srp+cg*cb*crp);
        cams=acos(cams)*180/3.1415926;
        return cams;
}  
char * HeaderR::Info(unsigned long long status){
	double cams=Zenith(); 
	unsigned int comp=0;
	unsigned long long one=1;
	char bits[66];
	for(int k=0;k<32;k++){
		if(status&(one<<k))bits[k]='1';
		else bits[k]='0';
	}
	bits[32]=' ';
	for(int k=32;k<64;k++){
		if(status&(one<<k))bits[k+1]='1';
		else bits[k+1]='0';
	}
	bits[65]='\0';  
	for(int i=0;i<6;i++){
		if(status&(1<<(i+2)))comp+=int(pow(10.,i));
	}
	float alpha,b1a,b3a,b1b,b3b;
	alpha=0;
	b1a=0;
	b3a=0;
	getISSSA(alpha,b1a,b3a,b1b,b3b);
	float r,phi,theta,v,vphi,vtheta;
	getISSCTRS(r,theta,phi,v,vtheta,vphi);

	AMSSetupR::DSPError a;
	int dsperr = getDSPError(a);

	sprintf(_Info,"Header:  Status %s %s, Lat %6.1f^{o}, Long %6.1f^{o}, Rad %7.1f km, Velocity %7.2f km/s,  #Theta^{M} %6.2f^{o}, Zenith %7.2f^{o}  #alpha %d #beta_{1a}%d #beta_{3a} %d TrRH %d  TrStat %x DSPError %s",
		bits,(status & (1<<30))?"Error ":"OK ",ThetaS*180/3.1415926,PhiS*180/3.1415926,RadS/100000,VelocityS*RadS/100000, ThetaM*180/3.1415926,cams,int(alpha),int(b1a),int(b3a),TrRecHits,TrStat,(dsperr)?"true":"false");
	return _Info;
}



char * ParticleR::Info(int number, AMSEventR* pev){
	double anti=AntiCoo[0][2];
	float btof=0;
	if(iBeta()>=0){
		if(pev){
			BetaR bta=pev->Beta(iBeta());
			btof=bta.Beta;
                        
		}
        }
	float btofh=0;float ebtofh=0;
	if(iBetaH()>=0){
		if(pev && pev->nBetaH()>iBetaH()){
			BetaHR bta=pev->BetaH(iBetaH());
			btofh=bta.GetBeta();
                        ebtofh=bta.GetEBetaV();
                        ebtofh=fabs(ebtofh/btofh/btofh);
		}
        }



	if(pev && pev->Version()<566){
		Loc2Gl(pev);
	}
	if(fabs(anti)>fabs(AntiCoo[1][2]))anti=AntiCoo[1][2];
	float lt=pev?pev->LiveTime():1;
	sprintf(_Info," Particle %s No %d Id=%d p=%7.3g#pm%6.2g M=%7.3g#pm%6.2g #theta=%4.2f #phi=%4.2f Q=%2.0f  #beta=%6.3f#pm%6.3f/%6.2f/ #betah=%6.3f#pm%6.3f  #theta_M %4.1f^{o} Coo=(%5.2f,%5.2f,%5.2f) LT %4.2f #theta_G %4.2f #phi_G %4.2f",pType(),number,Particle,Momentum,ErrMomentum,Mass,ErrMass,Theta,Phi,Charge,Beta,ErrBeta,btof,btofh,ebtofh,pev->fHeader.ThetaM/3.1415926*180,Coo[0],Coo[1],Coo[2],lt,ThetaGl,PhiGl);
	return _Info;





}

unsigned int DaqEventR::L3NodeError(int i){

int j=i*8;
return (unsigned int)((L3Error>>j)&255);
}

char * DaqEventR::Info(int number){
    int sc=L3dr&1;
    int sa1=(L3dr>>1)&1;
    int sa2=(L3dr>>2)&1;
    int sa3=(L3dr>>3)&1;
    int sa4=(L3dr>>4)&1;
    int status=(L3dr>>5)&7;
    int len=(L3dr>>8)&31;
    int e=(L3dr>>13)&1;
    int p=(L3dr>>14)&1;
    int a=(L3dr>>15)&1;
    int hwerr=HasHWError();
    sprintf(_Info,"Length  %d TDR %d UDR %d SDR %d RDR %d EDR %d LVL1 %d ; LVL3  %d %d %d %d %d %d %d %d %d %d %d L3counter %d L3Td %f7.1 msec ; HasHWError %d",Length,Tdr,Udr,Sdr,Rdr,Edr,L1dr,a,p,e,len,status,sa4,sa3,sa2,sa1,sc, (L3dr>>16)&32767,L3Event(),double(L3TimeD)*0.64e-3,hwerr);
  return _Info;
  } 



#ifdef _PGTRACK_
#include "TrRecon.h"
#include "TrCalDB.h"
#include "TrParDB.h"
#include "TrPdfDB.h"
#include "TrGainDB.h"
#include "TrOccDB.h"
#include "TrChargeLossDB.h"
#include "TrEDepDB.h"
#include "TrMipDB.h"
#include "TrLinearDB.h"
#include "TrLikeDB.h"
#endif

void AMSEventR::InitDB(TFile *_FILE){
static int master=0;
#ifdef _PGTRACK_
//if(TkDBc::Head==0&& _EVENT!=0){
//  TkDBc::CreateTkDBc();
//  TkDBc::Head->init((_EVENT->Run()>=1257416200)?2:1);
//}
#ifdef __ROOTSHAREDLIBRARY__
#else
#pragma omp master 
#endif
{
  if (_FILE){
    if(TrCalDB::Head) delete TrCalDB::Head;
    TrCalDB::Head = dynamic_cast<TrCalDB*>(_FILE->Get("TrCalDB"));
    if(!TrCalDB::Head ){
       cerr<<" AMSEventR::InitDB-S-UnableToFindTrCalDBiN "<<_FILE->GetName()<<endl;
   } 
    if(!TkDBc::Head){
      if (!TkDBc::Load(_FILE)) { // by default get TkDBc from _FILE
	TrExtAlignDB::ForceFromTDV=1;
        TrInnerDzDB::ForceFromTDV=1;
        TkDBc::CreateTkDBc();    // Init nominal TkDBc if not found in _FILE
	int setup=0;
	if(Run()>=1300000000)setup=3;
	else if(Run()>=1257416200)setup=2;
         else setup=1;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp master
#endif
	TkDBc::Head->init(setup);
      }

      ///// SH: Workaround to take care of the wrong TrackerAlignPM3
      else {
	if (getsetup()) {
	  AMSSetupR::TDVR tdv;
	  getsetup()->getTDV("TrackerAlignPM3", UTime(), tdv);
	  if (tdv.Insert == 1342182208) {
	    cout << "AMSEventR::InitDB-W-Remove wrong TrackerAlignPM3: " 
		 << tdv.Insert << " "
		 << tdv.FilePath.Data() << endl;
	    TkDBc::ForceFromTDV = 1;
	  }
	}
      }
      ///// SH: Workaround to take care of the wrong TrackerAlignPM3

      if(TkDBc::ForceFromTDV && !AMSEventR::Head()->nMCEventgC())
	TkDBc::GetFromTDV(UTime(), TkDBc::ForceFromTDV);
    }
    if(!TrExtAlignDB::ForceFromTDV) 
      TrExtAlignDB::Load(_FILE);
    if(!TrInnerDzDB::ForceFromTDV) 
    TrInnerDzDB::Load(_FILE);

    

try{
	int save=TKGEOMFFKEY.MaxAlignedRun;
                                 if (_FILE->Get("datacards/TKGEOMFFKEY_DEF"))
    TKGEOMFFKEY =*((TKGEOMFFKEY_DEF*)_FILE->Get("datacards/TKGEOMFFKEY_DEF"));
     if(TKGEOMFFKEY.MaxAlignedRun<save){
         TKGEOMFFKEY.MaxAlignedRun=save;
         cerr<<"AMSEventr::InitDB-W-TKGEOMFFKEY.MaxAlignedrun Redefined "<<save<<endl;
      } 

      if (TRMCFFKEY.ReadFromFile  && _FILE->Get("datacards/TRMCFFKEY_DEF"))
    TRMCFFKEY   =*((TRMCFFKEY_DEF*)  _FILE->Get("datacards/TRMCFFKEY_DEF"));
				 if (_FILE->Get("datacards/TRCALIB_DEF"))
    TRCALIB     =*((TRCALIB_DEF*)    _FILE->Get("datacards/TRCALIB_DEF"));
				 if (_FILE->Get("datacards/TRCLFFKEY_DEF"))
    TRCLFFKEY   =*((TRCLFFKEY_DEF*)  _FILE->Get("datacards/TRCLFFKEY_DEF"));
      if (TRFITFFKEY.ReadFromFile && _FILE->Get("datacards/TRFITFFKEY_DEF"))
    TRFITFFKEY  =*((TRFITFFKEY_DEF*) _FILE->Get("datacards/TRFITFFKEY_DEF"));
                                 if (_FILE->Get("datacards/TRCHAFFKEY_DEF"))
    TRCHAFFKEY  =*((TRCHAFFKEY_DEF*) _FILE->Get("datacards/TRCHAFFKEY_DEF"));

}
catch (...){
cerr<<"AMSEventR::InitDB-E-Unabletoget datacards "<<endl;
}

    // if(TrParDB::Head) delete TrParDB::Head;
    TrParDB::Head = (TrParDB*) _FILE->Get("TrParDB");
    if (!TrParDB::Head) {
      TrParDB* cc = new TrParDB();
      cc->init();
    }
    TrClusterR::UsingTrParDB(TrParDB::Head);
 
    TrClusterR::UsingTrCalDB(TrCalDB::Head);
    TrRawClusterR::UsingTrCalDB(TrCalDB::Head);
    TrRecon::Init();
    TrRecon::SetParFromDataCards();
    TrRecon::UsingTrCalDB(TrCalDB::Head);

    // if (TrPdfDB::IsNull()) TrPdfDB::Load(_FILE);
    TrPdfDB::GetHead()->LoadDefaults();

    bool isReal = !(AMSEventR::Head()->nMCEventgC());

    // TrGainDB (if all attempts fail use default)
    // 1st attempt: file
    if ( (!TrGainDB::GetHead()->Load(_FILE))||(TrGainDB::GetHead()->IsDefault()) ) { 
      // 2nd attempt: TDV
      if (LoadFromTDV) TrGainDB::GetHead()->LoadFromTDV(UTime()); 
    }

    // TrOccDB (if all attempts fail use default)
    // 1st attempt: file
    if (!TrOccDB::GetHead()->Load(_FILE)) {
      printf("InitDB::TrOccDB load from file failed. Try TDV... \n");
      // 2nd attempt: TDV
      if ((!LoadFromTDV)||(!TrOccDB::GetHead()->LoadFromTDV(UTime(),isReal))||(TrOccDB::GetHead()->GetNBadStrips()==0)) {
        if (isReal) {
          printf("InitDB::TrOccDB load from TDV failed (or empty). Try with last resort histogram ... \n");
          // 3rd attempt on-the-fly creation from histogram (only for real data)
          if (!TrOccDB::GetHead()->CreateFromRawOccupanciesHisto((TH2F*)_FILE->Get("HistoMan/TrOccStri"))) {
            printf("InitDB::TrOccDB load with histogram failed. Use default.\n");
          }
        }
      }
    }
    printf("InitDB::TrOccDB has %d bad strips.\n",TrOccDB::GetHead()->GetNBadStrips());

    // create and load from AMSDataDir the charge loss correction database
    TrChargeLossDB::GetHead()->Init();
    // create and load from AMSDataDir the energy deposition correction database
    TrEDepDB::GetHead()->Init();
    // create and load from AMSDataDir the MIP correction database
    TrMipDB::GetHead()->Init();
    // create and load from AMSDataDir the linearity correction database
    TrLinearDB::GetHead()->Init();
    // create and load from AMSDataDir the pdf library
    TrLikeDB::GetHead()->Init();

    //------Initialize BacktracingMIB class
        if(BackTracingMIB::GetStatusBT()!=1){
    	      BackTracingMIB::GetfBT()->Initialize();
     	}
    



}
master=1;  
}
#else
master=1;
#endif
 while (master==0){
   usleep(1);
 }
  

}
const char * AMSEventR::GetCurrentFileName(bool setup){
if(setup && _TreeSetup){
 return _TreeSetup->GetCurrentFile()->GetName();
}
else if(!setup && _Tree)return _Tree->GetCurrentFile()->GetName();
else return NULL;
}


bool AMSEventR::UpdateSetup(uinteger run){
/*
int ttry=0; 
beg:
     bool beg=_EntrySetup==-1;
     while(getsetup() && _TreeSetup && _EntrySetup+1<_TreeSetup->GetEntries() && (getsetup()->fHeader.Run!=run)) {
      _TreeSetup->GetEntry(++_EntrySetup);
      if(ttry==0 || getsetup()->fHeader.Run!=run){
       --_EntrySetup;
      _TreeSetup->GetEntry(--_EntrySetup);
      ttry=1;
      }
  }
     bool ret=(getsetup()?getsetup()->fHeader.Run==run:false);
     if(!ret){
        _EntrySetup=-1;
        if(!beg)goto beg;
     }
*/
if(!_TreeSetup)return 2;
     _TreeSetup->SetBranchStatus("*",false);
     if(ProcessSetup>0)_TreeSetup->SetBranchStatus("run.fHeader",true);
     if(ProcessSetup>0)_TreeSetup->SetBranchStatus("run.fEntries",true);
     if(ProcessSetup>1)_TreeSetup->SetBranchStatus("*",true);
     if(ProcessSetup>2 || Version()>=800)_TreeSetup->SetBranchStatus("run.fJGC",false);
     if(Version()<700)_TreeSetup->SetBranchStatus("run.fEntries",false);
map <unsigned int,int>::iterator it= _RunSetup.theMap.find(run);
     bool ret=it!=_RunSetup.theMap.end();
     if(ret)_TreeSetup->GetEntry(it->second);

return ret;
}

int AMSEventR::GetSetup(uinteger entry){
if(!_TreeSetup)return 2;
   if(entry>=_TreeSetup->GetEntries())return 1;
     _TreeSetup->SetBranchStatus("*",false);
     if(ProcessSetup>0)_TreeSetup->SetBranchStatus("run.fHeader",true);
     if(ProcessSetup>0)_TreeSetup->SetBranchStatus("run.fEntries",true);
     if(ProcessSetup>1)_TreeSetup->SetBranchStatus("*",true);
	 _TreeSetup->GetEntry(entry);
     return 0;

}


bool AMSEventR::InitSetup(TFile *_FILE, const char *name,uinteger run){
  if (ProcessSetup<0) return false;
static int master=0;
bool suc=false;
#ifdef __ROOTSHAREDLIBRARY__
#else
#pragma omp master 
#endif
{
  master=0;
  if (_FILE){
    _TreeSetup=(TTree*)_FILE->Get(name);
    if(_TreeSetup){
#ifdef __ROOTSHAREDLIBRARY__
//#pragma omp critical
#endif
{
      if(getsetup()){
       getsetup()->Reset();
       getsetup()->Init(_TreeSetup);
      }
      else{
         AMSSetupR *a=new AMSSetupR();
         a->Init(_TreeSetup);
     }
   }
     _EntrySetup=-1;
     _RunSetup.theMap.clear();
     int entry=-1;
     _TreeSetup->SetBranchStatus("*",false);
     if(ProcessSetup>0){
     _TreeSetup->SetBranchStatus("run.fHeader",true);
     _TreeSetup->SetBranchStatus("run.fEntries",true);
     while(entry+1<_TreeSetup->GetEntries() ) {
      _TreeSetup->GetEntry(++entry);   
      map <unsigned int,int>::iterator it= _RunSetup.theMap.find(getsetup()->fHeader.Run);
      
      if(it==_RunSetup.theMap.end()){
        _RunSetup.theMap[getsetup()->fHeader.Run]=entry;
      }
      else{
       unsigned int levent=getsetup()->fHeader.LEvent;
        _TreeSetup->GetEntry(it->second);
        if(levent>getsetup()->fHeader.LEvent){
          _TreeSetup->GetEntry(entry);   
          _RunSetup.theMap[getsetup()->fHeader.Run]=entry;
        }
       }
     }
     }
     if(ProcessSetup>1)_TreeSetup->SetBranchStatus("*",true);
      map <unsigned int,int>::iterator it=_RunSetup.theMap.find(run);
     suc=it!=_RunSetup.theMap.end();
     if(suc){
         int k= _TreeSetup->GetEntry(it->second);
          _EntrySetup=it->second;
	  cout <<"AMSEventR::InitSetup-I-Read "<<k<<" Bytes from "<< _EntrySetup<<endl;
     } 
//     while(_EntrySetup+1<_TreeSetup->GetEntries() && (getsetup()->fHeader.Run!=run)) {
//      _TreeSetup->GetEntry(++_EntrySetup);
//     suc=!(getsetup()->fHeader.Run!=run);
//     }

}
}
if(!suc)_EntrySetup=-1;
master=1;  
}
 while (master==0){
   usleep(1);
 }
  
return suc;
}

int AMSEventR::GetSlowControlData(const char* en, vector<float>&v, int method){
if(!getsetup()) {
  return -2;
}
if(en && strlen(en)){
unsigned int time=UTime(); 
return getsetup()->fSlowControl.GetData(en,time,Frac(),v,method);
}
else return 1;
}

int AMSEventR::GetTDVEl(const string & tdv, unsigned int index, if_t & value){
  if(!getsetup()){
     return 5;
  }
  else{
   unsigned int time=UTime();
   AMSSetupR::TDVR_i tdvi;
   int ret=getsetup()->getTDVi(tdv,time,tdvi);
   value.u=0;
   if(!ret){
     if(index<tdvi->second.Size){
       if(index<tdvi->second.Data.size()){
          value.u=tdvi->second.Data[index];
          return 0;
       }
       else return 4;
     }
     else return 3;
   }
   else return ret;
  }
}


vector <unsigned int>AMSEventR::RunType;
vector <unsigned int>AMSEventR::BadRunList;

unsigned int AMSEventR::MinRun=0;
unsigned int AMSEventR::MaxRun=4294967295;
int AMSEventR::ProcessSetup=2;

bool AMSEventR::isBadRun(unsigned int run){
 
for(unsigned int k=0;k<BadRunList.size();k++){
if(run==BadRunList[k])return true;
}
return false;
}

bool AMSEventR::RunTypeSelected(unsigned int runtype){
if(!RunType.size())return true; 
for(unsigned int k=0;k<RunType.size();k++){
if(runtype==RunType[k])return true;
}
return false;
}

int Level1R::GetGPSTime(unsigned int &sec, unsigned int &nsec){
 const unsigned long long ns=1000000000LL; 
 if(TrigTime[1]==0)return 6;
 unsigned long long gpstime=((unsigned long long)TrigTime[3]<<32);
 gpstime+=TrigTime[2];
 gpstime*=640;
 gpstime+=((unsigned long long)(sec+(TrigTime[1]-1)))*ns;
 sec=gpstime/ns;
 nsec=gpstime%ns ;
 return 0;
}

void AMSEventR::UpdateGPS(){
if(getsetup()){
AMSSetupR::GPS gps=getsetup()->GetGPS(Run(),Event());
if(gps.Epoche.size()){
 if(gps.EventLast<Event() && gps.EventLast>0){
// simulate gps time not valid
   gps.Epoche[0]&=~(1<<21);

 } 
 fHeader.GPSTime.clear();
 for(unsigned int k=0;k<gps.Epoche.size();k++)fHeader.GPSTime.push_back(gps.Epoche[k]);
}
}
}

int HeaderR::GetGPSEpoche(unsigned int &sec, unsigned int & nsec){
 sec=0;
 nsec=0;
 int size=GPSTime.size();
 if(!size)return 1;
 unsigned int a=( (GPSTime[0]>>16) & 7);
 if(a!=1 && a!=4 && a!=7) return 2;
 else if (size<3) return 3;
 else if (!((GPSTime[0]>>16) & 32))return 4;
 sec=GPSTime[1];
 nsec=GPSTime[2];
 return 0;
}



int AMSEventR::GetGPSTime(unsigned int &sec, unsigned int & nsec){
sec=0;
nsec=0;
if(!fHeader.GPSTime.size()){
 UpdateGPS();
}
int ret=fHeader.GetGPSEpoche(sec,nsec);
if(ret)return ret;
if(pLevel1(0))return pLevel1(0)->GetGPSTime(sec,nsec);
else return 5; 

}


int HeaderR::getISSSA(float & alpha, float &b1a, float &b3a, float &b1b,float &b3b,float dt){
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSSA a;
double xtime=UTCTime()+dt;
int ret=AMSEventR::getsetup()->getISSSA(a,xtime);
alpha=a.alpha;
b1a=a.b1a;
b3a=a.b3a;
b1b=a.b1b;
b3b=a.b3b;
return ret;

}

int AMSEventR::CheckRunTag(unsigned int bit){
if(!getsetup())return 2;
return getsetup()->fHeader.CheckRTB(UTime(),bit)?1:0;
}
int HeaderR::getISSCTRS(float & r, float &theta, float &phi, float &v,float &vtheta,float &vphi,float dt){
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSCTRSR a;
double xtime=UTCTime()+dt;
int ret=AMSEventR::getsetup()->getISSCTRS(a,xtime);
r=a.r;
theta=a.theta;
phi=a.phi;
v=a.v;
vphi=a.vphi;
vtheta=a.vtheta;
return ret;

}



int HeaderR::getISSGTOD(float & r, float &theta, float &phi, float &v,float &vtheta,float &vphi,float dt){
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSGTOD a;
double xtime=UTCTime()+dt;
int ret=AMSEventR::getsetup()->getISSGTOD(a,xtime);
r=a.r;
theta=a.theta;
phi=a.phi;
v=a.v;
vphi=a.vphi;
vtheta=a.vtheta;
return ret;

}


int HeaderR::getGPSWGS84(float & r, float &theta, float &phi, float &v,float &vtheta,float &vphi,float dt){
if(!AMSEventR::getsetup())return 2;
AMSSetupR::GPSWGS84R a;
unsigned int sec,nsec;
int ok=AMSEventR::Head()->GetGPSTime(sec,nsec);
double xtime=sec+double(nsec)/1e9+dt;
if(!ok){
  static int nprint=0;
  if(nprint++<100)cerr<<"HeaderR::getGPSWGS84-GetGPSTimeRetuens "<<ok<<endl;
  xtime=UTCTime()+dt;
}
int ret=AMSEventR::getsetup()->getGPSWGS84(a,xtime);
r=a.r;
theta=a.theta;
phi=a.phi;
v=a.v;
vphi=a.vphi;
vtheta=a.vtheta;
return ret;

}


int HeaderR::getISSAtt(float & roll, float &pitch, float &yaw){
if(!AMSEventR::getsetup())return 2;
double xtime=UTCTime();
return AMSEventR::getsetup()->getISSAtt(roll,pitch,yaw,xtime);

}
int HeaderR::getISSAtt(){
if(!AMSEventR::getsetup())return 2;
double xtime=UTCTime();

return AMSEventR::getsetup()->getISSAtt(Roll,Pitch,Yaw,xtime);

}

int AMSEventR::IsBadRun(const char * reason){

string ss(reason);
if(!AMSEventR::getsetup())return 2;
if(AMSEventR::getsetup()->fHeader.Run!=Run()){
AMSEventR::getsetup()->fHeader.Run=Run();
}
return AMSEventR::getsetup()->IsBadRun(ss,UTime(),Run());

}


   ///<get solar beta angle via geometrical calculation
double HeaderR::getBetaSun(){
// SunPosition sunPos;
// unsigned int time,nanoTime;
// double Beta;
// if (HeaderR::GetGPSEpoche( time, nanoTime)!=0 ){
//   time=HeaderR::Time[0];
//   sunPos.setUTCTime((double)time);
//   }
// else{
// sunPos.setGPSTime((double)time);
// }
// sunPos.setISSGTOD( HeaderR::RadS, HeaderR::ThetaS, HeaderR::PhiS, HeaderR::VelTheta, HeaderR::VelPhi, HeaderR::Yaw,HeaderR::Pitch,HeaderR::Roll);
//Beta=sunPos.GetBetaAngle();
// return Beta;

 unsigned int time;
 float ialtitude=RadS; 
 float ilatitude=ThetaS; 
 float ilongitude=PhiS;
 float ivelAng=VelocityS; 
 float iveltheta=VelTheta; 
 float ivelphi=VelPhi;
 float iyaw=Yaw; 
 float ipitch=Pitch;
 float iroll=Roll;
 
 time=(double)UTCTime();
 AMSSetupR::GPSWGS84R gpsw;
 if (AMSEventR::getsetup() && !AMSEventR::getsetup()->getGPSWGS84(gpsw, time)) {
      ialtitude = gpsw.r; ilongitude = gpsw.phi; ilatitude = gpsw.theta;
      ivelAng = gpsw.v;   ivelphi = gpsw.vphi;   iveltheta = gpsw.vtheta;
 }

 SunPosition sunPos(time, ialtitude, ilatitude, ilongitude, ivelAng, iveltheta, ivelphi, iyaw, ipitch, iroll);

 double Beta=sunPos.GetBetaAngle();

 return Beta;
}
   ///<get sun position in AMS coordinate
int HeaderR::getSunAMS(double & azimut, double & elevation ){
// SunPosition sunPos;
// unsigned int time,nanoTime;
// int res;
// if (HeaderR::GetGPSEpoche( time, nanoTime)!=0 ){
//   time=HeaderR::Time[0];
//   sunPos.setUTCTime((double)time);
//   }
// else{
// sunPos.setGPSTime((double)time);
// }
// sunPos.setISSGTOD( HeaderR::RadS, HeaderR::ThetaS, HeaderR::PhiS, HeaderR::VelTheta, HeaderR::VelPhi, HeaderR::Yaw,HeaderR::Pitch,HeaderR::Roll); 
// if (!sunPos.GetSunFromAMS(elevation,azimut)) return -1;
// return sunPos.ISSday_night();
 unsigned int time;
 float ialtitude=RadS; 
 float ilatitude=ThetaS; 
 float ilongitude=PhiS;
 float ivelAng=VelocityS; 
 float iveltheta=VelTheta; 
 float ivelphi=VelPhi;
 float iyaw=Yaw; 
 float ipitch=Pitch;
 float iroll=Roll;
 
 time=(double)UTCTime();
 AMSSetupR::GPSWGS84R gpsw;
 if (AMSEventR::getsetup() && !AMSEventR::getsetup()->getGPSWGS84(gpsw, time)) {
      ialtitude = gpsw.r; ilongitude = gpsw.phi; ilatitude = gpsw.theta;
      ivelAng = gpsw.v;   ivelphi = gpsw.vphi;   iveltheta = gpsw.vtheta;
 }

 SunPosition sunPos(time, ialtitude, ilatitude, ilongitude, ivelAng, iveltheta, ivelphi, iyaw, ipitch, iroll);
 if (!sunPos.GetSunFromAMS(elevation,azimut)) return -1;
 return sunPos.ISSday_night();
}

//--------DSP Errors-----------------------
int HeaderR::getDSPError(AMSSetupR::DSPError& dsperr){
  
  if(!AMSEventR::getsetup()) return 2;
  
  return AMSEventR::getsetup()->getDSPError(dsperr, Time[0]);
}

AMSSetupR::DSPError* HeaderR::pDSPError(){
  static AMSSetupR::DSPError a;
#pragma omp threadprivate (a)
  if (!getDSPError(a))
    return 0;
  else
    return &a;
}
//-----------------------------------------
double HeaderR::GetThetaM(){

 double deg2rad = TMath::DegToRad();
        time_t Utime =UTCTime() ;
        //...km
        double Re =  6371.2; //km Earth radius
        double Altitude = RadS/1.e5-Re;
        //...ISS rad-->deg
        double ThetaISS=  ThetaS/deg2rad;
        double PhiISS=    PhiS/deg2rad;

        //--deg
        double thetaM = GM_GetThetaM(Utime, Altitude, ThetaISS, PhiISS);
        //--opposite sign and deg-->rad:
        thetaM= thetaM*(-1)*deg2rad;

return thetaM;

}
//-------------------------------------------------------------------------------------------
double HeaderR::GetPhiM(){

 double deg2rad = TMath::DegToRad();
        time_t Utime =UTCTime() ;
        //...km
        double Re =  6371.2; //km Earth radius
        double Altitude = RadS/1.e5-Re;
        //...ISS rad-->deg
        double ThetaISS=  ThetaS/deg2rad;
        double PhiISS=    PhiS/deg2rad;

        //--deg
        double phiM = GM_GetPhiM(Utime, Altitude, ThetaISS, PhiISS);
        //--opposite sign and deg-->rad
         phiM= phiM*(-1)*deg2rad;

return phiM;
}
//-----------Coordinates -------------------

int HeaderR::get_gal_coo(double & gal_long, double & gal_lat, double AMSTheta, double AMSPhi, double RPT[3],double VelPT[3], double YPR[3], double  time, bool gtod, bool gal_coo){
/*
input  AMSTheta (rad) in AMS coo system (from ParticleR)
       AMSPhi   (rad) in AMS coo system (from ParticleR)
       RPT  coordinates in GTOD/CTRS coordinate system (RPT[0]==Radius -> in cm; RPT[1]==Phi (rad); RPT[2]==Theta(rad)
       VelPT velocity in GTOD/CTRS coordinate system  (VelPT[0]= Angular Velocity; VelPT[1]=VelPhi  ; VelPT[2]=VelTheta )
       YPR yaw-pitch-roll in radians in LVLH
       gtod  true if gtod coo system
       time UTC time
output
           gal_long  galactic longitude (l) in degrees
           gal_lat   galactic latitude (b)  in degrees
return values
0  success
1...n  error (if any)
*/
 //Direction of incident particle in AMS coo - convert from spherical to cartesian
 AMSDir dir(AMSTheta,AMSPhi);
 double AMS_x=-dir[0];
 double AMS_y=-dir[1];
 double AMS_z=-dir[2];
 // use the conversion procedure described in FrameTrans.h
 if (gal_coo)
   get_ams_l_b_fromGTOD( AMS_x,  AMS_y, AMS_z,gal_long, gal_lat, RPT, VelPT, YPR, time);
 else
   get_ams_ra_dec_fromGTOD( AMS_x,  AMS_y, AMS_z,gal_long, gal_lat, RPT, VelPT, YPR, time);

return 0;

}


//-----------Backtracing -------------------
int HeaderR::do_backtracing(double &gal_long, double &gal_lat,
			    double &time_trace, double RPTO[3], double GPT[2],
			    double theta, double phi,
			    double momentum, double velocity, int charge,
			    double RPT[3], double VelPT[3], double YPR[3],
			    double  xtime, int iatt, bool galactic)
{
  // InitModel check
  if (GeoMagField::GetInitStat() == 0) GeoMagField::GetHead();
  if (GeoMagField::GetInitStat() <  0) return -1;

  if (charge == 0) return -1;

  double rgt = momentum/charge;
  GeoMagTrace gp = (iatt == 3)
    ? GeoMagTrace(RPT,        YPR, xtime, theta, phi, rgt, charge, velocity)
    : GeoMagTrace(RPT, VelPT, YPR, xtime, theta, phi, rgt, charge, velocity);

  int stat = gp.Propagate(gp.NmaxStep);

  time_trace = gp.GetTof();

  RPTO[0] = gp.GetRadi()*1e5;
  RPTO[1] = gp.GetLong (false);
  RPTO[2] = gp.GetLati (false);
  GPT [0] = gp.GetDlong(false);
  GPT [1] = gp.GetDlati(false);

  int ret = 2;
  if (stat == GeoMagTrace::ATMOS) ret = 0;
  if (stat == GeoMagTrace::SPACE) ret = 1;
  if (stat == GeoMagTrace::TRAPP) ret = 2;

  double x = gp.GetDx(), y = gp.GetDy(), z = gp.GetDz(), r = 1;
  FT_GTOD2Equat  (x, y, z, xtime-time_trace);
  FT_Cart2Angular(x, y, z, r, gal_lat, gal_long);
  if (galactic) FT_Equat2Gal(gal_long, gal_lat);
  gal_long *= 180./M_PI;
  gal_lat  *= 180./M_PI;

  return ret;
}
//---------------------------------Backtracing MIB
int HeaderR::backtracing_mib(  double &gal_long, double &gal_lat, double &time_trace,double RPTO[3], double GPT[2],
                               double theta,  double phi, double Momentum, int Charge,
                               double RPT[3], double VPT[3], double YPR[3], double UnixTime,
                                int att ,  bool gal_coo, int bt_method, int Amass){


        double output[8];
        //------------set to zero
        for(int i=0 ; i<8 ; i++ ){
                output[i]=0;
                }
        RPTO[0]=0;
        RPTO[1]=0;
        RPTO[2]=0;
        GPT[0]=0;
        GPT[1]=0;
        gal_long=0;
        gal_lat=0;
        time_trace=0;


        int ext = bt_method -1;

        //bt_method==1 --> ext==0 --> No external Field
        //bt_method==2 --> ext==1 --> External Field T96
        //bt_method==3 --> ext==2 --> External Field T05
        //---------------err for ext par:
        if( ext<0 || ext>=3  ){
        cerr <<" HeaderR::backtracing_mib Error: bt_method  must be 1==no external field ; 2== External field T96 ;  3== External field T05  "<<endl;
        output[0]=-4;
        return  -4;
        }
        //------>BackTracingMIB self pointer already Initialized in AMSEventR::InitDB()
        BackTracingMIB* ExternBT =  BackTracingMIB::GetfBT();



        //-----> check if time has changed:
        time_t time =(int) UnixTime;
        int load= ExternBT->CheckParameters( time ,ext);
        //---if ( == -3 )  Cannot update parameters : ext ==2 out of range 
        //---if ( == -2 )  Cannot update parameters : ext ==1 out of range 
        //---if ( == -1 )  Cannot update parameters : the file does not exists --->return -1;
        //---if ( ==  0 )  not necessary to update parameters (the time has not changed)
        //---if ( ==  1 )  the parameters were updated
        //---if ( ==  2 )  ext0==0 no need to update PAR

        if ( load< 0 ) {
        //cerr<< "HeaderR::backtracing_mib Error: "<< load <<endl;
        output[0]=load;
        return load;
        }

        double deg2rad= TMath::DegToRad();

        //... AMS  rad:--->      
        double  thetaAMS = theta;//colatitude of AMS out going
        double  phiAMS =  phi;

        //... GTOD rad:---> 
        double thetaGTOD;
        double phiGTOD;

        if(att==3){//----------->INTL attitude
         ExternBT->PartDirGTOD_INTL(thetaGTOD, phiGTOD , YPR, UnixTime,thetaAMS, phiAMS);
        }else{//---------------> LVLH attitude 1:GTOD, 2:CTRS
         ExternBT->PartDirGTOD_LVLH(thetaGTOD, phiGTOD , RPT,YPR, VPT, UnixTime,thetaAMS, phiAMS);
        }
        //... GTOD deg:---> 
        double  theta_deg =thetaGTOD/deg2rad ; // latitude [deg]
        double  phi_deg  = phiGTOD/deg2rad;   // longitude[deg] 


        //......Geographic coo ISS-----------------> 
                //ALTITUDE cm (GTOD coo sys)
                double r0= RPT[0];//      -->RadS
                double oneRt =6.3712e+08;//  cm
                // in unit of Earth radius
               r0/=oneRt;

                double fi0rad=  RPT[1];// --> PhiS;
                double the0rad= RPT[2];// --> ThetaS;
                //..from rad to deg:
                double the0=the0rad/deg2rad;
                double fi0=fi0rad/deg2rad;
                //----to be centered in 0!!
                //............................ZeroCentered 16/01/2013
                if(fi0 > 180)  fi0-=360.;

        //===============================================================
        double zn =(double) -1.*Charge ;// charge (opposite sign)
        double amass = (double)Amass;//mass number (0 for electrons)
        double rig =0;
        if(Charge!=0) rig = fabs(Momentum/Charge);
        if(Charge==0) return -1;

        //================================== Do backtracing =============================================
       //............Set commons: 
        BACKTRACINGPAR.r0=r0;
        BACKTRACINGPAR.the0=the0;
        BACKTRACINGPAR.fi0=fi0;
        BACKTRACINGPAR.the1=theta_deg;
        BACKTRACINGPAR.fi1=phi_deg;
        BACKTRACINGPAR.rig=rig;
        BACKTRACINGPAR.zn =zn;
        BACKTRACINGPAR.amass = amass;
        BACKTRACINGPAR.dflag =0 ;//------> 1 To force even if res=5 [to be used in future(?)]
        BACKTRACINGPAR.rkflag = 6;//-----> RUNGE -KUTTA


/*
        cout<< "Inside -----------------BACKTRACINGPAR.PAR----"<<endl;

       cout << BACKTRACINGPAR.PAR[0]<<" " <<BACKTRACINGPAR.PAR[1] <<" "
             << BACKTRACINGPAR.PAR[2]<<" " <<BACKTRACINGPAR.PAR[3] <<" "
             << BACKTRACINGPAR.PAR[4]<<" " <<BACKTRACINGPAR.PAR[5] <<" "
             << BACKTRACINGPAR.PAR[6]<<" " <<BACKTRACINGPAR.PAR[7] <<" "
             << BACKTRACINGPAR.PAR[8]<<" " <<BACKTRACINGPAR.PAR[9] <<" "
             << BACKTRACINGPAR.PAR[10]<<" "<<endl;
*/

        //------------------------------------------------------------------------------

        //...extern call-----------------> 
                ExternBT->CallExternBacktracing();

       //.....fill
        output[0]=BACKTRACINGPAR.res;
        output[1]=BACKTRACINGPAR.r;
        output[2]=BACKTRACINGPAR.td;
        output[3]=BACKTRACINGPAR.fei;
        output[4]=BACKTRACINGPAR.ast;
        output[5]=BACKTRACINGPAR.asf;
        output[6]=BACKTRACINGPAR.time;
        output[7]=BACKTRACINGPAR.alast;

/*
        cout << "BACKTRACING output: "<<endl;
        for(int i =0 ; i <8 ; i++){
        cout << output[i]<<endl;
*/

        //-----longitude Zero centered:
        if(output[3]>180) output[3]-=360;
        if(output[5]>180) output[5]-=360;

        int out=output[0];             //: --0 secondary particle, 1 primary particle, 2 time limit (set at 10 seconds) + ....
        RPTO[0]= output[1]* oneRt;     //: --final radius in Earth radi*OneRe --> [cm]
        RPTO[2]= output[2]* deg2rad;   //: --final latitude  [deg] (GTOD) * deg2rad -->[rad]
        RPTO[1]= output[3]* deg2rad;   //: --final longitude [deg] (GTOD) * deg2rad -->[rad]
        GPT[1]=  output[4]* deg2rad;   //: --asymptotic latitude final pont  [deg] (GTOD)* deg2rad -->[rad]
        GPT[0]=  output[5]* deg2rad;   //: --asymptotic longitude final point [deg] (GTOD)* deg2rad -->[rad]
        time_trace= output[6];         //: --time spent by the particle in trajectory (in sec)
        // output[7]: --length of trajectory in Earth radii

        //----------------Galactic or Equatorial particle Directions
        double rIn = 1;
        double thetaIn=GPT[1] ;
        double phiIn =GPT[0] ;

        double lOut ,bOut;
        double timeIn=time-time_trace;
         ExternBT->FromGTODtoGAL(rIn,thetaIn,phiIn,timeIn,lOut,bOut,gal_coo);

        //------> final :
        gal_long =lOut/deg2rad;
        gal_lat= bOut/deg2rad;



return out;


}
//-------------------------------------------------------------------------



int HeaderR::GetIGRFCutoff(double &Rcut, int sign, AMSDir dir, 
			   double RPT[3], double VPT[3], double YPR[3],
			   double xtime)
{
  double ths = RPT[2]*TMath::RadToDeg();
  double phs = RPT[1]*TMath::RadToDeg();
  double alt = RPT[0]*1e-5-6371.2;
  if (phs > 180) phs -= 360;

  double th, ph;
  get_gtod_coo(ph, th, dir.gettheta(), dir.getphi(), RPT, VPT, YPR, xtime);

  int      pos = (sign > 0) ? 1 : 0;
  double  scut = GeoMagCutoff(xtime, alt, th, ph, ths, phs, pos);

  double rstep = 0.05;
  double  rcut = fabs(scut)*1.15;
  double  bcut =  0;
  int     bret = -1;

  GeoMagTrace::UTime = (int)xtime;
  GeoMagField::fModelYear = 2012+(xtime-GeoMagTrace::T2012)/3600/24/365;

  while (0.1 < rcut && rcut < 50) {
    double glon, glat, rpto[3], gpt[2], tim;
    int ret = do_backtracing(glon, glat, tim, rpto, gpt,
			     dir.gettheta(), dir.getphi(), rcut, 1, sign,
			     RPT, VPT, YPR, xtime);
    if (bret >= 0) {
      if ((bret == 1 && ret != 1) || (bret != 1 && ret == 1)) {
	if (rstep > 0.02) rstep = 0.02;
	else {
	  if (bret == 1 && ret != 1) { Rcut = bcut*sign; return 0; }
	  if (bret != 1 && ret == 1) { Rcut = rcut*sign; return 0; }
	}
      }
    }
    bcut = rcut;
    bret = ret;

    if (ret == 1) rcut *= 1-rstep;
    if (ret != 1) rcut *= 1+rstep;
  }

  return -1;
}

int HeaderR::GetMaxIGRFCutoff(double fov, double degbin, double *cutoff,
			      double RPT[3], double VPT[3], double YPR[3],
			      double xtime)
{
  cutoff[0] = cutoff[1] = 0;

  int mode = 1;
  if (degbin < 0) { mode = 2; degbin = -degbin; }

  double *cf = (mode == 2) ? cutoff : 0;
  int ii[3] = { 0, 0, 0 };
  if (mode == 2) { ii[0] = 25/degbin; ii[1] = 30/degbin; ii[2] = 35/degbin; }

  double tmin = 0, tmax = fov*TMath::DegToRad();
  double pmin = 0, pmax = TMath::TwoPi();

  double dth = degbin*TMath::DegToRad();

  int nth = (int)((tmax-tmin)/dth)+1;

  for (int i = 0; i < nth; i++) {
    double th  = tmin+dth*i;
    double dph = (i > 0) ? dth/TMath::Sin(th) : 1e6;
    int    nph = (int)((pmax-pmin)/dph)+1;

    if (mode == 2) {
      if (i == ii[0]+1) { cutoff = &cf[2]; cf[2] = cf[0]; cf[3] = cf[1]; }
      if (i == ii[1]+1) { cutoff = &cf[4]; cf[4] = cf[2]; cf[5] = cf[3]; }
      if (i == ii[2]+1) { cutoff = &cf[6]; cf[6] = cf[4]; cf[7] = cf[5]; }
    }
  
    for (int j = 0; j < nph; j++) {
      double ph = pmin+dph*j;

      AMSDir dir(th, ph);
      dir = dir*(-1);

      double scut[2];
      GetIGRFCutoff(scut[0], -1, dir, RPT, VPT, YPR, xtime);
      GetIGRFCutoff(scut[1],  1, dir, RPT, VPT, YPR, xtime);

      if (fabs(scut[0]) > fabs(cutoff[0])) cutoff[0] = scut[0];
      if (fabs(scut[1]) > fabs(cutoff[1])) cutoff[1] = scut[1];
    }
  }
  return 0;
}


int HeaderR::get_gtod_coo(double & gtod_long, double & gtod_lat, double AMSTheta, double AMSPhi, double RPT[3],double VelPT[3], double YPR[3], double  time, bool gtod){
/*
input  AMSTheta (rad) in AMS coo system (from ParticleR)
       AMSPhi   (rad) in AMS coo system (from ParticleR)
       RPT  coordinates in GTOD/CTRS coordinate system (RPT[0]==Radius -> in cm; RPT[1]==Phi (rad); RPT[2]==Theta(rad)
       VelPT velocity in GTOD/CTRS coordinate system   (VelPT[0]= Angular Velocity; VelPT[1]=VelPhi  ; VelPT[2]=VelTheta )
       YPR yaw-pitch-roll in radians in LVLH
       gtod  true if gtod coo system
       time UTC time
output
           gtod_long , phi   in degrees
           gtod_lat  , theta in degrees
return values
0  success
1...n  error (if any)
*/
 //Direction of incident particle in AMS coo - convert from spherical to cartesian
 AMSDir dir(AMSTheta,AMSPhi);
 double AMS_x=-dir[0];
 double AMS_y=-dir[1];
 double AMS_z=-dir[2];
 // use the conversion procedure described in FrameTrans.h
 get_ams_gtod_fromGTOD( AMS_x,  AMS_y, AMS_z,gtod_lat, gtod_long, RPT, VelPT, YPR, time);

return 0;

}



int HeaderR::get_gal_coo(double & gal_long, double & gal_lat, double  AMSTheta , double AMSPhi, int CamID, double CAM_RA, double CAM_DEC, double CAM_Orient, bool gal_coo){
/*
input   AMSTheta (rad) in AMS coo system (from ParticleR)
        AMSPhi   (rad) in AMS coo system (from ParticleR)
        CamID   camera id
        CAM_RA   camera right ascension (degrees)
        CAM_DEC   camera declination (degrees)
        CAM_Orient   camera orientation (degrees)
        time UTC time
output
        gal_long  galactic longitude (l) in degrees
        gal_lat   galactic latitude (b)  in degrees
return values
0  success
1...n  error (if any)
*/
 //Direction of incident particle in AMS coo - convert from spherical to cartesian
 AMSDir dir(AMSTheta,AMSPhi);
 double AMS_x=-dir[0];
 double AMS_y=-dir[1];
 double AMS_z=-dir[2];
 // use the conversion procedure described in FrameTrans.h
 if (gal_coo)
   get_ams_l_b_from_StarTracker(AMS_x,  AMS_y, AMS_z, gal_long, gal_lat,  CamID, CAM_RA, CAM_DEC, CAM_Orient);
 else
   get_ams_ra_dec_from_StarTracker(AMS_x,  AMS_y, AMS_z, gal_long, gal_lat,  CamID, CAM_RA, CAM_DEC, CAM_Orient);

return 0;
}


int HeaderR::get_gal_coo(double & gal_long, double & gal_lat, double AMSTheta, double AMSPhi, double YPR[3], double time, bool gal_coo)
{
// Get galactic coordinates using YPR attitude with respect to J2000
/*
input  AMSTheta (rad) in AMS coo system (from ParticleR)
       AMSPhi   (rad) in AMS coo system (from ParticleR)
       YPR yaw-pitch-roll in radians in INTL
       time UTC time
output
           gal_long  galactic longitude (l) in degrees
           gal_lat   galactic latitude (b)  in degrees
return values
0  success
1...n  error (if any)
*/
 //Direction of incident particle in AMS coo - convert from spherical to cartesian
 AMSDir dir(AMSTheta,AMSPhi);
 double AMS_x=-dir[0];
 double AMS_y=-dir[1];
 double AMS_z=-dir[2];
 // use the conversion procedure described in FrameTrans.h
 double ra=0, dec=0;
 get_ams_ra_dec_from_ALTEC_INTL(AMS_x,  AMS_y, AMS_z, ra, dec,YPR[0]*180./3.1415926, YPR[1]*180./3.1415926, YPR[2]*180./3.1415926);
 if (gal_coo) get_gal_coo(gal_long, gal_lat, ra, dec);
 else { gal_long = ra; gal_lat = dec; }
 return 0;
}

int HeaderR::get_gal_coo(double & gal_long, double & gal_lat,double ams_ra, double ams_dec){
  ams_ra=ams_ra/180.*3.1415926;
  ams_dec=ams_dec/180.*3.1415926;
  FT_Equat2Gal(ams_ra, ams_dec);
  gal_long=ams_ra*180./3.1415926;
  gal_lat=ams_dec*180./3.1415926;
return 0;
}

double AMSEventR::get_coo_diff(double RPT[3],
			       double r, double theta, double phi)
{
  AMSPoint dc1(RPT[0]*cos(RPT[2])*cos(RPT[1]),
	       RPT[0]*cos(RPT[2])*sin(RPT[1]), RPT[0]*sin(RPT[2]));
  AMSPoint dc2(r*cos(theta)*cos(phi),
	       r*cos(theta)*sin(phi), r*sin(theta));
  AMSPoint da = dc2-dc1;
  return sqrt(da.prod(da));
}

int AMSEventR::GetGalCoo(int &result, double &glong, double &glat, 
			 double  theta, double   phi,
			 int use_att, int use_coo, int use_time,
			 double   dt, int out_type)
  /*!
  \brief Convert direction in AMS coordinates into Galactic coordinates

  \param[out] result bits (1<<0 =  1): STK used, (1<<1 = 2):INTL used,
			  (1<<2 =  4): TLE used, (1<<3 = 8):GTOD used,
			  (1<<4 = 16):CTRS used, (1<<5 =32):AMS-GPS coo.used,
			  (1<<6 = 64):GPS time used,
			  (1<<7 =128):GPS time corrected used
  \param[out] glong       Galactic longitude (degree)
  \param[out] glat        Galactic latitude  (degree)

  \param[in]  theta (rad) in ams coo system (pi: down-going 0: up-going)
  \param[in]  phi   (rad) in ams coo system
  \param[in]  use_att     1:Use LVLH,  2:Use INTL, 3: Use STK
  \param[in]  use_coo     1:Use TLE,   2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS
  \param[in]  use_time    1:UTCTime(), 2:AMSGPS time, 3:AMSGPS Time corrected
  \param[in]  dt          time jitter (sec) for coordinates input
  \param[in]  out_type    1:Galactic coord. 2:Equatorial coord.(R.A. and Dec.)
                          3:GTOD coord.(use_att forced to 1)

  \retval     0 success
  \retval    -1 failure
  \retval     1 specified use_att  data not available; instead used TLE+LVLH
  \retval     2 specified use_coo  data not available; instead used TLE
  \retval     3 specified use_coo  data not reliable;  instead used TLE
  \retval     4 specified use_time data not available; instead used UTCTime()
  */
{
  enum { bSTK  = 0, bINTL = 1, bTLE  = 2, bGTOD = 3, bCTRS = 4, bGPSW = 5,
	 bGPST = 6, bGPSC = 7 };

  enum { rNAtt = 1, rNCoo = 2, rBCoo = 3, rNTim = 4 };

  enum { uLVLH = 1, uINTL = 2, uSTK  = 3,
	 uTLE  = 1, uCTRS = 2, uGTOD = 3, uGPSW = 4,
	 uUTCT = 1, uGPST = 2, uGPSC = 3 };

  int ret = result = 0;

  bool gal_coo = (out_type == 1) ? true : false;
  if (out_type == 3) use_att = uLVLH;

  double xtime = fHeader.UTCTime();

  // Check and use AMS GPS Time if OK
  if (use_time == uGPST) {
    unsigned int time,nanotime;
    if (GetGPSTime(time, nanotime)) ret = rNTim;
    else {
      xtime = double(time)+double(nanotime)/1.e9-AMSEventR::gpsdiff(time);
      result |= (1<<bGPST);
    }
  }

  // Check and use AMS GPS Time if OK
  else if (use_time == uGPSC){
    double err;
    if (UTCTime(xtime, err)) ret = rNTim;
    else result |= (1<<bGPSC);
  }

  // Check and use AMS STK if OK
  if (use_att == uSTK) {
    AMSSetupR::AMSSTK stk;
    if (getsetup() && !getsetup()->getAMSSTK(stk, xtime+dt)) {
      result |= (1<<bSTK);
      int ret2 = fHeader.get_gal_coo(glong, glat, theta, phi, stk.cam_id,
				     stk.cam_ra, stk.cam_dec, stk.cam_or,
				     gal_coo);
      return (ret2 == 0) ? ret : ret2;
    }
    else ret = rNAtt;
  }

  // Check and use INTL if OK
  else if (use_att == uINTL) {
    float Roll, Pitch, Yaw;
    if (getsetup() && !getsetup()->getISSINTL(Roll, Pitch, Yaw, xtime+dt)) {
      double YPR[3];
      YPR[0] = Yaw;
      YPR[1] = Pitch;
      YPR[2] = Roll;
      result |= (1<<bINTL);
      int ret2 = fHeader.get_gal_coo(glong, glat, theta, phi,
				     YPR, xtime, gal_coo);
      return (ret2 == 0) ? ret : ret2;
    }
    else ret = rNAtt;
  }

  double YPR[3] = { fHeader.Yaw,       fHeader.Pitch,  fHeader.Roll };
  double RPT[3] = { fHeader.RadS,      fHeader.PhiS,   fHeader.ThetaS };
  double VPT[3] = { fHeader.VelocityS, fHeader.VelPhi, fHeader.VelTheta };
    float RTP[3], VTP[3];
    if (getsetup() &&AMSEventR::getsetup()->getISSTLE(RTP, VTP, xtime) == 0) {
      RPT[0] = RTP[0];
      RPT[1] = RTP[2];
      RPT[2] = RTP[1];
      VPT[0] = VTP[0];
      VPT[1] = VTP[2];
      VPT[2] = VTP[1];
    }
   
  bool gtod = true;

  float Roll, Pitch, Yaw;
  if (getsetup() && !getsetup()->getISSAtt(Roll,Pitch,Yaw,xtime)){
    YPR[0] = Yaw;
    YPR[1] = Pitch;
    YPR[2] = Roll;
  }

  static int mprint = 0;
  double prec = 80e5;

  // Check and use GTOD if OK
  if (use_coo == uGTOD) {
    AMSSetupR::ISSGTOD gtod;
    if (getsetup() && !getsetup()->getISSGTOD(gtod, xtime+dt)) {
      double diff = get_coo_diff(RPT, gtod.r, gtod.theta, gtod.phi);
      if (diff < prec) {
	RPT[0] = gtod.r; RPT[1] = gtod.phi;  RPT[2] = gtod.theta;
	VPT[0] = gtod.v; VPT[1] = gtod.vphi; VPT[2] = gtod.vtheta;
	result |= (1<<bGTOD);
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use CTRS if OK
  else if (use_coo == uCTRS) {
    AMSSetupR::ISSCTRSR ctrs; 
    if (getsetup() && !getsetup()->getISSCTRS(ctrs, xtime+dt)) {
      double diff = get_coo_diff(RPT, ctrs.r, ctrs.theta, ctrs.phi);
      if (diff < prec) {
	RPT[0] = ctrs.r; RPT[1] = ctrs.phi;  RPT[2] = ctrs.theta;
	VPT[0] = ctrs.v; VPT[1] = ctrs.vphi; VPT[2] = ctrs.vtheta;
	result |= (1<<bCTRS);
	gtod = false;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use GPS if OK
  else if (use_coo == uGPSW) {
    AMSSetupR::GPSWGS84R gpsw;
    if (getsetup() && !getsetup()->getGPSWGS84(gpsw, xtime+dt)) {
      double diff = get_coo_diff(RPT, gpsw.r, gpsw.theta, gpsw.phi);
      if (diff < prec) {
	RPT[0] = gpsw.r; RPT[1] = gpsw.phi;  RPT[2] = gpsw.theta;
	VPT[0] = gpsw.v; VPT[1] = gpsw.vphi; VPT[2] = gpsw.vtheta;
	result |= (1<<bGPSW);
	gtod = false;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }
  else result |= (1<<bTLE);

  int ret2 = (out_type == 3)
    ? fHeader.get_gtod_coo(glong, glat, theta, phi, RPT, VPT, YPR, xtime, gtod)
    : fHeader.get_gal_coo (glong, glat, theta, phi, RPT, VPT, YPR, xtime, gtod,
			   gal_coo);
  return (ret2 == 0) ? ret : ret2;
}

int AMSEventR::DoBacktracing(int &result, int &status,
			     double &glong, double &glat,
			     double RPTO[3], double &TraceTime,
			     double theta, double phi,
			     double Momentum, double Velocity, int Charge, 
			     int bt_method, int Amass,
			     int use_att, int use_coo, int use_time,
			     double   dt, int out_type)
  /*!
  \brief Back trace charged particle into Galactic coordinates

  \param[out] result bits (1<<0 =  1): STK used, (1<<1 = 2):INTL used,
			  (1<<2 =  4): TLE used, (1<<3 = 8):GTOD used,
			  (1<<4 = 16):CTRS used, (1<<5 =32):AMS-GPS coo.used,
			  (1<<6 = 64):GPS time used,
			  (1<<7 =128):GPS time corrected used
  \param[out] status      0: errors (see retuned value)
                          1: Over cutoff, 
                          2: Under cutoff, 
                          3: Trapped 
                          4: Calculated velocity greater than c
                          5: External field model parameters out of validity range
                          6: External field model Parameters do not exist
                          7: All other cases where the program was exiting (max number of steps reached..)
  \param[out] glong       Galactic longitude (degree)
  \param[out] glat        Galactic latitude  (degree)
  \param[out] RPTO        GTOD coordinates Rad(cm), Phi(rad), Theta(rad)

  \param[in]  theta (rad) in ams coo system (pi: down-going 0: up-going)
  \param[in]  phi   (rad) in ams coo system
  \param[in]  Momentum    (GeV/c) Not signed
  \param[in]  Velocity    (=beta) Not signed
  \param[in]  Charge      Signed charge
  \param[in]  bt_method   0: Haino's  
                          1:MIB No ext.Field  
                          2:MIB ext.Field Tsyganenko 1996 (T96)  
                          3:MIB ext.Field Tsyganenko 2005 (T05) 
  \param[in]  Amass       atomic mass number(0:  e- , e+ ; 1:  p, anti-p ; 2: d , anti-d ; 3: He3...) 
  \param[in]  use_att     1:Use LVLH,  2:Use INTL, 3: Use STK
  \param[in]  use_coo     1:Use TLE,   2:Use CTRS, 3: Use GTOD, 4: Use AMS-GPS
  \param[in]  use_time    1:UTCTime(), 2:AMSGPS time, 3:AMSGPS Time corrected
  \param[in]  dt          time jitter (sec) for coordinates input
  \param[in]  out_type    1:Galactic coord. 2:Equatorial coord.(R.A. and Dec.)
                          3:GTOD coord.(use_att forced to 1)

  \retval     0 success
  \retval    -1 failure
  \retval    -2 error (if bt_method==2  particle UTime out of time limits for parameter file T96)
  \retval    -3 error (if bt_method==3  particle UTime out of time limits for parameter file T05)
  \retval    -4 error (if bt_method!=0) bt_method must be 1 || 2 || 3

  \retval     1 specified use_att  data not available; instead used TLE+LVLH
  \retval     2 specified use_coo  data not available; instead used TLE
  \retval     3 specified use_coo  data not reliable;  instead used TLE
  \retval     4 specified use_time data not available; instead used UTCTime()
  */
{
  enum { bSTK  = 0, bINTL = 1, bTLE  = 2, bGTOD = 3, bCTRS = 4, bGPSW = 5,
	 bGPST = 6, bGPSC = 7 };

  enum { rNAtt = 1, rNCoo = 2, rBCoo = 3, rNTim = 4 };

  enum { uLVLH = 1, uINTL = 2, uSTK  = 3,
	 uTLE  = 1, uCTRS = 2, uGTOD = 3, uGPSW = 4,
	 uUTCT = 1, uGPST = 2, uGPSC = 3 };

  int ret = result = 0;

  bool gal_coo = (out_type == 2) ? false : true;
  if (out_type == 3) use_att = uLVLH;

  double xtime = fHeader.UTCTime();

  // Check and use AMS GPS TIme if OK
  if (use_time == uGPST) {
    unsigned int time,nanotime;
    if (GetGPSTime(time, nanotime)) ret = rNTim;
    else {
      xtime = double(time)+double(nanotime)/1.e9-AMSEventR::gpsdiff(time);
      result |= (1<<bGPST);
    }
  }
  // Check and use AMS GPS Time if OK
  else if (use_time == uGPSC){
    double err;
    if (UTCTime(xtime, err)) ret = rNTim;
    else result |= (1<<bGPSC);
  }

  double YPR[3] = { fHeader.Yaw,       fHeader.Pitch,  fHeader.Roll };
  double RPT[3] = { fHeader.RadS,      fHeader.PhiS,   fHeader.ThetaS };
  double VPT[3] = { fHeader.VelocityS, fHeader.VelPhi, fHeader.VelTheta };

  int iatt = 1; // GTOD

  float Roll, Pitch, Yaw;
  if (getsetup() && !getsetup()->getISSAtt(Roll,Pitch,Yaw,xtime)){
    YPR[0] = Yaw;
    YPR[1] = Pitch;
    YPR[2] = Roll;
  }

  static int mprint = 0;
  double prec = 80e5;

  // Check and use GTOD if OK
  if (use_coo == uGTOD) {
    AMSSetupR::ISSGTOD gtod;
    if (getsetup() && !getsetup()->getISSGTOD(gtod, xtime+dt)) {
      double diff = get_coo_diff(RPT, gtod.r, gtod.theta, gtod.phi);
      if (diff < prec) {
	RPT[0] = gtod.r; RPT[1] = gtod.phi;  RPT[2] = gtod.theta;
	VPT[0] = gtod.v; VPT[1] = gtod.vphi; VPT[2] = gtod.vtheta;
	result |= (1<<bGTOD);
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use CTRS if OK
  else if (use_coo == uCTRS) {
    AMSSetupR::ISSCTRSR ctrs; 
    if (getsetup() && !getsetup()->getISSCTRS(ctrs, xtime+dt)) {
      double diff = get_coo_diff(RPT, ctrs.r, ctrs.theta, ctrs.phi);
      if (diff < prec) {
	RPT[0] = ctrs.r; RPT[1] = ctrs.phi;  RPT[2] = ctrs.theta;
	VPT[0] = ctrs.v; VPT[1] = ctrs.vphi; VPT[2] = ctrs.vtheta;
	result |= (1<<bCTRS);
	iatt = 2;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }

  // Check and use GPS if OK
  else if (use_coo == uGPSW) {
    AMSSetupR::GPSWGS84R gpsw;
    if (getsetup() && !getsetup()->getGPSWGS84(gpsw, xtime+dt)) {
      double diff = get_coo_diff(RPT, gpsw.r, gpsw.theta, gpsw.phi);
      if (diff < prec) {
	RPT[0] = gpsw.r; RPT[1] = gpsw.phi;  RPT[2] = gpsw.theta;
	VPT[0] = gpsw.v; VPT[1] = gpsw.vphi; VPT[2] = gpsw.vtheta;
	result |= (1<<bGPSW);
	iatt = 2;
      }
      else {
	if (mprint++ <= 10) {
	  cerr << "AMSEventR::GetGalCoo-E-GTODCooTooDistantFrom2Ele "
	       << diff << " km";
	  if (mprint == 10) cerr << " (LastMessage)";
	  cerr << endl;
	}
	result |= (1<<bTLE);
	ret = 3;
      }
    }
    else {
      result |= (1<<bTLE);
      ret = 2;
    }
  }
  else result |= (1<<bTLE);

  // STK is not implemented
  if (use_att == uSTK) ret = rNAtt;

  // Check and use INTL if OK
  else if (use_att == uINTL) {
    float Roll, Pitch, Yaw;
    if (getsetup() && !getsetup()->getISSINTL(Roll, Pitch, Yaw, xtime+dt)) {
      YPR[0] = Yaw;
      YPR[1] = Pitch;
      YPR[2] = Roll;
      result |= (1<<bINTL);
      iatt = 3;
    }
    else ret = rNAtt;
  }

  double GPT[2];

#ifndef __ROOTSHAREDLIBRARY__
  int ret2 = fHeader.do_backtracing(glong, glat, TraceTime, RPTO, GPT,
				    theta, phi, Momentum, Velocity, Charge,
				    RPT, VPT, YPR, xtime, iatt, gal_coo);

#else
  int ret2 = bt_method ==0?fHeader.do_backtracing(glong, glat, TraceTime, RPTO, GPT,
                                    theta, phi, Momentum, Velocity, Charge,
                                    RPT, VPT, YPR, xtime, iatt, gal_coo):
                          fHeader.backtracing_mib(glong, glat, TraceTime, RPTO, GPT,
                                    theta, phi, Momentum, Charge,
                                    RPT, VPT, YPR, xtime , iatt, gal_coo,
                                    bt_method, Amass);
#endif





  if (out_type == 3) {
    glong = GPT[0]*180./M_PI;
    glat  = GPT[1]*180./M_PI;
  }

  if      (ret2 == 1) status = 1;  // Over  cutoff
  else if (ret2 == 0) status = 2;  // Under cutoff
  else if (ret2 == 2) status = 3;  // Trapped

  else if (ret2 == 3) status = ret2+1;  // 4 calculated velocity greater than c
  else if (ret2 == 4) status = ret2+1;  // 5 External field model parameters out of validity range
  else if (ret2 == 5) status = ret2+1;  // 6 External field model Parameters do not exist
  else if (ret2 == 6) status = ret2+1;  // 7 all other cases where the program was exiting (max number of steps reached..)
  else if (ret2 <  0) status = 0;       //----> errors!
  return  (ret2 >= 0) ? ret : ret2;

}


//------------------------------------------



float AMSEventR::LiveTime(unsigned int ut){

if(getsetup()){
int k=0;
 if(!ut)k=getsetup()->getScalers(fHeader.Time[0],fHeader.Time[1]);
else k=getsetup()->getScalers(ut,0);
 if(!k)return -1;
 else if(getsetup()->fScalersReturn.size()==1 ){
//  cout <<"one "<<getsetup()->fScalersReturn[0]->second._LiveTime[0]<<endl;
   return getsetup()->fScalersReturn[0]->second._LiveTime[0];
}
 else if (getsetup()->fScalersReturn.size()==2){
  float s0[2]={-1.,-1};
  double tme[2]={0,0};
  for(int i=0;i<2;i++){
   s0[i]=getsetup()->fScalersReturn[i]->second._LiveTime[0];
   unsigned long long t=getsetup()->fScalersReturn[i]->first;
   
  unsigned long long mask=1;
//   cout << i <<" "<<(t>>32)<<endl;
   mask=(mask<<32)-1;
   tme[i]=double(t>>32)+double(t& mask)/1000000.;
  }
  double ct=double(UTime())+Frac(); 
  //cout <<" utime "<<UTime()<<" "<<Frac()<<" "<<ct-UTime()<<endl;
  float lt=s0[0]+(ct-tme[0])/(tme[1]-tme[0]+1.e-6)*(s0[1]-s0[0]);
 // cout <<tme[0]<<" "<<tme[1]<<" "<<ct<<" "<<tme[1]-tme[0]<<" "<<ct-tme[0]<<endl;
  //cout <<"two "<<s0[0]<<" "<<s0[1]<<" "<<lt<<endl;
  return lt;
}
 else{
   cerr<<" AMSEventR::Livetime-E-LogicError size "<<getsetup()->fScalersReturn.size()<<"  ret "<<k<<endl;
   return -1;
}
}

else return -1;

}


void  Level1R::RestorePhysBPat(int mg) {

 if( mg || PhysBPatt==0){

  unsigned short int ft_pattern=JMembPatt;

  if ( (ft_pattern & 0x0001) == 0x0001 ) PhysBPatt |= 0x0001; // unbiased 3/4 TOF-CP
  if ( (ft_pattern & 0x0090) == 0x0090 ) PhysBPatt |= 0x0002; // 4/4 TOF-CT & ACC0
  if ( (ft_pattern & 0x0300) == 0x0300 ) PhysBPatt |= 0x0004; // 4/4 TOF-BZ & ACC1
  if ( (ft_pattern & 0x0020) == 0x0020 ) PhysBPatt |= 0x0008; // 4/4 FTZ
  if ( (ft_pattern & 0x0410) == 0x0410 ) PhysBPatt |= 0x0010; // 4/4 TOF-CT & ECALF&
  if ( (ft_pattern & 0x1000) == 0x1000 ) PhysBPatt |= 0x0020; // ECALA&
  if ( (ft_pattern & 0x0800) == 0x0800 ) PhysBPatt |= 0x0040; // unbiased ECALA||

}
}

int Level1R::RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt){
//returns: two new trig-patterns and flag: 0/1 --> "Not"/"Was" PhysTrig
  L1TrMemPatt=0;
  PhysTrPatt=0;
  int tofpattft[4]={0,0,0,0};
  int AccPatt(0),NAccs(0);
  int TOFTrigFl1(-1),TOFTrigFl2(-1),ECTrigFl(-1);
  bool FTZ(0);
  
  int PhysTrSet[8]={0,0,0,0,0,0,0,0};
  PhysTrSet[0]=(1<<1);//ch_unb:FTCP0
  PhysTrSet[1]=(1<<4)+(1<<7);//singl_charged(not el):FTCT1&ACC0
  PhysTrSet[2]=(1<<8)+(1<<9);//ions:ACC1&BZ
  PhysTrSet[3]=(1<<5);//sl_ions:FTZ
  PhysTrSet[4]=(1<<4)+(1<<10);//el:FTCT1&EC-F_and
  PhysTrSet[5]=(1<<12);//ph:EC-A_and
  PhysTrSet[6]=(1<<11);//unbEC:EC-A_or
  
//------> get current trig. parameters:
  for(int ii=0;ii<4;ii++){
    tofpattft[ii]=TofPatt1[ii];
  }
//  FTZ=((TrigPatt&(1<<5))!=0);//SlowIon setting is disabled to match with Data 
  AccPatt=AntiPatt;
  TOFTrigFl1=TofFlag1;
  TOFTrigFl2=TofFlag2;//bz
  ECTrigFl=EcalFlag;//MN, M=0/1/2/3->noFTE/noFTE(1prj@2requested)/FTEor/FTEand, 
//                                N=0/1/2/3-> /undef/noLev1/Lev1or/Lev1and
  bool CentrOK=((tofpattft[2]&0x1FE01FEL)>0);//have any central counter in Lay-3 trig.patt
//-----------------------------------------
//------>FTC,FTCP0,FTCP1 rebuild:
//
  if(TOFTrigFl1<=4 && TOFTrigFl1>=0){
    L1TrMemPatt|=(1<<1);// set FTCP0(>=3of4)
    L1TrMemPatt|=1;// set FTC
    if(CentrOK)L1TrMemPatt|=(1<<3);// set FTCT0(not used in TrigDecision but let it be seen)
  }
  if(TOFTrigFl1==0)L1TrMemPatt|=(1<<2);// set FTCP1(4of4)
//-----------------------------------------
//------>FTCT1 rebuild:    
//
  if(TOFTrigFl1==0 && CentrOK){//was tof 4of4
    L1TrMemPatt|=(1<<4);// set FTCT1 
  }
//-----------------------------------------
//------> ACC0/1 rebuild:
//
  for(int i=0;i<8;i++)if((AccPatt & (1<<i))>0)NAccs+=1;//count nsectors       
  if(NAccs<1)L1TrMemPatt|=(1<<7);// set ACC0
  if(NAccs<5)L1TrMemPatt|=(1<<8);// set ACC1
//-----------------------------------------
//------> FTZ,BZ-bits rebuild(no need to exclude out.c in L3):
//
  if(FTZ)L1TrMemPatt|=(1<<5);// set FTZ as it was
  if(TOFTrigFl2==0)L1TrMemPatt|=(1<<9);// set BZ "4of4"
//-----------------------------------------
//------> EC-bits rebuild:
//
  if((ECTrigFl/10)==3){
    L1TrMemPatt|=(1<<10);// set EC-F_and
    L1TrMemPatt|=(1<<11);// set EC-F_or
  }  
  else if((ECTrigFl/10==1) || (ECTrigFl/10==2))L1TrMemPatt|=(1<<11);// set EC-F_or
  if(ECTrigFl%10>=2){
    L1TrMemPatt|=(1<<13);// set EC-A_or (not used in TrigDecision but let it be seen)
    if(ECTrigFl%10==3)L1TrMemPatt|=(1<<12);// set EC-A_and
  }  
  if((L1TrMemPatt&(1<<11))>0)L1TrMemPatt|=(1<<6);// set FTE (EC-F_or is required)
//-----------------------------------------
//------>rebuild PhysTrigPatt:
//
  for(int i=0;i<7;i++){
    if((L1TrMemPatt&PhysTrSet[i])==PhysTrSet[i])PhysTrPatt|=(1<<i);
  }
//-----------------------------------------
  if((PhysTrPatt&0x3EL)!=0)return 1;
  else return 0;
}
//---
int Level1R::RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt, int &AccSectPatt){
//returns: two new trig-patterns and flag: 0/1 --> "Not"/"Was" PhysTrig
  L1TrMemPatt=0;
  PhysTrPatt=0;
  int tofpattft[4]={0,0,0,0};
  int AccPatt(0),NAccs(0);
  int TOFTrigFl1(-1),TOFTrigFl2(-1),ECTrigFl(-1);
  bool FTZ(0);
  
  int PhysTrSet[8]={0,0,0,0,0,0,0,0};
  PhysTrSet[0]=(1<<1);//ch_unb:FTCP0
  PhysTrSet[1]=(1<<4)+(1<<7);//singl_charged(not el):FTCT1&ACC0
  PhysTrSet[2]=(1<<8)+(1<<9);//ions:ACC1&BZ
  PhysTrSet[3]=(1<<5);//sl_ions:FTZ
  PhysTrSet[4]=(1<<4)+(1<<10);//el:FTCT1&EC-F_and
  PhysTrSet[5]=(1<<12);//ph:EC-A_and
  PhysTrSet[6]=(1<<11);//unbEC:EC-A_or
  
//------> get current trig. parameters:
  for(int ii=0;ii<4;ii++){
    tofpattft[ii]=TofPatt1[ii];
  }
//  FTZ=((TrigPatt&(1<<5))!=0);//SlowIon setting is disabled to match with Data 
  AccSectPatt=AntiPatt;
  AccPatt=AntiPatt;
  TOFTrigFl1=TofFlag1;
  TOFTrigFl2=TofFlag2;//bz
  ECTrigFl=EcalFlag;//MN, M=0/1/2/3->noFTE/noFTE(1prj@2requested)/FTEor/FTEand, 
//                                N=0/1/2/3-> /undef/noLev1/Lev1or/Lev1and
  bool CentrOK=((tofpattft[2]&0x1FE01FEL)>0);//have any central counter in Lay-3 trig.patt
//-----------------------------------------
//------>FTC,FTCP0,FTCP1 rebuild:
//
  if(TOFTrigFl1<=4 && TOFTrigFl1>=0){
    L1TrMemPatt|=(1<<1);// set FTCP0(>=3of4)
    L1TrMemPatt|=1;// set FTC
    if(CentrOK)L1TrMemPatt|=(1<<3);// set FTCT0(not used in TrigDecision but let it be seen)
  }
  if(TOFTrigFl1==0)L1TrMemPatt|=(1<<2);// set FTCP1(4of4)
//-----------------------------------------
//------>FTCT1 rebuild:    
//
  if(TOFTrigFl1==0 && CentrOK){//was tof 4of4
    L1TrMemPatt|=(1<<4);// set FTCT1 
  }
//-----------------------------------------
//------> ACC0/1 rebuild:
//
  for(int i=0;i<8;i++)if((AccPatt & (1<<i))>0)NAccs+=1;//count nsectors       
  if(NAccs<1)L1TrMemPatt|=(1<<7);// set ACC0
  if(NAccs<5)L1TrMemPatt|=(1<<8);// set ACC1
//-----------------------------------------
//------> FTZ,BZ-bits rebuild(no need to exclude out.c in L3):
//
  if(FTZ)L1TrMemPatt|=(1<<5);// set FTZ as it was
  if(TOFTrigFl2==0)L1TrMemPatt|=(1<<9);// set BZ "4of4"
//-----------------------------------------
//------> EC-bits rebuild:
//
  if((ECTrigFl/10)==3){
    L1TrMemPatt|=(1<<10);// set EC-F_and
    L1TrMemPatt|=(1<<11);// set EC-F_or
  }  
  else if((ECTrigFl/10==1) || (ECTrigFl/10==2))L1TrMemPatt|=(1<<11);// set EC-F_or
  if(ECTrigFl%10>=2){
    L1TrMemPatt|=(1<<13);// set EC-A_or (not used in TrigDecision but let it be seen)
    if(ECTrigFl%10==3)L1TrMemPatt|=(1<<12);// set EC-A_and
  }  
  if((L1TrMemPatt&(1<<11))>0)L1TrMemPatt|=(1<<6);// set FTE (EC-F_or is required)
//-----------------------------------------
//------>rebuild PhysTrigPatt:
//
  for(int i=0;i<7;i++){
    if((L1TrMemPatt&PhysTrSet[i])==PhysTrSet[i])PhysTrPatt|=(1<<i);
  }
//-----------------------------------------
  if((PhysTrPatt&0x3EL)!=0)return 1;
  else return 0;
}


  char * Level1R::Info(int number){
    RestorePhysBPat();
    int antif=0;
    for(int k=0;k<8;k++){
     if(AntiPatt & (1<<(k))){
       antif++;
     }
    }
//
    int pat[8];
    for(int k=0;k<7;k++){
      pat[k]=0;
      if(PhysBPatt & (1<<k))pat[k]=1;
    }
//
    char toftyp[5],toftypz[5];
    if(TofFlag1==0)strcpy(toftyp,"4of4");
    else if(TofFlag1>0 && TofFlag1<5)strcpy(toftyp,"3of4");
    else if(TofFlag1>=5 && TofFlag1<8)strcpy(toftyp,"1t1b");
    else if(TofFlag1>=5 && TofFlag1<8)strcpy(toftyp,"1t1b");
    else if(TofFlag1==9)strcpy(toftyp,"2top");
    else if(TofFlag1==10)strcpy(toftyp,"2bot");
    else strcpy(toftyp,"unkn");
//
    if(TofFlag2==0)strcpy(toftypz,"4of4");
    else if(TofFlag2>0 && TofFlag2<5)strcpy(toftypz,"3of4");
    else if(TofFlag2>=5 && TofFlag2<8)strcpy(toftypz,"1t1b");
    else if(TofFlag2>=5 && TofFlag2<8)strcpy(toftypz,"1t1b");
    else if(TofFlag2==9)strcpy(toftypz,"2top");
    else if(TofFlag2==10)strcpy(toftypz,"2bot");
    else strcpy(toftypz,"unkn");
//
    double xtime=TrigTime[4]/1000.;

    sprintf(_Info,"TrigLev1: TofZ>=1 %s, TofZ>1 %s, EcalFT  %s, EcalLev1 %d,  TimeD[ms]%6.2f LiveTime%6.2f, PhysTr=|uTf:%d|Z>=1:%d|Ion:%d|SIon:%d|e:%d|ph:%d|uEc:%d|",toftyp,toftypz,IsEcalFtrigOK()?"Yes":"No",EcalFlag,xtime,LiveTime,pat[0],pat[1],pat[2],pat[3],pat[4],pat[5],pat[6]);
    return _Info;
  }

bool Level1R::isTOFTrigger(int level){
for(int i=1;i<5;i++){
if((PhysBPatt & (1<<i)))return true;
}
return false;
}
#ifndef _PGTRACK_
bool TrTrackR::setFitPattern(TrTrackFitR::kAtt k, int & pattern){
if (pattern<0){
  int p=PatternL();
  int npt=(NHits()+1)/2;
  switch(k){
    case TrTrackFitR::kF:
     pattern=BitPattern;
     return true;
    case TrTrackFitR::kH1:
     if(npt<3)npt=3;
     if (NHits()>4 && NHits()%2==0){
      // fit 1,,,n/2-1,n/2+1
       pattern=0;
       for(int k=0;k<npt-1;k++){
         int l=p%10-1;
         pattern|= (1<<l);
         p/=10;
       }
       p/10;
       int l=p%10-1;
       pattern|= (1<<l);
     }
     else if(NHits()>3){
       // fit 1,,,,n/2
       pattern=0;
       for(int k=0;k<npt;k++){
         int l=p%10-1;
         pattern|= (1<<l);
         p/=10;
       }
     }
     else return false;
     return true;
    case TrTrackFitR::kH2:
     if(npt<3)npt=3;
     if (NHits()>4 && NHits()%2==0){
      // fit 1,,,n/2-1,n/2+1
       pattern=0;
       for(int k=0;k<NHits()-npt-1;k++)p/=10;
        int l=p%10-1;
        pattern|= (1<<l);
        p/=10;
        p/=10;
       for(int k=0;k<npt-1;k++){
        int l=p%10-1;
        pattern|= (1<<l);
        p/=10;
       }
     }
     else if(NHits()>3){
       // fit 1,,,,n/2
       for(int k=0;k<NHits()-npt;k++)p/=10;
       pattern=0;
       for(int k=0;k<npt;k++){
         int l=p%10-1;
         pattern|= (1<<l);
         p/=10;
       }
     }
     else return false;
     return true;


     return true;
    case TrTrackFitR::kI:
     pattern=0;
     for(int i=1;i<8;i++)if(((BitPattern>>i)&1))pattern|=(1<<i) ;
     return true;
    case TrTrackFitR::kE:
     if(NHits()<4)return false;
     pattern=0;
     for(int i=0;i<2;i++){
      int l=(p%10)-1;
      pattern|=(1<<l);
      p/=10;
     }     
     for (int i=0;i<NHits()-4;i++)p/=10;
     for(int i=0;i<2;i++){
      int l=(p%10)-1;
      pattern|=(1<<l);
      p/=10;
     }     
     return true;
    default:
     return false;

  }
 
}
else return false;
}

TrTrackFitR::kAtt  TrTrackFitR::getAtt(){

if(Att==0)return kF;
else if(Att==1)return kH1;
else if(Att==2)return kH2;
else if(Att==3)return kI;
else if(Att==4)return kE;
else if(Att==5)return kC;
else  return kND;

}


int TrTrackR::iTrTrackFit(TrTrackFitR & fit,int refit){
// refit 0 just return; 1 refit if not 2 always refit and replace

// Check TrTrackFitR is well defined;
   fit.fTrTrack=this;
   for(int i=0;i<fTrTrackFit.size();i++)fTrTrackFit[i].fTrTrack=this;
   setFitPattern(fit.getAtt(),fit.Pattern);

   if(fit.Algo<0 || fit.Alig<0 || fit.MS<0)return -2;
   if( !isSubSet(fit.Pattern)){
     fit.Pattern=-1;
     return -3;
   }
   if(refit>1){
    static int err=0;
    for(int i=0;i<fTrTrackFit.size();i++){

    if(fTrTrackFit[i]==fit){
      fit=fTrTrackFit[i];
      break;
    }
    }
    int ierr=fit.Fit(this);
    if(ierr){
     if(err++<100)cerr<<"TrTrackR::iTrTrackFit-E-RefitNotYetImplementedOrFitError "<<ierr<<endl;
     return -4;
    }
    else{
    for(int i=0;i<fTrTrackFit.size();i++){

    if(fTrTrackFit[i]==fit){
      fTrTrackFit[i]=fit;
      return i;
    }
    }
    fTrTrackFit.push_back(fit);
    return fTrTrackFit.size()-1;
    }
   }
   else{
    for(int i=0;i<fTrTrackFit.size();i++){

    if(fTrTrackFit[i]==fit){
      fit=fTrTrackFit[i];
      return  i;
    }
    }
    if( refit==0)return -1;
    else{
      int ierr=fit.Fit(this);
    if(ierr){
      static int err=0;
     if(err++<100)cerr<<"TrTrackR::iTrTrackFit-E-RefitNotYetImplementedOrFitError "<<ierr<<endl;
     return -4;
    }
    else{
    fTrTrackFit.push_back(fit);
    return fTrTrackFit.size()-1;
    
    }
    }
   }
}
   
      


bool TrTrackR::isSubSet(int pattern){

for(int k=0;k<9;k++){
 if( (pattern & (1<<k))){
     if (!(BitPattern & (1<<k)))return false;
 }
}
return true;
}


int TrTrackR::PatternL(){
int ret=0;
unsigned int p=1;
for( int k=0;k<9;k++){
  if(BitPattern & (1<<k)){
    ret+=p*(k+1);
     p*=10;
}
}
return ret;
}


int TrTrackR::Layer(unsigned int i){
int ret=0;
for(int k=0;k<9;k++){
if(BitPattern &( 1<<k)){
 if(ret++==i)return k;
}
}
return -1;
}

int TrTrackR::ToBitPattern(int patl){

int p=patl;
int ret=0;
while(p){
int l=p%10-1;
ret|= (1<<l);
p/=10;
}
return patl==PatternL()?ret:-1;

}


unsigned int TrTrackFitR::NHits() const{
unsigned int ret=0;
for( int k=0;k<9;k++){
  if(Pattern & (1<<k))ret++;
}
return ret;
}
unsigned int TrTrackR::NHits() const{
unsigned int ret=0;
for( int k=0;k<9;k++){
  if(BitPattern & (1<<k))ret++;
}
return ret;
}

TrTrackFitR::TrTrackFitR( int pattern, int algo,int alig,int ms,int att,float r,float er,float chi2,AMSPoint coo,float t, float p,int size,TrSCooR ic[],float vel):Pattern(pattern),Algo(algo),Alig(alig),MS(ms),Att(att),Rigidity(r),ErrRigidity(er),Coo(coo),Theta(t),Phi(p),Chi2(chi2),Velocity(vel),fTrTrack(0){
fTrSCoo.clear();
for(int i=0;i<size;i++){
fTrSCoo.push_back(ic[i]);
}

}

bool TrTrackR::Compat(int refit){
if(fTrTrackFit.size()){
if(AdvancedFitDone!=1){
 AdvancedFitDone=1;


  int patternoratt=0;
  int alg=0;
  int ms=1;
  int alig=1;       

{ 
 TrTrackFitR a(patternoratt,alg,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     Rigidity=a.Rigidity;
     ErrRigidity=a.ErrRigidity;
     Theta=a.Theta;
     Phi=a.Phi;
     Chi2FastFit=a.Chi2;
     for(int k=0;k<3;k++)P0[k]=a.Coo[k];
   }
}
for(int k=0;k<3;k++){
    TrTrackFitR a(-1,k,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     HRigidity[0]=a.Rigidity;
     HTheta[0]=a.Theta;
     HPhi[0]=a.Phi;
     for(int l=0;l<3;l++)HP0[0][l]=a.Coo[l];
     HChi2[0]=a.Chi2;
     HErrRigidity[0]=a.ErrRigidity;
     break;
   }
  }
for(int k=0;k<3;k++){
    TrTrackFitR a(-2,k,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     HRigidity[1]=a.Rigidity;
     HTheta[1]=a.Theta;
     HPhi[1]=a.Phi;
     for(int l=0;l<3;l++)HP0[1][l]=a.Coo[l];
     HChi2[1]=a.Chi2;
     HErrRigidity[1]=a.ErrRigidity;
     break;
   }
  }
   
{ 
 TrTrackFitR a(0,1,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     PiRigidity=a.Rigidity;
     PiErrRig=a.ErrRigidity;
   }
}
{ 
 TrTrackFitR a(0,2,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     GeaneFitDone=1;
     GRigidity=a.Rigidity;
     GErrRigidity=a.ErrRigidity;
     GChi2=a.Chi2;
   }
}

{ 
 TrTrackFitR a(0,0,0,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     DBase[0]=a.Rigidity;
     DBase[1]=a.Chi2;
   }
}


{ 
 TrTrackFitR a(0,0,1,0);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     FChi2MS=a.Chi2;
     RigidityMS=a.Rigidity;
   }
}


{ 
 TrTrackFitR a(-3,0,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[0][0]=a.Rigidity;
   }
}
{ 
 TrTrackFitR a(-3,1,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[1][0]=a.Rigidity;
   }
}
{ 
 TrTrackFitR a(-3,2,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[2][0]=a.Rigidity;
   }
}

{ 
 TrTrackFitR a(-4,0,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[0][1]=a.Rigidity;
   }
}
{ 
 TrTrackFitR a(-4,1,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[1][1]=a.Rigidity;
   }
}
{ 
 TrTrackFitR a(-4,2,alig,ms);
  int ret=iTrTrackFit(a,refit);
  if(ret>=0){
     RigidityIE[2][1]=a.Rigidity;
   }
}
}
return true;
}
else{
AdvancedFitDone=0;
GeaneFitDone=0;
Rigidity=0;
Chi2FastFit=FLT_MAX;
PiRigidity=0;
GRigidity=0;
Theta=0;
HRigidity[0]=0;
HRigidity[1]=0;
return false;
}

}
  char * TrTrackR::Info(int number){
   int p=1;
    int pattern=0;
    for(int k=0;k<9;k++){
      if((BitPattern & (1<<k)))pattern+=p;
       p*=10;
     }
     Compat(1);

  char * Info(int number=-1);
    sprintf(_Info,"TrTrack No %d RigFast=%7.3g#pm%6.2g RigPath=%7.3g  RigMi=%7.3g  #theta=%4.2f #phi=%4.2f #chi^{2}=%7.3g/%7.3g Points=%d Pattern=%09d HRig=(%7.3g,%7.3g) IERig=(%7.3g,%7.3g) (%7.3g,%7.3g)",number,Rigidity,ErrRigidity*Rigidity*Rigidity,PiRigidity,GRigidity, Theta,Phi,Chi2FastFit,DBase[1],NTrRecHit(),pattern,HRigidity[0],HRigidity[1],RigidityIE[0][0],RigidityIE[0][1],RigidityIE[2][0],RigidityIE[2][1]);
  return _Info;
  }



#endif
int ParticleR::ReBuildTrdEcal(float distmax,float dirmax,float DistX,float DistY,bool force){
#ifdef _PGTRACK_
if(iVertex()>=0 )return 6; // vertex
if(iEcalShower()<0)return 1; // no ecal 
if(iTrdTrack()<0 && iTrdHTrack()<0)return 2;
bool usetrdh=false;
if(iTrdTrack()<0 && iTrdHTrack()>=0)usetrdh=true;
EcalShowerR *pecal=pEcalShower();
if(!pecal)return 3; // Logic error ecal not present while should be
  AMSDir ecal_dir(pecal->Dir);
  if(ecal_dir[2]>0){
    ecal_dir=AMSDir(-pecal->Dir[0],-pecal->Dir[1],-pecal->Dir[2]);
  } 
if(!usetrdh){
  TrdTrackR *ptrd=pTrdTrack();
  if(!ptrd)return 4;// Logic error trd not present while should be
  const int minAmp=5; //  Here the minimal amp for the tracker to take into account;
  float q=ptrd->Q>6?6:ptrd->Q;  
  float thr=minAmp*q*q;
  AMSDir trd_dir(ptrd->Theta,ptrd->Phi);
  if(trd_dir[2]>=0)return 5;
  double ct=ecal_dir.prod(trd_dir);
  bool catleak=(pecal->Status&(16384*2*2*2*2*2*2*2*2*2*2*2))!=0;
  if(acos(ct)>dirmax*180/3.1415926 && !catleak)return -2;
  double x=trd_dir[0]*(pecal->CofG[2]-ptrd->Coo[2])/trd_dir[2]+ptrd->Coo[0]-pecal->CofG[0];  
  double y=trd_dir[1]*(pecal->CofG[2]-ptrd->Coo[2])/trd_dir[2]+ptrd->Coo[1]-pecal->CofG[1];
  if(sqrt(x*x+y*y)>=distmax)return -1;  
  bool trdecal=!catleak;
again:
  AMSDir newdircommon=trdecal?AMSDir(-ptrd->Coo[0]+pecal->CofG[0],-ptrd->Coo[1]+pecal->CofG[1],-ptrd->Coo[2]+pecal->CofG[2]):trd_dir;
  AMSPoint newcoocommon=trdecal?AMSPoint((ptrd->Coo[0]+pecal->CofG[0])/2,(ptrd->Coo[1]+pecal->CofG[1])/2,(ptrd->Coo[2]+pecal->CofG[2])/2):AMSPoint(ptrd->Coo[0],ptrd->Coo[1],ptrd->Coo[2]);
  AMSDir newdirtrd=trd_dir;
  AMSPoint newcootrd=AMSPoint(ptrd->Coo[0],ptrd->Coo[1],ptrd->Coo[2]);
  AMSDir newdirecal=ecal_dir;
  AMSPoint newcooecal=AMSPoint(pecal->CofG[0],pecal->CofG[1],pecal->CofG[2]);
  bool done=false;
  double chisq_thr=10000; 
  if(AMSEventR::Head()->nTrTrack()>1){
    double cosmax=cos(dirmax/180*3.1415926);
    double cosmax_dmax=cosmax;
    double dmax=distmax;
    double dmax_cosmax=distmax;
    int j=-1;
    int jspace=-1;
   for(unsigned int i=0;i<AMSEventR::Head()->NTrTrack();i++){
     try{
       TrTrackR &trk= AMSEventR::Head()->TrTrack(i);
       if(trk.GetChisq()>chisq_thr)continue;
       AMSDir dir;
       AMSPoint pnt;
       double zpl=pecal->Entry[2]>pecal->Exit[2]?pecal->Entry[2]:pecal->Exit[2];
          
       trk.Interpolate(zpl,pnt,dir);
       double cosc=dir.prod(newdircommon);
       double dentry=sqrt((pnt[0]-pecal->Entry[0])*(pnt[0]-pecal->Entry[0])+(pnt[1]-pecal->Entry[1])*(pnt[1]-pecal->Entry[1])+(pnt[2]-pecal->Entry[2])*(pnt[2]-pecal->Entry[2]));
       double dexit=sqrt((pnt[0]-pecal->Exit[0])*(pnt[0]-pecal->Exit[0])+(pnt[1]-pecal->Exit[1])*(pnt[1]-pecal->Exit[1])+(pnt[2]-pecal->Exit[2])*(pnt[2]-pecal->Exit[2]));
        double dd=dentry<dexit?dentry:dexit;
       if(fabs(cosc)>cosmax){
         cosmax=fabs(cosc);
         dmax_cosmax=dd;             
         j=i;
       }
       if(dd<dmax){
         dmax=dd;
         cosmax_dmax=fabs(cosc);            
         jspace=i;
       }
     }
     catch (...){
       cerr<<"ReBuildTrdEcal-S-TrTrackelementOutOfSize "<<i<<" "<<AMSEventR::Head()->NTrTrack()<<endl;
     }    
    }
    if(j>=0 &&jspace>=0 ){
      if(jspace!=j){
       static int mprint=0;
       if(mprint++<100){
        cerr<<"ParticleR::ReBuldTrdEcal-W-SpaceDirMatchDisagree "<<dmax_cosmax<<" "<<dmax<<" "<<cosmax<<" "<<cosmax_dmax<<" "<<j<<" "<<jspace<<endl;
       }
       double dcos=fabs(AMSEventR::Head()->pTrTrack(j)->GetDir().prod(AMSEventR::Head()->pTrTrack(jspace)->GetDir()));
       if(dcos>0.999)j=jspace;
       j=fabs(AMSEventR::Head()->pTrTrack(j)->GetRigidity())>fabs(AMSEventR::Head()->pTrTrack(jspace)->GetRigidity())?j:jspace;
      }
       force=false;
}
    if(j>=0 && jspace>=0 && iTrTrack()!=j){
      TrTrackR *newtrack=AMSEventR::Head()->pTrTrack(j);
      //  update particle pars
  int iddflt=newtrack->Gettrdefaultfit();
  Theta=newtrack->GetTheta(iddflt);
  Phi=newtrack->GetPhi(iddflt);
  AMSDir dir(Theta,Phi);
  if((Beta<0 && dir[2]<0) || (Beta>0 && dir[2]>0)){
     for(int i=0;i<3;i++)dir[i]=-dir[i];
     Theta=dir.gettheta();
     Phi=dir.getphi();
   }
  
  if(iCharge()<0){
     fCharge=AMSEventR::Head()->NCharge()-1;
     if(fCharge>=0){
      Charge=AMSEventR::Head()->fCharge[fCharge].Charge();
     } 
  }
 
  _build(newtrack->GetRigidity(iddflt),newtrack->GetErrRinv(iddflt),Charge,Beta,ErrBeta,Mass,ErrMass,Momentum,ErrMomentum);
  for(int k=0;k<3;k++)Coo[k]=newtrack->GetP0(iddflt)[k];
  Loc2Gl(AMSEventR::Head());
  for(unsigned int k=0;k<sizeof(TOFCoo)/3/sizeof(TOFCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TOFCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TOFCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(EcalCoo)/3/sizeof(EcalCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(EcalCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)EcalCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TrCoo)/3/sizeof(TrCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TrCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TrCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TRDCoo)/3/sizeof(TRDCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TRDCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TRDCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(RichCoo)/3/sizeof(RichCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(RichCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)RichCoo[k][l]=pnt[l];
  }  
  fTrTrack= j;
//  cout <<" fTrTrack "<<fTrTrack<<endl;
  TofRecH::ReBuild(Charge);
  done=true;

    } 
  }
  if(iTrTrack()>=0 && !force){
    
    return done?0:7; 
 }
  else{
     //TrTrackR* track=0;
  //if(iTrTrack()>=0){
  //   track=AMSEventR::Head()->pTrTrack(iTrTrack());
//     for(int k=0;k<track->NTrRecHit();k++){
//       cout <<" k hit "<<AMSEventR::Head()->TrRecHit(track->iTrRecHit(k)).GetCoord()<<endl;
//     }
//     for(int k=1;k<4;k++){
//         int id=track->iTrTrackPar(k,0,1);
//         if(id>=0)cout<<" k "<<k<<" "<<track->gTrTrackPar(id).Rigidity<<endl;
//          id=track->iTrTrackPar(k,0,2);
//         if(id>=0)cout<<" kewfit "<<k<<" "<<track->gTrTrackPar(id).Rigidity<<endl;
//     }
         
//  } 
// no track should construct it
   TrTrackR *newtrack= new TrTrackR();
   int mfit1 = TrTrackR::kChoutko;
   int mfitext = TrTrackR::DefaultFitID;
   int nhits=0;
   int p[10]; 
   int p2[10]; 
   int pmulti[10]; 
   int pmulti2[10]; 
  for(int layer=0;layer<9;layer++){
   p[layer]=-1;
   pmulti[layer]=-1;
   p2[layer]=-1;
   pmulti2[layer]=-1;
   double mdistx=1e10;
   double mdisty=1e10;
     AMSDir newdir=newdircommon;
     AMSPoint newcoo=newcoocommon;
     if(layer==0 || layer==1){
        newdir=newdirtrd;
         newcoo=newcootrd;
     }
      else if(layer==8){
        newdir=newdirecal;
         newcoo=newcooecal;
      }
      else if(0){
       int ibeg=-1;
       int iend=-1;
       for(int j=layer-1;j>=1;j--){
         if(p[j]>=0){
           if(iend<0)iend=j;
           else{
             ibeg=j;
             break;
           }
        } 
       }
       if(iend>=0 && ibeg>=0){
         AMSPoint beg=AMSEventR::Head()->pTrRecHit(p[ibeg])->GetCoord(pmulti[ibeg]);
         AMSPoint end=AMSEventR::Head()->pTrRecHit(p[iend])->GetCoord(pmulti[iend]);
         newcoo=AMSPoint((beg[0]+end[0])/2,(beg[1]+end[1])/2,(beg[2]+end[2])/2);
         newdir=AMSDir((beg[0]-end[0])/2,(beg[1]-end[1])/2,(beg[2]-end[2])/2);
      }
       ibeg=-1;
       iend=-1;
     }
   for(unsigned int i=0;i<AMSEventR::Head()->NTrRecHit();i++){
     TrRecHitR &trh= AMSEventR::Head()->TrRecHit(i);
     if(trh.GetLayerJ()!=layer+1)continue;
     if(trh.Sum()<thr)continue;
     int mult=-1;
     AMSPoint ap;
     ap[2]=trh.GetCoord(0)[2];
     ap[0]=newdir[0]/newdir[2]*(ap[2]-newcoo[2])+newcoo[0];
     ap[1]=newdir[1]/newdir[2]*(ap[2]-newcoo[2])+newcoo[1];
     AMSPoint dist=trh.HitPointDist(ap,mult);
     ap[2]=trh.GetCoord(mult)[2];
     ap[0]=newdir[0]/newdir[2]*(ap[2]-newcoo[2])+newcoo[0];
     ap[1]=newdir[1]/newdir[2]*(ap[2]-newcoo[2])+newcoo[1];
     dist=trh.HitPointDist(ap,mult);
     if(fabs(dist[0])<DistX && fabs(dist[1])<DistY){
       if(fabs(dist[0])<mdistx && fabs(dist[1])<=mdisty+100e-4){
         p2[layer]=p[layer];
         p[layer]=i;
         pmulti2[layer]=pmulti[layer];
         pmulti[layer]=mult;
         mdistx=fabs(dist[0]);
         mdisty=fabs(dist[1]);
//         cout <<" layer "<<layer<<" "<<i<<" "<<ap<<" "<<dist<<" "<<mdistx<<" "<<mdisty<<" "<<trh.GetCoord(mult)<<endl;
       }
       if (!patt) {
       int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
       patt = new tkpatt(nn);
       patt->Init(nn);
       
     }
     }
    }
     if(p[layer]>=0){
      AMSEventR::Head()->pTrRecHit(p[layer])->SetResolvedMultiplicity(pmulti[layer]);
       newtrack->AddHit(AMSEventR::Head()->pTrRecHit(p[layer]),pmulti[layer]);
    
       if(layer!=0 && layer!=8)nhits++;
       else if (layer==0)mfitext|=TrTrackR::kFitLayer8;
       else  mfitext|=TrTrackR::kFitLayer9;

    }
}
   
  if(nhits>=3){
      float ret = newtrack->FitT(mfit1);


      if (ret < 0 ||
      newtrack->GetChisqX(mfit1) <= 0 || newtrack->GetChisqY(mfit1) <= 0 ||
      newtrack->GetNdofX (mfit1) <0 || newtrack->GetNdofY (mfit1) < 0) {
//      cout <<" ret "<<ret<<" "<<nhits<<" "<<newtrack->GetChisqX(mfit1)<<" "<<" "<<newtrack->GetNdofY(mfit1)<<" "<<newtrack->GetNdofX(mfit1)<<" "<<newtrack->GetChisqY(mfit1)<<endl;
      delete newtrack;
      return -3;
      
     }
     else{

//    try to test add hits in the vicinity
      bool change=false;
      for(int i=1;i<8;i++){
        if(p2[i]>=0){
         change=true;
         break;
        }
      }
if(change){
      TrTrackR *newtrack2=new TrTrackR(*newtrack);
      double chisq=newtrack->GetChisq(mfit1);
      for(int i=1;i<8;i++){
        if(p2[i]>=0){
           newtrack2->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p2[i])->GetLayer());
           newtrack2->AddHit(AMSEventR::Head()->pTrRecHit(p2[i]),pmulti2[i]);
      float ret2 = newtrack2->FitT(mfit1);
      if (ret2 < 0 ||
       newtrack2->GetChisqX(mfit1) <= 0 || newtrack2->GetChisqY(mfit1) <= 0 ||
       newtrack2->GetNdofX (mfit1) <0 || newtrack2->GetNdofY (mfit1) < 0 || (newtrack2->GetChisq(mfit1)>chisq && fabs(newtrack2->GetRigidity(mfit1))>0.2)) {
           newtrack2->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p2[i])->GetLayer());
           newtrack2->AddHit(AMSEventR::Head()->pTrRecHit(p[i]),pmulti[i]);
       }
       else{
         chisq= newtrack2->GetChisq(mfit1);
       }       

       }
          
      }
      if(chisq<newtrack->GetChisq(mfit1)){
       delete newtrack;
       newtrack=newtrack2;
      }
      else delete newtrack2;
}

      if(newtrack->GetChisq(mfit1)>chisq_thr && nhits>4){
      for(int i=1;i<8;i++){
        if(p[i]>=0){
           newtrack->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p[i])->GetLayer());
      float ret2 = newtrack->FitT(mfit1);
      if (ret2 < 0 ||
       newtrack->GetChisqX(mfit1) <= 0 || newtrack->GetChisqY(mfit1) <= 0 ||
       newtrack->GetNdofX (mfit1) <0 || newtrack->GetNdofY (mfit1) < 0 || newtrack->GetChisq(mfit1)>chisq_thr/2 || fabs(newtrack->GetRigidity(mfit1))<0.2) {
           newtrack->AddHit(AMSEventR::Head()->pTrRecHit(p[i]),pmulti[i]);
       }
       else{
         break;
       }       

       }
          
      }
        
      }
        if(mfit1!=mfitext){
          newtrack->FillExRes();
          newtrack->FitT(mfitext);
        }
          newtrack->RecalcHitCoordinates(mfitext); 
          newtrack->Settrdefaultfit(mfitext);
          newtrack->EstimateDummyX(mfitext);
          newtrack->UpdateBitPattern();
          newtrack->DoAdvancedFit();
        if (newtrack->GetRigidity() == 0 || newtrack->GetChisq() <= 0) {
             delete newtrack;
              return -4;
  }
  // Add the track to the collection
  // set used hits
  for(int layer=0;layer<9;layer++){
   if(p[layer]>=0)AMSEventR::Head()->pTrRecHit(p[layer])->SetUsed();
  }
  int iddflt=-1;
  for(int k=1;k<4;k++){
        int id=newtrack->iTrTrackPar(k,0,1);
        if(iddflt<0 && id>=0 && newtrack->GetChisq(id)<1000000000){
           newtrack->Settrdefaultfit(id);
           iddflt=id;
        }
//         if(id>=0)cout<<" k new track "<<k<<" "<<newtrack->gTrTrackPar(id).Rigidity<<" "<<newtrack->GetRigidity()<<endl;
   
 }
 if(iddflt<0){
   delete newtrack;
   return -5;
 }
       AMSDir dir;
       AMSPoint pnt;
       double zpl=pecal->Entry[2]>pecal->Exit[2]?pecal->Entry[2]:pecal->Exit[2];
          
       newtrack->Interpolate(zpl,pnt,dir);
       double dentry=sqrt((pnt[0]-pecal->Entry[0])*(pnt[0]-pecal->Entry[0])+(pnt[1]-pecal->Entry[1])*(pnt[1]-pecal->Entry[1])+(pnt[2]-pecal->Entry[2])*(pnt[2]-pecal->Entry[2]));
       double dexit=sqrt((pnt[0]-pecal->Exit[0])*(pnt[0]-pecal->Exit[0])+(pnt[1]-pecal->Exit[1])*(pnt[1]-pecal->Exit[1])+(pnt[2]-pecal->Exit[2])*(pnt[2]-pecal->Exit[2]));
        double dd=dentry<dexit?dentry:dexit;
        if(dd>distmax){
         delete newtrack;
         return -6;
        }
  if(iCharge()<0){
     fCharge=AMSEventR::Head()->NCharge()-1;
     if(fCharge>=0){
      Charge=AMSEventR::Head()->fCharge[fCharge].Charge();
     } 
  }

  _build(newtrack->GetRigidity(iddflt),newtrack->GetErrRinv(iddflt),Charge,Beta,ErrBeta,Mass,ErrMass,Momentum,ErrMomentum);
  newtrack->setstatus(AMSDBc::USED);
   
  AMSEventR::Head()->fTrTrack.push_back(*newtrack);
//  update particle pars
  Theta=newtrack->GetTheta(iddflt);
  Phi=newtrack->GetPhi(iddflt);
  if((Beta<0 && dir[2]<0) || (Beta>0 && dir[2]>0)){
     for(int i=0;i<3;i++)dir[i]=-dir[i];
     Theta=dir.gettheta();
     Phi=dir.getphi();
   }


  for(int k=0;k<3;k++)Coo[k]=newtrack->GetP0(iddflt)[k];
  Loc2Gl(AMSEventR::Head());
  for(unsigned int k=0;k<sizeof(TOFCoo)/3/sizeof(TOFCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TOFCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TOFCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(EcalCoo)/3/sizeof(EcalCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(EcalCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)EcalCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TrCoo)/3/sizeof(TrCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TrCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TrCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TRDCoo)/3/sizeof(TRDCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TRDCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TRDCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(RichCoo)/3/sizeof(RichCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(RichCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)RichCoo[k][l]=pnt[l];
  }  
  delete newtrack;
  fTrTrack= AMSEventR::Head()->fTrTrack.size()-1;
//  cout <<" fTrTrack "<<fTrTrack<<endl;
  TofRecH::ReBuild(Charge);
    
    
  return 0;
 
     }
   }
   else{
      delete newtrack;
      if(trdecal){
           trdecal=false;
           goto again;
      }
      return -7;
   }   

      
   
}
}
else{
 return 6;
}
#else
return 10;
#endif
}




int ParticleR::ReBuildTrdTOF(float distmax,float dirmax,float DistX,float DistY,bool force){
#ifdef _PGTRACK_
if(iVertex()>=0 )return 6; // vertex
if(iBetaH()<0)return 1; // no betah
//if(iTrdTrack()<0 && iTrdHTrack()<0)return 2;
BetaHR *ph=pBetaH();
if(!ph)return 3; // Logic error betah not present while should be
float zcofg=-130;
float ztcofg=130;
AMSPoint ecfg;
  AMSDir ecal_dir;
  double tm=0;
  ph->TInterpolate(zcofg,ecfg,ecal_dir,tm,false);
  if(ecal_dir[2]>0){
    ecal_dir=AMSDir(-ecal_dir[0],-ecal_dir[1],-ecal_dir[2]);
  } 
if(1){
  TrdTrackR *ptrd=pTrdTrack();
  const int minAmp=5; //  Here the minimal amp for the tracker to take into account;
   int nlay;
   float qrms; 
   float q=ph->GetQ(nlay,qrms);
   if(q>6)q=6;  
  float thr=minAmp*q*q;
again_notrd:
  AMSDir trd_dir;
  AMSPoint tcfg;
  if(ptrd){
   trd_dir=AMSDir(ptrd->Theta,ptrd->Phi);
   tcfg=AMSPoint(ptrd->Coo[0],ptrd->Coo[1],ptrd->Coo[2]);
  }
  else{
   ph->TInterpolate(ztcofg,tcfg,trd_dir,tm,false);
   DistX*=1.4;
   DistY*=1.4;
  }
 if(trd_dir[2]>0){
    trd_dir=AMSDir(-trd_dir[0],-trd_dir[1],-trd_dir[2]);
  }

  double ct=ecal_dir.prod(trd_dir);
  if(acos(ct)>dirmax*180/3.1415926 )return -2;
  double x=trd_dir[0]*(ecfg[2]-tcfg[2])/trd_dir[2]+tcfg[0]-ecfg[0];  
  double y=trd_dir[1]*(ecfg[2]-tcfg[2])/trd_dir[2]+tcfg[1]-ecfg[1];
  if(sqrt(x*x+y*y)>=distmax ){
    if(ptrd){
      ptrd=0;
      goto again_notrd;  
     }
     else return -1;
   } 
  bool trdecal=true;
again:
  AMSDir newdircommon=trdecal?AMSDir(-tcfg[0]+ecfg[0],-tcfg[1]+ecfg[1],-tcfg[2]+ecfg[2]):trd_dir;
  AMSPoint newcoocommon=trdecal?AMSPoint((tcfg[0]+ecfg[0])/2,(tcfg[1]+ecfg[1])/2,(tcfg[2]+ecfg[2])/2):AMSPoint(tcfg[0],tcfg[1],tcfg[2]);
  AMSDir newdirtrd=trd_dir;
  AMSPoint newcootrd=AMSPoint(tcfg[0],tcfg[1],tcfg[2]);
  AMSDir newdirecal=ecal_dir;
  AMSPoint newcooecal=ecfg;
  bool done=false;
  double chisq_thr=10000; 
  if(AMSEventR::Head()->nTrTrack()>1){
    double cosmax=cos(dirmax/180*3.1415926);
    double cosmax_dmax=cosmax;
    double dmax=distmax;
    double dmax_cosmax=distmax;
    int j=-1;
    int jspace=-1;
   for(unsigned int i=0;i<AMSEventR::Head()->NTrTrack();i++){
     try{
       TrTrackR &trk= AMSEventR::Head()->TrTrack(i);
       if(trk.GetChisq()>chisq_thr)continue;
       AMSDir dir;
       AMSPoint pnt;
       double zpl=zcofg;
          
       trk.Interpolate(zpl,pnt,dir);
       double cosc=dir.prod(newdircommon);
       double dentry=sqrt((pnt[0]-ecfg[0])*(pnt[0]-ecfg[0])+(pnt[1]-ecfg[1])*(pnt[1]-ecfg[1])+(pnt[2]-ecfg[2])*(pnt[2]-ecfg[2]));
        double dd=dentry;
       if(fabs(cosc)>cosmax){
         cosmax=fabs(cosc);
         dmax_cosmax=dd;             
         j=i;
       } 
       if(dd<dmax){
         dmax=dd;
         cosmax_dmax=fabs(cosc);            
         jspace=i;
       }
     }
     catch (...){
       cerr<<"ReBuildTrdTOF-S-TrTrackelementOutOfSize "<<i<<" "<<AMSEventR::Head()->NTrTrack()<<endl;
     }    
    }
    if(j>=0 &&jspace>=0 ){
      if(jspace!=j){
       static int mprint=0;
       if(mprint++<100){
        cerr<<"ParticleR::ReBuldTrdTOF-W-SpaceDirMatchDisagree "<<dmax_cosmax<<" "<<dmax<<" "<<cosmax<<" "<<cosmax_dmax<<" "<<j<<" "<<jspace<<endl;
       }
       double dcos=fabs(AMSEventR::Head()->pTrTrack(j)->GetDir().prod(AMSEventR::Head()->pTrTrack(jspace)->GetDir()));
       if(dcos>0.999)j=jspace;
       j=fabs(AMSEventR::Head()->pTrTrack(j)->GetRigidity())>fabs(AMSEventR::Head()->pTrTrack(jspace)->GetRigidity())?j:jspace;
      }
       force=false;
}
    if(j>=0 && jspace>=0 && iTrTrack()!=j){
      TrTrackR *newtrack=AMSEventR::Head()->pTrTrack(j);
      //  update particle pars
  int iddflt=newtrack->Gettrdefaultfit();
  Theta=newtrack->GetTheta(iddflt);
  Phi=newtrack->GetPhi(iddflt);
  AMSDir dir(Theta,Phi);
  if((Beta<0 && dir[2]<0) || (Beta>0 && dir[2]>0)){
     for(int i=0;i<3;i++)dir[i]=-dir[i];
     Theta=dir.gettheta();
     Phi=dir.getphi();
   }
  
  if(iCharge()<0){
     fCharge=AMSEventR::Head()->NCharge()-1;
     if(fCharge>=0){
      Charge=AMSEventR::Head()->fCharge[fCharge].Charge();
     } 
  }
 
  _build(newtrack->GetRigidity(iddflt),newtrack->GetErrRinv(iddflt),Charge,Beta,ErrBeta,Mass,ErrMass,Momentum,ErrMomentum);
  for(int k=0;k<3;k++)Coo[k]=newtrack->GetP0(iddflt)[k];
  Loc2Gl(AMSEventR::Head());
  for(unsigned int k=0;k<sizeof(TOFCoo)/3/sizeof(TOFCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TOFCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TOFCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(EcalCoo)/3/sizeof(EcalCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(EcalCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)EcalCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TrCoo)/3/sizeof(TrCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TrCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TrCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TRDCoo)/3/sizeof(TRDCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TRDCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TRDCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(RichCoo)/3/sizeof(RichCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(RichCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)RichCoo[k][l]=pnt[l];
  }  
  fTrTrack= j;
//  cout <<" fTrTrack "<<fTrTrack<<endl;
  TofRecH::ReBuild(Charge);
  done=true;

    } 
  }
  if(iTrTrack()>=0 && !force){
    
    return done?0:7; 
 }
  else{
  //   TrTrackR* track=0;
 // if(iTrTrack()>=0){
 //    track=AMSEventR::Head()->pTrTrack(iTrTrack());
//     for(int k=0;k<track->NTrRecHit();k++){
//       cout <<" k hit "<<AMSEventR::Head()->TrRecHit(track->iTrRecHit(k)).GetCoord()<<endl;
//     }
//     for(int k=1;k<4;k++){
//         int id=track->iTrTrackPar(k,0,1);
//         if(id>=0)cout<<" k "<<k<<" "<<track->gTrTrackPar(id).Rigidity<<endl;
//          id=track->iTrTrackPar(k,0,2);
//         if(id>=0)cout<<" kewfit "<<k<<" "<<track->gTrTrackPar(id).Rigidity<<endl;
//     }
         
  //} 
// no track should construct it
   TrTrackR *newtrack= new TrTrackR();
   int mfit1 = TrTrackR::kChoutko;
   int mfitext = TrTrackR::DefaultFitID;
   int nhits=0;
   int p[10]; 
   int p2[10]; 
   int pmulti[10]; 
   int pmulti2[10]; 
  for(int layer=0;layer<9;layer++){
   p[layer]=-1;
   pmulti[layer]=-1;
   p2[layer]=-1;
   pmulti2[layer]=-1;
   double mdistx=1e10;
   double mdisty=1e10;
     AMSDir newdir=newdircommon;
     AMSPoint newcoo=newcoocommon;
     if(layer==0 || layer==1){
        newdir=newdirtrd;
         newcoo=newcootrd;
     }
      else if(layer==8){
        newdir=newdirecal;
         newcoo=newcooecal;
      }
      else if(0){
       int ibeg=-1;
       int iend=-1;
       for(int j=layer-1;j>=1;j--){
         if(p[j]>=0){
           if(iend<0)iend=j;
           else{
             ibeg=j;
             break;
           }
        } 
       }
       if(iend>=0 && ibeg>=0){
         AMSPoint beg=AMSEventR::Head()->pTrRecHit(p[ibeg])->GetCoord(pmulti[ibeg]);
         AMSPoint end=AMSEventR::Head()->pTrRecHit(p[iend])->GetCoord(pmulti[iend]);
         newcoo=AMSPoint((beg[0]+end[0])/2,(beg[1]+end[1])/2,(beg[2]+end[2])/2);
         newdir=AMSDir((beg[0]-end[0])/2,(beg[1]-end[1])/2,(beg[2]-end[2])/2);
      }
       ibeg=-1;
       iend=-1;
     }
   for(unsigned int i=0;i<AMSEventR::Head()->NTrRecHit();i++){
     TrRecHitR &trh= AMSEventR::Head()->TrRecHit(i);
     if(trh.GetLayerJ()!=layer+1)continue;
     if(trh.Sum()<thr)continue;
     int mult=-1;
     AMSPoint ap;
     ap[2]=trh.GetCoord(0)[2];
     ap[0]=newdir[0]/newdir[2]*(ap[2]-newcoo[2])+newcoo[0];
     ap[1]=newdir[1]/newdir[2]*(ap[2]-newcoo[2])+newcoo[1];
     AMSPoint dist=trh.HitPointDist(ap,mult);
     ap[2]=trh.GetCoord(mult)[2];
     ap[0]=newdir[0]/newdir[2]*(ap[2]-newcoo[2])+newcoo[0];
     ap[1]=newdir[1]/newdir[2]*(ap[2]-newcoo[2])+newcoo[1];
     dist=trh.HitPointDist(ap,mult);
     if(fabs(dist[0])<DistX && fabs(dist[1])<DistY){
       if(fabs(dist[0])<mdistx && fabs(dist[1])<=mdisty+100e-4){
         p2[layer]=p[layer];
         p[layer]=i;
         pmulti2[layer]=pmulti[layer];
         pmulti[layer]=mult;
         mdistx=fabs(dist[0]);
         mdisty=fabs(dist[1]);
         //cout <<" layer "<<layer<<" "<<i<<" "<<ap<<" "<<dist<<" "<<mdistx<<" "<<mdisty<<" "<<trh.GetCoord(mult)<<endl;
       }
       if (!patt) {
       int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
       patt = new tkpatt(nn);
       patt->Init(nn);
       
     }
     }
    }
     if(p[layer]>=0){
      AMSEventR::Head()->pTrRecHit(p[layer])->SetResolvedMultiplicity(pmulti[layer]);
       newtrack->AddHit(AMSEventR::Head()->pTrRecHit(p[layer]),pmulti[layer]);
    
       if(layer!=0 && layer!=8)nhits++;
       else if (layer==0)mfitext|=TrTrackR::kFitLayer8;
       else  mfitext|=TrTrackR::kFitLayer9;

    }
}
   
  if(nhits>=3){
      float ret = newtrack->FitT(mfit1);


      if (ret < 0 ||
      newtrack->GetChisqX(mfit1) <= 0 || newtrack->GetChisqY(mfit1) <= 0 ||
      newtrack->GetNdofX (mfit1) <0 || newtrack->GetNdofY (mfit1) < 0) {
      //cout <<" ret  "<<ret<<" "<<nhits<<" "<<newtrack->GetChisqX(mfit1)<<" "<<" "<<newtrack->GetNdofY(mfit1)<<" "<<newtrack->GetNdofX(mfit1)<<" "<<newtrack->GetChisqY(mfit1)<<endl;
      delete newtrack;
      return -3;
      
     }
     else{

//    try to test add hits in the vicinity
      bool change=false;
      for(int i=1;i<8;i++){
        if(p2[i]>=0){
         change=true;
         break;
        }
      }
if(change){
      TrTrackR *newtrack2=new TrTrackR(*newtrack);
      double chisq=newtrack->GetChisq(mfit1);
      for(int i=1;i<8;i++){
        if(p2[i]>=0){
           newtrack2->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p2[i])->GetLayer());
           newtrack2->AddHit(AMSEventR::Head()->pTrRecHit(p2[i]),pmulti2[i]);
      float ret2 = newtrack2->FitT(mfit1);
      if (ret2 < 0 ||
       newtrack2->GetChisqX(mfit1) <= 0 || newtrack2->GetChisqY(mfit1) <= 0 ||
       newtrack2->GetNdofX (mfit1) <0 || newtrack2->GetNdofY (mfit1) < 0 || (newtrack2->GetChisq(mfit1)>chisq && fabs(newtrack2->GetRigidity(mfit1))>0.2)) {
           newtrack2->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p2[i])->GetLayer());
           newtrack2->AddHit(AMSEventR::Head()->pTrRecHit(p[i]),pmulti[i]);
       }
       else{
         chisq= newtrack2->GetChisq(mfit1);
       }       

       }
          
      }
      if(chisq<newtrack->GetChisq(mfit1)){
       delete newtrack;
       newtrack=newtrack2;
      }
      else delete newtrack2;
}

      if(newtrack->GetChisq(mfit1)>chisq_thr && nhits>4){
      for(int i=1;i<8;i++){
        if(p[i]>=0){
           newtrack->RemoveHitOnLayer(AMSEventR::Head()->pTrRecHit(p[i])->GetLayer());
      float ret2 = newtrack->FitT(mfit1);
      if (ret2 < 0 ||
       newtrack->GetChisqX(mfit1) <= 0 || newtrack->GetChisqY(mfit1) <= 0 ||
       newtrack->GetNdofX (mfit1) <0 || newtrack->GetNdofY (mfit1) < 0 || newtrack->GetChisq(mfit1)>chisq_thr/2 || fabs(newtrack->GetRigidity(mfit1))<0.2) {
           newtrack->AddHit(AMSEventR::Head()->pTrRecHit(p[i]),pmulti[i]);
       }
       else{
         break;
       }       

       }
          
      }
        
      }
        if(mfit1!=mfitext){
          newtrack->FillExRes();
          newtrack->FitT(mfitext);
        }
          newtrack->RecalcHitCoordinates(mfitext); 
          newtrack->Settrdefaultfit(mfitext);
          newtrack->EstimateDummyX(mfitext);
          newtrack->UpdateBitPattern();
          newtrack->DoAdvancedFit();
        if (newtrack->GetRigidity() == 0 || newtrack->GetChisq() <= 0) {
             delete newtrack;
              return -4;
  }
  // Add the track to the collection
  // set used hits
  for(int layer=0;layer<9;layer++){
   if(p[layer]>=0)AMSEventR::Head()->pTrRecHit(p[layer])->SetUsed();
  }
  int iddflt=-1;
  for(int k=1;k<4;k++){
        int id=newtrack->iTrTrackPar(k,0,1);
        if(iddflt<0 && id>=0 && newtrack->GetChisq(id)<1000000000){
           newtrack->Settrdefaultfit(id);
           iddflt=id;
        }
//         if(id>=0)cout<<" k new track "<<k<<" "<<newtrack->gTrTrackPar(id).Rigidity<<" "<<newtrack->GetRigidity()<<endl;
   
 }
 if(iddflt<0){
   delete newtrack;
   return -5;
 }
       AMSDir dir;
       AMSPoint pnt;
       double zpl=zcofg;
          
       newtrack->Interpolate(zpl,pnt,dir);
       double dentry=sqrt((pnt[0]-ecfg[0])*(pnt[0]-ecfg[0])+(pnt[1]-ecfg[1])*(pnt[1]-ecfg[1])+(pnt[2]-ecfg[2])*(pnt[2]-ecfg[2]));
        double dd=dentry;
        if(dd>distmax){
         delete newtrack;
         return -6;
        }
  if(iCharge()<0){
     fCharge=AMSEventR::Head()->NCharge()-1;
     if(fCharge>=0){
      Charge=AMSEventR::Head()->fCharge[fCharge].Charge();
     } 
  }

  _build(newtrack->GetRigidity(iddflt),newtrack->GetErrRinv(iddflt),Charge,Beta,ErrBeta,Mass,ErrMass,Momentum,ErrMomentum);
  newtrack->setstatus(AMSDBc::USED);
   
  AMSEventR::Head()->fTrTrack.push_back(*newtrack);
//  update particle pars
  Theta=newtrack->GetTheta(iddflt);
  Phi=newtrack->GetPhi(iddflt);
  if((Beta<0 && dir[2]<0) || (Beta>0 && dir[2]>0)){
     for(int i=0;i<3;i++)dir[i]=-dir[i];
     Theta=dir.gettheta();
     Phi=dir.getphi();
   }


  for(int k=0;k<3;k++)Coo[k]=newtrack->GetP0(iddflt)[k];
  Loc2Gl(AMSEventR::Head());
  for(unsigned int k=0;k<sizeof(TOFCoo)/3/sizeof(TOFCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TOFCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TOFCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(EcalCoo)/3/sizeof(EcalCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(EcalCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)EcalCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TrCoo)/3/sizeof(TrCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TrCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TrCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(TRDCoo)/3/sizeof(TRDCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(TRDCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)TRDCoo[k][l]=pnt[l];
  }  
  for(unsigned int k=0;k<sizeof(RichCoo)/3/sizeof(RichCoo[0][0]);k++){
   AMSPoint pnt;
   AMSDir dir;
   newtrack->Interpolate(RichCoo[k][2],pnt,dir,iddflt);
   for(int l=0;l<3;l++)RichCoo[k][l]=pnt[l];
  }  
  delete newtrack;
  fTrTrack= AMSEventR::Head()->fTrTrack.size()-1;
//  cout <<" fTrTrack "<<fTrTrack<<endl;
  TofRecH::ReBuild(Charge);
    
    
  return 0;
 
     }
   }
   else{
      delete newtrack;
      if(trdecal){
           trdecal=false;

           goto again;
      }
      return -7;
   }   

      
   
}
}
else{
 return 16;
}
#else
return 10;
#endif
}


void ParticleR::_calcmass(float momentum,float emomentum, float beta, float ebeta, float &mass, float &emass){
  if(fabs(beta)<=1.e-10 ){
    mass=FLT_MAX;
    emass=FLT_MAX;
  }
  else{ 
    double one=1;
    double xb=1/fabs(beta);
    if(xb<one)xb=2-xb;
    double gamma2=fabs(beta)!=1?one/(one-beta*beta):FLT_MAX; 
    double mass2=momentum*momentum*(xb*xb-one);
    mass=gamma2>0? sqrt(mass2) : -sqrt(mass2);
    emass=fabs(mass)*sqrt((emomentum/momentum)*(emomentum/momentum)+
			  (gamma2*ebeta/beta)*(gamma2*ebeta/beta));
  }
}

void  ParticleR::_build(double rid,double err,float charge,float beta, float ebeta,float & mass,float & emass,float & momentum,float & emomentum){
  momentum=rid*charge;
  emomentum=err*rid*rid*charge;
  _calcmass(momentum,emomentum,beta,ebeta,mass,emass);
  if(beta<0)momentum=-momentum;
  
}



#ifdef _PGTRACK_
int  UpdateInnerDz(){
  // not active for MC events!!
  if(AMSEventR::Head()->nMCEventgC() ) return 0;

  uint time;
#ifdef __ROOTSHAREDLIBRARY__ 
  time=AMSEventR::Head()->UTime();
#else
  time=AMSEvent::gethead()->gettime();
#endif
  // PZ Update also the Inner DzDB
  return TrInnerDzDB::GetHead()->UpdateTkDBc(time);
}
int  UpdateExtLayer(int type=0,int lad1=-1,int lad9=-1){
  // not active for MC events!!
  if(AMSEventR::Head()->nMCEventgC() ) return 0;

  //type 0 PG; 1 Madrid
  uint time,run;
#ifdef __ROOTSHAREDLIBRARY__ 
  time=AMSEventR::Head()->UTime();
  run=AMSEventR::Head()->Run();
#else
  time=AMSEvent::gethead()->gettime();
  run=AMSEvent::gethead()->getrun();
#endif
//   if(TrExtAlignDB::ForceLocalAlign){
//     int ret2=0;
//     char filenam[400];
//     if (type==0) {
//       sprintf(filenam,"%s/v5.00/PGExtLocalAlign.txt",getenv("AMSDataDir"));
//       ret2=TkDBc::Head->LoadPGExtAlign(filenam);
//     }
//     else{
//       sprintf(filenam,"%s/v5.00/MDExtLocalAlign.txt",getenv("AMSDataDir"));
//       ret2=TkDBc::Head->LoadMDExtAlign(filenam);
//     }
//     if(ret2!=0) return ret2;
//   } 

  int ret=0;
  if(type==0)
    ret=TrExtAlignDB::GetHead()->UpdateTkDBc(time);
  else  if(type==1){
    //    printf("updating withh l1 %d ln%d \n",lad1,lad9);
    ret=TrExtAlignDB::GetHead()->UpdateTkDBcDyn(run,time,3,lad1,lad9);
  }
  else  {
    //    printf("updating withh l1 %d ln%d \n",lad1,lad9);
    ret=TrExtAlignDB::GetHead()->UpdateTkDBc(time);
    ret=TrExtAlignDB::GetHead()->UpdateTkDBcDyn(run,time,3,lad1,lad9);
  }

  return ret;
} 

int MCtune(AMSPoint &coo, int tkid, double dmax, double ds)
{
#ifdef __ROOTSHAREDLIBRARY__
  if (!AMSEventR::Head()) return 0;
  if (AMSEventR::Head()->Version() >= 817) return 0;
  if (AMSEventR::Head()->NTrMCCluster() == 0) return 0;

  TrMCClusterR *mc = 0;
  double      dmin = dmax;
  for (unsigned int i = 0; i < AMSEventR::Head()->NTrMCCluster(); i++) {
    TrMCClusterR *m = AMSEventR::Head()->pTrMCCluster(i);
    if (!m || m->GetTkId() != tkid) continue;

    double d = coo.y()-m->GetXgl().y();
    if (TMath::Abs(d) < TMath::Abs(dmin)) {
      mc   = m;
      dmin = d;
    }
  }
  if (mc) {
   if(ds<0){
   double rnd[1];
#ifdef __ROOTSHAREDLIBRARY__
  int lj=TkDBc::Head?TkDBc::Head->GetJFromLayer(abs(tkid)/100):0;
  AMSEventR::GetRandArray(8993306+lj, 2,  1,rnd);
#else
rnd[1]=rnormx();
#endif
     coo[1]+=-ds*rnd[1];
     return 1;
   }
   else{
    if (ds < dmax && TMath::Abs(dmin) > ds) {
      coo[1] += (dmin > 0) ? -ds : ds;
      return 1;
    }
    if (ds > dmax) {
      coo = mc->GetXgl();
      return 1;
    }
  }}
 

#endif
  return 0;
}

int MCshift(AMSPoint &coo, double ds)
{
#ifdef __ROOTSHAREDLIBRARY__
  if (!AMSEventR::Head()) return 0;
  if (AMSEventR::Head()->Version() >= 817) return 0;

  MCEventgR *mc = AMSEventR::Head()->GetPrimaryMC();
  if (!mc || mc->Charge == 0) return 0;

  double rgen = mc->Momentum/mc->Charge;
  double dy   = ds*(2.5-0.75*TMath::Erfc(-(rgen-500)/100));
  coo[1] -= dy;
  return 1;

#endif
  return 0;
}

int MCscat(AMSPoint &coo, int layj, double prob, double scat, double pwr)
{
#ifdef __ROOTSHAREDLIBRARY__
  if (layj != 1 && layj != 9) return 0;

  MCEventgR *mc = AMSEventR::Head()->GetPrimaryMC();
  if (!mc || mc->Charge == 0) return 0;

  double rnd[2];
  AMSEventR::GetRandArray(90818097+layj, 1, 1, rnd);
  if (rnd[0] > prob) return 0;

  double rgen = mc->Momentum/mc->Charge;
  double rr   = pow(rgen/10, pwr);
  double sig  = (layj == 1) ? scat/rr*1.0 : scat/rr*0.7;

  AMSEventR::GetRandArray(8993306+layj, 2, 2, rnd);
  coo[0] += rnd[0]*sig;
  coo[1] += rnd[1]*sig;
  return 1;

#endif
  return 0;
}


int MCscatq2(AMSPoint &coo, int layj, float b,float prob){
// b (gev/c)^-2  coefficient  ( ~ -15 )  exp(b*q2)
// prob probability  of quasi-elastic scattering layer 1/9  (~ 0.05/0.04)
 
#ifdef __ROOTSHAREDLIBRARY__
  if (layj != 1 && layj != 9) return 0;
  MCEventgR *mc = AMSEventR::Head()->GetPrimaryMC();
  if (!mc || mc->Charge == 0) return 0;
  double p = mc->Momentum;
  double lp=log10(p);
  double c=log10(2.);
  if(lp<c)lp=c;
  if(lp>2)lp=2;

  double rnd[4];
  AMSEventR::GetRandArray(57538922+layj, 1, 1, rnd);
  if ( rnd[0]>prob*(3-lp)/2.)return 0;

  AMSEventR::GetRandArray(9886838+layj, 1, 4, rnd);

  int lay=(layj==1)?0:1;
  double q2=log(rnd[0])/b;
  double mass[2]={12,27}; //  scattering masses
  double pmass[2]={0.85,0.15}; // probability
  double l1[2]={104,86}; // length for scattering l1,l9 
  double m1=mc->Mass;
  double m2=mass[0];
  if(rnd[1]>pmass[0])m2=mass[1]; 
  double e=sqrt(p*p+m1*m1);
  double g=e+m2;
  double p1=g*g+q2+m2*m2-m1*m1-2*g*sqrt(q2+m2*m2);
  if(p1<0)p1=0;
  p1=sqrt(p1);
  double p1_par=p/2+p1/p/2*p1-q2/2/p;
  if(fabs(p1)<fabs(p1_par)){
    cout<<"MCscatq2-W-too big q^2 requested "<<q2<<endl;
    return 0;
  }
  double p1_per=sqrt((p1-p1_par)*(p1+p1_par));
  double shift=p1_per/p1_par*l1[lay]*rnd[2];
  double phi=2*3.1415926*rnd[3];
  
  
    
  coo[0] += shift*cos(phi);
  coo[1] += shift*sin(phi);
  return 1;

#endif
  return 0;
}



int DropExtHits(void)
{
  if (!AMSEventR::Head()) return 0;

  int ndrop = 0;

  for (unsigned int i = 0; i < AMSEventR::Head()->NTrTrack(); i++) {
    TrTrackR *trk = AMSEventR::Head()->pTrTrack(i);
    if (trk && trk->HasExtLayers()) {
      trk->RecalcHitCoordinates();
      ndrop += trk->DropExtHits();
    }
  }
  return ndrop;
}
#endif //_PGTRACK_


#ifdef __ROOTSHAREDLIBRARY__
int AMSEventR::GetRandArray(int key, int type, int n, double *array)
{
  int seed = 4357;
  if (AMSEventR::Head()) seed = AMSEventR::Head()->fHeader.RNDMSeed[0];

  TRandom3 rnd1(key);
  seed ^= rnd1.Integer(0x7fffffff);

  TRandom3 rand(seed);
  for (int i = 0; i < n; i++)
    array[i] = (type == 2) ? rand.Gaus() : rand.Rndm();

  return 0;
}

#endif

double HeaderR::UTCTime(int mode) const {

double error=10;
double etime=double(Time[0])+double(Time[1])/1000000.+TimeCorr(error,Time[0])-AMSEventR::gpsdiff(Time[0]);
if(mode==1){
    unsigned int time,nanotime;
    if (AMSEventR::Head() && !AMSEventR::Head()->GetGPSTime(time, nanotime)){
      double xtime= double(time)+double(nanotime)/1.e9-AMSEventR::gpsdiff(Time[0]);
      if(fabs(etime-xtime)<3*error){
       return xtime;
      }
    }
}
return etime;
}
int AMSEventR::UTCTime(double &xtime, double &error) {
int ret=0;
error=10;
xtime=double(UTime())+Frac()+fHeader.TimeCorr(error,UTime())-gpsdiff(UTime());
    unsigned int time,nanotime;
    if (!GetGPSTime(time, nanotime)){
           
      double etime= double(time)+double(nanotime)/1.e9-gpsdiff(UTime());
      if(fabs(etime-xtime)<3*error){
       xtime=etime;
      }
      else ret=1;
    }
    else ret=2;
  
    return ret;
}

double HeaderR::TimeCorr(double & error,unsigned int time) const{
if(!AMSEventR::getsetup()){
error=10;
return 0;
}
double corr;

AMSEventR::getsetup()->GetJMDCGPSCorr(corr,error,time);
return corr;
}

int AMSEventR::gpsdiff(unsigned int time){
unsigned int t16=1341100786+43200;
if(time<t16)return 15;
else return 16;
}

int HeaderR::getISSTLE(double dt){
if(!AMSEventR::getsetup())return 2;
double xtime=UTCTime()+dt;
float RTP[3],VelTP[3];

int ret= AMSEventR::getsetup()->getISSTLE(RTP,VelTP,xtime);
if(!ret){
 RadS=RTP[0];
 ThetaS=RTP[1];
 PhiS=RTP[2];
 VelocityS=VelTP[0];
 VelTheta=VelTP[1];
 VelPhi=VelTP[2];
 ThetaM=GetThetaM();
}
return ret;
}

#ifdef _PGTRACK_
#ifdef __ROOTSHAREDLIBRARY__
#include "amschain.h"
#include "bcorr.h"

int AMSEventR::DumpTrTrackPar(int run, int event, int itrack)
{
  TString dir = "/eos/ams/Data/AMS02/2011B/ISS.B620/pass4";
  TString xrd = "root://eosams.cern.ch/"+dir+"/";
  TString eos = "/afs/cern.ch/project/eos/installation/0.3.15/bin/eos.select";

  static AMSChain ach;
  static TString  str;
  static int srun = 0;

  if (run != srun) {
    str = gSystem->GetFromPipe(eos+" ls "+dir+Form(" | grep %d", run));

    TObjArray *sar = str.Tokenize("\n");
    if (sar->GetEntries() == 0) {
      cout << "AMSEventR::DumpTrTrackPar-E-No AMSRoot file found" << endl;
      delete sar;
      return -1;
    }
    cout << "AMSEventR::DumpTrTrackPar-I-Number of files found: "
	 << sar->GetEntries() << endl;

    ach.Reset();
    for (int i = 0; i < sar->GetEntries(); i++)
      ach.Add(xrd+sar->At(i)->GetName());
    delete sar;

    int ntr = ach.GetNtrees();
    int nen = ach.GetEntries();
    if (ntr <= 0 || nen <= 0) {
      cout << "AMSEventR::DumpTrTrackPar-E-Invalid Ntrees,Entreis: "
	   << ntr << " " << nen << endl;
      return -1;
    }
    srun = run;
  }

  int ntry =  0;
  int eofs = -1;
  AMSEventR *evt = 0;
  while ((!evt || int(evt->Event()) != event) && ntry++ < 5) {
    evt   = ach.GetEvent(event+eofs);
    eofs -= evt->Event()-event;
  }

  if (!evt) {
    cout << "AMSEventR::DumpTrTrackPar-E-No event found for: "
	 << event << endl;
    return -1;
  }
  if (int(evt->Event()) != event) {
    cout << "AMSEventR::DumpTrTrackPar-E-Event number mismatch: " 
	 << evt->Event() << " " << event << endl;
    return -1;
  }

  float bcor = -1;
  float bcor1 = 1, bcor2 = 1;
  int bret1 = MagnetVarp::btempcor(bcor1, 0, 1);
  int bret2 = MagnetVarp::btempcor(bcor2, 0, 2);
  if      (bret1 == 0 && bret2 == 0) bcor = (bcor1+bcor2)/2; 
  else if (bret1 != 0 && bret2 == 0) bcor =        bcor2;

  TrTrackR *trk = evt->pTrTrack(itrack);
  if (!trk) {
    cout << "AMSEventR::DumpTrTrackPar-E-TrTrack not found" << endl;
    return -1;
  }

  int itp0 = trk->iTrTrackPar(1, 3,  3);
  int itp1 = trk->iTrTrackPar(1, 7, 23);
  cout << endl;
  cout << "AMSEventR::DumpTrTrackPar-I-Dump: " << endl;
  cout << "Run/Event : " << evt->Run() << " " << evt->Event() << endl;
  cout << Form("btempcor= %.4f", bcor) << endl;
  cout << Form("iTrTrackPar(1, 3,  3)= %7d", itp0);
  if (itp0 > 0) {
    cout << Form(" Rigidity= %9.3f",   trk->GetRigidity  (itp0));
    cout << Form(" NormChisqY= %8.3f", trk->GetNormChisqY(itp0));
  }
  cout << endl;
  cout << Form("iTrTrackPar(1, 7, 23)= %7d", itp1);
  if (itp1 > 0) {
    cout << Form(" Rigidity= %9.3f",   trk->GetRigidity  (itp1));
    cout << Form(" NormChisqY= %8.3f", trk->GetNormChisqY(itp1));
  }
  cout << endl;

  TrRecHitR *hit1 = trk->GetHitLJ(1);
  TrRecHitR *hit9 = trk->GetHitLJ(9);
  if (hit1) {
    AMSPoint cpg = hit1->GetCoord(-1, 1);
    AMSPoint cmd = hit1->GetCoord(-1, 2);
    cout << Form("Hit(L1): PG= %8.4f %8.4f %9.4f  MD= %8.4f %8.4f %9.4f",
		 cpg.x(), cpg.y(), cpg.z(), cmd.x(), cmd.y(), cmd.z())
	 << endl;
  }
  if (hit9) {
    AMSPoint cpg = hit9->GetCoord(-1, 1);
    AMSPoint cmd = hit9->GetCoord(-1, 2);
    cout << Form("Hit(L9): PG= %8.4f %8.4f %9.4f  MD= %8.4f %8.4f %9.4f",
		 cpg.x(), cpg.y(), cpg.z(), cmd.x(), cmd.y(), cmd.z())
	 << endl;
  }

  return 0;
}
#endif

int AMSEventR::IsInsideTracker(int ilyJ, const AMSPoint &pntIn,
			                 const AMSDir   &dirIn,
			       double rigidity, double tolerance,
			       int &tkidOut,   AMSPoint &pntOut,
			                       AMSPoint &distOut)
{
  TrProp trp(pntIn, dirIn, rigidity);
  AMSDir dir(0, 0, 1);

  int ily = TkDBc::Head->GetLayerFromJ(ilyJ);
  AMSPoint pnt(0, 0, TkDBc::Head->GetZlayer(ily));
  trp.Interpolate(pnt, dir);

  AMSPoint dmin;
  AMSPoint pmin;
  int      tmin = 0;
  double   smin = 0;
  double   dtol = 0.1;
  double   xmin = 0, xmax = 0;
  double   ymin = 0, ymax = 0;

  double   hwid = TkDBc::Head->_ssize_active[1]/2;
  double   ylad = TkDBc::Head->_ladder_Ypitch/2+0.1;

  for (int s = -15; s <= 15; s++) {
    Int_t    tkid = TMath::Sign(ily*100+TMath::Abs(s), s);
    TkLadder *lad = TkDBc::Head->FindTkId(tkid);
    if (!lad || !lad->IsActive()) continue;

    AMSPoint pcen = TkCoo::GetLadderCenter(tkid);
    AMSPoint pint = pnt+dir*(pcen.z()-pnt.z())/dir.z();
    AMSPoint diff = pint-pcen;
    double   hlen = TkCoo::GetLadderLength(tkid) /2;
    double   difx = fabs(diff.x());
    double   dify = fabs(diff.y());

    if (difx < hlen-tolerance && dify < hwid-tolerance) {
      tkidOut = tkid;
       pntOut = pint;
      distOut.setp(hlen-difx, hwid-dify, 0);
      return 1;
    }

    if (dify < ylad) {
      if (xmin == 0 || pcen.x()-hlen < xmin) xmin = pcen.x()-hlen;
      if (xmax == 0 || pcen.x()+hlen > xmax) xmax = pcen.x()+hlen;
    }
    if (difx < hlen) {
      if (ymin == 0 || pcen.y()-hwid < ymin) ymin = pcen.y()-hwid;
      if (ymax == 0 || pcen.y()+hwid > ymax) ymax = pcen.y()+hwid;
    }

    double dist = std::max(difx-hlen, dify-hwid);
    if (tkidOut==999)
      cout<<Form("%4d | %5.1f %5.1f | %5.1f %5.1f %5.2f | %5.1f %5.1f",
		 tkid,difx-hlen,dify-hwid,dist,smin, fabs(dist-smin),
		 diff.norm(), dmin.norm());

    if (tmin == 0 || (dist < smin-dtol || (fabs(dist-smin) <= dtol &&
					   diff.norm() < dmin.norm()))) {
      tmin = tkid; dmin = diff;
      pmin = pint; smin = dist;

      if (tkidOut==999)cout<<" Min";
    }
    if (tkidOut==999)cout<<endl;
  }

  double hlen = TkCoo::GetLadderLength(tmin)/2;
  tkidOut = tmin;
   pntOut = pmin;
  distOut.setp(fabs(dmin.x())-hlen, fabs(dmin.y())-hwid, 0);

  if (xmin+tolerance < pmin.x() && pmin.x() < xmax-tolerance &&
      ymin+tolerance < pmin.y() && pmin.y() < ymax-tolerance) return 2;

  return 0;
}

double AMSEventR::GetRadiationLength(const AMSPoint &pnt,
				     const AMSDir   &dir,
				     double rigidity, double z1, double z2)
{
  static TFile *file = 0;
  static TH3F  *hist = 0;

  if (!hist) {
#pragma omp critical (getradiationlength)
    {
      if (!hist && !file) {
	TString sfn = getenv("AMSDataDir"); sfn += "/v5.01/g4mscan.root";
	if (getenv("G4MScan")) sfn = getenv("G4MScan");

	TDirectory *dsave = gDirectory;
	file = TFile::Open(sfn);
	if (dsave) dsave->cd();

	if (file) hist = (TH3F *)file->Get("hist1");
	if (hist)
	  cout << "AMSEventR::GetRadiationLength-I-Open file: "
	       << file->GetName() << endl;
	else {
	  cout << "AMSEventR::GetRadiationLength-E-hist1 not found" << endl;
	  hist = (TH3F *)1;
	}
      }
    }
  }
  if (!hist || hist == (TH3F *)1) return -1;

  TrProp trp(pnt, dir, rigidity);

  int ib1 = hist->GetZaxis()->FindBin(z1);
  int ib2 = hist->GetZaxis()->FindBin(z2);

  if (ib2 < ib1) { int swp = ib1 ; ib1 = ib2; ib2 = swp; }

  double ms = 0;
  for (int i = ib1; i <= ib2; i++) {
    double z = hist->GetZaxis()->GetBinCenter(i);
    trp.Propagate(z);
    int m = hist->GetBinContent(hist->GetXaxis()->FindBin(trp.GetP0x()),
				hist->GetYaxis()->FindBin(trp.GetP0y()), i);
    if (m != 0) ms += pow(10, m/10000.-3)/trp.GetD0z();
  }
  return ms;
}

int AMSEventR::GetElementAbundance(const AMSPoint &pnt,
				   const AMSDir   &dir,
				   double rigidity, double z1, double z2,
				   double elem[9])
{
  static TFile *file    = 0;
  static TH3F  *hist[9] = { 0, 0, 0, 0, 0, 0, 0 };

  for (int i = 0; i < 9; i++) elem[i] = 0;

  if (!hist[0]) {
#pragma omp critical (getelementabundance)
    {
      if (!hist[0] && !file) {
	TString sfn = getenv("AMSDataDir"); sfn += "/v5.01/g4elmap.root";
	if (getenv("G4elMap")) sfn = getenv("G4elMap");

	TDirectory *dsave = gDirectory;
	file = TFile::Open(sfn);
	if (dsave) dsave->cd();

	if (file) {
	  cout << "AMSEventR::GetElementAbundance-I-Open file: "
	       << file->GetName() << endl;
	  for (int i = 0; i < 9; i++)
	    hist[i] = (TH3F *)file->Get(Form("hist%d", i+1));
	}
	if (!hist[0] || !hist[8]) {
	  cout << "AMSEventR::GetElementAbundance-E-hist not found" << endl;
	  hist[0] = (TH3F *)1;
	}
      }
    }
  }
  if (!hist[0] || hist[0] == (TH3F *)1) return -3;

  TrProp trp(pnt, dir, rigidity);

  int ib1 = hist[0]->GetZaxis()->FindBin(z1);
  int ib2 = hist[0]->GetZaxis()->FindBin(z2);
  int nb  = hist[0]->GetNbinsZ();

  if (ib2 < ib1) { int swp = ib1 ; ib1 = ib2; ib2 = swp; }

  if (ib1 <  1) ib1 = 1;
  if (ib2 > nb) ib2 = nb;

  double zp = hist[0]->GetZaxis()->GetBinCenter(ib1);
         z2 = hist[0]->GetZaxis()->GetBinCenter(ib2);
  trp.Propagate(zp);
  AMSPoint p1 = trp.GetP0();

  int   ndv = 5;
  int   err = 0;

  do {
    double b = TMath::Abs(TrFit::GuFld(p1).x());
    double l = (b > 0) ? TMath::Abs(rigidity)/0.3/b : 0;
    if (l <= 0 || l > 10) l = 10;

    zp = p1.z()+l; if (zp > z2) zp = z2;
    trp.Propagate(zp);
    AMSPoint p2 = trp.GetP0();

    ib1 = hist[0]->GetZaxis()->FindBin(p1.z());
    ib2 = hist[0]->GetZaxis()->FindBin(p2.z());
    int ns = (ib2-ib1)*ndv;

    double dl = (p2-p1).norm();
    if (dl <= 0) break;

    double cosz = (p2.z()-p1.z())/dl;
    double dz   = hist[0]->GetZaxis()->GetBinWidth(ib1)/cosz/ndv;

    for (int i = 0; i < ns; i++) {
      double   z = p1.z()+(p2.z()-p1.z())/ns*(i+0.5);
      AMSPoint p = p1+(p2-p1)/(p2.z()-p1.z())*(z-p1.z());
      int     ix = hist[0]->GetXaxis()->FindBin(p.x());
      int     iy = hist[0]->GetYaxis()->FindBin(p.y());
      if (ix <= 0 || hist[0]->GetNbinsX() < ix ||
	  iy <= 0 || hist[0]->GetNbinsY() < iy) { err = -2; continue; }

      for (int j = 0; j < 9; j++)
	if (hist[j]) {
	  Double_t bc = hist[j]->GetBinContent(ix, iy, ib1+i/ndv);
	  if (bc > 0) elem[j] += bc*dz;
	}
    }

    p1 = p2;
  } while (zp < z2);

  return err;
}

double AMSEventR::GetCrossSection(int zp, int zt, double rgt, int model)
{
                    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14
  const int iZ[14] = { 0,-1,-1,-1,-1, 1, 2, 3, 4,-1, 5,-1, 6, 7 };
                    // 1  2  3  4  5  6  7  8
  const int iP[8]  = { 0, 1, 2, 4, 6, 8, 9,10 };  

  int ip = (1 <= zp && zp <=   8) ? iP[zp-1] :
        ((103 <= zp && zp <= 105) ? (zp-103)*2+3 : -1);
  int it = (1 <= zt && zt <= 14) ? iZ[zt-1] :
                     ((zt == 82) ? 8 : -1);

  if (ip < 0 || it < 0) return -1;

  static TFile *file = 0;
  static TH1D  *hist[9*11*3] = { 0 };

  if (!hist[0]) {
#pragma omp critical (getelementabundance)
    {
      if (!hist[0] && !file) {
	TString sfn = getenv("AMSDataDir"); sfn += "/v5.01/g4xsec.root";
	if (getenv("G4xSec")) sfn = getenv("G4xSec");

	TDirectory *dsave = gDirectory;
	file = TFile::Open(sfn);
       if (file) {
	if (dsave) dsave->cd();

	TDirectory *dir1 = (TDirectory *)file->Get("dir96");
	TDirectory *dir2 = (TDirectory *)file->Get("dir94");
	TDirectory *dir3 = (TDirectory *)file->Get("dir620");

	for (int i = 0; dir1 && i < 99; i++)
	  hist[i]     = (TH1D *)dir1->Get(Form("hist%d%d1", i/9+1, i%9+1));
	for (int i = 0; dir2 && i < 99; i++)
	  hist[i+99]  = (TH1D *)dir2->Get(Form("hist%d%d1", i/9+1, i%9+1));
	for (int i = 0; dir3 && i < 99; i++)
	  hist[i+198] = (TH1D *)dir3->Get(Form("hist%d%d1", i/9+1, i%9+1));
       }

	int nh = 0;
	for (int i = 0; i < 99*3; i++) if (hist[i]) nh++;

	if (nh == 99*3)
	  cout << "AMSEventR::GetCrossSection-I-Open file: "
	       << file->GetName() << endl;
	else {
	  cout << "AMSEventR::GetCrossSection-E-hist not found "
	       << nh << endl;
	  hist[0] = (TH1D *)1;
	}
      }
    }
  }
  if (!hist[0] || hist[0] == (TH1D *)1) return -2;

  int im = model-1;
  if (im < 0 || 2 < im) return -3;

  TH1D *hh = hist[ip*9+it+im*99];
  if (hh) return hh->Interpolate(fabs(rgt));

  return -1;
}

double AMSEventR::GetRelInteractionLength(const AMSPoint &pnt,
					  const AMSDir   &dir,
					  double rigidity,
					  double z1, double z2, int zp, 
					  int zt, int model, int norm)
{
                    // 1  2  3  4  5  6  7  8  9 10 11 12 13 14
  const int iZ[14] = { 0,-1,-1,-1,-1, 1, 2, 3, 4,-1, 5,-1, 6, 7 };
  int it = (0 <= zt && zt <= 14) ? iZ[zt-1] :
                     ((zt == 82) ? 8 : -1);
  if (zt > 0 && it < 0) return -1;

  double elm[9];
  if (GetElementAbundance(pnt, dir, rigidity, z1, z2, elm) < 0) return -2;

  double NA = 6.022e+23;  // Avogadro constant (1/mol)
  double mb =     1e-27;  // mb to cm^2

  if (zt > 0 && !norm) {
    double xsec = GetCrossSection(zp, zt, rigidity, model);
    return (xsec > 0) ? xsec*mb*NA*elm[it] : -3;
  }

  int zel[9] = { 1, 6, 7, 8, 9, 11, 13, 14, 82 };
  double intl = 0, sum = 0;
  for (int i = 0; i < 9; i++) {
    double xs = GetCrossSection(zp, zel[i], rigidity, model);
    if (xs < 0) return -3;

    double il = xs*mb*NA*elm[i];
    sum += il;
    if (zt == zel[i]) intl = il;
  }

  if (zt == 0 || intl == 0) return sum;
  return (sum > 0) ? intl/sum : -4;
}
#endif

int AMSEventR::GetNTofClustersInTime(BetaHR *betah, int ncls[4], float cutu,
				                                 float cutl)
{
  // Original code by VC/MH/EC; imported by SH
  if (!betah || !ncls) return -1;

  for (int i = 0; i < 4; i++) ncls[i] = 0;
      //-->look around(time) used hits:
      for(int il=0;il<4;il++){//layer loop
	int itb=(il<2)?0:1;
	if(betah->TestExistHL(il)){//hit exists
	//barn=betah->GetClusterHL(il)->Bar;//0-9
	  float ltime=betah->GetTime(il);//ns
	  int ntfcls=nTofClusterH();
	  for(int icl=0;icl<ntfcls;icl++){
	    TofClusterHR &tfcl=TofClusterH(icl);
	    if(tfcl.Layer!=il)continue;
	    if(tfcl.NBetaHUsed()>0)continue;//used by BetaHtfcl.Time();
	    if(!tfcl.IsGoodTime())continue;
	    float cltime=tfcl.Time;//ns
	    float dt=cltime-ltime;//later cluster has positive dt
	    //	       if(itb==0)prsh10->Fill(dt,1.);
	    //	       if(itb==1)prsh11->Fill(dt,1.);
	    float tcut(0);
	    if(itb==0)tcut=cutu;//ns,top
	    else tcut=cutl;//ns, bot
	    int itm(-1);
	    if(fabs(dt)<=tcut)itm=0;//around-time hits ("in time")
	    if(dt>tcut)itm=1;//later hits ("off time")
	    if(itm>=0){
	      int indx=itm+2*itb;
	      if(indx>=0 && indx<4){
		ncls[indx]++;
	      }
	    }
	  }//-->endof "secondary clust-loop"
	}//-->endof "beta-hit exists"
      }//-->endof "Tof layer loop"

  return ncls[0]+ncls[2];
}

double AMSEventR::GetMCCutoffWeight(double rgen, double rrec,
				    double sfac, LxMCcutoff::eRcutDist dist)
{
  LxMCcutoff *mcc = LxMCcutoff::GetHead();
  if (mcc &&
      (TMath::Abs(mcc->SafetyFactor-sfac) > 0.001 || mcc->RcutDist != dist)) {
    delete mcc;
    mcc = 0;
  }
  if (!mcc) {
    TH1D  *hbin = LxMCcutoff::GetBinning();
    TFile *fbin = 0;

    if (!hbin) {
      TString sfn = getenv("AMSDataDir"); sfn += "/v5.01/phe_bin.root";
      fbin = TFile::Open(sfn);
      if (!fbin) {
	cerr << "AMSEventR::GetMCCutoffWeight-E-Bin file not found" << endl;
	return -1;
      }
      hbin = (TH1D *)fbin->Get("hist2");
      if (!hbin) {
	cerr << "AMSEventR::GetMCCutoffWeight-E-Bin histogram not found"
	     << endl;
	return -1;
      }
    }
    TString sfn = getenv("AMSDataDir"); sfn += "/v5.01/RcutAll.root";
    if (getenv("RcutAll")) sfn = getenv("RcutAll");

    int nBin = hbin->GetNbinsX();
    double *xBin = (double*)hbin->GetXaxis()->GetXbins()->GetArray();

    mcc = new LxMCcutoff(sfn, new TH1D("hbin",";bin",nBin,xBin), sfac, dist);

    if (!LxMCcutoff::GetHead()) {
      cerr << "AMSEventR::GetMCCutoffWeight-E-Error "
	      "in creating LxMCcutoff instance" << endl;
      delete mcc;
      return -1;
    }
    else
      cout << "AMSEventR::GetMCCutoffWeight-I-Creating LxMCcutoff instance "
	   << "with bin= " << hbin->GetName() << " " 
	   << "safetyfactor= " << sfac << " " 
	   << "dist= " << mcc->GetRcutI()->GetName() << endl;

    delete fbin;
  }

  int seed = (fHeader.RNDMSeed[0] > 0) ? fHeader.RNDMSeed[0] : -1;
  return mcc->GetWeight(rgen, rrec, seed);
}

void AMSEventR::GTOD2CTRS(double RPT[3], double v,double  VelPT[2]){
//
// only one function GTOD2CTRS or CTRS2GTOD should be active 
//

         return;         
         const double ve=3.1415926535*2./86400.;
         double vx=v*cos(VelPT[1])*cos(VelPT[0]);
         double vy=v*cos(VelPT[1])*sin(VelPT[0]);
         double vz=v*sin(VelPT[1]);
         double vxe=-ve*cos(RPT[2])*sin(RPT[1]);
         double vye=ve*cos(RPT[2])*cos(RPT[1]);
         vx-=vxe;
         vy-=vye;
         v=sqrt(vz*vz+vx*vx+vy*vy);
         VelPT[1]=asin(vz/v);
         VelPT[0]=atan2(vy,vx);
}
void AMSEventR::CTRS2GTOD(double RPT[3], double v,double  VelPT[2]){
         
         const double ve=3.1415926535*2./86400.;
         double vx=v*cos(VelPT[1])*cos(VelPT[0]);
         double vy=v*cos(VelPT[1])*sin(VelPT[0]);
         double vz=v*sin(VelPT[1]);
         double vxe=-ve*cos(RPT[2])*sin(RPT[1]);
         double vye=ve*cos(RPT[2])*cos(RPT[1]);
         vx+=vxe;
         vy+=vye;
         v=sqrt(vz*vz+vx*vx+vy*vy);
         VelPT[1]=asin(vz/v);
         VelPT[0]=atan2(vy,vx);
}

void AMSEventR::CTRS2GTOD(double theta, double phi, double v, double &vtheta, double &vphi)
{         
         const double ve=3.1415926535*2./86400.;
         double vx=v*cos(vtheta)*cos(vphi);
         double vy=v*cos(vtheta)*sin(vphi);
         double vz=v*sin(vtheta);
         double vxe=-ve*cos(theta)*sin(phi);
         double vye=ve*cos(theta)*cos(phi);
         vx+=vxe;
         vy+=vye;
         v=sqrt(vz*vz+vx*vx+vy*vy);
         vtheta=asin(vz/v);
         vphi=atan2(vy,vx);
}
bool AMSEventR::LoadFromTDV=true;
void AMSEventR::RebuildBetaH(){
///--Tof BetaHs Init
    if(Version()<592){
       fHeader.TofClusterHs = 0;
       TofClusterH().clear();
///---
       fHeader.BetaHs = 0;
       for(unsigned int i=0;i<NParticle();i++)pParticle(i)->setBetaH(-1);
       for(unsigned int i=0;i<NCharge();i++)pCharge(i)->setBetaH(-1);
       BetaH().clear();
     }

//---Fix For Gbatch
    else if( (Version()>=610&&Version()<=622)&&nMCEventg()==0){
//---
      TofRecH::Init();
//---TofClusterHR
      if(Version()<=621){   
        for(unsigned int i=0;i<NTofClusterH();i++){
           TofClusterHR *tfclh=pTofClusterH(i);
           if(!tfclh)continue;
           TofRecH::EdepRecR(tfclh->Layer,tfclh->Bar,tfclh->Aadc,tfclh->Dadc,tfclh->Coo[tfclh->GetDirection()],tfclh->AQ2,tfclh->DQ2,tfclh->AEdep,tfclh->DEdep);
        }
       }
      if(nTrTrack()>=2){//Recover Index
         TofRecH::BuildBetaH(0);
      }
//----BetaH
      else if(Version()<=621){        
        bool ftk=0;
        for(unsigned int i=0;i<NBetaH();i++){//First Search if Trd-Track Mode
           BetaHR *betah=pBetaH(i);
           if(!betah)continue;
           if(betah->iTrTrack()>=0||betah->iTrdTrack()>=0){ftk=1;break;}
         }
//----
        if(ftk){  //Track Mode or Trd Mode only need Part Reconstruction
         for(unsigned int i=0;i<NBetaH();i++){
           BetaHR *betah=pBetaH(i);
           if(!betah)continue;
           TofClusterHR *tfhit[4]={0};        
           double tklcoo[4]={0},tkcosz[4]={1,1,1,1};
           double time;AMSPoint pnt;AMSDir dir;
           for(int ilay=0;ilay<4;ilay++){
             tfhit[ilay]=betah->GetClusterHL(ilay);
             if(tfhit[ilay]){
               betah->TInterpolate(tfhit[ilay]->Coo[2],pnt,dir,time);
               tklcoo[ilay]=pnt[tfhit[ilay]->GetDirection()];
               tkcosz[ilay]=fabs(dir[2]);
            }
         } 
         TofBetaPar par=betah->gTofBetaPar();
         TofRecH::EdepTkAtt(tfhit,tklcoo,tkcosz,par);
         betah->SetTofBetaPar(par);
        }
      }
      else {
       TofRecH::BuildBetaH(0); 
      }
    }//End 621 
//---
  }//End 622
  if(Version()<632){
    for(unsigned int i=0;i<NBetaH();i++)pBetaH(i)->setChargeHI(-1);
  }
}




bool MCEventgR::Rebuild=true;

void AMSEventR::RebuildMCEventg(){
if(Version()<645){
for(int i=0;i<nMCEventg();i++){
MCEventgR & mc=MCEventg(i);
if(mc.Particle<0 && mc.parentID!=-2 ){
mc.Momentum=sqrt( (mc.Momentum+mc.Mass)*(mc.Momentum+mc.Mass)-mc.Mass*mc.Mass);
}
}
}
}

bool AMSEventR::nMCEventgC(){
bool mcc=false;
#ifdef __ROOTSHAREDLIBRARY__
if(AMSEventR::Head())mcc=AMSEventR::Head()->nMCEventg()>0;
#else
if(AMSJob::gethead())mcc=AMSJob::gethead()->isRealData()?false:true;
#endif
return mcc;
}

unsigned int AMSEventR::NTrTrackG(){
        if(fHeader.TrTracks && fTrTrack.size()==0)bTrTrack->GetEntry(_Entry);
        int ret=0;
#ifdef _PGTRACK_
       const unsigned int tfg= (unsigned int)(1<<31);
       for(unsigned int k=0;k<fTrTrack.size();k++){
         if(!fTrTrack[k].checkstatus(tfg))ret++;
      }
#endif
      return ret;
}


MCEventgR * AMSEventR::GetPrimaryMC(int pos) {

double zmin=1e9;
int kmin=-1;
int search=1;
bool newmc=Version()>700;
bool old=Version()<600;
if(pos<0)search=-1;
if(!newmc && search<0)return NULL;
for(unsigned int k=0;k<NMCEventg();k++){
MCEventgR &mc=MCEventg(k);
if(old && mc.Particle>0 && mc.Particle<256)return &mc; 
else if (mc.parentID==0 &&!newmc )return &mc; 
else if(mc.parentID==-2 && mc.Particle>0)return &mc; 
else if( newmc && mc.parentID==0){
if(mc.Dir[2]){
 double z=mc.Coo[2]/(mc.Dir[2]>0?search:-search);
 if(zmin>z){
  zmin=z; 
  kmin=k;
}
else if(kmin<0)return &mc;
}

}
}
if(kmin>=0)return & MCEventg(kmin);
else return NULL;
}
