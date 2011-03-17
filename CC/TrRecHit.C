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

  _clusterX = clX;   
  _clusterY = clY;   
  VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
  _iclusterX = (_clusterX) ? cont2->getindex(_clusterX) : -1;
  _iclusterY = (_clusterY) ? cont2->getindex(_clusterY) : -1;
  delete cont2;

  Status    = status;
  if (!clX) Status |= YONLY;
  if (!clY) Status |= XONLY;
  _dummyX   = 0;
  int xaddr =  640;
  if(clX!=0)
	xaddr =  clX->GetAddress();
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
    sout.append(Form("tkid: %+03d Right Coo %d (x,y,z)=(%10.4f,%10.4f,%10.4f) AmpP: %5.0f AmpN: %5.0f corr: %8.4f  prob: %7.5f  stat: %2d\n",
		     _tkid,_imult,GetCoord(_imult).x(),GetCoord(_imult).y(),GetCoord(_imult).z(),
		     (GetYCluster())?GetYCluster()->GetTotSignal():0,
		     (GetXCluster())?GetXCluster()->GetTotSignal():0,
		     GetCorrelation(),GetProb(),getstatus()));
  else 
    sout.append(Form("tkid: %+03d Base Coo 0 (x,y,z)=(%10.4f,%10.4f,%10.4f) AmpP: %5.0f AmpN:%5.0f  corr: %8.4f  prob: %7.5f  stat: %2d\n",
		     _tkid,GetCoord(0).x(),GetCoord(0).y(),GetCoord(0).z(),
		     (GetYCluster())?GetYCluster()->GetTotSignal():0,
		     (GetXCluster())?GetXCluster()->GetTotSignal():0,
		     GetCoord(0).z(),GetCorrelation(),GetProb(),getstatus()));
  if(!opt) return;
  for(int ii=0;ii<_mult;ii++)
    sout.append(Form("mult %d (x,y,z)=(%10.4f,%10.4f,%10.4f)\n",
		     ii,GetCoord(ii).x(),GetCoord(ii).y(),GetCoord(ii).z()));
}


char *  TrRecHitR::Info(int iRef){
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


float TrRecHitR::GetXloc(int imult, int nstrips) {
  TrClusterR *cls = GetXCluster();
  if (!cls) return TkCoo::GetLocalCoo(_tkid,_dummyX+640,imult);
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips, imult);
}


float TrRecHitR::GetYloc(int nstrips) { 
  TrClusterR *cls = GetYCluster();
  if (!cls) return -1000;
  if (TasHit()) return ((TrTasClusterR *)cls)->GetXCofGTas();
  return cls->GetXCofG(nstrips);
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


AMSPoint TrRecHitR::GetGlobalCoordinate(int imult, const char* options,
					int nstripsx, int nstripsy) {
  // parsing options
  bool ApplyAlignement = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='A')||(character=='a') ) ApplyAlignement = true;
    cc++;
  }
  // calculation
  AMSPoint loc = GetLocalCoordinate(imult, nstripsx, nstripsy);
  
  AMSPoint glo;
  if (!ApplyAlignement) {
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
    if (TasHit()) glo = glo-TrTasClusterR::Align(GetXCluster(), GetYCluster());
  }
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


float TrRecHitR::GetSignalCombination() {
  int opt = TrClusterR::kAngle|TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss|TrClusterR::kPN;
  if ((GetXCluster()!=0)&&(GetYCluster()!=0)) {
    float sig_x = GetXCluster()->GetTotSignal(opt);
    float sig_y = GetYCluster()->GetTotSignal(opt);
    return (sig_x + sig_y)/2;     
  }
  else if (GetXCluster()!=0) { 
    return GetXCluster()->GetTotSignal(opt);
  }
  else if (GetYCluster()!=0) {
    return GetYCluster()->GetTotSignal(opt);
  }
  return 0;
}


float TrRecHitR::GetSignalCorrelation() {
  int opt = TrClusterR::kAngle|TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kVAGain|TrClusterR::kLoss|TrClusterR::kPN;
  if ((GetXCluster()!=0)&&(GetYCluster()!=0)) {
    float sig_x = GetXCluster()->GetTotSignal(opt);
    float sig_y = GetYCluster()->GetTotSignal(opt);
    return sig_x - sig_y;
  }
  else if (GetXCluster()!=0) {
    return -1000;
  }
  else if (GetYCluster()!=0) {
    return  1000; 
  }
  return 10000;
}


