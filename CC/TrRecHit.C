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
ClassImp(TrRecHitR);





#ifdef __ROOTSHAREDLIBRARY__
#ifdef _STANDALONE_
#include "VCon_root2.h"
#else
#include "VCon_root.h"
#endif
VCon* TrRecHitR::vcon= new VCon_root();
#else  
#include "VCon_gbatch.h"
VCon* TrRecHitR::vcon = new VCon_gb();
#endif


TrRecHitR::TrRecHitR(void) {
  Clear();
}


TrRecHitR::TrRecHitR(const TrRecHitR& orig) {
  _tkid     = orig._tkid;     
  _clusterX = orig._clusterX;   
  _clusterY = orig._clusterY;  

  _iclusterX = orig._iclusterX;   
  _iclusterY = orig._iclusterY;  
  _corr     = orig._corr;
  _prob     = orig._prob;     
  Status   = orig.Status;
  _mult     = orig._mult;
  _imult    = orig._imult;
  _coord    = orig._coord;
  _dummyX   = orig._dummyX;
}


TrRecHitR::TrRecHitR(int tkid, TrClusterR* clX, TrClusterR* clY, float corr, float prob, int imult) {
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
  VCon* cont2=vcon->GetCont("AMSTrCluster");
  _iclusterX = (_clusterX) ? cont2->getindex(_clusterX) : -1;
  _iclusterY = (_clusterY) ? cont2->getindex(_clusterY) : -1;
  delete cont2;

  Status   = 0;
  if (!clX) Status |= YONLY;
  if (!clY) Status |= XONLY;
  _corr     = corr;
  _prob     = prob;     
  _dummyX   = 0;

  // coordinate construction
  BuildCoordinates();
  _imult    = imult; 
}

TrClusterR* TrRecHitR::GetXCluster() { 
  if(_clusterX==NULL&& !(Status&YONLY)){
    VCon* cont2=vcon->GetCont("AMSTrCluster");
    _clusterX = (TrClusterR*)cont2->getelem(_iclusterX);
    delete cont2;
  }
  return _clusterX;
}


TrClusterR* TrRecHitR::GetYCluster() { 
  if(_clusterY==NULL&& !(Status&XONLY)){
    VCon* cont2=vcon->GetCont("AMSTrCluster");
    _clusterY = (TrClusterR*)cont2->getelem(_iclusterY);
    delete cont2;
  }
  return _clusterY;
}



void TrRecHitR::BuildCoordinates() {
  // coordinate construction
  TrClusterR* clX= GetXCluster();
  int xaddr =  640;
  if(clX!=0)
    xaddr =  clX->GetAddress();

  _mult     = TkCoo::GetMaxMult(GetTkId(), xaddr)+1;
  _coord.clear();
  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetGlobalCoordinate(imult));
}

TrRecHitR::~TrRecHitR() {
  Clear();
}





void TrRecHitR::Clear() {
  _tkid     = 0; 
  _clusterX = 0;
  _clusterY = 0;
  _iclusterX = -1;
  _iclusterY = -1;
  _prob     = 0;
  Status   = 0;
  _mult     = 0;
  _imult    = -1; 
  _dummyX   = 0;
  _coord.clear();
}


void TrRecHitR::Print(int opt){
  _PrepareOutput(opt);
  cout <<sout;

}

void TrRecHitR::_PrepareOutput(int opt) { 

  sout.clear();

  if(_imult>0) 
    sout.append(Form("tkid: %+03d Right Coo %d (x,y,z)=(%10.4f,%10.4f,%10.4f) corr: %8.4f  prob: %7.5f  stat: %2d\n",
		     _tkid,_imult,GetCoord(_imult).x(),GetCoord(_imult).y(),GetCoord(_imult).z(),
		     GetCorrelation(),GetProb(),getstatus()));
  else 
    sout.append(Form("tkid: %+03d Base Coo 0 (x,y,z)=(%10.4f,%10.4f,%10.4f)  corr: %8.4f  prob: %7.5f  stat: %2d\n",
		     _tkid,GetCoord(0).x(),GetCoord(0).y(),
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
  strncpy(_Info,aa.c_str(),len);
  return _Info;
}

std::ostream &TrRecHitR::putout(std::ostream &ostr)  {
  _PrepareOutput(1);

  return ostr << sout  << std::endl; 
    
}


AMSPoint TrRecHitR::GetGlobalCoordinate(int imult, char* options) {
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
  AMSPoint loc = GetLocalCoordinate(imult);
  
  AMSPoint glo;
  if (!ApplyAlignement) {
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
  }
  return glo;
}

