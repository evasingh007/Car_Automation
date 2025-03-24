#include <WiFi.h>
#include <WiFiClientSecure.h>  // Secure WiFi client for MQTT over TLS
#include <PubSubClient.h>
#include <ESP32Servo.h>  
#include <ESP32PWM.h>

// WiFi credentials
const char* ssid = "Eva";  // Replace with your WiFi SSID
const char* password = "1234567891";  // Replace with your WiFi password

// MQTT Broker details
const char* mqtt_server = "a5bc5fc65f7f4fc1b391cff98bfdcff2.s1.eu.hivemq.cloud";  
const int mqtt_port = 8883;  // Secure MQTT port
const char* mqtt_user = "hivemq.webclient.1741594079161";  
const char* mqtt_password = "2#cZgCmXY>kJ7rd10&@E";  

// Pins for sensors and actuators
const int tempPin = 32;    
const int presPin = 33;    
const int lightPin = 14;   
const int relayPin = 12;   
const int steeringPin = 27;
const int doorPin = 26;    
Servo myServo;             
const int servoPin = 13;   

WiFiClientSecure espClient;  // Secure client for MQTT over SSL/TLS
PubSubClient client(espClient);

// Connect to WiFi with timeout
void setup_wifi() {
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {  // Timeout after 10 seconds
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
  } else {
    Serial.println("\nFailed to connect to WiFi!");
  }
}

// Reconnect to MQTT broker if connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe("esp/cmd");  
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println("; retrying in 5 seconds...");
      delay(5000);
    }
  }
}

// Handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message received: " + message);

  int commaIndex = message.indexOf(",");
  if (commaIndex == -1) {
    Serial.println("Invalid message format!");
    return;
  }

  String device = message.substring(0, commaIndex);
  int state = message.substring(commaIndex + 1).toInt();

  if (device == "light") {
    digitalWrite(lightPin, state);  
    Serial.println("Light state: " + String(state));
  } else if (device == "servo") {
    myServo.write(state);  
    Serial.println("Servo angle set to: " + String(state));
  } else if (device == "relay") {
    digitalWrite(relayPin, state);  
    Serial.println("Relay state: " + String(state));
  } else if (device == "steering") {
    digitalWrite(steeringPin, state);  
    Serial.println("Steering wheel stop state: " + String(state));
  } else if (device == "door") {
    digitalWrite(doorPin, state);  
    Serial.println("Car door state: " + String(state));
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  Serial.println("Setup started!");

  // Secure MQTT Connection
  espClient.setInsecure();  // Disable SSL certificate validation (use only if broker allows it)
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Setup pins
  ESP32PWM::allocateTimer(0);  // Fix servo issue
  myServo.attach(servoPin);
  
  pinMode(lightPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(steeringPin, OUTPUT);
  pinMode(doorPin, OUTPUT);

  randomSeed(analogRead(0));  // Ensure truly random sensor values
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Simulate sensor data
  float temperature = random(20, 30) + random(0, 100) / 100.0;
  float pressure = random(900, 1100) + random(0, 100) / 100.0;
  float light = random(10, 100) + random(0, 100) / 100.0;

  // Publish sensor data
  String payload = String(temperature) + "," +
                   String(pressure) + "," +
                   String(light);
  client.publish("home/sensors/data", payload.c_str());
  Serial.println("Published sensor data: " + payload);

  delay(5000);
}

