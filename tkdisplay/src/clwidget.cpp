// $Id: clwidget.cpp,v 1.4 2010/01/18 11:17:00 shaino Exp $
#include <QtGui>

#include "clwidget.h"
#include "trcls.h"
#include "infotext.h"

#include <iostream>

ClWidget::ClWidget(TrCls *trcls, QWidget *parent) 
  : QMainWindow(parent), trCls(trcls)
{
  ui.setupUi(this);

  pChain = true;
  pLayer = 0;
  pTkID  = 0;

  ui.tEdit->setReadOnly(true);
  ui.tEdit->setLineWrapMode(QTextEdit::NoWrap);

  setWindowTitle((trCls->getMode() == TrCls::RAW) ? "RawCluster Display" 
		                                  : "Cluster Display");

  ui.sdXax->setRange(ui.sbXax->minimum(), ui.sbXax->maximum());
  connect(ui.sdXax, SIGNAL(valueChanged(int)), ui.sbXax, SLOT(setValue(int)));
  connect(ui.sbXax, SIGNAL(valueChanged(int)), ui.sdXax, SLOT(setValue(int)));
  connect(ui.sbXax, SIGNAL(valueChanged(int)), 
	  ui.qwChist, SLOT(setNBinPnt(int)));

  ui.sdXax->setValue(ui.qwChist->getNBinPnt());
  ui.sdXax->setTickPosition(QSlider::TicksBelow);
  ui.sdXax->setTickInterval(160);
}

void ClWidget::newEvent(AMSEventR *evt)
{
  trCls->newEvent(evt);

  if (trCls->nCluster() == 0) {
    ui.sbIndx->setMinimum(0); ui.sbIndx->setMaximum(0); 
    ui.sbLayr->setMinimum(0); ui.sbLayr->setMaximum(0);
    ui.sbTkid->setMinimum(0); ui.sbTkid->setMaximum(0);
    ui.sbIcls->setMinimum(0); ui.sbIcls->setMaximum(0);
    ui.rbP->setEnabled(false);
    ui.rbN->setChecked(true);
    ui.qwChist->fillCluster(trCls, -1);
    return;
  }

  ui.sbTkid->setMinimum(-815);
  ui.sbTkid->setMaximum(815);

  ui.sbIndx->blockSignals(true);
  ui.sbIndx->setMinimum(0);
  ui.sbIndx->setValue(0);
  ui.sbIndx->setMaximum(trCls->nCluster()-1);
  ui.sbIndx->blockSignals(false);

  int layer0 = 8, layer1 = 1;
  for (int ly = 1; ly <= 8; ly++)
    if (trCls->nTkLayer(ly) > 0) { 
      if (ly < layer0) layer0 = ly;
      if (ly > layer1) layer1 = ly;
    }
  ui.sbLayr->blockSignals(true);
  ui.sbLayr->setMinimum(layer0);
  ui.sbLayr->setValue  (layer0);
  ui.sbLayr->setMaximum(layer1);
  pLayer = layer0;
  ui.sbLayr->blockSignals(false);

  updateDigit();
  updateDisp();
}

void ClWidget::drawCluster(int icls)
{
  if (!trCls->pCluster(icls)) return;
  ui.sbIndx->setValue(icls);
}

void ClWidget::updateDisp()
{
  int icls = (ui.sbIndx->text()).toInt();
  if (!trCls->pCluster(icls)) return;

  ui.qwChist->fillCluster(trCls, icls);
  if (trCls->getMode() == TrCls::RAW)
    ui.tEdit->setPlainText(InfoText::RawClusInfo(trCls->getEvent(), icls));
  else
    ui.tEdit->setPlainText(InfoText::ClusterInfo(trCls->getEvent(), icls));
}

void ClWidget::updateDigit()
{
  if (trCls->nCluster() == 0) return;

  int idx = ui.sbIndx->text().toInt();
  if (!trCls->pCluster(idx)) return;

  int layer = trCls->getLayer(idx);
  int tkid  = trCls->getTkId (idx);
  int side  = trCls->getSide (idx);

  int icls = 0, ncls = trCls->nClsTkId(tkid, side);
  for (int i = 0; i < ncls; i++)
    if (trCls->iClsTkId(tkid, side, i) == idx) {
      icls = i;
      break;
    }
//cout << "updateDigit1 " << layer << " " << tkid << " " << side << endl;

  pChain = false;
  ui.sbLayr->setValue(layer);
  ui.sbTkid->setValue(tkid);

  if (side == 0) ui.rbN->setChecked(true);
  else ui.rbP->setChecked(true);

  ui.sbIcls->setValue(icls);
  pChain = true;
}

void ClWidget::updateLayer()
{
  if (trCls->nCluster() == 0) return;

  int layer = ui.sbLayr->text().toInt();

//cout << "updateLayer0 " << pLayer << " " << layer << endl;

  if (layer < ui.sbLayr->minimum() || ui.sbLayr->maximum() < layer) {
    std::cerr << "Consistency check failed at updateLayer1 " 
	      << ui.sbLayr->minimum() << " " << layer << " " 
	      << ui.sbLayr->maximum() << " " << pLayer << std::endl;
    return;
  }

  while (trCls->nTkLayer(layer) <= 0) {
    if (layer == pLayer) {
      std::cerr << "Consistency check failed at updateLayer2 " 
		<< pLayer << std::endl;
      return;
    }
    if      (pLayer < layer && layer < ui.sbLayr->maximum()) layer++;
    else if (pLayer > layer && layer > ui.sbLayr->minimum()) layer--;
    else layer = pLayer;
  }
  pLayer = layer;
  ui.sbLayr->setValue(layer);

  if (pChain) {
    int tkid = trCls->iTkLayer(layer, 0);

  //cout << "updateLayer1 " << layer << " " << tkid << endl;

    ui.sbTkid->blockSignals(true);
    if (tkid > ui.sbTkid->maximum()) ui.sbTkid->setMaximum(tkid);
    if (tkid < ui.sbTkid->minimum()) ui.sbTkid->setMinimum(tkid);
    ui.sbTkid->setValue(tkid);
    pTkID = tkid;
    ui.sbTkid->blockSignals(false);
    updateNP();
  }
}

void ClWidget::updateTkIdRange()
{
  if (trCls->nCluster() == 0) return;

  int layer = ui.sbLayr->text().toInt();
  int nlad  = trCls->nTkLayer(layer);
  if (nlad <= 0) {
    //cout << "updateTkIdRange-1 layer to be updated" << endl;
    return;
  }

  int tkid0 = trCls->iTkLayer(layer, 0);
  int tkid1 = trCls->iTkLayer(layer, nlad-1);

//cout << "updateTkIdRange1 layer=" << layer 
//       << " nlad= " << nlad << " " << tkid0 << " " << tkid1 << endl;

  ui.sbTkid->blockSignals(true);
  ui.sbTkid->setMinimum(tkid0);
  ui.sbTkid->setValue(tkid0);
  pTkID = tkid0;
  ui.sbTkid->setMaximum(tkid1);
  ui.sbTkid->blockSignals(false);

  if (pChain) updateNP();
}


void ClWidget::updateTkId()
{
  if (trCls->nCluster() == 0) return;

  int tkid = ui.sbTkid->text().toInt();
  if (abs(tkid)/100 < 1 || 8 < abs(tkid)/100 ||
      abs(tkid%100) > 15) {
    ui.sbTkid->setValue(pTkID);
    return;
  }

  int layer = ui.sbLayr->text().toInt();
  int nlad  = trCls->nTkLayer(layer);
  if (nlad <= 0) {
    std::cerr << "Consistency check failed at updateTkId1 " 
	      << layer << " " << tkid << " " << pTkID << std::endl;
    return;
  }

  int nclsn = trCls->nClsTkId(tkid, 0);
  int nclsp = trCls->nClsTkId(tkid, 1);

//cout << "updateTkId1 " << layer << " " << nlad << " | " 
//     << pTkID << " " << tkid << " | " << nclsn << " " << nclsp << endl;

  if (nclsn > 0 || nclsp > 0) {
    //cout << "updateTkId check OK" << endl;
    pTkID = tkid;
    return;
  }
  else if (tkid == pTkID) {
    std::cerr << "Consistency check failed at updateTkId2 " 
	      << layer << " " << tkid << " " << std::endl;
    return;
  }
    
  int tkid0 = trCls->iTkLayer(layer, 0);
  int tkid1 = trCls->iTkLayer(layer, nlad-1);
  if (abs(tkid)/100 != layer || tkid < tkid0 || tkid1 < tkid1) {
    ui.sbTkid->setValue(pTkID);
    return;
  }

  int idx = -1;
  for (int i = 0; i < nlad; i++)
    if (trCls->iTkLayer(layer, i) == pTkID) {
      idx = i;
      break;
    }
  if (idx < 0) {
    std::cerr << "Consistency check failed at updateTkId3 "
	      << layer << " " << nlad << " " << pTkID << " " << tkid 
	      << " " << trCls->getMode()
	      << std::endl;
    return;
  }
  int i0 = (idx == 0)      ? 0      : idx-1;
  int i1 = (idx == nlad-1) ? nlad-1 : idx+1;

  tkid0 = trCls->iTkLayer(layer, i0);
  tkid1 = trCls->iTkLayer(layer, i1);
  tkid = (tkid < pTkID) ? tkid0 : tkid1;

//cout << "updateTkId2 " << idx << " | " << i0 << " " << i1 << " | "
//     << tkid0 << " " << tkid1 << " | " << pTkID << " " << tkid << endl;

  pTkID = tkid;
  ui.sbTkid->setValue(tkid);
}


void ClWidget::updateNP()
{
  if (trCls->nCluster() == 0) return;

  int tkid  = ui.sbTkid->text().toInt();
  int nclsn = trCls->nClsTkId(tkid, 0);
  int nclsp = trCls->nClsTkId(tkid, 1);

  if (nclsn == 0 && nclsp == 0) {
    //cout << "updateNP tkid to be updated" << endl;
    return;
  }

  ui.rbN->setEnabled((nclsn > 0 && nclsp > 0));
  ui.rbP->setEnabled((nclsn > 0 && nclsp > 0));

//cout << "updateNP " << tkid << " " << nclsn << " " << nclsp << endl;

  ui.rbN->blockSignals(true);
  ui.rbP->blockSignals(true);
  if (nclsn > 0) { 
    ui.rbN->setChecked(true); ui.rbP->setChecked(false); }
  else if (nclsp > 0) { 
    ui.rbP->setChecked(true); ui.rbN->setChecked(false); }
  else std::cerr << "Consistency check failed at UpdatNP " 
		 << tkid << " " << nclsn << " " << nclsp << std::endl;
  ui.rbN->blockSignals(false);
  ui.rbP->blockSignals(false);

  if (pChain) updateIclsRange();
}

void ClWidget::updateIclsRange()
{
  if (trCls->nCluster() == 0) return;

  int tkid = ui.sbTkid->text().toInt();
  int side = (ui.rbN->isChecked()) ? 0 : 1;
  int ncls = (tkid != 0) ? trCls->nClsTkId(tkid, side) : 0;

//cout << "updateIclsRange " << tkid << " " << side << " " << ncls << endl;

  ui.sbIcls->blockSignals(true);
  ui.sbIcls->setMinimum(0);
  ui.sbIcls->setValue(0);
  ui.sbIcls->setMaximum((ncls-1 > 0) ? ncls-1 : 0);
  ui.sbIcls->blockSignals(false);

  if (pChain) updateIdx();
}


void ClWidget::updateIdx()
{
  if (trCls->nCluster() == 0) return;

  int tkid  = ui.sbTkid->text().toInt();
  int icls  = ui.sbIcls->text().toInt();
  int side  = (ui.rbN->isChecked()) ? 0 : 1;
  if (tkid == 0) return;

  int ncls = trCls->nClsTkId(tkid, side);
  if (ncls <= 0 || icls < 0 || icls >= ncls) return;

  int idx = trCls->iClsTkId(tkid, side, icls);

  int layer = ui.sbLayr->text().toInt();
//cout << "updateIdx " << layer << " " << tkid << " " << icls << " "
//     << side << " " << ncls << " " << idx << endl;

  if (!pChain) ui.sbIndx->blockSignals(true);
  if (0 <= idx && idx < trCls->nCluster()) ui.sbIndx->setValue(idx);
  if (!pChain) ui.sbIndx->blockSignals(false);
}


void ClWidget::on_rbP_toggled(bool sw)
{
  if (!(sw^ui.rbN->isChecked())) ui.rbN->setChecked(!sw);
  if (sw) updateIclsRange();
}


void ClWidget::on_rbN_toggled(bool sw)
{
  if (!(sw^ui.rbP->isChecked())) ui.rbP->setChecked(!sw);
  if (sw) updateIclsRange();
}
