#include "watermeas_SD.h"

#define SAMPLING_TIME 10

//Make measurement, write time and value to buffer
void measure_makeMeasurement(char* bufferStr,uint32_t startTime);

//Write data to measurement file
uint32_t measure_logData(SdFile* fileStream, uint32_t startTime);
