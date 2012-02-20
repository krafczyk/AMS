#include "TrCluster.h"


ClassImp(TrClusterR);


TrCalDB* TrClusterR::_trcaldb = NULL;
TrParDB* TrClusterR::_trpardb = NULL;


int   TrClusterR::DefaultCorrOpt       = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss;
int   TrClusterR::DefaultChargeCorrOpt = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss|TrClusterR::kAngle|TrClusterR::kBeta|TrClusterR::kMIP;
int   TrClusterR::DefaultEdepCorrOpt   = TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss|TrClusterR::kMIP|TrClusterR::kMeV;


int   TrClusterR::DefaultUsedStrips = -1;     // -1: inclination dependent
float TrClusterR::TwoStripThresholdX = 0.70;  // tan(35deg)
float TrClusterR::TwoStripThresholdY = 0.36;  // tan(20deg)


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
  sout.append(Form("TkId: %5d  Side: %1d  Address: %4d  Nelem: %3d  Status: %3d  Signal: %10.3f\n",
    GetTkId(),GetSide(),GetAddress(),GetNelem(),getstatus(),GetTotSignal(TrClusterR::DefaultCorrOpt)));
  if(!opt) return;
  if (opt>1) {
    int strip = -1;
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0) 
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip)));                      
  }
  for (int ii=0; ii<GetNelem(); ii++) {
    sout.append(Form("Address: %4d  Signal: %10.5f  Sigma: %10.5f  S/N: %10.5f  Status: %3d  ",
      GetAddress(ii),GetSignal(ii,DefaultCorrOpt),GetSigma(ii),GetSN(ii,TrClusterR::kAsym),GetStatus(ii)));
    if (ii==GetSeedIndex()) sout.append("<<< SEED\n");
    else sout.append(" \n");
  }
  if (opt>1) {
    int strip = GetNelem();
    int address = GetAddress(strip);
    if ( (GetSide()==0)&&(address>1023) ) address = -1;
    if ( (GetSide()==1)&&(address> 639) ) address = -1;
    if (address>0)
      sout.append(Form("Address: %4d                      Sigma: %10.5f                   Status: %3d\n",
        address,GetSigma(strip),GetStatus(strip))); 
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


float TrClusterR::GetTotSignal(int opt, float beta) {
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
  if (kGain&opt)  sum = sum*GetTrParDB()->FindPar_TkId(GetTkId())->GetGain(GetSide()); 
  if (kLoss&opt)  sum = sum*GetTrParDB()->GetChargeLoss(GetSide(),GetCofG(DefaultUsedStrips,opt),GetImpactAngle());
  if (kPN&opt)    sum = ConvertToPSideScale(sum); 
  if (kMIP&opt)   sum = GetNumberOfMIPs(sum);
  if (kAngle&opt) sum = sum*GetCosTheta();
  if (kBeta&opt)  sum /= BetaCorrection(beta);
  if (kMeV&opt)   sum *= 0.081; // 81 keV per MIP 
  return sum;
}


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float n_to_p_pars[7] = {0.99925,0.00125804,-0.000345359,4.77114e-06,-2.04879e-08,-6.26803e-12,1.71962e-13};
float TrClusterR::ConvertToPSideScale(float adc) {
  if (GetSide()==1) return adc; // already p-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<7; ipar++) corr += pow(x,ipar)*n_to_p_pars[ipar];
  double sqrt_p = corr*x;
  return sqrt_p*sqrt_p;
}


// function derived from truncated mean X/Y comparison of ISS data B538 (Oct 2011) 
static float p_to_n_pars[10] = {2.00371,-0.36054,0.0536171,-0.00417238,0.000186663,-4.94765e-06,7.89718e-08,-7.46039e-10,3.84518e-12,-8.33832e-15};
float TrClusterR::ConvertToNSideScale(float adc) {
  if (GetSide()==0) return adc; // already n-side
  double x = sqrt(adc);
  double corr = 0;
  for (int ipar=0; ipar<10; ipar++) corr += pow(x,ipar)*p_to_n_pars[ipar];
  double sqrt_n = corr*x;
  return sqrt_n*sqrt_n;
}


/* 
  Beta Correction Parameters
  - KSC 2010, muons + proton signal   
  - Plane-by-plane vs beta TOF normalized
  - Three values for 1st plane, anyone of the inner, 9th plane
  - Physical folding
*/
static float A_BetaCorr[3]  = { 1.14505, 0.73906, -0.39480};
static float B_BetaCorr[3]  = { 0.67118, 0.05288, -1.47569};
static float b0_BetaCorr[3] = { 0.85387, 0.89147,  0.93449};
float TrClusterR::BetaCorrection(float beta) {
  /*
     - Maximum Probability Energy Loss:
       MPEL(300 um of Si) = 53.6614 eV / beta^2 * { 12.1489 - 2 log(beta) - beta^2 - 0.1492 * 
                            * [max(0,2.8716-log(beta gamma)/log(10))]^3.2546}, beta > 0.20
     - Simplified fitting function:
       f(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + C
                 beta>beta0, k (the TOF beta "saturates")     
       Continuity imposed on beta0 (no derivative continuity)
       C = k - A/beta0^2 - B*log(beta0)/beta0^2
     - Beta Correction:
       g(beta) = beta<beta0, A/beta^2 + B*log(beta)/beta^2 + 1 - A/beta0^2 - B*log(beta0)/beta0^2
                 beta>beta0, 1
     - Physical folding:
       use abs(beta) instead of beta 
       use the upper plane coefficient for the lower one and viceversa in case of negative beta
  */
  int jlayer = GetLayerJ();
  int index  = 0;        // 0 for the layer over TRD
  if (jlayer>1) index++; // 1 for any inner tracker layer 
  if (jlayer>8) index++; // 2 for the layer over ECAL
  if ( (jlayer<1)||(jlayer>9) ) {
    printf("TrClusterR::BetaCorrection-W invalid layer index number (%d), returning 1.\n",index);
    return 1.;
  }
  if (beta<0) index = 2 - index; // physical folding
  beta = fabs(beta);
  if (beta>=b0_BetaCorr[index]) return 1.; // beta "saturation" region
  return A_BetaCorr[index]/pow(beta,2) +
         B_BetaCorr[index]*log(beta)/pow(beta,2) +
         1 -
         A_BetaCorr[index]/pow(b0_BetaCorr[index],2) -
         B_BetaCorr[index]*log(b0_BetaCorr[index])/pow(b0_BetaCorr[index],2);
}


/* 
  MIP Correction Parameters
  - Extracted by preliminar charge reconstruction ISS data (Nov 2011)
  - No p-strip correction
  - Forced o pass through 0,0
  - A point at charge 100 is added extrapolating linerly from the two last points (for around Iron stuff)
*/
static Int_t    npoints_x_iss11 = 13;
static Double_t sqrtmip_x_iss11[13] = {0,   1,    2,    3,    4,    5,    6,    7,    8,   10,   12,   26,   100};
static Double_t sqrtadc_x_iss11[13] = {0,5.77,11.85,17.63,26.35,33.27,40.04,46.75,52.89,63.82,72.94,121.4,377.55};
static Int_t    npoints_y_iss11 = 15;
static Double_t sqrtmip_y_iss11[15] = {0,   1,    2,    3,    4,    5,    6,    7,    8,   10,   12,   14,   16,   26,   100};
static Double_t sqrtadc_y_iss11[15] = {0,5.83,11.86,17.58,22.00,25.38,27.72,29.43,31.05,34.29,37.96,41.74,45.65,66.06,217.10};
TSpline3* TrClusterR::sqrtadc_to_sqrtmip_spline[2] = {0,0};

float TrClusterR::GetNumberOfMIPs(float adc) {
  // initialize if needed
  if (!sqrtadc_to_sqrtmip_spline[0]) sqrtadc_to_sqrtmip_spline[0] = new TSpline3("sqrtadc_to_sqrtmip_x",sqrtadc_x_iss11,sqrtmip_x_iss11,npoints_x_iss11);
  if (!sqrtadc_to_sqrtmip_spline[1]) sqrtadc_to_sqrtmip_spline[1] = new TSpline3("sqrtadc_to_sqrtmip_y",sqrtadc_y_iss11,sqrtmip_y_iss11,npoints_y_iss11);
  int   iside = GetSide();
  float sqrtadc = (adc>0) ? sqrt(adc) : 0;
  float sqrtmip = sqrtadc_to_sqrtmip_spline[iside]->Eval(sqrtadc); 
  return pow(sqrtmip,2);
}


/* 
  MIP Correction Parameters (DEPRECATED)
  - These parameters are extracted from TB2003.
  - Straight tracks 
  - On readout strip 
  - No p-strip correction
*/
static float adc_vs_z_tb03[2][12] = {
  {  40.50, 167.92, 387.77, 713.61,1124.73,1615.55,2166.01,2734.12,3257.81,3671.87,4021.40,4290.52}, // n-side
  {  31.50, 106.58, 214.47, 304.94, 368.64, 413.37, 472.73, 575.47, 702.39, 854.23,1039.92,1233.86}  // p-side
  // {  31.50, 106.58, 209.81, 335.58, 525.61, 747.78, 977.83,1299.27,1609.36,1919.63,2220.36,2533.95}  // p-side corr
};

float TrClusterR::GetNumberOfMIPs_TB03(float adc) {
  /*
    These parameters are extracted from TB2003.
    - straight tracks 
    - readout strip 
    - no p-strip correction
  */
  int iside = GetSide();
  double x = adc;
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
    if (GetSide() == 0 && fabs(GetDxDz()) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(GetDyDz()) > TwoStripThresholdY) nstrips = 3;
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
    if (GetSide() == 0 && fabs(GetDxDz()) > TwoStripThresholdX) nstrips = 3;
    if (GetSide() == 1 && fabs(GetDyDz()) > TwoStripThresholdY) nstrips = 3;
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


float TrClusterR::GetClusterSN(int opt) {
  if (GetNelem()<1) return -1;
  float sum = 0;
  float sigma = 0;
  for (int ii=0; ii<GetNelem(); ii++) {
    sum += GetSignal(ii,opt);
    // if problems this sum will blow-up (default sigma is -9999)
    sigma += pow(GetSigma(ii),2); 
  }
  return sum/sqrt(sigma);
}
