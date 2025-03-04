#include "arduino_secrets.h"
#include "A4WBitmaps.h"
#include "ANSICodes.h"
#include <WiFi.h> // For WiFi connection
#include "RTC.h" // For Real Time Clock
#include <EEPROM.h> // Persistant value storage 1Kb max
#include <NTPClient.h> // For NTP (Network Time Protocol) time
#include <WiFiUdp.h>
#include <String.h>
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
WiFiServer DataServer(8081); // Defines the port that the data server is hosted on
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
const int NUM_READINGS = 288; // 24 hours of data, one reading every 5 minutes
unsigned int sample; // Sample value from the pressure sensor
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

const char* weatherEmojis[] = { "⚡", "☔", "☁️", "🌨️", "🌧️", "🌩️", "⛈️", "🌦️", "🌥️", "⛅", "🌤️", "🌡️", "🔥", "❄️", "🌫️", "🌙", "☀️" };
char serialOutputBuffer [128]; // Buffer for serial output
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
  Serial.println("\n*************** Ardu4Weather v0.25.6 - Commit 22 *******************");

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
      int whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
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
  sprintf(serialOutputBuffer, "Hours: %.2f H", hoursOnline);
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
  sprintf(serialOutputBuffer, "NTP UP: %lu", previousMillis);
  display.println(serialOutputBuffer);
  display.setCursor(0, 64);
  sprintf(serialOutputBuffer, "DP: %.2f", DP24);
  display.println(serialOutputBuffer);
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
  int tPosition = stringTime.indexOf('T');
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

  // OLED Data Display Function
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
            sprintf(serialOutputBuffer, "%sMain Page Requested%s", Cblue, Creset);
            Serial.println(serialOutputBuffer);
            serialOutputBuffer[0] = '\0';
            pressure = bmp.readPressure();
            inTempDisplacement = constrain((map(dht.readTemperature(), -5, 30, 0, 100)), 0, 100); // Displacement for the indicators on the bars, maps and constrains input between -50 and 13300
            outTempDisplacement = constrain((map(dht2.readTemperature(), -5, 30, 0, 100)), 0, 100);
            lightDisplacement = constrain((map((analogRead(LightSensor) * 2), 0, 1000, 0, 100)), 0, 100);
            inHumidityDisplacement = constrain((map(dht.readHumidity(), 0, 100, 0, 100)), 0, 100);
            outHumidityDisplacement = constrain((map(dht2.readHumidity(), 0, 100, 0, 100)), 0, 100);
            pressureDisplacement = constrain((map((pressure / 100), 980, 1030, 0, 100)), 0, 100);
            altitudeDisplacement = constrain((map(bmp.readAltitude(seaLevelPressure), 0, 3000, 0, 100)), 0, 100);
            noiseDisplacement = constrain((map((MicLevels()), 10, 700, 0, 100)), 0, 100);
            if (isnan(dht2.readTemperature())) { // If the outdoor sensor is disconnected indicator is at 0
              outTempDisplacement = 0;
            }
            WebClient.print("HTTP/1.1 200 OK\r\n");
            WebClient.print("Content-Type: text/html; charset=utf-8\r\n");
            WebClient.print("X-Content-Type-Options: nosniff\r\n");
            WebClient.print("Cache-Control: no-cache, public\r\n");
            WebClient.print("Server: Arduino\r\n");
            WebClient.print("Content-Security-Policy: frame-ancestors 'self';\r\n");
            WebClient.print("\r\n");
            WebClient.print("<!DOCTYPE html><html lang='en'>");
            WebClient.print("<meta name='viewport' content='width=device-width'>");
            // Style/CSS Section
            WebClient.print("<head><meta charset='utf-8'>");
            WebClient.print("<title>Ardu4Weather</title>");
            WebClient.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            WebClient.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            WebClient.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            WebClient.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
            WebClient.print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
            WebClient.print("<style>");
            WebClient.print("nav { background-color: #434b4f; transition-duration: 0.4s; font-size: 16px; display: flex;  align-items: center; padding: 10px 0; position: relative; }");
            WebClient.print("nav:hover { background-color: #53636A; transition-duration: 0.4s; font-size: 18px; display: flex; align-items: center; position: relative; }");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; flex-grow: 1; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; }");
            WebClient.print("nav a { color: #ffffff; text-decoration: none; }");
            WebClient.print("nav a:hover { color: #ccc; }");
            WebClient.print("body { background: linear-gradient(0deg, hsla(198, 18%, 45%, 1) 0%, hsla(197, 66%, 90%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            //display: grid; grid-template-columns: 3fr 1fr; grid-gap: 20px; 
            WebClient.print(".main-container { width: 90%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #53636A; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
            WebClient.print(".sidebar-container { background-color: #53636A; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); transition-duration: 0.4s; }");
            WebClient.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
            WebClient.print(".data-item { display: flex; justify-content: space-between; /* Align label and value */ margin-bottom: 10px; color: #ffffff; font-size: 25px; }");
            WebClient.print(".data-label { font-weight: bold; color: #ffffff; font-size: 25px; }");
            WebClient.print(".slider-container { position: relative; }");
            WebClient.print(".slider-text { position: absolute; top: 50%; text-shadow: 0.5px 0.5px 1px rgba(255, 255, 255, 0.5);  transform: translateY(10px); font-size: 12px; }");
            WebClient.print(".slider-text.left { left: 0; color: #7293a1 }");
            WebClient.print(".slider-text.right { right: 0; color: #b95b5e}");
            WebClient.print(".slider-text.light-left { left: 0; color: #ffffff }");
            WebClient.print(".slider-text.light-right { right: 0; color: #ffe98a}");
            WebClient.print(".slider-text.humid-left { left: 0; color: #ffffff }");
            WebClient.print(".slider-text.humid-right { right: 0; color: #7293a1}");
            WebClient.print(".slider-text.altitude-left { left: 0; color: #7293a1 }");
            WebClient.print(".slider-text.altitude-right { right: 0; color: #ffffff}");
            WebClient.print(".slider-text.noise-left { left: 0; color: #ffffff }");
            WebClient.print(".slider-text.noise-right { right: 0; color: #b95b5e}");
            WebClient.print(".slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(198, 20%, 54%, 1) 0%,  hsla(0, 0%, 100%, 1) 50%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            WebClient.print(".light-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(0, 0%, 100%, 1) 0%, hsla(49, 100%, 77%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            WebClient.print(".humid-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(198, 20%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            WebClient.print(".altitude-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(198, 20%, 54%, 1) 0%, hsla(0, 0%, 100%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            WebClient.print(".noise-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            WebClient.print(".in-temp-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
            WebClient.print(".out-temp-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
            WebClient.print(".light-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
            WebClient.print(".in-humidity-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
            WebClient.print(".out-humidity-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
            WebClient.print(".pressure-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
            WebClient.print(".altitude-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
            WebClient.print(".noise-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
            WebClient.print(".header-image img { max-width: 4%; height: auto; padding-left: 10px; display: block; float: left; }");
            WebClient.print("button { background-color: #ffffff; color: #84a8b2; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transition-duration: 0.4s; }");
            WebClient.print("button:hover { background-color: #84a8b2; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ }");
            WebClient.print("button:active { background-color: #7293A1; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transform: translateY(2px);}");
            WebClient.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("</style>");
            WebClient.print("<nav><div class='header-image'><img src='https://i.imgur.com/mlL3Fiw.png' alt='Ardu4Weather Logo' href='/'></div>");
            WebClient.print("<ul><li><a href='/'>Home</a></li>");
            WebClient.print("<li><a href='/about'>About</a></li>");
            WebClient.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            WebClient.print("</head>");
            // Javascript Section
            // Body/HTML Section
            WebClient.print("<body><div class='main-container'><h1>Ardu4Weather - Arduino Weather Station</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Time:</span><span>");
            WebClient.print(formattedTime);
            WebClient.print("</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Indoor Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text left'>-5&deg;C</span>");
            WebClient.print("<div class='slider-bar'><div class='in-temp-indicator' style='left: ");
            WebClient.print(inTempDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text right'>30&deg;C</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Outdoor Temperature:</span><span>");
            WebClient.print(formattedOutC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedOutF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text left'>-5&deg;C</span>");
            WebClient.print("<div class='slider-bar'><div class='out-temp-indicator' style='left: ");
            WebClient.print(outTempDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text right'>30&deg;C</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text light-left'>0 Lm</span>");
            WebClient.print("<div class='light-slider-bar'><div class='light-indicator' style='left: ");
            WebClient.print(lightDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text light-right'>1000 Lm</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Indoor Humidity:</span><span>");
            WebClient.print(formattedHumiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text humid-left'>0%</span>");
            WebClient.print("<div class='humid-slider-bar'><div class='in-humidity-indicator' style='left: ");
            WebClient.print(inHumidityDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text humid-right'>100%</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Outdoor Humidity:</span><span>");
            WebClient.print(formattedOutHumiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text humid-left'>0%</span>");
            WebClient.print("<div class='humid-slider-bar'><div class='out-humidity-indicator' style='left: ");
            WebClient.print(outHumidityDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text humid-right'>100%</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text left'>980 mBar</span>");
            WebClient.print("<div class='slider-bar'><div class='pressure-indicator' style='left: ");
            WebClient.print(pressureDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text right'>1030 mBar</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Approx Altitude:</span><span>");
            WebClient.print(altitude);
            WebClient.print(" m</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text altitude-left'>0 m</span>");
            WebClient.print("<div class='altitude-slider-bar'><div class='altitude-indicator' style='left: ");
            WebClient.print(altitudeDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text altitude-right'>3000 m</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" Units</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text noise-left'>10 U</span>");
            WebClient.print("<div class='noise-slider-bar'><div class='noise-indicator' style='left: ");
            WebClient.print(noiseDisplacement);
            WebClient.print("%;'></div></div>");
            WebClient.print("<span class='slider-text noise-right'>700 U</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Presence Of Strong Magnetic Field:</span><span>");
            WebClient.print(formattedMagnetSensor);
            WebClient.print("</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Online For:</span><span>");
            WebClient.print(secondsOnline);
            WebClient.print(" Seconds (");
            WebClient.print(hoursOnline);
            WebClient.print("H)</span></div>");
            WebClient.print("<div><button id='toggleButton'>Show Averages</button></div>");
            // Hidden Average Data, 1H
            WebClient.print("<div id='1HaverageData' style='display: none;'><h1>1 Hour Rolling Average</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            // Hidden Average Data, 12H
            WebClient.print("<div id='12HaverageData' style='display: none;'><h1>12 Hour Rolling Average</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            // Hidden Avarage Data, 24H
            WebClient.print("<div id='24HaverageData' style='display: none;'><h1>24 Hour Rolling Average</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            WebClient.print("<body><div class='sidebar-container'><h1>Side Container</h1>");
            WebClient.print("</div></div>");
            // Javascript for button
            WebClient.print("<script>");
            WebClient.print("const toggleButton = document.getElementById('toggleButton');");
            WebClient.print("const avgDataDiv1 = document.getElementById('1HaverageData');");
            WebClient.print("const avgDataDiv12 = document.getElementById('12HaverageData');");
            WebClient.print("const avgDataDiv24 = document.getElementById('24HaverageData');");
            WebClient.print("toggleButton.addEventListener('click', () => {");
            WebClient.print("  if (toggleButton.textContent === 'Show Averages') {");
            WebClient.print("    toggleButton.textContent = '- Collapse -';");
            WebClient.print("    avgDataDiv1.style.display = 'block';");
            WebClient.print("    avgDataDiv12.style.display = 'block';");
            WebClient.print("    avgDataDiv24.style.display = 'block';");
            WebClient.print("  } else {");
            WebClient.print("    toggleButton.textContent = 'Show Averages';");
            WebClient.print("    avgDataDiv1.style.display = 'none';");
            WebClient.print("    avgDataDiv12.style.display = 'none';");
            WebClient.print("    avgDataDiv24.style.display = 'none';");
            WebClient.print("  }");
            WebClient.print("});");
            WebClient.print("</script>");
            WebClient.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS<br>");
            WebClient.print("<a href='https://github.com/Aprocryphan/Ardu4Weather' style='text-decoration: underline; display: flex; align-items: center;' target='_blank'>");
            WebClient.print("<img src='https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png' alt='GitHub Logo' style='width: 20px; height: 20px; margin-right: 5px;'>");
            WebClient.print("GitHub</a></p></footer>");
            WebClient.print("</body></html>");
            WebClient.flush();
            break;
          } else if (url == "/about") {
            sprintf(serialOutputBuffer, "%sAbout Page Requested%s", Cblue, Creset);
            Serial.println(serialOutputBuffer);
            serialOutputBuffer[0] = '\0';
            WebClient.print("HTTP/1.1 200 OK\r\n");
            WebClient.print("Content-Type: text/html; charset=utf-8\r\n");
            WebClient.print("X-Content-Type-Options: nosniff\r\n");
            WebClient.print("Cache-Control: no-cache, public\r\n");
            WebClient.print("Server: Arduino\r\n");
            WebClient.print("Content-Security-Policy: frame-ancestors 'self';\r\n");
            WebClient.print("\r\n");
            WebClient.print("<!DOCTYPE html><html lang='en'>");
            WebClient.print("<meta name='viewport' content='width=device-width'>");
            // Style/CSS Section
            WebClient.print("<head><meta charset='utf-8'>");
            WebClient.print("<title>Ardu4Weather</title>");
            WebClient.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            WebClient.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            WebClient.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            WebClient.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
            WebClient.print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
            WebClient.print("<nav><ul><li><a href='/'>Home</a></li>");
            WebClient.print("<li><a href='/about'>About</a></li>");
            WebClient.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            WebClient.print("<style>");
            WebClient.print("nav { background-color: #5e3434; padding: 10px 0; font-size: 15px; transition-duration: 0.4s; }");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
            WebClient.print("nav a { color: #ffffff; text-decoration: none; }");
            WebClient.print("nav a:hover { color: #ccc; }");
            WebClient.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
            WebClient.print("h2 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 30px; }");
            WebClient.print("p { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 20px; }");
            WebClient.print(".footer-p { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 16px; }");
            WebClient.print("body { background: linear-gradient(180deg, hsla(354, 53%, 70%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            WebClient.print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #7d4042; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
            WebClient.print("img { max-width: 30%; height: auto; border-radius: 16px; margin: 10px; }");
            WebClient.print(".image-text-container { display: flex; flex-direction: column; width: 80%; margin: 20px auto; }");
            WebClient.print(".image-text-pair { display: flex; align-items: center; margin-bottom: 20px; }");
            WebClient.print(".image-text-pair img { max-width: 30%; height: auto; border-radius: 16px; margin-right: -30px; margin-left: -40px; }");
            WebClient.print(".image-text p { width: 110%; padding: 40px; padding-top: 0px; box-sizing: border-box; margin-left: 30px; text-align: left; }");
            WebClient.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("</style></head>");
            WebClient.print("<body><div class='main-container'><h1>Ardu4Weather - About</h1>");
            WebClient.print("<h2>Arduino Uno R4 Wifi</h2>");
            WebClient.print("<div class='image-text-container'>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/2Vkg8gU.jpeg' alt=\'Arduino R4 WiFi Image Close'><div class='image-text'>");
            WebClient.print("<p>My Arduino Uno R4 WiFi is the heart of this project, hosting the website you're currently viewing, complete with all subpages. It's connected to a variety of sensors, including a DHT22 for temperature and humidity, a BMP180 for barometric pressure, and a photoresistor for light levels. The Arduino collects data constantly in the loop() function, processing it and sending it to the website in real-time.</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>Breadboard</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/Z8AHWJq.jpeg' alt=\'Arduino Breadboard Image'><div class='image-text'>");
            WebClient.print("<p>This is the breadboard that contains most of the sensors used in this project. It is a small, clear, standard 30x10 breadboard.</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>Arduino Full Image</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/yuNZByw.jpeg' alt=\'Arduino Full Image'><div class='image-text'>");
            WebClient.print("<p>This is an image of the full project with breadboard and Arduino in view. You can also see the wires that lead outside my window for the external DHT sensor.</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>Microphone Sensor Image</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/541JSSP.jpeg' alt=\'Microphone Sensor Image'><div class='image-text'>");
            WebClient.print("<p>MAX4466 Electret Microphone Amplifier for noise levels</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>Pressure Sensor Image</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/uKWry8u.jpeg' alt=\'Pressure Sensor Image'><div class='image-text'>");
            WebClient.print("<p>BMP180</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>DHT Sensor Image</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/A5zKZvm.jpeg' alt=\'DHT Sensor Image'><div class='image-text'>");
            WebClient.print("<p>This is the internal DHT22 sensor, it returns the temperature and humidity digitally.</p>");
            WebClient.print("</div></div>");
            WebClient.print("<h2>Light Sensor Image</h2>");
            WebClient.print("<div class='image-text-pair'><img src=\'https://i.imgur.com/7kgKpvS.jpeg' alt=\'Light Sensor Image'><div class='image-text'>");
            WebClient.print("<p>Photoresistor</p>");
            WebClient.print("</div></div></div></div>");
            WebClient.print("<footer><p class='footer-p'>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
            WebClient.print("</body></html>");
            WebClient.flush();
            break;
          } else if (url == "/data") {
            sprintf(serialOutputBuffer, "%sData Page Requested%s", Cblue, Creset);
            Serial.println(serialOutputBuffer);
            serialOutputBuffer[0] = '\0';
            WebClient.print("HTTP/1.1 200 OK\r\n");
            WebClient.print("Content-Type: text/html; charset=utf-8\r\n");
            WebClient.print("X-Content-Type-Options: nosniff\r\n");
            WebClient.print("Cache-Control: no-cache, public\r\n");
            WebClient.print("Server: Arduino\r\n");
            WebClient.print("Content-Security-Policy: frame-ancestors 'self';\r\n");
            WebClient.print("\r\n");
            WebClient.print("<!DOCTYPE html><html lang='en'>");
            WebClient.print("<meta name='viewport' content='width=device-width'>");
            // Style/CSS Section
            WebClient.print("<head><meta charset='utf-8'>");
            WebClient.print("<title>Ardu4Weather</title>");
            WebClient.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            WebClient.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            WebClient.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            WebClient.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=construction' />");
            WebClient.print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
            WebClient.print("<nav><ul><li><a href='/'>Home</a></li>");
            WebClient.print("<li><a href='/about'>About</a></li>");
            WebClient.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            WebClient.print("<style>");
            WebClient.print("nav { background-color: #24331e; padding: 10px 0; transition-duration: 0.4s; }");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
            WebClient.print("nav a { color: #ffffff; text-decoration: none; }");
            WebClient.print("nav a:hover { color: #ccc; }");
            WebClient.print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
            WebClient.print(".icon { font-size: 16px; color: #ffffff; z-index: 0; }");
            WebClient.print("body { background: linear-gradient(180deg, hsla(93, 64%, 79%, 1) 0%, hsla(96, 30%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            WebClient.print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #45573b; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
            WebClient.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
            WebClient.print("h2 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 20px; }");
            WebClient.print("p { color: #ffffff; text-align: center; margin-bottom: 20px; }");
            WebClient.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("</style></head>");
            WebClient.print("<body><div class='main-container'><h1>Ardu4Weather - Data</h1>");
            WebClient.print("<h2>Historical Data</h2>");
            WebClient.print("<p>Page Under Construction! <span class='material-symbols-outlined icon'>construction</span></p>");
            WebClient.print("</div>");
            WebClient.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
            WebClient.print("</body></html>");
            WebClient.flush();
            break;
          } else if (url == "/aprocryphan") {
            sprintf(serialOutputBuffer, "%sAbout Apro Page Requested%s", Cblue, Creset);
            Serial.println(serialOutputBuffer);
            serialOutputBuffer[0] = '\0';
            WebClient.print("HTTP/1.1 200 OK\r\n");
            WebClient.print("Content-Type: text/html; charset=utf-8\r\n");
            WebClient.print("X-Content-Type-Options: nosniff\r\n");
            WebClient.print("Cache-Control: no-cache, public\r\n");
            WebClient.print("Server: Arduino\r\n");
            WebClient.print("Content-Security-Policy: frame-ancestors 'self';\r\n");
            WebClient.print("\r\n");
            WebClient.print("<!DOCTYPE html><html lang='en'>");
            WebClient.print("<meta name='viewport' content='width=device-width'>");
            // Style/CSS Section
            WebClient.print("<head><meta charset='utf-8'>");
            WebClient.print("<title>Ardu4Weather</title>");
            WebClient.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            WebClient.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            WebClient.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            WebClient.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=construction' />");
            WebClient.print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
            WebClient.print("<nav><ul><li><a href='/'>Home</a></li>");
            WebClient.print("<li><a href='/about'>About</a></li>");
            WebClient.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            WebClient.print("<style>");
            WebClient.print("nav { background-color: #ffffff; padding: 10px 0; transition-duration: 0.4s; }");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
            WebClient.print("nav a { color: #000000; text-decoration: none; }");
            WebClient.print("nav a:hover { color: #ccc; }");
            WebClient.print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
            WebClient.print("body { background: #000000; font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            WebClient.print(".icon { font-size: 16px; color: #000000; z-index: 0; }");
            WebClient.print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #ffffff; border-radius: 8px; transition-duration: 0.4s; }");
            WebClient.print("h1 { color: #000000; text-align: center; margin-bottom: 20px; font-size: 40px; }");
            WebClient.print("h2 { color: #000000; text-align: center; margin-bottom: 20px; font-size: 20px; }");
            WebClient.print("p { color: #000000; }");
            WebClient.print("footer { background-color: #ffffff; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("</style></head>");
            WebClient.print("<body><div class='main-container'><h1>Ardu4Weather - Aprocryphan</h1>");
            WebClient.print("<h2>About</h2>");
            WebClient.print("<p>Page Under Construction! <span class='material-symbols-outlined icon'>construction</span></p>");
            WebClient.print("</div>");
            WebClient.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
            WebClient.print("</body></html>");
            WebClient.flush();
            break;
          } else if (url == "/web1") {
            WebClient.flush();
            break;
          } else { // Serve error 404 page
            sprintf(serialOutputBuffer, "%sError 404%s", Cyellow, Creset);
            Serial.println(serialOutputBuffer);
            serialOutputBuffer[0] = '\0';
            WebClient.print("HTTP/1.1 404 Not Found\r\n");
            WebClient.print("Content-Type: text/html; charset=utf-8\r\n");
            WebClient.print("X-Content-Type-Options: nosniff\r\n");
            WebClient.print("Cache-Control: no-cache, public\r\n");
            WebClient.print("Server: Arduino\r\n");
            WebClient.print("Content-Security-Policy: frame-ancestors 'self';\r\n");
            WebClient.print("\r\n");
            WebClient.print("<!DOCTYPE html><html lang='en'>");
            WebClient.print("<meta name='viewport' content='width=device-width'>");
            // Style/CSS Section
            WebClient.print("<head><meta charset='utf-8'>");
            WebClient.print("<title>Ardu4Weather</title>");
            WebClient.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            WebClient.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            WebClient.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            WebClient.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=sentiment_very_dissatisfied' />");
            WebClient.print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
            WebClient.print("<nav><ul><li><a href='/'>Home</a></li>");
            WebClient.print("<li><a href='/about'>About</a></li>");
            WebClient.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            WebClient.print("<style>");
            WebClient.print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
            WebClient.print("nav { background-color: #333; padding: 10px 0; transition-duration: 0.4s; }");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
            WebClient.print("nav a { color: #ffffff; text-decoration: none; }");
            WebClient.print("nav a:hover { color: #ccc; }");
            WebClient.print("body { font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            WebClient.print(".center-container { display: flex; justify-content: center; align-items: center; flex-grow: 1; }");
            WebClient.print(".background-icon { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); font-size: 1000px; color: #000000; opacity: 0.03; z-index: 0; }");
            WebClient.print("h1 { color: #999999; text-align: center; margin-bottom: 20px; font-size: 70px; }");
            WebClient.print("h2 { color: #999999; text-align: center; margin-bottom: 20px; font-size: 50px; }");
            WebClient.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("* { user-select: none; }");
            WebClient.print("</style></head>");
            WebClient.print("<body><div class='center-container'>");
            WebClient.print("<div>");
            WebClient.print("<h1>Error 404 - Page Not Found</h1>");
            WebClient.print("<h2>The page you're looking for wasn't found on the server</h2>");
            WebClient.print("</div>");
            WebClient.print("</div>");
            WebClient.print("<span class='material-symbols-outlined background-icon'>sentiment_very_dissatisfied</span>");
            WebClient.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
            WebClient.print("</body></html>");
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