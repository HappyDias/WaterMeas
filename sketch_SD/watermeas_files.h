/*This file is a part of the watermeas_files library*/

#include "watermeas_measure.h"
#include "watermeas_SD.h"

//Start a new measurement file
uint32_t initRun(char* thefilename, SdFile* theFileVar);

//End current measurement file
void endRun(SdFile *fileStream);

//Log data onto file
bool logData(SdFile* fileStream,uint32_t startTime);
