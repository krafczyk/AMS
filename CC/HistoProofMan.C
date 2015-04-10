#include "HistoProofMan.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TH3D.h"
#include "TFile.h"
#include "TROOT.h"
ClassImp(threp);

  

threp::threp(){
  htype=knull;
  nbin[0]=0;
  nbin[1]=0;
  nbin[2]=0;
 
}
  
threp::threp(const threp& orig){
  *this=orig;
}

threp& threp::operator=(const threp& orig){
  if(&orig==this)
    return *this;
  
  htype=orig.htype;
  name=orig.name;
  title=orig.title;
  opt=orig.opt;
  nbin[0]=orig.nbin[0];
  nbin[1]=orig.nbin[1];
  nbin[2]=orig.nbin[2];
  xlim=orig.xlim;
  ylim=orig.ylim;
  zlim=orig.zlim;

  return *this;
}



threp::threp(const TH1* hin){
  nbin[0]=0;
  nbin[1]=0;
  nbin[2]=0;
  
  name=hin->GetName();
  title=hin->GetTitle();
  opt=hin->GetOption();
  htype=getkind(hin);
  int ndim=getdimension();
  if(ndim>0){
    nbin[0]=hin->GetNbinsX();
    for (int jj=0;jj<nbin[0];jj++)
      xlim.push_back(hin->GetXaxis()->GetBinLowEdge(jj+1));
    xlim.push_back(hin->GetXaxis()->GetBinUpEdge(nbin[0]));
  }
  if(ndim>1){
    nbin[1]=hin->GetNbinsY();
    for (int jj=0;jj<nbin[1];jj++)
      ylim.push_back(hin->GetYaxis()->GetBinLowEdge(jj+1));
    ylim.push_back(hin->GetYaxis()->GetBinUpEdge(nbin[1]));
  }
  
  if(ndim>2){
    nbin[2]=hin->GetNbinsZ();
    for (int jj=0;jj<nbin[2];jj++)
      zlim.push_back(hin->GetZaxis()->GetBinLowEdge(jj+1));
    zlim.push_back(hin->GetZaxis()->GetBinUpEdge(nbin[2]));
  }

  if(htype==ktprofile){
    double ymin=((TProfile*)hin)->GetYmin();
    double ymax=((TProfile*)hin)->GetYmax();
    if(ymin==ymax) nbin[1]=0;
    else{
      nbin[1]=2;
      ylim.push_back(ymin);
      ylim.push_back(ymax);
    }
  }

  if(htype==ktprofile2d){
    double zmin=((TProfile2D*)hin)->GetZmin();
    double zmax=((TProfile2D*)hin)->GetZmax();
    if(zmin==zmax) nbin[2]=0;
    else{
      nbin[2]=2;
      zlim.push_back(zmin);
      zlim.push_back(zmax);
    }
  }

}

threp::hkinds threp::getkind(const TH1* hin){
  std::string htyp=hin->ClassName();
  if( htyp=="TH1F")
    return kth1f;
  else if( htyp=="TH1D")
    return kth1d;
  else if( htyp=="TH2F")
    return kth2f;
  else if( htyp=="TH2D")
    return kth2d;
  else if( htyp=="TH3F")
    return kth3f;
  else if( htyp=="TH3D")
    return kth3d;
  else if( htyp=="TProfile")
    return ktprofile;
  else if( htyp=="TProfile2D")
    return ktprofile2d;
  else return knull;

}


int threp::getdimension(){
  if( 
     (htype==kth1f)||
     (htype==kth1d)||
     (htype==ktprofile)
     )
    return 1;
  else if(
	  (htype==kth2f)||
	  (htype==kth2d)||
	  (htype==ktprofile2d)
	  )
    
    return 2;
  else if(
	  (htype==kth3f)||
	  (htype==kth3d)
	  )
    return 3;
  else return 0;
}



TH1* threp::gethisto()const {


  switch (htype){

  case kth1f:
    return new TH1F(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]));
    break;
  case kth1d:
    return new TH1D(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]));
    break;

  case kth2f:
    return new TH2F(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]),
		    nbin[1],&(ylim[0]));
    break;
  case kth2d:
    return new TH2D(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]),
		    nbin[1],&(ylim[0]));
    break;


  case kth3f:
    return new TH3F(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]),
		    nbin[1],&(ylim[0]),
		    nbin[2],&(zlim[0]));
    break;
  case kth3d:
    return new TH3D(name.c_str(),title.c_str(),
		    nbin[0],&(xlim[0]),
		    nbin[1],&(ylim[0]),
		    nbin[2],&(zlim[0]));
    break;

  case ktprofile:
    if(nbin[1]==0)
      return new TProfile(name.c_str(),title.c_str(),
			  nbin[0],&(xlim[0]),opt.c_str());
    else
      return new TProfile(name.c_str(),title.c_str(),
			  nbin[0],&(xlim[0]),
			  ylim[0],ylim[1],opt.c_str());
    break;

  case ktprofile2d:
    if(nbin[2]==0)
      return new TProfile2D(name.c_str(),title.c_str(),
			    nbin[0],&(xlim[0]),
			    nbin[1],&(ylim[0]),opt.c_str());
    else
      return new TProfile2D(name.c_str(),title.c_str(),
			    nbin[0],xlim[0],xlim[nbin[0]],
			    nbin[1],ylim[0],xlim[nbin[1]],
			    zlim[0],zlim[1],opt.c_str());
    break;

  default:
    return 0;

  }

}

ClassImp(HistoProofMan);

void HistoProofMan::Fill(const char * name, double a,double  b,double w){

  TH1*hist = Get(name);
  if(!hist){
    static int nerr = 0;
    if (nerr++ < 20)
      printf("HistoMan::Fill: Error Unknown histo %s\n",name);
    return;
  }

  int ndim = hist->GetDimension();
  if      (ndim == 3 && dynamic_cast<TH3*>(hist)) ((TH3*)hist)->Fill(a,b,w);
  else if (ndim == 2){
    if (hist->ClassName() == TString("TProfile2D") &&
	dynamic_cast<TProfile*>(hist)) ((TProfile2D*)hist)->Fill(a,b,w);
    else if( dynamic_cast<TH2*>(hist)) ((TH2*)hist)->Fill(a,b,w);
  }
  else if (ndim == 1) {
    if (hist->ClassName() == TString("TProfile") &&
	dynamic_cast<TProfile*>(hist)) ((TProfile*)hist)->Fill(a,b,w);
    else if (dynamic_cast<TH1*>(hist)) ((TH1*)     hist)->Fill(a,b);
  }

  return;
}

HistoProofMan::~HistoProofMan(){
  rlist.clear();
  // hashtable<TH1*>::iterator it;
//   for( it=hlist.begin();it!=hlist.end();it++)
//     if(it->second){ delete it->second; it->second=0;}
  hlist.clear();


}


void HistoProofMan::Add(TH1* hist ){

  rlist.Add(hist->GetName(),threp(hist));
  if(hist) 
    delete hist;
  return;

}
void HistoProofMan::Save(){

  if(!fname[0])return;
  printf("HistoMan::Save ----> Saving %s\n",fname);
  TDirectory *dsave = gDirectory;
  TFile* pp=TFile::Open(fname,"RECREATE");
  pp->cd();
  hashtable<TH1*>::iterator it;
  for(it=hlist.begin();it!=hlist.end();it++) {
    TH1 *obj = it->second;
    if (obj && obj->GetEntries() > 0) obj->Write();
  }
  pp->Write();
  pp->Close();
  printf(" ..... done\n");
  if (dsave) dsave->cd();
  //  fdir->Purge();
  return;
}


const threp* HistoProofMan::GetRep(const char* nn){
  hashtable<threp>::const_iterator ll;
  int ret=rlist.find(nn,ll);
  if(ret) return &(ll->second);
    else return 0;
}
TH1* HistoProofMan::GetNew(const char * name){
    hashtable<threp>::const_iterator ll;
    int ret=rlist.find(name,ll);
    if(ret)
      return ll->second.gethisto();
    else 
      return 0;
  }


void HistoProofMan::CreateHistos(){
    hashtable<threp>::iterator it;
    for( it=rlist.begin();it!=rlist.end();it++){
      AddH(it->second.gethisto());
    }
    return;
} 
TH1* HistoProofMan::Get(const char * name){
    hashtable<TH1*>::const_iterator hh;
    int ret=hlist.find(name,hh);
    if(ret) return hh->second;
    else return 0;
}
