// $Id: tkdisplay.cpp,v 1.3 2009/11/23 21:27:31 shaino Exp $
#include <QtGui>
#include <QFileDialog>

#include "tkdisplay.h"
#include "clwidget.h"
#include "glwidget.h"
#include "evthread.h"

#include "root.h"
#include "amschain.h"
#include "TrParDB.h"

#include "TTreeFormula.h"
#include "TError.h"
#include "TROOT.h"

TkDisplay *TkDisplay::thisPtr = 0;

TkDisplay::TkDisplay(QWidget *parent) : QMainWindow(parent)
{
  ui.setupUi(this);

  clWidget = new ClWidget;
  dlEvSel  = new DlEvSel(this);

  ui.sdFv->setRange(GLWidget::FOV_MIN, GLWidget::FOV_MAX);
  ui.sdDl->setRange(GLWidget::DOL_MIN, GLWidget::DOL_MAX);
  ui.sdRv->setRange(                      0, 360*GLWidget::ROT_SCALE);
  ui.sdRh->setRange(-90*GLWidget::ROT_SCALE,  90*GLWidget::ROT_SCALE);

  ui.sdLL->setRange(   1, 200);
  ui.sdLZ->setRange(-500, 500);

  on_pbGLlon_clicked();
  ui.glDisp->setLSize(ui.sdLL->value());
  ui.glDisp->setLZpos(ui.sdLZ->value());

  connect(ui.sdFv, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(cZoom  (int)));
  connect(ui.sdDl, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(cDolly (int)));
  connect(ui.sdRh, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(hRotate(int)));
  connect(ui.sdRv, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(vRotate(int)));

  connect(ui.glDisp, SIGNAL(zChanged(int)), ui.sdFv, SLOT(setValue(int)));
  connect(ui.glDisp, SIGNAL(dChanged(int)), ui.sdDl, SLOT(setValue(int)));
  connect(ui.glDisp, SIGNAL(hRotated(int)), ui.sdRh, SLOT(setValue(int)));
  connect(ui.glDisp, SIGNAL(vRotated(int)), ui.sdRv, SLOT(setValue(int)));

  connect(ui.sdLL, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(setLSize(int)));
  connect(ui.sdLZ, SIGNAL(valueChanged(int)), ui.glDisp, SLOT(setLZpos(int)));

  connect(ui.glDisp, SIGNAL(setEnabled(bool)), 
	  this, SLOT(gl_set_enabled_all(bool)));

  connect(ui.glDisp, SIGNAL(drawCluster(int)),
	  clWidget,  SLOT  (drawCluster(int)));
  connect(ui.glDisp, SIGNAL(drawCluster(int)),
	  clWidget,  SLOT  (show()));
  connect(this,      SIGNAL(newEvent(AMSEventR *)),
	  clWidget,  SLOT  (newEvent(AMSEventR *)));
  connect(clWidget,  SIGNAL(changeFocus()),
	  this,      SLOT  (actWindow()));

  connect(dlEvSel, SIGNAL(updateFormula(QString)), 
	  this,    SLOT  (updateFormula(QString)));


  ui.glDisp->setOpt(ALLTRK, (ui.cbGL11->checkState()&Qt::Checked));
  ui.glDisp->setOpt(TRKCLS, (ui.cbGL12->checkState()&Qt::Checked));
  ui.glDisp->setOpt(TRKHIT, (ui.cbGL13->checkState()&Qt::Checked));
  ui.glDisp->setOpt(TRACK,  (ui.cbGL14->checkState()&Qt::Checked));
  ui.glDisp->setOpt(MCTRK,  (ui.cbGL16->checkState()&Qt::Checked));
  ui.glDisp->setOpt(MCCLS,  (ui.cbGL18->checkState()&Qt::Checked));
  ui.glDisp->setOpt(ANYTOF, (ui.cbGL21->checkState()&Qt::Checked));
  ui.glDisp->setOpt(ALLTOF, (ui.cbGL22->checkState()&Qt::Checked));
  ui.glDisp->setOpt(TOFHIT, (ui.cbGL23->checkState()&Qt::Checked));

#ifndef Q_WS_MAC
  ui.cbWtEv->removeItem(ui.cbWtEv->findText("Drawer"));
  ui.cbWtOb->removeItem(ui.cbWtOb->findText("Drawer"));
  ui.cbWtEv->setCurrentIndex(ui.cbWtEv->findText("Tool"));
  ui.cbWtOb->setCurrentIndex(ui.cbWtOb->findText("Tool"));
#endif

  ui.cbStyle->addItem("Default");
  ui.cbStyle->addItems(QStyleFactory::keys());

  changeStyle("Default");
  ui.glDisp->setEvtTewType(ui.cbWtEv->currentText());
  ui.glDisp->setAllTewType(ui.cbWtOb->currentText());

  ui.glDisp->cReset();

  clWidget->hide();
  dlEvSel ->hide();

  ui.acEsel->setEnabled(false);
  ui.pbEsel->setEnabled(false);

  amsChain = 0;
  tFormula = 0;

  thisPtr = this;
}

#include "TrRecon.h"

void TkDisplay::openFile(QString fname, int entry)
{
  if (amsChain) delete amsChain;
  amsChain = new AMSChain;

  ////SH
  TrRecon::ReadMagField("dat/MagneticFieldMap07.dat");
  ////SH

  if (TrParDB::Head) {
    delete TrParDB::Head;
    TrParDB::Head = 0;
  }

  amsChain->Reset();
  if (amsChain->Add(fname.toAscii().data()) > 0 && 
      amsChain->GetEntries() > 0) {
    ui.sbEvent->setRange(0, amsChain->GetEntries()-1);
    ui.acEsel->setEnabled(true);
    ui.pbEsel->setEnabled(true);

    int draw = 0;
    if ((int)ui.sbEvent->value() == entry) draw = 1;

    ui.sbEvent->setValue(entry);
    if (draw) drawEvent();

    if (tFormula) updateFormula(tFormula->GetTitle());
  }
}

void TkDispError(int level, Bool_t abort, 
		 const char *loc, const char *msg)
{
  if (TkDisplay::thisPtr)
    TkDisplay::thisPtr->errorMsg(level, abort, loc, msg);
}

void TkDisplay::errorMsg(int level, bool abort, 
			 const char *loc, const char *msg)
{
  QString sloc = loc;
  QString str  = "Error in <"+sloc+">:\n"+msg;

  if (sloc == "TTreeFormula::Compile") {
    str += "\nFormula= \""+tfString+"\"";
    tfError = true;

    QMessageBox *mbox = new QMessageBox(QMessageBox::Information,
					"Error", str,
					QMessageBox::Ok, dlEvSel);
    mbox->setIconPixmap(QPixmap(QLatin1String(":/images/ams02t.png")));
    mbox->setStyleSheet(mboxStyle);
    mbox->setAttribute(Qt::WA_DeleteOnClose);
    mbox->exec();
  }
}

void TkDisplay::updateFormula(QString scut)
{
  if (!amsChain) return;

  if (tFormula) delete tFormula;
  tFormula = 0;
  tfError  = false;

  if (scut != "") {
    SetErrorHandler(TkDispError);
    tfString = scut;
    tFormula = new TTreeFormula("tfm", scut.toAscii().data(), amsChain);

    if (tfError) {
      delete tFormula;
      tFormula = 0;
    }
  }
}

int TkDisplay::scanEvent(int istep)
{
  int pent = (ui.sbEvent->text()).toInt(); 
  if (!amsChain) return pent;

  ui.glDisp->paintLock(true);
  
  ScanMessageBox *mbox = new ScanMessageBox(this);
  mbox->setIconPixmap(QPixmap(QLatin1String(":/images/ams02t.png")));
  mbox->setStyleSheet(mboxStyle);
  mbox->setAttribute(Qt::WA_DeleteOnClose);
  mbox->updateEntry(pent, amsChain->GetEntries());

  EvThread *thread = new EvThread(this, amsChain, tFormula, pent, istep);

  connect(thread, SIGNAL(finished()), mbox, SLOT(accept()));
  connect(mbox,   SIGNAL(buttonClicked(QAbstractButton*)),
	  thread, SLOT(cancelScan()));
  connect(thread, SIGNAL(updateEntry(int, int)),
	  mbox,   SLOT  (updateEntry(int, int)));
  thread->start();

  mbox->exec();

  int ent = thread->getEntry();
  delete thread;

  if (ent == pent) amsChain->GetEvent(ent);

  ui.glDisp->paintLock(false);

  return ent;
}

void TkDisplay::drawEvent()
{
  if (amsChain) {
    int ent = (ui.sbEvent->text()).toInt();
    AMSEventR *event = amsChain->GetEvent(ent);
    if (event) {
      ui.glDisp->drawOneEvent(event);
      emit newEvent(event);
    }
  }
}

void TkDisplay::Open()
{
  QFileDialog qfd(this, "Open File...", ".", "Root files (*.root)");
#ifndef Q_WS_MAC
  qfd.setStyleSheet("QPushButton { background-color: white; }"
		    "QPushButton, QLabel { color: black; }"
		    "QWidget, QLabel { border: 0px solid black; "
		    "                  border-radius: 0px }"
		    "QPushButton { border: 1px solid black; "
		    "              border-radius: 1px }");
#endif
  if (qfd.exec() == QDialog::Accepted)
    openFile(qfd.selectedFiles().value(0));
}

void TkDisplay::Info()
{
  QMessageBox *mbox = new QMessageBox(QMessageBox::Information,
				      QLatin1String("About TkDisplay"), 
    QMessageBox::tr("<h3>AMS-02 Tracker <br>"
		    "   event display</h3>"
		    "<p>by S.Haino <br>"
		    "   (Sadakazu.Haino@pg.infn.it)</p>"
		    "<p>CVS $Revision: 1.3 $<br>"
		    "   CVS $Date: 2009/11/23 21:27:31 $</p>"
		    "<p>Compiled: <br> at %1 on %2</p>"
		    "<p>Qt version: %3</p>"
		    "<p>ROOT version: %4</p>").arg(
		      QLatin1String(__TIME__), QLatin1String(__DATE__), 
		      QLatin1String(QT_VERSION_STR), 
		      QLatin1String(gROOT->GetVersion())), 
				      QMessageBox::Ok, this);

  mbox->setIconPixmap(QPixmap(QLatin1String(":/images/ams02s.png")));
  mbox->setStyleSheet(infoStyle);
  mbox->setAttribute(Qt::WA_DeleteOnClose);

  ui.glDisp->paintDark(true);
  mbox->exec();
  ui.glDisp->paintDark(false);
}

void TkDisplay::Esel()
{
  dlEvSel->setStyleSheet(dlogStyle);
  dlEvSel->show();
}

void TkDisplay::Prev()
{
  if (tFormula) ui.sbEvent->setValue(scanEvent(-1));
  else          ui.sbEvent->stepDown(); 
}

void TkDisplay::Next()
{
  if (tFormula) ui.sbEvent->setValue(scanEvent(1));
  else          ui.sbEvent->stepUp(); 
}

#include <fstream>

void TkDisplay::changeStyle(const QString &name)
{
  mboxStyle = "";
  infoStyle = "";
  dlogStyle = "";

  if (name == "Default" || name == "") {
#ifdef Q_WS_MAC
    QApplication::setStyle(QStyleFactory::create("Macintosh (aqua)"));
#else
    QApplication::setStyle(QStyleFactory::create("Cleanlooks"));
#endif

    QString str;
    std::ifstream fin("tkdisplay.sty");
    if (fin.good()) {
      char buf[1024];
      while (!fin.eof()) {
	fin.getline(buf, 1024);
	if (buf[0] != '#') str += buf;
      }
    }
    else
      str = defaultStyle();

    setStyleSheet(str);
    ui.glDisp->changeAllStyle(str);
    str += "QFrame { background-color: qlineargradient("
                    "spread:pad, x1: 0, y1: 0, x2: 0, y2: 1,"
                    "stop: 1 rgb( 0,  5,  0), stop: 0 rgb( 0, 30, 10)); }";
    clWidget->setStyleSheet(str);
    clWidget->darkStyle(true);

    mboxStyle = defMboxStyle();
    infoStyle = defInfoStyle();
    dlogStyle = defDlogStyle();
  }
  else {
    QApplication::setStyle(QStyleFactory::create(name));
    setStyleSheet("");
    ui.glDisp->changeAllStyle("");
    clWidget->setStyleSheet("");
    clWidget->darkStyle(false);
    setStyle(QApplication::style());
  }

  QApplication::setPalette(QApplication::style()->standardPalette());
}

void TkDisplay::changeAnime(const QString &mode)
{
  if (mode == "Normal")       ui.glDisp->setAnimeMode(GLTDisp::AN_NORMAL);
  if (mode == "Light")        ui.glDisp->setAnimeMode(GLTDisp::AN_LIGHT);
  if (mode == "No animation") ui.glDisp->setAnimeMode(GLTDisp::AN_NONE);
}

void TkDisplay::gl_set_enabled_all(bool sw)
{
  ui.acOpen->setEnabled(sw);
//ui.acExit->setEnabled(sw);
  ui.acPrev->setEnabled(sw);
  ui.acNext->setEnabled(sw);
  ui.acEsel->setEnabled(sw);
//ui.acMore->setEnabled(sw);
  ui.acFrst->setEnabled(sw);
  ui.acLast->setEnabled(sw);
  ui.acRcam->setEnabled(sw);

  ui.tabWidget->setEnabled(sw);
}

QString &TkDisplay::defaultStyle()
{
  static QString str = 
    "QMainWindow, QWidget#teWidget"
    "{ background-color: rgb(0, 10, 5); }"

    "QPushButton, QTabBar::tab, QTextEdit, QWidget#QWCHist,"
    "QWidget#tab1, QWidget#tab2, QWidget#tab3, QWidget#tab4, QWidget#tab5"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 1 rgb( 0,  5,  0), "
    "                      stop: 0 rgb( 0, 30, 10)); }"

    "QPushButton:pressed, QTabBar::tab:selected"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 0.7 rgb(0,  20,  20), "
    "                      stop: 0.0 rgb(0, 140, 140)); }"

    "QPushButton:disabled"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 1 rgb(20, 40, 20), "
    "                      stop: 0 rgb(50, 70, 50)); }"

    "QLabel, QCheckBox"
    "{ background-color: rgba(0, 0, 0, 0); }"

    "QLabel, QCheckBox, QPushButton, QRadioButton, "
    "QTab, QTabBar, QTabBar::tab, QTabBar::tab:selected,"
    "QWidget#teWidget, QTextEdit,"
    "QWidget#tab1, QWidget#tab2, QWidget#tab3, QWidget#tab4, QWidget#tab5"
    "{ color: rgb(180, 240, 220); }"

    "QPushButton:disabled, QCheckBox:disabled, QRadioButton:disabled, "
    "QTabBar::tab:disabled"
    "{ color: rgb(20, 80, 60); }"

    "QComboBox"
    "{ color: rgb(0, 10, 5); }"

    "QFrame, QPushButton,"
    "QTab, QTabBar::tab, QTabBar::tab:selected,"
    "QWidget#teWidget, QWidget#glDisp, QTextEdit"
    "{ border: 1px outset rgb(100, 200, 150); }"

    "QFrame#vframe"
    "{ border: 0px solid rgb(0, 0, 0); }"

    "QLabel"
    "{ border: 0px solid rgba(0, 0, 0, 0); }"

    "QFrame, QWidget#teWidget, QTextEdit "
    "{ border-radius: 4px; }"

    "QPushButton"
    "{ border-radius:  10px;"
    "  padding-top:     2px;"
    "  padding-bottom:  1px;"
    "  padding-right:  12px;"
    "  padding-left:   12px;"
    "  margin-top:      5px;"
    "  margin-bottom:   5px;"
    "  margin-right:   10px;"
    "  margin-left:    10px; }"

    "QTabBar::tab, QTabBar::tab:selected"
    "{ border-radius:   2px;"
    "  padding-top:     1px;"
    "  padding-bottom:  1px;"
    "  padding-right:  10px;"
    "  padding-left:   10px; }";
  return str;
}

QString &TkDisplay::defMboxStyle()
{
  static QString str = 
    "QFrame, QWidget"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10));"
    "  border: 1px outset rgb(100, 200, 150); "
    "  border-radius: 4px; }"

    "QLabel"
    "{ background-color: rgba(0, 0, 0, 0); "
    "  border: 0px solid black;"
    "  border-radius: 0px; }"

    "QPushButton"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10)); }"

    "QPushButton:pressed"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 0.7 rgb(0,  20,  20), "
    "                      stop: 0.0 rgb(0, 140, 140)); }";
  return str;
}

QString &TkDisplay::defInfoStyle()
{
  static QString str = 
#ifdef Q_WS_MAC
    "QWidget"
    "{ background-color: rgbs(0, 0, 0, 0); }"
#else
    "QWidget"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10));"
    "  border: 1px outset rgb(100, 200, 150); "
    "  border-radius: 4px; }"
#endif
    "QLabel"
    "{ background-color: rgba(0, 0, 0, 0); "
    "  border: 0px solid black;"
    "  border-radius: 0px; }"

    "QPushButton"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10)); }"

    "QPushButton:pressed"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 0.7 rgb(0,  20,  20), "
    "                      stop: 0.0 rgb(0, 140, 140)); }";
  return str;
}

QString &TkDisplay::defDlogStyle()
{
  static QString str = 
    "QFrame"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10));"
    "  border: 1px outset rgb(100, 200, 150); "
    "  border-radius: 4px; }"

    "QLabel, QCheckBox"
    "{ background-color: rgba(0, 0, 0, 0); "
    "  border: 0px solid black;"
    "  border-radius: 0px; }"

    "QLabel:disabled, QCheckBox:disabled"
    "{ color: rgb(20, 80, 60); }"

#ifndef Q_WS_MAC
    "QCheckBox:selected"
    "{ border: 1px outset rgb(100, 200, 150); "
    "  border-radius: 4px; }"
    
#endif

    "QPushButton"
    "{ background-color: qlineargradient(spread:pad,"
    "  x1: 0, y1: 0, x2: 0, y2: 1,"
    "  stop: 1 rgb( 0,  5,  0), "
    "  stop: 0 rgb( 0, 30, 10)); }"

    "QPushButton:pressed"
    "{ background-color: qlineargradient(spread:pad,"
    "                      x1: 0, y1: 0, x2: 0, y2: 1,"
    "                      stop: 0.7 rgb(0,  20,  20), "
    "                      stop: 0.0 rgb(0, 140, 140)); }";
  return str;
}
