// $Id: tkdisplay.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// TkDisplay : a class to manage main window of TkDisplay by SH
//
#ifndef TkDISPLAY_H
#define TkDISPLAY_H

#include "ui_tkdisplay.h"
#include "gltdisp.h"
#include "clwidget.h"
#include "dlevsel.h"

class TTreeFormula;
class AMSChain;
class AMSEventR;

class TkDisplay : public QMainWindow {
  Q_OBJECT

public:
  TkDisplay(QWidget *parent = 0);

  void openFile(QString fname, int entry = 0);
  void errorMsg(int level, bool abort, const char *loc, const char *msg);

  bool getTFError() const { return tfError; }

  static TkDisplay *thisPtr;

signals:
  void quitApp();
  void newEvent(AMSEventR *);
  void closeMbox();
  void cancelScan();
  void drawCluster(int);

private:
  int scanEvent(int istep);

private slots:
  void actWindow() { activateWindow(); }
  void drawEvent();
  void updateFormula(QString scut);

  void on_cbGL11_stateChanged(int sw) { ui.glDisp->setOpt(ALLTRK, sw); }
  void on_cbGL12_stateChanged(int sw) { ui.glDisp->setOpt(TRKCLS, sw); }
  void on_cbGL13_stateChanged(int sw) { ui.glDisp->setOpt(TRKHIT, sw); }
  void on_cbGL14_stateChanged(int sw) { ui.glDisp->setOpt(TRACK,  sw); }
  void on_cbGL16_stateChanged(int sw) { ui.glDisp->setOpt(MCTRK,  sw); }
  void on_cbGL18_stateChanged(int sw) { ui.glDisp->setOpt(MCCLS,  sw); }
  void on_cbGL21_stateChanged(int sw) { ui.glDisp->setOpt(ANYTOF, sw); }
  void on_cbGL22_stateChanged(int sw) { ui.glDisp->setOpt(ALLTOF, sw); }
  void on_cbGL23_stateChanged(int sw) { ui.glDisp->setOpt(TOFHIT, sw); }

  void on_cbGL51_stateChanged(int sw) { ui.glDisp->setLSet(LFRT, sw); }
  void on_cbGL52_stateChanged(int sw) { ui.glDisp->setLSet(LTOP, sw); }
  void on_cbGL53_stateChanged(int sw) { ui.glDisp->setLSet(LBTM, sw); }
  void on_cbGL54_stateChanged(int sw) { ui.glDisp->setLSet(LLFT, sw); }
  void on_cbGL55_stateChanged(int sw) { ui.glDisp->setLSet(LRGT, sw); }
  void on_cbGL56_stateChanged(int sw) { ui.glDisp->setLSet(LSPC, sw); }

  void on_pbGLrst_clicked() { ui.glDisp->cReset(); }

  void on_pbGLlon_clicked()  { ui.cbGL51->setCheckState(Qt::Checked);
                               ui.cbGL52->setCheckState(Qt::Checked);
                               ui.cbGL53->setCheckState(Qt::Checked);
                               ui.cbGL54->setCheckState(Qt::Checked);
                               ui.cbGL55->setCheckState(Qt::Checked);
                               ui.cbGL56->setCheckState(Qt::Checked); 
			       ui.sdLL->setValue(150);
			       ui.sdLZ->setValue(  0); }

  void on_pbGLloff_clicked() { ui.cbGL51->setCheckState(Qt::Unchecked);
                               ui.cbGL52->setCheckState(Qt::Unchecked);
                               ui.cbGL53->setCheckState(Qt::Unchecked);
                               ui.cbGL54->setCheckState(Qt::Unchecked);
                               ui.cbGL55->setCheckState(Qt::Unchecked);
                               ui.cbGL56->setCheckState(Qt::Unchecked); }

  void on_acOpen_activated();
  void on_acInfo_activated();
  void on_acEsel_activated();
  void on_acPrev_activated();
  void on_acNext_activated();
  void on_acFrst_activated() { ui.sbEvent->setValue(ui.sbEvent->minimum()); }
  void on_acLast_activated() { ui.sbEvent->setValue(ui.sbEvent->maximum()); }
  void on_acExit_activated() { QApplication::closeAllWindows(); }
  void on_acRcam_activated() { ui.glDisp->cReset(); }
  void on_acDcls_activated() { clWidget->show(); clWidget->activateWindow(); }
  void on_acMore_activated() { ui.glDisp->moreInfo(); }
  void on_acCall_activated() { ui.glDisp->closeAll(); }

  void on_pbOpen_clicked() { on_acOpen_activated(); }
  void on_pbPrev_clicked() { on_acPrev_activated(); }
  void on_pbEsel_clicked() { on_acEsel_activated(); }
  void on_pbNext_clicked() { on_acNext_activated(); }
  void on_pbExit_clicked() { on_acExit_activated(); }

  void on_sbEvent_valueChanged(int) { drawEvent(); }

  void on_cbWtOb_activated(const QString &str)
    { ui.glDisp->setAllTewType(str); }

  void on_cbWtEv_activated(const QString &str)
    { ui.glDisp->setEvtTewType(str); }

  void on_cbStyle_activated(const QString &name) { changeStyle(name); }
  void changeStyle(const QString &name = "");

  void on_cbAnime_activated(const QString &mode) { changeAnime(mode); }
  void changeAnime(const QString &mode);

  void gl_set_enabled_all(bool);

private:
  Ui::TkDisplay ui;
  ClWidget *clWidget;
  DlEvSel  *dlEvSel;

  AMSChain     *amsChain;
  TTreeFormula *tFormula;
  QString       tfString;
  bool          tfError;

  QString mboxStyle;
  QString infoStyle;
  QString dlogStyle;
  QString &defaultStyle();
  QString &defMboxStyle();
  QString &defInfoStyle();
  QString &defDlogStyle();
};

#include <QMessageBox>

class ScanMessageBox : public QMessageBox {
  Q_OBJECT

public:
  ScanMessageBox(QWidget *parent) 
    : QMessageBox(QMessageBox::Information, "Scanning ...",
		  "Scanning events\n  please wait ...",
		  QMessageBox::Cancel, parent) {}

public Q_SLOTS:
  void updateEntry(int ent, int tot) {
    setText("Scanning events ("+QString::number(ent)+"/"
	                       +QString::number(tot)+")\n  please wait ...");
    update();
  }
};

#endif
