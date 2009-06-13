// $Id: dlevsel.h,v 1.1 2009/06/13 21:40:46 shaino Exp $
//
// DlEvSel : a class to manage event selection dialog by SH
//
#ifndef DlEvSel_h
#define DlEvSel_h

#include "ui_dlevsel.h"
#include <vector>

class TkDisplay;
class WdCuts;
class QGridBox;
class QGroupBox;

class DlEvSel : public QDialog {
  Q_OBJECT

public:
  DlEvSel(TkDisplay *parent = 0);

signals:
  void updateFormula(QString);
  void resetAll();

private slots:
  void on_dbBox_clicked(QAbstractButton *button);
  void on_cbEnable_stateChanged(int sw) { 
    ui.wTab    ->setEnabled(sw); 
    ui.lineEdit->setEnabled(sw); 
  }

private:
  Ui::DlEvSel ui;

  QWidget   *addTab(const char *name, int imax = -1);
  QGroupBox *addBox(QWidget *qw);
  void       addCut(QGroupBox *gb, const char *title, const char *label,
		    double vmin, double vmax, double vdef1, double vdef2);
  std::vector<WdCuts *> cwVec;
};

#endif
