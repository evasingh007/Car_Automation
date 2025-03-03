# Smart Sensor Tracking System
## Description
This project is an IoT system using an ESP32 microcontroller to monitor and control various sensors and actuators via MQTT. It includes functionalities for temperature, pressure, air quality, and light measurement, as well as servo motor control.

## Table of Contents
- [Usage](#usage)
- [Features](#features)
- [Contributing](#contributing)


Arduino libraries:
- WiFi
- PubSubClient
- HTTPClient
- ESP32Servo

## Usage
After setting up the hardware and uploading the code, connect to the specified WiFi network. The ESP32 will connect to the MQTT broker and start publishing sensor data. You can subscribe to the "home/sensors/data" topic to receive sensor readings. 

## Features
- WiFi connectivity
- MQTT communication
- Temperature, pressure, air quality, and light measurement
- Servo motor control
- Data publishing to a topic

## Contributing
Contributions are welcome! Please open an issue or submit a pull request.

