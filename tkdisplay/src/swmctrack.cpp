// $Id: swmctrack.cpp,v 1.2 2010/12/13 21:43:08 shaino Exp $
#include <QtGui>

#include "swmctrack.h"
#include "root.h"

#include "TString.h"
#include "TMath.h"

SWMCTrack::SWMCTrack(AMSEventR *event, int tid) 
  : SubWindow(event, SW_TRACK, WIN_W, WIN_H, TBAR_H), tID(tid)
{
}

SWMCTrack::~SWMCTrack()
{
}

void SWMCTrack::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(QRect(0, 0, wWid-2, tHei), QColor(0, 0, 0, 200));
  drawClose(pnt);

  MCEventgR *mct = rEvent->pMCEventg(tID);
  if (!mct) return;

  double momt = mct->Momentum;
  double mass = mct->Mass;
  double beta = (mass > 0) ? fabs(momt/std::sqrt(momt*momt+mass*mass)) : 1;

  pnt->setPen(Qt::white);
  drawText(pnt,  28, 10, Form("MCEventg [%d]", tID));
  drawText(pnt, 130, 10, Form("Particle: %d", mct->Particle));
  drawText(pnt, 130, 30, Form("Momentum: %.2f GeV/c", mct->Momentum));
  drawText(pnt, 340, 30, Form("Beta:     %.3f", beta));

  drawText(pnt, 130, 50, (mct->Mass > 0.9) 
	   ? Form("Mass:           %.3f GeV/c2", mct->Mass)
	   : Form("Mass:           %.3f MeV/c2", mct->Mass*1e3));
  drawText(pnt, 340, 50, Form("Charge: %.0f", mct->Charge));
}
