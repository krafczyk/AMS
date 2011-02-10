//  $Id: root.C,v 1.247 2011/02/10 11:01:01 mdelgado Exp $

#include "TRegexp.h"
#include "root.h"
#include "ntuple.h"
#include "TSystem.h"
#include "TXNetFile.h"
#include <TChainElement.h>
#include "TFile.h"
#ifdef _OPENMP
#include <omp.h>
#endif
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
  ClassImp(RichRingBR)
  ClassImp(TofRawClusterR)
  ClassImp(TofRawSideR)
  ClassImp(TofClusterR)
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
  ClassImp(ChargeR)
  ClassImp(ParticleR)
  ClassImp(AntiMCClusterR)
  ClassImp(TrMCClusterR)
  ClassImp(TofMCClusterR)
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
int AMSEventR::fgThreads=1;
TString AMSEventR::Dir="";
TString AMSEventR::gWDir="";
void AMSEventR::hcopy(char dir[],int id1,int id2){
  char save[1024];
  strcpy(save,Dir.Data());
  hcdir(dir);
  TH1F *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hcdir(save);
    hbook1(id2,title,n,a,b);
    TH1F *h2p = h1(id2);
    if(h2p){
      for (int i=0;i<n+2;i++){
	h2p->SetBinContent(i,h1p->GetBinContent(i));
	h2p->SetBinError(i,h1p->GetBinError(i));
      
      }
    }
  }
  else{
    TH2F *h2p = h2(id1);
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
  TH1F *h1p = h1(id1);
  if(h1p){
    float a=h1p->GetBinLowEdge(1); 
    int n=h1p->GetNbinsX();
    float b=h1p->GetBinLowEdge(n)+h1p->GetBinWidth(n); 
    const char *title=h1p->GetTitle();
    hbook1(id2,title,n,a,b);
    TH1F *h2p = h1(id2);
    if(h2p){
      for (int i=0;i<n+2;i++){
	h2p->SetBinContent(i,h1p->GetBinContent(i));
	h2p->SetBinError(i,h1p->GetBinError(i));
      
      }
    }
  }
  else{
    TH2F *h2p = h2(id1);
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
  TH1F *h2p = h1(id2);
  if(h2p){
    //   h2p->Sumw2();
    hcopy(id1,id3);
    TH1F *h1p = h1(id3);
    if(h1p){
      //    h1p->Sumw2();
      h1p->Divide(h2p);
    }   
  }
}
void AMSEventR::hscale(int id1,double fac){
  TH1F *h2p = h1(id1);
  if(h2p){
    h2p->Sumw2();
    h2p->Scale(fac);
  }
}
void AMSEventR::hsub(int id1,int id2,int id3){
  TH1F *h2p = h1(id2);
  if(h2p){
    h2p->Sumw2();
    hcopy(id1,id3);
    TH1F *h1p = h1(id3);
    if(h1p){
      h1p->Sumw2();
      h1p->Add(h2p,-1);
    }   
  }
}
void AMSEventR::hadd(int id1,int id2,int id3){
  TH1F *h2p = h1(id1);
  if(h2p){
    h2p->Sumw2();
    if(id2!=id3)hcopy(id2,id3);
    TH1F *h1p = h1(id3);
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
  TH1F * p= new TH1F(hid,title,ncha,a,b);
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
  if(Service::hb2.find(id) != Service::hb2.end()){
    delete Service::hb2.find(id)->second;
    Service::hb2.erase((Service::hb2.find(id)));
    cerr<<"  AMSEventR::hbook2-S-Histogram "<<id<<" AlreadyExistsReplacing "<<endl;
  }
  char hid[1025];
  sprintf(hid,"hb2_%d_%s_%s",idd,title,Dir.Data()); 
  TH2F * p= new TH2F(hid,title,ncha,a,b,nchaa,aa,ba);
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
    for( Service::hb1i i=Service::hb1.begin();i!=Service::hb1.end();i++){
      delete i->second;
    }
    Service::hb1.clear();
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
    TH1F * f1 = dynamic_cast<TH1F*>(to);
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
	    else cerr<<"TH1F "<<t<<" IdNotDigitalSkipped"<<endl;
	  }
        }
      }
      else if(t!=TString(""))cerr<<"TH1F "<<t<<" NotCreatedByHBOOK1Skipped"<<endl;
    }
    else{
      TH2F * f1 = dynamic_cast<TH2F*>(to);
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
	      else cerr<<"TH2F "<<t<<" IdNotDigitalSkipped"<<endl;
	    }
	  }
	}
	else if(t!=TString(""))cerr<<"TH2F "<<t<<" NotCreatedByHBOOK2Skipped"<<endl;
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
TH1F * AMSEventR::h1(int idd){
  //
  // must be used with care as may be changed after map update operation
  //
  AMSID id(idd,Dir);
  Service::hb1i i1=Service::hb1.find(id);
  if(i1 != Service::hb1.end())return i1->second;
  else return 0; 
}

TH2F * AMSEventR::h2(int idd){
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

#pragma omp critical (hf1)
      i1->second->Fill(a,w);
      //*/
    }
  }
  else{
#pragma omp critical (cout)
{ 
cout << "id not found "<<id<<endl;
}
}
}

void AMSEventR::hfp(int idd, float a, float w=1){
  AMSID id(idd,Dir);
  Service::hbpi i1=Service::hbp.find(id);
  if(i1 != Service::hbp.end()){
    if(fgThickMemory)i1->second->Fill(a,w);
    else{
#pragma omp critical (hfp)
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
    TH1F *h1p = i->second;
    if(h1p){
      h1p->Sumw2();
      for(Service::hb1i j=i;j!=Service::hb1.end();j++){
	if(i!=j && id==j->first.getid()){
	  TH1F* h2p=  j->second;
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
    TH2F *h1p = i->second;
    if(h1p){
      //   h1p->Sumw2();
      for(Service::hb2i j=i;j!=Service::hb2.end();j++){
	if(i!=j && id==j->first.getid()){
	  TH2F* h2p=  j->second;
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
TBranch* AMSEventR::bVertex;
TBranch* AMSEventR::bCharge;
TBranch* AMSEventR::bParticle;
TBranch* AMSEventR::bAntiMCCluster;
TBranch* AMSEventR::bTrMCCluster;
TBranch* AMSEventR::bTofMCCluster;
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
void* AMSEventR::vVertex=0;
void* AMSEventR::vCharge=0;
void* AMSEventR::vParticle=0;
void* AMSEventR::vAntiMCCluster=0;
void* AMSEventR::vTrMCCluster=0;
void* AMSEventR::vTofMCCluster=0;
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
char  ParticleR::_Info[255];
#ifndef _PGTRACK_
//PZ FIXME
char TrRecHitR::_Info[255];
char TrTrackR::_Info[255];
char TrMCClusterR::_Info[255];
#endif
//char TrdHTrackR::_Info[255];
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
TTree*     AMSEventR::_ClonedTree=0;
TTree*     AMSEventR::_ClonedTreeSetup=0;
AMSEventR* AMSEventR::_Head=0;
AMSSetupR* AMSEventR::_HeadSetup=0;
AMSEventR::Service*    AMSEventR::pService=0;
int AMSEventR::_Count=0;
int AMSEventR::_NFiles=-1;
int AMSEventR::_Entry=-1;
int AMSEventR::_EntrySetup=-1;
char* AMSEventR::_Name="ev.";   



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
  fHeader.Vertexs=fVertex.size();
  fHeader.Charges=fCharge.size();
  fHeader.Particles=fParticle.size();
  fHeader.AntiMCClusters=fAntiMCCluster.size();
  fHeader.TrMCClusters=fTrMCCluster.size();
  fHeader.TofMCClusters=fTofMCCluster.size();
  fHeader.EcalMCHits=fEcalMCHit.size();
  fHeader.TrdMCClusters=fTrdMCCluster.size();
  fHeader.RichMCClusters=fRichMCCluster.size();
  fHeader.MCTracks=fMCTrack.size();
  fHeader.MCEventgs=fMCEventg.size();
  fHeader.DaqEvents=fDaqEvent.size();
}

bool AMSEventR::ReadHeader(int entry){
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
  clear();
  static TFile* local_pfile=0;
#pragma omp threadprivate (local_pfile)
  if(i>0){
    if( local_pfile!=_Tree->GetCurrentFile()){
      local_pfile=_Tree->GetCurrentFile();
      InitDB(local_pfile);
// workaround root static bug
      if(!gDirectory ||  !dynamic_cast<TDirectoryFile*>(gDirectory)){
          cout <<" Open "<<_Tree->GetCurrentFile()->GetName()<<endl;
          TFile::Open(_Tree->GetCurrentFile()->GetName());
          cout <<"AMSEventR::ReadHeader-I-SettinggDirectory "<<endl;
      }
      if(!InitSetup(local_pfile,"AMSRootSetup",UTime())){
      cout <<"AMSEventR::ReadHeader-I-Version/OS "<<Version()<<"/"<<OS()<<" "<<_Tree->GetCurrentFile()->GetName()<<endl;
      }
      else{
      cout <<"AMSSetupR::ReadHeader-I-Version/OS/BuildTime "<<getsetup()->fHeader.BuildNo<<"/"<<getsetup()->fHeader.OS<<" "<<getsetup()->BuildTime()<<" "<<_Tree->GetCurrentFile()->GetName()<<endl;
      cout <<"AMSSetupR::ReadHeader-I-"<<getsetup()->fScalers.size()<<" ScalersEntriesFound "<<endl;
   for(AMSSetupR::Scalers_i i=getsetup()->fScalers.begin();i!=getsetup()->fScalers.end();i++){
       //cout<< "FT " <<i->first<<" "<<i->second.FTtrig(0)<<endl; 
   }
      }     
     }
    if(Version()<160){
      // Fix rich rings
      NRichHit();
      for(int i=0;i<NRichRing();i++){
	RichRingR *ring=pRichRing(i);
	ring->FillRichHits(i);
      }
    }
    if(fHeader.Run!=runo){
      cout <<"AMSEventR::ReadHeader-I-NewRun "<<fHeader.Run<<endl;
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
  fCharge.reserve(MAXCHARGE02);
  fVertex.reserve(2);
  fParticle.reserve(MAXPART02);

  fAntiMCCluster.reserve(MAXANTIMC);
  fTofMCCluster.reserve(MAXTOFMC);
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
  fCharge.clear();
  fVertex.clear();
  fParticle.clear();

  fAntiMCCluster.clear();
  fTofMCCluster.clear();
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
    if(fTrRecHit.size()>root::MAXTRRH02*2 && !ptr->checkstatus(AMSDBc::USED))return;
#ifdef _PGTRACK_
    fTrRecHit.push_back(TrRecHitR(*ptr));
#else
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
  if (ptr) {
    fTrdTrack.push_back(TrdTrackR(ptr));
    ptr->SetClonePointer(fTrdTrack.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSTRDTrack ptr is NULL"<<endl;
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

void AMSEventR::AddAMSObject(AMSCharge *ptr, float probtof[],int chintof[], 
			     float probtr[], int chintr[], float probrc[], 
			     int chinrc[], float proballtr)
{
  if (ptr) {
    fCharge.push_back(ChargeR(ptr, probtof, chintof, probtr, chintr, probrc, chinrc, proballtr));
    ptr->SetClonePointer(fCharge.size()-1);
  }  else {
    cout<<"AddAMSObject -E- AMSCharge ptr is NULL"<<endl;
  }
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
  RadS=      ptr->RadS;
  ThetaS=    ptr->ThetaS;
  PhiS=      ptr->PhiS;
  ISSEqAsc=        ptr->ISSEqAsc;
  ISSEqDec=       ptr->ISSEqDec;
  ISSGalLat=      ptr->ISSGalLat;
  ISSGalLong=     ptr->ISSGalLong;
  AMSEqAsc=     ptr->AMSEqAsc;
  AMSEqDec=    ptr->AMSEqDec;
  AMSGalLat=   ptr->AMSGalLat;
  AMSGalLong=  ptr->AMSGalLong;
  Yaw=       ptr->Yaw;
  Pitch=     ptr->Pitch;
  Roll=      ptr->Roll;
  VelocityS= ptr->VelocityS;
  VelTheta=  ptr->VelTheta;
  VelPhi=    ptr->VelPhi;
  ThetaM=    ptr->ThetaM;
  BAv=ptr->BAv;
  TempTracker=ptr->TempTracker;
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


ChargeR::ChargeR(AMSCharge *ptr, float probtof[],int chintof[], 
		 float probtr[], int chintr[], float probrc[],
		 int chinrc[], float proballtr){
#ifndef __ROOTSHAREDLIBRARY__
  Status = ptr->_status;
  ChargeTOF     = ptr->_ChargeTOF;
  ChargeTracker = ptr->_ChargeTracker;
  ChargeRich    = ptr->_ChargeRich;
  for (int i=0; i<4; i++) {
    ProbTOF[i] = probtof[i];
    ChInTOF[i] = chintof[i];
    ProbTracker[i] = probtr[i];
    ChInTracker[i] = chintr[i];
    ProbRich[i] = probrc[i];
    ChInRich[i] = chinrc[i];
  }
  ProbAllTracker= proballtr;
  TrunTOF       = ptr->_TrMeanTOF;
  TrunTOFD      = ptr->_TrMeanTOFD;
  TrunTracker   = ptr->_TrMeanTracker;
  fBeta=-1;
  fRichRing=-1;
#endif
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
  Chi2Dir   = ptr->_AngleTrue3DChi2;
  FirstLayerEdep = ptr->_FrontEnergyDep;
  EnergyC   =   ptr->_EnergyC;
  Energy3C[0] = ptr->_Energy3C;
  Energy3C[1] = ptr->_Energy5C;
  Energy3C[2] = ptr->_Energy9C;
  ErEnergyC   = ptr->_ErrEnergyC;
  DifoSum     = ptr->_DifoSum;
  SideLeak    = ptr->_SideLeak;
  RearLeak    = ptr->_RearLeak;
  S13Leak    = ptr->_S13Leak;
  NLinLeak=ptr->_NLinLeak;
  S13R    = ptr->_S13R;
  DeadLeak    = ptr->_DeadLeak;
  AttLeak     = ptr->_AttLeak;
  OrpLeak     = ptr->_OrpLeak;
  Orp2DEnergy = ptr->_Orp2DEnergy;
  Chi2Profile = ptr->_ProfilePar[4+ptr->_Direction*5];
  for (int i=0; i<4; i++) ParProfile[i] = fabs(ptr->_ProfilePar[i+ptr->_Direction*5])>FLT_MAX?FLT_MAX:ptr->_ProfilePar[i+ptr->_Direction*5];
  Chi2Trans = ptr->_TransFitChi2;
  for (int i=0; i<3; i++) SphericityEV[i] = ptr->_SphericityEV[i];
  Nhits = ptr->_Nhits;
#endif
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

  TRDLikelihood = ptr->_TRDLikelihood;
  TRDHLikelihood = ptr->_TRDHLikelihood;
  TRDHElik = ptr->_TRDHElik;
  TRDHPlik = ptr->_TRDHPlik;
  TRDCCnhit = ptr->_TRDCCnhit;

#endif
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

TofMCClusterR::TofMCClusterR(AMSTOFMCCluster *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Idsoft = ptr->idsoft;
  for (int i=0; i<3; i++) {Coo[i] = ptr->xcoo[i];}
  TOF = ptr->tof;
  Edep= ptr->edep;
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

TrdSegmentR::TrdSegmentR(AMSTRDSegment *ptr){
#ifndef __ROOTSHAREDLIBRARY__
  Status        = ptr->_status;
  Orientation   = ptr->_Orientation;
  for (int i=0; i<2; i++) {FitPar[i] = ptr->_FitPar[i];}
  Chi2          = ptr->_Chi2;
  Pattern       = ptr->_Pattern;
#endif
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

RichRingR::RichRingR(AMSRichRing *ptr, int nhits) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    Used  = ptr->_used;
    UsedM = ptr->_mused;
    Beta  = ptr->_beta;
    ErrorBeta = ptr->_errorbeta;
    Chi2   = ptr->_quality;
    BetaRefit = ptr->_wbeta;
    Status    = ptr->_status;
    NpCol= ptr->_collected_npe;
    NpExp     = ptr->_npexp;
    Prob    = ptr->_probkl;
    KDist   = ptr->_kdist;  
    PhiSpread = ptr->_phi_spread;
    UDist = ptr->_unused_dist;
    Theta   =ptr->_theta;
    ErrorTheta  =ptr->_errortheta;
    for(int i=0;i<3;i++){
      TrRadPos[i]  = ptr->_radpos[i];
      TrPMTPos[i]  = ptr->_pmtpos[i];
    }
    for(int i=0;i<5;i++)
      AMSTrPars[i] = ptr->_crossingtrack[i];

    lipHitsUsed           = 0;
    lipThetaC             = 0;
    lipBeta               = 0;
    lipErrorBeta          = 0;
    lipLikelihoodProb     = 0;
    lipChi2               = 0;
    lipRecProb            = 0;

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
	    cout<<"FOUND "<<(ptr->_hit_pointer)[j]<<" HIT NUMBER "<<i<<endl;
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



//void RichRingBR::FillRichHits(int ringnew){
//  fRichHit.clear();
//  for(int i=0;i<HitsStatus.size();i++) {
//    if(HitsStatus[i]>=0) {
//      cout << "Adding hit " << i << " to RichRingBR" << endl;
//      fRichHit.push_back(i);
//    }
//  }
//}

RichRingBR::RichRingBR(AMSRichRingNew *ptr) {
#ifndef __ROOTSHAREDLIBRARY__
  fTrTrack = -1;
  if (ptr) {
    Status = ptr->_Status;
    Beta = ptr->_Beta;
    AngleRec = ptr->_AngleRec;
    Chi2 = ptr->_Chi2;
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
    NMirSec = ptr->_NMirSec;
    for(int i=0;i<3;i++){
      RingAccMsec1R[i] = (ptr->_RingAccMsec1R)[i];
      RingAccMsec2R[i] = (ptr->_RingAccMsec2R)[i];
      RingEffMsec1R[i] = (ptr->_RingEffMsec1R)[i];
      RingEffMsec2R[i] = (ptr->_RingEffMsec2R)[i];
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
  Length=ptr->getlength();
  Tdr=ptr->getsublength(0);
  Udr=ptr->getsublength(1);
  Sdr=ptr->getsublength(2);
  Rdr=ptr->getsublength(3);
  Edr=ptr->getsublength(4);
  L1dr=ptr->getsublength(5);
  L3dr=ptr->getsublength(6);
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

BetaR* ChargeR::pBeta(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
}
RichRingR* ChargeR::pRichRing(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pRichRing(fRichRing):0;
}


RichRingR* ParticleR::pRichRing(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pRichRing(fRichRing):0;
}

BetaR* ParticleR::pBeta(){
  return (AMSEventR::Head() )?AMSEventR::Head()->pBeta(fBeta):0;
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
      _ClonedTree=0; 
      (*pService)._w.Stop();
      if(fgThickMemory)hjoin();
      cout <<"AMSEventR::Terminate-I-CputimeSpent "<<(*pService)._w.CpuTime()<<" sec"<<endl;
      cout <<"AMSEventR::Terminate-I-Total/Bad "<<(*pService).TotalEv<<"/"<<(*pService).BadEv<<" events processed "<<endl;
      cout <<"AMSEventR::Terminate-I-ApproxTotal of "<<(*pService).TotalTrig<<" triggers processed "<<endl;
      if((*pService)._pOut){
	(*pService)._pDir->cd(); 
	(*pService)._pOut->Write();
	cout <<"AMSEventR::Terminate-I-WritedFile "<<GetOption()<<endl;
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
//    cout <<"  cloned treee "<<_ClonedTree<<endl;
    if (_ClonedTree==NULL) {
  TFile * input=_Tree->GetCurrentFile();
  if(!input){cerr<<"AMSEventR::Fill-E-annot find input file "<<endl;}
  char objlist[4][40]={"TkDBc","TrCalDB","TrParDB","TrReconPar"};
  TObject* obj[4]={0,0,0,0}; 
  TObjString* obj2=0;
  TObjString* obj3=0;
  if(!input){cerr<<"AMSEventR::Fill-E-annot find input file "<<endl;}
  else{
       cout <<input->GetName()<<endl;
   for(int ii=3;ii>=0;ii--){
    obj[ii]=input->Get(objlist[ii]);
     input->Get("TrParDB");
   }
   obj2=(TObjString*)input->Get("AMS02Geometry");
   obj3=(TObjString*)input->Get("DataCards");
  }
      _ClonedTree = _Tree->GetTree()->CloneTree(0);
//    cout <<"  cloned treeess "<<_ClonedTree<<endl;
      AMSEventR::_ClonedTree->SetDirectory(AMSEventR::OFD());
      TDirectory *gdir=gDirectory;
      gDirectory=AMSEventR::OFD();

      cout <<" obj2 "<<obj2<<" "<<(void*)obj3<<" "<<AMSEventR::OFD()->GetFile()->GetName()<<" "<<gDirectory->GetFile()->GetName()<<endl;
      if(obj2)obj2->Write("AMS02Geometry");
           if(obj3)obj3->Write("DataCards");
         for(int i=0;i<4;i++)if(obj[i]){cout<<" write "<<objlist[i]<<endl;obj[i]->Write();};
        gDirectory=gdir;
//    cout <<"  hopa "<<_ClonedTree<<" "<<_ClonedTree->GetCurrentFile()<<endl;
//    cout <<"2nd "<< _ClonedTree->GetCurrentFile()->GetName()<<endl;
}
//    cout <<"  cloned treeess "<<_ClonedTree<<endl;

    {
      _ClonedTree->SetBranchAddress(_Name,&Head());
      i= _ClonedTree->Fill();
//      cout <<"  i "<<i<<endl;
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
  bVertex->GetEntry(_Entry);
  bCharge->GetEntry(_Entry);
  bParticle->GetEntry(_Entry);
  bAntiMCCluster->GetEntry(_Entry);
  bTrMCCluster->GetEntry(_Entry);
  bTofMCCluster->GetEntry(_Entry);
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
fTofRawCluster(o.fTofRawCluster),fTofRawSide(o.fTofRawSide),fTofCluster(o.fTofCluster),
fAntiRawSide(o.fAntiRawSide),fAntiCluster(o.fAntiCluster),fTrRawCluster(o.fTrRawCluster),
fTrCluster(o.fTrCluster),fTrRecHit(o.fTrRecHit),fTrTrack(o.fTrTrack),fTrdRawHit(o.fTrdRawHit),
fTrdCluster(o.fTrdCluster),fTrdSegment(o.fTrdSegment),fTrdTrack(o.fTrdTrack),fTrdHSegment(o.fTrdHSegment),
fTrdHTrack(o.fTrdHTrack),fLevel1(o.fLevel1),fLevel3(o.fLevel3),fBeta(o.fBeta),fBetaB(o.fBetaB),fCharge(o.fCharge),
fVertex(o.fVertex),fParticle(o.fParticle),fAntiMCCluster(o.fAntiMCCluster),fTrMCCluster(o.fTrMCCluster),
fTofMCCluster(o.fTofMCCluster),fEcalMCHit(o.fEcalMCHit),fTrdMCCluster(o.fTrdMCCluster),
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
  size+=sizeof(ChargeR)*fCharge.size();
  size+=sizeof(VertexR)*fVertex.size();
  size+=sizeof(ParticleR)*fParticle.size();
  size+=sizeof(DaqEventR)*fDaqEvent.size();
  return size;
}

char * ParticleR::Info(int number, AMSEventR* pev){
  double anti=AntiCoo[0][2];
   float btof=0;
   if(iBeta()>=0){
    if(pev){
     BetaR bta=pev->Beta(iBeta());
    btof=bta.Beta;
   }}
   if(fabs(anti)>fabs(AntiCoo[1][2]))anti=AntiCoo[1][2];
    sprintf(_Info," Particle %s No %d Id=%d p=%7.3g#pm%6.2g M=%7.3g#pm%6.2g #theta=%4.2f #phi=%4.2f Q=%2.0f  #beta=%6.3f#pm%6.3f/%6.2f  Coo=(%5.2f,%5.2f,%5.2f) AntiC=%5.2f ",pType(),number,Particle,Momentum,ErrMomentum,Mass,ErrMass,Theta,Phi,Charge,Beta,ErrBeta,btof,Coo[0],Coo[1],Coo[2],anti);
return _Info;


 }
#ifdef _PGTRACK_
#include "TrRecon.h"
#endif

void AMSEventR::InitDB(TFile *_FILE){
static int master=0;
#ifdef _PGTRACK_
//if(TkDBc::Head==0&& _EVENT!=0){
//  TkDBc::CreateTkDBc();
//  TkDBc::Head->init((_EVENT->Run()>=1257416200)?2:1);
//}
#pragma omp master 
{
  if (_FILE){
    if(TrCalDB::Head) delete TrCalDB::Head;
    TrCalDB::Head = (TrCalDB*)_FILE->Get("TrCalDB");
    if(!TkDBc::Head){
      if (!TkDBc::Load(_FILE)) { // by default get TkDBc from _FILE
	TkDBc::CreateTkDBc();    // Init nominal TkDBc if not found in _FILE
	TkDBc::Head->init((Run()>=1257416200)?2:1);
      }
    }

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

    if(TrParDB::Head) delete TrParDB::Head;
    TrParDB::Head = (TrParDB*) _FILE->Get("TrParDB");
    if (!TrParDB::Head) {
      TrParDB* cc = new TrParDB();
      cc->init();
    }
    TrClusterR::UsingTrParDB(TrParDB::Head);
 
    TrClusterR::UsingTrCalDB(TrCalDB::Head);
    TrRawClusterR::UsingTrCalDB(TrCalDB::Head);
    TrRecon::UsingTrCalDB(TrCalDB::Head);
}
master=1;  
}
 while (master==0){
   usleep(1);
 }
#endif
  

}


bool AMSEventR::InitSetup(TFile *_FILE, char *name,uinteger time){
static int master=0;
bool suc=false;
#pragma omp master 
{
  if (_FILE){
    _TreeSetup=(TTree*)_FILE->Get(name);
    if(_TreeSetup){
      if(getsetup()){
       
      }
      else{
         getsetup()=new AMSSetupR();
     }
     getsetup()->Init(_TreeSetup);
     
     while(_EntrySetup+1<_TreeSetup->GetEntries() && (getsetup()->fHeader.FEventTime>time || getsetup()->fHeader.LEventTime<time)) {
      _TreeSetup->GetEntry(++_EntrySetup);
     };
     
     suc=!(getsetup()->fHeader.FEventTime>time || getsetup()->fHeader.LEventTime<time);
     
}

}
master=1;  
}
 while (master==0){
   usleep(1);
 }
  
return suc;
}
