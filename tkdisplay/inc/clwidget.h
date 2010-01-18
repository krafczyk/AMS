// $Id: clwidget.h,v 1.3 2010/01/18 11:17:00 shaino Exp $
//
// ClWidget : a class to manage cluster display by SH
//
#ifndef ClWidget_H
#define ClWidget_H

#include "ui_clwidget.h"

class TrCls;
class AMSEventR;

class ClWidget : public QMainWindow {
  Q_OBJECT

public:
  ClWidget(TrCls *trcls, QWidget *parent = 0);

public Q_SLOTS:
  void newEvent(AMSEventR *evt);
  void drawCluster(int icls);
  void darkStyle(bool ds) { ui.qwChist->darkStyle(ds); }

signals:
  void changeFocus();

private slots:
  void on_sbIndx_valueChanged(int icls) { updateDigit(); updateDisp(); }
  void on_sbLayr_valueChanged(int ilay) { updateLayer(); updateTkIdRange(); }
  void on_sbTkid_valueChanged(int tkid) { updateTkId (); updateNP(); }
  void on_sbIcls_valueChanged(int icls) { updateIdx(); }
  void on_rbP_toggled(bool);
  void on_rbN_toggled(bool);

  void on_acDisp_triggered () { emit changeFocus(); }
  void on_acPrev_triggered () { ui.sbIndx->stepDown(); }
  void on_acNext_triggered () { ui.sbIndx->stepUp(); }
  void on_acLast_triggered () { ui.sbIndx->setValue(ui.sbIndx->maximum()); }
  void on_acFirst_triggered() { ui.sbIndx->setValue(ui.sbIndx->minimum()); }
  void on_acClose_triggered() { close(); }

  void on_pbXa1_clicked() { ui.sdXax->setValue(ui.sdXax->minimum()); }
  void on_pbXa2_clicked() { ui.sdXax->setValue(ui.sdXax->maximum()); }

protected:
  void updateDisp ();
  void updateDigit();
  void updateLayer();
  void updateTkId ();
  void updateNP   ();
  void updateIdx  ();

  void updateTkIdRange();
  void updateIclsRange();

private:
  Ui::ClWidget ui;

  TrCls *trCls;

  bool pChain;
  int  pLayer;
  int  pTkID;
};

#endif
