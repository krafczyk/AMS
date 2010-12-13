// $Id: main.cpp,v 1.7 2010/12/13 21:43:08 shaino Exp $
#include <QApplication>
#include <QString>

#include "tkdisplay.h"
#include "qnetio.h"
#include "gvdraw.h"
#include "TrRecon.h"
#include "MagField.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  QString fname;
  QString nfurl = "root://ams-farm.pg.infn.it";
  QString nfdir = "/amssw";
  QString fnmag = "";

  int   entry = 0;
  int   anime = 1;
  int   setup = GVDraw::tkSetup;
  float bfscl = 1;

  int posx = -1, posy = -1;

  for (int i = 0; i < argc; i++) {
    QString str = argv[i];
    if (str.contains("entry:"))  entry = str.mid(str.indexOf(":")+1).toInt();
    if (str.contains("setup:"))  setup = str.mid(str.indexOf(":")+1).toInt();
    if (str.contains("dst:"))    fname = str.mid(str.indexOf(":")+1);
    if (str.contains("url:"))    nfurl = str.mid(str.indexOf(":")+1);
    if (str.contains("dir:"))    nfdir = str.mid(str.indexOf(":")+1);
    if (str.contains("magfn:"))  fnmag = str.mid(str.indexOf(":")+1);
    if (str.contains("magsc:"))  bfscl = str.mid(str.indexOf(":")+1).toFloat();
    if (str.contains("noanime")) anime = 0;
    if (str.contains("pos:")) {  posx  = str.mid(str.indexOf(":")+1,
						 str.indexOf(",")-
						 str.indexOf(":")-1).toInt();
                                 posy  = str.mid(str.indexOf(",")+1).toInt(); }
  }

  GVDraw::tkSetup = setup;

  if (fnmag != "") {
    TrRecon::ReadMagField(fnmag.toAscii().data(), bfscl, 1);
    MagField::GetPtr()->SetScale(bfscl);
  }

  NetFileEngineHandler netf;

  TkDisplay *w = new TkDisplay;
  if (posx >= 0 && posy >= 0) w->move(posx ,posy);

  if (fname != "") w->openFile(fname.toAscii().data(), entry);
  if (nfurl != "" && 
      nfdir != "") NetFileEngine::setUrl(nfurl, nfdir, false);
  w->show();

  if (anime == 0) w->setAnime(2);

  return app.exec();
}
