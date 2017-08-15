//Libraries:

//Standard libraries:

#include <TimeLib.h>

//Custom libraries:

#include "watermeas_defines.h"
#include "watermeas_SD.h"
#include "watermeas_measure.h"

//Defines:

#define FILE_BASE_NAME "Data" //Log file base name.  Must be six characters or less.

//Global variables:

//SD card variables:
SdClassWrap sd; //File system object.
SdFileWrap file; //File descriptor.
SD_dbconf dbconf; //Structure containing the entire database configuration.
const uint8_t chipSelect = 15; //SD chip select pin. Be sure to disable any other SPI devices such as Enet.

//Serial variables:
char read_buffer[150];
char write_buffer[150];
int serial_read_count;

//Measurement variables:
int active_run;

void setup() {

    //Starting serial connection.
    Serial.begin(115200);
    serial_read_count = 0; //Resetting the read counter (the program did not read anything yet).

    //Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with breadboards. Use SPI_FULL_SPEED for better performance.
    if (!sd.begin(chipSelect)) {
        Serial.print("SD card init failed.");
    }
    else {
      Serial.print("SD card initialized.");
      SD_configdb(&sd, &dbconf, FILE_BASE_NAME, 90); //Proceed with file initialization if SD library is working.
    }

    //In the beginning there are no ongoing measurement runs.
    active_run = 0;

    //Setup interrupt to control measurement run status.
    measure_config(1000);
    
}

void loop() {
    
    //Read data from serial.
    if(Serial.available() > 0)
        for(int k = 0, n = Serial.available(); k < n; k++) read_buffer[serial_read_count++] = Serial.read();

    //Check if a full command has arrived.
    if(serial_read_count >= 2){ //A command is composed by an identification byte and a command body byte.

        //Reset read count.
        serial_read_count = 0;

        //Check for specific serial commands
        if(read_buffer[0] == SERIAL_COMMAND_LS){ //An ls command asks the sensor to give the information of all the stored file (number of files and their name).

            //Identify command.
            Serial.print("LS command was issued.\n");

            //Get all the files by order (then write file name to serial).
            SD_list_files(&sd, &dbconf, write_buffer);

        }
        
        else if(read_buffer[0] == SERIAL_COMMAND_FILE_READ){ //A read command reads a file with a specific number

            //Auxiliary variables.
            char fileName[13];
            int file_number, aux;

            //Identify command.
            Serial.print("READ_FILE command was issued.\n");

            //Get file name.
            file_number = read_buffer[1] - 48;
            if(SD_get_file_name(&sd, &dbconf, file_number, fileName) == 1){

                //Open file for reading.
                Serial.println(fileName);
                if ( !(file = sd.open(fileName, O_READ | O_WRITE)) ){
                    sprintf(write_buffer, "File %d not opened.\n", file_number); //Issue an error if open failed.
                    Serial.print(write_buffer);
                }

                //Read file.
                else {

                    //Notify the user of the action.
                    sprintf(write_buffer, "Reading file %s...\n", fileName);

                    //Read target file.
                    while( (aux = file.read(write_buffer, 128)) > 0){
                        Serial.write(write_buffer, aux);
                    }

                    //Close file.
                    file.close();
                    
                }

            }
  
        }

        else if(read_buffer[0] == SERIAL_COMMAND_INIT_RUN){ //An init run command initializes a run file and gives the program permission to write sensor data to it

            //Start run.
            active_run = measure_start_run(&sd, &dbconf);
                
        }

        else if(read_buffer[0] == SERIAL_COMMAND_END_RUN){ //An end run command finalizes an ongoing run and closes the run file

            //Close measurement run file if a run was active.
            if(active_run){
                active_run = measure_stop_run(&sd, &dbconf); //The run is nolonger active.
            }
            else 
                Serial.println("No ongoing measurement run to close. Ignoring command...");
            
        }

        else if(read_buffer[0] == SERIAL_COMMAND_SYNC_TIME){ //Time sync command recieves a time string to get the most recent time onto the arduino

            //Aux vars for time
            char sent_time[11];
            unsigned long sent_time_int;

            Serial.println("A time Sync command was issued");
            //Getting timestamp - ten character string
            for(int k = 0; k < 10; k++){
                sent_time[k]=read_buffer[k+2];
            }
            sent_time_int=atol(sent_time);
            if(sent_time_int>1000000000 && sent_time_int<9999999999){

                Serial.println("Good date, updating time");
                Serial.println(sent_time_int);
                setTime(sent_time_int);
                Serial.print(hour());
                Serial.print(":");
                Serial.print(minute());
                Serial.print(":");
                Serial.print(second());
                Serial.print(" ");
                Serial.print(day());
                Serial.print(" ");
                Serial.print(month());
                Serial.print(" ");
                Serial.print(year()); 
                Serial.println();
            }
            else
                Serial.println("Bad date, not updated");
                
        }

        else{

            //Unrecognized command
            Serial.println("Command not recognized");
        }

    }

    //Control measurement run status
    active_run = measure_control(&sd, &dbconf);

}

