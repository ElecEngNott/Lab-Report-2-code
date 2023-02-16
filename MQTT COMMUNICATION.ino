//****************************************//
//* ESP32 MQTT EEEBot Template           *//
//* Modified from Rui Santos'            *//
//* https://randomnerdtutorials.com      *//
//*                                      *//
//* ESP32 Code                           *//
//*                                      *//
//* UoN 2022 - ND                        *//
//****************************************//

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <MPU6050_tockn.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
const int trigPin = 5;
const int echoPin = 18;
long duration;
float distanceCm;
float distanceInch;

MPU6050 mpu6050(Wire);
Adafruit_MPU6050 mpu;



// Replace the next variables with your SSID/Password combination
const char* ssid = "Bench222";                      
const char* password = "Bench222";                

// Add your MQTT Broker IP address, example:
//const char* mqtt_server = "192.168.1.144";                      
const char* mqtt_server = "192.168.2.1";          //IP address for Raspberry Pi

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  delay(100);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
     
      client.subscribe("esp32/output");
      client.subscribe("eps32/distance");
         
    }
    else {
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
  if (now - lastMsg > 1000) {
    lastMsg = now;
 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
    
    
  char String[8];
  dtostrf(distanceCm, 1, 2, String);
  client.publish("esp32/distance", String);

float x = mpu6050.getTemp();
  char Temperature[8];
  dtostrf(x, 1, 2, Temperature);
  client.publish("esp32/temperature", Temperature);

float z = mpu6050.getGyroAngleZ();
  char Zangle[8];
  dtostrf(z, 1, 2, Zangle);
  client.publish("esp32/Zangle", Zangle);
  }
}
