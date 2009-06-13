// $Id: clwidget.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>

#include "clwidget.h"
#include "infotext.h"

#include "TrCluster.h"
#include "TrRecon.h"
#include "VCon_root.h"

#include "TMath.h"

ClWidget::ClWidget(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);

  pChain = true;
  pLayer = 0;
  pTkID  = 0;

  ui.tEdit->setReadOnly(true);
  ui.tEdit->setLineWrapMode(QTextEdit::NoWrap);
}

void ClWidget::newEvent(AMSEventR *evt)
{
  rEvent = evt;
  ui.qwChist->newEvent(rEvent);

  if (!TrRecon::gethead()) TrRecon::Create();
  if (!TrRecon::TRCon)     TrRecon::TRCon = new VCon_root;

  TrRecon::gethead()->BuildClusterTkIdMap();
  TrRecon::gethead()->BuildLadderClusterMap();

  if (rEvent->NTrCluster() == 0) {
    ui.sbIndx->setMinimum(0); ui.sbIndx->setMaximum(0); 
    ui.sbLayr->setMinimum(0); ui.sbLayr->setMaximum(0);
    ui.sbTkid->setMinimum(0); ui.sbTkid->setMaximum(0);
    ui.sbIcls->setMinimum(0); ui.sbIcls->setMaximum(0);
    ui.rbP->setEnabled(false);
    ui.rbN->setChecked(true);
    ui.qwChist->drawCluster(-1);

    return;
  }

  ui.sbTkid->setMinimum(-815);
  ui.sbTkid->setMaximum(815);

  ui.sbIndx->blockSignals(true);
  ui.sbIndx->setMinimum(0);
  ui.sbIndx->setValue(0);
  ui.sbIndx->setMaximum(rEvent->NTrCluster()-1);
  ui.sbIndx->blockSignals(false);

  int layer0 = 8, layer1 = 1;
  for (int ly = 0; ly <= 8; ly++)
    if (TrRecon::gethead()->GetNladder(ly) > 0) { 
      if (ly < layer0) layer0 = ly;
      if (ly > layer1) layer1 = ly;
    }
  ui.sbLayr->blockSignals(true);
  ui.sbLayr->setMinimum(layer0);
  ui.sbLayr->setValue(layer0);
  ui.sbLayr->setMaximum(layer1);
  pLayer = layer0;
  ui.sbLayr->blockSignals(false);

  updateDigit();
  updateDisp();
}

void ClWidget::drawCluster(int icls)
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;
  if (!rEvent->pTrCluster(icls)) return;

  ui.sbIndx->setValue(icls);
}

void ClWidget::updateDisp()
{
  if (!rEvent) return;

  int icls = (ui.sbIndx->text()).toInt();

  TrClusterR *cls = rEvent->pTrCluster(icls);
  if (!cls) return;

  ui.qwChist->drawCluster(icls);

  ui.tEdit->setPlainText(InfoText::ClusterInfo(rEvent, icls));
}

void ClWidget::updateDigit()
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int idx = ui.sbIndx->text().toInt();
  if (idx < 0 || rEvent->NTrCluster() <= idx) return;

  TrClusterR *cls = rEvent->pTrCluster(idx);
  if (!cls) return;

  int layer = cls->GetLayer();
  int tkid  = cls->GetTkId();
  int side  = cls->GetSide();

  int icls = 0, ncls = TrRecon::gethead()->GetnTrClusters(tkid, side);
  for (int i = 0; i < ncls; i++)
    if (TrRecon::gethead()->GetTrCluster(tkid, side, i) == cls) {
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
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int layer = ui.sbLayr->text().toInt();

//cout << "updateLayer0 " << pLayer << " " << layer << endl;

  if (layer < ui.sbLayr->minimum() || ui.sbLayr->maximum() < layer) {
    cerr << "Consistency check failed at updateLayer1 " 
	 << ui.sbLayr->minimum() << " " << layer << " " 
	 << ui.sbLayr->maximum() << " " << pLayer << endl;
    return;
  }

  while (TrRecon::gethead()->GetNladder(layer) <= 0) {
    if (layer == pLayer) {
      cerr << "Consistency check failed at updateLayer2 " << pLayer << endl;
      return;
    }
    if      (pLayer < layer && layer < ui.sbLayr->maximum()) layer++;
    else if (pLayer > layer && layer > ui.sbLayr->minimum()) layer--;
    else layer = pLayer;
  }
  pLayer = layer;
  ui.sbLayr->setValue(layer);

  if (pChain) {
    int tkid = TrRecon::gethead()->GetLadderHit(layer, 0);

    //cout << "updateLayer1 " << layer << " " << tkid << endl;

    ui.sbTkid->blockSignals(true);
    ui.sbTkid->setValue(tkid);
    pTkID = tkid;
    ui.sbTkid->blockSignals(false);
    updateNP();
  }
}

void ClWidget::updateTkIdRange()
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int layer = ui.sbLayr->text().toInt();
  int nlad  = TrRecon::gethead()->GetNladder(layer);
  if (nlad <= 0) {
    //cout << "updateTkIdRange-1 layer to be updated" << endl;
    return;
  }

  int tkid0 = TrRecon::gethead()->GetLadderHit(layer, 0);
  int tkid1 = TrRecon::gethead()->GetLadderHit(layer, nlad-1);

//cout << "updateTkIdRange1 layer=" << layer 
//     << " nlad= " << nlad << " " << tkid0 << " " << tkid1 << endl;

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
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int tkid = ui.sbTkid->text().toInt();
  if (TMath::Abs(tkid)/100 < 1 || 8 < TMath::Abs(tkid)/100 ||
      TMath::Abs(tkid%100) > 15) {
    ui.sbTkid->setValue(pTkID);
    return;
  }

  int layer = ui.sbLayr->text().toInt();
  int nlad  = TrRecon::gethead()->GetNladder(layer);
  if (nlad <= 0) {
    cerr << "Consistency check failed at updateTkId1 " 
	 << layer << " " << tkid << " " << pTkID << endl;
    return;
  }

  int nclsn = TrRecon::gethead()->GetnTrClusters(tkid, 0);
  int nclsp = TrRecon::gethead()->GetnTrClusters(tkid, 1);

//cout << "updateTkId1 " << layer << " " << nlad << " | " 
//     << pTkID << " " << tkid << " | " << nclsn << " " << nclsp << endl;

  if (nclsn > 0 || nclsp > 0) {
    //cout << "updateTkId check OK" << endl;
    pTkID = tkid;
    return;
  }
  else if (tkid == pTkID) {
    cerr << "Consistency check failed at updateTkId2 " 
	 << layer << " " << tkid << " " << endl;
    return;
  }
    
  int tkid0 = TrRecon::gethead()->GetLadderHit(layer, 0);
  int tkid1 = TrRecon::gethead()->GetLadderHit(layer, nlad-1);
  if (TMath::Abs(tkid)/100 != layer || tkid < tkid0 || tkid1 < tkid1) {
    ui.sbTkid->setValue(pTkID);
    return;
  }

  int idx = -1;
  for (int i = 0; i < nlad; i++)
    if (TrRecon::gethead()->GetLadderHit(layer, i) == pTkID) {
      idx = i;
      break;
    }
  if (idx < 0) {
    cerr << "Consistency check failed at updateTkId3 "
	 << layer << " " << nlad << " " << pTkID << " " << tkid << endl;
    return;
  }
  int i0 = (idx == 0)      ? 0      : idx-1;
  int i1 = (idx == nlad-1) ? nlad-1 : idx+1;

  tkid0 = TrRecon::gethead()->GetLadderHit(layer, i0);
  tkid1 = TrRecon::gethead()->GetLadderHit(layer, i1);
  tkid = (tkid < pTkID) ? tkid0 : tkid1;

//cout << "updateTkId2 " << idx << " | " << i0 << " " << i1 << " | "
//     << tkid0 << " " << tkid1 << " | " << pTkID << " " << tkid << endl;

  pTkID = tkid;
  ui.sbTkid->setValue(tkid);
}


void ClWidget::updateNP()
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int tkid  = ui.sbTkid->text().toInt();
  int nclsn = TrRecon::gethead()->GetnTrClusters(tkid, 0);
  int nclsp = TrRecon::gethead()->GetnTrClusters(tkid, 1);

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
  else cerr << "Consistency check failed at UpdatNP " 
	    << tkid << " " << nclsn << " " << nclsp << endl;
  ui.rbN->blockSignals(false);
  ui.rbP->blockSignals(false);

  if (pChain) updateIclsRange();
}

void ClWidget::updateIclsRange()
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int tkid = ui.sbTkid->text().toInt();
  int side = (ui.rbN->isChecked()) ? 0 : 1;
  int ncls = (tkid != 0) ? TrRecon::gethead()->GetnTrClusters(tkid, side) : 0;
//cout << "updateIclsRange " << tkid << " " << side << " " << ncls << endl;

  ui.sbIcls->blockSignals(true);
  ui.sbIcls->setMinimum(0);
  ui.sbIcls->setValue(0);
  ui.sbIcls->setMaximum(TMath::Max(ncls-1, 0));
  ui.sbIcls->blockSignals(false);

  if (pChain) updateIdx();
}


void ClWidget::updateIdx()
{
  if (!rEvent || rEvent->NTrCluster() == 0 || !TrRecon::gethead()) return;

  int tkid  = ui.sbTkid->text().toInt();
  int icls  = ui.sbIcls->text().toInt();
  int side  = (ui.rbN->isChecked()) ? 0 : 1;
  if (tkid == 0) return;

  int ncls = TrRecon::gethead()->GetnTrClusters(tkid, side);
  if (ncls <= 0 || icls < 0 || icls >= ncls) return;

  TrClusterR *cls = TrRecon::gethead()->GetTrCluster(tkid, side, icls);

  int idx = -1;
  for (int i = 0; i < rEvent->NTrCluster(); i++)
    if (rEvent->pTrCluster(i) == cls) {
      idx = i;
      break;
    }

  int layer = ui.sbLayr->text().toInt();
//cout << "updateIdx " << layer << " " << tkid << " " << icls << " "
//     << side << " " << ncls << " " << idx << endl;

  if (!pChain) ui.sbIndx->blockSignals(true);
  if (0 <= idx && idx < rEvent->NTrCluster()) ui.sbIndx->setValue(idx);
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
