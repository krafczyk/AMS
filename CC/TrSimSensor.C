#include "TrSimSensor.h"
#include "tkdcards.h"


TrSimSensor& TrSimSensor::operator=(const TrSimSensor& that) {
  { if (this!=&that) Copy(that); return *this; }
}


void TrSimSensor::Init() {
  _sensor_type = -1;
  _nimplants = 0;
  _nreadout = 0;
  _implant_pitch = 0;
  _CD.clear();
  _CU.clear();
  _CL.clear();
  _CR.clear();
  _diff_type = 0;
  _diff_radius = 0;
  _Cint = 0;
  _Cbk = 0;
  _Cdec = 0;
  _mcfun = 0;
  _refun = 0;
}


void TrSimSensor::Clear() {
  _sensor_type = -1; 
  _nimplants = 0;
  _nreadout = 0;
  _implant_pitch = 0;
  _CD.clear();
  _CU.clear();
  _CL.clear();
  _CR.clear();
  _diff_type = 0;
  _diff_radius = 0;
  _Cint = 0;
  _Cbk = 0;
  _Cdec = 0;
  if (_mcfun!=0) delete _mcfun;
  _mcfun = 0;
  if (_refun!=0) delete _refun;
  _refun = 0;
}


void TrSimSensor::Copy(const TrSimSensor& that) {
  _sensor_type = that._sensor_type;
  _nimplants = that._nimplants;
  _nreadout = that._nreadout;
  _implant_pitch = that._implant_pitch;
  _CD = that._CD;
  _CU = that._CU;
  _CL = that._CL;
  _CR = that._CR;
  _diff_type = that._diff_type;
  _diff_radius = that._diff_radius;
  _Cint = that._Cint;
  _Cbk = that._Cbk;
  _Cdec = that._Cdec;
  if (that._mcfun!=0) _mcfun = new TF1(*that._mcfun);
  else _mcfun = 0;
  if (that._refun!=0) _refun = new TF1(*that._refun);
  else _refun = 0; 
}


void TrSimSensor::SetSensorType(int type) {
  Clear();
  _sensor_type = type;
  SetDefaults();
  CalculateEquivalentCapacitances();  
}


void TrSimSensor::SetDefaults() {
  // Sensor parameters
  switch (GetSensorType()) {
    case 0: // S
      _Cint = TRMCFFKEY.TrSim2010_Cint[1]; // pF
      _Cbk  = TRMCFFKEY.TrSim2010_Cbk[1];  // pF
      _Cdec = TRMCFFKEY.TrSim2010_Cdec[1]; // pF
      _diff_type = TRMCFFKEY.TrSim2010_DiffType[1];
      _diff_radius = TRMCFFKEY.TrSim2010_DiffRadius[1]; // um
      _nimplants = 2567;
      _nreadout = 640;
      _implant_pitch = 27.5;
      break; 
    case 1: // K5
      _Cint = TRMCFFKEY.TrSim2010_Cint[0]; // pF
      _Cbk  = TRMCFFKEY.TrSim2010_Cbk[0];  // pF
      _Cdec = TRMCFFKEY.TrSim2010_Cdec[0]; // pF
      _diff_type = TRMCFFKEY.TrSim2010_DiffType[0];
      _diff_radius = TRMCFFKEY.TrSim2010_DiffRadius[0]; // um
      _nimplants = 384;
      _nreadout = 192;
      _implant_pitch = 104;
      break;
    case 2: // K7
      _Cint = TRMCFFKEY.TrSim2010_Cint[0]; // pF
      _Cbk  = TRMCFFKEY.TrSim2010_Cbk[0];  // pF
      _Cdec = TRMCFFKEY.TrSim2010_Cdec[0]; // pF
      _diff_type = TRMCFFKEY.TrSim2010_DiffType[0];
      _diff_radius = TRMCFFKEY.TrSim2010_DiffRadius[0]; // um
      _nimplants = 384;
      _nreadout = 224;
      _implant_pitch = 104;
      break;
  }
  // Generalized Landau (with some scaling terms)
  char name[100];
  sprintf(name,"landau_model%1d",GetSensorType()); // diff. names needed by ROOT
  if (_mcfun==0) _mcfun = new TF1(name,LandauFun,0.,MAXADC,4); 
  _mcfun->SetNpx(MAXADC);
  sprintf(name,"langauexp_model%1d",GetSensorType()); // diff. names needed by ROOT
  if (_refun==0) _refun = new TF1(name,LanGauExpFun,0.,MAXADC,5);
  _refun->SetNpx(MAXADC); // ATTENTION: THIS COULD BE A PROBLEM (TOO SLOW IN THE INVERSION PROCESS)
  switch (GetSensorType()) {
    case 0: // S
      _mcfun->SetParameters(81.94,5.811,1,1);
      if      (TRMCFFKEY.TrSim2010_EDepType[1]==1) _refun->SetParameters(1.00,31.500,0.943,7.650, 43.380); // TB2003
      else if (TRMCFFKEY.TrSim2010_EDepType[1]==2) _refun->SetParameters(2.77,31.440,0.978,7.294,113.513); // CR2009
      else if (TRMCFFKEY.TrSim2010_EDepType[1]==3) _refun->SetParameters(2.90,29.393,0.976,5.855,108.803); // CR2010
      break;
    case 1: // K5
    case 2: // K7
      _mcfun->SetParameters(81.67,5.71,1,1);
      if      (TRMCFFKEY.TrSim2010_EDepType[0]==1) _refun->SetParameters(3.31,40.500,1.037,6.500,105.780); // TB2003
      else if (TRMCFFKEY.TrSim2010_EDepType[0]==2) _refun->SetParameters(2.62,29.584,0.975,6.297,101.405); // CR2009
      else if (TRMCFFKEY.TrSim2010_EDepType[0]==3) _refun->SetParameters(3.15,32.012,0.977,6.929,120.252); // CR2010
      break;
  }
  // MPV Normalization (horizontal scaling)
  // float scale1 = _refun->GetParameter(1)/_mcfun->GetParameter(0);
  float scale1 = _refun->GetMaximumX(0.,MAXADC)/_mcfun->GetMaximumX(0.,MAXADC);
  _mcfun->SetParameter(2,scale1);
  // Height normalization (needed for inversion)
  // float scale2 = _refun->Eval(_refun->GetParameter(1))/_mcfun->Eval(_mcfun->GetParameter(0));
  float scale2 = _refun->GetMaximum(0,MAXADC)/_mcfun->GetMaximum(0,MAXADC); 
  _mcfun->SetParameter(3,scale2);
  // Inversion computation ...
}

double TrSimSensor::GetkeVtoADC()  { 
  return _refun->GetParameter(1)/_mcfun->GetParameter(0); 
}

/*
double TrSimSensor::GetkeVtoADC(double keV)  { 
  double m = _refun->GetParameter(1)/_mcfun->GetParameter(0);
  return m*(keV - _mcfun->GetParameter(0)) + _refun->GetParameter(1);
}   
*/

double TrSimSensor::GetAdcMpvTb2003(double z) {
  double value = 0.;
  switch (GetSensorType()) {
    case 0: // S
      value = pow(-0.091538+6.63945*z,2.);
      break;
    case 1: // K5
    case 2: // K7
      value = pow(1.28205+4.34618*z,2.);
      break;
  }
  return value; 
}

bool TrSimSensor::IsReadoutStrip(int implantadd) {
  if ( (GetSensorType()<0)||(GetSensorType()>2) ) {
    printf("TrSimSensor::IsReadoutStrip-E no sensor type selected\n");
    return false;
  }
  if ( (implantadd<0)||(implantadd>=GetNImplantStrips()) ) {
    if (WARNING) printf("TrSimSensor::IsReadoutStrip-W invalid implant position (impl=%d) requested\n",implantadd);
    return false;
  }
  /* 
    Sensor Bonding Scheme
    S: 
      - 2567 implantation strips
      -  640 readout strips (642 possible positions) 
      implantation 0000 0001 0002 0003 0004 0005 0006 0007 0008 0009 0010 0011 0012 ... 2555 2556 2557 2558 2559 2560 2561 2562 2563 2564 2565 2566
      readout       000                 xxx                 001                 002           638                 xxx                 639
    K5 (one sensor):
      - 384 implantation strips
      - 192 readout strips
      implantation  000 001 002 003 004 005 006 ... 378 379 380 381 382 383
      readout         0       1       2       3     189     190         191
    K7 (one sensor):
      - 384 implantation strips
      - 224 readout strips
      implantation  000 001 002 003 004 005 006 ... 092 093 094 095 | 096 097 098 ... 286 | 287 288 289 290 291 ... 380 381 382 383
      readout         0       1   2       3   4      61  62      63 |  64      65     159 |     160     161 162     221 222     223
  */
  switch (GetSensorType()) {
    case kS: 
      return ((implantadd%4)==0) && (implantadd!=4) && (implantadd!=2560) && (implantadd<=2566);
      break;
    case kK5: 
      return ( ((implantadd%2)==0) && (implantadd<382) ) || (implantadd==383);
      break;
    case kK7:
      return ( ((implantadd%3)!=1) && ((implantadd< 96)||(implantadd> 287)) && (implantadd<=383) ) || 
             ( ((implantadd%2)==0) && ((implantadd>=96)&&(implantadd<=287)) );
      break;
  }
  return false;
}


int TrSimSensor::GetReadoutAddressFromImplantAddress(int implantadd, int nsens) {
  if ( (GetSensorType()<0)||(GetSensorType()>2) ) {
    printf("TrSimSensor::GetReadoutAddressFromImplantAddress-E no sensor type selected\n");
    return -1;
  }
  if ( (implantadd<0)||(implantadd>=GetNImplantStrips()) ) {
    if (WARNING) printf("TrSimSensor::GetImplantAddressFromReadoutAddress-W invalid implant address (add=%d) requested\n",implantadd);
    return -1;
  }
  if (!IsReadoutStrip(implantadd)) {
    if (WARNING) printf("TrSimSensor::GetImplantAddressFromReadoutAddress-W requested implant strip is not a readout strip (add=%d)\n",implantadd);
    return -1;
  }
  int readoutadd = -1;
  switch (GetSensorType()) {
    case kS:
      readoutadd = implantadd/4;
      if (readoutadd>   0) readoutadd -=   1;
      if (readoutadd>=640) readoutadd  = 639;  
      return readoutadd;
      break;
    case kK5:
      readoutadd = implantadd/2;
      if (implantadd==383) readoutadd  = 191; // even sensors
      if ((nsens%2)==1)    readoutadd += 192; // odd sensors
      return readoutadd;
      break;
    case kK7:
      readoutadd = 2*(implantadd/3) + ((implantadd%3)==2);                       // first 
      if (implantadd>286)                        readoutadd -= 32;               // third:  gap (192 - 160)
      if ( (implantadd>=96)&&(implantadd<=287) ) readoutadd = implantadd/2 + 16; // second: gap ( 64 -  48)    
      readoutadd = (readoutadd + nsens*224)%384;                                 // on nth sensor
      return readoutadd;
      break; 
  }
  return readoutadd;
}


int TrSimSensor::GetImplantAddressFromReadoutAddress(int readoutadd) {
  if ( (GetSensorType()<0)||(GetSensorType()>2) ) {
    printf("TrSimSensor::GetImplantAddressFromReadoutAddress-E no sensor type selected\n");
    return -1;
  }
  if ( (readoutadd<0)||(readoutadd>=GetNReadoutStrips()) ) {
    if (WARNING) printf("TrSimSensor::GetImplantAddressFromReadoutAddress-W invalid readout address (add=%d) requested\n",readoutadd);
    return -1;
  }
  int impladd = -1;
  switch (GetSensorType()) {
    case kS:       
      impladd = 4*readoutadd;
      if (readoutadd!=  0) impladd += 4;
      if (readoutadd==639) impladd += 4;
      break;
    case kK5: 
      impladd = 2*readoutadd;
      if (readoutadd==191) impladd = 383;       
      break;
    case kK7: 
      impladd = int(readoutadd/2)*3 + (readoutadd%2)*2;
      if ( (readoutadd>=64)&&(readoutadd<=159) ) impladd = 96 + (readoutadd-64)*2; // "regular" regime  
      else if (readoutadd>159) impladd += 48; // offset
      break;
  }
  return impladd;
}


double TrSimSensor::GetImplantAddressFromReadoutAddress(double readoutadd) {
  int lowerread = (int) floor(readoutadd);
  int upperread = (int) ceil(readoutadd);
  if ( (lowerread>=0)&&(upperread< 0) ) return GetImplantAddressFromReadoutAddress(lowerread);
  if ( (lowerread< 0)&&(upperread>=0) ) return GetImplantAddressFromReadoutAddress(upperread);
  if ( (lowerread< 0)&&(upperread< 0) ) return -1;
  // interpolation
  int lowerimpl = GetImplantAddressFromReadoutAddress(lowerread);
  int upperimpl = GetImplantAddressFromReadoutAddress(upperread); 
  // printf("input=%7.4f   low=%3d  ip=%37.4f  deltaimpl=%3d   result=%7.4f\n",readoutadd,lowerimpl,readoutadd-lowerread,upperimpl-lowerimpl,lowerimpl + (readoutadd-lowerread)*(upperimpl-lowerimpl));
  return lowerimpl + (readoutadd-lowerread)*(upperimpl-lowerimpl);
}


/* The Capacitive net (P-Side)
+
*            |                           |
*          Cd=                         Cd=
*            |                           |
*      Cl    |  Ci     Ci     Ci     Ci  |    Cr
* ...--||----+--||--+--||--+--||--+--||--+----||--...
*            |      |      |      |      |
*            |      |      |      |      |
*          Cb=    Cb=    Cb=    Cb=    Cb=
*            |      |      |      |      |
*
*/
void TrSimSensor::CalculateEquivalentCapacitances() {
  // init
  _CD.clear();
  _CU.clear();
  _CL.clear();
  _CR.clear();
  int nimplants = GetNImplantStrips(); 
  // left
  double tmp = 0.;
  for (int ii=0; ii<nimplants; ii++) {
    _CD.push_back(GetCbk());
    if (IsReadoutStrip(ii)) _CU.push_back(GetCdec());
    else                    _CU.push_back(0.);
    _CL.push_back(tmp);
    tmp = (GetCint()*(_CU.at(ii)+_CD.at(ii)+tmp))/(GetCint()+_CU.at(ii)+_CD.at(ii)+tmp);
  }
  // right
  tmp = 0.;
  for (int ii=nimplants-1; ii>=0; ii--) {
    _CR.push_back(tmp);
    tmp = (GetCint()*(_CU.at(ii)+_CD.at(ii)+tmp))/(GetCint()+_CU.at(ii)+_CD.at(ii)+tmp);
  }
  reverse(_CR.begin(),_CR.end());
}


void TrSimSensor::SeeEquivalentCapacitances() {
  for (int ii=0; ii<GetNImplantStrips(); ii++) {
    printf("%4d   Cup = %7.3f  Cdown = %7.3f  Cleft = %7.3f  Cright = %7.3f\n",
           ii,_CU.at(ii),_CD.at(ii),_CL.at(ii),_CR.at(ii));
  }
}


TrSimCluster TrSimSensor::MakeImplantCluster(double senscoo, double sensangle) {
 /* Charge sharing distribution:
  *
  *         x1      x2
  * |-------|-------|-------|-------| strips
  *            a |----| b             width
  * 
  */
  double coordimpl = senscoo*1.e4/GetImplantPitch(); // coordinate [implant pitch]
  if ( (coordimpl<0)||(coordimpl>=GetNImplantStrips()) ) {
    if (WARNING) printf("TrSimSensor::MakeImplantCluster-W sensor coordinate out of the sensor (coord=%7.4f), returning an empty cluster.\n",senscoo);
    return TrSimCluster();
  }
  // charge sharing profile calculation
  double track_proj = fabs(tan(sensangle)*SUBSTRATEWIDTH)/GetImplantPitch();       // projection of the track [interstrip pitch] 
  double diffu_proj = fabs(GetDiffusionRadius()/cos(sensangle))/GetImplantPitch(); // projection of the diffusion radius [interstrip pitch] 
  double projection = track_proj + diffu_proj;                                     // global projection of ionization [interstrip pitch] 
  double diffusion  = GetDiffusionRadius()/GetImplantPitch();                      // diffusion at 0 deg (for model 4)
  double sign       = (sensangle>0) ? -1 : 1;                                      // FIX ME: CHECK DELLA DIREZIONE 
  int    seedind    = (int) ceil(coordimpl-0.5);                                   // seed strip
  // printf("TrSimSensor::MakeImplantCluster-V senscoo=%7.3f sensangle=%7.3f coorind=%7.2f seedind=%4d track_proj=%7.3f diffu_proj=%7.3f projection=%7.3f diffusion=%7.3f\n",
  //       senscoo,sensangle,coordimpl,seedind,track_proj*GetImplantPitch(),diffu_proj*GetImplantPitch(),projection*GetImplantPitch(),diffusion*GetImplantPitch());
  // tmp cluster vars
  double check = 0.;
  vector<double> acluster;
  acluster.clear();
  int address = seedind;                                   
  // create on the right
  for (int istrip=seedind; istrip<GetNImplantStrips(); istrip++) {
    double weight = 0.;
    switch (GetDiffusionType()) {
      case 0:
        weight = GetWeightUniform(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 1:
        weight = GetWeightGauss(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 2:
        weight = GetWeightGaussBox(coordimpl,projection,diffu_proj,istrip-0.5,istrip+0.5);
        break;
      case 3:
        weight = GetWeightTriangular(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 4:
        weight = GetWeightGaussSum(coordimpl,sign*track_proj,diffusion,istrip-0.5,istrip+0.5);
        break;
      default:
        printf("TrSimSensor::MakeImplantCluster-E invalid diffusion model (model=%1d) selected\n",GetDiffusionType());
        return TrSimCluster();
        break;
    }
    if (weight<TOLERANCE) break;
    check += weight;
    acluster.push_back(weight);
  }
  // create on the left
  for (int istrip=seedind-1; istrip>=0; istrip--) {
    double weight = 0.;
    switch (GetDiffusionType()) {
      case 0:
        weight = GetWeightUniform(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 1:
        weight = GetWeightGauss(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 2:
        weight = GetWeightGaussBox(coordimpl,projection,diffu_proj,istrip-0.5,istrip+0.5);
        break;
      case 3:
        weight = GetWeightTriangular(coordimpl,projection,istrip-0.5,istrip+0.5);
        break;
      case 4:
        weight = GetWeightGaussSum(coordimpl,sign*track_proj,diffusion,istrip-0.5,istrip+0.5);
        break;
      default:
        printf("TrSimSensor::MakeImplantCluster-E invalid diffusion model (model=%1d) selected\n",GetDiffusionType());
        return TrSimCluster();
        break;
    }
    if (weight<TOLERANCE) break;
    address = istrip;	
    check += weight;
    acluster.insert(acluster.begin(),weight);
  }
  // cluster signal is not 1, rounding error
  if (fabs(check-1.)>10.*TOLERANCE) {
    if (WARNING) printf("TrSimSensor::MakeModelizedCluster-W signal check failed (check=%7.4f, nstrips=%d), returning an empty cluster.\n",check,(int)acluster.size());
    return TrSimCluster();
  }
  // cluster size is 0 
  if ((int)acluster.size()==0) {
    if (WARNING) printf("TrSimSensor::MakeModelizedCluster-W a 0-lenght cluster, returning an empty cluster.\n");
    return TrSimCluster(); 
  }
  // returning cluster 
  return TrSimCluster(acluster, address, seedind-address);
}

 
double TrSimSensor::GetWeightUniform(double x, double width, double x1, double x2) {
  double a = x - width/2.;
  double b = x + width/2.;
  double factor = 1./width;
  double maxim = min(b,x2);
  double minim = max(a,x1);
  double weight = (maxim-minim)*factor;
  if (weight<0.) weight = 0.;
  return weight;
}


double TrSimSensor::GetWeightGauss(double x, double sigma, double x1, double x2) {
  double w  = sigma*sqrt(2);
  double X1 = (x1-x)/w;
  double X2 = (x2-x)/w;
  return 0.5*(TMath::Erf(X2)-TMath::Erf(X1));
}


double TrSimSensor::GetWeightGaussBox(double x, double width, double sigma, double x1, double x2) {
  double X1 = x1-x;
  double X2 = x2-x;
  double int1 = TrSimSensor::CumulativeGaussBox(X1,width,sigma);
  double int2 = TrSimSensor::CumulativeGaussBox(X2,width,sigma);  
  return int2 - int1;
}


double TrSimSensor::CumulativeGaussBox(double x, double t, double sigma) {
  double x2 = (x + t/2)/sigma;
  double x1 = (x - t/2)/sigma;
  double erf2 = TMath::Erf(x2/sqrt(2));
  double erf1 = TMath::Erf(x1/sqrt(2));
  double term1 = 0.5*(erf1 + 1); 
  double term2 = 0.5*(x/t + 0.5)*(erf2 - erf1);
  double term3 = (sigma/t)*(TMath::Gaus(x2,0,1,kTRUE) - TMath::Gaus(x1,0,1,kTRUE));
  return term1 + term2 + term3;
}


double TrSimSensor::GetWeightTriangular(double x, double width, double x1, double x2) {
  double X1 = x1-x;
  double X2 = x2-x;
  double int1 = TrSimSensor::CumulativeTriangular(X1,width);
  double int2 = TrSimSensor::CumulativeTriangular(X2,width);
  return int2 - int1;
}


double TrSimSensor::CumulativeTriangular(double x, double t) {
  double integral = 0.5;
  if (fabs(x)<=t/2) integral = 2*(fabs(x)/t)*(1 - fabs(x)/t);
  if (x<0.) integral = -integral;
  return integral;
}


double TrSimSensor::GetWeightGaussSum(double x, double width, double sigma, double x1, double x2) {
  double erf = 0.;
  for (int i=0; i<NSTEPSGAUSSSUM; i++) {
    double mu = x - width/2 + (i+0.5)*width/NSTEPSGAUSSSUM;
    double si = sqrt(2)*sigma*sqrt((i+0.5)/NSTEPSGAUSSSUM);
    double X1 = (x1-mu)/si;
    double X2 = (x2-mu)/si;
    erf += 0.5*(TMath::Erf(X2)-TMath::Erf(X1))*(1./NSTEPSGAUSSSUM);
  }
  return erf;
}


TrSimCluster TrSimSensor::MakeClusterFromAChargeInjectionOnAnImplant(double Q, int impladd, int nsens) {         
  // tmp cluster init
  vector<double> acluster;
  acluster.clear();
  int address = -1;
  // from implant strip
  int    nimplants = GetNImplantStrips();
  double Ctot = _CU.at(impladd) + _CD.at(impladd) + _CL.at(impladd) + _CR.at(impladd);
  double QU = Q*_CU.at(impladd)/Ctot;
  // double QD = Q*_CD.at(impladd)/Ctot; // unused
  double QL = Q*_CL.at(impladd)/Ctot;
  double QR = Q*_CR.at(impladd)/Ctot;
  if (IsReadoutStrip(impladd)) { 
    address = GetReadoutAddressFromImplantAddress(impladd,nsens); 
    acluster.push_back(QU);
  }
  // right loop
  for (int ii=impladd+1; ii<nimplants; ii++) {
    double Ctot2 = _CU.at(ii) + _CD.at(ii) + _CR.at(ii);
    double QU2   = QR*_CU.at(ii)/Ctot2;
    // double QD2   = QR*_CD.at(ii)/Ctot2; // unused
    double QR2   = QR*_CR.at(ii)/Ctot2;
    QR = QR2;
    if (IsReadoutStrip(ii)) {
      if (QU2<TOLERANCE) break;
      if (address<0) address = GetReadoutAddressFromImplantAddress(ii,nsens);
      acluster.push_back(QU2);
    }
  }
  // left loop
  for (int ii=impladd-1; ii>=0; ii--) {
    double Ctot2 = _CU.at(ii) + _CD.at(ii) + _CL.at(ii);
    double QU2   = QL*_CU.at(ii)/Ctot2;
    // double QD2   = QL*_CD.at(ii)/Ctot2; // unused 
    double QL2   = QL*_CL.at(ii)/Ctot2;
    QL = QL2;
    if (IsReadoutStrip(ii)) { 
      if (QU2<TOLERANCE) break;
      address = GetReadoutAddressFromImplantAddress(ii,nsens); 
      acluster.insert(acluster.begin(),QU2);
    }
  }
  // check
  if ((int)acluster.size()==0) {
    if (WARNING) printf("TrSimSensor::MakeClusterFromAChargeInjectionOnAnImplant-W 0-lenght cluster, returning an empty cluster (impladd=%4d,Q=%7.3f).\n",impladd,Q);
    return TrSimCluster(); 
  }
  // create TrSimCluster (readout strips units)
  return TrSimCluster(acluster,address);
}


TrSimCluster TrSimSensor::MakeClusterFromImplantCluster(TrSimCluster& implclus, int nsens) {
  if ( (implclus.GetAddress()<0)||(implclus.GetWidth()==0) ) { 
    if (WARNING) printf("TrSimSensor::MakeClusterFromModelizedCluster-W passing a null or empty cluster, returning an empty cluster.\n");
    return TrSimCluster();
  }
  TrSimCluster cluster;
  // loop on implantation strips
  for (int ii=0; ii<implclus.GetWidth(); ii++) {
    int    impladd = implclus.GetAddress() + ii;
    double Q       = implclus.GetSignal(ii);
    TrSimCluster addcluster = MakeClusterFromAChargeInjectionOnAnImplant(Q,impladd,nsens);
    if (VERBOSE) { printf("TrSimSensor::MakeClusterFromImplantCluster-V add cluster:\n"); addcluster.Info(10); }
    cluster.AddCluster(addcluster);
  }
  if ( (cluster.GetWidth()==0) ) {
    if (WARNING) printf("TrSimSensor::MakeClusterFromModelizedCluster-W returning an empty cluster.\n");
    return TrSimCluster();
  }
  return cluster;
}


TrSimCluster TrSimSensor::MakeCluster(double senscoo, double sensangle, int nsens) { 
  TrSimCluster implclus = MakeImplantCluster(senscoo, sensangle); 
  if (VERBOSE) { printf("TrSimSensor::MakeCluster-V implant cluster:\n"); implclus.Info(10); }
  TrSimCluster readclus = MakeClusterFromImplantCluster(implclus, nsens);
  if (VERBOSE) { printf("TrSimSensor::MakeCluster-V read cluster:\n"); readclus.Info(10); }
  return readclus; 
}


double TrSimSensor::LanGauExpFun(Double_t *x, Double_t *par) {
  // Fit parameters:
  //
  // par[0] = Width (scale) parameter of Landau density
  // par[1] = Most Probable (MP, location) parameter of Landau density
  // par[2] = Total area (integral -inf to inf, normalization constant)
  // par[3] = Width (sigma) of convoluted Gaussian function
  // par[4] = Separation point between langaus and exp (connected by derivative and continuity)
  //   
  // In the Landau distribution (represented by the CERNLIB approximation), 
  // the maximum is located at x=-0.22278298 with the location parameter=0.
  // This shift is corrected within this function, so that the actual
  // maximum is identical to the MP parameter.
  //
  // The additional exponential function is connected imposing 2 condition
  // - continuity: LanGauss(sep) = k * exp(-xsep/slope)
  // - derivative continuity: LanGauss'(xsep) = -k/slope * exp(-xsep/slope)
  // slope = - LanGauss(xsep) / LanGauss'(xsep)
  // k     =   LanGauss(xsep) * exp(xsep/slope)
  if (x[0]<=par[4]){ //LanGauss    
    // Numeric constants
    Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    Double_t mpshift  = -0.22278298;       // Landau maximum location
    // Control constants
    Double_t np = 100.0;      // number of convolution steps
    Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    // Variables
    Double_t xx;
    Double_t mpc;
    Double_t fland;
    Double_t sum = 0.0;
    Double_t xlow,xupp;
    Double_t step;
    Double_t i;
    // MP shift correction
    mpc = par[1] - mpshift * par[0]; 
    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    step = (xupp-xlow) / np;
    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    return (par[2] * step * sum * invsq2pi / par[3]);
  }
  else { // Exponential tail
    // Control constants
    Double_t dx = 10.;     // infinitesimal interval for derivative calculation 
                           // must be longer than the bin width (how implement this check?)
    // Variables   
    Double_t xsep; 
    Double_t xpre; 
    Double_t fxsep;
    Double_t fxpre;
    Double_t deriv;
    Double_t slope;
    Double_t k;     
    // Derivative calculation
    xsep  = par[4];
    xpre  = par[4] - dx;
    fxsep = LanGauExpFun(&xsep,par);
    fxpre = LanGauExpFun(&xpre,par);
    deriv = (fxsep-fxpre)/dx;
    // check of existence
    if (deriv==0.) return -1e+06;  
    slope = -fxsep/deriv; 
    if (slope==0.) return -1e+06;  
    if ((xsep/slope)>709.) return -1e+06;
    k     = fxsep * exp(xsep/slope);
    //   cout << k << " " << fxsep << " " << xsep << " " << slope << " " << xsep/slope << " " << exp(xsep/slope) << endl;
    return k * exp(-x[0]/slope);
  }
}

double TrSimSensor::LandauFun(Double_t *x, Double_t *par) {
  return par[3]*TMath::Landau(x[0]/par[2],par[0],par[1]);
}

double TrSimSensor::LanGauFun(double* x, double* par) {
  Double_t x0 = x[0]/par[3];
  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location
  // Control constants
  Double_t np =  10.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;
  // MP shift correction
  mpc = par[1] - mpshift * par[0]; 
  // Range of convolution integral
  xlow = x0 - sc * par[2]/par[3];
  xupp = x0 + sc * par[2]/par[3];
  step = (xupp-xlow) / np;
  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x0,xx,par[2]);
    xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x0,xx,par[2]);
  }
  return (par[4] * step * sum * invsq2pi / par[2]);
}


double TrSimSensor::fromMCtoRealData(double adc) {
  // adc has to be given for 300 um straight tracks
  float xmax   = _refun->GetMaximumX(0.,MAXADC);
  float value  = _mcfun->Eval(adc);
  float newadc = (adc<xmax) ? _refun->GetX(value,0.,xmax) : _refun->GetX(value,xmax,MAXADC);
  return newadc; 
}

double TrSimSensor::BetheBlock(double z, double bg) {
  double z2 = z*z;
  double bg2 = bg*bg;
  double dd  = 2*log(1+bg/100); // Density effect (to be tuned)
  double dEdx = 10.73*z2*((1+bg2)/bg2)*(8.68+log(bg2)-bg2/(1+bg2)-dd); // keV

  return dEdx; 
}
