//Libraries:

//Standard libraries.
#include <stdio.h>
#include <stdlib.h>

//Custom libraries.
#include "watermeas_SD.h"

//Code:

//Auxiliar function for file names.
uint32_t SD_next_file_name(uint32_t base_name_size, char* file_name){

    if (file_name[base_name_size + 1] != '9') {
        file_name[base_name_size + 1]++;
    } else if (file_name[base_name_size] != '9') {
        file_name[base_name_size + 1] = '0';
        file_name[base_name_size]++;
    } else {
        return 0;
    }
    return 1;
    
}

//Configure database.
void SD_configdb(SdClassWrap* sd, SD_dbconf* dbconf, char* base_name, uint32_t nfiles_max){

    //The database is now configured.
    dbconf->initialized = 1;

    //Initialize data base's base_name.
    memcpy(dbconf->base_name, base_name, strlen(base_name) * sizeof(char) );

    //Get number of files stored in SD with the same base_name.
    dbconf->nfiles_max = nfiles_max; //NOTE: this line needs to be before the next one!
    dbconf->nfiles = SD_number_of_existing_files(sd, dbconf);
    

}

//Get next unused file name.
void SD_next_unused_file_name(SdClassWrap* sd, SD_dbconf* dbconf, char* file_name){

    //Auxiliary variables.
    uint8_t base_name_size = strlen(dbconf->base_name);

    //Return if database is not initialized.
    if(dbconf->initialized == 0) return ;

    //Check maximum file size.
    if (base_name_size > 6){
        Serial.println("base_name too long.");
        return;
    }

    //Initialize file name.
    sprintf(file_name, "%s00.csv", dbconf->base_name);

    //Look sequentially through a free file name (files go up to number 99).
    while(1) {

        //If file does not exist then this is the next file name to use.
        if(!sd->exists(file_name))
            return ; //Return immidiately.

        //Get next file name.
        if(SD_next_file_name(base_name_size, file_name) == 0){
            Serial.println("File number too large!!");
            return;
        }
        
    }

    //File name is not valid, so cut the string
    file_name[0] = '\0';
    
}

//Get number of files stored in SD with the same base_name.
int SD_number_of_existing_files(SdClassWrap* sd, SD_dbconf* dbconf){

    //Auxiliary variables.
    char file_name[13];
    uint8_t base_name_size = strlen(dbconf->base_name);
    int nfiles = 0;

    //Return if database is not initialized.
    if(dbconf->initialized == 0) return nfiles; //zero files in this case.

    //Check maximum file size.
    if (base_name_size > 6){
        Serial.println("base_name too long.");
        return 0;
    }

    //Initialize file name.
    sprintf(file_name, "%s00.csv", dbconf->base_name);

    //Look sequentially through a free file name (files go up to number 99).
    for(int k = 0; k < dbconf->nfiles_max; k++){

        //Increase file number if the file with this name exists
        if(sd->exists(file_name))
            nfiles++;

        //Get next file name.
        if(SD_next_file_name(base_name_size, file_name) == 0){
            Serial.println("File number too large!!");
            return 0;
        }
        
    }

    //Return number of files.
    return nfiles;
    
}

//Get file name.
int SD_get_file_name(SdClassWrap* sd, SD_dbconf* dbconf, int file_number, char* file_name){

    //Auxiliary variables.
    uint8_t base_name_size = strlen(dbconf->base_name);

    //Get number of existing files.
    dbconf->nfiles = SD_number_of_existing_files(sd, dbconf);
  
    //Check if required file is inside the number of files currently existing in the SD.
    if( (file_number < 0) || (file_number >= dbconf->nfiles) ){
        Serial.print("Required file does not exist.");
        return 0; //Return failure.
    }

    //Initialize file name.
    sprintf(file_name, "%s00.csv", dbconf->base_name);

    //Go through all possible file names.
    for(int k = 0; k < file_number; ){

        //Check the existance of this file
        if(sd->exists(file_name)) k++; //If it exists then go to the next file in the database.

        //Get next file name.
        if(SD_next_file_name(base_name_size, file_name) == 0)
            return 0; //File name cannot be retreived.
    }

    return 1; //Return success.

}

//List file names into string.
void SD_list_files(SdClassWrap* sd, SD_dbconf* dbconf, char* write_buffer){

    //Auxiliary variables.
    uint32_t write_buffer_count = 0;
    uint8_t base_name_size = strlen(dbconf->base_name);
    char file_name[13];

    //Get number of existing files.
    dbconf->nfiles = SD_number_of_existing_files(sd, dbconf);

    //Write number of files.
    write_buffer_count += sprintf(&write_buffer[write_buffer_count], "Number of files: %d\n", dbconf->nfiles);

    //Initialize file name.
    sprintf(file_name, "%s00.csv", dbconf->base_name);

    //Go through all possible file names.
    for(int k = 0; k < dbconf->nfiles; ){

        //Check the existance of this file.
        if(sd->exists(file_name)){
            write_buffer_count += sprintf(&write_buffer[write_buffer_count], "File %d: %s\n", k, file_name);
            k++; //Go to next file.
        }

        //Get to next file name.
        if(SD_next_file_name(base_name_size, file_name) == 0)
            break;
            
    }

}

//Start a new measurement file.
uint32_t SD_initRun(SdClassWrap* sd, char* file_name, SdFileWrap* file){

  //Get current time.
  uint32_t time_log=millis();

  //Try to open the file.
  (*file) = sd->open(file_name, FILE_WRITE);

  //Return the time of file creation.
  return time_log;
  
}

//End measurement file.
void SD_endRun(SdFileWrap *file){ file->close(); }

