// $Id: TrTrack.C,v 1.2 2009/02/01 15:58:33 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTrack.C
///\brief Source file of AMSTrTrack class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/20 SH  TkFit support
///\date  2007/12/28 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/03/01 AO  Preliminary new data format
///\date  2008/03/12 SH  Getting back to SH version
///\date  2008/07/01 PZ  New Tracker Lib Standalone but to be emebedded
///\date  2008/09/16 PZ  Rename as TrTrackR to be gbatch compatible
///\date  2008/10/23 PZ  Modifications for GBATCH compatibility
///\date  2008/11/05 PZ  New data format to be more compliant
///\date  2008/11/13 SH  Some updates for the new TrRecon
///\date  2008/11/20 SH  A new structure introduced
///$Date: 2009/02/01 15:58:33 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecHit.h"
#include "TrRecon.h"
#include "TrTrack.h"
#include "TrFit.h"
#include "MagField.h"
#include "TkDBc.h"
#include "point.h"
#include "tkdcards.h"
#include "TrPatt.h"
#include <cmath>

ClassImp(AMSTrTrack)

#ifdef __ROOTSHAREDLIBRARY__
#include "VCon_root.h"
  VCon* AMSTrTrack::vcon = new VCon_root();
#else  
#include "VCon_gbatch.h"
VCon* AMSTrTrack::vcon = new VCon_gb();
#endif

geant AMSTrTrack::_TimeLimit = 0;

int AMSTrTrack::NhitHalf     = 4;
//PZ FIXME int AMSTrTrack::DefaultFitID = AMSTrTrack::kChoutko;
int AMSTrTrack::DefaultFitID = AMSTrTrack::kSimple;

AMSTrTrack::AMSTrTrack():AMSlink(0,0), _Pattern(-1), _Nhits(0)
{
  _TrFit = 0;  
  AMSTrTrackPar par; 
  _TrackPar[DefaultFitID]=par;
  DBase[0] = DBase[1] = 0;
  _Status=0;
  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
  }

  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;

  DBase[0] = DBase[1] = 0;
}

AMSTrTrack::AMSTrTrack(int pattern, int nhits, AMSTrRecHit *phit[], int *imult)
  : _Pattern(pattern), _Nhits(nhits)
{
  int maskx = 0, masky = 0, maskc = 0;

  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit  && i < _Nhits) ? phit [i] :  0;
    _iMult[i] = (imult && i < _Nhits) ? imult[i] : -1;

    int ibit = trconst::maxlay-(i+1);
    if (phit && i < _Nhits) {
      if (phit[i]->GetXCluster()) _NhitsX++;  else maskx |= (1 << ibit);
      if (phit[i]->GetYCluster()) _NhitsY++;  else masky |= (1 << ibit);
      if (phit[i]->GetXCluster() && 
	  phit[i]->GetYCluster()) _NhitsXY++; else maskc |= (1 << ibit);
    }
  }

  _PatternX  = TrPatt::GetHitPatternIndex(maskx);
  _PatternY  = TrPatt::GetHitPatternIndex(masky);
  _PatternXY = TrPatt::GetHitPatternIndex(maskc);

  _TrFit = new TrFit();
  DBase[0] = DBase[1] = 0;
  _Status=0;
  BuildHitsIndex();
}

AMSTrTrack::AMSTrTrack(number theta, number phi, AMSPoint point)
  : AMSlink(0,0), _Pattern(-1), _Nhits(0)
{
  _Status=0;
  AMSDir dir(theta,phi);
  number rig = 1e7;
  number errig = 1e7;
  AMSTrTrackPar par; 
  _TrackPar[DefaultFitID]=par;
  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = point;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
  }

  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _TrFit = 0;
  DBase[0] = DBase[1] = 0;
}

AMSTrTrack::AMSTrTrack(AMSDir dir, AMSPoint point, number rig, number errig)
  : AMSlink(0,0), _Pattern(-1), _Nhits(0)
{
  _Status=0;
  AMSTrTrackPar par;
  _TrackPar[DefaultFitID]=par;
  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = point;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
  }

  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _TrFit = 0;
  DBase[0] = DBase[1] = 0;
}

AMSTrTrack::~AMSTrTrack()
{
  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] =  0;
    _iHits[i] = -1;
  }

  if(_TrFit) delete _TrFit;
  _TrFit=0;
}

AMSTrTrack::AMSTrTrack( const AMSTrTrack & orig):AMSlink(orig){

  if(!_TrackPar.empty())_TrackPar.clear();
  map<int, AMSTrTrackPar>::const_iterator aa=orig._TrackPar.begin();
  for(;aa!=orig._TrackPar.end();aa++)
    _TrackPar[aa->first]=aa->second;
  
  if(orig._TrFit)
    _TrFit= new TrFit(*(orig._TrFit));
  else
    _TrFit=0;

  _Status=orig._Status;
  for (int ii=0;ii<trconst::maxlay;ii++){
    _Hits[ii]  = orig._Hits[ii];
    _iHits[ii] = orig._iHits[ii];
    _iMult[ii] = orig._iMult[ii];
  }
  
  
  _Pattern= orig._Pattern;
  
  _Nhits= orig._Nhits;
  
  _PatternX= orig._PatternX;
  _PatternY= orig._PatternY;
  _PatternXY= orig._PatternXY;
  _NhitsX= orig._NhitsX;
  _NhitsY= orig._NhitsY;
  _NhitsXY= orig._NhitsXY;
  
  DBase[0]=orig.DBase[0];
  DBase[1]=orig.DBase[1];
}



AMSTrTrack& AMSTrTrack::operator=( const AMSTrTrack & orig){
  typedef map<int, AMSTrTrackPar>::const_iterator  myit; 
  _TrackPar.clear();
  for(myit aa=orig._TrackPar.begin();
      aa!=orig._TrackPar.end();aa++){
    _TrackPar[aa->first]=aa->second;

  }

  if(_TrFit&&orig._TrFit){
    delete _TrFit;
    _TrFit=new TrFit(*(orig._TrFit));
  }else if(!_TrFit&&orig._TrFit)
    _TrFit=new TrFit(*(orig._TrFit));
  else _TrFit=0;

  _Status=orig._Status;
  for (int ii=0;ii<trconst::maxlay;ii++){
    _Hits[ii]  = orig._Hits[ii];
    _iHits[ii] = orig._iHits[ii];
    _iMult[ii] = orig._iMult[ii];
  }
  
  
  _Pattern= orig._Pattern;
  
  _Nhits= orig._Nhits;
  
  _PatternX= orig._PatternX;
  _PatternY= orig._PatternY;
  _PatternXY= orig._PatternXY;
  _NhitsX= orig._NhitsX;
  _NhitsY= orig._NhitsY;
  _NhitsXY= orig._NhitsXY;
  
  DBase[0]=orig.DBase[0];
  DBase[1]=orig.DBase[1];
}



void AMSTrTrack::AddHit(AMSTrRecHit *hit, int imult)
{
  VCon* cont2=vcon->GetCont("AMSTrRecHit");
  if (_Nhits < trconst::maxlay) {
    _Hits [_Nhits] = hit;
    _iHits[_Nhits] = cont2->getindex(hit);
    _iMult[_Nhits] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
    _Nhits++;
    if (hit->GetXCluster()) _NhitsX++;
    if (hit->GetYCluster()) _NhitsY++;
    if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;
  }
  delete cont2;
}

void AMSTrTrack::BuildHitsIndex()
{
  VCon *cont2 = vcon->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++) {
    _iHits[i] = cont2->getindex(_Hits[i]);
  }
  delete cont2;
}

AMSPoint AMSTrTrack::GetPentry(int id)
{
  double zent = 0;

  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      double zh = hit->GetCoord().z();
      if (zent == 0 || zh > zent) zent = zh;
    }
  }

  // TO_BE_FIXED, interpolation to z=zent
  double xent = 0, yent = 0;

  return AMSPoint(xent, yent, zent);
}

AMSTrRecHit *AMSTrTrack::GetHit(int i) 
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = vcon->GetCont("AMSTrRecHit");
    _Hits[i] = (AMSTrRecHit*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return _Hits[i];
}

double AMSTrTrack::Fit(int id, int layer, const double *err, 
		       double mass, double chrg)
{
  if (id < 0) return -1;

  int idf = id&0xf;
  if (idf == kGEANE || idf == kGEANE_Kalman || idf == kChikanian) {
    cerr << "Error in TrTrack::Fit Fitting method not implemented: "
	 << idf << endl;
    return -1;
  }
  if (idf == kDummy) {
    cerr << "Warning in TrTrack::Fit Dummy fit is ignored " << endl;
    int aa=10/0;
    return -1;
  }

  // Force to use Linear fit if magnet is off
  if (MAGSFFKEY.magstat == 0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::NORMAL;
  if      (idf == kJuan)   method = TrFit::SIMPLE;  // TO_BE_UPDATED
  else if (idf == kLinear) method = TrFit::LINEAR;
  else if (idf == kCircle) method = TrFit::CIRCLE;
  else if (idf == kSimple) method = TrFit::SIMPLE;

  int update = 0;
  if (!_TrFit) {
    _TrFit = new TrFit();
    update = 1;
  }

  // Set multiple scattering option and assumed mass
  if (id & kMultScat) {
    TrFit::_ims = 1;
    _TrFit->SetMassChrg(mass, chrg);
  }

  // Sort hits in the ascending order of the layer number    : TO_BE_CHECKED
  int idx[trconst::maxlay], nhit = 0;
  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;
    idx[nhit++] = hit->GetLayer()*10+i;
  }
  std::sort(idx, &idx[nhit-1]);

  // For the half-fitting options    : TO_BE_CHECKED
  int i1 = 0, i2 = nhit;
  if (id & kUpperHalf) { i1 = 0;    i2 = std::min(nhit, NhitHalf); }
  if (id & kLowerHalf) { i2 = nhit; i1 = std::max(0, nhit-NhitHalf); }

  // Set fitting errors
  double errx = (err) ? err[0] : TRFITFFKEY.ErrX;
  double erry = (err) ? err[1] : TRFITFFKEY.ErrY;
  double errz = (err) ? err[2] : TRFITFFKEY.ErrZ;
  double zh0  = 0;
  int hitbits = 0;

  // Fill hit points
  _TrFit->Clear();
  for (int i = i1; i < i2; i++) {
    AMSTrRecHit *hit = GetHit(idx[i]%10);
    AMSPoint coo = (_iMult[i] >= 0) ? hit->GetCoord(_iMult[i])
      : hit->GetCoord();
    _TrFit->Add(coo, (hit->GetStatus() & YONLY) ? 0 : errx,
	       (hit->GetStatus() & XONLY) ? 0 : erry, errz);
    hitbits |= (1 << trconst::maxlay-hit->GetLayer());
    if (id != kLinear && i == 0) zh0 = coo.z();
  }

  // Perform fitting
  if (_TrFit->Fit(method) < 0) {
    return -1;
  }

  // Return if fitting values are not to be over written
  if (!update) return _TrFit->GetChisq();



  // Fill fittng parameters
  double p1 = _TrFit->GetParam(1);
  double p3 = _TrFit->GetParam(3);
  _TrackPar[id].FitDone  = true;
  _TrackPar[id].HitBits  = hitbits;
  _TrackPar[id].ChisqX   = _TrFit->GetChisqX();
  _TrackPar[id].ChisqY   = _TrFit->GetChisqY();
  _TrackPar[id].NdofX    = _TrFit->GetNdofX();
  _TrackPar[id].NdofY    = _TrFit->GetNdofY();
  _TrackPar[id].Chisq    = _TrFit->GetChisq();
  _TrackPar[id].Rigidity = _TrFit->GetRigidity();
  _TrackPar[id].ErrRinv  = _TrFit->GetErrRinv();

  // Interpolate to Z=0   : TO_BE_CHECKED
  AMSPoint pnt(0, 0,  0);
  AMSDir   dir(0, 0, -1);
  _TrFit->Interpolate(pnt, dir);
  _TrackPar[id].P0  = pnt;
  _TrackPar[id].Dir = dir;

  return GetChisq(id);
}

void AMSTrTrack::FillResidual()
{
  if (!_TrFit) return;

  // Fill fitting residual
  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit) continue;
    hit->SetResidual(AMSPoint(_TrFit->GetXr(i), _TrFit->GetYr(i), 0));
  }
}

void AMSTrTrack::print(void)
{
  //  std::cout << "AMSContainer:AMSTrTrack 0 Elements: " 
  // 	    << _event->NTrTracks << std::endl;
  //   for (int i = 0; i < _event->NTrTracks; i++)
  //     _event->GetTrTrack(i)->_printEl();
}

void AMSTrTrack::myprint(void)
{
  cout<<"==================================================\n";
  cout << " Pattern " << _Pattern 
  << " PatternX " << _PatternX 
  << " PatternY " << _PatternY 
  << " PatternXY " << _PatternXY<<endl; 
  

  cout  <<" NHits "<<_Nhits
    <<" NHitsX "<<_NhitsX
    <<" NHitsY "<<_NhitsY
    <<" NHitsXY "<<_NhitsXY<<endl;
  cout <<" DBase[0] "<<DBase[0]<<" DBase[1] "<<DBase[1]<<endl;
  for (int ii=0;ii<trconst::maxlay;ii++)
    cout <<" Hit: "<<ii<<" Point: "<<_Hits[ii]<<" Ind: "
	 <<_iHits[ii]<<" Imult: "<<_iMult[ii]<<endl;
  

  cout << "Npars= " << _TrackPar.size() << endl;
  for (map<int, AMSTrTrackPar>::iterator it = _TrackPar.begin();
       it != _TrackPar.end(); it++){
    cout <<"Fit Type "<< it->first << " " << endl;
    it->second.Print();
  }
  /*
    printf("Tracks  \n");
    for (int ii=0;ii<GetNhits();ii++)
    printf(" %d pointer %X  index: %d\n ",ii,(unsigned int)_Hits[ii],_iHits[ii]);
  */
}

void AMSTrTrack::_printEl(std::ostream& ost )
{
  /// FastFit is assumed as normal (Choutko) fit
  int id = kChoutko;
  ost << " Pattern " << _Pattern
      << " Rigidity (no MS)" << GetRigidity(id)
      << " Rigidity (Fast) " << GetRigidity(id+kMultScat)
      << " Chi2Fast "  << GetChisq(id)
      << " ThetaFast " << GetTheta(id)
      << " PhiFast "   << GetPhi  (id) << std::endl;
}

int AMSTrTrack::DoAdvancedFit()
{
  /*! "Advanced fit" is assumed to perform all the fitting: 
   *   1: 1st half, 2: 2nd half, 6: with nodb, 4: "Fast fit" ims=0 and 
   *   5: Juan with ims=1 */
  return 0;
}



bool AMSTrTrack::getres(int layer,AMSPoint &pnt,int id){
  if (!FitDone(id)) return false;
  int hitindex=-1;
  for (int ii=0;ii<trconst::nlays;ii++)
    if(_Hits[ii]->GetLayer()==layer)
      {  hitindex=ii; break;}
  if( hitindex<0) return false;
  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  AMSDir  dir(0,0,-1);
  AMSPoint  pnt2(0,0,_Hits[hitindex]->GetCoord()[2]);
  tprop.Interpolate(pnt2, dir);
  pnt=pnt2;
  return true;
}


bool AMSTrTrack::interpolateToZ(double Zint,AMSPoint &pnt,int id){
  if (!FitDone(id)) return false;
  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  AMSDir  dir(0,0,-1);
  AMSPoint  pnt2(0,0,Zint);
  tprop.Interpolate(pnt2, dir);
  pnt=pnt2;
  return true;
  
}
  

void AMSTrTrack::interpolate(AMSPoint pnt, AMSDir dir, AMSPoint &P1, 
			     number &theta, number &phi, number &length, 
			     int id)
{
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  length = tprop.Interpolate(pnt, dir);
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();
}


#include <strstream>

void AMSTrTrack::_printEl(std::string& sout)
{
  char cc[1024];
  strstream ost(cc, 1024);
  _printEl(ost);
  sout.append(cc);
}

#include "amsgobj.h"

AMSTrTrackError::AMSTrTrackError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
  AMSgObj::BookTimer.stop("TrTrack",1);
  AMSgObj::BookTimer.stop("RETKEVENT",1);
  AMSgObj::BookTimer.stop("REAMSEVENT",1);
  //  AMSEvent::gethead()->seterror(2);
}
char * AMSTrTrackError::getmessage(){return msg;}

