// $Id: main.cpp,v 1.2 2009/12/06 12:30:20 shaino Exp $
#include <QApplication>
#include <QString>

#include "tkdisplay.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QString fname;
  int entry = 0;
  int anime = 1;
  for (int i = 0; i < argc; i++) {
    QString str = argv[i];
    if (str.contains("entry:"))  entry = str.mid(str.indexOf(":")+1).toInt();
    if (str.contains("dst:"))    fname = str.mid(str.indexOf(":")+1);
    if (str.contains("noanime")) anime = 0;
  }

  TkDisplay *w = new TkDisplay;
  if (fname != "") w->openFile(fname.toAscii().data(), entry);
  w->show();

  if (anime == 0) w->setAnime(2);

  return app.exec();
}
