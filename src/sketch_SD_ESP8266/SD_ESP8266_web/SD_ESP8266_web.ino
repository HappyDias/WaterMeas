//Libraries:

//Standard libraries:

#include <TimeLib.h>

//Custom libraries:

#include "watermeas_defines.h"
#include "watermeas_SD.h"
#include "watermeas_measure.h"
#include "watermeas_webserver.h"

//Defines:

#define FILE_BASE_NAME "Data" //Log file base name.  Must be six characters or less.

//Global variables:

//SD card variables:
SdClassWrap sd; //File system object.
SdFileWrap file; //File descriptor.
SD_dbconf dbconf; //Structure containing the entire database configuration.
ESP8266WebServer server; //Webserver variable.
const char* ssid = "TP-LINK_886D"; //Wifi variables.
const char* password = "54122024";
const uint8_t chipSelect = 15; //SD chip select pin. Be sure to disable any other SPI devices such as Enet.

//Serial variables:
char read_buffer[512];
char write_buffer[512];
int serial_read_count;

//Measurement variables:
int active_run;

//Server functions.
void handleRoot(){server.send(strlen("Server is ON."), "text/plain", "Server is ON.");}

//LS - Get all the files by order (then write file name to serial).
void handleLs(){
    Serial.print("LS command was issued.\n");
    SD_list_files(&sd, &dbconf, write_buffer);
    server.send(strlen(write_buffer), "text/plain", write_buffer);
}

//Read a specific file.
void handleRead(){
  
    //Auxiliary variables.
    char fileName[13];
    int file_number, aux, aux1, num_bytes, write_buffer_count = 0;
    char* uri_text;

    //Identify command.
    Serial.print("READ_FILE command was issued.\n");

    //Check number of arguments in command
    if(server.args() != 2){
        server.send(strlen("Too many arguments for read command.\n"), "text/plain", "Too many arguments for read command.\n");
        return; 
    }

    //Get file name.
    file_number = atoi(server.arg(0).c_str());
    num_bytes = atoi(server.arg(1).c_str());
    if(SD_get_file_name(&sd, &dbconf, file_number, fileName) == 1){

        //Open file for reading.
        
        Serial.println(fileName);
        if ( !(file = sd.open(fileName, O_READ)) ){
            write_buffer_count += sprintf(&write_buffer[write_buffer_count], "File %d not opened.\n", file_number); //Issue an error if open failed.
            server.send(strlen(write_buffer), "text/plain", write_buffer);
        }

        //Read file.
        else {

            //Notify the user of the action.
            write_buffer_count = 0;
            write_buffer_count += sprintf(&write_buffer[write_buffer_count], "Reading file %s...\n", fileName);
            server.send(strlen(write_buffer), "text/plain", write_buffer);

            //Read target file.
            aux = 0;
            while( (aux1 = file.read(read_buffer, num_bytes - aux)) > 0){
                write_buffer_count = 0;
                memcpy(&write_buffer[write_buffer_count], read_buffer, aux1);
                write_buffer_count += aux1;
                write_buffer[write_buffer_count] = '\0';
                aux += aux1;
                server.send(strlen(write_buffer), "text/plain", write_buffer);
            }

            Serial.println(aux);
            Serial.println(strlen(write_buffer));
            write_buffer_count = 0;
            write_buffer_count += sprintf(&write_buffer[write_buffer_count], "END\n");
            server.send(strlen(write_buffer), "text/plain", write_buffer);

            //Close file.
            file.close();
            
        }

    }
    else {
      sprintf(write_buffer, "File to read - %d - does not exist.\n", file_number); //Issue an error if filedoes not exist.
      server.send(strlen(write_buffer), "text/plain", write_buffer);
    }
    
}

//An init run command initializes a run file and gives the program permission to write sensor data to it.
void handleInitRun(){
    Serial.print("INIT_RUN command was issued.\n");
    active_run = measure_start_run(&sd, &dbconf, write_buffer);
    server.send(strlen(write_buffer), "text/plain", write_buffer);
}

//An end run command finalizes an ongoing run and closes the run file
void handleEndRun(){

    Serial.print("END_RUN command was issued.\n");

    //Close measurement run file if a run was active.
    if(active_run){
        active_run = measure_stop_run(&sd, &dbconf, write_buffer); //The run is nolonger active.
        server.send(strlen(write_buffer), "text/plain", write_buffer);
    }
    else
        server.send(strlen("No ongoing measurement run to close. Ignoring command...\n"), "text/plain", "No ongoing measurement run to close. Ignoring command...\n");
        
}

void handleNotFound(){
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i=0; i<server.args(); i++){
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void setup() {

    //Starting serial connection.
    Serial.begin(115200);

    //Initializing HTTP server and services
    webserver_init(&server, (char*)ssid, (char*)password);
    server.on("/", handleRoot);
    server.on("/ls", handleLs);
    server.on("/read", handleRead);
    server.on("/init_run", handleInitRun);
    server.on("/end_run", handleEndRun);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");

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
    
    //Manage webserver
    server.handleClient();

    //Control measurement run status
    active_run = measure_control(&sd, &dbconf, write_buffer);

}

