// $Id: main.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QApplication>
#include <QString>

#include "tkdisplay.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QString fname;
  int entry = 0;
  for (int i = 0; i < argc; i++) {
    QString str = argv[i];
    if (str.contains("entry:"))
      entry = str.mid(str.indexOf(":")+1).toInt();
    if (str.contains("dst:"))
      fname = str.mid(str.indexOf(":")+1);
  }

  TkDisplay *w = new TkDisplay;
  if (fname != "") w->openFile(fname.toAscii().data(), entry);
  w->show();

  return app.exec();
}
