#include "watermeas_defines.h"
#include "watermeas_SD.h"

#define SAMPLING_TIME 10

//configure measure controls.
void measure_config(int delay_time_local);

//Start measurement run
int measure_start_run(SdClassWrap* sd, SD_dbconf* dbconf);

//Stop measurement run
int measure_stop_run(SdClassWrap* sd, SD_dbconf* dbconf);

//manage measure run start and end.
int measure_control(SdClassWrap* sd, SD_dbconf* dbconf);

//Make measurement, write time and value to buffer
void measure_makeMeasurement(char* bufferStr,uint32_t startTime);

//Write data to measurement file
uint32_t measure_logData(SdFileWrap* fileStream, uint32_t startTime);
