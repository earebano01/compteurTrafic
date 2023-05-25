#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "BELL209";            
char pass[] = "65E14F2C5217";       // your network password between the " "

// char ssid[] = "UNIFI_IDO1";      //  your network SSID (name) between the " "
// char pass[] = "42Bidules!";      // your network password between the " "

int status = WL_IDLE_STATUS;        //connection status

WiFiServer server(80);              //server socket
WiFiClient client = server.available();

const int echoPin = 1;              // Echo
const int trigPin = 2;              // Trigger
const int soundPin = A0;

long duration, cm, inches;

int bikeCounter = 0;
int vehicleCounter = 0;

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

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
            client.print("<meta http-equiv=\"refresh\" content=\"3\">"); // Auto-refresh every 3 seconds
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
  // Serial Port begin
  Serial.begin(9600);

  // Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soundPin, INPUT);

  while (!Serial);

  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();
}

void loop() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off an object.
  // pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // Convert the time into a distance
  cm = (duration / 2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration / 2) / 74;   // Divide by 74 or multiply by 0.0135

  int val;
  val = analogRead(soundPin); 
  Serial.println(val);
  
  // if (val > 150){
    if (val > 65){
    Serial.print(val);
    vehicleCounter++;
    Serial.print(",");
    Serial.println(cm);
  } 
  // else if (val > 50 && val < 150) {
    else if (val > 30 && val < 50) {
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
