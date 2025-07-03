#include "arduino_secrets.h"
#include "a4w_bitmaps.h"
#include "screensavers.h"
#include "websites.h"
#include <math.h> // For math functions like sin()
#include <stdlib.h> // For random number generation
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WiFi.h> // For WiFi communication
#include <AsyncUDP.h> // For UDP network communication
#include <NTPClient.h> // For NTP time sync
#include <time.h>
#include <String.h> // For string type
#include <Wire.h> // For SCL & SDA communication
#include <Adafruit_GFX.h> // For OLED Monitor
#include <Adafruit_SSD1306.h> // For OLED Monitor
#include "DHT.h" // For DHT11 sensor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#define DHTPIN A6 // Digital pin connected to the indoor DHT sensor
#define DHTOUTPIN A7 // Digital pin connected to the outdoor DHT sensor
#define DHTTYPE DHT11 // The type of DHT sensor
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declares the OLED display
AsyncWebServer WebServer(8080); // Defines the port that the web server is hosted on
WiFiClient WebClient; // Declare DataClient globally
AsyncWebServer DataServer(8081); // Defines the port that the data server is hosted on
WiFiClient DataClient; // Declare DataClient globally
WiFiUDP ntpUDP; // A UDP instance to let the NTPClient communicate over
NTPClient timeClient(ntpUDP); // Declare NTPClient object
unsigned long unixTime; // Variable to hold unix time fetched from NTP server
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object for the indoor sensor
DHT dht2(DHTOUTPIN, DHTTYPE); // Create a DHT object for the outdoor sensor
Adafruit_BMP085 bmp; // Create a BMP object for the BMP180 sensor

struct tm timeinfo;
const char* ntpServer = "pool.ntp.org"; // NTP server
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;

// Left Side
const int TempSensor = A0;
const int microphoneSensor = A1;
const int LightSensor = A2;
// DHT1 A6
// DHT2 A7

// Right Side
const int whiteLED = D2; //PWM
const int blueLED = D3; //PWM
const int cyanLED = D4; //PWM
const int greenLED = D5; //PWM
const int yellowLED = D6; //PWM
const int redLED = D7; //PWM
const int magneticSensor = D8;
const int piezo = D9;

const int CORE_0 = 0;
const int CORE_1 = 1;
const int STACK_SIZE_1 = 2048;
const int STACK_SIZE_2 = 4096;
const int STACK_SIZE_3 = 8192;
const int STACK_SIZE_4 = 16384;

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
float temp1 = 0.0;
float hum1 = 0.0;
float temp2 = 0.0;
float hum2 = 0.0;
float pres = 0.0;
float alt = 0.0;
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

TaskHandle_t sensorPollingTask;
TaskHandle_t oledUpdatingTask;
SemaphoreHandle_t i2cMutex;

bool dht_ok = false;
bool dht2_ok = false;
bool bmp_ok = false;

void setup() {
  pinMode(magneticSensor, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(cyanLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(whiteLED, OUTPUT);
  Serial.begin(57600);
  delay(3000); // Wait for Serial to initialize
  Serial.println("\n*************** Ardu4Weather v0.30.0 - Commit 29 *******************");

  // Initialize the OLED display
  display.cp437(true);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {   // Address 0x3C for 128x64
    Serial.println("serialSSD1306 allocation failed");
  }
  display.clearDisplay();
  display.setTextSize(1); 
  display.setTextColor(WHITE);
  display.drawBitmap(0, 0, epd_bitmap_CompositeLogo, 128, 64, WHITE); // Draw the logo while setup is running
  display.display();

  // connect to WiFi
  NetworkChange(); // Connect to WiFi
  if (WiFi.status() != WL_CONNECTED) { // If the WiFi connection fails
    Serial.println("Failed to connect to WiFi");
    serialOutputBuffer[0] = '\0';
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to connect to WiFi");
    display.display();
  }
  strncpy(localIP, WiFi.localIP().toString().c_str(), sizeof(localIP) - 1);
  localIP[sizeof(localIP) - 1] = '\0'; // Ensure null termination
  sprintf(serialOutputBuffer, "Connected to WiFi.\nIP address: %s", localIP);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0';
  // initalise web server
  WebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Main Page Requested");
    whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
    analogWrite(whiteLED, whiteLightness); // Indicate that data transfer has started
    screensaverActive = 0; // Disable screensaver
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    mainPage(
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
      hoursOnline,
      response
    ); // Call function, passing the stream object
    request->send(response); // Send the streamed response
    Serial.println("Streamed response sent");
    analogWrite(whiteLED, LOW);
  });
  WebServer.on("/about", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Root page requested (Stream)");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    aboutPage(serialOutputBuffer, Cblue, Creset, response); // Call function, passing the stream object
    request->send(response); // Send the streamed response
    Serial.println("Streamed response sent");
    analogWrite(whiteLED, LOW);
  });
  WebServer.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Root page requested (Stream)");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    dataPage(serialOutputBuffer, Cblue, Creset, response); // Call function, passing the stream object
    request->send(response); // Send the streamed response
    Serial.println("Streamed response sent");
    analogWrite(whiteLED, LOW);
  });
  WebServer.on("/aprocryphan", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Root page requested (Stream)");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    aprocryphanPage(serialOutputBuffer, Cblue, Creset, response); // Call function, passing the stream object
    request->send(response); // Send the streamed response
    Serial.println("Streamed response sent");
    analogWrite(whiteLED, LOW);
  });
  WebServer.on("/web1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Root page requested (Stream)");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    web1Page(response); // Call function, passing the stream object
    request->send(response); // Send the streamed response
    Serial.println("Streamed response sent");
    analogWrite(whiteLED, LOW);
  });
  WebServer.onNotFound([](AsyncWebServerRequest *request){
    Serial.print("NOT_FOUND: ");
    if(request->method() == HTTP_GET) Serial.print("GET");
    else if(request->method() == HTTP_POST) Serial.print("POST");
    Serial.print(" ");
    Serial.println(request->url());
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->setCode(404); // Set the HTTP status code to 404
    errPage(serialOutputBuffer, Cyellow, Creset, response); // Call your function to print the HTML to the stream
    request->send(response); // Send the streamed response
    analogWrite(whiteLED, LOW);
  });
  WebServer.begin(); // Start website hosting server, port 8080
  DataServer.begin(); // Start data sending server, port 8081

  // initialize DHT sensors
  dht.begin();
  dht2.begin();
  if (!isnan(dht.readTemperature())) {
    dht_ok = true;
  }
  if (!isnan(dht2.readTemperature())) {
    dht2_ok = true;
  }
  if (isnan(dht.readTemperature()) || isnan(dht2.readTemperature())) { // If the DHT sensors fail to initialize
    Serial.println("Failed to initialize DHT sensors");
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Failed to initialize DHT sensors");
    display.display();
  }

  // initialize RTC
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // initialize BMP sensor
  if (bmp.begin()) {
    bmp_ok = true;
  } else {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!"); 
    display.clearDisplay();
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    display.display();
  }

  // Reset reason
  disableLoopWDT();
  esp_reset_reason_t reason = esp_reset_reason();
  Serial.print("Reset reason: ");
  switch (reason) {
    case ESP_RST_UNKNOWN:    Serial.println("Unknown"); break;
    case ESP_RST_POWERON:    Serial.println("Power on"); break;
    case ESP_RST_EXT:        Serial.println("External reset"); break;
    case ESP_RST_SW:         Serial.println("Software reset"); break;
    case ESP_RST_PANIC:      Serial.println("Panic/exception"); break; // Illegal instruction or illegal memory access or stack overflow
    case ESP_RST_INT_WDT:    Serial.println("Interrupt watchdog"); break; // IWDT
    case ESP_RST_TASK_WDT:   Serial.println("Task watchdog"); break;     // TWDT
    case ESP_RST_WDT:        Serial.println("Other watchdog"); break;
    case ESP_RST_DEEPSLEEP:  Serial.println("Deep sleep wakeup"); break;
    case ESP_RST_BROWNOUT:   Serial.println("Brownout"); break;          // Power issue!
    case ESP_RST_SDIO:       Serial.println("SDIO"); break;
    default:                 Serial.println("Other"); break;
  }

  //RandomStaticLoad(); // Load a random static image onto the LED Matrix
  Serial.println("Setup Complete");

  // for screensavers
  randomSeed(analogRead(A7));
  lastEffectChangeTime = millis();
  display.clearDisplay();

  i2cMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(sensorPoll, "Sensor Polling Task", STACK_SIZE_4, NULL, 2, &sensorPollingTask, CORE_0);
  xTaskCreatePinnedToCore(oledFunctions, "OLED Updating Task", STACK_SIZE_4, NULL, 2, &oledUpdatingTask, CORE_0);
}

// NetworkChange, If the network disconnects, it reconnects to another predefined network
void NetworkChange() {
  while (WiFi.status() != WL_CONNECTED) {
    network = (network + 1) % 3;
    switch(network) {
      case 0:
        WiFi.begin(SECRET_SSID, SECRET_OPTIONAL_PASS);
        Serial.println("Connected To Network 1.");
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
        Serial.println("Connected To Network 2.");
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

void printLocalTime() {
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%B %d %Y %H:%M:%S");
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
  display.print("1/4");
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
  display.print("2/4");
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
  display.print("3/4");
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

void OLEDPanel4() {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("4/4");
  display.setCursor(0, 16);
  display.println("Ardu4Weather v0.30.0");
  display.setCursor(0, 24);
  display.println("DHT: " + String(dht_ok ? "OK" : "Failed"));
  display.setCursor(0, 32);
  display.println("DHT2: " + String(dht2_ok ? "OK" : "Failed"));
  display.setCursor(0, 40);
  display.println("BMP: " + String(bmp_ok ? "OK" : "Failed"));
  display.setCursor(0, 48);
  display.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "Failed"));
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

void sensorPoll(void *pvParameters) {
  while (true) {
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(1000))) {
      if (dht_ok) {
      temp1 = dht.readTemperature();
      hum1 = dht.readHumidity();
      }
      if (dht2_ok) {
      temp2 = dht2.readTemperature();
      hum2 = dht2.readHumidity();
      }
      if (bmp_ok) {
      pres = bmp.readPressure();
      alt = bmp.readAltitude(seaLevelPressure);
      }
      formattedC = temp1;
      formattedOutC = temp2;
      formattedF = (temp1 * 9.0/5.0) + 32.0;
      formattedOutF = (temp2 * 9.0/5.0) + 32.0;
      formattedHumiditySensor = hum1;
      formattedOutHumiditySensor = hum2;
      pressure = pres;
      formattedPressureSensor = pres / 100;
      altitude = alt;
      pressureDisplacement = constrain((map((pres / 100), 980, 1030, 0, 100)), 0, 100);
      altitudeDisplacement = constrain((map(alt, 0, 3000, 0, 100)), 0, 100);

      xSemaphoreGive(i2cMutex);
    }
    formattedLightSensorData = analogRead(LightSensor) * 2;
    formattedMicrophoneSensor = MicLevels();
    formattedMagnetSensor = (digitalRead(magneticSensor) == 1 ? 0 : 1);
    signalStrength = WiFi.RSSI();
    inTempDisplacement = constrain((map(formattedC, -5, 30, 0, 100)), 0, 100);
    outTempDisplacement = constrain((map(formattedOutC, -5, 30, 0, 100)), 0, 100);
    lightDisplacement = constrain((map((analogRead(LightSensor) * 2), 0, 1000, 0, 100)), 0, 100);
    inHumidityDisplacement = constrain((map(formattedHumiditySensor, 0, 100, 0, 100)), 0, 100);
    outHumidityDisplacement = constrain((map(formattedOutHumiditySensor, 0, 100, 0, 100)), 0, 100);
    noiseDisplacement = constrain((map((MicLevels()), 10, 700, 0, 100)), 0, 100);
    if (isnan(formattedOutC)) {
      outTempDisplacement = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
    taskYIELD();
  }
}

void oledFunctions(void *pvParameters) {
  int secondsCounter = 0;
  const int panelSwitchIntervalSeconds = 5;
  const int screensaverCheckIntervalSeconds = 300; // 5 minutes
  while (true) {
    secondsCounter++;
    if (secondsCounter % screensaverCheckIntervalSeconds == 0) {
      screensaverActive = (screensaverActive == 1) ? 0 : ((random(0, 4) == 1) ? 1 : screensaverActive);
    }
    if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
      display.clearDisplay();
      if (screensaverActive == 0) {
        if (secondsCounter % panelSwitchIntervalSeconds == 0) {
          OLEDPanel = (OLEDPanel + 1) % 4;
        }
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
        case 3:
          OLEDPanel4();
          break;
        }
      } else {
      screensavers();
      }
    display.display();
    }
    xSemaphoreGive(i2cMutex);
    vTaskDelay(pdMS_TO_TICKS(1000));
    taskYIELD(); // Yield to other tasks
  }
}

  /* CSV Server Data Function, When a connection is made, data is sent. a partner python script saves the data to a CSV file.
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
  }*/

int DebugMillis = 0;
void loop() {
  //DebugMillis = millis(); // Reset the debug timer
  // Put continuous updates here
  strftime(dateOnly, sizeof(dateOnly), "%d-%m-%Y", &timeinfo);
  strftime(timeOnly, sizeof(timeOnly), "%H:%M:%S", &timeinfo);
  sprintf(formattedTime, "%s / %s", dateOnly, timeOnly);
  LiveThermomiter();
  NetworkChange();
  //DeltaPressure24();
  //float DP24 = DeltaPressure24();

  // Time and Date Data
  secondsOnline = millis() / 1000;
  sprintf(hoursOnline, "%.2f", (millis() / 3600000.0));
  sprintf(daysOnline, "%.2f", (millis() / 86400000.0));
  vTaskDelay(pdMS_TO_TICKS(1000));
  taskYIELD(); // Yield to other tasks
}