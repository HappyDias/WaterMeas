/*This file is a part of the watermeas_files library*/

#include "watermeas_files.h"

//Start a new measurement file
uint32_t initRun(char* thefilename, SdFile* theFileVar){
  uint32_t timeLog=millis();
  
  theFileVar->open(thefilename, O_CREAT | O_WRITE | O_EXCL);

  return timeLog;
}

//End measurement file
void endRun(SdFile *fileStream){
  fileStream->close();
}

//Write data to measurement file
bool logData(SdFile* fileStream, uint32_t startTime) {
  char measurement[14];
  
  // Write data to file.  Start with log time in micros.
  makeMeasurement(measurement,startTime);
  
  return (fileStream->write(measurement)==sizeof(measurement));
}
