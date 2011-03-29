/// $Id: TrCluster.C,v 1.20 2011/03/29 15:48:45 pzuccon Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCluster.C
///\brief Source file of AMSTrCluster class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/02/18 AO  New data format 
///\date  2008/02/19 AO  Signal corrections
///\date  2008/02/26 AO  Eta based local coordinate (see TkCoo.h)
///\date  2008/03/01 AO  Added member _seedind
///\data  2008/03/06 AO  Changing some data members and methods
///\date  2008/03/06 AO  Changing some data members and methods
///\date  2008/03/31 AO  Eta and CofG methods changing
///\date  2008/04/11 AO  XEta and XCofG coordinate based on TkCoo
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date: 2011/03/29 15:48:45 $
///
/// $Revision: 1.20 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrCluster.h"

ClassImp(TrClusterR);

TrCalDB* TrClusterR::_trcaldb = NULL;
TrParDB* TrClusterR::_trpardb = NULL;

int      TrClusterR::DefaultCorrOpt = (TrClusterR::kAngle|TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss);
int      TrClusterR::DefaultUsedStrips = -1;     // -1: inclination dependent
float    TrClusterR::TwoStripThresholdX = 0.70;  // tan(35deg)
float    TrClusterR::TwoStripThresholdY = 0.36;  // tan(20deg)


TrClusterR::TrClusterR(void) {
  Clear();
}


TrClusterR::TrClusterR(const TrClusterR &orig):TrElem(orig) {
  _tkid    = orig._tkid;
  _address = orig._address;
  _nelem   = orig._nelem;
  _seedind = orig._seedind;
  for (int i = 0; i<_nelem; i++) _signal.push_back(orig._signal.at(i));
  Status  = orig.Status;
  _mult    = orig._mult;
  _dxdz    = orig._dxdz;
  _dydz    = orig._dydz;
  //  _coord   = orig._coord;
  //  _gcoord  = orig._gcoord;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int seedind, unsigned int status) {
  Clear();
  _tkid    = tkid;
  _address = add;
  _seedind = seedind;
  Status  = status;
}


TrClusterR::TrClusterR(int tkid, int side, int add, int nelem, int seedind, 
                           float* adc, unsigned int status)  {
  Clear();
  _tkid    =  tkid;
  _address =  add;
  if ( ((add+nelem-1)>=1024)&&(!TkDBc::Head->FindTkId(tkid)->IsK7()) ) {
    _nelem=1024-add;
    cerr<<"TrClusterR::TrClusterR -W- You are tring to create a Cluster with address "<< add<<" and length "<<nelem<<endl;
    cerr<<"                           Cluster has been truncated to the physical limit"<<endl;
  }
  else
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  Status  = status;
  _mult    = 0;
  // BuilCoordinates();
}


TrClusterR::~TrClusterR() {
  Clear();
}


int TrClusterR::GetMultiplicity()  {
  if (_mult==0) _mult = TkCoo::GetMaxMult(GetTkId(),GetAddress())+1;
  return _mult;
}


float TrClusterR::GetNoise(int ii) {
  if (_trcaldb==0) {
    printf("TrClusterRs::GetStatus Error, no _trcaldb specified.\n");
    return -9999.; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) { printf ("TrClusterR::GetNoise, WARNING calibration not found!!\n"); return -9999; }
  int address = _address+ii;
  return (float) ladcal->GetSigma(address);
}


short TrClusterR::GetStatus(int ii) {
  if (_trcaldb==0) {
    printf("TrClusterRs::GetStatus Error, no _trcaldb specified.\n");
    return -1; 
  }
  int tkid = GetTkId();
  TrLadCal* ladcal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!ladcal) {printf ("TrClusterR::GetNoise, WARNING calibration not found!!\n"); return -9999;}
  int address = _address+ii;
  return (short) ladcal->GetStatus(address);
}


void TrClusterR::Clear() {
  _tkid    =   0;
  _address =  -1;
  _nelem   =   0;
  _seedind =   0;
  _signal.clear();
  Status   =   0;
  _mult    =   0;
  _dxdz    =   0;  // vertical inclination by default 
  _dydz    =   0;  // vertical inclination by default 
  //  _coord.clear();
  //  _gcoord.clear();
}

void TrClusterR::push_back(float adc) {
  _signal.push_back(adc);
  _nelem = (int) _signal.size();
}


float TrClusterR::GetGCoord(int imult)  {
  float lcoo = GetXCofG(DefaultUsedStrips,imult);
  if (GetSide() == 0) return TkCoo::GetGlobalA(_tkid, lcoo, TkDBc::Head->_ssize_active[1]/2).x();
  else                return TkCoo::GetGlobalA(_tkid, TkDBc::Head->_ssize_active[0]/2, lcoo).y();
}


void TrClusterR::Print(int opt) { 
  _PrepareOutput(opt);
  cout << sout;
}


void TrClusterR::_PrepareOutput(int opt){
  sout.clear();
  sout.append(Form("TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  Status: %d Signal:  %f\n",
		   GetTkId(),GetSide(),GetAddress(),GetNelem(),getstatus(),GetTotSignal(DefaultCorrOpt)));
  if(!opt) return;
  for (int ii=0; ii<GetNelem(); ii++) {
    sout.append(Form("Address: %4d  Signal: %10.5f  Sigma: %10.5f  S/N: %10.5f  Status: %3d  ",
		      GetAddress(ii),GetSignal(ii,DefaultCorrOpt),GetSigma(ii),GetSN(ii,DefaultCorrOpt),GetStatus(ii)));
    if (ii==GetSeedIndex()) sout.append("<<< SEED\n");
    else sout.append(" \n");
  }
}


char* TrClusterR::Info(int iRef){
  string aa;
  aa.append(Form("TrCluster #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::ostream &TrClusterR::putout(std::ostream &ostr) {
  _PrepareOutput(1);
  return ostr << sout  << std::endl;
}


float TrClusterR::GetSignal(int ii, int opt) {
  float signal = _signal.at(ii);
  if ( (kAsym&opt)&&(ii>0) ) signal = signal - _signal.at(ii-1)*GetTrParDB()->GetAsymmetry(GetSide());
  return signal;
}


int TrClusterR::GetAddress(int ii) {
  int address = GetAddress() + ii;
  int side    = GetSide();
  // cyclical definition, in order to avoid errors and manage the K7 clusters
  if ( (side==0)&&(address>1023) ) address = address - 384; //  640 + (address - 1024);
  if ( (side==0)&&(address< 640) ) address = address + 384; // 1024 - (640  - address);
  if ( (side==1)&&(address> 639) ) address = address - 640; //    0 + (address -  640);
  if ( (side==1)&&(address<   0) ) address = address + 640; //  640 - (   0 - address);
  return address;
}


int TrClusterR::GetSeedIndex(int opt) {
  if (opt==kNoCorr) return _seedind;  
  float maxadc  = -9999.;
  int   seedind = -1;
  for (int ii=0; ii<GetNelem(); ii++) { 
    if (GetSignal(ii,opt)>maxadc) { 
      maxadc  = GetSignal(ii,opt); 
      seedind = ii; 
    } 
  }
  return seedind;
}


float TrClusterR::GetTotSignal(int opt) {
  float sum = 0.;
  if (!(kVAGain&opt)) {
    for (int ii=0; ii<GetNelem(); ii++) {
      sum += GetSignal(ii,opt);
    }
  }
  else {
    for (int ii=0; ii<GetNelem(); ii++) {
       int iva = int(GetAddress(ii)/64);  
       sum += GetSignal(ii,opt)*GetTrParDB()->FindPar_TkId(GetTkId())->GetVAGain(iva); 
    }
  }
  if (kAngle&opt)  sum = sum*sqrt(1./(1.+_dxdz*_dxdz+_dydz*_dydz));
  if (kGain&opt)   sum = sum*GetTrParDB()->FindPar_TkId(GetTkId())->GetGain(GetSide()); 
  if (kLoss&opt)   sum = sum*GetTrParDB()->GetChargeLoss(GetSide(),GetCofG(DefaultUsedStrips,opt),GetImpactAngle());
  if ((kPN&opt)&&(GetSide()==0)) sum = sum*GetTrParDB()->GetPNGain();
  return sum;
}


float TrClusterR::GetNumberOfMIPs(int opt) {
  // init
  double adc_vs_z_tb03[3][12] = {
    {  40.50, 167.92, 387.77, 713.61,1124.73,1615.55,2166.01,2734.12,3257.81,3671.87,4021.40,4290.52}, // n-side
    {  31.50, 106.58, 214.47, 304.94, 368.64, 413.37, 472.73, 575.47, 702.39, 854.23,1039.92,1233.86}, // p-side
    {  31.50, 106.58, 209.81, 335.58, 525.61, 747.78, 977.83,1299.27,1609.36,1919.63,2220.36,2533.95}  // p-side corr
  };
  double x = GetTotSignal(opt);
  double y = 0.;
  int iside = GetSide();
  if ((kPStrip&opt)&&(GetSide()==0)) iside = 2;
  // TMP: to be improved 
  // interpolation 
  for (int i=0; i<11; i++) {
    if ((x>=adc_vs_z_tb03[iside][i])&&(x<adc_vs_z_tb03[iside][i+1])) {
      double x0 = adc_vs_z_tb03[iside][i];
      double x1 = adc_vs_z_tb03[iside][i+1];    
      double y0 = i*i;
      double y1 = (i+1)*(i+1);
      y = ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0); 
    }
  }  
  // lower extrapolation 
  if (x<adc_vs_z_tb03[iside][0]) {
    double x0 = adc_vs_z_tb03[iside][0];
    double x1 = adc_vs_z_tb03[iside][1];
    double y0 = 1*1;
    double y1 = 2*2;
    y = ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // upper extrapolation
  if (x>=adc_vs_z_tb03[iside][11]) {
    double x0 = adc_vs_z_tb03[iside][10];
    double x1 = adc_vs_z_tb03[iside][11];
    double y0 = 11*11;
    double y1 = 12*12;
    y = ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  return y; 
}


void TrClusterR::GetBounds(int &leftindex, int &rightindex, int nstrips, int opt) {
  // loop on strips (adding asymmetrically the strips)
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt);
  int nright     = GetRightLength(opt);
  int nleftused  = 0; 
  int nrightused = 0; 
  for (int ii=0; ii<nstrips-1; ii++) {
    if      ( (nleft==nleftused)&&(nright==nrightused) ) break;  
    else if (nleft  == nleftused)  nrightused++; 
    else if (nright == nrightused) nleftused++; 
    else    ( GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt) ) ? nleftused++ : nrightused++; 
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


float TrClusterR::GetCofG(int nstrips, int opt) {
  if (nstrips==-1) {
    nstrips = 2;
    if (GetSide() == 0 && fabs(_dxdz) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(_dydz) > TwoStripThresholdY) nstrips = 3;
  }
  if (nstrips==1) return 0.;
  int leftindex; 
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;  
  GetBounds(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    numerator   += weight*(index-cstrip);
    denominator += weight;
  }
  float CofG = numerator/denominator;
  return CofG;
}


float TrClusterR::GetXCofG(int nstrips, int imult, const int opt) {
  if (nstrips==-1) {
    nstrips = 2;
    if ((GetSide()==0)&&(fabs(_dxdz)>TwoStripThresholdX)) nstrips = 3;
    if ((GetSide()==1)&&(fabs(_dydz)>TwoStripThresholdY)) nstrips = 3;
  } 
  if (nstrips==1) return 0.;
  int leftindex;
  int rightindex;
  float numerator   = 0.;
  float denominator = 0.;
  GetBounds(leftindex,rightindex,nstrips,opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    numerator   += weight*GetX(index,imult);
    denominator += weight;
  }
  float CofG = numerator/denominator;
  //  if (fabs(GetXCofG_old(nstrips,imult,opt)-CofG)>0.0001) printf("CofG: %10.6f %10.6f %10.6f\n",GetXCofG_old(nstrips,imult,opt),CofG,GetXCofG_old(nstrips,imult,opt)-CofG); 
  return CofG; 
}


float TrClusterR::GetEta(int opt) {
  if (GetNelem()<1) return -1;
  int cstrip = GetSeedIndex(opt);
  int nleft  = GetLeftLength(opt);
  int nright = GetRightLength(opt);
  // 1 strip
  if ( (nleft==0)&&(nright==0) ) return 0.; 
  // 2 strips
  if (nleft==0)  return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  if (nright==0) return GetSignal(cstrip,  opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  // >2 strips
  if ( GetSignal(cstrip+1,opt)>GetSignal(cstrip-1,opt) ) {
    return GetSignal(cstrip+1,opt)/(GetSignal(cstrip+1,opt) + GetSignal(cstrip,opt));
  }
  else {                                           
    return GetSignal(cstrip  ,opt)/(GetSignal(cstrip-1,opt) + GetSignal(cstrip,opt));
  } 
  return -1.;
}


float TrClusterR::GetDHT(int nstrips, int opt) {
  if (nstrips==1) return 0.;
  int leftindex; 
  int rightindex;
  GetBounds(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  return (leftindex + rightindex)/2. - cstrip;
}


float TrClusterR::GetAHT(int nstrips, int opt) {
  if (nstrips==1) return 0.;
  if (nstrips==2) return GetDHT(2,opt);
  int leftindex; 
  int rightindex;
  GetBounds(leftindex,rightindex,nstrips,opt);
  if (fabs(rightindex-leftindex+1)<3) return GetDHT(nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  float mean = 0.;
  for (int index=leftindex+1; index<=rightindex-1; index++) mean += GetSignal(index,opt);
  mean /= (leftindex+rightindex+1-2);
  return GetDHT(nstrips,opt) + (GetSignal(rightindex,opt) - GetSignal(leftindex,opt))/mean/2.;
}
