// $Id: tewidget.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// TEWidget : a class to manage text information of Tracker objects by SH
//
#ifndef TEWidget_H
#define TEWidget_H

#include "qwidget.h"
#include "qstring.h"
#include <QTextEdit>


QT_BEGIN_NAMESPACE
class QPushButton;
QT_END_NAMESPACE

class TEWidget : public QWidget {
  Q_OBJECT

public:
  TEWidget(QWidget *parent);

  void setText(QString &text) { tEdit->setPlainText(text); }
  void clearText() { tEdit->clear(); }

public Q_SLOTS:
  void changeStyle(const QString &style) { setStyleSheet(style); }

protected:
  QTextEdit   *tEdit;
  QPushButton *pbClose;
};

#endif
