#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Base64.h>
#include <OneWire.h>
#include <SDS011.h>
#include <SoftwareSerial.h>


#define DHTPIN 4     // Digital pin connected to the DHT sensor -> Data von Sensor an GPOI 5  //nicht 2 benutzen!!
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321 -> Welcher Sensor Typ
DHT dht(DHTPIN, DHTTYPE, 22); //DHT Objekt (dht), dem Pin und Typ zugeordnet werden

  const char* ssid = "LANCOM"; //Hier SSID eures WLAN Netzes eintragen
  const char* password = "12345678"; //Hier euer Passwort des WLAN Netzes eintragen

  const char* host = "172.23.56.228"; //Server der die temperatur empfangen soll 
                                   //(er sollte feste IP haben)
  const char* script = "/sensor/temperatur/"; //URL/Verzeichnis das wir gewaehlt haben

  const char* passwort = "passwortxyz"; //Passwort, derzeit ungenutzt
  const char* idnr = "1"; //Hier kann man dem Sensor eine beliebe ID zuteilen (derzeit ungenutzt)

////////////////////////////////////////
#define SDS_RX 5
#define SDS_TX 3


float p10,p25;
int error;
SDS011 my_sds;
/////////////////////////////////////////////


void setup() {

  Serial.begin(115200);
  delay(10);

//////////////////////////////////////////////////
  my_sds.begin(5,3);  //GPIO2 = D4
	Serial.begin(115200);
/////////////////////////////////////////////////////

  Serial.println();
  Serial.println();
  Serial.print("Verbinde mich mit Netz: ");
  Serial.println(ssid);
 
  dht.begin();
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Verbindung aufgebaut"); 
  Serial.print("Eigene IP des ESP-Modul: ");
  Serial.println(WiFi.localIP());
}


void loop() {

  float t = dht.readTemperature();
  if (isnan(t)) {
    t = 9999999;
  };
 // Serial.printf(" ");
 // Serial.print(t);
 // Serial.printf(" ");

  char temperaturStr[6];
  dtostrf(t, 2, 1, temperaturStr);
  
  Serial.printf("TemperturAußen: ");
  Serial.print(temperaturStr);
  Serial.printf(" ");

  String Wert_1 = "leer";
  String Wert_2 = "leer";
  ///////////////////////////////////////////////////////
  error = my_sds.read(&p25,&p10);
	if (! error) {
		Serial.println("P2.5: "+String(p25));
		Serial.println("P10:  "+String(p10));

     Wert_1 = String(p25);
     Wert_2 = String(p10);
    Serial.println(Wert_1);
    Serial.println(Wert_2);
	}

  
/////////////////////////////////////////////////////

  int versuche=1; 
  WiFiClient client;
  const int httpPort = 80;
  int erg;
 
  do{
    
    Serial.print("Verbindungsaufbau zu Server ");
    Serial.println(host);
    
    erg =client.connect(host, httpPort);
    Serial.println(erg);
    if (!erg) {
      versuche++; 
      Serial.println("Verbindungsaufbau nicht moeglich!!!");
      if (versuche>3){
        Serial.println("Klappt nicht, ich versuche es spaeter noch mal!");
        client.stop();
        WiFi.disconnect(); 
        ESP.deepSleep(60*1000000); //Etwas später neu probieren, solange schlafen 10*60000000
      }
    }
    delay(1000);
  } 
  while (erg!=1);
 
  String url = script; //Url wird zusammengebaut
  url += "?pw=";
  url += passwort;
  url += "&idnr=";
  url += idnr;
  url += "&wert=";
  url += temperaturStr;
  ////////////////////
  url += "&wert1=";
  url += Wert_1;
  url += "&wert2=";
  url += Wert_2;
  /////////////////////
  Serial.print("Folgende URL wird aufgerufen: ");
  Serial.println(host + url);
 
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" + 
  "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
        Serial.println("Timeout!");
        Serial.println("Uebergabe klappt nicht, ich versuche es spaeter noch mal!");
        client.stop();
        WiFi.disconnect(); 
        ESP.deepSleep(60*1000000); //Etwas später 60 Sekunden neu probieren,solange schlafen  60*1000000 1min 15*60000000 15min
    }
  }

  Serial.print("Rueckgabe vom Server:\n");
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  client.stop();
  WiFi.disconnect(); 
  Serial.println("\nVerbindung beendet");
 


  Serial.print("Schlafe jetzt ...");
  ESP.deepSleep(5*60000000);//5min
}




