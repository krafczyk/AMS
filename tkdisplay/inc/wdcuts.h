// $Id: wdcuts.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// WdCuts : a class to manage a set of widgets for an event cut by SH
//
#ifndef WdCUTS_H
#define WdCUTS_H

#include <QtGui>

class WdCuts : public QObject {
  Q_OBJECT

public:
  WdCuts(QWidget *parent, const QString &pname, 
	 const QString &title, const QString &label,
	 double vmin, double vmax, double vdef1, double vdef2);

  const QString &getString();

  void setCIndex(WdCuts *ci) { cIndex = ci; }
  void setFocus() { cBox->setFocus(); }

  double getSB1() const { return (sBox1) ? sBox1->value() : sBox3->value(); }
  double getSB2() const { return (sBox2) ? sBox2->value() : sBox4->value(); }

public Q_SLOTS:
  void resetAll();
  void stateChanged(int sw);
  void setEnabled(bool sw);
  void valueChanged1();
  void valueChanged2();

signals:
  void enableCuts(bool);

private:
  WdCuts          *cIndex;
  QString          pName;
  QCheckBox       *cBox;
  QSpinBox        *sBox1;
  QSpinBox        *sBox2;
  QDoubleSpinBox  *sBox3;
  QDoubleSpinBox  *sBox4;
  QLabel          *lbFrm;
  QSpacerItem     *hSpac;

  double vMin;
  double vMax;
  double vDef1;
  double vDef2;
};

#endif
