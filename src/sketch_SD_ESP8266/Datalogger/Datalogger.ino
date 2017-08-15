//SD card dataloger for ESP8266 - basiacally the same as the arduino

//SD card library includes.
#include <SPI.h>
#include <SD.h>

//Auxiliary global variables.
const int chipSelect = 15;
volatile double sensor_value, current_value; //Where the sensor values are stored at each time step.
volatile int count; //Counter that determines the time step.

//Interrupt routine that reads the analog sensor pin.
void timer1_ISR(void){
  sensor_value = 0.95*sensor_value + 0.05*analogRead(A0);
  count++;
  timer0_write(ESP.getCycleCount() + 800000L); 
}

void setup()
{
  //Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; //wait for serial port to connect. Needed for Leonardo only.
  }
  
  Serial.println("Initializing SD card...");

  //See if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    //don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  //Initialize sensor readings.
  count = 0;
  sensor_value = analogRead(A0);
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timer1_ISR);
  timer0_write(ESP.getCycleCount() + 800000L); //80MHz == 1sec
  interrupts();
}

void loop()
{
  //Make a string for assembling the data to log:
  String dataString = "";

  //Serial.println("I am here.");

  //Read the sensor.
  //The sensor is being read with a timer.
  current_value = sensor_value;
  dataString += String(current_value);
  dataString += ";";
  
  //Open the file. Note that only one file can be open at a time,
  //so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  //If the file is available, write to it:
  if(dataFile){
    dataFile.println(dataString);
    dataFile.close();
    //Print to the serial port too:
    Serial.println(dataString);
  }
  //If the file isn't open, pop up an error:
  else{
    Serial.println("error opening datalog.txt");
  }
 
}





