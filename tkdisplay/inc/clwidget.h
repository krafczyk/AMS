// $Id: clwidget.h,v 1.1 2009/06/13 21:40:46 shaino Exp $
//
// ClWidget : a class to manage cluster display by SH
//
#ifndef ClWidget_H
#define ClWidget_H

#include "ui_clwidget.h"

class AMSEventR;

class ClWidget : public QMainWindow {
  Q_OBJECT

public:
  ClWidget(QWidget *parent = 0);

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

  void on_acDisp_activated () { emit changeFocus(); }
  void on_acPrev_activated () { ui.sbIndx->stepDown(); }
  void on_acNext_activated () { ui.sbIndx->stepUp(); }
  void on_acLast_activated () { ui.sbIndx->setValue(ui.sbIndx->maximum()); }
  void on_acFirst_activated() { ui.sbIndx->setValue(ui.sbIndx->minimum()); }
  void on_acClose_activated() { close(); }

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

  AMSEventR *rEvent;

  bool pChain;
  int  pLayer;
  int  pTkID;
};

#endif
