#include "watermeas_webserver.h"

//The webserver variable (using port 80)
ESP8266WebServer local_server_var(80);

//Initializing webserver.
void webserver_init(ESP8266WebServer* server, char* ssid, char* password){
  
    //Start Wifi connection.
    Serial.print("Initializing Wifi connection");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    //Configuring DNS.
    if(MDNS.begin("esp8266")){
        Serial.println("MDNS responder started");
    }

    //Return server variable.
    (*server) = local_server_var;
    
}

