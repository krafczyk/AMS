//  $Id: root.C,v 1.415 2012/07/05 17:12:50 afiasson Exp $

#include "TRegexp.h"
#include "root.h"
#include "ntuple.h"
#include <TRandom.h>
#include "TSystem.h"
#include "TXNetFile.h"
#include <TChainElement.h>
#include "TFile.h"
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
#include "Tofrec02_ihep.h"
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
  TH1D *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hcdir(save);
    hbook1(id2,title,n,a,b);
    TH1D *h2p = h1(id2);
    if(h2p){
      for (int i=0;i<n+2;i++){
	h2p->SetBinContent(i,h1p->GetBinContent(i));
	h2p->SetBinError(i,h1p->GetBinError(i));
      
      }
    }
  }
  else{
    TH2D *h2p = h2(id1);
    if(h2p){
    }
    else{
      TProfile *hpp=hp(id1);
      if(hpp){
      }
    }
  }
 
}

void AMSEventR::hcopy(int id1,int id2){
  TH1D *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hbook1(id2,title,n,a,b);
    TH1D *h2p = h1(id2);
    if(h2p){
      for (int i=0;i<n+2;i++){
	h2p->SetBinContent(i,h1p->GetBinContent(i));
	h2p->SetBinError(i,h1p->GetBinError(i));
      
      }
    }
  }
  else{
    TH2D *h2p = h2(id1);
    if(h2p){
    }
    else{
      TProfile *hpp=hp(id1);
      if(hpp){
      }
    }
  }
}
void AMSEventR::hdivide(int id1,int id2,int id3){
  TH1D *h2p = h1(id2);
  if(h2p){
    //   h2p->Sumw2();
    hcopy(id1,id3);
    TH1D *h1p = h1(id3);
    if(h1p){
      //    h1p->Sumw2();
      h1p->Divide(h2p);
    }   
  }
}
void AMSEventR::hscale(int id1,double fac,bool calcsumw2){
  TH1D *h2p = h1(id1);
  if(h2p){
    if(calcsumw2)h2p->Sumw2();
    h2p->Scale(fac);
  }
}
void AMSEventR::hsub(int id1,int id2,int id3){
  TH1D *h2p = h1(id2);
  if(h2p){
    h2p->Sumw2();
    hcopy(id1,id3);
    TH1D *h1p = h1(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
    }   
  }
}
void AMSEventR::hadd(int id1,int id2,int id3){
  TH1D *h2p = h1(id1);
  if(h2p){
    h2p->Sumw2();
    if(id2!=id3)hcopy(id2,id3);
    TH1D *h1p = h1(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,1);
    }   
  }
}
void AMSEventR::hbook1s(int id,const char title[], int ncha, float  a, float b,int howmany,int shift){
  for (int i=0;i<howmany;i++){
    hbook1(id+shift*i,title,ncha,a,b);
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

void AMSEventR::hprint(int idd, char opt[]){
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
void AMSEventR::hlist(char ptit[]){
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
  hfetch(*f,file,id);
  hcdir(file);
  // f->Close();
  // delete f;
}
#include <TKey.h>


void AMSEventR::hfetch(TFile &f, const char dir[],int idh){
  int fetch1=0;
  int fetch2=0;
  int fetchp=0;
  TIter nextkey(f.GetListOfKeys());
  TKey *key;
  while (key = (TKey*)nextkey()) {
    TObject *to=(key->ReadObj());
    TH1D * f1 = dynamic_cast<TH1D*>(to);
    if(f1){
      TString t(f1->GetName());
      if(t.BeginsWith("hb1")){
        for(int i=4;i<strlen(f1->GetName());i++){
          if(i>4 && f1->GetName()[i] =='_'){
	    TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
              int idd=st.Atoi(); 
              AMSID id(idd,dir);
              if(!idh || idh==idd)fetch1++;
              if(Service::hb1.find(id) == Service::hb1.end()){
              }
              else{
		cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		Service::hb1.erase((Service::hb1.find(id)));
              }
              if(!idh || idh==idd)(Service::hb1).insert(make_pair(id,f1));
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
	  for(int i=4;i<strlen(f1->GetName());i++){
	    if(i>4 && f1->GetName()[i] =='_'){
	      TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
		int idd=st.Atoi(); 
		AMSID id(idd,dir);
		if(!idh || idh==idd)fetch2++;
		if(Service::hb2.find(id) == Service::hb2.end()){
		}
		else{
		  cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		  Service::hb2.erase((Service::hb2.find(id)));
		}
		if(!idh || idh==idd)(Service::hb2).insert(make_pair(id,f1));
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
	    for(int i=4;i<strlen(f1->GetName());i++){
	      if(i>4 && f1->GetName()[i] =='_'){
		TString st(f1->GetName()+4,i-4);
	      TString st1(f1->GetName()+5,i-5);
	      if(st.IsDigit() || (st[0]=='-' && st1.IsDigit())){
		  int idd=st.Atoi(); 
		  AMSID id(idd,dir);
		  if(!idh || idh==idd)fetchp++;
		  if(Service::hbp.find(id) == Service::hbp.end()){
		  }
		  else{
		    cerr<<"  AMSEventR::hfetch-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
		    Service::hbp.erase((Service::hbp.find(id)));
		  }
		  if(!idh || idh==idd)(Service::hbp).insert(make_pair(id,f1));
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

void AMSEventR::hf1s(int id, float a, bool cuts[], int ncuts, int icut,int shift,float w){
  //  Fill the series of histos 
  //  1st before cuts
  //  before cut icut-1
  //  before icut-1 as last cut
  //  after icut-1 as last cut
  //  after last cut as first cut
  //  after cut icut-1 as first cut
  hf1(id,a,w);
  bool cut=true;
  if(icut-1>0)for(int k=0;k<icut-1;k++)cut=cut && cuts[k];
  if(cut)hf1(id+shift,a,w);
  for(int k=icut;k<ncuts;k++)cut=cut && cuts[k];
  if(cut)hf1(id+shift+shift,a,w);
  if(cut && cuts[icut-1])hf1(id+shift+shift+shift,a,w);             
  if(cuts[ncuts-1])hf1(id+shift+shift+shift+shift,a,w);             
  if(icut-1>=0 && cuts[icut-1])hf1(id+shift+shift+shift+shift+shift,a,w);             
}

void AMSEventR::hf1(int idd, float a, float w){
  static int sem[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   
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
  static int sem[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
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

  float MvOnPMT[nPMTs][nsuplayrs]={0};

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
  int hitPMT[nsuplayrs][nPMTs]={0};
  int dummyint=0;
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
				if(((antipatt>>i)&1==1) || ((antipatt>>(i+8))&1==1)) nanti++;
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
	
	bool TOF_OK;
	bool layer_ok[4]={false,false,false,false};
	for(int irc=0; irc<NTofRawSide(); irc++) {
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
char  AntiClusterR::_Info[255];
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
char HeaderR::_Info[255];


TTree*     AMSEventR::_Tree=0;
TTree*     AMSEventR::_TreeSetup=0;
TTree*     AMSEventR::_ClonedTree[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
TTree*     AMSEventR::_ClonedTreeSetup[32]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
AMSEventR* AMSEventR::_Head=0;
AMSEventR::Service*    AMSEventR::pService=0;
int AMSEventR::_Count=0;
int AMSEventR::_NFiles=-1;
int AMSEventR::_Entry=-1;
unsigned long long  AMSEventR::_Lock=0;
int AMSEventR::_EntrySetup=-1;
AMSEventR::MY_TLS_ITEM  AMSEventR::_RunSetup;
char* AMSEventR::_Name="ev.";   

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
  static double dif;
  static double difm;
  static double dif2;
#pragma omp threadprivate(runo,evento,dif,difm,dif2)
  fStatus=0;
  if(bStatus){
    int j;
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
#pragma omp threadprivate (local_pfile)
  if(i>0){
    if( local_pfile!=_Tree->GetCurrentFile() || entry==0){
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

    if(Version()<160){
      // Fix rich rings
      NRichHit();
      for(int i=0;i<NRichRing();i++){
	RichRingR *ring=pRichRing(i);
	ring->FillRichHits(i);
      }
    }

    // Build corrections for each PMT
    if (RichRingR::loadPmtCorrections && AMSEventR::Head()->nMCEventg()) {
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
    if(RichRingR::shouldLoadCorrection==RichRingR::fullUniformityCorrection && 
       !RichBetaUniformityCorrection::getHead()){
      if(!RichBetaUniformityCorrection::Init()) cout<<"*********************** Failed to load RICH velocity uniformity corrections. Skiping."<<endl; 
      if(RichRingR::isCalibrating()) 
	cout<<"RICH Uniformity Corrections disable RICH dynamic calibration. If the latter is required, "<<endl
	    <<"consider setting RichRingR::shouldLoadCorrection=RichRingR::tileCorrection before starting"<<endl
	    <<"the event loop"<<endl; 
      RichRingR::shouldLoadCorrection=RichRingR::tileCorrection;
    }
    // Rich Uniformity Charge Correction Loading. Only once per run
#pragma omp critical(rd)
    if(RichRingR::loadChargeUniformityCorrection && !RichChargeUniformityCorrection::getHead()){
      if(!RichChargeUniformityCorrection::Init()) cout<<"*********************** Failed to load RICH charge uniformity corrections. Skiping."<<endl; 
      RichRingR::loadChargeUniformityCorrection=false;
    }

    // Rich Default Beta Correction Loading. Only once per run
    if(fHeader.Run!=runo && !nMCEventg())
#pragma omp critical(rd)
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

    // Rich Dynamic Calibration
    RichRingR::_isCalibrationEvent=false;
    if(!RichBetaUniformityCorrection::getHead())
    if(RichRingR::isCalibrating() && RichRingR::calSelect(*this)){
#pragma omp critical (rd)
	 RichRingR::updateCalibration(*this); 
    }


    if(fHeader.Run!=runo){
      cout <<"AMSEventR::ReadHeader-I-NewRun "<<fHeader.Run<<endl;
      if(!UpdateSetup(fHeader.Run)){
         cerr<<"AMSEventR::UpdateSetup-E-UnabletofindSetupEntryfor "<<fHeader.Run<<endl;
       }
      runo=fHeader.Run;
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
  else{ cout<<"AMSEventR::ctor-I-SingletonInitialized "<<this<<endl;
#ifdef _OPENMP
    cout <<"  thread "<<omp_get_thread_num()<<endl;
#endif
#ifdef __root__new
    TStreamerInfo::SetFactory(new TStreamerInfo()); 
#endif
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
    if(fTofMCPmtHit.size()<=MAXTOFMCPMT){
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
  for(int k=0;k<ptr->GPSL;k++)GPSTime.push_back(ptr->GPS[k]);
  VelocityS= ptr->VelocityS;
  VelTheta=  ptr->VelTheta;
  VelPhi=    ptr->VelPhi;
  ThetaM=    ptr->ThetaM;
  PhiM =     ptr->PhiM;
      TrStat=ptr->TrStat;
  Alpha=ptr->Alpha;    
  B1a=ptr->B1a;    
  B3a=ptr->B3a;    
  B1b=ptr->B1b;    
  B3b=ptr->B3b;
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
  ntdct = ptr->_ntdct<sizeof(tdct)/sizeof(tdct[0])?ptr->_ntdct:sizeof(tdct)/sizeof(tdct[0]);
  for(int i=0;i<ntdct;i++)tdct[i] = ptr->_tdct[i];
#endif
}


AntiClusterR::AntiClusterR(AMSAntiCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Sector = ptr->_sector;
  //Npairs = ptr->_npairs;
  //Time.clear();
  //TimeE.clear();
  //for(int i=0;i<ptr->_ntimes;i++){
  //  Time.push_back(ptr->_times[i]);
  //  TimeE.push_back(ptr->_etimes[i]);
//  }
  Ntimes = ptr->_ntimes;
  Npairs = ptr->_npairs;
  for(int i=0;i<sizeof(Times)/sizeof(Times[0]);i++)Times[i] = i<Ntimes?ptr->_times[i]:0;
  for(int i=0;i<sizeof(Timese)/sizeof(Timese[0]);i++)Timese[i] = i<Ntimes?ptr->_etimes[i]:0;

  Edep   = ptr->_edep;
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) ErrorCoo[i] = ptr->_ecoo[i];
#endif
}

AntiMCClusterR::AntiMCClusterR(AMSAntiMCCluster *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->_idsoft;
  for (int i=0; i<3; i++) Coo[i]=ptr->_xcoo[i];
  TOF    = ptr->_tof;
  Edep   = ptr->_edep;
#endif
}


double BetaR::GetTRDBetaCorr(int datamc){
if(datamc!=0)datamc=1;
              double p[2][3]={0.522677,-0.16927,0.676221,
                              0.522677,-0.16927,0.676221};
              double betamc1=0;
              for(int k=0;k<sizeof(p)/sizeof(p[0][0])/2;k++)betamc1+=p[datamc][k]*pow(fabs(Beta),k);
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
  BetaPar=ptr->_betapar;
#endif
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
  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  ErEnergyE   = ptr->_ErrEnergyPIC;
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
  float energy;
  if (partid==1 ) {
    // photon hypothesis
  }
  else{
    // electron hypothesis
  }
  // temporary
  switch (method){
  case 0:
    energy=EnergyC;
    break;
  case 1:
    energy=EnergyA;
    break;
  case 2:
    energy=EnergyE;
    break;
  default:
    energy=EnergyE;    
  }

  return energy;
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

float varBDT[23];
float cEnergyC;
float cMomentum;

float ESENLayerSigma[18],ESENS1tot[3],ESENS3tot[3],ESENS5tot[3],ESENShowerLatDisp[3],ESENShowerLongDisp;
float ESENShowerFootprint[3],ESENZprofile[4],ESENZprofileChi2,ESENZProfileMaxRatio,ESENEnergyFractionLayer[18];
float ESENS1S3[3],ESENS3S5[3],ESENS13R,ESENEnergy3C2,ESENEnergy3C3,ESENShowerMeanNorm,ESENShowerSigmaNorm;float ESENShowerSkewnessNorm,ESENShowerKurtosisNorm,ESENF2LEneDepNorm,ESENL2LFracNorm,ESENF2LFracNorm,ESENR3cmFracNorm;
float ESENR5cmFracNorm,ESENDifoSumNorm,ESENS3totxNorm,ESENS3totyNorm,ESENS5totxNorm,ESENS5totyNorm;
float ESENEcalHitsNorm,ESENShowerFootprintXNorm,ESENShowerFootprintYNorm;
float nEnergyA;



float EcalShowerR::EcalStandaloneEstimatorV2(AMSEventR *pev){

	float NewESEv2;
	float nEnergyC;
	float nEcalStandaloneEstimator,necalBDT,nMomentum,nEnergyE,nRigInn;

        NormaliseVariableLAPP();

   	float S1totL[18]={-20.};
   	float S3totL[18]={-20.};
   	float S5totL[18]={-20.};

        // Case Processing version <B584        
	if(pev->Version()<=584)
                nEnergyA = EnergyA;
        // Case Processing version >=B584
        if(pev->Version()>=584)                
		nEnergyA = EnergyA*1000.;
  
        float log10E = TMath::Log10(nEnergyA/1000.);

        if(log10E<0.4 || log10E>3.){
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
  ESENLayerSigma[15] = (LayerSigma[15] - (0.25737+log10E*1.56722+pow(log10E,2)*2.11807+pow(log10E,3)*-2.75949+pow(log10E,4)*0.977555+pow(log10E,5)*-0.111681))/( 0.652372+log10E*3.38822+pow(log10E,2)*-5.89548+pow(log10E,3)*3.67127+pow(log10E,4)*-1.03417+pow(log10E,5)*0.119647+pow(log10E,6)*-0.00272773);
  ESENLayerSigma[16] = (LayerSigma[16] - (0.234222+log10E*0.636947+pow(log10E,2)*4.52065+pow(log10E,3)*-4.5979+pow(log10E,4)*1.54902+pow(log10E,5)*-0.174913))/( 0.586191+log10E*3.46126+pow(log10E,2)*-4.58645+pow(log10E,3)*1.57458+pow(log10E,4)*0.191528+pow(log10E,5)*-0.194067+pow(log10E,6)*0.0269499);
  ESENLayerSigma[17] = (LayerSigma[17] - (0.159336+log10E*-0.0104728+pow(log10E,2)*5.62076+pow(log10E,3)*-5.25354+pow(log10E,4)*1.73196+pow(log10E,5)*-0.19523))/( 0.444376+log10E*3.58308+pow(log10E,2)*-3.7803+pow(log10E,3)*0.494137+pow(log10E,4)*0.702942+pow(log10E,5)*-0.297369+pow(log10E,6)*0.0343403);


if(Zprofile[3]!=0.) ESENZProfileMaxRatio = (Zprofile[2]/Zprofile[3]  - (1.0129+log10E*-0.763376+pow(log10E,2)*0.988021+pow(log10E,3)*-0.458768+pow(log10E,4)*0.0717106))/( 0.00100992+log10E*0.785363+pow(log10E,2)*-1.44717+pow(log10E,3)*1.12741+pow(log10E,4)*-0.393829+pow(log10E,5)*0.0504198);
else ESENZProfileMaxRatio = -10.;
ESENZprofile[3] = (Zprofile[3] - (4.50263+log10E*2.34572+pow(log10E,2)*0.143905+pow(log10E,3)*-0.0833355+pow(log10E,4)*0.0150589))/( 0.0434271+log10E*0.0464243+pow(log10E,2)*-0.0974044+pow(log10E,3)*0.0767125+pow(log10E,4)*-0.0251018+pow(log10E,5)*0.00290821);
ESENZprofile[2] = (Zprofile[2] - (3.67016+log10E*2.50798+pow(log10E,2)*-0.107036+pow(log10E,3)*0.18317+pow(log10E,4)*-0.0449247))/( 1.44785+log10E*0.131643+pow(log10E,2)*-1.50198+pow(log10E,3)*2.22714+pow(log10E,4)*-1.11549+pow(log10E,5)*0.179176);
ESENZprofile[1] = (Zprofile[1] - (0.326949+log10E*0.528463+pow(log10E,2)*-0.670308+pow(log10E,3)*0.339608+pow(log10E,4)*-0.0569746))/( 0.189716+log10E*0.0343689+pow(log10E,2)*-0.251494+pow(log10E,3)*0.255097+pow(log10E,4)*-0.102414+pow(log10E,5)*0.014367);
ESENZprofile[0] = (Zprofile[0] - (0.976082+log10E*-0.0705222+pow(log10E,2)*0.175072+pow(log10E,3)*-0.0914683+pow(log10E,4)*0.0137933))/( 0.165222+log10E*-0.249009+pow(log10E,2)*0.373086+pow(log10E,3)*-0.254006+pow(log10E,4)*0.0733925+pow(log10E,5)*-0.00739734);
ESENZprofileChi2 = (ZprofileChi2 - (8.50299+log10E*-13.0275+pow(log10E,2)*8.10907+pow(log10E,3)*-2.32256+pow(log10E,4)*0.254447))/( 3.19023+log10E*-3.17808+pow(log10E,2)*-0.446047+pow(log10E,3)*1.90832+pow(log10E,4)*-0.87448+pow(log10E,5)*0.123363);
ESENShowerFootprint[2] = (ShowerFootprint[2] - (6.30702+log10E*2.64898+pow(log10E,2)*-6.91719+pow(log10E,3)*4.00913+pow(log10E,4)*-0.690694))/( 2.57879+log10E*1.61286+pow(log10E,2)*-6.8939+pow(log10E,3)*5.43924+pow(log10E,4)*-1.7182+pow(log10E,5)*0.195773);
ESENShowerFootprint[1] = (ShowerFootprint[1] - (4.02898+log10E*2.04488+pow(log10E,2)*-3.08223+pow(log10E,3)*1.57129+pow(log10E,4)*-0.256888))/( 1.58523+log10E*-0.295658+pow(log10E,2)*-1.52979+pow(log10E,3)*1.33371+pow(log10E,4)*-0.407086+pow(log10E,5)*0.043129);
ESENShowerFootprint[0] = (ShowerFootprint[0] - (10.7414+log10E*4.30687+pow(log10E,2)*-10.1444+pow(log10E,3)*5.82378+pow(log10E,4)*-1.00416))/( 3.85793+log10E*0.288424+pow(log10E,2)*-6.63226+pow(log10E,3)*5.81433+pow(log10E,4)*-1.91808+pow(log10E,5)*0.224113);
ESENShowerLatDisp[2] = (ShowerLatDisp[2] - (19.0553+log10E*54.0248+pow(log10E,2)*-69.1774+pow(log10E,3)*29.9681+pow(log10E,4)*-4.15628))/( 15.5826+log10E*32.9272+pow(log10E,2)*-68.8799+pow(log10E,3)*45.2201+pow(log10E,4)*-12.5163+pow(log10E,5)*1.26381);
ESENShowerLatDisp[1] = (ShowerLatDisp[1] * (1.+VarLeakYA) - (10.5906+log10E*31.8959+pow(log10E,2)*-35.8796+pow(log10E,3)*13.6127+pow(log10E,4)*-1.66184))/( 8.70862+log10E*17.5474+pow(log10E,2)*-34.899+pow(log10E,3)*22.0739+pow(log10E,4)*-6.09356+pow(log10E,5)*0.634846);
// Remove additionnal correction 
ESENShowerLatDisp[0] = (ShowerLatDisp[0] * (1.+VarLeakXA) - (29.8002+log10E*88.9076+pow(log10E,2)*-110.919+pow(log10E,3)*46.8499+pow(log10E,4)*-6.38335))/( 19.7325+log10E*41.8142+pow(log10E,2)*-87.4022+pow(log10E,3)*58.9758+pow(log10E,4)*-17.2272+pow(log10E,5)*1.87766);
if(S5tot[2]) ESENS3S5[2] = (S3tot[2]/S5tot[2] - (0.925137+log10E*-0.100376+pow(log10E,2)*0.125556+pow(log10E,3)*-0.0499635+pow(log10E,4)*0.0065578))/( 0.0537135+log10E*-0.00338936+pow(log10E,2)*-0.0120451+pow(log10E,3)*-0.0114605+pow(log10E,4)*0.0100869+pow(log10E,5)*-0.00179196);
else ESENS3S5[2] = -20.;
if(S5tot[1]) ESENS3S5[1] = (S3tot[1]/S5tot[1] - (0.931109+log10E*-0.0265477+pow(log10E,2)*0.0315243+pow(log10E,3)*-0.0115648+pow(log10E,4)*0.00144984))/( 0.0478292+log10E*-0.012534+pow(log10E,2)*-0.0458203+pow(log10E,3)*0.044711+pow(log10E,4)*-0.015465+pow(log10E,5)*0.00188226);
else ESENS3S5[1] = -20.;
if(S5tot[0]) ESENS3S5[0] = (S3tot[0]/S5tot[0] - (0.921565+log10E*-0.175071+pow(log10E,2)*0.272039+pow(log10E,3)*-0.133165+pow(log10E,4)*0.0210554))/( 0.0428441+log10E*0.157715+pow(log10E,2)*-0.333628+pow(log10E,3)*0.22896+pow(log10E,4)*-0.067359+pow(log10E,5)*0.00729265);
else ESENS3S5[0] = -20.;
if(S3tot[0]) ESENS1S3[0] = (S1tot[0]/S3tot[0] - (0.622985+log10E*-0.243001+pow(log10E,2)*0.31769+pow(log10E,3)*-0.122334+pow(log10E,4)*0.0150953))/( 0.0946042+log10E*0.0751587+pow(log10E,2)*-0.0922105+pow(log10E,3)*-0.0159299+pow(log10E,4)*0.0304762+pow(log10E,5)*-0.00617571);
else ESENS1S3[0] = -20.;
ESENShowerLongDisp = (ShowerLongDisp - (11.6608+log10E*2.5556+pow(log10E,2)*-0.379147+pow(log10E,3)*-0.356563+pow(log10E,4)*0.0694614))/( 3.59225+log10E*-1.85411+pow(log10E,2)*-0.283144+pow(log10E,3)*0.856242+pow(log10E,4)*-0.398109+pow(log10E,5)*0.0618494);
ESENS1tot[0] = (S1tot[0] - (0.507233+log10E*-0.357514+pow(log10E,2)*0.593993+pow(log10E,3)*-0.286615+pow(log10E,4)*0.0443787))/( 0.153756+log10E*0.186214+pow(log10E,2)*-0.47659+pow(log10E,3)*0.313955+pow(log10E,4)*-0.0838449+pow(log10E,5)*0.00795356);
ESENEnergy3C3 = (Energy3C[2] - (0.830358+log10E*-0.14142+pow(log10E,2)*0.143214+pow(log10E,3)*-0.0440383+pow(log10E,4)*0.00393432))/( 0.0602811+log10E*0.0689515+pow(log10E,2)*-0.0959921+pow(log10E,3)*0.0137288+pow(log10E,4)*0.0125294+pow(log10E,5)*-0.00318491);
ESENS13R = (S13R - (0.726479+log10E*-0.0412848+pow(log10E,2)*-0.0995643+pow(log10E,3)*0.0911045+pow(log10E,4)*-0.0186589))/( 0.0440288+log10E*-0.00203543+pow(log10E,2)*0.0317023+pow(log10E,3)*-0.0455753+pow(log10E,4)*0.0201134+pow(log10E,5)*-0.00284211);
ESENEnergy3C2 = (Energy3C[1] - (0.977856+log10E*0.0272133+pow(log10E,2)*-0.015759+pow(log10E,3)*0.00327821+pow(log10E,4)*-0.000232916+pow(log10E,5)*0))/( 0.0152631+log10E*-0.0194549+pow(log10E,2)*0.0118655+pow(log10E,3)*-0.00409424+pow(log10E,4)*0.000934227+pow(log10E,5)*-0.000109527+pow(log10E,6)*0);



  const int nECAL_VIEWs  = 2;
   const int nLAYERs = 18;
   const int nCELLs  = 72;

   float EneDep = 0.; // Total energy deposited [GeV] (sum of every cell of every layer)

   float F2LEneDep = 0.; // Energy deposit [GeV] in the first 2 layer

   float NEcalHits = Nhits;

   float L2LFrac = 0.; // Energy fraction of last 2 layer wrt energy deposit
   float F2LFrac = 0.; // Energy fraction of first 2 layer wrt energy deposit

   float ShowerFootprintX = ShowerFootprint[1];
   float ShowerFootprintY = ShowerFootprint[2];
   float S1S3x  = S13Rpi[0]; // Energy fraction in the cells near the maximum deposit cell on x wrt maximum deposit
   float S1S3y  = S13Rpi[1]; // Energy fraction in the cells near the maximum deposit cell on y wrt maximum deposit
   float S3totx = S3tot[1];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S3toty = S3tot[2];  // Energy fraction of the 2 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5totx = S5tot[1];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit
   float S5toty = S5tot[2];  // Energy fraction of the 4 cells near the maximum deposit cell on x wrt to total energy deposit

   float R3cmFrac = Energy3C[0]; // Energy fraction in a circle +- 3 cm around maximum wrt to total energy
   float R5cmFrac = Energy3C[1]; // Energy fraction in a circle +- 5 cm around maximum wrt to total energy

   float LayerClusterEnergy[nLAYERs];      // Corrected energy deposit [GeV] in each layer (sum of clusters' energy for each layer)
   float LayerTruncClusterEnergy[nLAYERs]; // Corrected energy [GeV] for each layer between 3 sigma from the maximum

   float ShowerMean  = 0.; // Longitudinal mean [layer]: (sum_i i*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerSigma = 0.; // Longitudinal sigma [layer]: TMath::Sqrt((sum_i (i-ShowerMean)^2*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i]))
   float ShowerSkewness = 0.; // Longitudinal skewness [layer^3]: (sum_i (i-ShowerMean)^3*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])
   float ShowerKurtosis = 0.; // Longitudinal kurtosis [layer^4]: (sum_i (i-ShowerMean)^4*LayerClusterEnergy[i])/(sum_i LayerClusterEnergy[i])

   float LayerEnergy = 0.; // sum_i LayerClusterEnergy[i]

   float LayerTruncSigma[nLAYERs]; // Sigma [cell] for each layer, between 3 sigma from the maximum

   for (Int_t ilayer = 0; ilayer < nLAYERs; ++ilayer)
   {
      LayerClusterEnergy[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell) MapEneDep[ilayer][icell] = 0.;
   }

   Int_t n2DCLUSTERs = NEcal2DCluster();
   for (Int_t i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster)
   {
      Ecal2DClusterR *cluster2d = pEcal2DCluster(i2dcluster);

      Int_t nCLUSTERs = cluster2d->NEcalCluster();
      for (Int_t icluster = 0; icluster < nCLUSTERs; ++icluster)
      {
         EcalClusterR *cluster = cluster2d->pEcalCluster(icluster);

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
   {
      LayerEneDep[ilayer] = 0.;
      for (Int_t icell = 0; icell < nCELLs; ++icell)
      {
         MapEneDep[ilayer][icell] /= 1000.;
   
         LayerEneDep[ilayer] += MapEneDep[ilayer][icell];
   
         if (ilayer == 16 || ilayer == 17) L2LFrac += MapEneDep[ilayer][icell];
         if (ilayer ==  0 || ilayer ==  1) F2LFrac += MapEneDep[ilayer][icell];
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
   unsigned int ivar = 0;
      
      
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
                        NESEreaderv2->AddVariable("N2ShowerLatDispx",&ESENShowerFootprint[1]);
                        NESEreaderv2->AddVariable("N2ShowerLatDispy",&ESENShowerFootprint[2]);
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
                        sprintf(name,"%s/%s/TMVA_ePrej_V3_12_BCat.weights.xml",getenv("AMSDataDir"),AMSCommonsI::getversion());
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
  for(int i=0; i<sizeof(TrigTime)/sizeof(TrigTime[0]); i++){
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
  for (int i=0; i<3; i++) Coo[i] = ptr->_coo[i];
  for (int i=0; i<3; i++) Dir[i] = ptr->_dir[i];
  Momentum = ptr->_mom;
  Mass     = ptr->_mass;
  Charge   = ptr->_charge;
  tbline   = ptr->_tbline;
#endif
}           

MCTrackR::MCTrackR(AMSmctrack *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  RadL = ptr->_radl;
  AbsL = ptr->_absl;
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
  fCharge = -1;
  fTrTrack  = -1;
  fTrdTrack    = -1;
  fTrdHTrack   = -1;
  fRichRing   = -1;
  fEcalShower = -1;
  fVertex = -1;
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

#endif
}

int ParticleR::Loc2Gl(AMSEventR *pev){
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
             cerr<<"AMSParticleR-E-UnableToLoadRPY "<<k<<endl;
             return 1;
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

bool ParticleR::IsInsideTRD()
{
  // 8 points of the acceptance octagon at z = zTrd
  const int npoints = 8;
  double acceptance_x[npoints] = {+40, +78, +78, +40, -40, -78, -78, -40};
  double acceptance_y[npoints] = {+76, +35, -35, -76, -76, -35, +35, +76};

  // check if extrapolated point is inside acceptance octagon
  double x = TRDCoo[0][0];
  double y = TRDCoo[0][1];
  return TMath::IsInside(x, y, npoints, acceptance_x, acceptance_y);
}

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
    double beta=pRichRing()->getBeta();
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

TofClusterHR::TofClusterHR(AMSTOFClusterH *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  Pattern= ptr->_pattern;
  Layer  = ptr->_idsoft/1000%10;
  Bar    = ptr->_idsoft/100%10;
  for(int is=0;is<2;is++){
    Aadc[is]= ptr->_adca[is];
    Rtime[is]=ptr->_sdtm[is];
  }
  Time= ptr->_timer;
  ETime=ptr->_etimer;
  for(int i=0;i<3;i++){
    Coo[i]= ptr->_coo[i];
    ECoo[i]=ptr->_ecoo[i];
  }
  AEdep=ptr->_edepa;
  DEdep=ptr->_edepd;
#endif
}


TofMCClusterR::TofMCClusterR(AMSTOFMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Edep= ptr->edep;
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
  for(int ip=0;ip<TOF2GC::PMTSMX && ip<sizeof(adcd)/sizeof(adcd[0]);ip++)adcd[ip]=ptr->_adcd[ip];
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
 unsigned int id=getid();
 const string name("TRDPedestals");
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 return value.f;
}

float TrdRawHitR::getsig(int & error){
 unsigned int id=getid();
 const string name("TRDSigmas");
 AMSEventR::if_t value;
 value.u=0;
 error=AMSEventR::Head()->GetTDVEl(name,getid(),value);
 return value.f;
}
float TrdRawHitR::getgain(int & error){
 unsigned int id=getid();
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
 for(int k=0;k<AMSEventR::Head()->getsetup()->fTDV_Name[0].Size;k++){
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
for(int k=0;k<sizeof(ChargePDF)/sizeof(ChargePDF[0]);k++)ChargePDF[k]=0;
int ptr=0;
const int span=1003;
while(file.good() && !file.eof()){

file>>ChargePDF[ptr*span+span-3];  //e2c
file>>ChargePDF[ptr*span+span-2];  //id
file>>ChargePDF[ptr*span+span-1];  //ok
for(int k=0;k<span-3;k++)file>>ChargePDF[ptr*span+k];
double smax=0;
for(int k=0;k<span-3;k++){
  float a=ChargePDF[ptr*span+k];
  if(a<=0){
    float sum=0;
    for(int j=k;j<span-3;j++){
      if(ChargePDF[ptr*span+j]<10)sum+=ChargePDF[ptr*span+j];
    }
    for(int j=k;j<span-3;j++){
        if(ChargePDF[ptr*span+j]<10)ChargePDF[ptr*span+j]=(sum==0?1:sum)/(span-3-k);
      }
    }
  }
 
for(int k=0;k<span-3;k++){
smax+=ChargePDF[ptr*span+k];
}
for(int k=0;k<span-3;k++)ChargePDF[ptr*span+k]/=smax;
if(ChargePDF[ptr*span+span-1])ptr++;


}

cout <<"Charge::ChargePDF-I- "<<ptr<<" pdf loaded for ";
for(int k=0;k<ptr;k++)cout<<ChargePDF[k*span+span-2]<<" ";
cout <<endl;
if(ptr<3){
 cerr<<"Charge::ChargePDF-E-minimal 3 PDF Needed "<<endl;
 return false;
}
for(int i=ptr;i<sizeof(ChargePDF)/sizeof(ChargePDF[0])/span;i++){
  ChargePDF[i*span+span-2]=i;
}
  int thr=0;
#ifdef _OPENMP
//  thr=omp_get_thread_num();
#endif
static int done=0;
for(int k=0;k<10;k++){
 if(!done) AMSEventR::hbook1(-50000-k-thr*100,"pdf func ",1000,0.,100.);
  for(int j=0;j<span-3;j++){
     AMSEventR::hf1(-50000-k-thr*100,j/10.+0.05,ChargePDF[span*k+j]);
}
}
done=1;
return true;
}



void TrdTrackR::ComputeCharge(double betacorr){
            for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=-1;
            for(int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=10000;
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
   if(edepc.size()%2)for(int k=edepc.size()/2-2;k<edepc.size()/2+3;k++)medianc+=edepc[k]/5;
   else for(int k=edepc.size()/2-2;k<edepc.size()/2+2;k++)medianc+=edepc[k]/4;
   Q=sqrt(medianc/betacorr)*1/1.0925+0.115/1.0925;

    
   //lkhd
   const int span =sizeof(ChargePDF)/sizeof(ChargePDF[0])/(sizeof(Charge)/sizeof(Charge[0]));
   for (int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
    if(ChargePDF[k*span+span-1]){
    Charge[k]=ChargePDF[k*span+span-2];  
    ChargeP[k]=0;
    for(int i=0;i<edepc.size();i++){
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
          for(int i=0;i<edepc.size();i++){
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
   for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     prsum+=exp(-double(ChargeP[k]));
   }
    
   for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     ChargeP[k]+=log(prsum);
   }
   multimap<float,short int> chmap;
   for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++){
     chmap.insert(make_pair(ChargeP[k],Charge[k]));
   }
   int l=0;
   for(multimap<float,short int>::iterator k=chmap.begin();k!=chmap.end();k++){
        Charge[l]=k->second;
        ChargeP[l++]=k->first;
   }


 }


        }


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
 for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=o.Charge[k];
 for(int k=k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=o.ChargeP[k];
 for(int i=0;i<o.fTrdSegment.size();i++)fTrdSegment.push_back(o.fTrdSegment[i]);  
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
  for(int k=0;k<sizeof(Charge)/sizeof(Charge[0]);k++)Charge[k]=ptr->_Charge.Charge[k];
  for(int k=0;k<sizeof(ChargeP)/sizeof(ChargeP[0]);k++)ChargeP[k]=ptr->_Charge.ChargeP[k];
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

  double mean_index=_sumIndex[tile]/_totalIndex[tile];

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
  if(AMSEventR::Head()->nMCEventg()){
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
    else NpExpCorr[pmt]=corr->EfficiencyCorrection(pmt) * corr->EfficiencyTemperatureCorrection(pmt);
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
  for(int i=0;i<AMSEventR::Head()->NRichHit();i++){
    RichHitR hit=AMSEventR::Head()->RichHit(i);
    if((hit.Status>>ring)%2){
      fRichHit.push_back(i);
    }
  }
  if(Used!=fRichHit.size())cerr<<" problem hits for ring "<<ring<<" "<<Used<<" "<<fRichHit.size()<<endl;
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
      for(int i=0;i<(ptr->_beta_direct).size();i++)
	if((ptr->_beta_direct)[i]>0) fBetaHit.push_back((ptr->_beta_direct)[i]);
      for(int i=0;i<(ptr->_beta_reflected).size();i++)
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
  for(int k=0;k<sizeof(JError)/sizeof(JError[0]);k++){
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
	AMSSetupR::SlowControlR *cr=&AMSEventR::getsetup()->fSlowControl;
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
	AMSSetupR::SlowControlR *cr=&AMSEventR::getsetup()->fSlowControl;
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

TofRawSideR* TofClusterHR::pTofRawSide(unsigned int i){
  return (AMSEventR::Head() && i<fTofRawSide.size())?AMSEventR::Head()->pTofRawSide(fTofRawSide[i]):0;
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
  
  for(int i=0;i<sizeof(p)/sizeof(p[0]);i++){
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
double rmin2=-b*b/a+(d2-dn*dn);
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

//-------TofBetaPar
void TofBetaPar::Init(){
  Status=SumHit=UseHit=0;
  Beta=BetaC=0;
  UseHit=0;
  for(int ilay=0;ilay<4;ilay++){
    Pattern[ilay]=0;
    Len[ilay]=0;
    Time[ilay]=0;
    ETime[ilay]=1000;
    TResidual[ilay]=1000;
    for(int is=0;is<2;is++){CResidual[ilay][is]=0;}
   }
   Beta=Beta=InvErrBeta=InvErrBetaC=Chi2T=T0=Chi2C=0;
   Mass=EMass=0;
};

const TofBetaPar& TofBetaPar::operator=(const TofBetaPar &right){
  Status=right.Status;
  SumHit=right.SumHit;
  UseHit=right.SumHit;
  for(int ilay=0;ilay<4;ilay++){
    Pattern[ilay]=right.Pattern[ilay];
    Len[ilay]=right.Len[ilay];
    Time[ilay]=right.Time[ilay];
    ETime[ilay]=right.ETime[ilay];
    TResidual[ilay]=right.TResidual[ilay];
    for(int is=0;is<2;is++){CResidual[ilay][is]=right.CResidual[ilay][is];}
  }
  T0=right.T0;
  Beta=right.Beta;
  BetaC=right.BetaC;
  InvErrBeta=right.InvErrBeta;
  InvErrBetaC=right.InvErrBeta;
  Chi2T=right.Chi2T;
  Chi2C=right.Chi2C;
//---
  Mass=right.Mass;
  EMass=right.EMass;
  return *this;
}

void TofBetaPar::SetFitPar(float _Beta,float _BetaC,float _InvErrBeta,float _InvErrBetaC,float _Chi2T,float _T0){
    Beta=_Beta;BetaC= _BetaC;
    InvErrBeta=_InvErrBeta;InvErrBetaC=_InvErrBetaC;
    Chi2T=_Chi2T;T0=_T0;
}

void TofBetaPar::CalFitRes(){
  for(int ilay=0;ilay<4;ilay++){
    if((Pattern[ilay]%10==4)){
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


TofClusterHR* BetaHR::pTofClusterH(unsigned int i){
  return (AMSEventR::Head() && i<fTofClusterH.size())?AMSEventR::Head()->pTofClusterH(fTofClusterH[i]):0;
}

//-------BetaH Function
TofClusterHR* BetaHR::GetClusterHL(int ilay){
  if(ilay<0||ilay>=4||fLayer[ilay]<0)return 0;
  return (AMSEventR::Head() && fLayer[ilay]<fTofClusterH.size())?AMSEventR::Head()->pTofClusterH(fLayer[ilay]):0;
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
  double time[4],etime[4],len[4];
  int nhit=0;
  int ndiv=1000;
  for(int ilay=0;ilay<4;ilay++){
      if((GetPattern(ilay))%10==4&&((pattern/ndiv)%10>0)){
        time [nhit]=GetTime(ilay);
        etime[nhit]=GetETime(ilay);
        len  [nhit]=GetTkTFLen(ilay);
        nhit++;
      }
     ndiv=ndiv/10;
   }
  TofRecH::betapar=BetaPar;
  TofRecH::BetaFitT(time,etime,len,nhit,TofRecH::betapar,mode);
  beta=TofRecH::betapar.Beta;
  ebetav=TofRecH::betapar.InvErrBeta;
  if(update){BetaPar=TofRecH::betapar;BetaPar.UseHit=nhit;}
  return 0;
}

#ifdef _PGTRACK_
double BetaHR::TInterpolate(double zpl,AMSPoint &pnt,AMSDir &dir,double &time){
  double path=0;
  if(fTrTrack>=0){
     path=pTrTrack()->Interpolate(zpl,pnt,dir);
     time =path/GetBeta()/TofRecH::cvel+GetT0();
   }
  else {time=0;}
  return path;
}
#endif
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

void AMSEventR::CreateBranch(TTree *tree, int branchSplit){
  if(tree){
    _Head=this;
    tree->Branch(BranchName(),"AMSEventR",&_Head,128000,branchSplit);
    TBranch * branch=tree->GetBranch(BranchName());
    branch->SetCompressionLevel(6);
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
#pragma omp critical (cls)
  {
    if(--_NFiles==0 && pService){
      // Function called at the end of the event loop.
      //_Tree->SetMakeClass(0);
      UTerminate();
      (*pService)._w.Stop();
      if(fgThickMemory)hjoin();
      for(int k=0;k<sizeof(_ClonedTree)/sizeof(_ClonedTree[0]);k++)_ClonedTree[k]=0;
      for(int k=0;k<sizeof(_ClonedTreeSetup)/sizeof(_ClonedTreeSetup[0]);k++)_ClonedTreeSetup[k]=0;
      cout <<"AMSEventR::Terminate-I-CputimeSpent "<<(*pService)._w.CpuTime()<<" sec"<<endl;
      cout <<"AMSEventR::Terminate-I-Total/Bad "<<(*pService).TotalEv<<"/"<<(*pService).BadEv<<" events processed "<<endl;
      cout <<"AMSEventR::Terminate-I-ApproxTotal of "<<(*pService).TotalTrig<<" triggers processed "<<endl;
      if((*pService)._pOut){
	(*pService)._pDir->cd(); 
	(*pService)._pOut->Write();
	cout <<"AMSEventR::Terminate-I-WritedFile "<<GetOption()<<endl;
      for(int k=0;k<sizeof(fgOutSep)/sizeof(fgOutSep[0]);k++){
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
      char objlist[7][40]={"TkDBc","TrCalDB","TrParDB","TrGainDB","TrReconPar","TrExtAlignDB","TrInnerDzDB"};
      TObject* obj[7]={0,0,0,0,0,0,0}; 
      TObjString* obj2=0;
      TObjString* obj3=0;
      if(!input){cerr<<"AMSEventR::Fill-E-cannot find input file "<<endl;}
      else{
	cout <<input->GetName()<<endl;
	for(int ii=6;ii>=0;ii--){
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
      for(int i=0;i<7;i++)if(obj[i]){cout<<" write "<<objlist[i]<<endl;obj[i]->Write();};
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
fEcalShower(o.fEcalShower),fRichHit(o.fRichHit),fRichRing(o.fRichRing),fRichRingB(o.fRichRingB),
fTofRawCluster(o.fTofRawCluster),fTofRawSide(o.fTofRawSide),fTofCluster(o.fTofCluster),fTofClusterH(o.fTofClusterH),
fAntiRawSide(o.fAntiRawSide),fAntiCluster(o.fAntiCluster),fTrRawCluster(o.fTrRawCluster),
fTrCluster(o.fTrCluster),fTrRecHit(o.fTrRecHit),fTrTrack(o.fTrTrack),fTrdRawHit(o.fTrdRawHit),
fTrdCluster(o.fTrdCluster),fTrdSegment(o.fTrdSegment),fTrdTrack(o.fTrdTrack),fTrdHSegment(o.fTrdHSegment),
fTrdHTrack(o.fTrdHTrack),fLevel1(o.fLevel1),fLevel3(o.fLevel3),fBeta(o.fBeta),fBetaB(o.fBetaB),fBetaH(o.fBetaH),fCharge(o.fCharge),
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
  size+=sizeof(ChargeR)*fCharge.size();
  size+=sizeof(VertexR)*fVertex.size();
  size+=sizeof(ParticleR)*fParticle.size();
  size+=sizeof(DaqEventR)*fDaqEvent.size();
  return size;
}

//----------------------------------------------------------------------
int AMSEventR::isInShadow(AMSPoint&  ic,int ipart){
// Says if particle ipart is in shadow and returns also  AMSPoints of interception of particles 
// with iss solar panel rotation plane (FLT_MAX if directed away);

        double deg2rad =3.141592654/180.;


        ic=0;
        //cout<<"\n----------------------New Event --------------------"<<endl;

        if(ipart>=nParticle()){
                cerr<<"AMSEventR::isInShadow-E-NiSuchParticle "<<ipart<<endl;
                return -1;
        }


        ParticleR part=Particle(ipart);
                if(part.Beta<0)return -2;

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
        double addon=13.6e-3/fabs(part.Momentum)*part.Charge*sqrt(0.35)*5/sqrt(2.);

        //Direction of incident particle in AMS coo
        AMSDir dir(part.Theta,part.Phi);

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
        alpha=fHeader.Alpha;
        beta1=fHeader.B1a;
        beta3=fHeader.B3a;
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
        int sr[2];

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

char * HeaderR::Info(unsigned long long status){
	if(Pitch==0 && Yaw==0 && Roll==0){
		int ret=getISSAtt();
	}
	double  cb = cos(Pitch);
	double  sb = sin(Pitch);
	double  cg = cos(Roll);
	double  sg = sin(Roll);

	const float angle=12./180*3.1415926;
	double crp=cos(angle);
	double srp=sin(angle);     
	double cams=(sg*cb*srp+cg*cb*crp);
	cams=acos(cams)*180/3.1415926; 
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
	int ret=getISSSA(alpha,b1a,b3a,b1b,b3b);
	float r,phi,theta,v,vphi,vtheta;
	int ret2=getISSCTRS(r,theta,phi,v,vtheta,vphi);

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
		}}
	if(pev && pev->Version()<566){
		int k=Loc2Gl(pev);
	}
	if(fabs(anti)>fabs(AntiCoo[1][2]))anti=AntiCoo[1][2];
	float lt=pev?pev->LiveTime():1;
	sprintf(_Info," Particle %s No %d Id=%d p=%7.3g#pm%6.2g M=%7.3g#pm%6.2g #theta=%4.2f #phi=%4.2f Q=%2.0f  #beta=%6.3f#pm%6.3f/%6.2f  Coo=(%5.2f,%5.2f,%5.2f) LT %4.2f #theta_G %4.2f #phi_G %4.2f",pType(),number,Particle,Momentum,ErrMomentum,Mass,ErrMass,Theta,Phi,Charge,Beta,ErrBeta,btof,Coo[0],Coo[1],Coo[2],lt,ThetaGl,PhiGl);
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
    sprintf(_Info,"Length  %d TDR %d UDR %d SDR %d RDR %d EDR %d LVL1 %d ; LVL3  %d %d %d %d %d %d %d %d %d %d %d L3counter %d L3Td %f7.1 msec",Length,Tdr,Udr,Sdr,Rdr,Edr,L1dr,a,p,e,len,status,sa4,sa3,sa2,sa1,sc, (L3dr>>16)&32767,L3Event(),double(L3TimeD)*0.64e-3);
  return _Info;
  } 



#ifdef _PGTRACK_
#include "TrRecon.h"
#include "TrCalDB.h"
#include "TrParDB.h"
#include "TrPdfDB.h"
#include "TrGainDB.h"
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
    TrCalDB::Head = (TrCalDB*)_FILE->Get("TrCalDB");
    if(!TkDBc::Head){
      if (!TkDBc::Load(_FILE)) { // by default get TkDBc from _FILE
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
    }
    if(!TrExtAlignDB::ForceFromTDV) 
      TrExtAlignDB::Load(_FILE);
    TrInnerDzDB::Load(_FILE);
try{
                                 if (_FILE->Get("datacards/TKGEOMFFKEY_DEF"))
    TKGEOMFFKEY =*((TKGEOMFFKEY_DEF*)_FILE->Get("datacards/TKGEOMFFKEY_DEF"));
				 if (_FILE->Get("datacards/TRMCFFKEY_DEF"))
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

    // TrGainDB load
    // 1st attempt: file
    if (!TrGainDB::GetHead()->Load(_FILE)) { 
      // 2 attempt: TDV
      TrGainDB::GetHead()->LoadFromTDV(Run()+30); 
      // if TDV load fails the default is used
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
map <unsigned int,int>::iterator it= _RunSetup.theMap.find(run);
     bool ret=it!=_RunSetup.theMap.end();
     if(ret)_TreeSetup->GetEntry(it->second);

return ret;
}
bool AMSEventR::InitSetup(TFile *_FILE, char *name,uinteger run){
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
          _TreeSetup->GetEntry(it->second);
          _EntrySetup=it->second;
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

int AMSEventR::GetSlowControlData(char *en, vector<float>&v, int method){
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
 
for(int k=0;k<BadRunList.size();k++){
if(run==BadRunList[k])return true;
}
return false;
}

bool AMSEventR::RunTypeSelected(unsigned int runtype){
if(!RunType.size())return true; 
for(int k=0;k<RunType.size();k++){
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
 for(int k=0;k<gps.Epoche.size();k++)fHeader.GPSTime.push_back(gps.Epoche[k]);
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
unsigned int gpsdiff=15;
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSSA a;
double xtime=Time[0]+Time[1]/1000000.-gpsdiff+dt;
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
unsigned int gpsdiff=15;
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSCTRSR a;
double xtime=Time[0]+Time[1]/1000000.-gpsdiff+dt;
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
unsigned int gpsdiff=15;
if(!AMSEventR::getsetup())return 2;
AMSSetupR::ISSGTOD a;
double xtime=Time[0]+Time[1]/1000000.-gpsdiff+dt;
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
unsigned int gpsdiff=15;
AMSSetupR::GPSWGS84R a;
unsigned int sec,nsec;
int ok=AMSEventR::Head()->GetGPSTime(sec,nsec);
double xtime=sec+double(nsec)/1e9+dt;
if(!ok){
  static int nprint=0;
  if(nprint++<100)cerr<<"HeaderR::getGPSWGS84-GetGPSTimeRetuens "<<ok<<endl;
  xtime=Time[0]+Time[1]/1000000.-gpsdiff+dt;
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
unsigned int gpsdiff=15;
if(!AMSEventR::getsetup())return 2;
double xtime=Time[0]+Time[1]/1000000.-gpsdiff;
return AMSEventR::getsetup()->getISSAtt(roll,pitch,yaw,xtime);

}
int HeaderR::getISSAtt(){
unsigned int gpsdiff=15;
if(!AMSEventR::getsetup())return 2;
double xtime=Time[0]+Time[1]/1000000.-gpsdiff;

return AMSEventR::getsetup()->getISSAtt(Roll,Pitch,Yaw,xtime);

}

   ///<get solar beta angle via geometrical calculation
double HeaderR::getBetaSun(){
 SunPosition sunPos;
 unsigned int time,nanoTime;
 double Beta;
 if (HeaderR::GetGPSEpoche( time, nanoTime)!=0 ){
   time=HeaderR::Time[0];
   sunPos.setUTCTime((double)time);
   }
 else{
 sunPos.setGPSTime((double)time);
 }
 sunPos.setISSGTOD( HeaderR::RadS, HeaderR::ThetaS, HeaderR::PhiS, HeaderR::VelTheta, HeaderR::VelPhi, HeaderR::Yaw,HeaderR::Pitch,HeaderR::Roll);
Beta=sunPos.GetBetaAngle();
 return Beta;
}
   ///<get sun position in AMS coordinate
int HeaderR::getSunAMS(double & azimut, double & elevation ){
 SunPosition sunPos;
 unsigned int time,nanoTime;
 int res;
 if (HeaderR::GetGPSEpoche( time, nanoTime)!=0 ){
   time=HeaderR::Time[0];
   sunPos.setUTCTime((double)time);
   }
 else{
 sunPos.setGPSTime((double)time);
 }
sunPos.setISSGTOD( HeaderR::RadS, HeaderR::ThetaS, HeaderR::PhiS, HeaderR::VelTheta, HeaderR::VelPhi, HeaderR::Yaw,HeaderR::Pitch,HeaderR::Roll); 
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


float AMSEventR::LiveTime(){

if(getsetup()){
 int k=getsetup()->getScalers(fHeader.Time[0],fHeader.Time[1]);
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


void  Level1R::RestorePhysBPat() {

 if(PhysBPatt==0){

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
    int b15=(JMembPatt>>15)&1;
    int b14=(JMembPatt>>14)&1;

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

#ifdef _PGTRACK_
int  UpdateInnerDz(){

  uint time,run;
#ifdef __ROOTSHAREDLIBRARY__ 
  time=AMSEventR::Head()->UTime();
  run=AMSEventR::Head()->Run();
#else
  time=AMSEvent::gethead()->gettime();
  run=AMSEvent::gethead()->getrun();
#endif

  // PZ Update also the Inner DzDB
  return TrInnerDzDB::GetHead()->UpdateTkDBc(time);
}
int  UpdateExtLayer(int type=0,int lad1=-1,int lad9=-1){
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
  else{
    //    printf("updating withh l1 %d ln%d \n",lad1,lad9);
    ret=TrExtAlignDB::GetHead()->UpdateTkDBcDyn(run,time,3,lad1,lad9);
  }
  return ret;
} 

#endif
