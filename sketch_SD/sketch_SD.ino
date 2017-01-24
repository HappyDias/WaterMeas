//Libraries.

//Standard libraries.

//Custom libraries.
#include "watermeas_serial.h"
#include "watermeas_SD.h"
#include "watermeas_files.h"

//Defines:

#define error(msg) sd.errorHalt(F(msg)) //Error messages stored in flash.
#define FILE_BASE_NAME "Data" //Log file base name.  Must be six characters or less.

//Global variables:

//SD card variables
SdFat sd; //File system object.
SdFile file; //Log file.
SD_dbconf dbconf; //Structure containing the entire database configuration.
const uint8_t chipSelect = 4; //SD chip select pin. Be sure to disable any other SPI devices such as Enet.
String fileNAME;
uint32_t value, theTime;

//Serial variables
char read_buffer[100];
char write_buffer[100];
int serial_read_count;

void setup() {

    //Starting serial connection
    Serial.begin(9600);
    serial_read_count = 0; //Resetting the read counter (the program did not read anything yet).

    //Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with breadboards. Use SPI_FULL_SPEED for better performance.
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
        Serial.print("We have failed to open a file.");
    }
    else {
      Serial.print("We have succeeded to open a file.");
      SD_configdb(&sd, &dbconf, FILE_BASE_NAME, 90); //Proceed with file initialization if SD library is working.
    }   


    value=1;
    fileNAME=FILE_BASE_NAME;
    fileNAME=fileNAME+value;
    fileNAME=fileNAME+".csv";
}

void loop() {
  
    //Read data from serial
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
                    while( (aux = file.read(write_buffer, 10)) > 0){
                        Serial.write(write_buffer, aux);
                    }

                    //Close file.
                    file.close();
                    
                }

            }
  
        }

    }

    if(millis()<10000*value){
        if(file.isOpen()){
            Serial.print("open");
            logData(&file,theTime);
        }
        else{
            Serial.print("closed");
            theTime=initRun(fileNAME.c_str(), &file);
        }
    }
    else{
        endRun(&file);
        value=value+1;
        fileNAME=FILE_BASE_NAME;
        fileNAME=fileNAME+value;
        fileNAME=fileNAME+".csv";
        Serial.println(10000*value);
        Serial.println(fileNAME.c_str());
    }
}

