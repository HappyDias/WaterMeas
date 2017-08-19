#ifndef WATERMEAS_WEBSERVER
#define WATERMEAS_WEBSERVER

//Webserver includes:

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//Initializing webserver.
void webserver_init(ESP8266WebServer* server, char* ssid, char* password);

#endif //WATERMEAS_WEBSERVER

