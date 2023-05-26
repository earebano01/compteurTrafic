/*
Titre : Collecte de données du microcontrôleur Arduino vers Postgresql
Auteur: Adonis Rebano
Date : 25/05/2023
Description : Le but de ce projet est de collecter des données en utilisant les composants suivants 
              afin de les enregistrer dans la base de données et d'utiliser les données collectées à 
              des fins statistiques.
Version : 0.0.1
*/

/*-----on faire notre declaration pour la librairie dont on besoin dans notre projet-----*/
#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>

/*-----on definit notre SSID et notre mot de passe------*/
// char ssid[] = "BELL209";            
// char pass[] = "65E14F2C5217";       

char ssid[] = "UNIFI_IDO1";      
char pass[] = "42Bidules!";      

int status = WL_IDLE_STATUS;        

WiFiServer server(80);             
WiFiClient client = server.available();

/*-----on definit le delai different qu'on utilise dans notre projet-----*/
const int echoPin = 1;              
const int trigPin = 2;              
const int soundPin = A0;

long duration, cm, inches;

int bikeCounter = 0;
int vehicleCounter = 0;

/*-----une fonction qui affiche l'état actuel du WiFi et les informations associées-----*/
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

/*-----une fonction pour vérifier la version du firmware-----*/
void enable_WiFi() {
  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

/*-----une fonction qui tente de se connecter au réseau Wifi-----*/
void connect_WiFi() {
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
}

/*-----on crée la base pour notre HTML pour afficher le compte actuel sur le web-----*/
void printWEB() {
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.print("<!DOCTYPE html><html><head><title>Compteur de Traffic</title>");
            client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.print("<link rel=\"icon\" href=\"data:,\"><style>body {text-align: center;font-family: \"Trebuchet MS\", Arial;}");
            client.print("table {border-collapse: collapse;width: 35%;margin-left: auto;margin-right: auto;}");
            client.print("th {padding: 12px;background-color: #c74a4a;color: white;}");
            client.print("tr {border: 1px solid #ddd;padding: 12px;}");
            client.print("tr:hover {background-color: #bcbcbc;}");
            client.print("td {border: none;padding: 12px;}");
            client.print("</style>");
            client.print("<meta http-equiv=\"refresh\" content=\"3\">");                            // actualisation automatique de la page web toutes les 3 secondes
            client.print("</head><body>");
            client.print("<h1>Compteur de Traffic</h1>");
            client.print("<table><tr>");
            client.print("<th colspan=\"4\" style=\"text-align:center\">COMPTE ACTUEL</th>");
            client.print("</tr><tr>");
            client.print("<td>Voiture</td><td>" + String(vehicleCounter) + "</td>");
            client.print("<td>Velo</td><td>" + String(bikeCounter) + "</td></tr>");
            client.print("</table></body></html>");

            String request = client.readStringUntil('\r');
            client.flush();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}

void setup() {
  
  Serial.begin(9600);               //  on fait notre initialisation pour le moniteur série

  /*-----on definit notre broche pour l'entree et sortie-----*/
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soundPin, INPUT);

  while (!Serial);

/*-----on appelle notre fonction pour connecter sur le reseaux et afficher l'etat actuel*/
  enable_WiFi();
  connect_WiFi();
  server.begin();
  printWifiStatus();
}

void loop() {
  /*-----Par défaut, le capteur est déclenché par une impulsion haute, et pour garantir l'obtention d'une impulsion haute propre, on fournit une valeur basse.-----*/
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  /*-----La fonction pulseIn() attend le début de l'impulsion avant de commencer à mesurer sa durée.-----*/
  duration = pulseIn(echoPin, HIGH);

  /*-----on convertit le temps en une distance en utilisant les calculs suivants.-----*/
  cm = (duration / 2) / 29.1;     
  inches = (duration / 2) / 74;

  /*-----on definit notre variable pour notre capteur de son-----*/
  int val;
  val = analogRead(soundPin); 
  // Serial.println(val);         //on a commenté cette ligne parce qu'on l'utilise seulement pour tester et observer les valeurs sur le moniteur série
  
  // if (val > 150 && cm < 200){              // la condition qu'on a utilisée dans le test réel concerne le passage d'une voiture dans la rue
    if (val > 65 && cm > 10){
    Serial.print(val);
    vehicleCounter++;
    Serial.print(",");
    Serial.println(cm);
  } 
  // else if (val > 50 && val < 150 && cm < 70) {          // la condition qu'on a utilisée dans le test réel concerne le passage d'un vélo dans la rue
    else if (val > 30 && val < 50 && cm < 10) {
    Serial.print(val);
    bikeCounter++;
    Serial.print(",");
    Serial.println(cm);
  } else {
    
  }
  
  delay(500);

  client = server.available();

  if (client) {
    printWEB();
  }
}
