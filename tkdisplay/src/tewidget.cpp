// $Id: tewidget.cpp,v 1.1 2009/06/13 21:40:48 shaino Exp $
#include <QtGui>

#include "tewidget.h"

TEWidget::TEWidget(QWidget *parent) : QWidget(parent)
{
  setObjectName(QString::fromUtf8("teWidget"));

  tEdit = new QTextEdit;
  tEdit->setReadOnly(true);
  tEdit->setLineWrapMode(QTextEdit::NoWrap);

  pbClose = new QPushButton(tr("&Close"));
  connect(pbClose, SIGNAL(clicked()), this, SLOT(close()));

  QVBoxLayout *vlay = new QVBoxLayout;
  QHBoxLayout *hlay = new QHBoxLayout;
  QSpacerItem *hspc = new QSpacerItem(0, 0, QSizePolicy::Expanding);
  vlay->addWidget(tEdit);
  vlay->addLayout(hlay);

  hlay->addSpacerItem(hspc);
  hlay->addWidget(pbClose);

  setLayout(vlay);

  setWindowTitle(tr("More"));
}
