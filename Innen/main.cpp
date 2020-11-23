#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Base64.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

#define DHTPIN 5     // Digital pin connected to the DHT sensor -> Data von Sensor an GPOI 5  //nicht 2 benutzen!!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 -> Welcher Sensor Typ
DHT dht(DHTPIN, DHTTYPE, 22); //DHT Objekt (dht), dem Pin und Typ zugeordnet werden

float merketemperatur=0;
float merkeaussentemperatur=0;

//////////////////////////////////////
float Wert_1 = 0;
float Wert_2 = 0;
//////////////////////////////////////

const char* ssid = "LANCOM"; //Ihr Wlan,Netz SSID eintragen
const char* pass = "12345678"; //Ihr Wlan,Netz Passwort eintragen
IPAddress ip(172,23,56,228); //Feste IP des neuen Servers, frei wählbar
IPAddress gateway(172,23,56,254); //Gatway (IP Router eintragen)
IPAddress subnet(255,255,255,0); //Subnet Maske eintragen

ESP8266WebServer server(80);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void handleRoot() {
 //Temperatur auch bei Url-Aufruf zurückgeben
 String message="*** Ueberwachungs Server ***\n";
 String tempstr= String(merketemperatur, 2); 
 message += "Temperatur Innen : " + tempstr +"\n";
 tempstr= String(merkeaussentemperatur, 2); 
 message += "Temperatur Aussen: " + tempstr +"\n";
 /////////////////////////////////

String Wert1String = String(Wert_1);
String Wert2String = String(Wert_2);

message += "LuftWert1: " + Wert1String +"\n";
message += "LuftWert2: " + Wert2String +"\n";

 ////////////////////////////////
 server.send(200, "text/plain", message);
}


void handleTemperatur() {
 //printUrlArg(); //fuer Debugz Zwecke

 String stemp =server.arg("wert");
 float temperatur = stemp.toFloat();
 if (merkeaussentemperatur!=temperatur) {
 //zeigeAussenTemperatur(temperatur);
 merkeaussentemperatur=temperatur;

 }

/////////////////////////////////////
 String stemp1 = server.arg("wert1");
 Wert_1 = stemp1.toFloat();

 String stemp2 = server.arg("wert2");
 Wert_2 = stemp2.toFloat();

///////////////////////////////////////


 //Temperatur auch bei Url-Aufruf zurückgeben
 String message="*** Ueberwachungs Server ***\n";
 String tempstr= String(merketemperatur, 2); 
 message += "Temperatur Innen : " + tempstr +"\n";
 tempstr= String(merkeaussentemperatur, 2); 
 message += "Temperatur Aussen: " + tempstr +"\n";

/////////////////////////////////

String Wert1String = String(Wert_1);
String Wert2String = String(Wert_2);

message += "LuftWert1: " + Wert1String +"\n";
message += "LuftWert2: " + Wert2String +"\n";

 ////////////////////////////////

 server.send(200, "text/plain", message);
}

void printUrlArg() {
 //Alle Parameter seriell ausgeben
 String message="";
 for (uint8_t i=0; i<server.args(); i++){
 message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
 }
 server.send(200, "text/plain", message);
 Serial.println(message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Serial.printf("Jetzt gehts los!");
  Serial.println(" \r\nINIT \r\n") ;

  dht.begin();

  WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  } 
  Serial.println("");
  Serial.print("Verbunden mit ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/",handleRoot) ;
  server.on("/sensor/temperatur/", handleTemperatur);
  server.begin();
  
  Serial.println("HTTP Server wurde gestartet!");
}

void loop() {
  server.handleClient();
 
 float temperatur = dht.readTemperature();
 //Serial.print(temperatur);
 if (merketemperatur!=temperatur) {
 //zeigeTemperatur(temperatur);
 merketemperatur=temperatur;
 }

  delay(500); 
}