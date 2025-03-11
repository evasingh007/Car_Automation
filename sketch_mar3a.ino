#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>  // Include the ESP32Servo library

// WiFi credentials
const char* ssid = "Eva";  // Replace with your WiFi SSID
const char* password = "1234567891";  // Replace with your WiFi password

// MQTT Broker details
const char* mqtt_server = "a5bc5fc65f7f4fc1b391cff98bfdcff2.s1.eu.hivemq.cloud";  // HiveMQ Broker
const int mqtt_port = 8883;  // Secure MQTT port
const char* mqtt_user = "hivemq.webclient.1741594079161";  // Replace with your MQTT username
const char* mqtt_password = "2#cZgCmXY>kJ7rd10&@E";  // Replace with your MQTT password

// Pins for sensors and actuators
const int tempPin = 32;    // Simulated temperature sensor pin
const int presPin = 33;    // Simulated pressure sensor pin
const int lightPin = 14;   // Simulated light sensor pin
Servo myServo;             // Create servo object
const int servoPin = 13;   // Servo motor pin

WiFiClient espClient;        // Use WiFiClient for unencrypted MQTT
PubSubClient client(espClient);  // PubSubClient for MQTT

// Connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

// Reconnect to MQTT broker if connection is lost
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT broker!");
      client.subscribe("esp/cmd");  // Subscribe to MQTT topic for commands
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

  // Parse incoming message
  int commaIndex = message.indexOf(",");
  String device = message.substring(0, commaIndex);
  int state = message.substring(commaIndex + 1).toInt();

  if (device == "light") {
    digitalWrite(lightPin, state);  // Toggle light
    Serial.println("Light state: " + String(state));
  } else if (device == "servo") {
    myServo.write(state);  // Rotate servo
    Serial.println("Servo angle set to: " + String(state));
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  Serial.println("Setup started!");

  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Setup pins
  myServo.attach(servoPin);
  pinMode(lightPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Simulate sensor data for testing
  float temperature = random(20, 30) + random(0, 100) / 100.0;
  float pressure = random(900, 1100) + random(0, 100) / 100.0;
  float light = random(10, 100) + random(0, 100) / 100.0;

  // Publish sensor data
  String payload = String(temperature) + "," +
                   String(pressure) + "," +
                   String(light);
  client.publish("home/sensors/data", payload.c_str());
  Serial.println("Published sensor data: " + payload);

  delay(5000);  // Delay for 5 seconds before next publish
}
