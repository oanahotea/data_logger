
#include <WiFi.h>
#include <PubSubClient.h>
#include "EmonLib.h"        

#define VOLT_CAL 195
EnergyMonitor emon1;             


const char* ssid = "";
const char* password = "";

// MQTT Broker IP address
const char* mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];


void setup() {

  Serial.begin(9600);

 emon1.voltage(36, VOLT_CAL, 1.7);  
  
  setup_wifi();
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageVolt;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageVolt += (char)message[i];
  }
  Serial.println();
}


void reconnect(){
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    

  emon1.calcVI(20,2000);         // Calculate all. No.of half wavelengths (crossings), time-out
  
  float supplyVoltage   = emon1.Vrms; 
 
    
    // Valoarea este convertită intr-un șir de caractere
    char tempString[8];
    dtostrf(supplyVoltage, 1, 2, tempString);
    Serial.print("Voltage: ");
    Serial.println(tempString);
    client.publish("sensorDB", tempString);
  }
}
