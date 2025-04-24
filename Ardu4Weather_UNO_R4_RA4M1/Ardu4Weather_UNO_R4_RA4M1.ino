#include "arduino_secrets.h"
#include "a4w_bitmaps.h"
#include "screensavers.h"
#include "websites.h"
#include <math.h> // For math functions like sin()
#include <stdlib.h> // For random number generation
#include <WiFiS3.h> // For WiFi connection
#include "RTC.h" // For Real Time Clock
#include <EEPROM.h> // Persistant value storage 1Kb max
#include <NTPClient.h> // For NTP (Network Time Protocol) time
#include <WiFiUdp.h> // For WiFi UDP (User Datagram Protocol) communication
#include <String.h> // For string type
#include <Wire.h> // For SCL & SDA communication
#include <Adafruit_GFX.h> // For OLED Monitor
#include <Adafruit_SSD1306.h> // For OLED Monitor
#include "DHT.h" // For DHT11 sensor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#include "ArduinoGraphics.h" // For LED Matrix
#include "Arduino_LED_Matrix.h" // For LED Matrix
#define DHTPIN 7 // Digital pin connected to the indoor DHT sensor
#define DHTOUTPIN 8 // Digital pin connected to the outdoor DHT sensor
#define DHTTYPE DHT11 // The type of DHT sensor
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declares the OLED display
WiFiServer WebServer(8080); // Defines the port that the web server is hosted on
WiFiClient WebClient; // Declare DataClient globally
WiFiServer DataServer(8081); // Defines the port that the data server is hosted on
WiFiClient DataClient; // Declare DataClient globally
WiFiUDP ntpUDP; // A UDP instance to let the NTPClient communicate over
NTPClient timeClient(ntpUDP); // Declare NTPClient object
unsigned long unixTime; // Variable to hold unix time fetched from NTP server
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object for the indoor sensor
DHT dht2(DHTOUTPIN, DHTTYPE); // Create a DHT object for the outdoor sensor
Adafruit_BMP085 bmp; // Create a BMP object for the BMP180 sensor
ArduinoLEDMatrix matrix; // Create a matrix object for the LED Matrix

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

// Initialisation of variables used later
const int sampleWindow = 50;  // Sample window width in mS (50 mS = 20Hz)
const int NUM_READINGS = 288; // 24 hours of data, one reading every 5 minute
unsigned int signalMax = 0;
unsigned int signalMin = 1024;
unsigned int sample; // Sample value from the pressure sensor
unsigned int peakToPeak = 0;   // peak-to-peak level
int screensaverActive = 0;
int network = -1; // Initial network connection attempt for switch case
int OLEDPanel = 0; // Initial OLED Panel
int readingIndex = 0; // Keep track of the current position in the array
int SensorPreviousMillis = 0;
int formattedLightSensorData = 0;
int formattedMicrophoneSensor = 0;
int signalStrength = 0;
int formattedMagnetSensor = 0;
int inTempDisplacement = 0;
int outTempDisplacement = 0;
int lightDisplacement = 0;
int inHumidityDisplacement = 0;
int outHumidityDisplacement = 0;
int pressureDisplacement = 0;
int altitudeDisplacement = 0;
int noiseDisplacement = 0;
int oldestIndex = 0;
int whiteLightness = 0;
int tPosition = 0;

const float offset = 0.0; // Offset For Temp Sensor (adjust as needed)
const float gain = 1.00; // Gain For Temp Sensor (adjust as needed)
const float seaLevelPressure = 101325.0;
float Temp = 0.0;
float TempAverage = 0.0; // Stores average temperature
float LightAverage = 0.0; // Stores average light level
float deltaP = 0.0;
float DP24 = 0.0;
float pressure = 0.0;
float formattedC = 0.0;
float formattedF = 0.0;
float formattedOutF = 0.0;
float formattedHumiditySensor = 0.0;
float formattedPressureSensor = 0.0;
float formattedOutC = 0.0;
float formattedOutHumiditySensor = 0.0;
float altitude = 0.0;
float pressureReadings[NUM_READINGS]; // Array to store pressure readings

unsigned long long previousMillis = 0; // for NTPUpdate function
unsigned long long OLEDPreviousMillis = 0; // For OLED Panel
unsigned long long DPPreviousMillis = 0;
unsigned long long secondsOnline = 0;
unsigned long long screensaverMillis = 0;
unsigned long startMillis = millis(); // Start of sample window

const char CRed[13] = "\033[0;31m";
const char CGreen[13] = "\033[0;32m";
const char COrange[13] = "\033[0;33m";
const char CBlue[13] = "\033[0;34m";
const char CPurple[13] = "\033[0;35m";
const char CCyan[13] = "\033[0;36m";
const char CWhite[13] = "\033[0;37m"; 
const char Cblack[13] = "\033[0;30m";
const char Cred[13] = "\033[0;91m";
const char Cgreen[13] = "\033[0;92m";
const char Cyellow[13] = "\033[0;93m";
const char Cblue[13] = "\033[0;94m";
const char Cmagenta[13] = "\033[0;95m";
const char Ccyan[13] = "\033[0;96m";
const char Cbright_black[13] = "\033[0;90m";
const char Cbright_red[13] = "\033[0;91m";
const char Cbright_green[13] = "\033[0;92m";
const char Cbright_yellow[13] = "\033[0;93m";
const char Cbright_blue[13] = "\033[0;94m";
const char Cbright_magenta[13] = "\033[0;95m";
const char Cbright_cyan[13] = "\033[0;96m";
const char Cbright_white[13] = "\033[0;97m";
const char Ccyan_back[13] = "\033[0;46m";
const char Cpurple_back[13] = "\033[0;45m";
const char Cwhite_back[13] = "\033[0;47m";
const char Cblue_back[13] = "\033[0;44m";
const char Corange_back[13] = "\033[0;43m";
const char Cgreen_back[13] = "\033[0;42m";
const char Cpink_back[13] = "\033[0;41m";
const char Cgrey_back[13] = "\033[0;40m";
const char Cgrey[13] = "\033[38;4;236m";
const char Cbold[13] = "\033[1m";
const char Cunderline[13] = "\033[4m";
const char Citalic[13] = "\033[3m";
const char Cdarken[13] = "\033[2m";
const char Cinvisible[13] = "\033[08m";
const char Creverse[13] = "\033[07m";
const char Creset[13] = "\033[0m";
const char Cgrey_old[13] = "\x1b[90m";
const char* weatherEmojis[] = { "⚡", "☔", "☁️", "🌨️", "🌧️", "🌩️", "⛈️", "🌦️", "🌥️", "⛅", "🌤️", "🌡️", "🔥", "❄️", "🌫️", "🌙", "☀️" };
char serialOutputBuffer [256]; // Buffer for serial output
char formattedTime[23] = "null"; // XX-XX-XX / XX:XX:XX\0
char dateOnly[11] = "null"; // XX-XX-XX\0
char timeOnly[9] = "null"; // XX:XX:XX\0
char hoursOnline[10] = "null"; // XX.XX\0
char daysOnline[10] = "null"; // XX.XX\0
char localIP[21] = "null"; // XXX.XXX.XXX.XXX\0
char subnetMask[21] = "null"; // XXX.XXX.XXX.XXX\0
char gatewayIP[21] = "null"; // XXX.XXX.XXX.XXX\0
char NTPIP[21] = "null"; // XXX.XXX.XXX.XXX\0
char data[128] = "null"; // <START>XX-XX-XX,XX:XX:XX,XX.X,XX.X,XXX,XX.X,XX.X,XX.X,XXX,XXXXX<END>\0
//char url[128] = ""; // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0
//char referrer[128] = ""; // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0
//char request[2048] = ""; // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\0

String stringTime = "null";
String url = "null";
String referrer = "null";
String request = "null";

/* Colour Palette
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
  Serial.println("\n*************** Ardu4Weather v0.26.0 - Commit 23 *******************");

  // Initialize the OLED display
  display.cp437(true);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // Address 0x3C for 128x64
    sprintf(serialOutputBuffer, "%sSSD1306 allocation failed%s", Cred, Creset);
    Serial.println(serialOutputBuffer);
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.drawBitmap(0, 0, epd_bitmap_CompositeLogo, 128, 64, WHITE); // Draw the logo while setup is running
  display.display();

  NetworkChange(); // Connect to WiFi
  if (WiFi.status() != WL_CONNECTED) { // If the WiFi connection fails
    sprintf(serialOutputBuffer, "%sFailed to connect to WiFi%s", Cred, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to connect to WiFi");
    display.display();
  }
  strncpy(localIP, WiFi.localIP().toString().c_str(), sizeof(localIP) - 1);
  localIP[sizeof(localIP) - 1] = '\0'; // Ensure null termination
  sprintf(serialOutputBuffer, "%sConnected to WiFi. IP address: %s%s", Cgreen, localIP, Creset);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0';
  WebServer.begin(); // Start website hosting server, port 8080
  DataServer.begin(); // Start data sending server, port 8081

  // initialize DHT sensors
  dht.begin();
  dht2.begin();
  if (isnan(dht.readTemperature()) || isnan(dht2.readTemperature())) { // If the DHT sensors fail to initialize
    sprintf(serialOutputBuffer, "%sFailed to initialize DHT sensors%s", Cred, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to initialize DHT sensors");
    display.display();
  }

  // initialize RTC
  RTC.begin();
  if (!RTC.begin()) {
    sprintf(serialOutputBuffer, "%sFailed to initialize RTC%s", Cred, Creset);
    Serial.println(serialOutputBuffer);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to initialize RTC");
    display.display();
    for (;;); // Don't proceed, loop forever
  }
  timeClient.begin();
  timeClient.update();
  unixTime = timeClient.getEpochTime();
  while (unixTime < 1000)
  { // If the unix time is less than 1000, it's not a valid time
    timeClient.update();
    unixTime = timeClient.getEpochTime();
    sprintf(serialOutputBuffer, "%sFailed to get proper unix time, refreshing.%s", Cred, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to get proper unix time, refreshing.");
    display.display();
    delay(200);
  }
  sprintf(serialOutputBuffer, "%sUnix Time: %lu%s", Cblue, unixTime, Creset);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0';
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
  RTCTime currentTime;
  RTC.getTime(currentTime);
  String currentTimeS = String(currentTime);
  sprintf(serialOutputBuffer, "%sRTC Time: %s%s", Cblue, currentTimeS, Creset);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0';

  // initialize LED Matrix
  matrix.begin();

  // initialize BMP sensor
  if (!bmp.begin()) { // If the BMP sensor fails to initialize
    sprintf(serialOutputBuffer, "%sCould not find a valid BMP085/BMP180 sensor, check wiring!%s", Cred, Creset);
    Serial.println(serialOutputBuffer); 
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    display.display();
    for (;;); // Don't proceed, loop forever
  }

  //RandomStaticLoad(); // Load a random static image onto the LED Matrix
  sprintf(serialOutputBuffer, "%sSetup Complete%s", Cgreen, Creset);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0'; // Clear the serial output buffer by setting the first character to null

  // for screensavers
  randomSeed(analogRead(0));
  lastEffectChangeTime = millis();

  display.clearDisplay();
}

// NetworkChange, If the network disconnects, it reconnects to another predefined network
void NetworkChange() {
  while (WiFi.status() != WL_CONNECTED) {
    network = (network + 1) % 3;
    switch(network) {
      case 0:
        WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);
        sprintf(serialOutputBuffer, "%sConnected To Network 1.%s", Cblue, Creset);
        Serial.println(serialOutputBuffer);
        serialOutputBuffer[0] = '\0';
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xAE);
        display.display();
        strncpy(localIP, WiFi.localIP().toString().c_str(), sizeof(localIP) - 1);
        localIP[sizeof(localIP) - 1] = '\0'; // Ensure null termination
        strncpy(subnetMask, WiFi.subnetMask().toString().c_str(), sizeof(subnetMask) - 1);
        subnetMask[sizeof(subnetMask) - 1] = '\0'; // Ensure null termination
        strncpy(gatewayIP, WiFi.gatewayIP().toString().c_str(), sizeof(gatewayIP) - 1);
        gatewayIP[sizeof(gatewayIP) - 1] = '\0'; // Ensure null termination
        delay(100);
        break;
      case 1:
        WiFi.begin(SECRET_SSID_2, SECRET_OPTIONAL_PASS_2);
        sprintf(serialOutputBuffer, "%sConnected To Network 2.%s", Cblue, Creset);
        Serial.println(serialOutputBuffer);
        serialOutputBuffer[0] = '\0';
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xAF);
        display.display();
        strncpy(localIP, WiFi.localIP().toString().c_str(), sizeof(localIP) - 1);
        localIP[sizeof(localIP) - 1] = '\0'; // Ensure null termination
        strncpy(subnetMask, WiFi.subnetMask().toString().c_str(), sizeof(subnetMask) - 1);
        subnetMask[sizeof(subnetMask) - 1] = '\0'; // Ensure null termination
        strncpy(gatewayIP, WiFi.gatewayIP().toString().c_str(), sizeof(gatewayIP) - 1);
        gatewayIP[sizeof(gatewayIP) - 1] = '\0'; // Ensure null termination
        delay(100);
        break;
    }
  }
}

// RandomStaticLoad, Loads a random predefined image onto the Arduino R4 WiFi led matrix
//const int StaticAnimationSelection = random(0,12);
const int StaticAnimationSelection = 3; // For testing
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

// calculateAverage, A modular function that calculates the average from a number of samples
template <typename T>
T calculateAverage(T value, unsigned long interval, unsigned long &lastUpdateTime, T &sum, int &count, int &samples) { 
  unsigned long currentTimeT = millis();
  if (currentTimeT - lastUpdateTime >= interval) {
    sum += value;
    count++;
    if (count >= samples) { // Adjusts the reactiveness, samples recorded
      T average = sum / count;
      sum /= count; // Divide sum by count to keep a running average
      count = 0; 
      lastUpdateTime = currentTimeT; 
    }
  }
  return sum / (count > 0 ? count : 1); // Avoid division by zero
}

// LiveThermomiterNew, Doesn't work currently
void LiveThermomiterNew() {
  float currentTemp = dht.readTemperature();
  int redIntensity = constrain(map(currentTemp, 22, 25, 0, 100), 0, 100);
  int yellowIntensity = (constrain(map(currentTemp, 18, 25, 0, 100), 0, 100)) - (redIntensity);
  int greenIntensity = (constrain(map(currentTemp, 10, 22, 0, 100), 0, 100)) - (yellowIntensity);
  int cyanIntensity = (constrain(map(currentTemp, 10, 22, 0, 100), 0, 100)) - (greenIntensity);
  int blueIntensity = (constrain(map(currentTemp, 0, 10, 0, 100), 0, 100));
  analogWrite(redLED, redIntensity);
  analogWrite(yellowLED, yellowIntensity);
  analogWrite(greenLED, greenIntensity);
  analogWrite(cyanLED, cyanIntensity);
  analogWrite(blueLED, blueIntensity); 
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
  //Serial.print("Average Value: ");
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
  if (millis() - previousMillis >= 300000) {
    previousMillis = millis();
    if (timeClient.update()) { // If the NTP update is successful
      whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
      analogWrite(whiteLED, whiteLightness);
      unixTime = timeClient.getEpochTime();
      sprintf(serialOutputBuffer, "%sUnix Time: %s", Cblue, Creset);
      Serial.println(serialOutputBuffer);
      serialOutputBuffer[0] = '\0';
      Serial.println(unixTime);
      sprintf(serialOutputBuffer, "%sNTP Time Synced%s", Cgreen, Creset);
      Serial.println(serialOutputBuffer);
      serialOutputBuffer[0] = '\0';
      RTCTime timeToSet = RTCTime(unixTime);
      RTC.setTime(timeToSet);
    } else {
      sprintf(serialOutputBuffer, "%sNTP Update Failed%s", Cred, Creset);
      Serial.println(serialOutputBuffer);
      serialOutputBuffer[0] = '\0';
    }
  }
  analogWrite(whiteLED, LOW);
}

float DeltaPressure24() {
  if (millis() - DPPreviousMillis >= 500000) {
    DPPreviousMillis = millis(); // Fixing to use the current millis() for accurate timing
    pressure = bmp.readPressure(); // Get current pressure reading
    pressureReadings[readingIndex] = pressure;   // Store the new reading in the array
    readingIndex = (readingIndex + 1) % NUM_READINGS; // Wrap around the array
    oldestIndex = (readingIndex + NUM_READINGS - 1) % NUM_READINGS; // Index of the oldest reading
    deltaP = pressure - pressureReadings[oldestIndex]; // Calculate the change in pressure
  }
  return deltaP;
}

// OLEDHeader, inputs the header of the oled to buffer for displaying, shows some symbols and the current ntp date and time
void OLEDHeader(char dateOnly[], char timeOnly[]) {
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
void OLEDPanel1(float formattedC, int formattedLightSensorData, float formattedHumdiditySensor, float formattedPressureSensor, int formattedMicrophoneSensor, char hoursOnline[]) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("1/3");
  display.setCursor(0, 16); 
  sprintf(serialOutputBuffer, "Temp: %.1f", formattedC);
  display.print(serialOutputBuffer);
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  sprintf(serialOutputBuffer, "Light: %d Lm", formattedLightSensorData);
  display.println(serialOutputBuffer);
  display.setCursor(0, 32); 
  sprintf(serialOutputBuffer, "Humid: %.1f%%", formattedHumdiditySensor);
  display.println(serialOutputBuffer);
  display.setCursor(0, 40); 
  sprintf(serialOutputBuffer, "Pressure: %.2f mBar", formattedPressureSensor);
  display.println(serialOutputBuffer);
  display.setCursor(0, 48); 
  sprintf(serialOutputBuffer, "Noise: %d U", formattedMicrophoneSensor);
  display.println(serialOutputBuffer);
  display.setCursor(0, 56); 
  sprintf(serialOutputBuffer, "Hours: %s H", hoursOnline);
  display.println(serialOutputBuffer);
}

// OLEDPanel2, Shows a few different more readings that couldnt fit on 1 from the sensors without having to access the website, useful for debugging.
void OLEDPanel2(float formattedOutC, float formattedOutHumiditySensor, int formattedMagnetSensor, float altitude, char daysOnline[]) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("2/3");
  display.setCursor(0, 16);
  sprintf(serialOutputBuffer, "OTemp: %.1f", formattedOutC);
  display.print(serialOutputBuffer);
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  sprintf(serialOutputBuffer, "OHumid: %.1f%%", formattedOutHumiditySensor);
  display.println(serialOutputBuffer);
  display.setCursor(0, 32); 
  sprintf(serialOutputBuffer, "Magnet: %d", formattedMagnetSensor);
  display.println(serialOutputBuffer);
  display.setCursor(0, 40); 
  sprintf(serialOutputBuffer, "Altitude: %.2f m", altitude);
  display.println(serialOutputBuffer);
  display.setCursor(0, 48); 
  sprintf(serialOutputBuffer, "Days: %sD", daysOnline);
  display.println(serialOutputBuffer);
  display.setCursor(0, 56); 
}

// OLEDPanel3, Shows a few different network statistics, useful for debugging
void OLEDPanel3(String localIP, String subnetMask, String gatewayIP, String NTPIP, int signalStrength, unsigned long previousMillis, float DP24) {
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
  sprintf(serialOutputBuffer, "Signal: %ddBm", signalStrength);
  display.println(serialOutputBuffer);
  display.setCursor(0, 56);
  sprintf(serialOutputBuffer, "NTP UP: %lu", 0);
  display.println(serialOutputBuffer);
  display.setCursor(0, 64);
  sprintf(serialOutputBuffer, "DP: %.2f", DP24);
  display.println(serialOutputBuffer);
}

void screensavers() {
  if (millis() - lastEffectChangeTime > effectDuration) {
    currentEffect = random(0, 10);
    lastEffectChangeTime = millis();
    display.clearDisplay(); // Clear display before drawing new effect
  }
  switch (currentEffect) {
    case 0:
      drawRandomPixels();
      break;
    case 1:
      drawMovingLine();
      break;
    case 2:
      drawExpandingCircles();
      break;
    case 3:
      drawFallingStars();
      break;
    case 4:
      drawBouncingBall();
      break;
    case 5:
      drawSineWave();
      break;
    case 6:
      drawRotatingBox();
      break;
    case 7:
      drawZigZag();
      break;
    case 8:
      drawRain();
      break;
    case 9:
      drawCircleWave();
      break;
  }
}

int MicLevels() {
  startMillis = millis(); // Start of sample window
  peakToPeak = 0;   // peak-to-peak level
  signalMax = 0;
  signalMin = 1024;
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

int DebugMillis = 0;
void loop() {
  //DebugMillis = millis(); // Reset the debug timer
  // Put continuous updates here
  LiveThermomiter();
  NetworkChange();
  NTPSync();
  //DeltaPressure24();
  //float DP24 = DeltaPressure24();

  // Time and Date Data
  RTCTime currentTime;
  RTC.getTime(currentTime);
  stringTime = String(currentTime);
  tPosition = stringTime.indexOf('T');
  sprintf(timeOnly, "%s", stringTime.substring(tPosition + 1).c_str());
  sprintf(dateOnly, "%s", stringTime.substring(0, tPosition).c_str());
  secondsOnline = millis() / 1000;
  sprintf(hoursOnline, "%.2f", (millis() / 3600000.0));
  sprintf(daysOnline, "%.2f", (millis() / 86400000.0));

  // Sensor Data
  if (millis() - SensorPreviousMillis >= 5000) {
    SensorPreviousMillis = millis();
    sprintf(formattedTime, "%s / %s", dateOnly, timeOnly);
    formattedC = dht.readTemperature();
    formattedOutC = dht2.readTemperature();
    formattedF = (dht.readTemperature() * 9.0/5.0) + 32.0;
    formattedOutF = (dht2.readTemperature() * 9.0/5.0) + 32.0;
    formattedLightSensorData = analogRead(LightSensor) * 2;
    formattedHumiditySensor = dht.readHumidity();
    formattedOutHumiditySensor = dht2.readHumidity();
    pressure = bmp.readPressure();
    formattedPressureSensor = pressure / 100;
    formattedMicrophoneSensor = MicLevels();
    formattedMagnetSensor = (digitalRead(magneticSensor) == 1 ? 0 : 1);
    altitude = bmp.readAltitude(seaLevelPressure);
    signalStrength = WiFi.RSSI();
  }

  // OLED Data Display Function and Screensavers
  if (millis() - screensaverMillis >= 300000) {
    screensaverMillis = millis();
    screensaverActive = (screensaverActive == 1) ? 0 : ((random(0, 4) == 1) ? 1 : screensaverActive);
  }
  if (screensaverActive == 0) {
    if (millis() - OLEDPreviousMillis >= 5000) {
      OLEDPreviousMillis = millis();
      OLEDPanel = (OLEDPanel + 1) % 3;
    }
    display.clearDisplay();
    OLEDHeader(dateOnly, timeOnly);
    switch (OLEDPanel) {
      case 0:
        OLEDPanel1(formattedC, formattedLightSensorData, formattedHumiditySensor, formattedPressureSensor, formattedMicrophoneSensor, hoursOnline);
        break;
      case 1:
        OLEDPanel2(formattedOutC, formattedOutHumiditySensor, formattedMagnetSensor, altitude, daysOnline);
        break;
      case 2:
        OLEDPanel3(localIP, subnetMask, gatewayIP, NTPIP, signalStrength, previousMillis, DP24);
        break;
    }
    display.display(); // Display everything held in buffer
  } else {
    screensavers();
    display.display();
  }

  // CSV Server Data Function, When a connection is made, data is sent. a partner python script saves the data to a CSV file.
  WiFiClient DataClient = DataServer.available(); // Check for incoming connections
  if (DataClient) { // If a client connects
    sprintf(serialOutputBuffer, "%sNew Data Client.%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    while (DataClient.connected()) {
      if (DataClient.available()) {
        String request = DataClient.readStringUntil('\n');
        sprintf(serialOutputBuffer, "%sRequest received: %s%s", Cblue, request, Creset);
        Serial.println(serialOutputBuffer);
        serialOutputBuffer[0] = '\0';
        if (request == "REQUEST_DATA") {
          sprintf(data, "<START>%s,%s,%.1f,%.1f,%d,%.1f,%.2f,%.2f,%d,%lu<END>", dateOnly, timeOnly, formattedC, formattedOutC, formattedLightSensorData, formattedHumiditySensor, formattedOutHumiditySensor, formattedPressureSensor, formattedMicrophoneSensor, secondsOnline);
          DataClient.print(data);
          sprintf(serialOutputBuffer, "%sData sent to client: %s%s", Cgreen, data, Creset); // Debug statement
          Serial.println(serialOutputBuffer);
          serialOutputBuffer[0] = '\0';
          break;  // Exit the loop after sending the data once
        }
      }
    }
    sprintf(serialOutputBuffer, "%sData Client Disconnected.%s", Cyellow, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
  }
  
  // Website Function
  WiFiClient WebClient = WebServer.available(); // Check for incoming connections
  if (WebClient) { // If a client connects
    sprintf(serialOutputBuffer, "%sNew Web Client.%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    while (WebClient.connected()) { // Keep connection open until client disconnects
      if (WebClient.available()) {
        whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
        analogWrite(whiteLED, whiteLightness); // Indicate that data transfer has started
        screensaverActive = 0; // Disable screensaver
        char c = WebClient.read(); // Read and save incoming data, initial connection including subpage request
        referrer += c;
        if (c == '\n') { // Check for end of line
          int firstSpace = referrer.indexOf(' ');
          int secondSpace = referrer.indexOf(' ', firstSpace + 1);
          url = referrer.substring(firstSpace + 1, secondSpace);
          url.trim(); // Isolated subpage request
          String request = "";
          while (WebClient.available()) {
            char c = WebClient.read();
            request += c;
          }
          sprintf(serialOutputBuffer, "%sURL :%s%s", Cblue, url, Creset);
          Serial.println(serialOutputBuffer);
          serialOutputBuffer[0] = '\0';
          if (url == "/") {
            mainPage(
              WebClient,
              bmp,
              dht,
              dht2,
              Cblue,
              Creset,
              serialOutputBuffer,
              seaLevelPressure,
              LightSensor,
              pressure,
              formattedTime,
              formattedC,
              formattedF,
              inTempDisplacement,
              formattedOutC,
              formattedOutF,
              outTempDisplacement,
              formattedLightSensorData,
              lightDisplacement,
              formattedHumiditySensor,
              inHumidityDisplacement,
              formattedOutHumiditySensor,
              outHumidityDisplacement,
              formattedPressureSensor,
              pressureDisplacement,
              altitude,
              altitudeDisplacement,
              formattedMicrophoneSensor,
              noiseDisplacement,
              formattedMagnetSensor,
              secondsOnline,
              hoursOnline
            );
            break;
          } else if (url == "/about") {
            aboutPage(serialOutputBuffer, WebClient, Cblue, Creset);
            break;
          } else if (url == "/data") {
            dataPage(serialOutputBuffer, WebClient, Cblue, Creset);
            break;
          } else if (url == "/aprocryphan") {
            aprocryphanPage(serialOutputBuffer, WebClient, Cblue, Creset);
            break;
          } else if (url == "/web1") {
            web1Page(WebClient);
            break;
          } else { // Serve error 404 page
            errPage(serialOutputBuffer, WebClient, Cyellow, Creset);
            break;
          }
        }
      }
    }
    WebClient.stop(); // Disconnect the client because all data has been sent
    sprintf(serialOutputBuffer, "%sWeb Client disconnected.%s", Cyellow, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    analogWrite(whiteLED, LOW);
    url = ""; // Empty out for next connection request
    referrer = ""; // Empty out for next connection request
  }
  //Serial.println(millis() - DebugMillis); // Debugging
}