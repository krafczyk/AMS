#ifndef DATABASEREADER_HH
#define DATABASEREADER_HH

#include <FileManager.hh>

namespace ACsoft {

namespace Analysis {

/** A class to get information from the database loaded by the FileManager.
  *
  * For every AMS run there is a corresponding database providing information about the time dependent TRD-calibration, external tracker alignments and sensor data
  * from pressure and temperature sensors inside the TRD. It's saved as an .root file.
  *
  * The database associated with the current run is loaded and refreshed automatically by the Filemanager. Just create a DatabaseReader-object
  * once before your event-loop and use it inside.
  * If you use the DatabaseReader outside of an event-loop, be aware that any given sensor information from the DatabaseReader refers to the last used event.
  *
  * The sensor data is refreshed every 5min. Therefore for every run the sensor data is saved inside a vector with size 5 (~24min per run overall).
  * The number/index for the sensors ranges from 0..227. Sensors 0..201 are temperature sensors attached to the octagon structure frame of the TRD.
  * The other sensors are pressure sensors and not used yet by the DatabaseReader.
  *
  *
  *  Example usage (on ACQt input files):
  *
  *  \code
  *  #include "DatabaseReader.hh"
  *  #include "FileManager.hh"
  *
  *  int main() {
  *
  *    ...
  *    IO::FileManager fm;
  *    if (!fm.ReadFileList("mylist.list"))
  *      return 1;
  *
  *    // create a DatabaseReader object with a pointer to your FileManager
  *    Analysis::DatabaseReader dbReader(&fm);
  *
  *    while (fm.GetNextEvent()) {
  *      ...
  *      // use the DatabaseReader inside the event loop to get event-specific informations, e.g. temperature information
  *      // for the location of a particular TRD-hit
  *      TVector3 hitCoordinates = particle.TrdHits()[trdHitVectorIndex].Position3D();   // vector of the hit-coordinates
  *      double hitTemperature = dbReader.GetTemperatureForPosition( hitCoordinates.x(), hitCoordinates.y(), hitCoordinates.z() );
  *      // now you can use the temperature for further purposes.
  *      ...
  *    }
  *    ...
  *  }
  *  \endcode
  *
  */

class DatabaseReader{

public:
  DatabaseReader(IO::FileManager* fmPointer);
  virtual ~DatabaseReader();

  double GetTemperatureForSensor(int sensorID);
  double GetTemperatureForPosition( double x, double y, double z );

  static const int nTemperatureSensors = 202;

private:
  IO::FileManager* fFileManagerPointer;
  int xPositionOfSensor[nTemperatureSensors];
  int yPositionOfSensor[nTemperatureSensors];
  int zPositionOfSensor[nTemperatureSensors];

};

}

}

#endif // DATABASEREADER_HH
