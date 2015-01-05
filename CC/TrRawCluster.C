/// $Id$ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrRawCluster.C
///\brief Source file of AMSTrRawCluster class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/18 SH  Updates for TrCluster support
///\date  2008/01/18 AO  Some analysis methods 
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date$
///
/// $Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrRawCluster.h"
#include "TkDBc.h"
#include <iostream>
#include <string>

ClassImp(TrRawClusterR);

TrCalDB* TrRawClusterR::_trcaldb = NULL;

TrRawClusterR::TrRawClusterR(void) {
  Clear();
}

void TrRawClusterR::Clear() {
  _tkid        =  0;
  _addressword = -1;
  _lengthword  =  0;
  Status       =  0;
  _signal.clear();
}

TrRawClusterR::TrRawClusterR(const TrRawClusterR &orig):TrElem(orig)  {
  _tkid        = orig._tkid;
  _addressword = orig._addressword;
  _lengthword  = orig._lengthword;
  Status       = orig.Status;
  for (unsigned int i = 0; i < orig._signal.size(); i++) _signal.push_back(orig._signal.at(i));
}

TrRawClusterR::TrRawClusterR(int tkid, int clsaddwrd, int clslenwrd, short int* adc) {
  _tkid        = tkid;
  _addressword = clsaddwrd;
  _lengthword  = clslenwrd; 
  Status       = 0;
  _signal.reserve((_lengthword&0x7f)+1);
  if (adc) for (int i = 0; i <((_lengthword&0x7f)+1) ; i++) _signal.push_back(adc[i]/8.);
}

TrRawClusterR::TrRawClusterR(int tkid, int address, int nelem, float *adc) {
  _tkid        = tkid;
  _addressword = address;
  _lengthword  = nelem;
  Status       = 0;
  _signal.reserve((_lengthword&0x7f)+1);
  if(adc) for (int i = 0; i < ((_lengthword&0x7f)+1); i++) _signal.push_back(adc[i]);
}

int TrRawClusterR::GetHwId() const {
  TkLadder* lad=TkDBc::Head->FindTkId(GetTkId());
  if (lad) return lad->GetHwId();
  else return -1;
}


float TrRawClusterR::GetDSPSeedSN() {
  if (GetTrLadCal()==0) {
    printf("TrRawClusterR::GetDSPSeedSN, WARNING TrLadCal not found! HwId = %+03d\n",GetHwId());
    return 0;
  }
  int dspver = GetTrLadCal()->GetDSPCodeVersion();

  if      (dspver<=0x9a11) return -1; 
  else if ( (dspver>0x9a11) && (dspver<=0xa810) ) { 
    // this conversion is not exact, because a cluster could begin in S1 but the seed sould be on S2 ...
    int seedadd = GetAddress(max_element(_signal.begin(),_signal.end()) - _signal.begin());
    int ADC     = seedadd/320;
    int value = (_lengthword>>7)&0x1ff;
    if      (ADC==0) return value*GetTrLadCal()->S1_lowthres; // S1
    else if (ADC==1) return value*GetTrLadCal()->S2_lowthres; // S2
    else             return value*GetTrLadCal()->K_lowthres;  // K
  }
  else if (dspver>0xa810) {
    int value = (_lengthword>>7)&0x1ff;
    if (value<0x1ff) return value/4.;  
    else             return 4000.; // overflow  
  }

  return 0;
}


float TrRawClusterR::GetNoise(int ii) {
  if (GetTrCalDB()==0) {
    printf("TrRawClusters::GetNoise Error, no _trcaldb specified.\n");
    return -9999.; 
  }
  int hwid = GetHwId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_HwId(hwid);
  if (!ladcal) {
    static int nerr=0;
    if (nerr++<100) printf ("TrRawClusterR::GetNoise, WARNING calibration not found!! HwID %+03d\n",hwid); 
    return -9999;
  } 
  int address = GetAddress()+ii;
  return (float) ladcal->GetSigma(address);
}

short TrRawClusterR::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("TrRawClusters::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int hwid = GetHwId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_HwId(hwid);
  if (!ladcal) {printf ("TrRawClusterR::GetStatus, WARNING calibration not found!!HwID %+03d\n",hwid); return -9999;} 
  int address = GetAddress()+ii;
  return (short)ladcal->GetStatus(address);
}

std::ostream &TrRawClusterR::putout(std::ostream &ostr){
  return ostr << _PrepareOutput(1) << std::endl;
}

void TrRawClusterR::Print(int full) { 
  cout << _PrepareOutput(full);
}

std::string TrRawClusterR::_PrepareOutput(int full) { 
  std::string sout;
  sout.append(Form("TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d Signal: %7.2f  SeedSN: %7.2f  CNStatus: %2d  PwStatus: %1d\n",
		   GetTkId(),GetSide(),GetAddress(),GetNelem(),GetTotSignal(),GetDSPSeedSN(),GetCNStatus(),GetPowerStatus()));
  if(!full) return sout;
  for (int ii=0; ii<GetNelem(); ii++) 
    sout.append(Form("Address: %4d  Signal: %8.3f  Sigma: %8.3f  S/N: %8.3f  Status: %3d\n",
	       ii+GetAddress(),GetSignal(ii),GetSigma(ii),GetSN(ii),GetStatus(ii)));
  return sout;
}

const char* TrRawClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrRawCluster #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}

int TrRawClusterR::GetSeedIndex(float thseed){
  float maxadc  = -1000.;
  int   seedadd = -1;
#ifdef PZVERS
  for (int ii=0; ii<GetNelem(); ii++) { 
    if ( (GetSN(ii)>3.5)&&(GetStatus(ii)==0) ) { 
      maxadc  = GetSignal(ii); 
      seedadd = ii; 
      break;
    }
  }	 
  if(seedadd>=0) {
    if(GetSN(seedadd)>thseed) return seedadd;
    else return seedadd*-1.;
  }
  return -1;
#else
  for (int ii=0; ii<GetNelem(); ii++) { 
    if ( (GetSignal(ii)>maxadc)&& (GetSN(ii)>thseed) &&(GetStatus(ii)==0) ) { 
      maxadc  = GetSignal(ii); 
      seedadd = ii; 
    }
  }
  return seedadd;
#endif
}

int TrRawClusterR::GetSeedAddress(float thseed){
  Int_t cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  return cstrip + GetAddress();
}

int TrRawClusterR::GetStatusnStrips(int nstrips, float thseed, float thneig){
  Int_t cstrip = GetSeedIndex(thseed);
  Int_t nleft  = GetLeftLength(thseed,thneig);
  Int_t nright = GetRightLength(thseed,thneig);
  if ( (cstrip<0)||(nleft<0)||(nright<0) ) return -1; 

  Int_t lindex = 0;
  Int_t rindex = 0;
  Int_t nside  = 0;
  Int_t sum    = 0;
  // number strip selection
  if (nstrips%2){ // odd (same number of left/right strips)
    nside  = (nstrips-1)/2;
    lindex = cstrip-nside;
    rindex = cstrip+nside;
  }
  else { // even (take the highest strip between the n-ths left and right strips)
    nside = nstrips/2;
    if ( (nleft<=nside)&&(nright<=nside) ) { // se posso fare il confronto
      if (GetSignal(cstrip-nside)>=GetSignal(cstrip+nside)){
	lindex = cstrip-nside;
	rindex = cstrip+nside - 1;
      }
      else {
	lindex = cstrip-nside + 1;
	rindex = cstrip+nside;
      }
    }
    else { // altrimenti prendo il massimo dal lato in cui posso
      lindex = cstrip-nside;
      rindex = cstrip+nside;
    }
  }
  // check sull'overflow
  if (lindex<0)          lindex = 0;
  if (rindex>GetNelem()) rindex = GetNelem();
  for (int ii=lindex; ii<rindex; ii++) sum += GetStatus(ii);

  return sum;
}

int TrRawClusterR::GetLength(float thseed,float thneig) {
  if ( (GetLeftLength(thseed,thneig)<0)||(GetRightLength(thseed,thneig)<0) ) return -1;
  return GetLeftLength(thseed,thneig) + GetRightLength(thseed,thneig) + 1;
}

int TrRawClusterR::GetLeftLength(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  int length = 0;
  for (int ii=cstrip-1; ii>=0; ii--) {
    if (GetSN(ii)<thneig) break;
    length++;
  }
  return length;
}

int TrRawClusterR::GetRightLength(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1;
  int length = 0;
  for (int ii=cstrip+1; ii<GetNelem(); ii++) {
    if (GetSN(ii)<thneig) break;
    length++;
  }
  return length;
}

float TrRawClusterR::GetTotSignal(float thseed,float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  float sum = GetSignal(cstrip);
  for (int ii=cstrip-1; ii>=0; ii--) {
    if (GetSN(ii)<thneig) break;
    sum += GetSignal(ii);
  }
  for (int ii=cstrip+1; ii<GetNelem(); ii++) {
    if (GetSN(ii)<thneig) break;
    sum += GetSignal(ii);
  }
  return sum;
}

float TrRawClusterR::GetTotSignal2(float thseed,float thneig) {
  int cstrip = abs(GetSeedIndex(thseed));
  float sum = GetSignal(cstrip);
  float left=0;
  float right=0;
  float add=0;
  if(cstrip>0) left= GetSignal(cstrip-1);
  else left=0;
  if(cstrip<((int)_signal.size()-1)) right= GetSignal(cstrip+1);
  else right=0;

  if( right > left) add=right;
  else add=left;
  if(add>0) sum+=add;
    
  
  if (GetSeedIndex(thseed)<0) return sum*-1.;
  return sum;
}

float TrRawClusterR::GetSeedSignal(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetSignal(cstrip);
}

float TrRawClusterR::GetSeedNoise(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetNoise(cstrip);
}

float TrRawClusterR::GetSeedSN(float thseed){
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  return GetSN(cstrip);
}

float TrRawClusterR::GetEta(float thseed,float thneig){
  /*
    eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )

       _                                    _
     l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
      | |_    eta = --------- = ---       _| |       eta = --------- = ---
     _| | |           c + r     c+r      | | |_              l + c     l+c
  __|_|_|_|__                          __|_|_|_|__
       0 1                                0 1

       - eta is 1 when the particle is near to the right strip,
       while is approx 0 when is near to the left strip
  */
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  int nleft  = GetLeftLength(thseed,thneig);
  int nright = GetRightLength(thseed,thneig);
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return 0.; 
  // 2 strips
  if (nleft<=0)  return GetSignal(cstrip+1)/(GetSignal(cstrip+1) + GetSignal(cstrip));
  if (nright<=0) return GetSignal(cstrip)/(GetSignal(cstrip-1) + GetSignal(cstrip));
  // >2 strips
  if ( GetSignal(cstrip+1)>GetSignal(cstrip-1) ) return GetSignal(cstrip+1)/(GetSignal(cstrip+1) + GetSignal(cstrip));
  else                                           return GetSignal(cstrip)/(GetSignal(cstrip-1) + GetSignal(cstrip));
  return -1.;
}

float TrRawClusterR::GetEtaAddress(float thseed, float thneig) {
  int cstrip = GetSeedIndex(thseed);
  if (cstrip<0) return -1.;
  int address = GetSeedAddress(thseed);
  int nleft   = GetLeftLength(thseed,thneig);
  int nright  = GetRightLength(thseed,thneig);
  // 1 strip
  if ( (nleft<=0)&&(nright<=0) ) return (float)address; 
  // 2 strips
  if (nleft<=0)  return ( (address+1.)*GetSignal(cstrip+1) + address*GetSignal(cstrip) )/( GetSignal(cstrip+1) + GetSignal(cstrip) );
  if (nright<=0) return ( (address-1.)*GetSignal(cstrip-1) + address*GetSignal(cstrip) )/( GetSignal(cstrip-1) + GetSignal(cstrip) );
  // >2 strips
  if ( GetSignal(cstrip+1)>GetSignal(cstrip-1) ) 
    return ( (address+1.)*GetSignal(cstrip+1) + address*GetSignal(cstrip) )/( GetSignal(cstrip+1) + GetSignal(cstrip) );
  else 
    return ( (address-1.)*GetSignal(cstrip-1) + address*GetSignal(cstrip) )/( GetSignal(cstrip-1) + GetSignal(cstrip) );
  return -1.;
}

integer TrRawClusterR::lvl3format(int16 * adc, integer nmax,int lvl3dcard_par  ,integer matchedonly){
	// PZ FIXME level 3 format is not documented return an array of zeros
	for (int ii=0;ii<nmax;ii++)
		adc[ii]=0;
	return nmax;
/*
 //
  // convert my stupid format to lvl3 one for shuttle flight (mb also stupid)(vc)
  //
  int16 pos =0;
  if (
      nmax-pos < 2+GetNelem() || GetNelem() > 63 || GetNelem()==0 
      ||
      //PZ FIXME      (GetSide()==0 && matchedonly && !checkstatus(MATCHED))
      (GetSide()==0 && matchedonly )
      ) return pos;
  
  adc[pos+1]=1000*GetAddress()+GetHwId();
  //if(id.getside())cout <<" haddr "<<id.gethaddr()<<" "<<id.gettdr()<<" "<<id.getcrate()<<" "<<id.getdrp()<<endl;
  integer imax=0;
  geant rmax=-1000000;
  int16 sn;
  for (int i=0;i<GetNelem();i++){
    
    if(GetSigma(i) && GetSN(i) > rmax ){
      rmax=GetSN(i);
      sn=(int16)(rmax+0.5);
      if(sn>63)sn=63;
      if(sn<0)sn=0;
      imax=i;
    }
    adc[pos+i+2]=int16(GetSignal(i));
  }
  //  if(id.getside()==1)cout <<"sn "<<sn<<endl;

  if(lvl3dcard_par>0)
    adc[pos]=(GetNelem()-1) | (int16u(GetSeedSN()*8)<<6);
  
  else if(lvl3dcard_par<0)
    adc[pos]=(GetNelem()-1) | (  int16u(GetSeedSN()) <<6);
  
  else  
    adc[pos]=(GetNelem()-1) | (imax<<6);
  
  pos+=2+GetNelem();
 return pos;
 */
}




