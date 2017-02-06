#include "watermeas_measure.h"

//Make measurement, write time and value to buffer
void measure_makeMeasurement(char* bufferStr, uint32_t startTime){
  String dataStr;
  uint32_t currentTime;
  int sensorValue;
  int analogInPin=A0;

  //Get measurement from sensor
  sensorValue = analogRead(analogInPin);

  //Eventually convert the data here into Newtons
  
  //Update time
  currentTime=millis()-startTime;

  //Concatenate data string
  dataStr=currentTime;
  dataStr=dataStr+",";
  dataStr=dataStr+sensorValue;
  dataStr=dataStr+"\n";

  sprintf(bufferStr,"%s",dataStr.c_str());
}

//Write data to measurement file
uint32_t measure_logData(SdFile* fileStream, uint32_t startTime) {
  char measurement[14];
  
  // Write data to file.  Start with log time in micros.
  measure_makeMeasurement(measurement,startTime);
  
  return (fileStream->write(measurement)==sizeof(measurement));
}
