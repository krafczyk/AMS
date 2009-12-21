// $Id: main.cpp,v 1.3 2009/12/21 17:41:49 shaino Exp $
#include <QApplication>
#include <QString>

#include "tkdisplay.h"
#include "qnetio.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QString fname;
  QString nfurl = "root://ams-farm.pg.infn.it";
  QString nfdir = "/amssw";

  int entry = 0;
  int anime = 1;
  for (int i = 0; i < argc; i++) {
    QString str = argv[i];
    if (str.contains("entry:"))  entry = str.mid(str.indexOf(":")+1).toInt();
    if (str.contains("dst:"))    fname = str.mid(str.indexOf(":")+1);
    if (str.contains("url:"))    nfurl = str.mid(str.indexOf(":")+1);
    if (str.contains("dir:"))    nfdir = str.mid(str.indexOf(":")+1);
    if (str.contains("noanime")) anime = 0;
  }

  NetFileEngineHandler netf;

  TkDisplay *w = new TkDisplay;
  if (fname != "") w->openFile(fname.toAscii().data(), entry);
  if (nfurl != "") NetFileEngine::setUrl(nfurl);
  if (nfdir != "") NetFileEngine::setDir(nfdir);
  w->show();

  if (anime == 0) w->setAnime(2);

  return app.exec();
}
