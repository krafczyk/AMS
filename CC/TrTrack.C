// $Id: TrTrack.C,v 1.15 2009/12/17 16:11:11 shaino Exp $

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
///$Date: 2009/12/17 16:11:11 $
///
///$Revision: 1.15 $
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


geant TrTrackR::_TimeLimit = 0;

int TrTrackR::NhitHalf     = 4;
int TrTrackR::DefaultFitID = TrTrackR::kChoutko;

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


TrTrackPar &TrTrackR::GetPar(int id) {
    int id2= (id==0)? trdefaultfit: id;
    if(_MagFieldOn==0&& id2 !=kDummy) id2=kLinear;
    if (ParExists(id2)) return _TrackPar[id2];
    cerr << "Warning in TrTrackR::GetPar, Parameter not exists " 
         << id << endl;
    //    int aa=10/0; //for debug
    static TrTrackPar parerr;
    return parerr;
  }











void TrTrackR::AddHit(TrRecHitR *hit, int imult, AMSPoint *bfield)
{
  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  if (_Nhits < trconst::maxlay) {
    _Hits  [_Nhits] = hit;
    _iHits [_Nhits] = cont2->getindex(hit);
    _iMult [_Nhits] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
    _BField[_Nhits] = (bfield) ? *bfield : AMSPoint(0, 0, 0);
    if(_BField[_Nhits].norm() != 0) _MagFieldOn = 1;
    _Nhits++;
    if (hit->GetXCluster()) _NhitsX++;
    if (hit->GetYCluster()) _NhitsY++;
    if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;
  }
  delete cont2;
}

void TrTrackR::BuildHitsIndex()
{
  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++)
    _iHits[i] = cont2->getindex(_Hits[i]);

  delete cont2;
}

AMSPoint TrTrackR::GetPentry(int id)
{
  double zent = 0;

  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      double zh = hit->GetCoord().z();
      if (zent == 0 || zh > zent) zent = zh;
    }
  }


  AMSPoint pnt(0,0,0);
  AMSDir dir(0,0,0);
  Interpolate(zent,  pnt,  dir, id);
  return pnt;
}

AMSDir TrTrackR::GetPdir(int id)
{
  double zent = 0;

  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      double zh = hit->GetCoord().z();
      if (zent == 0 || zh > zent) zent = zh;
    }
  }


  AMSPoint pnt(0,0,0);
  AMSDir dir(0,0,0);
  Interpolate(zent,  pnt,  dir, id);
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
  if (_MagFieldOn==0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::CHOUTKO;
  if      (idf == kAlcaraz) method = TrFit::ALCARAZ;
  else if (idf == kLinear)  method = TrFit::LINEAR;
  else if (idf == kCircle)  method = TrFit::CIRCLE;
  else if (idf == kSimple)  method = TrFit::SIMPLE;

  // Set multiple scattering option and assumed mass
  if (id & kMultScat) {
    TrFit::_mscat = 1;
    _TrFit.SetMassChrg(mass, chrg);
  }

  // Sort hits in the ascending order of the layer number
  int idx[trconst::maxlay], nhit = 0;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;
    idx[nhit++] = hit->GetLayer()*10+i;
  }
  sort(idx, &idx[nhit-1]);

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
  _TrFit.Clear();
  for (int i = i1; i < i2; i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);
    AMSPoint coo = (_iMult[j] >= 0) ? hit->GetCoord(_iMult[j])
                                    : hit->GetCoord();
    _TrFit.Add(coo, hit->OnlyY() ? 0 : errx,
	            hit->OnlyX() ? 0 : erry, errz);
    hitbits |= (1 << (trconst::maxlay-hit->GetLayer()));
    if (id != kLinear && j == 0) zh0 = coo.z();
  }

  // Perform fitting
  int rsul=_TrFit.Fit(method);
  //    if (rsul < 0) return -1;

  // Return if fitting values are not to be over written
  if (!update) return _TrFit.GetChisq();

  // Fill fittng parameters
  TrTrackPar &par = _TrackPar[id];
  par.FitDone  = (rsul>=0);
  par.HitBits  = hitbits;
  par.ChisqX   = _TrFit.GetChisqX();
  par.ChisqY   = _TrFit.GetChisqY();
  par.NdofX    = _TrFit.GetNdofX();
  par.NdofY    = _TrFit.GetNdofY();
  par.Chisq    = _TrFit.GetChisq();
  par.Rigidity = _TrFit.GetRigidity();
  par.ErrRinv  = _TrFit.GetErrRinv();

  for (int i = 0, j = 0; i < trconst::maxlay; i++) {
    if (hitbits & (1 << (trconst::maxlay-(i+1)))) {
      par.Residual[i].setp(_TrFit.GetXr(j), _TrFit.GetYr(j), _TrFit.GetZr(j));
      j++;
    }
    else par.Residual[i].setp(0, 0, 0);
  }

  // Interpolate to Z=0
  AMSPoint pnt(0, 0, 0);
  AMSDir   dir(0, 0, 1);
  _TrFit.Interpolate(pnt, dir);
  par.P0  = pnt;
  par.Dir = dir;

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
                               AMSDir &dir, Int_t id2)
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  dir.setp(0, 0,   1);
  pnt.setp(0, 0, zpl);
  return tprop.Interpolate(pnt, dir);
}

void TrTrackR::Interpolate(int nz, double *zpl, 
                             AMSPoint *pvec, AMSDir *dvec, double *lvec,
                             Int_t id2)
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

  TkSens tks(pnt);
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

int TrTrackR::DoAdvancedFit()
{
  /*! "Advanced fit" is assumed to perform all the fitting: 
   *   1: 1st half, 2: 2nd half, 6: with nodb, 4: "Fast fit" ims=0 and 
   *   5: Juan with ims=1 */

  Fit(kChoutko | kUpperHalf);
  Fit(kChoutko | kLowerHalf);
  Fit(kChoutko);
  Fit(kAlcaraz);
  trdefaultfit=kChoutko;
  return AdvancedFitDone();
}

int TrTrackR::AdvancedFitDone() { 
  int out= FitDone(kChoutko|kUpperHalf) & FitDone(kChoutko|kLowerHalf) & 
    FitDone(kChoutko) & FitDone(kAlcaraz);
  if(_MagFieldOn==0) return FitDone(kLinear);
  else return out;
}


