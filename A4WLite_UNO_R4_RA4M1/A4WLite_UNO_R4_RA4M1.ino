#include "thingProperties.h"
#include <DHT.h>
#include <DHT_U.h>
#include <splash.h>
#include <RTC.h>
#include <EEPROM.h> // Persistant value storage 1Kb max
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h> // For SCL & SDA communication
#include <Adafruit_GFX.h> // For OLED Monitor
#include <Adafruit_SSD1306.h> // For OLED Monitor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include <ArduinoGraphics.h> // For LED Matrix
#include <Arduino_LED_Matrix.h> // For LED Matrix
#define DHTPIN 7 // Digital pin connected to the indoor DHT sensor
#define DHTOUTPIN 8 // Digital pin connected to the outdoor DHT sensor
#define DHTTYPE DHT11 // The type of DHT sensor
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declares the OLED display
const char* ssid = "SSID";
const char* ssid2 = "SSID2";
const char* password = "PASS1";
const char* password2 = "PASS2";
WiFiUDP ntpUDP; // A UDP instance to let the NTPClient communicate over
NTPClient timeClient(ntpUDP); // Declare NTPClient object
unsigned long unixTime; // Variable to hold unix time fetched from NTP server
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object for the indoor sensor
DHT dht2(DHTOUTPIN, DHTTYPE); // Create a DHT object for the outdoor sensor
Adafruit_BMP085 bmp;
ArduinoLEDMatrix matrix;

//const int placeholder = 0;
//const int placeholder = 1;
const int magneticSensor = 2;
const int redLED = 3; //PWM
const int piezo = 4;
const int yellowLED = 5; //PWM
const int greenLED = 6; //PWM
//const int placeholder = 7;
//const int placeholder = 8;
const int cyanLED = 9; //PWM
const int blueLED = 10; //PWM
const int whiteLED = 11; //PWM
//const int placeholder = 12;
//const int placeholder = 13;

const int TempSensor = A0;
const int microphoneSensor = A1;
//const int Placeholder = A2;
const int LightSensor = A3;
//const int Placeholder = A4;
//const int Placeholder = A5;

float offset = 0; // Offset For Temp Sensor (adjust as needed)
float gain = 1.00; // Gain For Temp Sensor (adjust as needed)
float Temp = 0.0;
float TempAvarage = 0.0; // Stores average temperature
float LightAvarage = 0.0; // Stores average light level
const float seaLevelPressure = 101325;
int interval = 300000; // Referesh interval for NTPUpdate function
int currentMillis = 0; // for NTPUpdate function
int previousMillis = 0; // for NTPUpdate function
int network = 0; // Initial network connection attempt for switch case
int OLEDCurrentMillis = 0; // For OLED Panel
int OLEDPreviousMillis = 0; // For OLED Panel
int OLEDInterval = 5000; // For OLED Panel
int OLEDPanel = 0; // Initial OLED Panel
const int sampleWindow = 50;  // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

// Initialisation of string variables used later
String formattedTime = "null";
String stringTime = "null";
String formattedF = "null";
String formattedOutF  ="null";
String dateOnly = "null";
String timeOnly = "null";
String formattedLightSensorData = "null";
String formattedMicrophoneSensor = "null";
String secondsOnline = "null";
String hoursOnline = "null";
String daysOnline= "null";
String localIP = "null";
String subnetMask = "null";
String gatewayIP = "null";
String signalStrength = "null";
String formattedMagnetSensor = "null";
String altitude = "null";
String NTPIP = "null";
// ⚡☔☁️🌨️🌧️🌩️⛈️🌦️🌥️⛅🌤️🌡️🔥❄️🌫️🌙☀️
/*
#333333
#434B4F
#53636A
#7293A1
#84A8B2
#99C0CC
#ADD8E6
#D6ECF3
#EBF6F9
#FFFFFF
*/

// 'Ardu4WeatherLogoBitmap', 128x64px
const unsigned char epd_bitmap_Ardu4WeatherLogoBitmap [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x7c, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x3c, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x3c, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x3c, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xc0, 0x07, 0x80, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x3f, 0x80, 0x1f, 0xe0, 0x1f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x80, 0x7e, 0x00, 0x07, 0xf0, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xe0, 0xf8, 0x00, 0x01, 0xf8, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf8, 0xf0, 0x00, 0x00, 0xf8, 0x3f, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x60, 0x00, 0x00, 0x7c, 0x18, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xc0, 0x38, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xc1, 0xff, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xc7, 0xff, 0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xef, 0xff, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0x8f, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x0f, 0x0f, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0x0f, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0x0f, 0xf0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xc3, 0xff, 0xfe, 0x1f, 0xff, 0x06, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x7f, 0xf0, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x00, 0x1f, 0xe0, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x00, 0x0f, 0x80, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xf8, 0x3e, 0x07, 0x07, 0xe0, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 0x82, 0x0f, 0xf0, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 0xc0, 0x1f, 0xf8, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0xff, 0xe0, 0x3f, 0xf8, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xf1, 0xff, 0xf0, 0x7f, 0xf8, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0xff, 0xf0, 0x7f, 0xf8, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0xff, 0xe0, 0x3f, 0xf8, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0xff, 0xc0, 0x1f, 0xf8, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7f, 0xff, 0xf8, 0x7f, 0x06, 0x07, 0xf0, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7f, 0xff, 0xf8, 0x1c, 0x0f, 0x01, 0xc0, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x1f, 0xc0, 0x01, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xfe, 0x00, 0x7f, 0xe0, 0x03, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0x81, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'CompositeLogo', 128x64px
const unsigned char epd_bitmap_CompositeLogo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf0, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x01, 0xf0, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xf0, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xf0, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x01, 0xf0, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x0e, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x01, 0xff, 0x00, 0xff, 0x80, 0x7e, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0x03, 0xf8, 0x00, 0x3f, 0xc0, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x87, 0xf0, 0x00, 0x0f, 0xe0, 0xff, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x1f, 0xe3, 0xc0, 0x00, 0x07, 0xe0, 0xfc, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x01, 0xf3, 0x80, 0x00, 0x03, 0xf0, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x79, 0x80, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x07, 0x03, 0xf0, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07, 0x1f, 0xfc, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x03, 0xbf, 0xff, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0xf8, 0x0f, 0x80, 0x3e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0xe0, 0x03, 0xc0, 0x3e, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0xc0, 0x01, 0xc0, 0x3e, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x01, 0xc0, 0x00, 0xe0, 0x3e, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00, 0x60, 0x3c, 0x3f, 0xc0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00, 0x70, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00, 0x70, 0x7c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 0x00, 0x70, 0x7c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x03, 0x80, 0x00, 0x7e, 0x18, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0xff, 0x80, 0x00, 0x03, 0x80, 0x01, 0xff, 0x88, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0xe0, 0x00, 0x03, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0x39, 0xe0, 0x00, 0x06, 0x00, 0x07, 0xf1, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3c, 0x3c, 0x60, 0x00, 0x04, 0x00, 0x02, 0xe0, 0x78, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0x08, 0x00, 0x01, 0xe0, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x00, 0x60, 0x21, 0xc0, 0x1e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xf0, 0x07, 0x82, 0x00, 0x00, 0x78, 0x61, 0x80, 0x0e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe0, 0x07, 0x82, 0x00, 0xc0, 0x3f, 0xe1, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xe0, 0x01, 0x82, 0x01, 0xc0, 0x1f, 0xe0, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xc0, 0x00, 0xc3, 0x3f, 0x80, 0x03, 0xe0, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xc0, 0x00, 0x03, 0xfe, 0x00, 0x00, 0xc0, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xc0, 0x00, 0x01, 0xfc, 0x00, 0x00, 0xc0, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xe0, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xf0, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x70, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x78, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x38, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3c, 0x00, 0x78, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3e, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x1f, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() {
  pinMode(magneticSensor, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(cyanLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial.println("*******************************************************");
  NetworkChange();
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  String localIP = WiFi.localIP().toString();
  Serial.println(localIP);
  dht.begin();
  dht2.begin();
  RTC.begin();
  matrix.begin();
  timeClient.begin();
  timeClient.update();
  //NTPIP = String(timeClient.getNTPserverIP());
  NTPIP = String("null");
  unixTime = timeClient.getEpochTime();
  while (unixTime < 1000) {
    timeClient.update();
    unixTime = timeClient.getEpochTime();
    Serial.println("Failed to get proper unix time, refreshing.");
    delay(500);
  }
  Serial.print("Unix Time: ");
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
  RTCTime currentTime;
  RTC.getTime(currentTime);
  Serial.print("RTC Time: ");
  Serial.println(currentTime);
  display.cp437(true);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  }
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE); 
  display.drawBitmap(0, 0, epd_bitmap_CompositeLogo, 128, 64, WHITE);
  display.display();
  //RandomStaticLoad();
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); // Connect to arduino cloud
  setDebugMessageLevel(2); // Set debug verbosity 0-4
  ArduinoCloud.printDebugInfo(); // Print to cloud console
}

int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// NetworkChange, If the network disconnects, it reconnects to another predefined network
void NetworkChange() {
  while (WiFi.status() != WL_CONNECTED) {
    network = (network + 1) % 3;
    switch(network) {
      case 0:
        WiFi.begin(ssid, password);
        Serial.println("Changed to network 1.");
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xEA);
        delay(100);
        break;
      case 1:
        WiFi.begin(ssid2, password2);
        Serial.println("Changed to network 2.");
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xEF);
        delay(100);
        break;
    }
  }
}

// RandomStaticLoad, Loads a random predefined image onto the Arduino R4 WiFi led matrix
//const int StaticAnimationSelection = random(0,12);
const int StaticAnimationSelection = 3;
void RandomStaticLoad() {
  switch (StaticAnimationSelection) {
    case 0:
      matrix.loadFrame(LEDMATRIX_UNO);
      break;
    case 1:
      matrix.loadFrame(LEDMATRIX_BLUETOOTH);
      break;
    case 2:
      matrix.loadFrame(LEDMATRIX_BOOTLOADER_ON);
      break;
    case 3:
      matrix.loadFrame(LEDMATRIX_CLOUD_WIFI);
      break;
    case 4:
      matrix.loadFrame(LEDMATRIX_DANGER);
      break;
    case 5:
      matrix.loadFrame(LEDMATRIX_EMOJI_BASIC);
      break;
    case 6:
      matrix.loadFrame(LEDMATRIX_EMOJI_HAPPY);
      break;
    case 7:
      matrix.loadFrame(LEDMATRIX_EMOJI_SAD);
      break;
    case 8:
      matrix.loadFrame(LEDMATRIX_HEART_BIG);
      break;
    case 9:
      matrix.loadFrame(LEDMATRIX_LIKE);
      break;
    case 10:
      matrix.loadFrame(LEDMATRIX_MUSIC_NOTE);
      break;
    case 11:
      matrix.loadFrame(LEDMATRIX_RESISTOR);
      break;
  }
}

// ReadTempC, Redundant, reds temp from an analog sensor
float ReadTempC() {
  int SensorValue = analogRead(TempSensor);
  float Voltage = SensorValue * (5.0 / 1023.0); // Assuming 5V reference
  float Temp = (Voltage - 0.5) * 100.0 * gain + offset;
  return Temp;
}

// calculateAverage, A modular function that calculates the avarage from a number of samples
template <typename T>
T calculateAverage(T value, unsigned long interval, unsigned long &lastUpdateTime, T &sum, int &count, int &samples) { 
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= interval) {
    sum += value;
    count++;
    if (count >= samples) { // Adjusts the reactiveness, samples recorded
      T average = sum / count;
      sum /= count; // Divide sum by count to keep a running average
      count = 0; 
      lastUpdateTime = currentTime; 
    }
  }
  return sum / (count > 0 ? count : 1); // Avoid division by zero
}

// LiveThermomiter, Lights up a different colour LED based on what temperature is fetched from the dht1 sensor
void LiveThermomiter() {
  float currentTemp = dht.readTemperature();
  float currentLight = analogRead(LightSensor);
  unsigned long averagingInterval = 10000; 
  static unsigned long lastTempUpdate = 0; 
  static float sumTemp = 0; 
  static int tempCount = 0;
  int samples = 250;
  float averageLight = calculateAverage(currentLight, averagingInterval, lastTempUpdate, sumTemp, tempCount, samples);
  int PWMLightness = map(averageLight, 50, 500, 10, 150);
  //Serial.print("Avarage Value: ");
  //Serial.println(averageLight);
  //Serial.print("Current Value: ");
  //Serial.println(analogRead(LightSensor));
  if (currentTemp >= 25) {
    analogWrite(redLED, PWMLightness);
    analogWrite(yellowLED, LOW);
    analogWrite(greenLED, LOW);
    analogWrite(cyanLED, LOW);
    analogWrite(blueLED, LOW);
  } else if (currentTemp >= 22 && currentTemp < 25) {
    analogWrite(redLED, LOW);
    analogWrite(yellowLED, PWMLightness);
    analogWrite(greenLED, LOW);
    analogWrite(cyanLED, LOW);
    analogWrite(blueLED, LOW);
  } else if (currentTemp >= 18 && currentTemp < 22) {
    analogWrite(redLED, LOW);
    analogWrite(yellowLED, LOW);
    analogWrite(greenLED, PWMLightness);
    analogWrite(cyanLED, LOW);
    analogWrite(blueLED, LOW);
  } else if (currentTemp >= 10 && currentTemp < 18) {
    analogWrite(redLED, LOW);
    analogWrite(yellowLED, LOW);
    analogWrite(greenLED, LOW);
    analogWrite(cyanLED, PWMLightness);
    analogWrite(blueLED, LOW);
  } else if (currentTemp <= 10) {
    analogWrite(redLED, LOW);
    analogWrite(yellowLED, LOW);
    analogWrite(greenLED, LOW);
    analogWrite(cyanLED, LOW);
    analogWrite(blueLED, PWMLightness);
  }
} 

// NTPSync, Syncs the RTC module with the time fetched from the NTP server every 5 mins, helps account for RTC module drift.
void NTPSync() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (timeClient.update()) {
      unixTime = timeClient.getEpochTime();
      Serial.print("Unix Time: ");
      Serial.println(unixTime);
      Serial.println("NTP Time Synced");
      RTCTime timeToSet = RTCTime(unixTime);
      RTC.setTime(timeToSet);
    } else {
      Serial.println("NTP Update Failed");
    }
  }
}

// OLEDHeader, inputs the header of the oled to buffer for displaying, shows some symbols and the current ntp date and time
void OLEDHeader(String dateOnly, String timeOnly) {
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.write(0xB2);
  display.write(0x7E);
  display.setTextSize(1);
  display.setCursor(68, 0); 
  display.println(dateOnly);
  display.setCursor(80, 8); 
  display.println(timeOnly);
}

// OLEDPanel1, Shows a few different readings from the sensors without having to access the website, useful for debugging.
void OLEDPanel1(String formattedLightSensorData, String formattedHumdiditySensor, String formattedPressureSensor, String formattedMicrophoneSensor, String secondsOnline) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("1/3");
  display.setCursor(0, 16); 
  display.print(String("Temp: ") + String(dht.readTemperature()));
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  display.println(formattedLightSensorData);
  display.setCursor(0, 32); 
  display.println(formattedHumdiditySensor);
  display.setCursor(0, 40); 
  display.println(formattedPressureSensor);
  display.setCursor(0, 48); 
  display.println(formattedMicrophoneSensor);
  display.setCursor(0, 56); 
  display.println(secondsOnline);
}

// OLEDPanel2, Shows a few different more readings that couldnt fit on 1 from the sensors without having to access the website, useful for debugging.
void OLEDPanel2(String formattedOutHumdiditySensor, String formattedMagnetSensor, String altitude, String hoursOnline, String daysOnline) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("2/3");
  display.setCursor(0, 16); 
  display.print(String("OTemp: ") + String(dht2.readTemperature()));
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  display.println(formattedOutHumdiditySensor);
  display.setCursor(0, 32); 
  display.println(formattedMagnetSensor);
  display.setCursor(0, 40); 
  display.println(altitude);
  display.setCursor(0, 48); 
  display.println(hoursOnline);
  display.setCursor(0, 56); 
  display.println(daysOnline);
}

// OLEDPanel3, Shows a few different network statistics, useful for debugging
void OLEDPanel3(String localIP, String subnetMask, String gatewayIP, String NTPIP, String signalStrength, int previousMillis) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("3/3");
  display.setCursor(0, 16);
  display.println("IP: " + localIP);
  display.setCursor(0, 24);
  display.println("SM: " + subnetMask);
  display.setCursor(0, 32);
  display.println("GIP: " + gatewayIP);
  display.setCursor(0, 40);
  display.println("NTPIP: " + NTPIP);
  display.setCursor(0, 48);
  display.println("Strength: " + signalStrength);
  display.setCursor(0, 56);
  display.println("NTP UP: " + previousMillis);
}

int MicLevels() {
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;
  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(microphoneSensor);
    if (sample < 1024) {  // toss out spurious readings
      if (sample > signalMax) {
        signalMax = sample;  // save just the max levels
      } else if (sample < signalMin) {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  return peakToPeak;
}

void loop() {
  // Put continuous updates here
  LiveThermomiter();
  NetworkChange();
  NTPSync();
  ArduinoCloud.update();

  RTCTime currentTime;
  RTC.getTime(currentTime);
  stringTime = String(currentTime);
  int tPosition = stringTime.indexOf('T');
  timeOnly = stringTime.substring(tPosition + 1);
  dateOnly = stringTime.substring(0, tPosition);
  formattedTime = String(dateOnly + " / " + timeOnly);
  formattedC = String("Temp: ") + String(dht.readTemperature()) + String("°C");
  formattedF = String("Temp: ") + String(dht.readTemperature()) + String("°F");
  formattedOutC = String("OTemp: ") + String(dht2.readTemperature()) + String("°C");
  formattedOutF = String((dht2.readTemperature() * 9.0 / 5.0) + 32.0);
  formattedLightSensorData = String("Bright: ") + String(analogRead(LightSensor) * 2) + String(" Lm");
  formattedHumdiditySensor = String("Humid: ") + String(dht.readHumidity()) + String("%");
  formattedOutHumdiditySensor = String("OHumid: ") + String(dht2.readHumidity()) + String("%");
  float pressure = (bmp.readPressure());
  formattedPressureSensor = String("Pressure: ") + String(pressure / 100.0) + String(" mBar");
  formattedMicrophoneSensor = String("Noise: ") + String(MicLevels()) + String(" U");
  formattedMagnetSensor = String("Magnet: ") + String(digitalRead(magneticSensor) == 1 ? 0 : 1);
  altitude = String("Altitude: ") + String(bmp.readAltitude(seaLevelPressure)) + String("m");
  secondsOnline = String("Time: ") + String(millis() / 1000) + String(" Secs");
  hoursOnline = String("Hours: ") + String(float(millis() / 3600000.0)) + String("H");
  daysOnline = String("Days: ") + String(float(millis() / 86400000.0)) + String("D");
  localIP = WiFi.localIP().toString();
  subnetMask = WiFi.subnetMask().toString();
  gatewayIP = WiFi.gatewayIP().toString();
  signalStrength = String(WiFi.RSSI());

  // OLED Data Display Function
  OLEDCurrentMillis = millis();
  if (OLEDCurrentMillis - OLEDPreviousMillis >= OLEDInterval) {
    OLEDPreviousMillis = OLEDCurrentMillis;
    OLEDPanel = (OLEDPanel + 1) % 3;
  }
  display.clearDisplay();
  OLEDHeader(dateOnly, timeOnly);
  switch (OLEDPanel) {
    case 0:
      OLEDPanel1(formattedLightSensorData, formattedHumdiditySensor, formattedPressureSensor, formattedMicrophoneSensor, secondsOnline);
      break;
    case 1:
      OLEDPanel2(formattedOutHumdiditySensor, formattedMagnetSensor, altitude, hoursOnline, daysOnline);
      break;
    case 2:
      OLEDPanel3(localIP, subnetMask, gatewayIP, NTPIP, signalStrength, previousMillis);
      break;
  }
  display.display(); // Display everything held in buffer
}