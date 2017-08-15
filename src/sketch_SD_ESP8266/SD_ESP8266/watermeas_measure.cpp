#include "watermeas_measure.h"

//Interrupt routine variables:
const byte ledPin = 18;
const byte interruptPin = 2;
volatile byte measure_button_pressed;
volatile uint32_t measure_button_press_time;
volatile int measure_button_first_time;
int delay_time;

//Measurement variables:
SdFileWrap measure_file; //Log file.
uint32_t theTime;
volatile byte measurement_run_active = LOW;
int measurement_run_initialized = LOW;
uint32_t last_measurement_time;

//Routine that togles measurement run status.
void togle_measurement_run_status() {

    //Only togle measurement run status if this is the first time the button is pressed
    if(measure_button_pressed == 0){
        measure_button_first_time = 1;
        measure_button_pressed = 1; //Activate first button press event flag.
        measurement_run_active = !measurement_run_active; //togle measurement run status.
    }
    
}

//Configure measure controls.
void measure_config(int delay_time_local){

    delay_time = delay_time_local;
    measure_button_pressed = 0;
    measurement_run_active = LOW;
    measurement_run_initialized = LOW;
    pinMode(ledPin, OUTPUT);
    pinMode(interruptPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interruptPin), togle_measurement_run_status, RISING);
    
}

//Start measurement run
int measure_start_run(SdClassWrap* sd, SD_dbconf* dbconf){

    //Auxiliary variables for the new file name.
    char file_name[13];

    //Check if run has already started.
    
    if(measurement_run_initialized){
        Serial.println("Measurement run already active.");
        return measurement_run_active;
    }

    //Get the new file name.
    SD_next_unused_file_name(sd, dbconf, file_name);
    
    //Open the new file name and initialize the measurement run.
    theTime=SD_initRun(sd, file_name, &measure_file);

    //Check if file is opened so the run can proceed.
    if(!measure_file){
        measurement_run_active = HIGH;
        measurement_run_initialized = HIGH;
        last_measurement_time = millis();
        Serial.print("Measurement started using the file name: ");
        Serial.println(file_name);
    }
    else{
        measurement_run_active = LOW;
        measurement_run_initialized = LOW;
        Serial.println("The measurement run could not be started (check used file name).");
    }

    return measurement_run_active;
    
}

//Stop measurement run
int measure_stop_run(SdClassWrap* sd, SD_dbconf* dbconf){
    SD_endRun(&measure_file);
    measurement_run_active = LOW;
    measurement_run_initialized = LOW;
    Serial.println("Ongoing measurement run stopped.");
    return measurement_run_active;
}

//Manage measure run start and end.
int measure_control(SdClassWrap* sd, SD_dbconf* dbconf){

    //Visualy identify measurement run status.
    digitalWrite(ledPin, measurement_run_active);

    //Control the button press times and initializations
    if(measure_button_pressed){

        //check if this is the first time this is is checked, so the delay can be set and the propper initializations as well     
        if(measure_button_first_time){
            measure_button_first_time = 0;
            measure_button_press_time = millis();
            if(measurement_run_active)
                measure_start_run(sd, dbconf); //Measurement run can be set to low here if initialization failed.
            else
                measure_stop_run(sd, dbconf);
        }
        
        if( (millis() - measure_button_press_time) > delay_time ){ //Button can only be pressed within periodes of delay_time to filter out transient unstable switch responses
            measure_button_pressed = 0;
            Serial.println("Ready for next button press.");
        }
    }

    //Acquire data if there is an on-going measurement run.
    if(measurement_run_active){

        //Acquire according to the specified sampling time.
        if( (millis() - last_measurement_time) >= SAMPLING_TIME ){
            last_measurement_time = millis();
            measure_logData(&measure_file,theTime);
        }
    }

    //Return measurement run status.
    return measurement_run_active;
            
}

//Make measurement, write time and value to buffer.
void measure_makeMeasurement(char* bufferStr, uint32_t startTime){
  String dataStr;
  uint32_t currentTime;
  int sensorValue;
  int analogInPin=A0;

  //Get measurement from sensor
  sensorValue = analogRead(analogInPin);

  //Eventually convert the data here into Newtons
  
  //Update time
  currentTime=millis()-startTime;

  //Concatenate data string
  dataStr=currentTime;
  dataStr=dataStr+",";
  dataStr=dataStr+sensorValue;
  dataStr=dataStr+"\n";

  sprintf(bufferStr,"%s",dataStr.c_str());
}

//Write data to measurement file
uint32_t measure_logData(SdFileWrap* fileStream, uint32_t startTime) {
  char measurement[14];
  
  // Write data to file.  Start with log time in micros.
  measure_makeMeasurement(measurement,startTime);
  
  return ( (fileStream->write((char*)measurement))==sizeof(measurement));
}
