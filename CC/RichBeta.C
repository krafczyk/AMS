#include "RichBeta.h"
#include "RichTools.h"
#include "TFile.h"
#include "root.h"

RichBetaUniformityCorrection* RichBetaUniformityCorrection::_head=0;

bool RichBetaUniformityCorrection::Init(TString file){
  if(_head) delete _head; _head=0;
  if(file==""){
    // Get default
#ifndef _PGTRACK_
    file=Form("%s/v4.00/RichBetaUniformityCorrection.root",getenv("AMSDataDir"));
#else
    file=Form("%s/v5.00/RichBetaUniformityCorrection.root",getenv("AMSDataDir"));
#endif
  }

  bool fail=false;
#pragma omp critical  
  if(!_head){
    _head=new RichBetaUniformityCorrection;
    if(_head){
      TFile *currentFile=0;
      if(gDirectory) currentFile=gDirectory->GetFile();
      TFile f(file);
      _head->_agl=(GeomHashEnsemble*)f.Get("BetaAgl");
      _head->_naf=(GeomHashEnsemble*)f.Get("BetaNaF");
      f.Close();
      if(currentFile) currentFile->cd();
      
      if(!_head->_agl || !_head->_naf) fail=true;
    }else fail=true;
  }

  if(fail){if(_head) delete _head;_head=0;return false;}

  return true;
}

float RichBetaUniformityCorrection::getCorrection(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}

float RichBetaUniformityCorrection::getCorrection(float *x){
  return getCorrection(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getCorrection(RichRingR *ring){
  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return 1.0/corr->MeanPeak;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return 1.0/corr->MeanPeak;
}


///////////////////////////////////////

float RichBetaUniformityCorrection::getRms(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

float RichBetaUniformityCorrection::getRms(float *x){
  return getRms(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getRms(RichRingR *ring){

  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanRms;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanRms;
}

///////////////////////////////////////

float RichBetaUniformityCorrection::getWidth(float x,float y,float vx,float vy){
  // Determine if it is AGL or NaF
  GeomHashEnsemble *corr=_agl;
  if(RichRingR::getTileIndex(x,y)==121) corr=_naf;
  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}

float RichBetaUniformityCorrection::getWidth(float *x){
  return getWidth(x[0],x[1],x[2],x[3]);
}

float RichBetaUniformityCorrection::getWidth(RichRingR *ring){

  GeomHashEnsemble *corr=_agl;
  if(ring->IsNaF()) corr=_naf;

  float x=ring->AMSTrPars[0];
  float y=ring->AMSTrPars[1];
  float theta=ring->AMSTrPars[3];
  float phi=ring->AMSTrPars[4];

  float vx=sin(theta)*cos(phi);
  float vy=sin(theta)*sin(phi);
  if(cos(theta)>0) {vx*=-1;vy*=-1;} // Keep a coherent definition

  if(_latest[0]==x && _latest[1]==y && _latest[2]==vx && _latest[3]==vy) return corr->MeanPeakWidth;
  _latest[0]=x;_latest[1]=y;_latest[2]=vx;_latest[3]=vy;
  corr->Eval(x,y,vx,vy);
  return corr->MeanPeakWidth;
}
