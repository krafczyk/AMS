#include <math.h>
#include "HistoMan.h"
#include "TFile.h"


void HistoMan::Save(){
  if(!enabled) return;
  TFile* pp=TFile::Open(fname,"RECREATE");
  pp->cd();
  fhist.Write();
  pp->Write();
  pp->Close();
  //  fdir->Purge();
  return;
}


void HistoMan::Fill(char* name, double a,double  b,double w){
  if(!enabled) return;
  TH1*hist = (TH1*) fhist.FindObject(name);
  if(!hist){
    printf("HistoMan::Fill: Error Unknown histo %s\n",name);
    return;
  }
#pragma omp critical (hmanfill)
{
  if( hist->InheritsFrom("TH2"))
    ((TH2*)hist)->Fill(a,b,w);
  else if( hist->InheritsFrom("TH1"))
    ((TH1*)hist)->Fill(a,b);
} 
  return;
}



TH1F* TH1F_L(char *name,char* title,int nbin, float low, float up){
if(low<=0||up<=0){
printf("TH1F_L: Error low or up lower than 0 not allowed on log\n");
return 0;
}
  float lowL = log10(low);
  float upL  = log10(up);
  float step = (upL-lowL)/nbin;
  float *bins= new float[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);

  TH1F* hh= new TH1F(name,title,nbin, bins);
  delete bins;
  return hh;
}

TH2F* TH2F_L(char *name,char* title,int nbin, float low, float up, int nbiny, float lowy, float upy){
  if(low<=0||up<=0|| lowy<=0||upy<=0 ){
    printf("TH2F_L: Error low or up lower than 0 not allowed on log\n");
    return 0;
  }
  float lowL = log10(low);
  float upL  = log10(up);
  float step = (upL-lowL)/nbin;
  float *bins= new float[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);


  float lowyL = log10(lowy);
  float upyL  = log10(upy);
  float stepy = (upyL-lowyL)/nbiny;
  float *binsy= new float[nbiny+1];
  binsy[nbiny]=upy;
  for (int ii=0;ii<nbiny;ii++)
    binsy[ii]=pow(10,lowyL+ii*stepy);



  TH2F* hh= new TH2F(name,title,nbin, bins, nbiny, binsy );
  delete bins;
  delete binsy;
  return hh;
}

TH1D* TH1D_L(char *name,char* title,int nbin, double low, double up){
if(low<=0||up<=0){
printf("TH1D_L: Error low or up lower than 0 not allowed on log\n");
return 0;
}
  double lowL = log10(low);
  double upL  = log10(up);
  double step = (upL-lowL)/nbin;
  double *bins= new double[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);

  TH1D* hh= new TH1D(name,title,nbin, bins);
  delete bins;
  return hh;
}

TH2D* TH2D_L(char *name,char* title,int nbin, double low, double up, int nbiny, double lowy, double upy){
  if(low<=0||up<=0|| lowy<=0||upy<=0 ){
    printf("TH2D_L: Error low or up lower than 0 not allowed on log\n");
    return 0;
  }
  double lowL = log10(low);
  double upL  = log10(up);
  double step = (upL-lowL)/nbin;
  double *bins= new double[nbin+1];
  bins[nbin]=up;
  for (int ii=0;ii<nbin;ii++)
    bins[ii]=pow(10,lowL+ii*step);


  double lowyL = log10(lowy);
  double upyL  = log10(upy);
  double stepy = (upyL-lowyL)/nbiny;
  double *binsy= new double[nbiny+1];
  binsy[nbiny]=upy;
  for (int ii=0;ii<nbiny;ii++)
    binsy[ii]=pow(10,lowyL+ii*stepy);



  TH2D* hh= new TH2D(name,title,nbin, bins, nbiny, binsy );
  delete bins;
  delete binsy;
  return hh;
}
//==============================================================
//==============================================================
//==============================================================


void HistoMan::BookHistos(){
  if(!enabled) return;


  Add(TH1D_L("TrTime","TrTime",400,1e-6,1E4));

}
