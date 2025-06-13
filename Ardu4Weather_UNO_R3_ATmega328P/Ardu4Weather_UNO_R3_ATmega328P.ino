#include "arduino_secrets.h"
#include "a4w_bitmaps.h"
#include "screensavers.h"
#include <math.h> // For math functions like sin()
#include <stdlib.h> // For random number generation
#include <EEPROM.h> // Persistant value storage 1Kb max
#include <Adafruit_GFX.h> // For OLED Monitor
#include <Adafruit_SSD1306.h> // For OLED Monitor
#include "DHT.h" // For DHT11 sensor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#define DHTPIN 7 // Digital pin connected to the indoor DHT sensor
#define DHTOUTPIN 8 // Digital pin connected to the outdoor DHT sensor
#define DHTTYPE DHT11 // The type of DHT sensor
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // Declares the OLED display
DHT dht(DHTPIN, DHTTYPE); // Create a DHT object for the indoor sensor
DHT dht2(DHTOUTPIN, DHTTYPE); // Create a DHT object for the outdoor sensor
Adafruit_BMP085 bmp; // Create a BMP object for the BMP180 sensor
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
int OLEDPanel = 0; // Initial OLED Panel
int readingIndex = 0; // Keep track of the current position in the array
int SensorPreviousMillis = 0;
int formattedLightSensorData = 0;
int formattedMicrophoneSensor = 0;
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
char hoursOnline[10] = "null"; // XX.XX\0
char daysOnline[10] = "null"; // XX.XX\0
char dateOnly[11] = "null"; // XX-XX-XX\0
char timeOnly[9] = "null"; // XX:XX:XX\0
char formattedTime[23] = "null"; // XX-XX-XX / XX:XX:XX\0


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

  sprintf(serialOutputBuffer, "%sSetup Complete%s", Cgreen, Creset);
  Serial.println(serialOutputBuffer);
  serialOutputBuffer[0] = '\0'; // Clear the serial output buffer by setting the first character to null

  // for screensavers
  randomSeed(analogRead(0));
  lastEffectChangeTime = millis();

  display.clearDisplay();
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
void OLEDPanel3() {
  display.setTextSize(1);
  display.setCursor(110, 16);
  display.print("3/3");
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
  //DeltaPressure24();
  //float DP24 = DeltaPressure24();

  // Time Data
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
        OLEDPanel3();
        break;
    }
    display.display(); // Display everything held in buffer
  } else {
    screensavers();
    display.display();
  }
  //Serial.println(millis() - DebugMillis); // Debugging
}