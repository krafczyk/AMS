// $Id: gvdraw.cpp,v 1.4 2012/11/25 15:10:06 shaino Exp $
#include "gvconst.h"
#include "gvdraw.h"
#include "gvgeom.h"

#include "TkCoo.h"
#include "TrCluster.h"
#include "root.h"
#include "amsdbc.h"

#include "TMath.h"

int GVDraw::tkSetup = 3; // AMS02P

GVDraw::GVDraw()
{
  currEvent = 0;
  drawOpt   = 0;
  ladVec    = 0;
}

GVDraw::~GVDraw()
{
}

TkLadder *GVDraw::getLadder(int sid) const
{
  return (TkDBc::Head && ladVec && 
	  1 <= sid && sid <= TkDBc::Head->GetEntries()) 
    ? TkDBc::Head->GetEntry(ladVec[sid-1]) : 0;
}

void GVDraw::drawObject(bool render, bool select, int idsel)
{
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);

  glAlphaFunc(GL_GREATER, 0.1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (render) {
    drawTrRecHits();
    drawTrdHits ();
    drawTofHits ();
    drawRichHits();
    drawEcalHits();
    drawTrClusters();
    drawAxes();
  }
  drawMCTracks (render, select, idsel);
  drawTrTracks (render, select, idsel);
  drawTrdTracks(render, select, idsel);
  drawTracker  (render, select, idsel);
  drawAMSgeom  (render, select, idsel);

  glDisable(GL_ALPHA_TEST);
}

void GVDraw::drawAMSgeom(bool render, bool select, int idsel)
{
  GVGeom::drawAll(render, select, idsel);
}

void GVDraw::drawTracker(bool render, bool select, int idsel)
{
  if (!render || !ladVec || !TkDBc::Head) return;

  static const double clad[8] = { 0.0, 1.0, 0.0, 0.6,  0.0, 0.8, 0.8, 0.6 };
  static const double cclx[8] = { 0.9, 0.0, 0.0, 0.6,  0.9, 0.2, 0.2, 1.0 };
  static const double ccly[8] = { 0.0, 0.0, 0.9, 0.6,  0.2, 0.2, 0.9, 1.0 };
  static const double cclc[8] = { 0.8, 0.0, 0.8, 0.6,  0.8, 0.2, 0.8, 1.0 };
  static const double cnum[4] = { 0.9, 0.9, 0.9, 0.6 };

  int clsx[NLAY*35], clsy[NLAY*35];
  fillHitVec(clsx, clsy);

  int ent = TkDBc::Head->GetEntries();
  for (int i = 0; i < ent; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(ladVec[i]);
    int tkid = lad->GetTkId();
    int lyr  = lad->GetLayer();
    int slot = tkid%100;
    int lid  = (lyr-1)*35+slot+16;

    const double *cpar = clad;
    if (clsx[lid] && clsy[lid]) cpar = cclc;
    else if (clsx[lid])         cpar = cclx;
    else if (clsy[lid])         cpar = ccly;

    int glid = typeToGLID(Geom_TRK, i+1);
    GLRender *gobj = GLRender::findObject(glid);

    gobj->setMatCol(&cpar[0]);
    gobj->setSelCol(&cpar[4]);

    if (GVGeom::isGeomEnabled(Geom_LTRK))
      gobj->setEnabled(cpar != clad);

    if (render && glid == idsel) {
      double x  = TkCoo::GetLadderCenterX(tkid);
      double y  = TkCoo::GetLadderCenterY(tkid);
      double z  = TkDBc::TkDBc::Head->GetZlayer(lyr);
      double dx = TkCoo::GetLadderLength(tkid)/2;
      GLRender::glMatCol(cnum);
      GLRender::glDigit(x+TMath::Sign(dx-15, x)-8, y, z+1, tkid, -2, 3.5);
      GLRender::glDigit(x+TMath::Sign(dx-15, x)-8, y, z-1, tkid,  2, 3.5);
    }
  }
}

void GVDraw::drawTrTracks(bool render, bool select, int idsel)
{
  if (!currEvent) return;
  if (!testOpt(Draw_TRK)) return;
  if (!GVGeom::isGeomEnabled(Geom_TRK) &&
      !GVGeom::isGeomEnabled(Geom_LTRK)) return;

  static const double ctrk[4] = { 0.9, 0.0, 0.0, 1.0 };
  static const double csel[4] = { 0.9, 0.4, 0.4, 1.0 };

  for (int i = 0; i < currEvent->nTrTrack(); i++) {

    int sid = typeToGLID(Geom_TRK, i+SID_OFFS_TRACK);
    if (render) {
      glLineWidth(2);
      if (idsel == sid) GLRender::glMatCol(csel);
      else              GLRender::glMatCol(ctrk);
    }
    else if (select) glLoadName(sid);

    TrTrackR *trk = currEvent->pTrTrack(i);

    int fid = TrTrackR::DefaultFitID;  //kChoukto;
    if (!trk->ParExists(fid)) fid = TrTrackR::kLinear;

    if (fid == TrTrackR::kLinear) {
      double z[2] = { -180, 180 };
      double p0z  =   trk->GetP0z(fid);
      double x[2] = { trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[0]-p0z), 
		      trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[1]-p0z) };
      double y[2] = { trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[0]-p0z), 
		      trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[1]-p0z) };
      GLRender::glLine(x[0], y[0], z[0], x[1], y[1], z[1]);
    }
    else {
      enum { NPL = 40 };
      double   zpl [NPL+1];
      AMSPoint pint[NPL+1];
      for (int j = 0; j <= NPL; j++) zpl[j] = -180.+360.*j/NPL;

      fid = TrTrackR::kVertex;
      TrProp trp(trk->GetP0(fid), trk->GetDir(fid), trk->GetRigidity(fid));
      trp.Interpolate(NPL+1, zpl, pint);

      double xp[NPL+1], yp[NPL+1], zp[NPL+1];
      for (int j = 0; j <= NPL; j++) {
	xp[j] = pint[j].x();
	yp[j] = pint[j].y();
	zp[j] = pint[j].z();
      }
      GLRender::glPLines(NPL+1, xp, yp, zp);
    }
  }
}

void GVDraw::drawMCTracks(bool render, bool select, int idsel)
{
  if (!currEvent) return;
  if (!testOpt(Draw_MCP) && !testOpt(Draw_MCA)) return;

  static const double ctrk[4] = { 0.0, 0.0, 0.9, 0.8 };
  static const double csel[4] = { 0.4, 0.4, 0.9, 0.8 };

  int nmc = currEvent->nMCEventg();
  if (testOpt(Draw_MCP) && nmc > 0) nmc = 1;

  for (int i = 0; i < nmc; i++) {

    int sid = typeToGLID(Geom_NONE, i+SID_OFFS_TRACK);
    if (render) {
      glLineWidth(2);
      if (idsel == sid) GLRender::glMatCol(csel);
      else              GLRender::glMatCol(ctrk);
    }
    else if (select) glLoadName(sid);

    MCEventgR *mct = currEvent->pMCEventg(i);

    if (mct->Charge == 0) {
      double z[2] = { -180, 180 };
      double p0z  =   mct->Coo[2];
      double dxz  =   mct->Dir[0]/mct->Dir[2];
      double dyz  =   mct->Dir[1]/mct->Dir[2];
      double x[2] = { mct->Coo[0]+dxz*(z[0]-p0z), mct->Coo[0]+dxz*(z[1]-p0z) };
      double y[2] = { mct->Coo[1]+dyz*(z[0]-p0z), mct->Coo[1]+dyz*(z[1]-p0z) };

      GLRender::glLine(x[0], y[0], z[0], x[1], y[1], z[1]);
    }
    else {
      enum { NPL = 40 };
      double   zpl [NPL+1];
      AMSPoint pint[NPL+1];
      for (int j = 0; j <= NPL; j++) zpl[j] = -180.+360.*j/NPL;

      TrProp trp(AMSPoint(mct->Coo[0], mct->Coo[1], mct->Coo[2]),
		 AMSDir  (mct->Dir[0], mct->Dir[1], mct->Dir[2]),
		 mct->Momentum/mct->Charge);
      trp.Interpolate(NPL+1, zpl, pint);

      double xp[NPL+1], yp[NPL+1], zp[NPL+1];
      for (int j = 0; j <= NPL; j++) {
	xp[j] = pint[j].x();
	yp[j] = pint[j].y();
	zp[j] = pint[j].z();
      }

      GLRender::glPLines(NPL+1, xp, yp, zp);
    }
  }
}

void GVDraw::drawTrdTracks(bool render, bool select, int idsel)
{
  if (!currEvent) return;
  if (!testOpt(Draw_TRK)) return;
  if (!GVGeom::isGeomEnabled(Geom_TRD)) return;

  static const double ctrk[4] = { 0.0, 0.5, 0.9, 1.0 };
  static const double csel[4] = { 0.5, 0.9, 0.9, 1.0 };

  for (int i = 0; i < currEvent->nTrdTrack(); i++) {

    int sid = typeToGLID(Geom_TRD, i+SID_OFFS_TRACK);
    if (render) {
      glLineWidth(2);
      if (idsel == sid) GLRender::glMatCol(csel);
      else              GLRender::glMatCol(ctrk);
    }
    else if (select) glLoadName(sid);

    TrdTrackR *trd = currEvent->pTrdTrack(i);
    if (!trd) continue;

    AMSDir dir(trd->Theta, trd->Phi);
    if (dir.z() == 0) continue;

    double dxdz = dir.x()/dir.z();
    double dydz = dir.y()/dir.z();

    double z[2] = { 80, 150 };
    double x[2] = { trd->Coo[0]+dxdz*(z[0]-trd->Coo[2]), 
		    trd->Coo[0]+dxdz*(z[1]-trd->Coo[2]) };
    double y[2] = { trd->Coo[1]+dydz*(z[0]-trd->Coo[2]), 
		    trd->Coo[1]+dydz*(z[1]-trd->Coo[2]) };
    GLRender::glLine(x[0], y[0], z[0], x[1], y[1], z[1]);
  }
}

void GVDraw::drawTrClusters()
{
  if (!currEvent) return;
  if (!testOpt(Draw_CLS)) return;
  if (!GVGeom::isGeomEnabled(Geom_TRK) &&
      !GVGeom::isGeomEnabled(Geom_LTRK)) return;

  static const double ccls[4] = { 0.0, 0.9, 0.9, 1.0 };

  for (int i = 0; i < currEvent->nTrCluster(); i++) {
    TrClusterR *cls = currEvent->pTrCluster(i);
    if (!cls) continue;

    int tkid = cls->GetTkId();
    int side = cls->GetSide();
    int lyr  = cls->GetLayer();

    double x  = TkCoo::GetLadderCenterX(tkid);
    double y  = TkCoo::GetLadderCenterY(tkid);
    double z  = TkDBc::TkDBc::Head->GetZlayer(lyr);
    double dx = TkCoo::GetLadderLength(tkid)/2;
    double dy = TkDBc::Head->_ssize_inactive[1]/2;

    //cls->BuildCoordinates();
    double px[2], py[2], pz[4];
    for (int j = 0; j < cls->GetMultiplicity(); j++) {
      double coo = cls->GetGCoord(j);
      px[0] = (side == 0) ? coo : x-dx;
      px[1] = (side == 0) ? coo : x+dx;
      py[0] = (side == 1) ? coo : y-dy;
      py[1] = (side == 1) ? coo : y+dy;
      pz[0] = pz[1] = z+0.3; pz[2] = pz[3] = z-0.3;
      GLRender::glMatCol(ccls);
      GLRender::glLine(px[0], py[0], pz[0], px[1], py[1], pz[1]);
      GLRender::glLine(px[0], py[0], pz[2], px[1], py[1], pz[3]);
    }
  }
}

void GVDraw::drawTrRecHits()
{
  if (!currEvent) return;
  if (!testOpt(Draw_HIT)) return;
  if (!GVGeom::isGeomEnabled(Geom_TRK) &&
      !GVGeom::isGeomEnabled(Geom_LTRK)) return;

  static const double chit1[4] = { 0.0, 0.9, 0.9, 1.0 };
  static const double chit2[4] = { 0.9, 0.6, 0.4, 1.0 };

  for (int i = 0; i < currEvent->nTrRecHit(); i++){
    TrRecHitR *hit = currEvent->pTrRecHit(i);
    if (!hit) continue;

    const double *chit = chit1;
    int im1 = 0, im2 = hit->GetMultiplicity()-1;

    if (hit->checkstatus(AMSDBc::USED)) {
      chit = chit2;
      im1 = im2 = hit->GetResolvedMultiplicity();
    }
    else if (hit->OnlyY()) continue;

    for (int j = im1; j <= im2; j++) {
      AMSPoint coo = hit->GetCoord(j);
      double x = coo.x();
      double y = coo.y();
      double z = coo.z();

      GLRender::glMatCol(chit);
      GLRender::glSphere(x, y, z, 1);
    }
  }
}

void GVDraw::drawTrdHits()
{
  if (!currEvent) return;
  if (!testOpt(Draw_HIT)) return;
  if (!GVGeom::isGeomEnabled(Geom_TRD)) return;

  static const double chit1[4] = { 0.0, 0.9, 0.9, 1.0 };
  static const double chit2[4] = { 0.9, 0.6, 0.4, 1.0 };
  static const double chitc[4] = { 0.0, 0.2, 0.9, 1.0 };

  glLineWidth(2);

  for (int i = 0; i < currEvent->nTrdCluster(); i++){
    TrdClusterR *hit = currEvent->pTrdCluster(i);
    if (!hit) continue;

    float *coo = hit->Coo;
    float  len = hit->ClSizeZ;

    const double *chit = chit1;
    if (hit->Status & AMSDBc::USED) chit = chit2;

    GLRender::glMatCol(chit);
    GLRender::glSphere(coo[0], coo[1], coo[2], 1);

    GLRender::glMatCol(chitc);
    if (hit->Direction == 0)
      GLRender::glLine(-len, coo[1], coo[2], len, coo[1], coo[2]);
    else
      GLRender::glLine(coo[0], -len, coo[2], coo[0], len, coo[2]);
  }
}

void GVDraw::drawTofHits()
{
  if (!currEvent) return;
  if (!testOpt(Draw_HIT)) return;
  if (!GVGeom::isGeomEnabled(Geom_TOF) &&
      !GVGeom::isGeomEnabled(Geom_LTOF)) return;

  static const double chit[4] = { 0.9, 0.0, 0.0, 1.0 };

  glLineWidth(2);

  for (int i = 0; i < currEvent->nTofCluster(); i++){
    TofClusterR *hit = currEvent->pTofCluster(i);
    if (!hit) continue;

    float *coo = hit->Coo;
    float *err = hit->ErrorCoo;

    GLRender::glMatCol(chit);
    GLRender::glBox(coo[0]-err[0], coo[1]-err[1], coo[2]-err[2], 
		         2*err[0],      2*err[1],      2*err[2]);
  }
}

void GVDraw::drawRichHits()
{
  if (!currEvent) return;
  if (!testOpt(Draw_HIT)) return;
  if (!GVGeom::isGeomEnabled(Geom_RICH)) return;

  static const double chit[4] = { 0.9, 0.6, 0.4, 1.0 };

  glLineWidth(2);

  for (int i = 0; i < currEvent->nRichHit(); i++){
    RichHitR *hit = currEvent->pRichHit(i);
    if (!hit) continue;

    float *coo = hit->Coo;
    GLRender::glMatCol(chit);
    GLRender::glSphere(coo[0], coo[1], coo[2], 1);
  }
}

void GVDraw::drawEcalHits()
{
  if (!currEvent) return;
  if (!testOpt(Draw_HIT)) return;
  if (!GVGeom::isGeomEnabled(Geom_ECAL)) return;

  static const double chit[4] = { 0.9, 0.6, 0.4, 1.0 };

  glLineWidth(2);

  for (int i = 0; i < currEvent->nEcalCluster(); i++){
    EcalClusterR *hit = currEvent->pEcalCluster(i);
    if (!hit) continue;

    float *coo = hit->Coo;
    GLRender::glMatCol(chit);
    GLRender::glSphere(coo[0], coo[1], coo[2], 1);
  }
}

void GVDraw::drawAxes()
{
  static const double col[4] = { 0.7, 0.7, 0.7, 1.0 };
  GLRender::glMatCol(col);
  glLineWidth(0.5);

  double p0 = -170, p1 = -140;
  GLRender::glLine(p0, p0, p0, p1, p0, p0);
  GLRender::glLine(p0, p0, p0, p0, p1, p0);
  GLRender::glLine(p0, p0, p0, p0, p0, p1);

  glLineWidth(1);
}

void GVDraw::fillHitVec(int *clsx, int *clsy)
{
  for (int i = 0; i < NLAY*35; i++) clsx[i] = clsy[i] = 0;

  for (int i = 0; currEvent && i < currEvent->nTrCluster(); i++) {
    TrClusterR *cls = currEvent->pTrCluster(i);
    if (!cls) continue;

    int tkid = cls->GetTkId();
    int side = cls->GetSide();
    int lyr  = cls->GetLayer();
    int slot = tkid%100;
    int id   = (lyr-1)*35+slot+16;

    if (side == 0) clsx[id]++;
    else           clsy[id]++;
  }
}

#include "TFile.h"
#include "TSystem.h"
#include "TGeometry.h"
#include "TNode.h"
#include "TBRIK.h"

#include <iostream>

void GVDraw::initGeom(const char *fname, const char *key, const char *nname)
{
  TGeometry *geom = 0;

  TString sfn = gSystem->ExpandPathName(Form("$PWD/%s", fname));

  TFile f(sfn);
  if (f.IsOpen()) {
    geom = (TGeometry *)f.Get(key);
    if (!geom)
      std::cerr << "\"" << key << "\" not found in " << fname << std::endl;
  }
  if (!geom) geom = new TGeometry("ams02", "AMS-02");

  TNode *node = geom->GetNode(nname);
  if (!node) {
    std::cerr << "Node \"" << nname << "\" not found in " << fname 
	      << "/" << key << std::endl;
    node = new TNode("AMS", "AMS-02", 
		     new TBRIK("BAMS", "Mother", "", 200, 200, 200),
		     0, 0, 0);
  }

  node->cd();
  buildTracker();

  node->cd();
  TShape *brik = new TBRIK("ECAL", "Ecal", "", 33, 33, 12.5);
  TNode  *ecal = new TNode("BECL", "Ecal", brik, 0, 0, -150);

  std::cout << "AMS GL objects have been built. "
	    << "Nobj: " << GVGeom::buildGeom(node, 1)
	    << std::endl;

  GVGeom::sortGeom();
}

void GVDraw::buildTracker()
{
  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(tkSetup);
  }

  if (!ladVec) {
    int nlad = TkDBc::Head->GetEntries();
    ladVec = new int[nlad];

    for (int i = 0, n = 0; i < NLAY; i++) {
      for (int j = 0; j < nlad; j++) {
	TkLadder *lad = TkDBc::Head->GetEntry(j);
	if (lad->GetLayer() != NLAY-i) continue;
	ladVec[n++] = j;
      }
    }
  }

  TShape *stk = new TBRIK("TBRK", "Tracker box", "", 70, 70, 170);
  TNode  *ntk = new TNode("STK", "Tracker", stk, 0, 0, 0);
  ntk->SetVisibility(0);
  ntk->cd();

  int ent = TkDBc::Head->GetEntries();

  for (int i = 0; i < ent; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(ladVec[i]);
    int tkid = lad->GetTkId();
    int lyr  = lad->GetLayer();
    int slot = tkid%100;
    int lid  = (lyr-1)*35+slot+16;

    double x  = TkCoo::GetLadderCenterX(tkid);
    double y  = TkCoo::GetLadderCenterY(tkid);
    double z  = TkDBc::TkDBc::Head->GetZlayer(lyr);
    double dx = TkCoo::GetLadderLength(tkid)/2;
    double dy = TkDBc::Head->_ssize_active[1]/2;
    double dz = 0.2;

    TShape *blad = new TBRIK(Form("TBR%03d", i+1),
			     Form("Ladder %4d", tkid), "", dx, dy, dz);
    new TNode(Form("STK%03d", i+1),
	      Form("Ladder %4d", tkid), blad, x, y, z);
  }

  buildLines();
}

void GVDraw::buildLines()
{
  if (!TkDBc::Head) return;

  enum { BLAY = 64, SIZE = NLAY*BLAY };

  int nll[NLAY];
  double xll[SIZE], yll[SIZE], zll[SIZE];
  for (int i = 0; i < SIZE; i++) xll[i] = yll[i] = zll[i] = 0;

  int ent = TkDBc::Head->GetEntries();
  for (int i = 0; i < ent; i++) {
    TkLadder *ladder = TkDBc::Head->GetEntry(i);
    int tkid  = ladder->GetTkId();
    int slot  = ladder->GetSlot();
    int layer = ladder->GetLayer();

    double x  = TkCoo::GetLadderCenterX(tkid);
    double y  = TkCoo::GetLadderCenterY(tkid);
    double z  = TkDBc::TkDBc::Head->GetZlayer(layer);
    double dx = TkCoo::GetLadderLength(tkid)/2;
    double dy = TkDBc::Head->_ssize_active[1]/2;

    if (TkDBc::Head->GetSetup() == 3 && layer == 8) x = -x; // SH FIXME

    int j = (layer-1)*BLAY+(slot+16)*2;
    xll[j] = x+TMath::Sign(dx, x);
    yll[j] = y+TMath::Sign(dy, x); zll[j] = z; j++;
    xll[j] = x+TMath::Sign(dx, x); 
    yll[j] = y-TMath::Sign(dy, x); zll[j] = z;

    if (!TkDBc::Head->FindTkId(-tkid)) {
      j = (layer-1)*BLAY+(-slot+16)*2;
      xll[j] = x-TMath::Sign(dx, x);
      yll[j] = y-TMath::Sign(dy, x); zll[j] = z; j++;
      xll[j] = x-TMath::Sign(dx, x); 
      yll[j] = y+TMath::Sign(dy, x); zll[j] = z;
    }
  }

  for (int i = 0; i < NLAY; i++) {
    int n = 0;
    for (int j = 0; j < BLAY; j++)
      if (zll[i*BLAY+j] != 0) {
	xll[i*BLAY+n] = xll[i*BLAY+j];
	yll[i*BLAY+n] = yll[i*BLAY+j];
	zll[i*BLAY+n] = zll[i*BLAY+j]; n++;
      }
    nll[i] = n+1;
    xll[i*BLAY+n] = xll[i*BLAY];
    yll[i*BLAY+n] = yll[i*BLAY];
    zll[i*BLAY+n] = zll[i*BLAY];

    EGeomType type = Geom_LTRK;
    GVGeom *gobj = new GVGeom(0, typeToGLID(type, i+1), type);
    GVGeom::addObject(gobj);
    gobj->buildGLPLines(nll[i], &xll[i*BLAY], &yll[i*BLAY], &zll[i*BLAY]);
  }
}
