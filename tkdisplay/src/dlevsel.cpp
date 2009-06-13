// $Id: dlevsel.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>

#include "dlevsel.h"
#include "tkdisplay.h"
#include "wdcuts.h"

DlEvSel::DlEvSel(TkDisplay *parent) : QDialog(parent)
{
  ui.setupUi(this);

  // Tab: AMSEventR
  QGroupBox *gb = addBox(addTab("AMSEventR"));
  addCut(gb, "NTrTrack()",      "N",  0,  99,  0,   1);
  addCut(gb, "NTrTrack()",      "N",  0,  99,  0,   1);
  addCut(gb, "NTrRecHit()",     "N",  0, 999,  0, 999);
  addCut(gb, "NTrCluster()",    "N",  0, 999,  0, 999);
  addCut(gb, "NTrRawCluster()", "N",  0, 999,  0, 999);

  // Tab: pLevel1
  gb = addBox(addTab("pLevel1", 1));
  addCut(gb, "TrigTime[4]", "T",  0, 1e8,  0, 1e8);

  // Tab: pTrTrack
  gb = addBox(addTab("pTrTrack", 10));
  addCut(gb, "GetNhits()",    "N",    0,   8,    0,   8);
  addCut(gb, "GetNhitsXY()",  "N",    0,   8,    0,   8);
  addCut(gb, "GetChisqX()",  "x2",    0, 1e6,    0, 1e6);
  addCut(gb, "GetChisqY()",  "x2",    0, 1e6,    0, 1e6);
  addCut(gb, "GetRigidity()", "R", -1e6, 1e6, -1e6, 1e6);

  ui.cbEnable->setChecked(true);

  ui.wTab->setCurrentIndex(0);
  cwVec.at(0)->setFocus();
}

void DlEvSel::on_dbBox_clicked(QAbstractButton *button)
{
  if (button->text() == "OK") {
    QString str;

    if (ui.cbEnable->isChecked()) {
      for (int i = 0; i < cwVec.size(); i++) {
	QString scut = cwVec.at(i)->getString();
	if (scut != "" && str != "") str += " && ";
	str += scut;
      }

      QString scut = ui.lineEdit->text();
      if (scut != "" && str != "") str += " && ";
      str += scut;
    }

    emit updateFormula(str);

    TkDisplay *tkd = dynamic_cast<TkDisplay*>(parentWidget());
    if (tkd && !tkd->getTFError()) accept();
  }
  else if (button->text() == "Reset") {
    emit resetAll();
    ui.lineEdit->clear();
    ui.cbEnable->setChecked(true);
  }
}

QWidget *DlEvSel::addTab(const char *name, int imax)
{
  QWidget *tab = new QWidget;
  tab->setObjectName(name);
  ui.wTab->addTab(tab, name);

  QVBoxLayout *vl = new QVBoxLayout(tab);
  vl->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, 
			              QSizePolicy::Expanding));

  if (imax >= 0) {
    int index = cwVec.size();
    QGroupBox *gb = addBox(tab);

    QString gbn = "index"+QString::number(index);
    gb->setObjectName(gbn);
    addCut(gb, "index", "i", 0, imax-1, 0, imax-1);
    cwVec.at(index)->setObjectName(gbn);
  }

  return tab;
}

QGroupBox *DlEvSel::addBox(QWidget *qw)
{
  QGroupBox   *gb = new QGroupBox(qw);
  QVBoxLayout *vl = dynamic_cast<QVBoxLayout *>(qw->layout());

  int nw = qMax(vl->count(), 1);
  vl->insertWidget(nw-1, gb);

  new QGridLayout(gb);

  return gb;
}

void DlEvSel::addCut(QGroupBox *gb, const char *title, const char *label,
		     double vmin, double vmax, double vdef1, double vdef2)
{
  QWidget *qw = dynamic_cast<QWidget *>(gb->parent());
  QString pname = qw->objectName();

  if (pname[0] != 'p') pname = "";
  WdCuts *wc = new WdCuts(gb, pname, title, label, vmin, vmax, vdef1, vdef2);
  connect(this, SIGNAL(resetAll()), wc, SLOT(resetAll()));

  QWidget *qg = qw->layout()->itemAt(0)->widget();
  QString gname = qg->objectName();
  if (qg != gb && gname != "") {
    for (int i = cwVec.size()-1; i >= 0; i--)
      if (cwVec.at(i)->objectName() == gname) {
	connect(cwVec.at(i), SIGNAL(enableCuts(bool)), 
		wc, SLOT(setEnabled(bool)));
	wc->setEnabled(false);
	wc->setCIndex(cwVec.at(i));
	break;
      }
  }

  cwVec.push_back(wc);
}
