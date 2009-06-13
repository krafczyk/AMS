// $Id: wdcuts.cpp,v 1.1 2009/06/13 21:40:48 shaino Exp $
#include <QtGui>

#include "wdcuts.h"

WdCuts::WdCuts(QWidget *parent, //QGridLayout *layout, 
	       const QString &pname,
	       const QString &title, const QString &label,
	       double vmin, double vmax, double vdef1, double vdef2)
  : vMin(vmin), vMax(vmax), vDef1(vdef1), vDef2(vdef2)
{
  pName = pname;
  cBox  = new QCheckBox(title);
  hSpac = new QSpacerItem(40, 20, QSizePolicy::Expanding,
			          QSizePolicy::Minimum);
  lbFrm = new QLabel(parent);

  if (label[0] == 'N' || label[0] == 'n' ||
      label[0] == 'I' || label[0] == 'i') {
    sBox1 = new QSpinBox(parent);
    sBox2 = new QSpinBox(parent);
    sBox3 = sBox4 = 0;
  }
  else {
    sBox3 = new QDoubleSpinBox(parent);
    sBox4 = new QDoubleSpinBox(parent);
    sBox1 = sBox2 = 0;
  }

  QGridLayout *layout = dynamic_cast<QGridLayout *>(parent->layout());
  int row = layout->rowCount();
  layout->addWidget(cBox,  row, 0, 1, 1);
  layout->addItem  (hSpac, row, 1, 1, 1);
  layout->addWidget(lbFrm, row, 3, 1, 1);

  if (sBox1 && sBox2) {
    layout->addWidget(sBox1, row, 2, 1, 1);
    layout->addWidget(sBox2, row, 4, 1, 1);
  }
  else {
    layout->addWidget(sBox3, row, 2, 1, 1);
    layout->addWidget(sBox4, row, 4, 1, 1);
  }

  cBox->setFocusPolicy(Qt::StrongFocus);
  parent->setTabOrder(cBox, sBox1);
  parent->setTabOrder(sBox1, sBox2);

  cBox ->setText(title);
  lbFrm->setText("<= "+label+" <=");

  resetAll();

  connect(cBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));

  if (sBox1 && sBox2) {
    connect(sBox1, SIGNAL(valueChanged(int)), this, SLOT(valueChanged1()));
    connect(sBox2, SIGNAL(valueChanged(int)), this, SLOT(valueChanged2()));
  }
  if (sBox3 && sBox4) {
    connect(sBox3, SIGNAL(valueChanged(double)), this, SLOT(valueChanged1()));
    connect(sBox4, SIGNAL(valueChanged(double)), this, SLOT(valueChanged2()));
  }

  cIndex = 0;
}

void WdCuts::resetAll()
{
  if (sBox1) { sBox1->setRange(vMin, vMax); sBox1->setValue(vDef1); }
  if (sBox2) { sBox2->setRange(vMin, vMax); sBox2->setValue(vDef2); }
  if (sBox3) { sBox3->setRange(vMin, vMax); sBox3->setValue(vDef1); }
  if (sBox4) { sBox4->setRange(vMin, vMax); sBox4->setValue(vDef2); }

  cBox->setChecked(false);
  stateChanged(false);
}

const QString& WdCuts::getString()
{
  static QString str;
  str = "";

  if (!cBox->isChecked() || objectName() != "") return str;

  int imin = -1;
  int imax = -1;
  if (cIndex) {
    imin = cIndex->getSB1();
    imax = cIndex->getSB2();
  }

  str += "(";

  for (int i = imin; i <= imax; i++) {
    QString ss;
    if (i > imin) str += " && ";

    if (i >= 0) {
      str += "(";
      str += "N"+pName.mid(1)+"()<="+QString::number(i)+" || ";
      ss = pName+"("+QString::number(i)+")->";
    }
    if (sBox1 && sBox2) 
      str += "("+sBox1->text()+"<="+ss+cBox->text()+" && "
	                           +ss+cBox->text()+"<="+sBox2->text()+")";
    if (sBox3 && sBox4) {
      QString str3 = sBox3->text();
      QString str4 = sBox4->text();
      str3.replace(",", ".");
      str4.replace(",", ".");
      str += "("+str3+"<="+ss+cBox->text()+" && "
	                  +ss+cBox->text()+"<="+str4+")";
    }

    if (i >= 0) str += ")";
  }

  str += ")";

  return str;
}

void WdCuts::stateChanged(int sw)
{
  if (sBox1) sBox1->setEnabled(sw);
  if (sBox2) sBox2->setEnabled(sw);
  if (sBox3) sBox3->setEnabled(sw);
  if (sBox4) sBox4->setEnabled(sw);

  emit enableCuts(sw);

  lbFrm->setEnabled(sw);
}

void WdCuts::setEnabled(bool sw)
{
  cBox->setEnabled(sw);
  if (!sw || cBox->isChecked()) stateChanged(sw);
}

void WdCuts::valueChanged1()
{
  if (sBox1 && sBox2 && 
      sBox1->value() > sBox2->value()) sBox2->setValue(sBox1->value());
  if (sBox3 && sBox4 && 
      sBox3->value() > sBox4->value()) sBox4->setValue(sBox3->value());
}

void WdCuts::valueChanged2()
{
  if (sBox1 && sBox2 && 
      sBox1->value() > sBox2->value()) sBox1->setValue(sBox2->value());
  if (sBox3 && sBox4 && 
      sBox3->value() > sBox4->value()) sBox3->setValue(sBox4->value());
}
