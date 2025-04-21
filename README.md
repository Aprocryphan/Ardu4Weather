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
* Sends data when requested OTA to save on computer
* Works with Arduino Cloud IoT & Google Home

Components Used:
* Arduino UNO R4 WiFi
* DHT11 sensors for temperature and humidity
* BMP180 sensor for pressure and altitude
* MAX4466 Electret Microphone Amplifier for noise levels
* OLED Module 12864 (128x64, Yellow & Blue)
* Photoresistor for light level detection
* Various LEDs for visual indicators

## Upcoming Updates
Support for the Arduino Nano ESP32 and the Uno R3 are coming

## Versions
1. Base Verion (Actively being developed)\
This is the main version of this project, it serves you the webpage and takes in weather data.
2. A4WLite\
This is a lighter verison, it doesn't have website capabilities but it syncs with the arduino cloud and takes in weather data.
3. A4WLiteGHome\
This version is almost an exact copy of A4WLite but it uses google home compatable variables instead.
4. IoT+Website\
This version attempts to combine website functionality with the arduino cloud, this creates some hanging problems so i won't be maintaining this as much as the main branch.

## Installation
**Disclaimer: The vast majority of features used in this project require either the Arduino UNO R4 WiFi or the Arduino UNO R4 Minima**
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
4. Upload the code to your Arduino UNO R4 WiFi.

## Usage
1. Connect the sensors and components as per the pin configuration in the code.
2. Power on the Arduino.
3. The device will connect to the predefined WiFi networks.
4. Sensor data will be displayed on the OLED screen.
5. Access the web server hosted on the Arduino to view the data in a web browser.

## Acknowledgements
Some of the code that I use was taken from online tutorials and libraries, I would like to thank the authors of all of the libraries I use and these tutorials
1. [MAX4466 Tutorial](https://lastminuteengineers.com/max4466-arduino-tutorial/)
2. [R4 Cheat Sheet](https://docs.arduino.cc/tutorials/uno-r4-wifi/cheat-sheet/)
3. [Image To Arduino Code](https://javl.github.io/image2cpp/)
4. [Google Font Used](https://fonts.google.com/shareselection.family=Funnel+Display:wght@300..800)

## Links
Here are some links to certain components I use\
[Arduino UNO R4 WiFi](https://store.arduino.cc/collections/boards-modules/products/uno-r4-wifi)

## Contributing
Contributions are welcome! Feel free to open issues or submit pull requests.

## License
This project is licensed under a modified version of the MIT License.\
Please read the licence file for more details.\
https://opensource.org/license/mit

Icons Used For Website Lisence
https://www.apache.org/licenses/LICENSE-2.0.html
