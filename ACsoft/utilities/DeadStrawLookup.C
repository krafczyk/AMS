#include "DeadStrawLookup.hh"

#include "AMSGeometry.h"
#include "Settings.h"

#include <assert.h>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>

#include "LookupFileValidation.h"

#define DEBUG 0
#define INFO_OUT_TAG "DeadStrawLookup> "
#include <debugging.hh>

ACsoft::Utilities::DeadStrawLookup::DeadStrawLookup() {

  if (!ReadDeadStrawLookupFile()) {
    WARN_OUT << "Unable to read data for DeadStrawLookup from file." << std::endl;
    throw std::runtime_error("Unable to read data for DeadStrawLookup from file.");
  }
}

static inline bool ParseDate(const QStringList& dateList, unsigned int& year, unsigned int& month, unsigned int& day) {

  if (dateList.size() != 3)
    return false;

  bool ok = true;
  year = dateList.at(0).toInt(&ok);
  if (!ok)
    return false;

  month = dateList.at(1).toInt(&ok);
  if (!ok)
    return false;

  day = dateList.at(2).toInt(&ok);
  if (!ok)
    return false;

  return true;
}

bool ACsoft::Utilities::DeadStrawLookup::ReadDeadStrawLookupFile() {

  char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    WARN_OUT << "ACROOTSOFTWARE variable not set." << std::endl;
    return false;
  }

  std::stringstream filenameStream;
  filenameStream << acrootsoftware << "/acroot/data/" << ::AC::Settings::gTrdQtDeadStrawsFileName;
  std::string filename = filenameStream.str();

  INFO_OUT << "Read dead straw information from file " << filename << std::endl;

  std::ifstream file(filename.c_str());
  if (!file.good()) {
    WARN_OUT << "ERROR opening file " << filename << std::endl;
    return false;
  }

  VALIDATE_LOOKUP_FILE("TRDDeadStraws", gTrdQtDeadStrawsFileName);
  file.close();

  // TODO: It's inefficienct to open thw file twice, once via std::ifstream and once via QFile.
  QFile qfile(filename.c_str());
  if (!qfile.open(QIODevice::ReadOnly)) {
    WARN_OUT << "ERROR opening file " << filename << std::endl;
    return false;
  }

  bool ok = true;

  QTextStream in(&qfile);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList list = line.split("\t", QString::SkipEmptyParts);
    if (list.size() != 5) {
      ok = false;
      continue;
    }

    ACsoft::Utilities::DeadStraw deadStraw;
    deadStraw.straw = list.at(0).toUShort(&ok);
    if (!ok)
      break;

    deadStraw.deadBegin = list.at(1).toDouble(&ok);
    if (!ok)
      break;

    deadStraw.deadEnd = list.at(2).toDouble(&ok);
    if (!ok)
      break;

    QStringList dateList = list.at(3).split("-");
    unsigned int year = 0, month = 0, day = 0;
    if (!ParseDate(dateList, year, month, day)) {
      ok = false;
      break;
    }
    deadStraw.timeBegin = TTimeStamp(year, month, day, 0, 0, 0);

    dateList = list.at(4).split("-");
    if (!ParseDate(dateList, year, month, day)) {
      ok = false;
      break;
    }
    deadStraw.timeEnd = TTimeStamp(year, month, day, 0, 0, 0);

    fDeadStraws.push_back(deadStraw);
  }

  if (!ok) {
    WARN_OUT << "Dead straw lookup file is invalid." << std::endl;
    return false;
  }

  file.close();
  return true;
}

void ACsoft::Utilities::DeadStrawLookup::DumpDeadStraws() {

  INFO_OUT << "---------------------------------------------------------------------------------" << std::endl;
  INFO_OUT << "Use the following dead straws: " << std::endl;
  for (unsigned int i = 0; i<fDeadStraws.size(); i++) {
    INFO_OUT << "Straw number: " << QString("%1").arg(fDeadStraws.at(i).straw, 4).toStdString()
             << "\t deadBegin: " << QString("%1").arg(fDeadStraws.at(i).deadBegin, 5).toStdString()
             << "\t deadEnd: " << QString("%1").arg(fDeadStraws.at(i).deadEnd, 5).toStdString()
             << "\t timeBegin: " << fDeadStraws.at(i).timeBegin << "\t timeEnd: "
             << fDeadStraws.at(i).timeEnd << std::endl;
  }

  INFO_OUT << "---------------------------------------------------------------------------------" << std::endl;
}

bool ACsoft::Utilities::DeadStrawLookup::IsStrawDead(unsigned short straw, double secondCoordinate, const TTimeStamp& timeStamp) {

  assert(straw < AC::AMSGeometry::TRDStraws);

  // FIXME: If this turns out to be performance relevant, a hash table with O(1) lookup time shall be used.
  // For now this is O(N) where N is the number of dead straws.
  for (unsigned int i = 0; i < fDeadStraws.size(); ++i) {
    if (straw != fDeadStraws.at(i).straw)
      continue;
    if (secondCoordinate < fDeadStraws.at(i).deadBegin || secondCoordinate > fDeadStraws.at(i).deadEnd)
      continue;
    if (timeStamp < fDeadStraws.at(i).timeBegin || timeStamp > fDeadStraws.at(i).timeEnd)
      continue;
    return true;
  }

  return false;
}

