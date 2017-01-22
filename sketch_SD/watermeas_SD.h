#ifndef watermeas_SD
#define watermeas_SD

#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SystemInclude.h>
#include <SdFatConfig.h>
#include <SdFat.h>
#include <SdFatUtil.h>

//SD file structure information.
typedef struct t_SD_info {
    uint16_t initialized; //Tells if the database was already configured.
    uint32_t nfiles; //Number of files in the database.
    uint32_t nfiles_max; //Maximum possible number of files in the database.
    char base_name[100]; //Base name for any file belonging to the database.
    char next_file_name[100]; //Next file to be created in the database.
} SD_dbconf;

//Configure database.
void SD_configdb(SdFat* sd, SD_dbconf* dbconfig, char* base_name, uint32_t nfiles_max);

//Get next unused file name.
void SD_next_unused_file_name(SdFat* sd, SD_dbconf* dbconfig, char* file_name);

//Get number of files stored in SD with the same base_name.
int SD_number_of_existing_files(SdFat* sd, SD_dbconf* dbconf);

//Get file name.
int SD_get_file_name(SdFat* sd, SD_dbconf* dbconfig, int file_number, char* file_name);

//List files.
void SD_list_files(SdFat* sd, SD_dbconf* dbconfig, char* write_buffer);

#endif //watermeas_SD
