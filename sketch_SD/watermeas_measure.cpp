#include "watermeas_measure.h"

//Make measurement, write time and value to buffer
void makeMeasurement(char* bufferStr, uint32_t startTime){
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
