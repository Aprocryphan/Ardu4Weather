# Ardu4Weather
## Overview

Ardu4Weather is an Arduino-based weather station project that collects and displays environmental data such as temperature, humidity, pressure, and light levels. It connects to WiFi, updates the time using NTP, and hosts a web server to display the collected data.

It is constantly evolving and changing as I develop it and right now it isn't fit to be used generally and is more of a personal project but still feel free to create pull requests and use the code for yourself!

![Screenshot of the Ardu4Weather Website](https://i.imgur.com/ARaHL09.png)
## Features:
* Connects to multiple WiFi networks
* Reads data from various sensors (temperature, humidity, pressure, light)
* Displays data on an OLED screen
* Hosts a web server to serve an HTML page with sensor data
* Synchronizes time with NTP servers
* LED Matrix for visual indicators

Components Used:
* Arduino Uno R4 WiFi
* DHT11 sensors for temperature and humidity
* BMP180 sensor for pressure and altitude
* OLED display (128x64)
* Photoresistor for light level detection
* Various LEDs for visual indicators

## Installation
1. Clone the repository:
```
  git clone https://github.com/Aprocryphan/Ardu4Weather.git
```
2. Open the Ardu4Weather.ino file in the Arduino IDE.

3. Ensure you have the necessary libraries installed:
* WiFi
* RTC
* EEPROM
* NTPClient
* Adafruit_GFX
* Adafruit_SSD1306
* DHT
* Adafruit_BMP085
* ArduinoGraphics
* Arduino_LED_Matrix
4. Upload the code to your Arduino Uno R4 WiFi.

## Usage
1. Connect the sensors and components as per the pin configuration in the code.
2. Power on the Arduino.
3. The device will connect to the predefined WiFi networks.
4. Sensor data will be displayed on the OLED screen.
5. Access the web server hosted on the Arduino to view the data in a web browser.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## License

This project is licensed under the MIT License.
