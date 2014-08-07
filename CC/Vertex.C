/////////////////////////////////////////////
/////////   PGTRACK VERSION     /////////////
/////////////////////////////////////////////

// Author J. Alcaraz June-July 2003
// Modified P. Zuccon January 2009 for new tk sw
//

#include "Vertex.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "tkdcards.h"
#include "amsdbc.h"
#include "TString.h"
#include "VCon.h"


void VertexR::Clear(){
  Momentum=0;
  ErrMomentum=0;
  Theta=0;
   Phi=0;
  Mass=0;
  Charge=0;
  Chi2=0;
  Ndof=0;
  fTrTrack.clear();
  _pTrTrack.clear();
  Vertex[0]=Vertex[1]=Vertex[2]=0;
  _filled=0;
  Status=0;
}

//
///////////////////////////////////////////////////////////

VertexR::VertexR(int ntracks, TrTrackR *ptrack[]) {
  
  //Reset to initial values
  Clear();
  //Set the track number

  for (int i=0; i<ntracks; i++) { _pTrTrack.push_back(ptrack[i]); }
  BuildTracksIndex();
  number mom[3] = {0.0, 0.0, 0.0};
     
  if (NTrTrack() <= 0) return ;
  
  // Most of the information
  number u[3];
  for (int i=0; i<NTrTrack(); i++) { 
    _pTrTrack[i]->setstatus(AMSDBc::USED);
    number rig =  _pTrTrack[i]->GetRigidity();
    number en =  fabs(rig);
    number erig =  _pTrTrack[i]->GetErrRinv()*en*en;
    Momentum += en;
    Charge += (rig>=0.0)? 1:-1;
    ErrMomentum += erig*erig;
    Chi2 += _pTrTrack[i]->GetChi2(); 
    Ndof += 2*_pTrTrack[i]->GetNhits() - 5; 
    AMSDir dir = AMSDir(_pTrTrack[i]->GetTheta(),_pTrTrack[i]->GetPhi());
    dir.getp(u);
    mom[0] += en*u[0]; 
    mom[1] += en*u[1]; 
    mom[2] += en*u[2];
  }

  ErrMomentum = sqrt(ErrMomentum)/Momentum/Momentum;
  Mass = Momentum*Momentum - mom[0]*mom[0] - mom[1]*mom[1] - mom[2]*mom[2];
  if (Mass>0.0) Mass = sqrt(Mass); else Mass = 0.0; 
  AMSDir U = AMSDir(mom);
  Theta = U.gettheta();
  Phi = U.getphi();

   set_vertex();

  _filled=1;

}

VertexR::VertexR(TrTrackR *track1, TrTrackR *track2)
{
  Clear();
  _pTrTrack.push_back(track1);
  _pTrTrack.push_back(track2);
  BuildTracksIndex();

  track1->setstatus(AMSDBc::USED);
  track2->setstatus(AMSDBc::USED);

  FitV(track1, track2);
}


std::string VertexR::_PrepareOutput(int full){ 

  std::string sout;
  sout.append(Form("Vtx: #tracks %d  Mom %f ErrMom  %f Chrg %d Coo: (%f, %f, %f)",
		   NTrTrack(), Momentum, ErrMomentum, Charge,Vertex[0],Vertex[1],Vertex[2]));
  
  if(!full)
    sout.append("\n");
  else
    sout.append(Form("Theta %f  Phi %f Chi2/Ndf %f/%d \n",
		   Theta,Phi, Chi2, Ndof));

  return sout;
}

void VertexR::Print(int opt) { 
  cout << _PrepareOutput(opt);
}


const char* VertexR::Info(int iRef){
  string aa;
  aa.append(Form("Vtx #%d ",iRef));
  aa.append(_PrepareOutput(0));
  unsigned int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}

std::ostream &VertexR::putout(std::ostream &ostr) {
  return ostr << _PrepareOutput(1) << std::endl;
}









void VertexR::BuildTracksIndex()
{
  VCon *cont2 = GetVCon()->GetCont("AMSTrTrack");
  if (!cont2) return;
  fTrTrack.clear();
  for (unsigned int i = 0; i < _pTrTrack.size(); i++) {
    fTrTrack.push_back(cont2->getindex(_pTrTrack[i]));
  }
  delete cont2;
}


void VertexR::BuildTracksPointerVector()
{
  VCon* cont2 = GetVCon()->GetCont("AMSTrTrack");
  if (!cont2) return;
  _pTrTrack.clear();
  for (unsigned int i = 0; i < fTrTrack.size(); i++) {
    _pTrTrack.push_back(((TrTrackR*)cont2->getelem(fTrTrack[i])));
  }
  delete cont2;
}

TrTrackR *VertexR::pTrTrack(unsigned int i) 
{

//if(i>=fTrTrack.size()) return 0;
  if ((i >= _pTrTrack.size() || _pTrTrack[i] == 0) && fTrTrack[i] >= 0) {
    BuildTracksPointerVector();
  }

  return _pTrTrack[i];
}


void VertexR::set_vertex(){
  
  Vertex[0]=  Vertex[1]=  Vertex[2]=0.;
  if (NTrTrack()<2) return;

  // Find minimum distance in the case of 2 tracks
  //  if (NTrTrack()==2) {

  //  VC 12-18-2003
  //  Correct vertex coordinates
  //

  {

    int number_of_pairs = 0;
    float maxz=2000;
    for (int i0=0; i0<NTrTrack()-1; i0++){ 
      for (int i1=i0+1; i1<NTrTrack(); i1++){ 

	AMSDir dir[2];

	dir[0] = AMSDir(_pTrTrack[i0]->GetPdir().gettheta(),_pTrTrack[i0]->GetPdir().getphi());
	dir[1] = AMSDir(_pTrTrack[i1]->GetPdir().gettheta(),_pTrTrack[i1]->GetPdir().getphi());

	number dirprod = dir[0].prod(dir[1]);
	AMSPoint deltax =  _pTrTrack[i0]->GetPentry() - _pTrTrack[i1]->GetPentry();

	number lambda[2];
	if (fabs(dirprod)<1.) {
	  AMSPoint aux = dir[0] - dir[1]*dirprod;
	  lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
	  aux = dir[1] - dir[0]*dirprod;
	  lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
	  AMSPoint poi[2];
	  poi[0] = AMSPoint(_pTrTrack[i0]->GetPentry()+dir[0]*lambda[0]);
	  poi[1] = AMSPoint(_pTrTrack[i1]->GetPentry()+dir[1]*lambda[1]);
	  number mom=fabs(_pTrTrack[i0]->GetRigidity())+fabs(_pTrTrack[i1]->GetRigidity());
	  //       cout <<" poi0 "<<poi[0]<<endl;
	  //       cout <<" poi1 "<<poi[1]<<endl;
	  if(fabs(poi[0][2])<maxz && fabs(poi[1][2])<maxz){
	    number_of_pairs++;
	    for (int j=0; j<3; j++) {
	      Vertex[j] += poi[0][j] * fabs(_pTrTrack[i0]->GetRigidity())/mom;
	      Vertex[j] += poi[1][j] * fabs(_pTrTrack[i1]->GetRigidity())/mom;
	    }
	  }
	  else{
	    static int nerr=0;
	    if(nerr++<100)cerr<<"AMSVTx::set_vertex-W-VertexZTooBig "<<poi[0]<<" "<<poi[1]<<endl; 
	    setstatus(AMSDBc::BAD);
	  }
	}

   
      }
    }
    for (int j=0; j<3; j++) {
      if(number_of_pairs)Vertex[j] = Vertex[j]/(number_of_pairs);
    }
#ifdef __AMSDEBUG__
    if(checkstatus(AMSDBc::BAD))cout <<" vertex "<<Vertex[0]<<" "<<Vertex[1]<<" "<<Vertex[2]<<endl;
#endif  
  }
  if(checkstatus(AMSDBc::BAD)){


    int number_of_pairs = 0;
    for (int i0=0; i0<NTrTrack()-1; i0++){ 
      for (int i1=i0+1; i1<NTrTrack(); i1++){ 

	AMSDir dir[2];

	dir[0] = AMSDir(_pTrTrack[i0]->GetPdir().gettheta(),_pTrTrack[i0]->GetPdir().getphi());
	dir[1] = AMSDir(_pTrTrack[i1]->GetPdir().gettheta(),_pTrTrack[i1]->GetPdir().getphi());

	number dirprod = dir[0].prod(dir[1]);
	AMSPoint deltax =  _pTrTrack[i0]->GetPentry() - _pTrTrack[i1]->GetPentry();

	number lambda[2];
	if (fabs(dirprod)<1.) {
	  AMSPoint aux = dir[0] - dir[1]*dirprod;
	  lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
	  aux = dir[1] - dir[0]*dirprod;
	  lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
	  AMSPoint poi[2];
	  poi[0] = AMSPoint(_pTrTrack[i0]->GetPentry()+dir[0]*lambda[0]);
	  poi[1] = AMSPoint(_pTrTrack[i1]->GetPentry()+dir[1]*lambda[1]);
	  number mom=fabs(_pTrTrack[i0]->GetRigidity())+fabs(_pTrTrack[i1]->GetRigidity());
	  number_of_pairs++;
	  for (int j=0; j<3; j++) {
	    Vertex[j] += poi[0][j] * fabs(_pTrTrack[i0]->GetRigidity())/mom;
	    Vertex[j] += poi[1][j] * fabs(_pTrTrack[i1]->GetRigidity())/mom;
	  }
	}

   
      }
    }
    for (int j=0; j<3; j++) {
      if(number_of_pairs)Vertex[j] = Vertex[j]/(number_of_pairs);
    }
#ifdef __AMSDEBUG__
    if(checkstatus(AMSDBc::BAD))cout <<" vertex "<< Vertex[0] <<" " << Vertex[1] <<" " << Vertex[2] <<endl;
#endif  
  }


}

double VertexR::ZrefV = 80;

#include "root.h"

void VertexR::Recover()
{
  if (NTrTrack() > 0) return;

  AMSEventR *evt = AMSEventR::Head();
  if (!evt || evt->nTrTrack() < 2) return;

  for (int i = 0; i < evt->nTrTrack(); i++) {
    TrTrackR *trk = evt->pTrTrack(i);
    if (!trk || !trk->ParExists(TrTrackR::kVertex)) continue;

    double dz = trk->GetP0z(TrTrackR::kVertex)-Vertex[2];
    if (fabs(dz) < 1) _pTrTrack.push_back(trk);
  }
  BuildTracksIndex();
}

double VertexR::FitV(TrTrackR *trk1, TrTrackR *trk2)
{
  if (!trk1 || !trk2) return -1;

  enum { NL = 8, ND = 5 };

  double zref = ZrefV;
  double sigx =   1;
  double sigy = 0.5;
  double smin =   1;

  int fidp  = TrTrackR::kAlcaraz;
  int fitid = TrTrackR::kVertex;
  int niter =  3;
  int ndiv  = 20;

  double   csqx[2], csqy[2], csq[2], rgt[2], erv[2];
  int      ndfx[2], ndfy[2];
  int      hbit[2];
  AMSPoint ppnt[2];
  AMSDir   pdir[2];
  trk1->Interpolate(zref, ppnt[0], pdir[0], fidp);
  trk2->Interpolate(zref, ppnt[1], pdir[1], fidp);

  double xs[NL*2], ys[NL*2], xr[NL*2], yr[NL*2];
  int    il[NL*2];

  for (int iter = 0; iter < niter; iter++) {
    double    xh[NL*2], yh[NL*2], zh[NL*2], ln[NL*2];
    AMSPoint pix[NL*2];
    AMSPoint piu[NL*2];
    double     F[NL*2][3];
    double     G[NL*2][3];

    int nh[2] = { 0, 0 };

    hbit[0] = hbit[1] = 0;

    for (int i = 0; i < NL*2; i++) il[i] = -1;

    for (int i = 0; i < 2; i++) {
      TrTrackR *trk = (i == 0) ? trk1 : trk2;
      AMSPoint   pb = ppnt[i];

      for (int k = 0; k < NL; k++) {
	double xsg = 0, ysg = 0;
	AMSPoint p = pb;
	if (k > 0) {
	  TrRecHitR *hit = trk->GetHitLO(k);
	  if (!hit) continue;

	  xsg = (hit->OnlyY()) ? 0 : sigx;
	  ysg = (hit->OnlyX()) ? 0 : sigy;

	  hbit[i] |= (1 << (hit->GetLayer()-1));
	  p = hit->GetCoord();
	}
	AMSPoint d = p-pb;

	int jj  = nh[i]+i*NL;
	xh [jj] = p.x();
	yh [jj] = p.y();
	zh [jj] = p.z();
	xs [jj] = xsg;
	ys [jj] = ysg;
	ln [jj] = d.norm();
	pix[jj] = piu[jj] = 0;

	il[i*NL+k] = jj;

	if(ln[jj] > 0) {
	  for (int j = 0; j <= ndiv; j++) {
	    double   e  = (double)j/ndiv;
	    double   w  = (j == 0 || j == ndiv) ? 1 : ((j%2 == 1) ? 4 : 2);
	    AMSPoint ed = d*e;
	    AMSPoint b  = TrFit::GuFld(pb+ed);
	    pix[jj] = pix[jj]+d.crossp(b)/ln[jj]*w*(1-e);
	    piu[jj] = piu[jj]+d.crossp(b)/ln[jj]*w;
	  }
	  pix[jj] = pix[jj]/(3*ndiv);
	  piu[jj] = piu[jj]/(3*ndiv);
	}

	pb = p;
	nh[i]++;
      }

      for (int j = i*NL; j < i*NL+nh[i]; j++) {
	F[j][0] = G[j][0] = 1;
	F[j][1] = G[j][1] = F[j][2] = G[j][2] = 0;

	for (int k = i*NL; k <= j; k++) {
	  F[j][1] += ln[k];
	  G[j][1] += ln[k];

	  for (int l = i*NL; l <= k; l++) {
	    if (l == k) { F[j][2] += ln[k]*ln[k]*pix[k][0];
	                  G[j][2] += ln[k]*ln[k]*pix[k][1]; } 
	    else        { F[j][2] += ln[k]*ln[l]*piu[l][0];
	                  G[j][2] += ln[k]*ln[l]*piu[l][1]; }
	  }
	}
      }
    }

    double Mx[ND][ND], Vx[ND];
    double My[ND][ND], Vy[ND];
    for (int i = 0; i < ND; i++) {
      for (int j = 0; j < ND; j++) Mx[i][j] = My[i][j] = 0;
                                   Vx[i]    = Vy[i]    = 0;
    }

    int ip[6] = { 0, 1, 2, 3, 1, 4 };

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 3; j++)
	for (int k = 0; k < 3; k++) {
	  int jj = ip[i*3+j];
	  int kk = ip[i*3+k];
	  for (int l = i*NL; l < i*NL+nh[i]; l++) {
	    if (k == 0) {
	      if (xs[l] > 0)   Vx[jj] += F[l][j]/xs[l]/xs[l]*xh[l];
	      if (ys[l] > 0)   Vy[jj] += G[l][j]/ys[l]/ys[l]*yh[l];
	    }
	    if (xs[l] > 0) Mx[jj][kk] += F[l][j]/xs[l]/xs[l]*F[l][k];
	    if (ys[l] > 0) My[jj][kk] += G[l][j]/ys[l]/ys[l]*G[l][k];
	  }
	}
    }
    if (TrFit::Inv55(My) < 0) return -2;

    double par[ND*2];
    for (int i = 0; i < ND*2; i++) par[i] = 0;

    int pi[8] = { 1, 3, 4, 6, 9,  0, 2, 5 };
    for (int i = 0; i < 5; i++)
      for (int j = 0; j < 5; j++) par[pi[j]] += My[j][i]*Vy[i];
    par[8] = par[3];

    Vx[0] -= Mx[0][2]*par[4]; Mx[0][2] = Mx[2][0] = 0;
    Vx[1] -= Mx[1][2]*par[4]; Mx[1][2] = Mx[2][1] = 0;
    Vx[2]  =          par[4]; Mx[2][2] = 1;
    Vx[3] -= Mx[0][4]*par[9]; Mx[0][4] = Mx[4][0] = 0; Mx[1][4] = Mx[4][1] = 0;
    Vx[4]  =          par[9]; Mx[4][4] = 1;            Mx[3][4] = Mx[4][3] = 0;

    if (TrFit::Inv55(Mx) < 0) return -3;

    int mj[3] = { 0, 1, 3 };
    for (int i = 0; i < ND; i++)
      for (int j = 0; j < 3; j++) par[pi[j+5]] += Mx[mj[j]][i]*Vx[i];
    par[7] = par[2];

    erv[0] = erv[1] = 0;
    if (My[2][2] > 0) erv[0] = std::sqrt(My[2][2])*1e12/TrProp::Clight;
    if (My[4][4] > 0) erv[1] = std::sqrt(My[4][4])*1e12/TrProp::Clight;

    double ds = 0;

    for (int i = 0; i < 2; i++) {
      double *pp = &par[i*ND];
      double  dz = 1-pp[2]*pp[2]-pp[3]*pp[3];
      if (dz > 0) dz = std::sqrt(dz);

      ppnt[i].setp( pp[0],  pp[1], zh[i*NL]);
      pdir[i].setp(-pp[2], -pp[3], dz);
      rgt [i] =   ( pp[4] != 0) ? TrProp::Clight*1e-12/pp[4] : 0;

      TrProp trp(ppnt[i], pdir[i], rgt[i]);
      double sx = 0, sy = 0;

      csqx[i] = csqy[i] = csq[i] = 0;
      ndfx[i] = ndfy[i] = 0;

      for (int j = i*NL; j < i*NL+nh[i]; j++) {
	trp.Propagate(zh[j]);
	xr[j] = xh[j]-trp.GetP0x();
	yr[j] = yh[j]-trp.GetP0y();

	if (xs[j] > 0) { csqx[i] += xr[j]*xr[j]
	                           /xs[j]/xs[j]; ndfx[i]++; sx += xr[j]; }
	if (ys[j] > 0) { csqy[i] += yr[j]*yr[j]
	                           /ys[j]/ys[j]; ndfy[i]++; sy += yr[j]; }
      }
      if (ndfx[i] > 0) sx /= ndfx[i];
      if (ndfy[i] > 0) sy /= ndfy[i];

      ndfx[i] -= 2;
      ndfy[i] -= 3;
      if (ndfx[i]+ndfy[i] > 0) csq[i] = (csqx[i]+csqy[i])/(ndfx[i]+ndfy[i]);

      pp[0] += sx;
      pp[1] += sy;
      ppnt[i].setp(pp[0], pp[1], zh[i*NL]);

      ds += std::sqrt(sx*sx+sy*sy);
    }
    if (ds < smin) break;
  }

  csqy[0] *= fabs(rgt[0]);
  csqy[1] *= fabs(rgt[1]);

  for (int i = 0; i < 2; i++) {
    TrTrackR    *trk = (i == 0) ? trk1 : trk2;
    TrTrackPar &tpar = trk->AddPar(fitid);
    //trk->Settrdefaultfit(fitid);

    tpar.FitDone  = true;
    tpar.HitBits  = hbit[i];
    tpar.ChisqX   = csqx[i];
    tpar.ChisqY   = csqy[i];
    tpar.NdofX    = ndfx[i];
    tpar.NdofY    = ndfy[i];
    tpar.Chisq    = csq [i];
    tpar.Rigidity = rgt [i];
    tpar.ErrRinv  = erv [i];
    tpar.P0       = ppnt[i];
    tpar.Dir      = pdir[i];

    for (int j = 0; j < trconst::maxlay; j++) {
      int jj = (j < NL) ? il[i*NL+j] : -1;
      if (0 <= jj && jj < NL*2) {
	tpar.Residual[j][0] = xr[jj];
	tpar.Residual[j][1] = yr[jj];
	tpar.weight  [j][0] = xs[jj];
	tpar.weight  [j][1] = ys[jj];
      }
      else {
	tpar.Residual[j][0] = tpar.Residual[j][1] = 0;
	tpar.weight  [j][0] = tpar.weight  [j][1] = 0;
      }
    }

    Momentum    += fabs(rgt[i]);
    Charge      += (rgt[i] > 0) ? 1 : -1;
    ErrMomentum += erv[i]*erv[i];
    Chi2        += csqx[i]+csqy[i];
    Ndof        += ndfx[i]+ndfy[i];
  }
  if (Momentum <= 0) return -4;

  ErrMomentum = std::sqrt(ErrMomentum);
  Ndof += 2;
  Chi2 /= (Ndof > 0) ? Ndof : 1;

  Theta = pdir[0].gettheta();
  Phi   = pdir[0].getphi();

  Vertex[0] = (ppnt[0].x()+ppnt[1].x())/2;
  Vertex[1] = (ppnt[0].y()+ppnt[1].y())/2;
  Vertex[2] = (ppnt[0].z()+ppnt[1].z())/2;

  _filled = 2;

  return 0;
}

