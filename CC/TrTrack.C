// $Id: TrTrack.C,v 1.34 2010/06/09 15:49:10 pzuccon Exp $

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
///$Date: 2010/06/09 15:49:10 $
///
///$Revision: 1.34 $
///
//////////////////////////////////////////////////////////////////////////


#include "MagField.h"
#include "TkDBc.h"
#include "point.h"
#include "tkdcards.h"
#include "VCon.h"

#include "TrTrack.h"
#include "TrRecHit.h"
#include "TkSens.h"

#include <cmath>
#include <algorithm>


ClassImp(TrTrackPar)

void TrTrackPar::Print(int full){
  printf("Rigidity:  %f Err(1/R):  %f P0: %f %f %f  Dir:  %f %f %f\n",
	 Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]);
  if(!full)return;
  printf("HitBits: %06d, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
	 HitBits,ChisqX,NdofX,ChisqY,NdofY,Chisq);
}
void  TrTrackPar::Print_stream(std::string &ostr,int full){
  ostr.append(Form("Rigidity:  %f Err(1/R):  %f P0: %f %f %f  Dir:  %f %f %f\n",
		   Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]));
  if(!full)return;
  ostr.append(Form("HitBits: %06d, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
		   HitBits,ChisqX,NdofX,ChisqY,NdofY,Chisq));
  return ;
}




ClassImp(TrTrackR);


int TrTrackR::NhitHalf     = 4;
int TrTrackR::DefaultFitID = TrTrackR::kChoutko;

const int TrTrackR::DefaultAdvancedFitFlags[DEF_ADVFIT_NUM]=
  { kChoutko, kChoutko|kMultScat, kChoutko|kUpperHalf, kChoutko|kLowerHalf, 
    kAlcaraz, kAlcaraz|kMultScat, kAlcaraz|kUpperHalf, kAlcaraz|kLowerHalf 
  };

TrTrackR::TrTrackR(): _Pattern(-1), _Nhits(0)
{
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iMult[i] = -1;
    _BField[i]=AMSPoint(0,0,0);
  }
  trdefaultfit=0;
  _MagFieldOn=0;
  _PatternX=0;
  _PatternY=0;
  _PatternXY=0;
  _NhitsX=0;
  _NhitsY=0;
  _NhitsXY=0;
  DBase[0]=0;
  DBase[1]=0;
  Status=0;
//   if(TkDBc::Head && TkDBc::Head->GetSetup()==3)
//     DefaultAdvancedFitFlags = TrTrackR::kChoutkoDef | 
//       TrTrackR::kAlcarazFit|TrTrackR::kLayer89All;
}

TrTrackR::TrTrackR(int pattern, int nhits, TrRecHitR *phit[],AMSPoint bfield[], int *imult,int fitmethod)
  :_Pattern(pattern), _Nhits(nhits), _NhitsX(0), _NhitsY(0), _NhitsXY(0)

{
  _MagFieldOn=0;
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit   && i < _Nhits) ? phit [i] :  0;
    _iMult[i] = (imult  && i < _Nhits) ? imult[i] : -1;
    _BField[i]= (bfield && i < _Nhits) ? bfield[i]: AMSPoint(0,0,0);
    if(_BField[i].norm()!=0) _MagFieldOn=1;
    if (phit && i < _Nhits) {
      if (phit[i]->GetXCluster()) _NhitsX++;
      if (phit[i]->GetYCluster()) _NhitsY++;
      if (phit[i]->GetXCluster() && phit[i]->GetYCluster()) _NhitsXY++;
    }
  }

  // These patterns should be filled with SetPatterns()
  _PatternX = _PatternY = _PatternXY = 0;

  DBase[0] = DBase[1] = 0;
  Status=0;
  BuildHitsIndex();
  trdefaultfit=fitmethod;
  if(trdefaultfit==0) trdefaultfit=DefaultFitID;
//   if(TkDBc::Head->GetSetup()==3)
//     DefaultAdvancedFitFlags = TrTrackR::kChoutkoDef | 
//       TrTrackR::kAlcarazFit|TrTrackR::kLayer89All;
}

TrTrackR::TrTrackR(number theta, number phi, AMSPoint point)
  : _Pattern(-1), _Nhits(0)
{
  trdefaultfit=kDummy;
  TrTrackPar &par = _TrackPar[trdefaultfit];
  par.FitDone = true;
  par.P0      = point;
  par.Dir     = AMSDir(theta,phi); 
  par.Rigidity=1e6;
  par.ErrRinv  = 1e7;
  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i] = 0;
  }
  Status=0;
  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;
  
}

TrTrackR::TrTrackR(AMSDir dir, AMSPoint point, number rig, number errig)
  : _Pattern(-1), _Nhits(0)
{
  trdefaultfit=kDummy;
  TrTrackPar &par = _TrackPar[trdefaultfit];
  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = point;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i] = 0;
  }
  Status=0;
  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;

}

TrTrackR::~TrTrackR()
{
}


TrTrackPar &TrTrackR::GetPar(int id)
{
  int id2 = (id == 0) ? trdefaultfit : id;
  if (_MagFieldOn == 0 && id2 != kDummy) id2 = kLinear;
  if (ParExists(id2)) return _TrackPar[id2];
  cerr << "Warning in TrTrackR::GetPar, Parameter not exists " 
       << id << endl;
  static TrTrackPar parerr;
  return parerr;
}

double TrTrackR::GetNormChisqX(int id)
{
  Double_t enorm = TRFITFFKEY.ErrX/50e-4;
  return (GetNdofX(id) > 0) ? GetChisqX(id)/GetNdofX(id)*enorm*enorm : 0;
}

double TrTrackR::GetNormChisqY(int id)
{
  Double_t enorm = TRFITFFKEY.ErrY/30e-4;
  return (GetNdofY(id) > 0) ? GetChisqY(id)/GetNdofY(id)*enorm*enorm : 0;
}

AMSPoint TrTrackR::GetPlayer(int ilay, int id)
{
  AMSPoint pres = GetResidual(ilay, id);

  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayer() == ilay+1) {
      AMSPoint coo = (_iMult[i] >= 0) ? hit->GetCoord(_iMult[i])
                                      : hit->GetCoord();
      return coo-pres;
    }
  }
  return pres;
}

void TrTrackR::AddHit(TrRecHitR *hit, int imult)
{
  int ihit = -1;
  for (int i = 0; i < _Nhits; i++)
    if (GetHit(i)->GetLayer() == hit->GetLayer()) ihit = i;

  if (ihit < 0) {
    if (_Nhits >= trconst::maxlay) {
      cerr << "Error in TrTrack::AddHit the hit vector is already full"
	   << endl;
      return;
    }
    ihit = _Nhits;
    _Nhits++;
  }
  else {
    TrRecHitR *hh = GetHit(ihit);
    if (hh->GetXCluster()) _NhitsX--;
    if (hh->GetYCluster()) _NhitsY--;
    if (hh->GetXCluster() && hh->GetYCluster()) _NhitsXY--;
//  hh->clearstatus(AMSDBc::USED);
  }
//hit->setstatus(AMSDBc::USED);

  if (hit->GetXCluster()) _NhitsX++;
  if (hit->GetYCluster()) _NhitsY++;
  if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;

  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  _Hits [ihit] = hit;
  _iHits[ihit] = cont2->getindex(hit);
  _iMult[ihit] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
  delete cont2;

  if (MagFieldOn()){
    float coo[3], bf[3];
    hit->GetCoord(_iMult[ihit]).getp(coo);
    GUFLD(coo, bf);
    _BField[ihit] = AMSPoint(bf);
    _MagFieldOn = 1;
  }
  else {
    _BField[ihit] = AMSPoint(0, 0, 0);
    _MagFieldOn = 0;
  }
}

void TrTrackR::BuildHitsIndex()
{
  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++)
    _iHits[i] = cont2->getindex(_Hits[i]);

  delete cont2;
}


void TrTrackR::GetMaxShift(int& left, int& right){
  left=99; right=99;
  for(int ii=0;ii<_Nhits;ii++){
    int tkid=_Hits[ii]->GetTkId();
    int mult=_Hits[ii]->GetMultiplicity();
    int ll=0; int rr=0;
    if(tkid>=0){ll=mult-_iMult[ii]; rr=_iMult[ii];}
    else {rr=mult-_iMult[ii]; ll=_iMult[ii];}
    if(ll<left)  left=ll;
    if(rr<right) right=rr;
  }
  left*=-1;
  return;
}

void TrTrackR::Move(int shift){
 for(int ii=0;ii<_Nhits;ii++){
    int tkid=_Hits[ii]->GetTkId();
    if(_Hits[ii]->OnlyY ()){
       int layer = std::abs(tkid)/100;
       float ln  = TkCoo::GetLadderLength (tkid);
       float lx  = TkCoo::GetLadderCenterX(tkid);
       float ly  = TkCoo::GetLadderCenterY(tkid);
       float lz  = TkDBc::Head->GetZlayer(layer);
       float previousX=_Hits[ii]->GetCoord()[0];
       if(tkid>0) previousX-=shift*TkDBc::Head->_SensorPitchK*2;
       else previousX+=shift*TkDBc::Head->_SensorPitchK*2;
       AMSPoint gcoo(previousX, ly, lz);
       TkSens tks(tkid, gcoo,0);
       if (tks.LadFound() && tks.GetStripX() < 0) {
	 float sx = tks.GetSensCoo().x();
	 float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->rot.GetEl(0, 0);
	 if (sx < 0) gcoo[0] -= sg*(sx-1e-3);
	 if (sx > TkDBc::Head->_ssize_active[0])
	   gcoo[0] -= sg*(sx-TkDBc::Head->_ssize_active[0]+1e-3);
	 tks.SetGlobalCoo(gcoo);
       }  
       _Hits[ii]->SetResolvedMultiplicity(tks.GetMultIndex());
       _Hits[ii]->SetDummyX(tks.GetStripX());
       _Hits[ii]->BuildCoordinates();
    }else{


      int mult=_Hits[ii]->GetMultiplicity();
      int nmult=_iMult[ii];
      if(tkid>=0) nmult-=shift;
      else nmult+=shift;
      if(nmult>=0 && nmult<=mult) {
	_iMult[ii]=nmult;
	_Hits[ii]->SetResolvedMultiplicity(nmult);
      }
      else cerr<<"TrTrackR::Move-E- Very Bad problem moving the Track\n";
    }
 }
 ReFit();
 return;
}



void TrTrackR::ReFit( const float *err,
		      float mass, float chrg){
  map<int, TrTrackPar>::iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++)
    Fit(it->first,-1,1,err,mass,chrg);
  return;
}


AMSPoint TrTrackR::GetPentry(int id)
{
  int ilay = 7;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      ilay = hit->GetLayer()-1;
      break;
    }
  }

  return InterpolateLayer(ilay);
}

AMSDir TrTrackR::GetPdir(int id)
{
  int ilay = 8;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      ilay = hit->GetLayer()-1;
      break;
    }
  }

  AMSPoint pnt;
  AMSDir   dir;
  InterpolateLayer(ilay, pnt, dir, id);
  return dir;
}

TrRecHitR *TrTrackR::GetHit(int i) 
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    _Hits[i] = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return _Hits[i];
}

TrRecHitR *TrTrackR::GetHitL(int ilay)
{
  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayer() == ilay+1) return hit;
  }
  return 0;
}

TrRecHitR & TrTrackR::TrRecHit(int i) 
{
  if (i < 0 || trconst::maxlay <= i) return *_Hits[0];
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    _Hits[i] = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return (*_Hits[i]);
}

float TrTrackR::Fit(int id2, int layer, bool update, const float *err, 
                      float mass, float chrg)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (id < 0) return -1;

  int idf = id&0xf;
  if (idf == kGEANE || idf == kGEANE_Kalman) {
    cerr << "Error in TrTrack::Fit Fitting method not implemented: "
         << idf << endl;
    return -1;
  }
  if (idf == kDummy) {
    cerr << "Warning in TrTrack::Fit Dummy fit is ignored " << endl;
    return -1;
  }

  // Force to use Linear fit if magnet is off
  if (_MagFieldOn==0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::CHOUTKO;
  if      (idf == kAlcaraz)   method = TrFit::ALCARAZ;
  else if (idf == kChikanian) method = TrFit::CHIKANIAN;
  else if (idf == kLinear)    method = TrFit::LINEAR;
  else if (idf == kCircle)    method = TrFit::CIRCLE;
  else if (idf == kSimple)    method = TrFit::SIMPLE;

  // Set multiple scattering option and assumed mass
  if (id & kMultScat) {
    TrFit::_mscat = 1;
    _TrFit.SetMassChrg(mass, chrg);
  }

  // One drop fitting
  if (id & kOneDrop) {
    double csymin = 0;
    int    ihmin  = -1;
    for (int i = 0; i < _Nhits; i++) {
      TrRecHitR *hit = GetHit(i);
      if (hit && Fit(idf, hit->GetLayer(), false) > 0 && 
	  _TrFit.GetChisqY() > 0) {
	if (ihmin < 0 || _TrFit.GetChisqY() < csymin) {
	  ihmin  = i;
	  csymin = _TrFit.GetChisqY();
	}
      }
    }
    if (ihmin < 0) return -1;
    layer = GetHit(ihmin)->GetLayer();
  }

  // Noise drop fitting
  if (id & kNoiseDrop) {
    double snmin = 0;
    int    ihmin = -1;
    for (int i = 0; i < _Nhits; i++) {
      TrRecHitR *hit = GetHit(i);
      if (hit && hit->GetYCluster()) {
	if (ihmin < 0 || hit->GetYCluster()->GetSeedSN() < snmin) {
	  ihmin = i;
	  snmin = hit->GetYCluster()->GetSeedSN();
	}
      }
    }
    if (ihmin < 0) return -1;
    layer = GetHit(ihmin)->GetLayer();
  }

  // Sort hits in the ascending order of the layer number
  int idx[trconst::maxlay], nhit = 0;
  int bhit[2] = { 0, 0 };
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;

    // For AMS02P (AKA AMS-B)
    if (TkDBc::Head->GetSetup() == 3) {
      if (hit->GetLayer() == 8) { 
	if (!(id & kFitLayer8)) continue; 
	else bhit[0] = 1;
      }
      if (hit->GetLayer() == 9) {
	if (!(id & kFitLayer9)) continue;
	else bhit[1] = 1;
      }

      int lyr = hit->GetLayer();
      if (lyr == 8) lyr = 0;
      idx[nhit++] = lyr*10+i;
    }
    // For AMS02P (AKA AMS-B)

    // For AMS02-Ass1/PreInt with S.C. magnet
    else
      idx[nhit++] = hit->GetLayer()*10+i;
  }

  // AMS02P
  if (TkDBc::Head->GetSetup() == 3) {
    if ((id & kFitLayer8) && !bhit[0]) return -1;
    if ((id & kFitLayer9) && !bhit[1]) return -1;
  }
  // AMS02P

  std::sort(idx, &idx[nhit]);

  // For the half-fitting options
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
  _TrFit.Clear();
  for (int i = i1; i < i2; i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);
    AMSPoint coo = (_iMult[j] >= 0) ? hit->GetCoord(_iMult[j])
                                    : hit->GetCoord();
    double ery = erry;

    // For AMS02P (AKA AMS-B)
    if (TkDBc::Head->GetSetup() == 3) {
      int lyr = hit->GetLayer();
      if (lyr == 8 || lyr == 9) {
	double rpar = (lyr == 8) ? 100 : 0.70; //  1.00 : 0.70;
	ery = 1;
	for (int k = 0; k < 2; k++) {
	  int mfit = (k == 0) ? id : kChoutko;
	  if (ParExists(mfit)) {
	    double rini = std::fabs(GetRigidity(mfit));
	    if (rini > 0) { 
	      ery = (lyr == 8) ? rpar/rini/rini : rpar/rini;
	      if (ery < erry) ery = erry;
	      break;
	    }
	  }
	}
      }
    }
    // For AMS02P (AKA AMS-B)
    float bf[3]={0,0,0};
    float pp[3];
    pp[0]=coo[0];pp[1]=coo[1];pp[2]=coo[2];
    GUFLD(pp, bf);
    _TrFit.Add(coo, hit->OnlyY() ? 0 : errx,
                    hit->OnlyX() ? 0 : ery,  errz
	       ,bf[0],bf[1],bf[2]);
    
    hitbits |= (1 << (trconst::maxlay-hit->GetLayer()));
    if (id != kLinear && j == 0) zh0 = coo.z();
  }

  if (method == TrFit::CHIKANIAN && ParExists(kChoutko))
    _TrFit.SetRigidity(GetRigidity(kChoutko));

  // Perform fitting
  bool done = (_TrFit.Fit(method) > 0);

  // Return if fitting values are not to be over written
  if (!update) return _TrFit.GetChisq();

  // Fill fittng parameters
  TrTrackPar &par = _TrackPar[id];
  if (!done) return -1;

  par.FitDone  = true;
  par.HitBits  = hitbits;
  par.ChisqX   = _TrFit.GetChisqX();
  par.ChisqY   = _TrFit.GetChisqY();
  par.NdofX    = _TrFit.GetNdofX();
  par.NdofY    = _TrFit.GetNdofY();
  par.Chisq    = _TrFit.GetChisq();
  par.Rigidity = _TrFit.GetRigidity();
  par.ErrRinv  = _TrFit.GetErrRinv();

  // Interpolate to Z=0
  AMSPoint pnt(0, 0, 0);
  AMSDir   dir(0, 0, 1);
  _TrFit.Interpolate(pnt, dir);
  par.P0  = pnt;
  par.Dir = dir;

  // Fill residuals
  for (int i = 0, j = 0; i < trconst::maxlay; i++) {
    if (hitbits & (1 << (trconst::maxlay-(i+1)))) {
      par.Residual[i].setp(_TrFit.GetXr(j), _TrFit.GetYr(j), _TrFit.GetZr(j));
      j++;
    }
    else {
      TrRecHitR *hit = GetHitL(i);
      AMSPoint pint = InterpolateLayer(i, id);
      if (hit)
	par.Residual[i] = hit->GetCoord()-pint;
      else
	par.Residual[i] = pint;
    }
  }

  return GetChisq(id);
}

void TrTrackR::Print(int opt){
  _PrepareOutput(opt);
  cout <<sout;

}
char *  TrTrackR::Info(int iRef){
  string aa;
  aa.append(Form("TrTrack #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}
std::ostream &TrTrackR::putout(std::ostream &ostr)  {
  _PrepareOutput(1);

  return ostr << sout  << std::endl; 
    
}

void TrTrackR::_PrepareOutput(int full )
{
  sout.clear();
  sout.append(Form("NHits %d (x:%d,y:%d,xy:%d)Pattern: %d, MOn: %d DefFit: %d ",
		   GetNhits(),GetNhitsX(),GetNhitsY(),GetNhitsXY(),GetPattern(),
		   _MagFieldOn,trdefaultfit));
  TrTrackPar &bb=GetPar();
  bb.Print_stream(sout,full);
  if(!full) return;
  map<int, TrTrackPar>::iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++){
    sout.append(Form("\nFit mode %d ",it->first));
    it->second.Print_stream(sout,full);
  }
}
  



double TrTrackR::Interpolate(double zpl, AMSPoint &pnt, 
                               AMSDir &dir, int id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  dir.setp(0, 0,   1);
  pnt.setp(0, 0, zpl);
  return tprop.Interpolate(pnt, dir);
}

AMSPoint TrTrackR::InterpolateLayer(int ily, int id)
{
  AMSPoint pnt;
  AMSDir   dir;
  InterpolateLayer(ily, pnt, dir, id);
  return pnt;
}

double TrTrackR::InterpolateLayer(int ily, AMSPoint &pnt, 
				  AMSDir &dir, int id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  dir.setp(0, 0, 1);
  pnt.setp(0, 0, TkDBc::Head->GetZlayer(ily+1));

  double ret = tprop.Interpolate(pnt, dir);

  TkSens tks(pnt,0);
  if (!tks.LadFound()) return ret;

  TkLadder *lad = TkDBc::Head->FindTkId(tks.GetLadTkID());
  TkPlane  *pla = lad->_plane;
  AMSRotMat lrm = lad->GetRotMatA()*lad->GetRotMat();
  AMSRotMat prm = pla->GetRotMatA()*pla->GetRotMat();
  dir.setp(0, 0, 1);

  pnt = prm*(lad->GetPos()+lad->GetPosA())+pla->GetPosA()+pla->GetPos();
  dir = prm*lrm*dir;

  return tprop.Interpolate(pnt, dir);
}

void TrTrackR::Interpolate(int nz, double *zpl, 
                             AMSPoint *pvec, AMSDir *dvec, double *lvec,
                             int id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  tprop.Interpolate(nz, zpl, pvec, dvec, lvec);
}

void TrTrackR::interpolate(AMSPoint pnt, AMSDir dir, AMSPoint &P1, 
			   number &theta, number &phi, number &length, 
			   int id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  length = tprop.Interpolate(pnt, dir);
  //PZ bugfix lenght must be signed for beta calculation.
  if(pnt[2]>0)length*=-1;
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();
}

bool TrTrackR::interpolateCyl(AMSPoint pnt, AMSDir dir, number rad, 
                                number idir, AMSPoint &P1, number &theta, 
                                number &phi, number &length, int id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return false;

  int sdir = (idir < 0) ? -1 : 1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if ((length = tprop.InterpolateCyl(pnt, dir, rad, sdir)) < 0) return false;
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();

  return true;
}

int TrTrackR::intercept(AMSPoint &pnt, int layer, 
			number &theta, number &phi, number &local, int id)
{
  AMSDir dir;
  Interpolate(TkDBc::Head->GetZlayer(layer+1), pnt, dir, id);
  theta = dir.gettheta();
  phi   = dir.getphi();
  local = 0;

  TkSens tks(pnt,0);
  if (tks.LadFound()) {
    AMSPoint ps(TkDBc::Head->_ssize_active[0], 
		TkDBc::Head->_ssize_active[1], TkDBc::Head->_silicon_z);
    AMSPoint xloc = tks.GetSensCoo();
    xloc = (xloc/ps).abs();
    if (xloc[0] > xloc[1]) local = 1-xloc[0];
    else                   local = 1-xloc[1];
  }
  else return -1;

  return 1;
}

void TrTrackR::getParFastFit(number& Chi2,  number& Rig, number& Err, 
		   number& Theta, number& Phi, AMSPoint& X0) {
    /// FastFit is assumed as normal (Choutko) fit without MS
  int id = kChoutko;
  if(_MagFieldOn==0) id=kLinear;
  if(trdefaultfit==kDummy) id=kDummy;
  Chi2 = GetChisq(id); Rig = GetRigidity(id); Err = GetErrRinv(id); 
  Theta = GetTheta(id); Phi = GetPhi(id); X0 = GetP0(id);
}

int TrTrackR::DoAdvancedFit(int add_flag)
{
 if (!_MagFieldOn) return Fit(kLinear|add_flag);
 for(int ii=0;ii<DEF_ADVFIT_NUM;ii++)
   Fit(DefaultAdvancedFitFlags[ii]| add_flag);

 return AdvancedFitDone(add_flag);
}

int TrTrackR::AdvancedFitDone(int add_flag)
{ 
  if (!_MagFieldOn) return FitDone(kLinear|add_flag);
  bool done = true;
  for(int ii=0;ii<DEF_ADVFIT_NUM;ii++)
    done &=FitDone(DefaultAdvancedFitFlags[ii]| add_flag);
  return done;

}


char * TrTrackR::GetFitNameFromID(int fitnum){
  static char out[200];
  out[0]='\0';
  int basefit= fitnum & 0x000F;
  if(basefit == -1)  strcat(out,"kDummy");
  if(basefit ==  1)  strcat(out,"kChoutko");
  if(basefit ==  2)  strcat(out,"kGEANE");
  if(basefit ==  3)  strcat(out,"kGEANE_Kalman");
  if(basefit ==  4)  strcat(out,"kAlcaraz");
  if(basefit ==  5)  strcat(out,"kChikanian");
  if(basefit == 10)  strcat(out,"kLinear");
  if(basefit == 11)  strcat(out,"kCircle");
  if(basefit == 12)  strcat(out,"kSimple");
  if(fitnum & 0x10)  strcat(out,"kMultScat");
  if(fitnum & 0x20)  strcat(out,"kUpperHalf");
  if(fitnum & 0x40)  strcat(out,"kLowerHalf");
  if(fitnum & 0x100)  strcat(out,"kOneDrop");
  if(fitnum & 0x200)  strcat(out,"kNoiseDrop");
  if(fitnum & 0x400)  strcat(out,"kFitLayer8");
  if(fitnum & 0x800)  strcat(out,"kFitLayer9");
  return out;
}

int TrTrackR::GetFitID(int pos){
  if(pos >= _TrackPar.size()) return 0;
  int count=0;
  map<int, TrTrackPar>::iterator it;
  
  for(it=_TrackPar.begin();it!=_TrackPar.end();it++)
    if(count++==pos) return it->first;
    
  return 0;

}
void TrTrackR::PrintFitNames(){
  map<int, TrTrackPar>::iterator it;
  for(it=_TrackPar.begin();it!=_TrackPar.end();it++)
    printf("%s\n", GetFitNameFromID(it->first));
}
