

#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SystemInclude.h>
#include <SdFatConfig.h>
#include <SdFat.h>
#include <SdFatUtil.h>

//Defines:

#define error(msg) sd.errorHalt(F(msg)) //Error messages stored in flash.
#define FILE_BASE_NAME "Data" //Log file base name.  Must be six characters or less.

//Global variables:

//SD card variables
SdFat sd; //File system object.
SdFile file; //Log file.
const uint8_t chipSelect = 4; //SD chip select pin. Be sure to disable any other SPI devices such as Enet.
uint8_t file_opened; //variable that allows to check if the file was opened



void setup() {

    //Auxiliary variables.
    const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
    char fileName[13] = FILE_BASE_NAME "00.csv";

    //Starting serial connection
    Serial.begin(9600);

    //Initialize the SD card at SPI_HALF_SPEED to avoid bus errors with breadboards. Use SPI_FULL_SPEED for better performance.
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
        Serial.print("We have failed to open a file.");
        file_opened = 0;
    }
    else {
      Serial.print("We have succeeded to open a file.");
      file_opened = 1;
    }

    //Find an unused file name.
    if (BASE_NAME_SIZE > 6){
        error("FILE_BASE_NAME too long");
    }
    while (sd.exists(fileName)) {
        if (fileName[BASE_NAME_SIZE + 1] != '9') {
            fileName[BASE_NAME_SIZE + 1]++;
        } else if (fileName[BASE_NAME_SIZE] != '9') {
            fileName[BASE_NAME_SIZE + 1] = '0';
            fileName[BASE_NAME_SIZE]++;
        } else {
            error("Can't create file name");
        }
    }
    if (!file.open(fileName, O_CREAT | O_WRITE | O_EXCL)) {
        error("file.open");
    }

    //Write in file.
    file.write("It worked.");
    file.println();

    //Close file
    file.close();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Was the file opened?");
  Serial.print(file_opened);
  Serial.print("\n");

}

