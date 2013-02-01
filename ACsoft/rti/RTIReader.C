#include <assert.h>

#include <QDateTime>
#include <QFile>
#include <QProcessEnvironment>
#include <QString>
#include <QTextStream>

#include "AnalysisParticle.hh"
#include "RTIData.hh"
#include "RTIReader.hh"

#define DEBUG 0
#define INFO_OUT_TAG "RTIReader> "
#include <debugging.hh>

namespace ACsoft {

namespace RTI {

static const unsigned int gEntriesPerFile = 86400;

RTIReader::RTIReader() 
  : fStartTime(-1)
  , fEndTime(-1) {

  fData.reserve(gEntriesPerFile);
  for (unsigned int i = 0; i < gEntriesPerFile; ++i)
    fData.push_back(new RTIData);
}

RTIReader::~RTIReader() {

  for (unsigned int i = 0; i < gEntriesPerFile; ++i)
    delete fData.at(i);

  fData.clear();
}

RTIReader* RTIReader::Instance() {

  static RTIReader* gInstance = 0;
  if( !gInstance )
    gInstance = new RTIReader;

  return gInstance;
}

static std::string ComputeCurrentRTIFilename(int time) {

  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();

  QString path;
  if (environment.value("AMSDataDir").isEmpty()) {
    path = environment.value("ACROOTLOOKUPS");
    path.append("/RTI/");
  } else {
    path = environment.value("AMSDataDir");
    path.append("/altec/RTI/");
  }

  QDateTime qDate;
  qDate.setTime_t(time);

  int year = qDate.date().year();
  int day = qDate.date().dayOfYear();
  QString rtiName = QString("RTI_%1_%2-24H.csv").arg(year).arg(day);

  path.append(rtiName);
  return path.toStdString();
}

/** Reads the RTI file and fills the RTIData object.
  * if the full RTI inforamtion is not available, the time is filled, the rest of the variables are set to their default (-1)
  * called by the GetRTIData() functions.
  */
bool RTIReader::ReadRTIFile(int time) {

  std::string fileName = ComputeCurrentRTIFilename(time);
  INFO_OUT << " Reading new RTI File " << fileName << std::endl;

  for (unsigned int i = 0; i < gEntriesPerFile; ++i)
    fData.at(i)->Clear();

  QFile file(QString::fromStdString(fileName));
  if (!file.exists()) {
    INFO_OUT << " ERROR: need RTI-file: " << fileName << std::endl;
    INFO_OUT << "        can be found on HPC-cluster in $ACROOTLOOKUPS/RTI" << std::endl;
    INFO_OUT << "                     or lxplus, $AMSDataDir/altec/RTI/"    << std::endl;
    return false;
  }

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  QTextStream in(&file);
  QString header = in.readLine();
  QStringList array = header.split(",");
  unsigned int lineCount = 0;
  int lastTime = 0;
  while (!in.atEnd()) {
    ++lineCount;

    QString line = in.readLine();
    QStringList data = line.split(" ");
    int dataCount = data.size();
    if (dataCount != 24) {
      if (dataCount == 2) // Common problem: no data available from RTI files indicated by only a run number followed by '0'.
        fData.at(time - fStartTime)->SetData(time, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);
      else
        WARN_OUT << "Incomplete RTI data record detected in line: " << lineCount << " (data count: " << dataCount << " expecting: 24)" << std::endl;
      continue;
    }

    int time = data[0].toInt();
    int run = data[1].toInt();
    int evbegin = data[2].toInt();
    double livetime = data[3].toDouble();
    double cutoff25N = data[4].toDouble();
    double cutoff25P = data[5].toDouble();
    double cutoff30N = data[6].toDouble();
    double cutoff30P = data[7].toDouble();
    double cutoff35N = data[8].toDouble();
    double cutoff35P = data[9].toDouble();
    double cutoff40N = data[10].toDouble();
    double cutoff40P = data[11].toDouble();
    double MPVHERIG = data[12].toDouble();
    double thetaS = data[13].toDouble();
    double phiS = data[14].toDouble();
    double radS = data[15].toDouble();
    double zenith = data[16].toDouble();
    double gLat = data[17].toDouble();
    double gLong = data[18].toDouble();
    int events = data[19].toInt();
    int missedEvents = data[20].toInt();
    int level1Trigger = data[21].toInt();
    int parts = data[22].toInt();
    short good = data[23].toShort();

    lastTime = time;
    if (lineCount == 1)
      fStartTime = time;

    assert(fStartTime > 0);
    fData.at(time - fStartTime)->SetData(time, run, evbegin, livetime, cutoff25N, cutoff25P, cutoff30N, cutoff30P,
                                         cutoff35N, cutoff35P, cutoff40N, cutoff40P, MPVHERIG, thetaS, phiS, radS,
                                         zenith, gLat, gLong, events, missedEvents, level1Trigger, parts, good);
  }

  fEndTime = lastTime;
  assert (fEndTime >= fStartTime);
  return true;
}

/** gets the unix time from the particle timestamp and calls GetRTIData(time) */
const RTIData* RTIReader::GetRTIData(const Analysis::Particle& particle) {

  return GetRTIData(particle.TimeStamp().GetSec());
}

/** Tests if the RTIData object is empty. If that is the case it looks up the needed file name and reads the file.
  * the whole file (24h) is stored in the RTIData object. When a new file is nessesary, the RTIData object is cleared,
  * and filled with the RTI information from the new file.
  *
  * use this if you dont want to run with acqt files.
  *
  */
const RTIData* RTIReader::GetRTIData(int time) {

  if (time < fStartTime || time > fEndTime) {
    if (!ReadRTIFile(time))
      return 0;
  }

  assert(fStartTime > 0);
  assert(fEndTime >= fStartTime);
  return fData.at(time - fStartTime);
}

}

}
