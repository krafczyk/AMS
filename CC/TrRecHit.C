//////////////////////////////////////////////////////////////////////////
///
///\file  TrRecHit.C
///\brief Source file of AMSTrRecHit class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\data  2008/03/06 AO  Changing some data members and methods
///\data  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecHit.h"
#include "TkDBc.h"
#include "TrTasCluster.h"
#include "MagField.h"
#include "TMath.h"
#include "TrExtAlignDB.h"

int UpdateExtLayer(int type=0,int lad1=-1,int lad9=-1);
ClassImp(TrRecHitR);

#include "VCon.h"

float TrRecHitR::GGpars[6]={1428., 0.0000,
			    0.1444, 1645.,
			    0.0109, 0.0972};
float TrRecHitR::GGintegral=91765.;


TrRecHitR::TrRecHitR(void) {
  Clear();
}


TrRecHitR::TrRecHitR(const TrRecHitR& orig) {
  _tkid     = orig._tkid;     
  _clusterX = orig._clusterX;   
  _clusterY = orig._clusterY;  

  _iclusterX = orig._iclusterX;   
  _iclusterY = orig._iclusterY;  
  //  _corr     = orig._corr;
  //  _prob     = orig._prob;     
  Status   = orig.Status;
  _mult     = orig._mult;
  _imult    = orig._imult;
  _coord    = orig._coord;
  //  _bfield    = orig._bfield;
  _dummyX   = orig._dummyX;
}


TrRecHitR::TrRecHitR(int tkid, TrClusterR* clX, TrClusterR* clY,  int imult, int status) {
  _tkid     = tkid;   
  if((clX&&clX->GetTkId()!=_tkid)|| (clY&&clY->GetTkId()!=_tkid)){
    printf("TrRecHitR::TrRecHitR--> BIG problems you are building ans hit on Ladder %d  \n",_tkid);
    printf("                                        with a cluster X from Ladder %d and \n",clX->GetTkId());
    printf("                                        with a cluster Y from Ladder %d     \n",clY->GetTkId());
  }
  if(clX&&clX->GetSide()!=0)
    printf("TrRecHitR::TrRecHitR--> BIG problems The cluster you passed as X is on Y!!!!!  \n");
  if(clY&&clY->GetSide()!=1)
    printf("TrRecHitR::TrRecHitR--> BIG problems The cluster you passed as Y is on X!!!!!  \n");

  Status    = status;
  _clusterX = clX;   
  _clusterY = clY;   
  if (Reblt()) {
    _iclusterX = -1;
    _iclusterY = -1;
  }
  else {
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _iclusterX = (_clusterX) ? cont2->getindex(_clusterX) : -1;
    _iclusterY = (_clusterY) ? cont2->getindex(_clusterY) : -1;
    delete cont2;
  }

  if (!clX) Status |= YONLY;
  if (!clY) Status |= XONLY;
  _dummyX   = 0;
  int xaddr =  640;
  if(clX!=0)
	xaddr =  clX->GetAddress(); // maybe better to use clX->GetSeedAddress();
  else if(_dummyX>=0)
	xaddr += _dummyX;
	  
  _mult = (TasHit()) ? 1 : TkCoo::GetMaxMult(GetTkId(), xaddr)+1;

  // coordinate construction
  _imult    = imult; 
  _coord=GetGlobalCoordinate(_imult);	
}


const AMSPoint TrRecHitR::HitPointDist(const AMSPoint& aa,int& mult){
  int mm=GetMultiplicity();
  float max=999999.;
  mult=-1;
  for (int ii=0;ii<mm;ii++){
    AMSPoint cc = aa - GetCoord(ii);
    if(fabs(cc[0])<max){
      mult=ii;
      max=fabs(cc[0]);
    }
  }
  
  if (mult>-1&& mult <mm)
    return aa-GetCoord(mult);
  else
    return aa-GetCoord(0); 
}


float TrRecHitR::HitDist(TrRecHitR& B,int coo){
  if(coo==1 || coo==2){
    return (this->GetCoord()-B.GetCoord())[coo];
  }
  else if(coo==0){
    int MminA=0;
    int MminB=0;
    int MmaxA=this->GetMultiplicity();
    int MmaxB=B.GetMultiplicity();
    
    if(this->GetResolvedMultiplicity()>=0){
      MminA=this->GetResolvedMultiplicity();
      MmaxA=MminA+1;
    }
    
    if(B.GetResolvedMultiplicity()>=0){
      MminB=B.GetResolvedMultiplicity();
      MmaxB=MminB+1;
    }
    float dmin=1000;
    float mind=1000;
    for (int la=MminA;la <MmaxA;la++)
      for (int lb=MminB;lb <MmaxB;lb++){
	float dist=(this->GetCoord(la)-B.GetCoord(lb))[0];
	if(fabs(dist) < dmin) {dmin=fabs(dist) ; mind=dist;}
    }
    return mind;
  } else 
    return -1;
}


TrClusterR* TrRecHitR::GetXCluster() { 
  if(_clusterX==NULL&& !(Status&YONLY)){
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _clusterX = (TrClusterR*)cont2->getelem(_iclusterX);
    delete cont2;
  }
  return _clusterX;
}


TrClusterR* TrRecHitR::GetYCluster() { 
  if(_clusterY==NULL&& !(Status&XONLY)){
    VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
    _clusterY = (TrClusterR*)cont2->getelem(_iclusterY);
    delete cont2;
  }
  return _clusterY;
}

//void TrRecHitR::BuildCoordinates() {
//  // coordinate construction
//  TrClusterR* clX= GetXCluster();
//  int xaddr =  640;
//  if(clX!=0)
//    xaddr =  clX->GetAddress();
//  else if(_dummyX>=0)
//    xaddr += _dummyX;
//  
//  _mult = (TasHit()) ? 1 : TkCoo::GetMaxMult(GetTkId(), xaddr)+1;
//  _coord.clear();
//  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetGlobalCoordinate(imult));
//
////   for (int ii=0;ii<_coord.size();ii++){
////     float x[3],b[3];
////     x[0]=_coord[ii].x();
////     x[1]=_coord[ii].y();
////     x[2]=_coord[ii].z();
////       GUFLD(x,b);
////     _bfield.push_back(AMSPoint(b));
////   }
//
//
//  if (TasHit()) {
//    if (!GetXCluster() || !GetXCluster()->TasCls() || 
//        !GetYCluster() || !GetYCluster()->TasCls()) Status &= ~TASHIT;
//  }
//}

TrRecHitR::~TrRecHitR() {
  Clear();
}


void TrRecHitR::Clear() {
  _tkid     = 0; 
  _clusterX = 0;
  _clusterY = 0;
  _iclusterX = -1;
  _iclusterY = -1;
  Status   = 0;
  _mult     = 0;
  _imult    = -1; 
  _dummyX   = 0;
  _coord=AMSPoint(0,0,0);
  //  _bfield.clear();
}


void TrRecHitR::Print(int opt){
  _PrepareOutput(opt);
  cout <<sout;
}


void TrRecHitR::_PrepareOutput(int opt) { 
  sout.clear();
  if(_imult>0) 
    sout.append(Form("tkid: %+03d Right Coo %d (x,y,z)=(%10.4f,%10.4f,%10.4f)  AmpY: %8.2f  AmpX: %8.2f  Prob: %8.5f  Status: %4d  QStatus: %8X\n",
		     _tkid,_imult,GetCoord(_imult).x(),GetCoord(_imult).y(),GetCoord(_imult).z(),
		     (GetYCluster()) ? GetYCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     (GetXCluster()) ? GetXCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     GetCorrelationProb(),getstatus(),GetQStatus()));
  else 
    sout.append(Form("tkid: %+03d Base  Coo 0 (x,y,z)=(%10.4f,%10.4f,%10.4f)  AmpY: %8.2f  AmpX: %8.2f  Prob: %8.5f  Status: %4d  QStatus: %8X\n",
		     _tkid,GetCoord(0).x(),GetCoord(0).y(),GetCoord(0).z(),
                     (GetYCluster()) ? GetYCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
                     (GetXCluster()) ? GetXCluster()->GetTotSignal(TrClusterR::DefaultCorrOpt) : 0,
		     GetCorrelationProb(),getstatus(),GetQStatus()));
  if(!opt) return;
  for(int ii=0;ii<_mult;ii++)
    sout.append(Form("mult %d (x,y,z)=(%10.4f,%10.4f,%10.4f)\n",
		     ii,GetCoord(ii).x(),GetCoord(ii).y(),GetCoord(ii).z()));
}


const char *  TrRecHitR::Info(int iRef){
  string aa;
  aa.append(Form("TrRecHit #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}


std::ostream &TrRecHitR::putout(std::ostream &ostr)  {
  _PrepareOutput(1);
  return ostr << sout  << std::endl; 
}


float TrRecHitR::GetXloc(int imult, int nstrips, int opt) {
  TrClusterR *cls = GetXCluster();
  if (!cls) return TkCoo::GetLocalCoo(_tkid,_dummyX+640,imult);
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips, imult, opt);
}


float TrRecHitR::GetYloc(int nstrips, int opt) { 
  TrClusterR *cls = GetYCluster();
  if (!cls) return -1000;
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips, opt);
}


void TrRecHitR::SetUsed() {
  setstatus(AMSDBc::USED);
  if (GetXCluster()) GetXCluster()->SetUsed();
  if (GetYCluster()) GetYCluster()->SetUsed();
}


void TrRecHitR::ClearUsed() {
  clearstatus(AMSDBc::USED);
  if (GetXCluster()) GetXCluster()->ClearUsed();
  if (GetYCluster()) GetYCluster()->ClearUsed();
}

// #include "root.h"
// #include "DynAlignment.h"
// void     TrRecHitR::BuildCoordinateDynExt(){
//   if(GetLayerJ()!=1 && GetLayerJ()!=9) {BuildCoordinate();return;}  // Apply the patch only for layer 1 and 9
//   if(_imult<0) return;
  
//   double newcoor[3]={0,0,0};
//   if(DynAlManager::FindAlignment(*AMSEventR::Head(),*this,newcoor[0],newcoor[1],newcoor[2])){
//     for(int i=0;i<3;i++) _coord[i]=newcoor[i];
//   }else{
//     cout<<"TrRecHitR::BuildCoordinateDynExt-W-DynAlManager::FindAlignment did not find an alignment for run "<<AMSEventR::Head()->fHeader.Run<<" hit at layer "<<this->GetLayerJ()<<endl;
//   }
// }


AMSPoint TrRecHitR::GetGlobalCoordinate(int imult, const char* options,
					int nstripsx, int nstripsy, int optx, int opty) {
  // parsing options
  bool ApplyAlignement = false;
  bool ApplyCiemat = false;
  bool NoExtAlignment = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='Z')||(character=='z') ) NoExtAlignment= true;
    if ( (character=='A')||(character=='a') ) ApplyAlignement = true;
    if ( (character=='M')||(character=='m') ) ApplyCiemat = true;
    cc++;
  }
  // calculation
  AMSPoint loc = GetLocalCoordinate(imult, nstripsx, nstripsy, optx, opty);
  if(ApplyCiemat){
    if (GetLayer()==8)
      UpdateExtLayer(1,GetSlotSide()*10+lad()*100,-1);
    else if(GetLayer()==9)
      UpdateExtLayer(1,-1,GetSlotSide()*10+lad()*100);
    TrExtAlignDB::SetAlKind(1);
  }else if(NoExtAlignment){
    TrExtAlignDB::SetAlKind(2);
  }else
    TrExtAlignDB::SetAlKind(0);

  AMSPoint glo;
  if (!ApplyAlignement) {
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    if( (GetLayer()==8)|| (GetLayer()==9))UpdateExtLayer(0);
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
    if (TasHit()) glo = glo-TrTasClusterR::Align(GetXCluster(), GetYCluster());
  }
  TrExtAlignDB::SetAlKind(0);
  return glo;
}

void TrRecHitR::SetiTrCluster(int iclsx, int iclsy)
{
  VCon *cont = GetVCon()->GetCont("AMSTrCluster");

  _iclusterX = iclsx;
  _iclusterY = iclsy;
  _clusterX  = (!OnlyY()) ? (TrClusterR *)cont->getelem(iclsx) : 0;
  _clusterY  = (!OnlyX()) ? (TrClusterR *)cont->getelem(iclsy) : 0;

  delete cont;
}


float TrRecHitR::GetCorrelation()   {
  if (!GetXCluster()) return -1.;
  if (!GetYCluster()) return 1.;
  float n = GetXCluster()->GetTotSignal();
  float p = GetYCluster()->GetTotSignal();
  return (p - n)/(p + n);
}


float TrRecHitR::GetProb()   {
  float correlation = GetCorrelation();
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
           GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}


float TrRecHitR::GetSignalCombination(int iside, int opt, float beta, float rigidity, float mass_on_Z) {
  int mult = GetResolvedMultiplicity();
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  float sig_x = (clx!=0) ? clx->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult) : 0;
  float sig_y = (cly!=0) ? cly->GetTotSignal(opt,beta,rigidity,mass_on_Z,mult) : 0;
  float wei_x = 1; // to be tuned 
  float wei_y = 1; // to be tuned 
  if      (iside==0) return sig_x;
  else if (iside==1) return sig_y; 
  else if (iside==2) return (clx!=0) ? sig_x : sig_y;
  else if (iside==3) return (sig_x*wei_x + sig_y*wei_y)/(wei_x + wei_y);
  else if (iside==4) return (sig_x + sig_y)/2;
  return 0.;
}


int TrRecHitR::GetQStatus(int nstrip_from_seed) { 
  int Sx = (GetXCluster()) ? GetXCluster()->GetQStatus(nstrip_from_seed,GetResolvedMultiplicity()) : 0;
  int Sy = (GetYCluster()) ? GetYCluster()->GetQStatus(nstrip_from_seed,GetResolvedMultiplicity()) : 0; 
  return (Sy<<12)|Sx;
}


float TrRecHitR::GetSignalDifference() {
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  if ( (clx!=0)&&(cly!=0) ) {
    float sig_x = clx->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kPN);
    float sig_y = cly->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kLoss|TrClusterR::kPN);
    return sig_x - sig_y;
  }
  else if (clx!=0) return -10000;
  else if (cly!=0) return  10000;
  return -1000000;
}


/* 
  Hit Correlation (updated on Sep 2012): 
  - Hypothesys: for a fixed x the y distribution is gaussian
  - Plot sqrt(signal y) VS sqrt(signal x) with only basic correction (to be independent from charge reconstruction)
  - Gaussian slice fit 
  - Polynomial fit of the mean (through 0,0) and sigma behaviour
*/
/* Old parameters (2011)
static double HitCorrelation_XMax = 134; // after this value I give always 1 as probability
static double HitCorrelation_MeanPar[7] = {0,1.15735,-1.36923e-02,7.01365e-05,0,0,0};
static double HitCorrelation_SigmPar[7] = {0.943443,-0.0388644,0.00313217,-5.64444e-06,-1.0751e-06,1.35689e-08,-4.64308e-11};
static double HitCorrelation_SigmMin = 0.8; // evaluated by hand for very low x
*/
// options used for probability calculation
static int    HitCorrelation_Opt = TrClusterR::kAsym;
// mean position
static double HitCorrelation_MeanPar[7] = {0, 1.10018e+00,-6.99183e-03,-1.92904e-04,3.55675e-06,-1.44755e-08,0};
// monotonic function (explodes after 123 ADC counts)!
static double HitCorrelation_XMax = 160;
static double HitCorrelation_SigmPar[7] = {1,-5.50738e-02, 1.24566e-02,-4.60968e-04,7.55153e-06,-5.75738e-08,1.67042e-10}; 
static double HitCorrelation_SigmMin = 1;
// list of low gain on p (added on nov. 2013)
static int HitCorrelation_TkId_Gain15[15] = {-803, 307, 305, 802, 813, -813, 811, -610, 611, 110, 703, 503, 603, -706, -903};
static int HitCorrelation_TkId_Gain20[ 2] = {213, -904};
float TrRecHitR::GetCorrelationProb() {
  TrClusterR* clx = GetXCluster();
  TrClusterR* cly = GetYCluster();
  if ( (clx==0)||(cly==0) ) return -1; // no definition, default value!
  // cluster signal
  double sig_x = clx->GetTotSignal(HitCorrelation_Opt); 
  double x = sqrt(sig_x);
  if (x>HitCorrelation_XMax) return 1; // good if out of range
  double sig_y = cly->GetTotSignal(HitCorrelation_Opt);
  if ( (HitCorrelation_TkId_Gain15+15)!=find(HitCorrelation_TkId_Gain15,HitCorrelation_TkId_Gain15+15,GetTkId())) sig_y *= 1.5;
  if ( (HitCorrelation_TkId_Gain20+ 2)!=find(HitCorrelation_TkId_Gain20,HitCorrelation_TkId_Gain20+ 2,GetTkId())) sig_y *= 2.0;
  double y = sqrt(sig_y);
  // parameters for the test
  double mean = 0.;
  double sigma = 0.;
  for (int ipar=0; ipar<7; ipar++) {
    double tmp = pow(x,ipar);
    mean  += HitCorrelation_MeanPar[ipar]*tmp;
    sigma += HitCorrelation_SigmPar[ipar]*tmp;
  }
  // gaussian p-value
  float n = fabs(y - mean)/sigma;
  return 1-TMath::Erf(n/sqrt(2));
}
