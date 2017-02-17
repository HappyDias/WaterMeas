//Libraries.

//Standard libraries.

//Custom libraries.
#include "watermeas_serial.h"
#include "watermeas_SD.h"
#include "watermeas_measure.h"
#include <TimeLib.h>

//Defines:

#define error(msg) sd.errorHalt(F(msg)) //Error messages stored in flash.
#define FILE_BASE_NAME "Data" //Log file base name.  Must be six characters or less.

//Global variables:

//SD card variables:
SdFat sd; //File system object.
SdFile file; //Log file.
SD_dbconf dbconf; //Structure containing the entire database configuration.
const uint8_t chipSelect = 4; //SD chip select pin. Be sure to disable any other SPI devices such as Enet.

//Serial variables:
char read_buffer[150];
char write_buffer[150];
int serial_read_count;

//Measurement variables:
uint32_t theTime;
uint32_t active_run;
uint32_t last_measurement_time;

//Testing the interrupt routines
const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;

void blink() {
  state = !state;
  //Serial.print("WHOOOOPT");
}

void setup() {

    //Starting serial connection
    Serial.begin(115200);
    serial_read_count = 0; //Resetting the read counter (the program did not read anything yet).

    //Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with breadboards. Use SPI_FULL_SPEED for better performance.
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
        Serial.print("We have failed to open a file.");
    }
    else {
      Serial.print("We have succeeded to open a file.");
      SD_configdb(&sd, &dbconf, FILE_BASE_NAME, 90); //Proceed with file initialization if SD library is working.
    }

    //In the beginning there are no ongoing measurement runs.
    active_run = 0;

    //Interrupt pins - testing
    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
    
}

void loop() {
    digitalWrite(ledPin, state);
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
                if (!file.open(fileName, O_READ | O_WRITE)){
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

            //Auxiliary variables for the new file name.
            char file_name[13];

            //Get the new file name.
            SD_next_unused_file_name(&sd, &dbconf, file_name);
            
            //Open the new file name and initialize the measurement run.
            theTime=SD_initRun(file_name, &file);

            //Check if file is opened so the run can proceed.
            if(file.isOpen()){
                active_run = 1;
                last_measurement_time = millis();
                Serial.print("Measurement started using the file name: ");
                Serial.println(file_name);
            }
            else
                Serial.println("The measurement run could not be started (check used file name).");
                
        }

        else if(read_buffer[0] == SERIAL_COMMAND_END_RUN){ //An end run command finalizes an ongoing run and closes the run file

            //Close measurement run file if a run was active.
            if(active_run){
                SD_endRun(&file);
                active_run = 0; //The run is nolonger active.
                Serial.println("Ongoing measurement run stopped.");
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
            Serial.println("Command not recognized")  ;
        }

    }

    //Acquire data if there is an on-going measurement run.
    if(active_run){

        //Acquire according to the specified sampling time.
        if( (millis() - last_measurement_time) >= SAMPLING_TIME ){
            last_measurement_time = millis();
            measure_logData(&file,theTime);
        }
    }

}
