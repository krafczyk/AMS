// $Id: tkdisplay.h,v 1.8 2010/12/10 21:38:01 shaino Exp $
//
// TkDisplay : a class to manage main window of TkDisplay by SH
//
#ifndef TkDISPLAY_H
#define TkDISPLAY_H

#include "ui_tkdisplay.h"
#include "gltdisp.h"
#include "clwidget.h"
#include "dlevsel.h"

#include "gvconst.h"

class TTreeFormula;
class AMSChain;
class AMSEventR;
class NetFileEngineHandler;

class TkDisplay : public QMainWindow {
  Q_OBJECT

public:
  TkDisplay(QWidget *parent = 0);

  void openFile(QString fname, int entry = 0);
  void openMagf(QString fname, float fscale = 1);
  void errorMsg(int level, bool abort, const char *loc, const char *msg);

  void setAnime(int anime);

  bool getTFError() const { return tfError; }

  static TkDisplay *thisPtr;

signals:
  void quitApp();
  void newEvent(AMSEventR *);
  void closeMbox();
  void cancelScan();
  void drawCluster(int);

private:
  void initUI();
  void initGeom();
  void connectObjs();

  void cbGeomCheck(int sw, int type1, int type2 = -1);
  void cbOptCheck (int sw, int type1, int type2 = -1);
  int  scanEvent(int istep);

private slots:
  void actWindow() { activateWindow(); }
  void drawEvent();
  void updateFormula(QString scut);
  void swipeEvent(int);

  void on_cbUSS_stateChanged (int sw) { ui.glDisp->setGeom(Geom_USS,  sw); }
  void on_cbMag_stateChanged (int sw) { ui.glDisp->setGeom(Geom_MAG,  sw); }
  void on_cbTRD_stateChanged (int sw) { ui.glDisp->setGeom(Geom_TRD,  sw); }
  void on_cbACC_stateChanged (int sw) { ui.glDisp->setGeom(Geom_ACC,  sw); }
  void on_cbRICH_stateChanged(int sw) { ui.glDisp->setGeom(Geom_RICH, sw); }
  void on_cbEcal_stateChanged(int sw) { ui.glDisp->setGeom(Geom_ECAL, sw); }
  void on_cbTOF_stateChanged (int sw) { cbGeomCheck(sw, Geom_TOF, Geom_LTOF); }
  void on_cbTRK_stateChanged (int sw) { cbGeomCheck(sw, Geom_TRK, Geom_LTRK); }

  void on_cbTrak_stateChanged(int sw) { ui.glDisp->setOpt(Draw_TRK, sw); }
  void on_cbHits_stateChanged(int sw) { ui.glDisp->setOpt(Draw_HIT, sw); }
  void on_cbClus_stateChanged(int sw) { ui.glDisp->setOpt(Draw_CLS, sw); }
  void on_cbMCtr_stateChanged(int sw) { cbOptCheck(sw, Draw_MCA, Draw_MCP); }

  void on_pbGmRst_clicked();
  void on_pbGmOn_clicked ();
  void on_pbGmOff_clicked();

  void on_cbGL51_stateChanged(int sw) { ui.glDisp->setLSet(Light_FR, sw); }
  void on_cbGL52_stateChanged(int sw) { ui.glDisp->setLSet(Light_TP, sw); }
  void on_cbGL53_stateChanged(int sw) { ui.glDisp->setLSet(Light_BT, sw); }
  void on_cbGL54_stateChanged(int sw) { ui.glDisp->setLSet(Light_LF, sw); }
  void on_cbGL55_stateChanged(int sw) { ui.glDisp->setLSet(Light_RT, sw); }
  void on_cbGL56_stateChanged(int sw) { ui.glDisp->setLSet(Light_SP, sw); }

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

  void Open();
  void Netf();
  void Magf();
  void Info();
  void Esel();
  void Prev();
  void Next();
  void Frst() { ui.sbEvent->setValue(ui.sbEvent->minimum()); }
  void Last() { ui.sbEvent->setValue(ui.sbEvent->maximum()); }
  void Exit() { QApplication::closeAllWindows(); }
  void Rcam() { ui.glDisp->cReset(); }
  void Draw() { rcWidget->show(); rcWidget->activateWindow(); }
  void Dcls() { clWidget->show(); clWidget->activateWindow(); }
  void More() { ui.glDisp->moreInfo(); }
  void Call() { ui.glDisp->closeAll(); }

#if (QT_VERSION >= 0x040503) 
  // for Qt 4.5.3 or later
  void on_acOpen_triggered() { Open(); }
  void on_acNetf_triggered() { Netf(); }
  void on_acMagf_triggered() { Magf(); }
  void on_acInfo_triggered() { Info(); }
  void on_acEsel_triggered() { Esel(); }
  void on_acPrev_triggered() { Prev(); }
  void on_acNext_triggered() { Next(); }
  void on_acFrst_triggered() { Frst(); }
  void on_acLast_triggered() { Last(); }
  void on_acExit_triggered() { Exit(); }
  void on_acRcam_triggered() { Rcam(); }
  void on_acDcls_triggered() { Dcls(); }
  void on_acDraw_triggered() { Draw(); }
  void on_acMore_triggered() { More(); }
  void on_acCall_triggered() { Call(); }
#else
  // for Qt <= 4.5.2
  void on_acOpen_activated() { Open(); }
  void on_acNetf_activated() { Netf(); }
  void on_acMagf_activated() { Magf(); }
  void on_acInfo_activated() { Info(); }
  void on_acEsel_activated() { Esel(); }
  void on_acPrev_activated() { Prev(); }
  void on_acNext_activated() { Next(); }
  void on_acFrst_activated() { Frst(); }
  void on_acLast_activated() { Last(); }
  void on_acExit_activated() { Exit(); }
  void on_acRcam_activated() { Rcam(); }
  void on_acDcls_activated() { Dcls(); }
  void on_acDraw_activated() { Draw(); }
  void on_acMore_activated() { More(); }
  void on_acCall_activated() { Call(); }
#endif

  void on_pbOpen_clicked() { Open(); }
  void on_pbNetf_clicked() { Netf(); }
  void on_pbPrev_clicked() { Prev(); }
  void on_pbEsel_clicked() { Esel(); }
  void on_pbNext_clicked() { Next(); }
  void on_pbExit_clicked() { Exit(); }

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
  ClWidget *rcWidget;
  ClWidget *clWidget;
  DlEvSel  *dlEvSel;

  AMSChain     *amsChain;
  TTreeFormula *tFormula;
  QString       tfString;
  bool          tfError;

  QString mboxStyle;
  QString flopStyle;
  QString infoStyle;
  QString dlogStyle;
  QString &defaultStyle();
  QString &defMboxStyle();
  QString &defFlopStyle();
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
