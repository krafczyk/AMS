// $Id: gltdisp.cpp,v 1.4 2009/11/29 12:17:12 shaino Exp $
#include <QtGui>
#include <QtOpenGL>

#include "gltdisp.h"
#include "tewidget.h"
#include "infotext.h"
#include "swevent.h"
#include "swtrack.h"
#include "swladder.h"

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrCluster.h"
#include "root.h"
#include "amsdbc.h"

#include "TMath.h"

GLTDisp::GLTDisp(QWidget *parent) : GLWidget(parent)
{
  buildLines();

  currEvent = 0;
  drawOpt   = 0;
  darkOpt   = false;
  glLock    = false;

  twEvt = twLad = twCls = twHit = twTrk = 0;

  swEvent  = new SWEvent;
  subWin   = 0;
  lsStatus = LS_NONE;

  aTimer.setSingleShot(false);
  connect(&aTimer, SIGNAL(timeout()), this, SLOT(animate()));
  aTimer.start(LS_WAIT);

  ladVec = 0;

  setAnimeMode();
}

GLTDisp::~GLTDisp()
{
}

void GLTDisp::drawOneEvent(AMSEventR *event)
{
  currEvent = event;
  if (currEvent) {
    updateGL();
    if (twEvt) twEvt->setText(InfoText::EventInfo(currEvent));
    if (twLad) twLad->clearText();
    if (twCls) twCls->clearText();
    if (twHit) twHit->clearText();
    if (twTrk) twTrk->clearText();
  }
}

void GLTDisp::moreInfo()
{
  if (twEvt && twEvt->isVisible()) 
    twEvt->close();
  else {
    twEvt = openTEWidget(twEvt, wtEvt);
    twEvt->setText(InfoText::EventInfo(currEvent));
    twEvt->show();
  }
}

void GLTDisp::closeAll()
{
  if (lsNstep > 0) lsStatus = LS_ACLOSE;
  else closeSubWin();
  emit closeTew();
}

void GLTDisp::closeSubWin()
{
  lsStatus = LS_NONE;
  aStep    = 0;

  if (subWin) {
    delete subWin;
    subWin = 0;
    emit setEnabled(true);
    update();
  }
}

void GLTDisp::setEvtTewType(const QString &type)
{
  wtEvt = Qt::Drawer;
  if (type == "Drawer" ) wtEvt = Qt::Drawer;
  if (type == "Sheet"  ) wtEvt = Qt::Sheet;
  if (type == "Popup"  ) wtEvt = Qt::Popup;
  if (type == "ToolTip") wtEvt = Qt::ToolTip;
  if (type == "Tool"   ) wtEvt = Qt::Tool;
  if (type == "Window" ) wtEvt = Qt::Window;

  if (twEvt) {
    int vis = twEvt->isVisible();
    twEvt->setWindowFlags((Qt::WindowFlags(wtEvt)));
    if (vis) {
      openTEWidget(twEvt);
      twEvt->show();
    }
  }
}

void GLTDisp::setAllTewType(const QString &type)
{
  wtObj = Qt::Drawer;
  if (type == "Drawer" ) wtObj = Qt::Drawer;
  if (type == "Sheet"  ) wtObj = Qt::Sheet;
  if (type == "Popup"  ) wtObj = Qt::Popup;
  if (type == "ToolTip") wtObj = Qt::ToolTip;
  if (type == "Tool"   ) wtObj = Qt::Tool;
  if (type == "Window" ) wtObj = Qt::Window;

  if (twLad) twLad->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twCls) twCls->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twHit) twHit->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twTrk) twTrk->setWindowFlags((Qt::WindowFlags(wtObj)));
}

void GLTDisp::changeAllStyle(const QString &sty)
{
  styleSheet = sty;
  emit changeStyle(styleSheet);
}

void GLTDisp::paintDark(bool sw)
{
  darkOpt = sw;
  update();
}

void GLTDisp::paintLock(bool sw)
{
  if (!glLock && sw) {
    QImage image = grabFrameBuffer();
    glPixmap = QPixmap::fromImage(image); 
    glLock = true;
  }
  if (glLock && !sw) glLock = false;
}

void GLTDisp::mousePressEvent(QMouseEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->isFocus(SubWindow::FOCUS_MORE)) {
      moreInfo();
      return;
    }
    GLWidget::mousePressEvent(event);
    return;
  }

  if (lsStatus != LS_OPEN || !subWin) return;

  subWin->checkFocus(event->x(), event->y());
  if (subWin->isFocus(SubWindow::FOCUS_CLOSE)) {
    closeAll();
    return;
  }

  if (subWin->isFocus(SubWindow::FOCUS_MORE)) {
    if (subWin->getType() == SubWindow::SW_TRACK) {
      if (twTrk && twTrk->isVisible()) twTrk->close();
      else {
	twTrk = openTEWidget(twTrk);
	twTrk->setText(InfoText::TrackInfo(currEvent, subWin->getID()));
	twTrk->show();
      }
    }
    else if (subWin->getType() == SubWindow::SW_LADDER) {
      if (twLad && twLad->isVisible()) twLad->close();
      else {
	twLad = openTEWidget(twLad);
	twLad->setText(InfoText::LadderInfo(currEvent, subWin->getID()));
	twLad->show();
      }
    }
    return;
  }

  if (!subWin->isFocus(SubWindow::FOCUS_DOBJ)) return;

  if (subWin->getType() == SubWindow::SW_LADDER) {
    if (subWin->getDobjType() == SubWindow::DOBJ_CLS)
      emit drawCluster(subWin->getDobjID());

    else if (subWin->getDobjType() >= SubWindow::DOBJ_TRK) {
      int tid = subWin->getDobjType()/SubWindow::DOBJ_TRK-1;
      TrTrackR *trk = currEvent->pTrTrack(tid);	  
      if (trk) {
	delete subWin;
	subWin = new SWTrack(currEvent, tid);
	aStep  = 0;

	if (lsNstep > 0) lsStatus = LS_AOPEN;
	else {
	  lsStatus = LS_OPEN;
	  update();
	}
      }
    }
/*
    else if (subWin->getDobjType() == SubWindow::DOBJ_HIT) {
      twHit = openTEWidget(twHit);
      twHit->setText(InfoText::HitInfo(currEvent, 
				       subWin->getDobjID()));
      twHit->show();
    }
*/
  }
  else if (subWin->getType() == SubWindow::SW_TRACK) {
    int tkid = 0;
    if (subWin->getDobjType() == SubWindow::DOBJ_HIT) {
      int hid = ((SWTrack *)subWin)->getFocusHid();
      TrTrackR  *trk = currEvent->pTrTrack(subWin->getID());
      TrRecHitR *hit = trk->GetHit(hid);
      if (hit) tkid = hit->GetTkId();
    }
    else if (subWin->getDobjType() == SubWindow::DOBJ_LAD) 
      tkid = subWin->getDobjID();

    if (tkid != 0) {
      delete subWin;
      subWin = new SWLadder(currEvent, tkid);
      aStep  = 0;

      if (lsNstep > 0) lsStatus = LS_AOPEN;
      else {
	lsStatus = LS_OPEN;
	update();
      }
    }
  }
}

void GLTDisp::mouseMoveEvent(QMouseEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->checkFocus(event->x(), event->y())) {
      update();
      return;
    }

    GLWidget::mouseMoveEvent(event);
    return;
  }

  if (subWin && subWin->checkFocus(event->x(), event->y())) update();
}

void GLTDisp::leaveEvent(QEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->clearFocus()) update();
    GLWidget::leaveEvent(event);
    return;
  }

  if (lsStatus == LS_OPEN && subWin && subWin->clearFocus()) update();
}

void GLTDisp::wheelEvent(QWheelEvent *event)
{
  if (lsStatus == LS_NONE) {
    GLWidget::wheelEvent(event);
    return;
  }
}

void GLTDisp::keyPressEvent(QKeyEvent *event)
{
  if (lsStatus == LS_NONE) {
    GLWidget::keyPressEvent(event);
    return;
  }

  if (lsStatus == LS_OPEN && event->key() == Qt::Key_Escape)
    closeAll();
}

void GLTDisp::paintEvent(QPaintEvent *)
{
  makeCurrent();

  if (!glLock)
    paintGL();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  if (glLock)
    painter.drawPixmap(0, 0, glPixmap);
  else {
    swEvent->setEvent(currEvent);
    swEvent->drawWindow(&painter, vpWidth, vpHeight);
  }

  if (subWin)
    subWin->drawWindow(&painter, vpWidth, vpHeight, 
		       (lsNstep > 0) ? (float)aStep/lsNstep : 1);

  if (darkOpt)
    painter.fillRect(0, 0, vpWidth, vpHeight, QColor(0, 0, 0, 150));

  painter.end();
}

void GLTDisp::drawObject(GLenum mode)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  drawTracker (mode);
  drawTracks  (mode);
  drawMCTracks(mode);

  if (mode == GL_RENDER) {
    drawClusters();
    drawHits();
    drawAxes();
  }
}

void GLTDisp::drawTracker(GLenum mode)
{
  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init();
  }

  if (!ladVec) {
    int nlad = TkDBc::Head->GetEntries();
    ladVec = new int[nlad];

    for (int i = 0, n = 0; i < 8; i++) {
      for (int j = 0; j < nlad; j++) {
	TkLadder *lad = TkDBc::Head->GetEntry(j);
	if (lad->GetLayer() != 8-i) continue;
	ladVec[n++] = j;
      }
    }
  }

  double alpha = 0.6;

  static const double clad[8] = { 0.0, 1.0, 0.0, alpha,
				  0.0, 0.8, 0.8, alpha };
  static const double cclx[8] = { 0.9, 0.0, 0.0, alpha,
				  0.9, 0.4, 0.4, alpha };
  static const double ccly[8] = { 0.0, 0.0, 0.9, alpha,
				  0.4, 0.4, 0.9, alpha };
  static const double cclc[8] = { 0.9, 0.0, 0.9, alpha,
				  0.9, 0.4, 0.9, alpha };
  static const double cnum[4] = { 0.9, 0.9, 0.9, alpha };

  int clsx[8*35], clsy[8*35];
  fillHitVec(clsx, clsy);

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

    int sid = ladVec[i]+1;

    const double *cpar = 0;
    if (mode == GL_RENDER) {
      if (clsx[lid] && clsy[lid]) cpar = cclc;
      else if (clsx[lid])         cpar = cclx;
      else if (clsy[lid])         cpar = ccly;
      else if (drawOpt & ALLTRK)  cpar = clad;

      if (idSel == sid) cpar = (cpar) ? &cpar[4] : &clad[4];
    }
    else if (mode == GL_SELECT) glLoadName(sid);

    if (cpar) glMatCol(cpar);

    if (cpar || mode == GL_SELECT) glBox(x, y, z, dx, dy, dz);

    if (mode == GL_RENDER && idSel == sid) {
      glMatCol(cnum);
      glDigit(x+TMath::Sign(dx-15, x)-8, y, z+1, tkid, -2, 3.5);
      glDigit(x+TMath::Sign(dx-15, x)-8, y, z-1, tkid,  2, 3.5);
    }
  }

  if (mode == GL_RENDER && !(drawOpt & ALLTRK)) {
    glMatCol(clad);
    for (int i = 0; i < 8; i++)
      glPLines(nLadLine[i], &xLadLine[i*64], &yLadLine[i*64], &zLadLine[i*64]);
  }
}

void GLTDisp::drawTracks(GLenum mode)
{
  if (!currEvent) return;
  if (!(drawOpt & TRACK)) return;

  static const double ctrk[4] = { 0.9, 0.0, 0.0, 1.0 };
  static const double csel[4] = { 0.9, 0.4, 0.4, 1.0 };

  for (int i = 0; i < currEvent->nTrTrack(); i++) {

    TrTrackR *trk = currEvent->pTrTrack(i);

    int fid = TrTrackR::DefaultFitID;  //kChoukto;
    if (!trk->ParExists(fid)) fid = TrTrackR::kLinear;

    if (fid == TrTrackR::kLinear) {
      double z[2] = { -70, 70 };
      double p0z  =   trk->GetP0z(fid);
      double x[2] = { trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[0]-p0z), 
		      trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[1]-p0z) };
      double y[2] = { trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[0]-p0z), 
		      trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[1]-p0z) };

      int sid = i+SID_OFFS_TRACK;

      if (mode == GL_RENDER) {
	glLineWidth(2);
	if (idSel == sid) glMatCol(csel);
	else              glMatCol(ctrk);
      }
      else if (mode == GL_SELECT) glLoadName(sid);
      glPLines(2, x, y, z);
    }
    else {
      enum { NPL = 20 };
      double   zpl [NPL+1];
      AMSPoint pint[NPL+1];
      for (int j = 0; j <= NPL; j++) zpl[j] = -70.+140.*j/NPL;

      TrProp trp(trk->GetP0(), trk->GetDir(), trk->GetRigidity());
      trp.Interpolate(NPL+1, zpl, pint);

      double xp[NPL+1], yp[NPL+1], zp[NPL+1];
      for (int j = 0; j <= NPL; j++) {
	xp[j] = pint[j].x();
	yp[j] = pint[j].y();
	zp[j] = pint[j].z();
      }

      int sid = i+SID_OFFS_TRACK;

      if (mode == GL_RENDER) {
	glLineWidth(2);
	if (idSel == sid) glMatCol(csel);
	else              glMatCol(ctrk);
      }
      else if (mode == GL_SELECT) glLoadName(sid);
      glPLines(NPL+1, xp, yp, zp);
    }
  }
}

void GLTDisp::drawMCTracks(GLenum mode)
{
  if (!currEvent) return;
  if (!(drawOpt & TRACK)) return;

  static const double ctrk[4] = { 0.9, 0.0, 0.0, 1.0 };
  static const double csel[4] = { 0.9, 0.4, 0.4, 1.0 };

  for (int i = 0; i < currEvent->nMCEventg(); i++) {

    int sid = i+SID_OFFS_TRACK;
    if (mode == GL_RENDER) {
      glLineWidth(2);
      if (idSel == sid) glMatCol(csel);
      else              glMatCol(ctrk);
    }
    else if (mode == GL_SELECT) glLoadName(sid);

    MCEventgR *mct = currEvent->pMCEventg(i);

    if (mct->Charge == 0) {
      double z[2] = { -70, 70 };
      double p0z  =   mct->Coo[2];
      double dxz  =   mct->Dir[0]/mct->Dir[2];
      double dyz  =   mct->Dir[1]/mct->Dir[2];
      double x[2] = { mct->Coo[0]+dxz*(z[0]-p0z), mct->Coo[0]+dxz*(z[1]-p0z) };
      double y[2] = { mct->Coo[1]+dyz*(z[0]-p0z), mct->Coo[1]+dyz*(z[1]-p0z) };

      glPLines(2, x, y, z);
    }
    else {
      enum { NPL = 20 };
      double   zpl [NPL+1];
      AMSPoint pint[NPL+1];
      for (int j = 0; j <= NPL; j++) zpl[j] = -70.+140.*j/NPL;

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

      glPLines(NPL+1, xp, yp, zp);
    }
  }
}

void GLTDisp::drawClusters()
{
  if (!currEvent) return;
  if (!(drawOpt & TRKCLS)) return;

  static const double ccls[4] = { 0.0, 0.9, 0.9, 1.0 };

  for (int i = 0; (drawOpt & TRKCLS) && i < currEvent->nTrCluster(); i++) {
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

    cls->BuildCoordinates();
    double px[2], py[2], pz[4];
    for (int j = 0; j < cls->GetMultiplicity(); j++) {
      double coo = cls->GetGCoord(j);
      px[0] = (side == 0) ? coo : x-dx;
      px[1] = (side == 0) ? coo : x+dx;
      py[0] = (side == 1) ? coo : y-dy;
      py[1] = (side == 1) ? coo : y+dy;
      pz[0] = pz[1] = z+0.3; pz[2] = pz[3] = z-0.3;
      glMatCol(ccls);
      glPLines(2, px, py, pz);
      glPLines(2, px, py, pz+2);
    }
  }
}

void GLTDisp::drawHits()
{
  if (!currEvent) return;
  if (!(drawOpt & TRKHIT)) return;

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

      glMatCol(chit);
      glSphere(x, y, z, 1);
    }
  }
}

void GLTDisp::drawAxes()
{
  static const double col[4] = { 0.7, 0.7, 0.7, 1.0 };
  glMatCol(col);
  glLineWidth(0.5);

  double p0 = -65, p1 = -40;
  glLine(p0, p0, p0, p1, p0, p0);
  glLine(p0, p0, p0, p0, p1, p0);
  glLine(p0, p0, p0, p0, p0, p1);

  glLineWidth(1);
}

void GLTDisp::processPick()
{
  if (lsStatus != LS_NONE) return;

  if (idSel < SID_OFFS_TRACK) {
    TkLadder *lad = TkDBc::Head->GetEntry(idSel-1);
    if (!lad) return;

    subWin = new SWLadder(currEvent, lad->GetTkId());
  }
  else {
    int tid = idSel-SID_OFFS_TRACK;
    if (!currEvent->pTrTrack(tid)) return;

    subWin = new SWTrack(currEvent, tid);
  }

  if (subWin) {
    emit setEnabled(false);
    aStep = 0;

    if (lsNstep > 0) lsStatus = LS_AOPEN;
    else {
      lsStatus = LS_OPEN;
      update();
    }
  }
}

TEWidget *GLTDisp::openTEWidget(TEWidget *tew, int wtype)
{
  if (wtype < 0) wtype = wtObj;

  if (!tew) {
    tew = new TEWidget(this);
    if (wtype != Qt::Drawer) {
      tew->setMinimumWidth (480);
      tew->setMinimumHeight(480);
    }
    tew->setStyleSheet(styleSheet);
    tew->setWindowFlags((Qt::WindowFlags(wtype)));

    connect(this, SIGNAL(closeTew()), tew, SLOT(close()));
    connect(this, SIGNAL(changeStyle(QString)), 
	       tew, SLOT(changeStyle(QString)));
  }

  QPoint pos = tew->pos();
  if (pos.x() < 0) pos.setX(0);
  if (pos.y() < 0) pos.setY(0);
  tew->move(pos);

  return tew;
}

void GLTDisp::buildLines()
{
  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init();
  }

  for (int i = 0; i < 512; i++)
    xLadLine[i] = yLadLine[i] = zLadLine[i] = 0;

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

    int j = (layer-1)*64+(slot+16)*2;
    xLadLine[j] = x+TMath::Sign(dx, x);
    yLadLine[j] = y+TMath::Sign(dy, x); zLadLine[j] = z; j++;
    xLadLine[j] = x+TMath::Sign(dx, x); 
    yLadLine[j] = y-TMath::Sign(dy, x); zLadLine[j] = z;

    if (!TkDBc::Head->FindTkId(-tkid)) {
      j = (layer-1)*64+(-slot+16)*2;
      xLadLine[j] = x-TMath::Sign(dx, x);
      yLadLine[j] = y-TMath::Sign(dy, x); zLadLine[j] = z; j++;
      xLadLine[j] = x-TMath::Sign(dx, x); 
      yLadLine[j] = y+TMath::Sign(dy, x); zLadLine[j] = z;
    }
  }

  for (int i = 0; i < 8; i++) {
    int n = 0;
    for (int j = 0; j < 64; j++)
      if (zLadLine[i*64+j] != 0) {
	xLadLine[i*64+n] = xLadLine[i*64+j];
	yLadLine[i*64+n] = yLadLine[i*64+j];
	zLadLine[i*64+n] = zLadLine[i*64+j]; n++;
      }
    nLadLine[i] = n+1;
    xLadLine[i*64+n] = xLadLine[i*64];
    yLadLine[i*64+n] = yLadLine[i*64];
    zLadLine[i*64+n] = zLadLine[i*64];
  }
}

void GLTDisp::fillHitVec(int *clsx, int *clsy)
{
  for (int i = 0; i < 8*35; i++) clsx[i] = clsy[i] = 0;

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

void GLTDisp::setAnimeMode(int mode)
{
  if (mode == AN_NORMAL) {
    lsNstep = LS_NSTEP;
    lsWait  = LS_WAIT;
  }
  else if (mode == AN_LIGHT) {
    lsNstep = 3;
    lsWait  = 50;
  }
  else {
    lsNstep = 0;
    lsWait  = 1000;
  }

  aTimer.setInterval(lsWait);
}

void GLTDisp::animate()
{
  if (lsStatus != LS_AOPEN && lsStatus != LS_ACLOSE) return;

  if (lsStatus == LS_AOPEN  && ++aStep >= lsNstep) lsStatus = LS_OPEN;

  if (lsStatus == LS_ACLOSE && --aStep <= 0) closeSubWin();

  update();
}
