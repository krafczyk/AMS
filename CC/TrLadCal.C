//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadCal.C
///\brief Source file of TrLadCal class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 AO  Some methods are added 
///
//////////////////////////////////////////////////////////////////////////
#include "TrLadCal.h"
#include "TkDBc.h"

#include <cstdio>
#include <string>
#include <cstring>    // for memset
#include <iostream>
#include <vector>
#include <algorithm>


ClassImp(TrLadCal);

int TrLadCal::dead=0x5;
// Default to Flight format
int TrLadCal::version=3;

void  TrLadCal::SetVersion(int ver){
  if      (ver==3) { version=ver; printf("TrLadCal:: Set the Flight Cal Format %d\n",ver); }
  else if (ver==2) { version=ver; printf("TrLadCal:: Set the Pre-Flight Cal Format %d\n",ver); }
  else if (ver==1) { version=ver; printf("TrLadCal:: Set the Pre-Integration Cal Format %d\n",ver); }
  else             { printf("TrLadCal:: Unknown Cal Format %d\n I Give Up",ver); exit(3); }
}

float TrLadCal::AddNoiseK[9] = { 0,0,0,0,0,0,0,0,0 };
float TrLadCal::AddNoiseS[9] = { 0,0,0,0,0,0,0,0,0 };

float TrLadCal::GetSigma(int ii)
{
  int  tkid = TkDBc::Head->HwId2Tkid(HwId);
  int  layj = TkDBc::Head->GetJFromLayer(abs(tkid/100));
  float add = 0;
  if (1 <= layj && layj <= 9)
    add = (ii >= 640) ? AddNoiseK[layj-1] : AddNoiseS[layj-1];

  float sig = _getnum(_Sigma,ii);
  if (add > 0) sig = sqrt(sig*sig+add*add);

  return sig;
}

geant TrLadCal::_getnum(geant * pp, int ii){
  // if (!_filled||ii<0||ii>1023) return -9999;
  return pp[ii];
}

short int TrLadCal::_getnum(short int * pp, int ii){
  // if (!_filled||ii<0||ii>1023) return -9999;
  return pp[ii];
}

unsigned short int TrLadCal::_getnum(unsigned short int * pp, int ii){
  // if (!_filled||ii<0||ii>1023) return -9999;
  return pp[ii];
}

void TrLadCal::_setnum(geant * pp, int ii,geant val){
  if (!_filled||ii<0||ii>1023) return;
  pp[ii]=val;
}

void TrLadCal::_setnum(short int * pp, int ii,short int val){
  if (!_filled||ii<0||ii>1023) return;
  pp[ii]=val;
}

TrLadCal::TrLadCal(short int hwid) {
  HwId=hwid;
  _filled=0;
  Clear();
}

unsigned short int TrLadCal::OccupancyGaus(int ii) { 
  if (GetDSPCodeVersion()<0xa902) {
    // Occupancy from calibration 
    return _getnum(_OccupancyGaus,ii);
  }
  // Occupancy table from data
  return (_getnum(_OccupancyGaus,ii)&0x7fff);
}


void TrLadCal::Fill_old(CaloutDSP* cc) {
  for (int ii=0;ii<320;ii++)    _Sigma[ii] = cc->sig[ii]/8./(cc->S1_lowthres*1.); 
  for (int ii=320;ii<640;ii++)  _Sigma[ii] = cc->sig[ii]/8./(cc->S2_lowthres*1.);
  for (int ii=640;ii<1024;ii++) _Sigma[ii] = cc->sig[ii]/8./(cc->K_lowthres*1.);
  Fill_same(cc);
}


void TrLadCal::Fill_new(CaloutDSP* cc) {
  for (int ii=0;ii<1024;ii++)  _Sigma[ii] = cc->sig[ii]/8.;
  Fill_same(cc);
}


void TrLadCal::Fill_same(CaloutDSP* cc) {
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]      = short(int(cc->ped[ii])&0xFFFF)/8.;
    _SigmaRaw[ii]      = cc->rsig[ii]/8./(cc->sigrawthres*1.);
    _Status[ii]        = cc->status[ii];
    _Occupancy[ii]     = cc->occupancy[ii];
    _OccupancyGaus[ii] = cc->occupgaus[ii];
  }
  for(int ii=0;ii<16;ii++){
    _CNmean[ii]=cc->CNmean[ii];
    _CNrms[ii]=cc->CNrms[ii];
  }
  dspver         = cc->dspver;
  S1_lowthres    = cc->S1_lowthres;
  S1_highthres   = cc->S1_highthres;
  S2_lowthres    = cc->S2_lowthres;
  S2_highthres   = cc->S2_highthres;
  K_lowthres     = cc->K_lowthres;
  K_highthres    = cc->K_highthres;
  sigrawthres    = cc->sigrawthres;
  calstatus      = cc->calstatus;    
  Power_failureS = cc->Power_failureS;
  Power_failureK = cc->Power_failureK;
  _filled=1;
}


void TrLadCal::CopyCont(TrLadCal& orig){
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]      = orig._Pedestal[ii];
    _SigmaRaw[ii]      = orig._SigmaRaw[ii];
    _Sigma[ii]         = orig._Sigma[ii];
    _Status[ii]        = orig._Status[ii];
    _Occupancy[ii]     = orig._Occupancy[ii];
    _OccupancyGaus[ii] = orig._OccupancyGaus[ii];
  }
  for(int ii=0;ii<16;ii++){
    _CNmean[ii] = orig._CNmean[ii];
    _CNrms[ii]  = orig._CNrms[ii];
  }
}

TrLadCal::TrLadCal(const TrLadCal& orig):TObject(orig){
  HwId  = orig.HwId;
  for (int ii=0;ii<1024;ii++){
    _Pedestal[ii]      = orig._Pedestal[ii];
    _SigmaRaw[ii]      = orig._SigmaRaw[ii];
    _Sigma[ii]         = orig._Sigma[ii];
    _Status[ii]        = orig._Status[ii];
    _Occupancy[ii]     = orig._Occupancy[ii];
    _OccupancyGaus[ii] = orig._OccupancyGaus[ii];
  }
 for(int ii=0;ii<16;ii++){
    _CNmean[ii] = orig._CNmean[ii];
    _CNrms[ii]  = orig._CNrms[ii];
  }
  dspver         = orig.dspver;
  S1_lowthres    = orig.S1_lowthres;
  S1_highthres   = orig.S1_highthres;
  S2_lowthres    = orig.S2_lowthres;
  S2_highthres   = orig.S2_highthres;
  K_lowthres     = orig.K_lowthres;
  K_highthres    = orig.K_highthres;
  sigrawthres    = orig.sigrawthres;
  calstatus      = orig.calstatus;    
  Power_failureS = orig.Power_failureS;
  Power_failureK = orig.Power_failureK;
}

void TrLadCal::Clear(const Option_t*aa){
  TObject::Clear(aa);
  _filled = 0;
  memset(_Pedestal,0,sizeof(_Pedestal[0])*1024);
  memset(_SigmaRaw,0,sizeof(_SigmaRaw[0])*1024);
  memset(_Sigma   ,0,sizeof(_Sigma[0])*1024);
  memset(_Status  ,0,sizeof(_Status[0])*1024);
  memset(_Status  ,0,sizeof(_Occupancy[0])*1024);
  memset(_Status  ,0,sizeof(_OccupancyGaus[0])*1024);
  for(int ii=0;ii<16;ii++){
    _CNmean[ii] = 0;
    _CNrms[ii]  = 0;
  }
  dspver         = 0;
  S1_lowthres    = 0.;
  S1_highthres   = 0.;
  S2_lowthres    = 0.;
  S2_highthres   = 0.;
  K_lowthres     = 0.;
  K_highthres    = 0.;
  sigrawthres    = 0.;
  calstatus      = 0;
  Power_failureS = 0;
  Power_failureK = 0;
}

void TrLadCal::PrintInfo(int long_format){
  TkLadder* lad=TkDBc::Head->FindHwId(HwId);
  if(!lad) {
    printf("TrLadCal::PrintInfo() - Error - Can't find the ladder with HwId %4d\n",HwId);
    return;
  }
  printf("Ladder Name: %s  Filled: %d\n",lad->name,_filled);
  printf("Layer: %d Slot: %2d  Side: %d \n", 
	 lad->GetLayer(),lad->GetSlot(),lad->GetSide()); 
  printf("Octant: %d %s Crate: %d TDR:  %2d  PwGroup: %d PwgrupPos: %d\n",
	 lad->GetOctant(),TkDBc::Head->GetOctName(lad->GetOctant()),
         lad->GetCrate(),lad->GetTdr(),
         lad->GetPwGroup(),lad->GetPwgroupPos());
  if(long_format){
    printf(" Chan     Ped  SigRaw  Sigma  Occ  OccG  Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7\n");
    for (int ii=0;ii<1024;ii++)
      // printf(" %4d %7.3f  %7.3f %7.3f  %4d \n",
      //             ii,Pedestal(ii),SigmaRaw(ii),Sigma(ii),Status(ii));
      printf(" %4d %7.3f  %7.3f %7.3f  %4hd  %4hd %1d %1d %1d %1d %1d %1d %1d %1d \n",
             ii,GetPedestal(ii),GetSigmaRaw(ii),GetSigma(ii),Occupancy(ii),OccupancyGaus(ii),
             GetStatus(ii)&1,GetStatus(ii)>>1&1,GetStatus(ii)>>2&1,GetStatus(ii)>>3&1,
             GetStatus(ii)>>4&1,GetStatus(ii)>>5&1,GetStatus(ii)>>6&1,GetStatus(ii)>>7&1);
    for(int ii=0;ii<16;ii++){
      printf("VA#:  %2d Common noise mean   %f  rms:  %f\n",ii,_CNmean[ii],_CNrms[ii]);
   }
  }  
}


TrLadCal& TrLadCal::operator-(TrLadCal &orig){
  TrLadCal* pp= new TrLadCal(*this);
  for(int ii=0;ii<1024;ii++){
     pp->SetPedestal (ii,GetPedestal(ii)-orig.Pedestal(ii));
     pp->SetSigmaRaw (ii,GetSigmaRaw(ii)-orig.SigmaRaw(ii));
     pp->SetSigma    (ii,GetSigma(ii)-orig.Sigma(ii));
     pp->SetStatus   (ii,GetStatus(ii)^orig.Status(ii));
  }
  return *pp;
}
 
TrLadCal& TrLadCal::operator=(TrLadCal& orig){
  HwId=orig.HwId;
  for (int ii=0;ii<1024;ii++){
    SetPedestal(ii,orig._Pedestal[ii]);
    SetSigmaRaw(ii,orig._SigmaRaw[ii]);
    SetSigma(ii,orig._Sigma[ii]);
    SetStatus(ii,orig._Status[ii]);
  }
  return *this;
}

//Finds the kth smallest number 
short int TrLadCal::DSPSmallest(short int a[], int n, int k){
  short int i,j,l,m ;
  short int x ;
  l=0 ; m=n-1 ;
  while (l<m) {
    x=a[k] ;
    i=l ;
    j=m ;
    do {
      while (a[i]<x) i++ ;
      while (x<a[j]) j-- ;
      if (i<=j) {
	//	ELEM_SWAP(a[i],a[j]) ;
	int tt=a[i];
	a[i]=a[j];
	a[j]=tt;
	i++ ; j-- ;
      }
    } while (i<=j) ;
    if (j<k) l=i ;
    if (k<i) m=j ;
  }
  return a[k] ;
}

geant TrLadCal::GetDSPSigmaRawOnMediane(int ii) {
  int iVA   = ii/64;
  int first = iVA*64;
  int last  = (iVA+1)*64;
  short int raw[64];
  for (int jj=first; jj<last; jj++) raw[jj-first] = (short int) (8*GetSigmaRaw(jj)); 
  short int mediane  = DSPSmallest(raw,64,32);
  short int sigmaraw = (short int) (8*GetSigmaRaw(ii));
  return 1.*sigmaraw/mediane;
}

geant TrLadCal::GetDSPSigmaOnMediane(int ii) {
  int iADC  = (ii<640) ? ii/320       : 3;
  int first = (iADC<2) ? iADC*320     : 640;
  int last  = (iADC<2) ? (iADC+1)*320 : 1024;
  short int sig[384];
  for (int jj=first; jj<last; jj++) sig[jj-first] = (short int) (8*GetSigma(jj));
  short int mediane = (iADC<2) ? DSPSmallest(sig,320,160) : DSPSmallest(sig,384,192);
  short int sigma   = (short int) (8*GetSigma(ii));
  return 1.*sigma/mediane;
}

geant TrLadCal::GetSigmaMean(int side) {
  int first = (side==0) ? 640  : 0;
  int last  = (side==0) ? 1023 : 639;
  int   ngoodchannels = 0;
  geant mean = 0.;
  for (int ii=first; ii<=last; ii++) {
    if (GetStatus(ii)!=0) continue;
    mean += GetSigma(ii);
    ngoodchannels++;
  }
  return (ngoodchannels>0) ? mean/ngoodchannels : 0;
}

geant TrLadCal::GetSigmaRMS(int side) {
  int first = (side==0) ? 640  : 0;
  int last  = (side==0) ? 1023 : 639;
  int   ngoodchannels = 0;
  geant mean = GetSigmaMean(side);
  geant rms = 0.;
  for (int ii=first; ii<=last; ii++) {
    if (GetStatus(ii)!=0) continue;
    rms += pow(GetSigma(ii)-mean,2);
    ngoodchannels++;
  }
  return sqrt(rms/ngoodchannels);
}
  
int TrLadCal::GetnChannelsByStatus(int statusbit) {
  int nchannels = 0;
  for (int ii=0; ii<=1023; ii++) {
    if (!(GetStatus(ii)>>statusbit&1)) continue;
    nchannels++;
  }
  return nchannels;
} 

int  TrLadCal::Cal2Lin(float* offset){
  if(!offset) return -1;
  offset[0]  = GetHwId();
  offset[1]  = *((float*) &dspver);
  offset[2]  = S1_lowthres;
  offset[3]  = S1_highthres;
  offset[4]  = S2_lowthres;
  offset[5]  = S2_highthres;
  offset[6]  = K_lowthres;
  offset[7]  = K_highthres;
  offset[8]  = sigrawthres;
  offset[9]  = *((float*)&calstatus);
  offset[10] = (float) Power_failureS;
  offset[11] = (float) Power_failureK;
  float* off2=&(offset[12]);
  for (int ii=0;ii<1024;ii++){
    off2[ii]          = _Pedestal[ii];
    off2[ii+  1024]   = _Sigma[ii];
    off2[ii+2*1024]   = _SigmaRaw[ii];
    int aa = _Status[ii];
    off2[ii+3*1024]   = *((float*)&aa);
    int bb = _Occupancy[ii];
    off2[ii+4*1024]   = (float)bb;
  }
  float* off3=&(off2[5*1024]);
  for (int ii=0;ii<16;ii++){
    off3[ii]    = _CNmean[ii];
    off3[ii+16] = _CNrms[ii];
  }
  return 0;
}

int  TrLadCal::Lin2Cal(float* offset){
  if(!offset) return -1;
  if(version==1){
    HwId           = (int)offset[0];
    dspver         = (int)offset[1];
    S1_lowthres    = offset[2];
    S1_highthres   = offset[3];
    S2_lowthres    = offset[2];
    S2_highthres   = offset[3];
    K_lowthres     = offset[2];
    K_highthres    = offset[3];
    sigrawthres    = offset[4];
    calstatus      = (int)offset[5];
    Power_failureS = 0;
    Power_failureK = 0;
    float* off2=&(offset[6]);
    for (int ii=0;ii<1024;ii++){
      _Pedestal[ii] = off2[ii];
      _Sigma[ii]    = off2[ii+  1024];
      _SigmaRaw[ii] = off2[ii+2*1024];
      int aa = (int)off2[ii+3*1024];
      _Status[ii]   = aa; 
      _Occupancy[ii]     = 0;
      _OccupancyGaus[ii] = 0;
    }
  } 
  else if (version==2) {
    HwId           = (int)offset[0];
    dspver         = (int)offset[1];
    S1_lowthres    = offset[2];
    S1_highthres   = offset[3];
    S2_lowthres    = offset[4];
    S2_highthres   = offset[5];
    K_lowthres     = offset[6];
    K_highthres    = offset[7];
    sigrawthres    = offset[8];
    calstatus      = (int)offset[9];
    Power_failureS = (int)offset[10];
    Power_failureK = (int)offset[11];
    float* off2=&(offset[12]);
    for (int ii=0;ii<1024;ii++){
      _Pedestal[ii]  = off2[ii];
      _Sigma[ii]     = off2[ii+  1024];
      _SigmaRaw[ii]  = off2[ii+2*1024];
      int aa = (int) off2[ii+3*1024];
      _Status[ii]    = aa;
      int bb = (int) off2[ii+4*1024];
      _Occupancy[ii] = bb;
      _OccupancyGaus[ii] = 0;
    }
    float* off3=&(off2[5*1024]);
    for (int ii=0;ii<16;ii++){
      _CNmean[ii] = off3[ii];
      _CNrms[ii]  = off3[ii+16];
    }  
  }
  else if (version==3) {
    HwId           = (int)offset[0];
    dspver         = *((int*)&(offset[1]));
    S1_lowthres    = offset[2];
    S1_highthres   = offset[3];
    S2_lowthres    = offset[4];
    S2_highthres   = offset[5];
    K_lowthres     = offset[6];
    K_highthres    = offset[7];
    sigrawthres    = offset[8];
    calstatus      = *((int*)&(offset[9]));
    Power_failureS = (int)offset[10];
    Power_failureK = (int)offset[11];
    float* off2=&(offset[12]);
    for (int ii=0;ii<1024;ii++){
      _Pedestal[ii]  = off2[ii];
      _Sigma[ii]     = off2[ii+  1024];
      _SigmaRaw[ii]  = off2[ii+2*1024];
      int aa = *((int*) &(off2[ii+3*1024]));
      _Status[ii]    = aa;
      int bb = (int) off2[ii+4*1024];
      _Occupancy[ii] = bb;
      _OccupancyGaus[ii] = 0;
    }
    float* off3=&(off2[5*1024]);
    for (int ii=0;ii<16;ii++){
      _CNmean[ii] = off3[ii];
      _CNrms[ii]  = off3[ii+16];
    }
  }
  return 0;
}

TH1F* TrLadCal::DrawOccupancy(){
  TH1F* pp=new TH1F("occ","occ",1024,0,1024);
  for (int ii=0;ii<1024;ii++){
    pp->SetBinContent(ii+1,_Occupancy[ii]*1.);
    
  }
  pp->Draw();
  return pp;
}

TH1F* TrLadCal::DrawStatus(unsigned short  mask){
  TH1F* pp=new TH1F("occ","occ",1024,0,1024);
  for (int ii=0;ii<1024;ii++) pp->SetBinContent(ii+1,Status(ii)&mask);
  pp->Draw();
  return pp;
}
