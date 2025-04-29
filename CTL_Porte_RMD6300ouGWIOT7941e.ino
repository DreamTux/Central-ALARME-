// Chargement des LIBRAIRIES
// #include <Gwiot7941e.h>     // Version Gwiot
#include <rdm6300.h>        // Version RDM6300
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <RGBLED.h>

// Definition des GPIO (Relier TX a D4)
#define RDM6300_RX_PIN 13                 // Le lesteur RFID sur "force hardware uart" (D7) Version Gwiot
//#define GWIOT_7941E_RX_PIN 13           // Le lesteur RFID sur "force hardware uart" (D7) Version RDM6300
#define READ_LED_PIN 16                 // Juste pour le visuel sur la ESP8266
const int AIMANT_PORTE = 12;            // AIMANT_PORTE sur D5 == 14(Relay)
const int BUZZER = 14;                  // BUZZER sur D6 (BUZZER)
RGBLED rgbLed(5,4,0,COMMON_CATHODE);    // Rouge en D1 Vert en D2 et Bleu en D3

// Declaration des variables
const char* host = "192.168.1.1";  // Le site a consulter
String url = "/door/api.php";      // Url de l'API       
int adcvalue = 0;
const int httpPort = 80;
String message;
String Vporte = "WC01";
String serverMessage;
int ValeurDuTag;
String RetourCtrl;
String RetourCtrlSOS;
unsigned long previousMillis = 0;
const long interval = 30000;
String LitlaPage(String Tagactrl);

// Les information pour le WIFI
#define MySSID "ssidwifi" 
#define MyWifiPassword "motdepassewifi"

// Chargement des PILOTES
// Gwiot7941e gwiot7941e; // Vertion Gwiot
Rdm6300 rdm6300; // Version Rdm6300
ESP8266WiFiMulti WiFiMulti;
WiFiClient client;

void setup()
{
  WiFiMulti.addAP(MySSID, MyWifiPassword);
  Serial1.begin(115200);
  Serial1.println();
  Serial1.print("Connecting to Wifi: ");
  Serial1.println(MySSID);  

  // Attendra la connexion wifi.
  rgbLed.writeRGB(255,0,0);
  while(WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial1.print("."); tone(BUZZER, 1000); delay(150);
  }
  if(WiFiMulti.run() == WL_CONNECTED) {
    rgbLed.writeRGB(0,255,0);
    Serial1.println("");
    Serial1.print("Connexion au WiFi. ");
    Serial1.print("IP address: ");
    Serial1.println(WiFi.localIP());
    Bipdecnxwifi();
    rgbLed.turnOff();
  }
    // Configuration des GPIO
    pinMode(BUZZER, OUTPUT);
    pinMode(AIMANT_PORTE, OUTPUT);
    pinMode(READ_LED_PIN, OUTPUT);
    digitalWrite(READ_LED_PIN, LOW);
    digitalWrite(AIMANT_PORTE, LOW);
     
    // gwiot7941e.begin(GWIOT_7941E_RX_PIN);  // Version Gwiot
    rdm6300.begin(RDM6300_RX_PIN); // Version Rdm6300
    Serial1.println("\nPresente le badge RFID devant le lecteur 7941E...");
}

void loop() {
    // La boucle du progamme
    unsigned long currentMillis = millis(); // Init de millis pour que toutes les 30 secondes on regarde le mode SOS pas recommendé mais rassurant
    rgbLed.writeRGB(0,0,255);
    // if (gwiot7941e.update()) { // Version Gwiot
    if (rdm6300.get_new_tag_id()){ // Version Rdm6300
        // Cette partie n'est lue que si le lecteur a lue un TAG
        rgbLed.writeRGB(255,128,0);
        // int ValeurDuTag = gwiot7941e.getLastTagId(); // Version Gwiot
        int ValeurDuTag = rdm6300.get_tag_id(); // Version Rdm6300
        digitalWrite(READ_LED_PIN, rdm6300.get_tag_id()); // Version Rdm6300
        Biplecture();
        RetourCtrl = LitlaPage(ValeurDuTag);
        
        Serial1.println("Le tag RFID " + String(ValeurDuTag)+ " est " + RetourCtrl);
                
        if (RetourCtrl == "ok") {
                 
          digitalWrite(AIMANT_PORTE, HIGH); // Envoie la commande d'ouverture
          BipTagValide(); // Petit bip pour dir ok
          delay(100);
          digitalWrite(AIMANT_PORTE, LOW); // Envoie la commande de fermeture
          
        
        } else {
          BipTagInconue();
        
        }
}
    delay(250);
    // On force la verification du mode SOS
    // Pour pouvoir ouvrir une porte si on a oubliè le badge derrier celle-ci
    
    if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    RetourCtrlSOS = LitlaPageSOS();
    if (RetourCtrlSOS == "ok") {
          digitalWrite(AIMANT_PORTE, HIGH); // Envoie la commande d'ouverture
          BipTagValide(); // Petit bip pour dir ok
          delay(100);
          digitalWrite(AIMANT_PORTE, LOW); // Envoie la commande de fermeture          
    }
    }
    delay(250);
  
}

void BipTagValide(){
        rgbLed.writeRGB(0,255,0);
        tone(BUZZER, 500); delay(100);
        noTone(BUZZER); delay(50);        
        tone(BUZZER, 1000); delay(100);        
        noTone(BUZZER);
}

void Biplecture() {
        tone(BUZZER, 2000); delay(100);
        noTone(BUZZER); delay(50);        
        tone(BUZZER, 3000); delay(100);        
        noTone(BUZZER);
}

void BipTagInconue() {
        rgbLed.writeRGB(255,0,0);
        tone(BUZZER, 1000); delay(150);
        tone(BUZZER, 3000); delay(150);
        tone(BUZZER, 1100); delay(150);
        tone(BUZZER, 3100); delay(150);
        tone(BUZZER, 1200); delay(150);
        tone(BUZZER, 3200); delay(150);
        tone(BUZZER, 1300); delay(150);
        tone(BUZZER, 3300); delay(150);
        tone(BUZZER, 1400); delay(150);
        tone(BUZZER, 3400); delay(150);        
        noTone(BUZZER);

}

void Bipdecnxwifi() {
        tone(BUZZER, 3000); delay(100);
        noTone(BUZZER); delay(50);        
        tone(BUZZER, 1000); delay(100);        
        noTone(BUZZER); delay(50);        
        tone(BUZZER, 3000); delay(100);        
        tone(BUZZER, 2000); delay(100);
        noTone(BUZZER); delay(50);        
        tone(BUZZER, 1000); delay(100);        
        noTone(BUZZER);
}

String LitlaPage(int Tagactrl) {
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient
  
    http.begin(client, "http://192.168.1.1/door/api.php?AIMANT_PORTE="+ Vporte +"&tag=" + String(Tagactrl)); // + Tagactrl);      // Destination de la requette
    http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    
    int httpCode = http.GET();   // Envoie la valeur a controler
    String payload = http.getString();                  // Recupere la reponse
 
    //Serial1.println(httpCode);   //Debug le code retour
    //Serial1.println(payload);    //Recupere la valeur de retour
 
    http.end();  //Close connection
 
  return payload;  
}

String LitlaPageSOS() {
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient
  
    http.begin(client, "http://192.168.1.1/door/api.php?AIMANT_PORTE=111&tag=SOSSOSSOS"); // + Tagactrl);      // Destination de la requette
    http.addHeader("Content-Type", "text/plain");  //Specify content-type header
    
    int httpCode = http.GET();   // Envoie la valeur a controler
    String payload = http.getString();                  // Recupere la reponse
 
    //Serial1.println(httpCode);   //Debug le code retour
    //Serial1.println(payload);    //Recupere la valeur de retour
 
    http.end();  //Close connection
 
  return payload;  
}
