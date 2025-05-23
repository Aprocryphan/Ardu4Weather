#include "thing_properties.h"
#include "arduino_secrets.h"
#include "a4w_bitmaps.h"
#include "ansi_codes.h"
#include <WDT.h> // Watchdog Timer
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
IPAddress local_IP(192, 168, 1, 106); // Redundant
IPAddress gateway(192, 168, 1, 1); // Redundant
IPAddress subnet(255, 255, 255, 0); // Redundant
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

float offset = 0; // Offset For Temp Sensor (adjust as needed)
float gain = 1.00; // Gain For Temp Sensor (adjust as needed)
float Temp = 0.0;
float TempAvarage = 0.0; // Stores average temperature
float LightAvarage = 0.0; // Stores average light level
const float seaLevelPressure = 101325;
int interval = 300000; // Referesh interval for NTPUpdate function
long long unsigned currentMillis = 0; // for all millis functions
long long unsigned previousMillis = 0; // for NTPUpdate function
int network = -1; // Initial network connection attempt for switch case
long long unsigned OLEDPreviousMillis = 0; // For OLED Panel
int OLEDInterval = 5000; // For OLED Panel
int OLEDPanel = 0; // Initial OLED Panel
const int sampleWindow = 50;  // Sample window width in mS (50 mS = 20Hz)
unsigned int sample; // Sample value from the pressure sensor
const int NUM_READINGS = 288; // 24 hours of data, one reading every 5 minutes
float pressureReadings[NUM_READINGS]; // Array to store pressure readings
int readingIndex = 0; // Keep track of the current position in the array
long long unsigned DPPreviousMillis = 0;
const int DPInterval = 500000;
float deltaP = 0.0;

// Initialisation of string variables used later, Commented variables are handled by cloud.
String formattedTime = "null";
String dateOnly = "null";
String timeOnly = "null";
//String formattedC = "null";
String formattedLightSensorData = "null";
//String formattedHumdiditySensor = "null";
//String formattedPressureSensor = "null";
String formattedMicrophoneSensor = "null";
String secondsOnline = "null";
String hoursOnline = "null";
String daysOnline= "null";
String localIP = "null";
String subnetMask = "null";
String gatewayIP = "null";
String signalStrength = "null";
//String formattedOutC = "null";
//String formattedOutHumdiditySensor = "null";
String formattedMagnetSensor = "null";
String altitude = "null";
String NTPIP = "null";
String url = "";
String referrer = "";
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
  Serial.println("\n*************** Ardu4Weather v0.25.3 - Commit 18 *******************");

  // Initialize the OLED display
  display.cp437(true);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // Address 0x3C for 128x64
    Serial.println(Cred + "SSD1306 allocation failed" + Creset);
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.drawBitmap(0, 0, epd_bitmap_CompositeLogo, 128, 64, WHITE); // Draw the logo while setup is running
  display.display();

  NetworkChange(); // Connect to WiFi
  if (WiFi.status() != WL_CONNECTED) { // If the WiFi connection fails
    Serial.println(Cred + "Failed to connect to WiFi" + Creset);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to connect to WiFi");
    display.display();
  }
  Serial.print(Cgreen + "Connected to WiFi. IP address: " + Creset);
  String localIP = WiFi.localIP().toString();
  Serial.println(localIP);
  WebServer.begin(); // Start website hosting server, port 8080
  DataServer.begin(); // Start data sending server, port 8081

  // initialize DHT sensors
  dht.begin();
  dht2.begin();
  if (isnan(dht.readTemperature()) || isnan(dht2.readTemperature())) { // If the DHT sensors fail to initialize
    Serial.println(Cred + "Failed to initialize DHT sensors" + Creset);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to initialize DHT sensors");
    display.display();
  }

  // initialize RTC
  RTC.begin();
  if (!RTC.begin()) {
    Serial.println(Cred + "Failed to initialize RTC" + Creset);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to initialize RTC");
    display.display();
    for (;;); // Don't proceed, loop forever
  }
  timeClient.begin();
  timeClient.update();
  // NTPIP = String(timeClient.getNTPserverIP());
  NTPIP = String("null"); // Set to null, as the NTP server IP is not used yet
  unixTime = timeClient.getEpochTime();
  while (unixTime < 1000)
  { // If the unix time is less than 1000, it's not a valid time
    timeClient.update();
    unixTime = timeClient.getEpochTime();
    Serial.println(Cred + "Failed to get proper unix time, refreshing." + Creset);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to get proper unix time, refreshing.");
    display.display();
    delay(200);
  }
  Serial.print(Cblue + "Unix Time: " + Creset);
  Serial.println(unixTime);
  RTCTime timeToSet = RTCTime(unixTime);
  RTC.setTime(timeToSet);
  RTCTime currentTime;
  RTC.getTime(currentTime);
  Serial.print(Cblue + "RTC Time: " + Creset);
  Serial.println(currentTime);

  // initialize LED Matrix
  matrix.begin();

  // initialize BMP sensor
  if (!bmp.begin()) { // If the BMP sensor fails to initialize
    Serial.println(Cred + "Could not find a valid BMP085/BMP180 sensor, check wiring!" + Creset);
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    display.display();
    for (;;); // Don't proceed, loop forever
  }

  //RandomStaticLoad(); // Load a random static image onto the LED Matrix
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection); // Connect to arduino cloud
  setDebugMessageLevel(2); // Set debug verbosity 0-4
  ArduinoCloud.printDebugInfo(); // Print to cloud console
  Serial.println(Cgreen + "Setup Complete" + Creset);
  display.clearDisplay();
}

// NetworkChange, If the network disconnects, it reconnects to another predefined network
void NetworkChange() {
  while (WiFi.status() != WL_CONNECTED) {
    network = (network + 1) % 3;
    switch(network) {
      case 0:
        WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);
        Serial.println(Cblue + "Changed to network 1." + Creset);
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xAE);
        display.display();
        delay(100);
        break;
      case 1:
        WiFi.begin(SECRET_SSID_2, SECRET_OPTIONAL_PASS_2);
        Serial.println(Cblue + "Changed to network 2." + Creset);
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xAF);
        display.display();
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

// LiveThermomiterNew, Doesn't work currently
void LiveThermomiterNew() {
  float currentTemp = dht.readTemperature();
  static int redIntensity = 0;
  static int yellowIntensity = 0;
  static int greenIntensity = 0;
  static int cyanIntensity = 0;
  static int blueIntensity = 0;
  if (currentTemp >= 25) {
    redIntensity = 100; 
    yellowIntensity = 0;
    greenIntensity = 0;
    cyanIntensity = 0;
    blueIntensity = 0;
  } else if (currentTemp >= 22 && currentTemp < 25) { 
    redIntensity = map(currentTemp, 22, 25, 100, 0);
    yellowIntensity = map(currentTemp, 22, 25, 0, 100);
    greenIntensity = 0;
    cyanIntensity = 0;
    blueIntensity = 0;
  } else if (currentTemp >= 18 && currentTemp < 22) { 
    redIntensity = 0;
    yellowIntensity = map(currentTemp, 18, 22, 100, 0);
    greenIntensity = map(currentTemp, 18, 22, 0, 100);
    cyanIntensity = 0;
    blueIntensity = 0;
  } else if (currentTemp >= 10 && currentTemp < 18) { 
    redIntensity = 0;
    yellowIntensity = 0;
    greenIntensity = map(currentTemp, 10, 18, 100, 0);
    cyanIntensity = map(currentTemp, 10, 18, 0, 100);
    blueIntensity = 0;
  } else if (currentTemp <= 10) { 
    redIntensity = 0;
    yellowIntensity = 0;
    greenIntensity = 0;
    cyanIntensity = map(currentTemp, 0, 10, 100, 0);
    blueIntensity = map(currentTemp, 0, 10, 0, 100); 
  }
  const int fadeStep = 5; // Adjust for faster/slower fading
  redIntensity = constrain(redIntensity, redIntensity - fadeStep, redIntensity + fadeStep);
  yellowIntensity = constrain(yellowIntensity, yellowIntensity - fadeStep, yellowIntensity + fadeStep);
  greenIntensity = constrain(greenIntensity, greenIntensity - fadeStep, greenIntensity + fadeStep);
  cyanIntensity = constrain(cyanIntensity, cyanIntensity - fadeStep, cyanIntensity + fadeStep);
  blueIntensity = constrain(blueIntensity, blueIntensity - fadeStep, blueIntensity + fadeStep);
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
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (timeClient.update()) { // If the NTP update is successful
      int whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
      analogWrite(whiteLED, whiteLightness);
      unixTime = timeClient.getEpochTime();
      Serial.print(Cblue + "Unix Time: " + Creset);
      Serial.println(unixTime);
      Serial.println(Cgreen + "NTP Time Synced" + Creset);
      RTCTime timeToSet = RTCTime(unixTime);
      RTC.setTime(timeToSet);
    } else {
      Serial.println(Cred + "NTP Update Failed" + Creset);
    }
  }
  analogWrite(whiteLED, LOW);
}

float DeltaPressure24() {
  if (currentMillis - DPPreviousMillis >= DPInterval) {
    previousMillis = currentMillis;
    float currentPressure = bmp.readPressure(); // Get current pressure reading
    pressureReadings[readingIndex] = currentPressure;   // Store the new reading in the array
    readingIndex = (readingIndex + 1) % NUM_READINGS; // Wrap around the array
    int oldestIndex = (readingIndex + NUM_READINGS - 1) % NUM_READINGS; // Index of the oldest reading
    float deltaP = currentPressure - pressureReadings[oldestIndex]; // Calculate the change in pressure
  }
  return deltaP;
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
void OLEDPanel1(String formattedC, String formattedLightSensorData, String formattedHumdiditySensor, String formattedPressureSensor, String formattedMicrophoneSensor, String hoursOnline) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("1/3");
  display.setCursor(0, 16); 
  display.print("Temp: " + formattedC);
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  display.println("Bright: " + formattedLightSensorData + " Lm");
  display.setCursor(0, 32); 
  display.println("Humid: " + formattedHumdiditySensor + "%");
  display.setCursor(0, 40); 
  display.println("Pressure: " + formattedPressureSensor + " mBar");
  display.setCursor(0, 48); 
  display.println("Noise: " + formattedMicrophoneSensor + " U");
  display.setCursor(0, 56); 
  display.println("Hours: " + hoursOnline + "H");
}

// OLEDPanel2, Shows a few different more readings that couldnt fit on 1 from the sensors without having to access the website, useful for debugging.
void OLEDPanel2(String formattedOutC, String formattedOutHumdiditySensor, String formattedMagnetSensor, String altitude, String daysOnline) {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("2/3");
  display.setCursor(0, 16); 
  display.print("OTemp: " + formattedOutC);
  display.write(0xF8);
  display.print("C");
  display.setCursor(0, 24); 
  display.println("OHumid: " + formattedOutHumdiditySensor + "%");
  display.setCursor(0, 32); 
  display.println("Magnet: " + formattedMagnetSensor);
  display.setCursor(0, 40); 
  display.println("Altitude: " + altitude + "m");
  display.setCursor(0, 48); 
  display.println("Days: " + daysOnline + "D");
  display.setCursor(0, 56); 
}

// OLEDPanel3, Shows a few different network statistics, useful for debugging
void OLEDPanel3(String localIP, String subnetMask, String gatewayIP, String NTPIP, String signalStrength, int previousMillis, float DP24) {
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
  //display.println("NTP UP: " + previousMillis);
  display.println("DP: " + String(DP24));
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
  //DeltaPressure24();
  //float DP24 = DeltaPressure24();
  float DP24 = 0.0;
  ArduinoCloud.update(); // causes arduino to hang when website requested
  currentMillis = millis();

  RTCTime currentTime;
  RTC.getTime(currentTime);
  String stringTime = String(currentTime);
  int tPosition = stringTime.indexOf('T');
  timeOnly = stringTime.substring(tPosition + 1);
  dateOnly = stringTime.substring(0, tPosition);
  formattedTime = String(dateOnly + " / " + timeOnly);
  formattedC = String(dht.readTemperature()); 
  String formattedF = String((dht.readTemperature() * 9/5) + 32);
  formattedOutC = String(dht2.readTemperature()); 
  String formattedOutF = String((dht2.readTemperature() * 9/5) + 32);
  formattedLightSensorData = String((analogRead(LightSensor) * 2)); 
  formattedHumdiditySensor = String(dht.readHumidity());
  formattedOutHumdiditySensor = String(dht2.readHumidity());
  float pressure = bmp.readPressure();
  formattedPressureSensor = String(pressure / 100);
  formattedMicrophoneSensor = String(MicLevels());
  formattedMagnetSensor = String(digitalRead(magneticSensor) == 1 ? 0 : 1);
  altitude = String(bmp.readAltitude(seaLevelPressure));
  String secondsOnline = String((millis() / 1000));
  String hoursOnline = String(float(millis() / 3600000.0));
  String daysOnline = String(float(millis() / 86400000.0));
  String inTempDisplacement = String(constrain((map(dht.readTemperature(),-5,30,-50,13300)),-50,13300)); // Displacement for the indicators on the bars, maps and constrains input between -50 and 13300
  String outTempDisplacement = String(constrain((map(dht2.readTemperature(),-5,30,-50,13300)),-50,13300));
  String lightDisplacement = String(constrain((map((analogRead(LightSensor) * 2),0,1000,-50,13300)),-50,13300));
  String inHumidityDisplacement = String(constrain((map(dht.readHumidity(),0,100,-50,13300)),-50,1330));
  String outHumidityDisplacement = String(constrain((map(dht2.readHumidity(),0,100,-50,13300)),-50,13300));
  String pressureDisplacement = String(constrain((map((pressure / 100),980,1030,-50,13300)),-50,13300));
  String altitudeDisplacement = String(constrain((map(bmp.readAltitude(seaLevelPressure),0,3000,-50,13300)),-50,13300));
  String noiseDisplacement = String(constrain((map((MicLevels()),10,700,-50,13300)),-50,13300));
  localIP = WiFi.localIP().toString();
  subnetMask = WiFi.subnetMask().toString();
  gatewayIP = WiFi.gatewayIP().toString();
  signalStrength = String(WiFi.RSSI());

  // OLED Data Display Function
  if (currentMillis - OLEDPreviousMillis >= OLEDInterval) {
    OLEDPreviousMillis = currentMillis;
    OLEDPanel = (OLEDPanel + 1) % 3;
  }
  display.clearDisplay();
  OLEDHeader(dateOnly, timeOnly);
  switch (OLEDPanel) {
    case 0:
      OLEDPanel1(formattedC, formattedLightSensorData, formattedHumdiditySensor, formattedPressureSensor, formattedMicrophoneSensor, hoursOnline);
      break;
    case 1:
      OLEDPanel2(formattedOutC, formattedOutHumdiditySensor, formattedMagnetSensor, altitude, daysOnline);
      break;
    case 2:
      OLEDPanel3(localIP, subnetMask, gatewayIP, NTPIP, signalStrength, previousMillis, DP24);
      break;
  }
  display.display(); // Display everything held in buffer

  // CSV Server Data Function, When a connection is made, data is sent. a partner python script saves the data to a CSV file.
  WiFiClient DataClient = DataServer.available(); // Check for incoming connections
  if (DataClient) { // If a client connects
    Serial.println(Cblue + "New Data Client." + Creset);
    while (DataClient.connected()) {
      if (DataClient.available()) {
        String request = DataClient.readStringUntil('\n');
        Serial.println(Cblue + "Request received: " + request + Creset);
        if (request == "REQUEST_DATA") {
          String data = "<START>" + dateOnly + "," + timeOnly + "," + formattedC + "," + formattedOutC + "," + formattedLightSensorData + "," + formattedHumdiditySensor + "," + formattedOutHumdiditySensor + "," + formattedPressureSensor + "," + formattedMicrophoneSensor + "," + String(secondsOnline) + "<END>";
          DataClient.print(data);
          Serial.print(Cgreen + "Data sent to client: " + data + Creset);  // Debug statement
          break;  // Exit the loop after sending the data once
        }
      }
    }
    Serial.println(Cyellow + "Data Client Disconnected." + Creset);
  }
  
  // Website Function
  WiFiClient WebClient = WebServer.available(); // Check for incoming connections
  if (WebClient) { // If a client connects
    Serial.println(Cblue + "New Web Client." + Creset);
    while (WebClient.connected()) { // Keep connection open until client disconnects
      if (WebClient.available()) {
        int whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
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
          Serial.print(Cblue + "URL :");
          Serial.println(url + Creset);
          if (url == "/") {
            Serial.println(Cblue + "Main Page Requested" + Creset);
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
            WebClient.print("nav { background-color: #434b4f; padding: 10px 0; transition-duration: 0.4s; font-size: 16px;}");
            WebClient.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            WebClient.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
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
            WebClient.print(".in-temp-indicator { position: absolute; transform: translate(");
            WebClient.print(inTempDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".out-temp-indicator { position: absolute; transform: translate(");
            WebClient.print(outTempDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".light-indicator { position: absolute; transform: translate(");
            WebClient.print(lightDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".in-humidity-indicator { position: absolute; transform: translate(");
            WebClient.print(inHumidityDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".out-humidity-indicator { position: absolute; transform: translate(");
            WebClient.print(outHumidityDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".pressure-indicator { position: absolute; transform: translate(");
            WebClient.print(pressureDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".altitude-indicator { position: absolute; transform: translate(");
            WebClient.print(altitudeDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".noise-indicator { position: absolute; transform: translate(");
            WebClient.print(noiseDisplacement);
            WebClient.print("%, -40%); width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            WebClient.print(".footer-image img { max-width: 7%; height: auto; display: block; margin: 20px auto; }");
            WebClient.print("button { background-color: #ffffff; color: #84a8b2; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transition-duration: 0.4s; }");
            WebClient.print("button:hover { background-color: #84a8b2; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ }");
            WebClient.print("button:active { background-color: #7293A1; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transform: translateY(2px);}");
            WebClient.print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24; height: auto; display: block; margin: 20px auto;}");
            WebClient.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            WebClient.print("</style></head>");
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
            WebClient.print("<div class='slider-bar'><div class='in-temp-indicator'></div></div>");
            WebClient.print("<span class='slider-text right'>30&deg;C</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Outdoor Temperature:</span><span>");
            WebClient.print(formattedOutC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedOutF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text left'>-5&deg;C</span>");
            WebClient.print("<div class='slider-bar'><div class='out-temp-indicator'></div></div>");
            WebClient.print("<span class='slider-text right'>30&deg;C</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text light-left'>0 Lm</span>");
            WebClient.print("<div class='light-slider-bar'><div class='light-indicator'></div></div>");
            WebClient.print("<span class='slider-text light-right'>1000 Lm</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Indoor Humidity:</span><span>");
            WebClient.print(formattedHumdiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text humid-left'>0%</span>");
            WebClient.print("<div class='humid-slider-bar'><div class='in-humidity-indicator'></div></div>");
            WebClient.print("<span class='slider-text humid-right'>100%</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Outdoor Humidity:</span><span>");
            WebClient.print(formattedOutHumdiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text humid-left'>0%</span>");
            WebClient.print("<div class='humid-slider-bar'><div class='out-humidity-indicator'></div></div>");
            WebClient.print("<span class='slider-text humid-right'>100%</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text left'>980 mBar</span>");
            WebClient.print("<div class='slider-bar'><div class='pressure-indicator'></div></div>");
            WebClient.print("<span class='slider-text right'>1030 mBar</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Approx Altitude:</span><span>");
            WebClient.print(altitude);
            WebClient.print(" m</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text altitude-left'>0 m</span>");
            WebClient.print("<div class='altitude-slider-bar'><div class='altitude-indicator'></div></div>");
            WebClient.print("<span class='slider-text altitude-right'>3000 m</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" Units</span></div>");
            WebClient.print("<div class='slider-container'>");
            WebClient.print("<span class='slider-text noise-left'>10 U</span>");
            WebClient.print("<div class='noise-slider-bar'><div class='noise-indicator'></div></div>");
            WebClient.print("<span class='slider-text noise-right'>700 U</span> ");
            WebClient.print("</div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Presence Of Strong Magnetic Field:</span><span>");
            WebClient.print(formattedMagnetSensor);
            WebClient.print("</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Online For:</span><span>");
            WebClient.print(secondsOnline);
            WebClient.print(" Seconds</span></div>");
            WebClient.print("<div><button id='toggleButton'>Show Averages</button></div>");
            // Hidden Avarage Data, 1H
            WebClient.print("<div id='1HaverageData' style='display: none;'><h1>1 Hour Rolling Avarage</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumdiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            // Hidden Avarage Data, 12H
            WebClient.print("<div id='12HaverageData' style='display: none;'><h1>12 Hour Rolling Avarage</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumdiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            // Hidden Avarage Data, 24H
            WebClient.print("<div id='24HaverageData' style='display: none;'><h1>24 Hour Rolling Avarage</h1>");
            WebClient.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            WebClient.print(formattedC);
            WebClient.print("&deg;C / ");
            WebClient.print(formattedF);
            WebClient.print("&deg;F</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            WebClient.print(formattedLightSensorData);
            WebClient.print(" Lumens</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            WebClient.print(formattedHumdiditySensor);
            WebClient.print("%</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            WebClient.print(formattedPressureSensor);
            WebClient.print(" mBar</span></div>");
            WebClient.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            WebClient.print(formattedMicrophoneSensor);
            WebClient.print(" dB</span></div>");
            WebClient.print("</div>");
            WebClient.print("<body><div class='sidebar-container'><h1>Side Container</h1>");
            WebClient.print("</div>");
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
            WebClient.print("<div class='footer-image'><img src='https://i.imgur.com/mlL3Fiw.png' alt='Ardu4Weather Logo'></div></div>");
            WebClient.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS<br>");
            WebClient.print("<a href='https://github.com/Aprocryphan/Ardu4Weather' style='text-decoration: underline; display: flex; align-items: center;' target='_blank'>");
            WebClient.print("<img src='https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png' alt='GitHub Logo' style='width: 20px; height: 20px; margin-right: 5px;'>");
            WebClient.print("GitHub</a></p></footer>");
            WebClient.print("</body></html>");
            WebClient.flush();
            break;
          } else if (url == "/about") {
              Serial.println(Cblue + "About Page Requested" + Creset);
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
              Serial.println(Cblue + "Data Page Requested" + Creset);
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
              Serial.println(Cblue + "About Apro Page Requested" + Creset);
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
          } else { // Serve error 404 page
              Serial.println(Cyellow + "Error 404" + Creset);
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
    Serial.println(Cyellow + "Web Client disconnected." + Creset);
    analogWrite(whiteLED, LOW);
    url = ""; // Empty out for next connection request
    referrer = ""; // Empty out for next connection request
  }
}