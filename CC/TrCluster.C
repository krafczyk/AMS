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
    cerr<<"TrClusterR::TrClusterR-W You are tring to create a Cluster with address "<< add<<" and length "<<nelem<<endl;
    cerr<<"                         Cluster has been truncated to the physical limit"<<endl;
  }
  else
  _nelem   =  nelem;
  _seedind =  seedind;
  _signal.reserve(_nelem);
  for (int i = 0; i<_nelem; i++) _signal.push_back(adc[i]);
  Status  = status;
  _mult    = 0;
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
  if (kAngle&opt) sum = sum*sqrt(1./(1.+_dxdz*_dxdz+_dydz*_dydz));
  if (kGain&opt)  sum = sum*GetTrParDB()->FindPar_TkId(GetTkId())->GetGain(GetSide()); 
  if (kLoss&opt)  sum = sum*GetTrParDB()->GetChargeLoss(GetSide(),GetCofG(DefaultUsedStrips,opt),GetImpactAngle());
  if (kMIP&opt)   sum = GetNumberOfMIPs(GetSide(),sum);
  if ((kPN&opt)&&(GetSide()==0)) sum = sum*GetTrParDB()->GetPNGain();
  return sum;
}


float TrClusterR::GetNumberOfMIPs(int iside, float adc) {
  /*
    These parameters are extracted from TB2003.
    - straight tracks 
    - readout strip 
    - no p-strip correction
  */
  double x = adc;
  double adc_vs_z_tb03[2][12] = {
    {  40.50, 167.92, 387.77, 713.61,1124.73,1615.55,2166.01,2734.12,3257.81,3671.87,4021.40,4290.52}, // n-side
    {  31.50, 106.58, 214.47, 304.94, 368.64, 413.37, 472.73, 575.47, 702.39, 854.23,1039.92,1233.86}  // p-side
    // {  31.50, 106.58, 209.81, 335.58, 525.61, 747.78, 977.83,1299.27,1609.36,1919.63,2220.36,2533.95}  // p-side corr
  };
  // lower extrapolation 
  if (adc<adc_vs_z_tb03[iside][0]) {
    double x0 = adc_vs_z_tb03[iside][0];
    double x1 = adc_vs_z_tb03[iside][1];
    double y0 = 1*1;
    double y1 = 2*2;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // upper extrapolation
  if (adc>=adc_vs_z_tb03[iside][11]) {
    double x0 = adc_vs_z_tb03[iside][10];
    double x1 = adc_vs_z_tb03[iside][11];
    double y0 = 11*11;
    double y1 = 12*12;
    return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0);
  }
  // inside range
  for (int i=0; i<11; i++) {
    if ((adc>=adc_vs_z_tb03[iside][i])&&(adc<adc_vs_z_tb03[iside][i+1])) {
      double x0 = adc_vs_z_tb03[iside][i];
      double x1 = adc_vs_z_tb03[iside][i+1];    
      double y0 = (i+1)*(i+1);
      double y1 = (i+2)*(i+2);
      return ( (x-x0)*y1 + (x1-x)*y0 ) / (x1-x0); 
    }
  }  
  return -100; 
}


void TrClusterR::GetBoundsSymm(int &leftindex, int &rightindex, int nstrips, int opt) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_    
   *  __|_|_|_|_|_|_|_|_|__                   
   *     X X 6 4 3 1 2 5
   *     X X 6 5 2 1 3 4 (kFlip)          
   *         
   * Trying to contruct symmetrycally the bounds given the number of strips
   * - n = min(int((nstrips-1)/2),min(nleft,nright)) to be used automatically on left an right (symmetry)
   * - add one more strip if nstrips is odd or if the nleft and nright are dramatically different 
   */
  int cstrip     = GetSeedIndex(opt);
  int nleft      = GetLeftLength(opt);
  int nright     = GetRightLength(opt);
  int n          = min(int((nstrips-1)/2),min(nleft,nright));
  int nleftused  = min(n,nleft);
  int nrightused = min(n,nright);
  if ((nstrips-nleftused-nrightused-1)>0) { // add one more strip
    if      ( (nleft==nleftused)&&(nright==nrightused) ) {}
    else if (nleft  == nleftused)  nrightused++;
    else if (nright == nrightused) nleftused++;
    else    ( ( (GetSignal(cstrip-nleftused-1,opt)>GetSignal(cstrip+nrightused+1,opt))&&(!(opt&kFlip)) ) ||
              ( (GetSignal(cstrip-nleftused-1,opt)<GetSignal(cstrip+nrightused+1,opt))&&(  opt&kFlip ) )  ) ? nleftused++ : nrightused++;
  }
  leftindex  = cstrip - nleftused;
  rightindex = cstrip + nrightused;
}


void TrClusterR::GetBoundsAsym(int &leftindex, int &rightindex, int nstrips, int opt) {
  /*               _        
   *              | |_      
   *           _ _| | |     
   *     _ _ _| | | | |_ _   
   *  __|_|_|_|_|_|_|_|_|_|__                   
   *     8 7 5 4 3 1 2 5 6                      
   *
   * Trying to construct asymmetrycally the bounds given the number of strips
   * - add strips in hierarchical order
   */
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


float TrClusterR::GetXCofG(int nstrips, int imult, int opt) {
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
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  float seedposition = GetX(cstrip,imult);
  for (int index=leftindex; index<=rightindex; index++) {
    // signal weight
    float weight = GetSignal(index,opt);
    if (opt&kCoupl) { // trying to eliminate coupling effect
      if (index-1>0)          weight -= 0.04*GetSignal(index-1,opt);
      if (index+1<GetNelem()) weight -= 0.04*GetSignal(index+1,opt);
    }
    // position
    float position = GetX(index,imult);
    int   mask = 1;
    if (IsK7()!=0) { 
      // special computation for K7 (cyclic clusters)
      // - take seed strip as the strip with a fixed multiplicity
      int address = GetAddress(index); 
      int maxmult = TkCoo::GetMaxMult(GetTkId(),address);
      if ( (index<cstrip)&&(position>seedposition) ) {
        position = (imult>0) ? GetX(index,imult-1) : 0; 
        mask     = (imult>0) ? 1 : 0;
      }
      else if ( (index>cstrip)&&(position<seedposition) ) {  
        position = (imult<(maxmult-1)) ? GetX(index,imult+1) : 0;
        mask     = (imult<(maxmult-1)) ? 1 : 0;
      }
    }
    // weighted average
    numerator   += weight*position*mask;
    denominator += weight*mask;
    // cout << "numerator: " << numerator << "  denominator: " << denominator << endl;
  } 
  float CofG = numerator/denominator;
  /*
  if (fabs(CofG-GetXCofG_old(nstrips,imult,opt))>0.001) {
    cout << "THIS IS A WARNING NOT AN ERROR " << endl;
    Print(10);
    cout << "nstrips: " << nstrips << "   imult: " << imult << endl; 
    cout << "CofGX: " << CofG << "   CofGX_old: " << GetXCofG_old(nstrips,imult,opt) << endl;
    for (int index=leftindex; index<=rightindex; index++) {
      cout << "index: " << index << "  address: " << GetAddress(index) << "  mult-1: " << GetX(index,imult-1) 
           << "  mult: " << GetX(index,imult) << "   mult+1: " << GetX(index,imult+1) 
           << "  maxmult: " << TkCoo::GetMaxMult(GetTkId(),GetAddress(index)) << endl;
    }
    cout << endl;
  }
  */
  return CofG; 
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
  // GetBoundsAsym(leftindex,rightindex,nstrips,opt);
  GetBoundsSymm(leftindex,rightindex,nstrips,opt);
  int cstrip = GetSeedIndex(opt);
  for (int index=leftindex; index<=rightindex; index++) {
    float weight = GetSignal(index,opt);
    numerator   += weight*(index-cstrip);
    denominator += weight;
  }
  float CofG = numerator/denominator;
  return CofG;
}


float TrClusterR::GetEta(int opt) {
  /*! Eta = center of gravity with the two higher strips = Q_{R} / ( Q_{L} + Q_{R} )
   *      _                                    _ 
   *    l|c|r          c*0 + r*1    r        l|c|r            l*0 + c*1    c
   *     | |_    eta = --------- = ---       _| |       eta = --------- = ---
   *    _| | |           c + r     c+r      | | |_              l + c     l+c
   * __|_|_|_|__                          __|_|_|_|__
   *      0 1                                0 1 
   *  Eta is 1 for particle near to the right strip,
   *  while is approx 0 when is near to the left strip (old definition) */
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


