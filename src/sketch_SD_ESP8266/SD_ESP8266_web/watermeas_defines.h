#ifndef __WATERMEAS_DEFINES__
#define __WATERMEAS_DEFINES__

//SD card library includes.
#include <SPI.h>
#include <SD.h>

//SD card library defines
typedef SDClass SdClassWrap;
typedef File SdFileWrap;

//Serial defines.
#define SERIAL_COMMAND_LS 48
#define SERIAL_COMMAND_FILE_READ 49
#define SERIAL_COMMAND_INIT_RUN 50
#define SERIAL_COMMAND_END_RUN 51
#define SERIAL_COMMAND_SYNC_TIME 52

#endif //__WATERMEAS_DEFINES__
