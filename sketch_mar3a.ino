#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>  // Include the ESP32Servo library

// Setup credentials for WiFi and MQTT
const char* ssid = "Eva";
const char* password = "1234567891";
const char* mqtt_server = "5.196.78.28";

// Define pins for the sensors
const int tempPin = 32;
const int presPin = 33;
const int airPin = 12;
const int LightPin = 14;

Servo myServo;  // Create a servo object
int servoPin = 13;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ......");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe to a topic from the broker
      client.subscribe("esp/cmd");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);  // try to connect after 5 seconds
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // handle the message
  if (String(topic) == "esp/cmd") {
    Serial.println("Received message: " + message);
    // parse string
    parseStr(message);
  }
}

void parseStr(const String& str) {
  // find the commaIndex
  int commaIndex = str.indexOf(",");

  // Extract the sensor Name and sensor state
  String sensorName = str.substring(0, commaIndex);
  int pinState = str.substring(commaIndex + 1).toInt();
  Serial.println("sensorName: " + sensorName + ", pinState: " + pinState);

  // Execute the command
  if (sensorName == "temperature") {
    digitalWrite(tempPin, pinState);
  } else if (sensorName == "pressure") {
    digitalWrite(presPin, pinState);
  } else if (sensorName == "air") {
    run_servo(pinState);
  } else if (sensorName == "light") {
    digitalWrite(LightPin, pinState);
  } else {
    // handle unknown sensor names
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  myServo.attach(servoPin);  // Attach Servo

  // setup the pins
  pinMode(tempPin, OUTPUT);
  pinMode(presPin, OUTPUT);
  pinMode(airPin, OUTPUT);
  pinMode(LightPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Simulate sensor
  float temperature = random(0, 100) + random(0, 100) / 100.0;
  float pressure = random(0, 1000) + random(0, 100) / 100.0;
  float airQuality = random(0, 500) + random(0, 100) / 100.0;
  float light = random(0, 100) + random(0, 100) / 100.0;

  // Combine values into a String
  String payload = String(temperature) + "," +
                   String(pressure) + "," +
                   String(airQuality) + "," +
                   String(light);

  // Publish payload
  client.publish("home/sensors/data", payload.c_str());
  Serial.print("Sensors data sent:");
  Serial.println(payload);

  delay(5000);  // Publish after every 5 seconds
}

void run_servo(int value) {
  if (value == 0) {
    myServo.write(0);
  } else {
    myServo.write(value);  // Complete the function call
  }
}
