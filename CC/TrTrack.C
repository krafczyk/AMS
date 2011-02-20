// $Id: TrTrack.C,v 1.93 2011/02/20 14:30:11 pzuccon Exp $

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
///$Date: 2011/02/20 14:30:11 $
///
///$Revision: 1.93 $
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
#include "TrRecon.h"
#include <cmath>
#include <algorithm>

int my_int_pow(int base, int exp){
  if (exp<0) return -1;
  if (base==0) return 0;
  int ss=1;
  if (base<0 && exp%2==1)ss=-1;
  int bb=abs(base);
  int out=1;
  for(int ii=0;ii<exp;ii++)
    out*=bb;
  return out*ss;
}


ClassImp(TrTrackPar)

void TrTrackPar::Print(int full) const {
  printf("Rigidity:  %6.3f Err(1/R):  %7.5f P0: %6.3f %6.3f %6.3f  Dir:  %6.4f %6.4f %6.4f\n",
	 Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]);
  if(!full)return;
  printf("HitBits: %s, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
	 TrTrackR::HitBitsString(HitBits),ChisqX,NdofX,ChisqY,NdofY,Chisq);
}

void  TrTrackPar::Print_stream(std::string &ostr,int full) const {
  ostr.append(Form("Rigidity:  %6.3f Err(1/R):  %7.5f P0: %6.3f %6.3f %6.3f  Dir:  %6.4f %6.4f %6.4f\n",
		   Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]));
  if(!full)return;
  ostr.append(Form("HitBits: %06d, Chi2X/Ndf: %f/%d, Chi2Y/Ndf: %f/%d, Chi2: %f \n",
		   HitBits,ChisqX,NdofX,ChisqY,NdofY,Chisq));
  return ;
}




ClassImp(TrTrackR);


int   TrTrackR::NhitHalf      = 4;
int   TrTrackR::DefaultFitID  = TrTrackR::kChoutko | TrTrackR::kMultScat;
float TrTrackR::DefaultMass   = 0.938272297;
float TrTrackR::DefaultCharge = 1;

const int TrTrackR::DefaultAdvancedFitFlags[DEF_ADVFIT_NUM]=
  { kChoutko, kChoutko|kMultScat, 
    kAlcaraz, kAlcaraz|kMultScat,
    kChikanian, kChikanianF, 0, 0 };

int TrTrackR::AdvancedFitBits = 0x0f;

TrTrackR::TrTrackR(): _Pattern(-1), _Nhits(0)
{
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    //    _iMult[i] = -1;
    //    _BField[i]=AMSPoint(0,0,0);
  }
  trdefaultfit=0;
  _MagFieldOn=0;
  _bit_pattern=0;
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
  _bit_pattern=0;
  _MagFieldOn=0;
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit   && i < _Nhits) ? phit [i] :  0;
    //    _iMult[i] = (imult  && i < _Nhits) ? imult[i] : -1;

    if (imult  && i < _Nhits && phit[i] ) 
      phit[i]->SetResolvedMultiplicity(imult[i]);

    //    _BField[i]= (bfield && i < _Nhits) ? bfield[i]: AMSPoint(0,0,0);
    if(bfield && i < _Nhits && bfield[i].norm()!=0) _MagFieldOn=1;
    if (phit && i < _Nhits) {
      _bit_pattern|= 1<<(phit[i]->GetLayer()-1);
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
  AMSDir dir(theta,phi);
  AMSPoint p0;
  p0[0]=point[0]-dir[0]/dir[2]*point[2];
  p0[1]=point[1]-dir[1]/dir[2]*point[2];
  p0[2]=0;

  par.FitDone = true;
  par.P0      = p0;
  par.Dir     = AMSDir(theta,phi); 
  par.Rigidity=1e6;
  par.ErrRinv  = 1e7;
  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = -1; //_iMult[i] = -1;
    //    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i][0] = 0;
    par.Residual[i][1] = 0;
  }
  Status=0;
  _bit_pattern=_PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;
  
}

TrTrackR::TrTrackR(AMSDir dir, AMSPoint point, number rig, number errig)
  : _Pattern(-1), _Nhits(0)
{
  trdefaultfit=kDummy;
  TrTrackPar &par = _TrackPar[trdefaultfit];
  
  AMSPoint p0;
  p0[0]=point[0]-dir[0]/dir[2]*point[2];
  p0[1]=point[1]-dir[1]/dir[2]*point[2];
  p0[2]=0;

  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = p0;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = -1; //_iMult[i] = -1;
    //    _BField[i]= AMSPoint(0,0,0);
    par.Residual[i][0] = 0;
    par.Residual[i][1] = 0;
  }
  Status=0;
  _bit_pattern=_PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  _MagFieldOn=0;
  DBase[0] = DBase[1] = 0;

}

TrTrackR::TrTrackR(const TrTrackR& orig){
  for (int ii=0;ii<trconst::maxlay;ii++){
    _Hits[ii]=orig._Hits[ii] ;
    //    _BField[ii]=orig._BField[ii] ; 
    _iHits[ii]=orig._iHits[ii] ;
    //    _iMult[ii]=orig._iMult[ii] ;
  }
  _bit_pattern=orig._bit_pattern ;
  _Pattern=orig._Pattern ;
  _Nhits=orig._Nhits ;
  _PatternX=orig._PatternX ;
  _PatternY=orig._PatternY ;
  _PatternXY=orig._PatternXY ;
  _NhitsX=orig._NhitsX ;
  _NhitsY=orig._NhitsY ;
  _NhitsXY=orig._NhitsXY ;
  _TrFit=orig._TrFit ;  //!
  _TrackPar=orig._TrackPar;
  _MagFieldOn=orig._MagFieldOn ;
  for(int ii=0;ii<2;ii++)
    DBase[ii]=orig.DBase[ii] ;
  trdefaultfit=orig.trdefaultfit ;
  Status=orig.Status ;

}

TrTrackR::~TrTrackR()
{
}


const TrTrackPar &TrTrackR::GetPar(int id) const
{
  int id2 = (id == 0) ? trdefaultfit : id;
  if (_MagFieldOn == 0 && id2 != kDummy) id2 = kLinear;
  if (ParExists(id2)) return _TrackPar.find(id2)->second;
  static int i=0;
   if(i++<100)cerr << "Warning in TrTrackR::GetPar, Parameter not exists " 
       << id << " "  << endl;
  static TrTrackPar parerr;
  return parerr;
}

TrTrackPar &TrTrackR::GetPar(int id)
{
  int id2 = (id == 0) ? trdefaultfit : id;
  if (ParExists(id2)) return _TrackPar[id2]; // Be careful of [] operator !
  static TrTrackPar parerr;
  return parerr;
}


double TrTrackR::GetNormChisqX(int id)
{
  double enorm = 1;
  return (GetNdofX(id) > 0) ? GetChisqX(id)/GetNdofX(id)*enorm*enorm : 0;
}

double TrTrackR::GetNormChisqY(int id)
{
  double enorm = 1;
  return (GetNdofY(id) > 0) ? GetChisqY(id)/GetNdofY(id)*enorm*enorm : 0;
}

AMSPoint TrTrackR::GetPlayer(int ilay, int id)
{
  AMSPoint pres = GetResidual(ilay, id);

  for (int i = 0; i < GetNhits(); i++) {
    TrRecHitR *hit = GetHit(i);
    if (hit && hit->GetLayer() == ilay+1) {
      //      AMSPoint coo = (_iMult[i] >= 0) ? hit->GetCoord(_iMult[i])
      //                              : hit->GetCoord();
      AMSPoint coo = hit->GetCoord();
      return coo-pres;
    }
  }
  return pres;
}

void TrTrackR::AddHit(TrRecHitR *hit, int imult)
{
  int ihit = -1;
  for (int i = 0; i < _Nhits; i++){
    TrRecHitR *hh = GetHit(i);
    if (hh && hh->GetLayer() == hit->GetLayer()) ihit = i;
  }
  if (ihit < 0) {
    if (_Nhits >= trconst::maxlay) {
      cerr << "Error in TrTrack:AddHit the hit vector is already full"
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
  }

  if (hit->GetXCluster()) _NhitsX++;
  if (hit->GetYCluster()) _NhitsY++;
  if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;
  _bit_pattern|= (1<<(hit->GetLayer()-1));

  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  _Hits [ihit] = hit;
  _iHits[ihit] = cont2->getindex(hit);
  //  _iMult[ihit] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
  delete cont2;

  if (MagFieldOn()){
    float coo[3], bf[3];
    coo[0]=hit->GetCoord().x();
    coo[1]=hit->GetCoord().y();
    coo[2]=hit->GetCoord().z();
    //GUFLD(coo, bf);
    //_BField[ihit] = AMSPoint(bf);
    _MagFieldOn = 1;
  }
  else {
    //    _BField[ihit] = AMSPoint(0, 0, 0);
    _MagFieldOn = 0;
  }
}


bool TrTrackR::RemoveHitOnLayer( int layer){
  if(layer <1 || layer >9) return false;
  int idx=-1;
  TrRecHitR* phit=0;
  for (int ii=0; ii< _Nhits; ii++){
    TrRecHitR* phit2=pTrRecHit(ii);
    if( phit2->GetLayer() == layer)
      {idx=ii; phit=phit2;}
  }
  if(idx==-1) return false;
  // Remove the entry in the arrays
  _Nhits--;
  for (int kk=idx;kk<_Nhits;kk++){
    _Hits   [kk] = _Hits   [kk+1];
    //    _BField [kk] = _BField [kk+1];
    _iHits  [kk] = _iHits  [kk+1];
  }
  // Update the number of projection hits
  if (phit->GetXCluster()) _NhitsX--;
  if (phit->GetYCluster()) _NhitsY--;
  if (phit->GetXCluster() && phit->GetYCluster()) _NhitsXY--;
  // Update the bitted pattern

#ifdef __DARWIN__
  typedef unsigned short ushort;
#endif
  ushort _bit=_bit_pattern ^ (1<<(phit->GetLayer()-1));  
  _bit_pattern=_bit;
  _bit=0;
  ushort _bitX=0;
  ushort _bitY=0;
  ushort _bitXY=0;
  for (int jj=0;jj<_Nhits;jj++){
    _bit|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetXCluster()) _bitX|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()) _bitY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
    if( pTrRecHit(jj)->GetYCluster()&&pTrRecHit(jj)->GetYCluster())
      _bitXY|=(1<<(9-pTrRecHit(jj)->GetLayer()-1));
  }
  _bit   = _bit>>1;
  _bitX  = _bitX>>1;
  _bitY  = _bitY>>1;
  _bitXY = _bitXY>>1;
  _Pattern   = patt->GetHitPatternIndex(_bit);
  _PatternX  = patt->GetHitPatternIndex(_bitX);
  _PatternY  = patt->GetHitPatternIndex(_bitY);
  _PatternXY = patt->GetHitPatternIndex(_bitXY);
  return true;
}

void TrTrackR::BuildHitsIndex()
{
  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++)
    _iHits[i] = cont2->getindex(_Hits[i]);

  delete cont2;
}

void TrTrackR::SetHitsIndex(int *ihit)
{
  VCon *cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return;

  for (int i = 0; i < _Nhits; i++) {
    _iHits[i] = ihit[i];
    _Hits[i] = (TrRecHitR *)cont->getelem(_iHits[i]);
  }

  delete cont;
}

void TrTrackR::GetMaxShift(int &left, int &right)
{
  left = right = 99;
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;

    int tkid  = hit->GetTkId();
    int nmult = hit->GetMultiplicity();
    int imult =  hit->GetResolvedMultiplicity();
    //    int imult = _iMult[i];
    int ll = 0;
    int rr = 0;

    if (tkid >= 0) { ll = nmult-imult-1; rr = imult; }
    else           { rr = nmult-imult-1; ll = imult; }
    if (ll < left)  left  = ll;
    if (rr < right) right = rr;
  }
  left *= -1;
}

void TrTrackR::Move(int shift, int fit_flags)
{
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit) continue;
   
    int tkid = hit->GetTkId();
    //   if (!hit->OnlyY()) {
      int nmult = hit->GetMultiplicity();
      int imult = hit->GetResolvedMultiplicity();
      //int imult = _iMult[i];

      if (tkid >= 0) imult -= shift;
      else           imult += shift;

      if (imult >= 0 && imult < nmult) {
	//	_iMult[i] = imult;
	_Hits [i]->SetResolvedMultiplicity(imult);
      }
      else cerr
	<< "TrTrackR::Move-E- Problem moving the Track "
	<< hit->GetTkId() << " " << nmult << " " << imult << " " << shift
	<< endl;
      //  }
  }
  if (fit_flags != 0 && ParExists(fit_flags)) FitT(fit_flags);
  else ReFit();
}

void TrTrackR::FillExRes(int idsel)
{
  map<int, TrTrackPar>::iterator it = _TrackPar.begin();
  for(;it!=_TrackPar.end();it++) {
    int id = it->first;
    if (id & (kFitLayer8 | kFitLayer9)) continue;
    if (idsel > 0 && id != idsel) continue;

    for (int ily = 7; ily <= 8; ily++) {
      TrRecHitR *hit = GetHitL(ily);
      if (hit) {
	AMSPoint pint = InterpolateLayer(ily, id);
	GetPar(id).Residual[ily][0] = (hit->GetCoord()-pint)[0];
	GetPar(id).Residual[ily][1] = (hit->GetCoord()-pint)[1];
      }
    }
  }
}

void TrTrackR::EstimateDummyX(int fitid)
{
  for (int i = 0; i < _Nhits; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || !hit->OnlyY()) continue;

    int tkid = hit->GetTkId();
    int ily  = hit->GetLayer()-1;
    AMSPoint gcoo = InterpolateLayer(ily, fitid);
    TkSens tks(tkid, gcoo, 0);

    if (tks.LadFound()) {
      float dy = TkDBc::Head->_ssize_active[1]/2;
      float ly = TkCoo::GetLadderCenterY(tkid);

      if (tks.GetStripX() < 0) {
	float sx = tks.GetSensCoo().x();
	float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->rot.GetEl(0, 0);
	if (sx < 0) gcoo[0] -= sg*(sx-1e-3);
	if (sx > TkDBc::Head->_ssize_active[0])
	  gcoo[0] -= sg*(sx-TkDBc::Head->_ssize_active[0]+1e-3);
      }
      if (fabs(gcoo.y()-ly) > dy) gcoo[1] = ly;
      tks.SetGlobalCoo(gcoo);
    }

    if (!tks.LadFound() || tks.GetLadTkID() != tkid) {
      static int nerr = 0;
      if (nerr++ < 100) {
	float dy = TkDBc::Head->_ssize_active[1]/2;
	float dx = TkCoo::GetLadderLength (tkid)/2;
	float lx = TkCoo::GetLadderCenterX(tkid);
	float ly = TkCoo::GetLadderCenterY(tkid);
	cerr << "TrTrackR::EstimateDummyX-W-Problem in X-coo estimation: "
	     << "tkid= " << tkid << " " 
	     << tks.GetLadTkID() << " " << tks.LadFound() << " "
	     << "g=" << gcoo.x() << " " << gcoo.y() << " "
	     << "dx=" << gcoo.x()-lx << " "
	     << "dy=" << gcoo.y()-ly  << " "
	     << "d=" << dx << " " << dy << endl;
      }

      //      _iMult[i] = 0;
      hit->SetDummyX(0);
    }
    else {
      hit->SetDummyX(tks.GetStripX());
      hit->SetResolvedMultiplicity(tks.GetMultIndex());
    }

    //    hit->BuildCoordinates();
  }
}

void TrTrackR::ReFit( const float *err,
		      float mass, float chrg){
  map<int, TrTrackPar>::iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++)
    FitT(it->first,-1,1,err,mass,chrg);
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

TrRecHitR *TrTrackR::GetHit(int i) const
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  TrRecHitR *hh = _Hits[i];
  
  if (!hh && _iHits[i] >= 0) {
    VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
    hh = (TrRecHitR*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return hh;
}

TrRecHitR *TrTrackR::GetHitL(int ilay) const
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




float TrTrackR::FitT(int id2, int layer, bool update, const float *err, 
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
    return -2;
  }

  // Force to use Linear fit if magnet is off
  if (_MagFieldOn==0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::CHOUTKO;
  if      (idf == kAlcaraz)    method = TrFit::ALCARAZ;
  else if (idf == kChikanian)  method = TrFit::CHIKANIAN;
  else if (idf == kChikanianF) method = TrFit::CHIKANIANF;
  else if (idf == kLinear)     method = TrFit::LINEAR;
  else if (idf == kCircle)     method = TrFit::CIRCLE;
  else if (idf == kSimple)     method = TrFit::SIMPLE;

  _TrFit.Clear();
  _TrFit.SetMassChrg(mass, chrg);

  // Set multiple scattering option and assumed mass
  if (id & kMultScat) 
    TrFit::_mscat = 1;
  else
    TrFit::_mscat = 0;

  // HIT Selection Section

  // One drop fitting
  if (id & kOneDrop) {
    double csymin = 0;
    int    ihmin  = -1;
    for (int i = 0; i < _Nhits; i++) {
      TrRecHitR *hit = GetHit(i);
      if (hit && FitT(idf, hit->GetLayer(), false) > 0 && 
	  _TrFit.GetChisqY() > 0) {
	if (ihmin < 0 || _TrFit.GetChisqY() < csymin) {
	  ihmin  = i;
	  csymin = _TrFit.GetChisqY();
	}
      }
    }
    if (ihmin < 0) return -4;
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
    if (ihmin < 0) return -5;
    layer = GetHit(ihmin)->GetLayer();
  }

  

  // Sort hits in the ascending order of the layer number
  int idx[trconst::maxlay], nhit = 0;
  int bhit[2] = { 0, 0 };

  for (int i = 0; i < _Nhits && nhit < trconst::maxlay; i++) {
    TrRecHitR *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;

    // For AMS02P
    if (TkDBc::Head->GetSetup() == 3) {
      if (hit->GetLayer() == 8) { 
	bhit[0] = 1;
	if (!(id & kFitLayer8)) continue; 
      }
      if (hit->GetLayer() == 9) {
	bhit[1] = 1;
	if (!(id & kFitLayer9)) continue; 
      }

      int lyr = hit->GetLayer();
      if (lyr == 8) lyr = 0;
      idx[nhit++] = lyr*10+i;
    }
    // For AMS02P

    // For AMS02-Ass1/PreInt with S.C. magnet
    else
      idx[nhit++] = hit->GetLayer()*10+i;
  }

  // AMS02P
  if (TkDBc::Head->GetSetup() == 3) {
    if ((id & kFitLayer8) && !bhit[0]) return -6;
    if ((id & kFitLayer9) && !bhit[1]) return -7;
    if ((id & kExternal)  && !bhit[0] && !bhit[1]) return -17;
  }
  // AMS02P

  std::sort(idx, &idx[nhit]);

  // External fit
  if (TkDBc::Head->GetSetup() == 3 && (id & kExternal) ){
    int idx2[4];
    if(!bhit[0] || !bhit[1]||nhit<4) return -8;
    idx2[0]=idx[0];
    idx2[1]=idx[1];
    idx2[2]=idx[nhit-2];
    idx2[3]=idx[nhit-1];
    nhit=4;
    for(int kk=0;kk<4;kk++)idx[kk]=idx2[kk];
  }  

  // Selected Pattern fit
  if(id & kPattern){
     int idx2[9];
     int nhit2=0;
     for (int kk=0;kk<nhit;kk++)
       if(CheckLayFit(id,idx[kk]/10)) idx2[nhit2++]=idx[kk];
     for(int kk=0;kk<nhit2;kk++)idx[kk]=idx2[kk];
     nhit=nhit2;
  }

  int i1 = 0, i2 = nhit;

  // Set fitting errors
  double errx = (err) ? err[0] : TRFITFFKEY.ErrX;
  double erry = (err) ? err[1] : TRFITFFKEY.ErrY;
  double errz = (err) ? err[2] : TRFITFFKEY.ErrZ;
  double zh0  = 0;
  int hitbits = 0;

  // Get the reference rigidity for weight tuning
  double rrgt = 0;
  int     id0 = kChoutko;
  if      (ParExists(id )) rrgt = GetRigidity(id);
  else if (ParExists(id0)) rrgt = GetRigidity(id0);

  // Fill hit points
  _TrFit.Clear();
  for (int i = i1; i < i2 && i < _Nhits; i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);
    if (!hit) continue;

    int lyr = hit->GetLayer();
    if ((id & kUpperHalf) && lyr >= 6) continue;
    if ((id & kLowerHalf) && lyr == 1) continue;

    AMSPoint coo =  hit->GetCoord();

    double fmscx = 1;
    double fmscy = 1;

    // Tune fitting weight
    if (!TrFit::_mscat) {
      int    ily  = hit->GetLayer()-1;
      double fitw = TRFITFFKEY.FitwMsc[ily];
      double fwxy = (errx > 0) ? erry/errx*2 : 1;
      if (rrgt != 0 && fitw > 0) {
	fmscx = std::sqrt(1+fitw*fitw/rrgt/rrgt*fwxy*fwxy);
	fmscy = std::sqrt(1+fitw*fitw/rrgt/rrgt);
      }
    }

    float ferx = (hit->OnlyY()) ? 0 : 1;
    float fery = (hit->OnlyX()) ? 0 : 1;

    double bf[3] = { 0, 0, 0 };
    TrFit::GuFld(coo[0], coo[1], coo[2], bf);
    _TrFit.Add(coo, ferx*errx*fmscx, fery*erry*fmscy, errz, 
	       bf[0], bf[1], bf[2]);
    
    hitbits |= (1 << (hit->GetLayer()-1));
    if (id != kLinear && j == 0) zh0 = coo.z();
  }

  if (method == TrFit::CHIKANIAN  || 
      method == TrFit::CHIKANIANF || TrFit::_mscat) {
    Double_t rini = 0;
    Int_t idr = kChoutko;
    Int_t idl = id & (kFitLayer8 | kFitLayer9);

    if (ParExists(idr | idl)) idr = idr | idl;
    if (ParExists(idr)) rini = GetRigidity(idr);
    _TrFit.SetRigidity(rini);
  }

  // Perform fitting
  float fdone = _TrFit.Fit(method);
  bool done = (fdone > 0 && _TrFit.GetChisqX() > 0 && 
	                    _TrFit.GetChisqY() > 0);
  if (done && method != TrFit::LINEAR && _TrFit.GetRigidity() == 0)
    done = false;
  if (!done) return -90000+fdone;

  // Return if fitting values are not to be over written
  if (!update) return _TrFit.GetChisq();

  // Fill fittng parameters
  TrTrackPar &par = _TrackPar[id];

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
  for (int i = 0; i < _TrFit.GetNhit(); i++) {
    int j = idx[i]%10;
    TrRecHitR *hit = GetHit(j);

    int il = hit->GetLayer()-1;
    par.Residual[il][0]= _TrFit.GetXr(i);
    par.Residual[il][1]= _TrFit.GetYr(i);
  }
  for (int i = 0; i < trconst::maxlay; i++) {
    if (GetResidual(i).norm() == 0) {
      TrRecHitR *hit = GetHitL(i);
      AMSPoint pint  = InterpolateLayer(i, id);
      if (hit){
	par.Residual[i][0] = (hit->GetCoord()-pint)[0];
	par.Residual[i][1] = (hit->GetCoord()-pint)[1];
      }else{
	par.Residual[i][0] = pint[0];
	par.Residual[i][1] = pint[1];
      }
    }
  }

  return GetChisq(id);
}

void TrTrackR::Print(int opt) {
  _PrepareOutput(opt);
  cout <<sout;

}

bool TrTrackR::CheckLayFit(int fittype,int lay) const{
  if ( lay==0) return ((fittype & kFitLayer8)>0) ;
  if ( lay==1) return ((fittype & kFitLayer1)>0) ; 
  if ( lay==2) return ((fittype & kFitLayer2)>0) ;
  if ( lay==3) return ((fittype & kFitLayer3)>0) ;
  if ( lay==4) return ((fittype & kFitLayer4)>0) ;
  if ( lay==5) return ((fittype & kFitLayer5)>0) ;
  if ( lay==6) return ((fittype & kFitLayer6)>0) ;
  if ( lay==7) return ((fittype & kFitLayer7)>0) ;
  if ( lay==8) return ((fittype & kFitLayer8)>0) ;
  if ( lay==9) return ((fittype & kFitLayer9)>0) ;
  return false;
}

char *  TrTrackR::Info(int iRef) {
  string aa;
  aa.append(Form("TrTrack #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}
std::ostream &TrTrackR::putout(std::ostream &ostr) {
  _PrepareOutput(1);

  return ostr << sout  << std::endl; 
    
}

void TrTrackR::_PrepareOutput(int full )
{
  sout.clear();
  sout.append(Form("NHits %d (x:%d,y:%d,xy:%d)Pattern: %d  %s,   DefFit: %d, Chi2 %6.3f Pirig %6.3f",
		   GetNhits(),GetNhitsX(),GetNhitsY(),GetNhitsXY(),GetPattern(),HitBitsString(GetBitPattern()),
		   trdefaultfit,Chi2FastFitf(),GetRigidity(kAlcaraz)));
  const TrTrackPar &bb=GetPar();
  bb.Print_stream(sout,full);
  if(!full) return;
  map<int, TrTrackPar>::const_iterator it=_TrackPar.begin();
  for(;it!=_TrackPar.end();it++){
    sout.append(Form("\nFit mode %d ",it->first));
    it->second.Print_stream(sout,full);
  }
}
  



double TrTrackR::Interpolate(double zpl, AMSPoint &pnt, 
                               AMSDir &dir, int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;

  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  dir.setp(0, 0,   1);
  pnt.setp(0, 0, zpl);
  return tprop.Interpolate(pnt, dir);
}

AMSPoint TrTrackR::InterpolateLayer(int ily, int id) const
{
  AMSPoint pnt;
  AMSDir   dir;
  InterpolateLayer(ily, pnt, dir, id);
  return pnt;
}

double TrTrackR::InterpolateLayer(int ily, AMSPoint &pnt, 
				  AMSDir &dir, int id2) const
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
  TkPlane  *pla = lad->GetPlane();
  AMSRotMat lrm = lad->GetRotMatA()*lad->GetRotMat();
  AMSRotMat prm = pla->GetRotMatA()*pla->GetRotMat();
  dir.setp(0, 0, 1);

  pnt = prm*(lad->GetPos()+lad->GetPosA())+pla->GetPosA()+pla->GetPos();
  dir = prm*lrm*dir;

  return tprop.Interpolate(pnt, dir);
}

void TrTrackR::Interpolate(int nz, double *zpl, 
                             AMSPoint *pvec, AMSDir *dvec, double *lvec,
                             int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  tprop.Interpolate(nz, zpl, pvec, dvec, lvec);
}

void TrTrackR::interpolate(AMSPoint pnt, AMSDir dir, AMSPoint &P1, 
			   number &theta, number &phi, number &length, 
			   int id2) const
{
  int id=id2;
  if (id2==0) id=trdefaultfit;
  if (!FitDone(id)) return;
  if(trdefaultfit==kDummy){
    AMSPoint pp=GetP0();
    AMSDir  dd=GetDir();

    P1[0]=pp[0]+dd[0]/dd[2]*pnt[2];
    P1[1]=pp[1]+dd[1]/dd[2]*pnt[2];
    P1[2]=pnt[2];
    theta=dd.gettheta();
    phi=dd.getphi();
    AMSPoint P2=P1-pp;
    length =P2.norm();
    if(pnt[2]>0)length*=-1;
 //    printf(" Z %f P0  %f %f %f \n",pnt[2],pp[0],pp[1],pp[2]);
//     printf("       P1  %f %f %f \n",pnt[2],P1[0],P1[1],P1[2]);
//     printf(" dist %f P2  %f %f %f \n",length,P2[0],P2[1],P2[2]);
    return;
  }

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
 if (!_MagFieldOn) return (int)FitT(kLinear|add_flag);
 for(int ii=0;ii<DEF_ADVFIT_NUM;ii++) {
/*
   // Chikanian fit only with ext. layers
   if ((DefaultAdvancedFitFlags[ii] == kChikanian ||
	DefaultAdvancedFitFlags[ii] == kChikanianF) && 
       !(add_flag & (TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9)))
     continue;
*/
   if ((AdvancedFitBits & (1 << ii)) && DefaultAdvancedFitFlags[ii] > 0) {
     FitT(DefaultAdvancedFitFlags[ii]| add_flag);
     if (add_flag == 0) {
       FitT(DefaultAdvancedFitFlags[ii]| kUpperHalf);
       FitT(DefaultAdvancedFitFlags[ii]| kLowerHalf);
     }
     else if (ii < 4)
       FitT(DefaultAdvancedFitFlags[ii]| add_flag);

     if (add_flag == (TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9)) 
       FitT(DefaultAdvancedFitFlags[ii]| add_flag| kExternal);
   }
 }
 return AdvancedFitDone(add_flag);
}

int TrTrackR::AdvancedFitDone(int add_flag)
{ 
  if (!_MagFieldOn) return FitDone(kLinear|add_flag);
  bool done = true;
  for(int ii=0;ii<DEF_ADVFIT_NUM;ii++)
    if ((AdvancedFitBits & (1 << ii)) && DefaultAdvancedFitFlags[ii] > 0) {
      done &=FitDone(DefaultAdvancedFitFlags[ii]| add_flag);

     if (add_flag == 0) {
       done &= FitDone(DefaultAdvancedFitFlags[ii]| kUpperHalf);
       done &= FitDone(DefaultAdvancedFitFlags[ii]| kLowerHalf);
     }
     if (add_flag == (TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9)) 
       done &= FitDone(DefaultAdvancedFitFlags[ii]| add_flag| kExternal);
    }
  return done;
}

char * TrTrackR::GetFitNameFromID(int fitnum){
  static char out[200];
  out[0]='\0';
  int basefit= fitnum & 0x000F;
  if(basefit == kDummy       ) strcat(out,"kDummy");
  if(basefit == kChoutko     ) strcat(out,"kChoutko");
  if(basefit == kGEANE       ) strcat(out,"kGEANE");
  if(basefit == kGEANE_Kalman) strcat(out,"kGEANE_Kalman");
  if(basefit == kAlcaraz     ) strcat(out,"kAlcaraz");
  if(basefit == kChikanian   ) strcat(out,"kChikanian");
  if(basefit == kChikanianF  ) strcat(out,"kChikanianF");
  if(basefit == kLinear      ) strcat(out,"kLinear");
  if(basefit == kCircle      ) strcat(out,"kCircle");
  if(basefit == kSimple      ) strcat(out,"kSimple");
  if(fitnum   & kMultScat    ) strcat(out," | kMultScat");
  if(fitnum   & kUpperHalf   ) strcat(out," | kUpperHalf");
  if(fitnum   & kLowerHalf   ) strcat(out," | kLowerHalf");
  if(fitnum   & kExternal    ) strcat(out," | kExternal");
  if(fitnum   & kOneDrop     ) strcat(out," | kOneDrop");
  if(fitnum   & kNoiseDrop   ) strcat(out," | kNoiseDrop");
  if(fitnum   & kFitLayer8   ) strcat(out," | kFitLayer8");
  if(fitnum   & kFitLayer9   ) strcat(out," | kFitLayer9");
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
    printf("[0x%04x] %s\n", it->first, GetFitNameFromID(it->first));
}

 const TrTrackPar &  TrTrackR::gTrTrackPar(int fitcode)throw (string){
   if(ParExists(fitcode)) return  _TrackPar[fitcode];
    //  throw an exception
    char tbt[255];
    sprintf(tbt,"TrTrackPar-E-NotFound %d", fitcode);
    throw string(tbt); 


}


int  TrTrackR::iTrTrackPar(int algo, int pattern, int refit, float mass, float  chrg){
  int type=algo%10;
  bool mscat=((algo/10)==1);
  int fittype=0;
  switch (type){
    case 0 :
      fittype|=trdefaultfit;
      break;
  case 1 :
    fittype|=kChoutko;
    break;
  case 2 :
    fittype|=kAlcaraz;
    break;
  case 3 :
    fittype|=kChikanianF;
    break;
  case 4 :
    fittype|=kChikanian;
    break;
    default :
      fittype|=kChoutko;
  }
  if((!mscat) && (fittype!=kChikanianF && fittype!=kChikanian))
    fittype|=kMultScat;
  int ebpat = _bit_pattern & 0x180;
  int basetype=fittype;
  if(pattern==0){
    // Has1N 
    if ((_bit_pattern & 0x80)>0) fittype|= kFitLayer8;
    // Has9 
    if ((_bit_pattern & 0x100)>0) fittype|= kFitLayer9;
  }
  else if(pattern==1) fittype|= kUpperHalf;
  else if(pattern==2) fittype|= kLowerHalf;
  else if(pattern==3) fittype=basetype;
  else if(pattern==4){ 
    if (ebpat == 0x180) fittype|= kExternal;
    else return -1;
  }
  else if(pattern==5){ 
    if (ebpat  & 0x080) fittype|= kFitLayer8;
      else return -1;
  }
  else if(pattern==6){
    if (ebpat  & 0x100) fittype|= kFitLayer9;
    else return -1;
  }
  else if(pattern==7){ 
    if (ebpat == 0x180) fittype|= kFitLayer8|kFitLayer9;
      else return -1;
  }
  else if(pattern>9){ //it is a base10 hit pattern	
    fittype|=kPattern;
    int fflayer[9]={
      kFitLayer1, kFitLayer2,	kFitLayer3, kFitLayer4,	kFitLayer5,
      kFitLayer6, kFitLayer7, kFitLayer8, kFitLayer9 };
    for(int kk=0;kk<9;kk++)
      if(((pattern/my_int_pow(10,kk))%10)>0){
	  if(_bit_pattern & (1<<kk))  fittype|=fflayer[kk];
          else
	    return -1;
      }
  }
  else
    return -1;

  
  bool FitExists=ParExists(fittype);
  if(refit==2 || (!FitExists && refit==1)) { 
    float ret=FitT(fittype,-1,true,0,mass,chrg);
    if (ret>0) 
      return fittype; 
    else 
       return -3;
  }
  FitExists=ParExists(fittype);
  if(!FitExists && refit==0) return -2;
  else if(FitExists) return fittype;
  else
    return -4;
}

int TrTrackR::Pattern(int input) {
int pat=0;
int p=1;
for(int k=0;k<maxlay;k++){
 if((input/p)%10)pat+=((_bit_pattern>>k)&1)*p;
p*=10;
}
return pat;
}    
