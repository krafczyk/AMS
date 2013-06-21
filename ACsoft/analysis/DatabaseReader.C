#include "DatabaseReader.hh"

#include "FileManager.hh"
#include "Event.h"
#include "RunHeader.h"

#include <TFile.h>
#include <TTree.h>
#include <math.h>
#include <stdlib.h>

#include <assert.h>
#include <stdexcept>

#define DEBUG 0
#define INFO_OUT_TAG "DatabaseReader> "
#include <debugging.hh>

/** Initialize DatabaseReader
  *
  */
ACsoft::Analysis::DatabaseReader::DatabaseReader( ACsoft::IO::FileManager* fmPointer ){

  fFileManagerPointer = fmPointer;
  assert(fmPointer);

  char *dirPath;
  dirPath = getenv( "ACROOTSOFTWARE" );
  std::string dirPathString = std::string(dirPath);
  dirPathString = dirPathString + "/acroot/data/TrdQt_tempSensors_v01.root";
  TFile* file = TFile::Open( dirPathString.c_str() );
  if( !(file->IsOpen() ) ) {
    WARN_OUT << "Opening temperature sensors file " << dirPathString << " failed!" << std::endl;
    throw std::runtime_error("Opening temperature sensors file failed");
  }

  TTree *tree = (TTree*)file->Get( "tree;1" );
  int xCoordinates;
  int yCoordinates;
  int zCoordinates;

  tree->SetBranchAddress("xCoordinates", &xCoordinates);
  tree->SetBranchAddress("yCoordinates", &yCoordinates);
  tree->SetBranchAddress("zCoordinates", &zCoordinates);

  for ( int i=0; i < nTemperatureSensors; i++ )
  {
    tree->GetEntry(i);
    xPositionOfSensor[i] = xCoordinates;
    yPositionOfSensor[i] = yCoordinates;
    zPositionOfSensor[i] = zCoordinates;
  }

  file->Close();

}

/** Default Destructor
  *
  */
ACsoft::Analysis::DatabaseReader::~DatabaseReader(){

}

/** Get temperature for a sensor identified by its ID-number (ID: 0..201). The temperature is given in °C
  *
  */
double ACsoft::Analysis::DatabaseReader::GetTemperatureForSensor( int sensorID ){

  if ( sensorID < 0 || sensorID >= nTemperatureSensors ) {
    WARN_OUT << "Invalid sensor ID. Temperatursensor #" << sensorID << " does not exist." << std::endl;
    return -273.15;
  }

  // calculate the time distance from run-begin to event-time to find the right database.
  // The data is refreshed every 5min or 300s inside the database,
  // by dividing the time by 300s, you get the corresponding index of the database-vector.
  int runStartTime = fFileManagerPointer->RunHeader()->FirstEventTimeStamp().GetSec();
  int eventTime = fFileManagerPointer->Event()->EventHeader().TimeStamp().GetSec();
  int timeFromRunStart = eventTime - runStartTime;
  int sensorDataIndex = timeFromRunStart / 300;

  // get current database pointer from the Filemanager
  ::AC::Database* DatabasePointer = fFileManagerPointer->Database();
  assert(DatabasePointer);
  char tmpData = DatabasePointer->TRDSensorData()[sensorDataIndex].TemperatureForSensor( sensorID );
  double temperature = ( tmpData/4.0 ) + 15;

  DEBUG_OUT << "T(" << sensorID << ") = " << temperature << std::endl;

  return temperature;

}

/** Get temperature for any spatial position (x, y, z) interpolated from sensors.
  * This function uses "inverse distance weighting" of the temperature data.
  * You need the "TrdQt_tempSensors_v01.root" lookup.
  *
  */
double ACsoft::Analysis::DatabaseReader::GetTemperatureForPosition( double x, double y, double z ){

  //calculate the time distance from run-begin to event-time to find the right database
  int runStartTime = fFileManagerPointer->RunHeader()->FirstEventTimeStamp().GetSec();
  int eventTime = fFileManagerPointer->Event()->EventHeader().TimeStamp().GetSec();
  int timeFromRunStart = eventTime - runStartTime;
  int sensorDataIndex = timeFromRunStart / 300;

  // get current database pointer from the Filemanager
  ::AC::Database* DatabasePointer = fFileManagerPointer->Database();
  assert(DatabasePointer);

  double distance;
  double distanceNorm = 0.;
  char tmpData;
  double measuredTemperature;
  double temperature = 0.;
  double pExp = 4.;     // exponent of inverse distant weighting

  // looping over all 202 temperature sensors.
  for( int sensorNr=0; sensorNr < nTemperatureSensors; sensorNr++ )
  {
    distance = sqrt( pow( (x - xPositionOfSensor[sensorNr]) ,2) + pow( (y - yPositionOfSensor[sensorNr]) ,2) + pow( (z - zPositionOfSensor[sensorNr]) ,2) );
    tmpData = DatabasePointer->TRDSensorData()[sensorDataIndex].TemperatureForSensor( sensorNr );
    measuredTemperature =  ( tmpData/4.0 ) + 15;
    if( distance <= 0.1 ) return measuredTemperature;

    temperature = temperature + ( measuredTemperature / pow( distance, pExp ) );
    distanceNorm = distanceNorm + ( 1.0 / pow( distance, pExp ) );
  }

  temperature = temperature /distanceNorm;

  return temperature;

}

