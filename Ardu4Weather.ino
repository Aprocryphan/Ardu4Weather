#include <WiFi.h>
#include "RTC.h"
#include <EEPROM.h> // Persistant value storage 1Kb max
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <String.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
//#include <ArduinoIoTCloud.h>
//#include <Arduino_ConnectionHandler.h>
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#define DHTPIN 7 // Digital pin connected to the DHT sensor
#define DHTOUTPIN 8
#define DHTTYPE DHT11 // DHT 11
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const char* ssid = "9D5D Hyperoptic Fibre Broadband";
const char* ssid2 = "Hd6qIo3iiR_2.4GHz";
const char* password = "9hRWzQX2MFFd";
const char* password2 = "04162004";
IPAddress local_IP(192, 168, 1, 100); // Replace with your desired static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(8080); //8080
WiFiUDP ntpUDP; // A UDP instance to let the NTPClient communicate over
NTPClient timeClient(ntpUDP); // Declare NTPClient object
unsigned long unixTime;
DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTOUTPIN, DHTTYPE);
Adafruit_BMP085 bmp;
ArduinoLEDMatrix matrix;

const int magneticSensor = 2;
const int redLED = 3; //PWM
const int piezo = 4;
const int yellowLED = 5; //PWM
const int greenLED = 6; //PWM
const int cyanLED = 9; //PWM
const int blueLED = 10; //PWM
const int whiteLED = 11; //PWM

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
int interval = 300000; // NTP
int currentMillis = 0;
int previousMillis = 0;
int network = 0;
int OLEDCurrentMillis = 0;
int OLEDPreviousMillis = 0;
int OLEDInterval = 5000;
int OLEDPanel = 0;
String formattedTime = "null";
String dateOnly = "null";
String timeOnly = "null";
String formattedC = "null";
String formattedLightSensorData = "null";
String formattedHumdiditySensor = "null";
String formattedPressureSensor = "null";
String formattedMicrophoneSensor = "null";
String timeOnline = "null";
String localIP = "null";
String subnetMask = "null";
String gatewayIP = "null";
String signalStrength = "null";
String formattedOutC = "null";
String formattedOutHumdiditySensor = "null";
String formattedMagnetSensor = "null";
String altitude = "null";
String url = "";
String refferer = "";
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
        delay(1000);
        break;
      case 1:
        WiFi.begin(ssid2, password2);
        Serial.println("Changed to network 2.");
        display.setTextSize(2);
        display.setCursor(0, 30);
        display.write(0xEF);
        delay(1000);
        break;
    }
  }
}

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
  server.begin();
  dht.begin();
  dht2.begin();
  RTC.begin();
  matrix.begin();
  timeClient.begin();
  timeClient.update();
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
  RandomStaticLoad();
}

const int StaticAnimationSelection = random(0,12);
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

float ReadTempC() {
  int SensorValue = analogRead(TempSensor);
  float Voltage = SensorValue * (5.0 / 1023.0); // Assuming 5V reference
  float Temp = (Voltage - 0.5) * 100.0 * gain + offset;
  return Temp;
}

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

void OLEDPanel1(String formattedC, String formattedLightSensorData, String formattedHumdiditySensor, String formattedPressureSensor, String formattedMicrophoneSensor, String timeOnline) {
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
  display.println("Noise: " + formattedMicrophoneSensor + " dB");
  display.setCursor(0, 56); 
  display.println("Time: " + timeOnline + " Secs");
}

void OLEDPanel2(String formattedOutC, String formattedOutHumdiditySensor, String formattedMagnetSensor, String altitude) {
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
  display.setCursor(0, 56); 
}

void OLEDPanel3(String localIP, String subnetMask, String gatewayIP, String signalStrength) {
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
  display.println("Strength: " + signalStrength);
  display.setCursor(0, 48);
}

void loop() {
  // Put continuous updates here
  LiveThermomiter();
  NetworkChange();
  NTPSync();
  RTCTime currentTime;
  RTC.getTime(currentTime);
  String stringTime = String(currentTime);
  int tPosition = stringTime.indexOf('T');
  String timeOnly = stringTime.substring(tPosition + 1);
  String dateOnly = stringTime.substring(0, tPosition);
  String formattedTime = String(dateOnly + " / " + timeOnly);
  String formattedC = String(dht.readTemperature()); 
  String formattedF = String((dht.readTemperature() * 9/5) + 32);
  String formattedOutC = String(dht2.readTemperature()); 
  String formattedOutF = String((dht2.readTemperature() * 9/5) + 32);
  String formattedLightSensorData = String((analogRead(LightSensor) * 2)); 
  String formattedHumdiditySensor = String(dht.readHumidity());
  String formattedOutHumdiditySensor = String(dht2.readHumidity());
  float pressure = bmp.readPressure();
  String formattedPressureSensor = String(pressure / 100);
  String formattedMicrophoneSensor = String(analogRead(microphoneSensor) / 10);
  String formattedMagnetSensor = String(digitalRead(magneticSensor) == 1 ? 0 : 1);
  String altitude = String(bmp.readAltitude(seaLevelPressure));
  String timeOnline = String((millis() / 1000));
  String inTempDisplacement = String(constrain((map(dht.readTemperature(),-5,30,-50,13300)),-50,13300));
  String outTempDisplacement = String(constrain((map(dht2.readTemperature(),-5,30,-50,13300)),-50,13300));
  String lightDisplacement = String(constrain((map((analogRead(LightSensor) * 2),0,1000,-50,13300)),-50,13300));
  String inHumidityDisplacement = String(constrain((map(dht.readHumidity(),0,100,-50,13300)),-50,1330));
  String outHumidityDisplacement = String(constrain((map(dht2.readHumidity(),0,100,-50,13300)),-50,13300));
  String pressureDisplacement = String(constrain((map((pressure / 100),980,1030,-50,13300)),-50,13300));
  String altitudeDisplacement = String(constrain((map(bmp.readAltitude(seaLevelPressure),0,3000,-50,13300)),-50,13300));
  String noiseDisplacement = String(constrain((map((analogRead(microphoneSensor) / 10),10,110,-50,13300)),-50,13300));
  localIP = WiFi.localIP().toString();
  subnetMask = WiFi.subnetMask().toString();
  gatewayIP = WiFi.gatewayIP().toString();
  signalStrength = String(WiFi.RSSI());
  // Display Function
  OLEDCurrentMillis = millis();
  if (OLEDCurrentMillis - OLEDPreviousMillis >= OLEDInterval) {
    OLEDPreviousMillis = OLEDCurrentMillis;
    OLEDPanel = (OLEDPanel + 1) % 3;
  }
  display.clearDisplay();
  OLEDHeader(dateOnly, timeOnly);
  switch (OLEDPanel) {
    case 0:
      OLEDPanel1(formattedC, formattedLightSensorData, formattedHumdiditySensor, formattedPressureSensor, formattedMicrophoneSensor, timeOnline);
      break;
    case 1:
      OLEDPanel2(formattedOutC, formattedOutHumdiditySensor, formattedMagnetSensor, altitude);
      break;
    case 2:
      OLEDPanel3(localIP, subnetMask, gatewayIP, signalStrength);
      break;
  }
  display.display();
  // Website Function
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    while (client.connected()) { // Keep connection open until client disconnects
      if (client.available()) {
        int whiteLightness = map(analogRead(LightSensor), 50, 500, 10, 100);
        analogWrite(whiteLED, whiteLightness);
        char c = client.read(); // Read and discard incoming data
        refferer += c;
        Serial.print("refferrer: ");
        Serial.println(refferer); //Print the data to the serial monitor for debugging
        if (c == '\n') { // Check for end of line
          int firstSpace = refferer.indexOf(' ');
          int secondSpace = refferer.indexOf(' ', firstSpace + 1);
          url = refferer.substring(firstSpace + 1, secondSpace);
          url.trim();
          String request = "";
          while (client.available()) {
            char c = client.read();
            request += c;
          }
          Serial.print("URL :");
          Serial.println(url);
          if (url == "/") {
            Serial.println("Main Page Requested");
            String base64Image = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA8AAAAPCAYAAAA71pVKAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcwAADsMAAA7DAcdvqGQAAAJtSURBVDhPjZJbS1RRFMf/a8854zjjXMS8jKVEZlIPoaOWgVpKty8QmkPhQOSD+BBUb2HzGkSPRqBpQdg30BTthklaFgZpRkGiUjN5YS5qZ+bs1h5l6EVycRabdfb6rctei/CvdHZqOUUlTRagRUiziqT0wDRXYcpJPp8shRaeIhhMbHsjDTsf9B0ion6NUKFBQkgJMiUgTf5MZk0kpZwiGM3hjo4viknBVgVK+doqKFcnwk5wQmnSDEnNrI1wAFKlUkHxhFWIclb8F2Y1TPPD+lq4WghvcTO7lKsKdIZL3S44dV2ZaXFarTjo8aTut6Vcd+U0a5zAr6xabz56G+uxvLEJr92OxzOzuDs5heuVPlw6UobFaAw5NhuuDA7hxfwCJMFP1NUd0oTY863lAlpHXmIqFEY2Z+49fQp2zYJ1I4HAwBAHzIRN09Bz/gwO9zxCPJkMqzqy9zrsiBoG5tbW0Fntw7H8XNx68xa+PD7HxlHjLcC9xpOYW15BhP0KsxyMyWwFry7E4tyXjgMuJ4IT7zH+8xeCNdX4yFXcPnEcY4tLuPH8FUo8brjYT7WgOKL73QPc97mz+wrxsKEeS/E4ijhy/+xX3OFAN6sqcLGsFPORCAocDlwdHMbIj3kYUj4j0dV7mcffl8EvmaVrKHO7EYrFENncTI/KxW+Ql2lLlR1PJNSowGvWShgd1Wj2+zsr0dFdz1nK6XW7XinQ0JBgoAlEYdXILuS3Bezf1makpv6nLTCj63odJ51OXe8gvMufNB110Wvtn5WdXplowD9TmtyotEAECGKIf61s3dAKdzFMoMD+DOFbbt8CAeAvAWQk4b6NVW4AAAAASUVORK5CYII=";
            client.print("HTTP/1.1 200 OK\r\n");
            client.print("Content-Type: text/html\r\n\r\n");
            // Style/CSS Section
            client.print("<html><head><title>Ardu4Weather</title>");
            client.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
            client.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
            client.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
            client.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
            client.print("<nav><ul><li><a href='/'>Home</a></li>");
            client.print("<li><a href='/about'>About</a></li>");
            client.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
            client.print("<style>");
            //client.print("<link rel='icon' href='");
            //client.print(base64image); what the fuck
            //client.print("'>");
            client.print("nav { background-color: #434b4f; padding: 10px 0; transition-duration: 0.4s; }");
            client.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
            client.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
            client.print("nav a { color: #ffffff; text-decoration: none; }");
            client.print("nav a:hover { color: #ccc; }");
            client.print("body { background: linear-gradient(0deg, hsla(198, 18%, 45%, 1) 0%, hsla(197, 66%, 90%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
            //display: grid; grid-template-columns: 3fr 1fr; grid-gap: 20px; 
            client.print(".main-container { width: 90%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #53636A; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
            client.print(".sidebar-container { background-color: #53636A; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); transition-duration: 0.4s; }");
            client.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
            client.print(".data-item { display: flex; justify-content: space-between; /* Align label and value */ margin-bottom: 10px; color: #ffffff; font-size: 25px; }");
            client.print(".data-label { font-weight: bold; color: #ffffff; font-size: 25px; }");
            client.print(".slider-container { position: relative; }");
            client.print(".slider-text { position: absolute; top: 50%; text-shadow: 0.5px 0.5px 1px rgba(255, 255, 255, 0.5);  transform: translateY(10px); font-size: 12px; }");
            client.print(".slider-text.left { left: 0; color: #7293a1 }");
            client.print(".slider-text.right { right: 0; color: #b95b5e}");
            client.print(".slider-text.light-left { left: 0; color: #ffffff }");
            client.print(".slider-text.light-right { right: 0; color: #ffe98a}");
            client.print(".slider-text.humid-left { left: 0; color: #ffffff }");
            client.print(".slider-text.humid-right { right: 0; color: #7293a1}");
            client.print(".slider-text.altitude-left { left: 0; color: #7293a1 }");
            client.print(".slider-text.altitude-right { right: 0; color: #ffffff}");
            client.print(".slider-text.noise-left { left: 0; color: #ffffff }");
            client.print(".slider-text.noise-right { right: 0; color: #b95b5e}");
            client.print(".slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(198, 20%, 54%, 1) 0%,  hsla(0, 0%, 100%, 1) 50%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            client.print(".light-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(0, 0%, 100%, 1) 0%, hsla(49, 100%, 77%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            client.print(".humid-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(198, 20%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            client.print(".altitude-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(198, 20%, 54%, 1) 0%, hsla(0, 0%, 100%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            client.print(".noise-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
            client.print(".in-temp-indicator { position: absolute; transform: translate(");
            client.print(inTempDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".out-temp-indicator { position: absolute; transform: translate(");
            client.print(outTempDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".light-indicator { position: absolute; transform: translate(");
            client.print(lightDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".in-humidity-indicator { position: absolute; transform: translate(");
            client.print(inHumidityDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".out-humidity-indicator { position: absolute; transform: translate(");
            client.print(outHumidityDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".pressure-indicator { position: absolute; transform: translate(");
            client.print(pressureDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".altitude-indicator { position: absolute; transform: translate(");
            client.print(altitudeDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print(".noise-indicator { position: absolute; transform: translate(");
            client.print(noiseDisplacement);
            client.print("%, -40%); width: 6px;  height: 20; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; }");
            client.print("img { max-width: 100%; height: auto; display: block; margin: 20px auto; border-radius: 8px; box-shadow: 0 0 5px rgba(0, 0, 0, 0.1); }");
            client.print("button { background-color: #ffffff; color: #84a8b2; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transition-duration: 0.4s; }");
            client.print("button:hover { background-color: #84a8b2; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ }");
            client.print("button:active { background-color: #7293A1; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transform: translateY(2px);}");
            client.print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24; height: auto; display: block; margin: 20px auto;}");
            client.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
            client.print("</style></head>");
            // Javascript Section
            // Body/HTML Section
            client.print("<body><div class='main-container'><h1>Ardu4Weather - Arduino Weather Station</h1>");
            client.print("<div class='data-item'><span class='data-label'>Time:</span><span>");
            client.print(formattedTime);
            client.print("</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Indoor Temperature:</span><span>");
            client.print(formattedC);
            client.print("&deg;C / ");
            client.print(formattedF);
            client.print("&deg;F</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text left'>-5&deg;C</span>");
            client.print("<div class='slider-bar'><div class='in-temp-indicator'></div></div>");
            client.print("<span class='slider-text right'>30&deg;C</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Outdoor Temperature:</span><span>");
            client.print(formattedOutC);
            client.print("&deg;C / ");
            client.print(formattedOutF);
            client.print("&deg;F</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text left'>-5&deg;C</span>");
            client.print("<div class='slider-bar'><div class='out-temp-indicator'></div></div>");
            client.print("<span class='slider-text right'>30&deg;C</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            client.print(formattedLightSensorData);
            client.print(" Lumens</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text light-left'>0 Lm</span>");
            client.print("<div class='light-slider-bar'><div class='light-indicator'></div></div>");
            client.print("<span class='slider-text light-right'>1000 Lm</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Indoor Humidity:</span><span>");
            client.print(formattedHumdiditySensor);
            client.print("%</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text humid-left'>0%</span>");
            client.print("<div class='humid-slider-bar'><div class='in-humidity-indicator'></div></div>");
            client.print("<span class='slider-text humid-right'>100%</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Outdoor Humidity:</span><span>");
            client.print(formattedOutHumdiditySensor);
            client.print("%</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text humid-left'>0%</span>");
            client.print("<div class='humid-slider-bar'><div class='out-humidity-indicator'></div></div>");
            client.print("<span class='slider-text humid-right'>100%</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            client.print(formattedPressureSensor);
            client.print(" mBar</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text left'>980 mBar</span>");
            client.print("<div class='slider-bar'><div class='pressure-indicator'></div></div>");
            client.print("<span class='slider-text right'>1030 mBar</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Approx Altitude:</span><span>");
            client.print(altitude);
            client.print(" m</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text altitude-left'>0 m</span>");
            client.print("<div class='altitude-slider-bar'><div class='altitude-indicator'></div></div>");
            client.print("<span class='slider-text altitude-right'>3000 m</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            client.print(formattedMicrophoneSensor);
            client.print(" dB</span></div>");
            client.print("<div class='slider-container'>");
            client.print("<span class='slider-text noise-left'>10 dB</span>");
            client.print("<div class='noise-slider-bar'><div class='noise-indicator'></div></div>");
            client.print("<span class='slider-text noise-right'>110 dB</span> ");
            client.print("</div>");
            client.print("<div class='data-item'><span class='data-label'>Presence Of Strong Magnetic Field:</span><span>");
            client.print(formattedMagnetSensor);
            client.print("</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Online For:</span><span>");
            client.print(timeOnline);
            client.print(" Seconds</span></div>");
            client.print("<div><button id='toggleButton'>Show Averages</button></div>");
            // Hidden Avarage Data, 1H
            client.print("<div id='1HaverageData' style='display: none;'><h1>1 Hour Rolling Avarage</h1>");
            client.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            client.print(formattedC);
            client.print("&deg;C / ");
            client.print(formattedF);
            client.print("&deg;F</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            client.print(formattedLightSensorData);
            client.print(" Lumens</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            client.print(formattedHumdiditySensor);
            client.print("%</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            client.print(formattedPressureSensor);
            client.print(" mBar</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            client.print(formattedMicrophoneSensor);
            client.print(" dB</span></div>");
            client.print("</div>");
            // Hidden Avarage Data, 12H
            client.print("<div id='12HaverageData' style='display: none;'><h1>12 Hour Rolling Avarage</h1>");
            client.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            client.print(formattedC);
            client.print("&deg;C / ");
            client.print(formattedF);
            client.print("&deg;F</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            client.print(formattedLightSensorData);
            client.print(" Lumens</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            client.print(formattedHumdiditySensor);
            client.print("%</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            client.print(formattedPressureSensor);
            client.print(" mBar</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            client.print(formattedMicrophoneSensor);
            client.print(" dB</span></div>");
            client.print("</div>");
            // Hidden Avarage Data, 24H
            client.print("<div id='24HaverageData' style='display: none;'><h1>24 Hour Rolling Avarage</h1>");
            client.print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
            client.print(formattedC);
            client.print("&deg;C / ");
            client.print(formattedF);
            client.print("&deg;F</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
            client.print(formattedLightSensorData);
            client.print(" Lumens</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
            client.print(formattedHumdiditySensor);
            client.print("%</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
            client.print(formattedPressureSensor);
            client.print(" mBar</span></div>");
            client.print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
            client.print(formattedMicrophoneSensor);
            client.print(" dB</span></div>");
            client.print("</div>");
            client.print("<body><div class='sidebar-container'><h1>Side Container</h1>");
            client.print("</div>");
            // Javascript for button
            client.print("<script>");
            client.print("const toggleButton = document.getElementById('toggleButton');");
            client.print("const avgDataDiv1 = document.getElementById('1HaverageData');");
            client.print("const avgDataDiv12 = document.getElementById('12HaverageData');");
            client.print("const avgDataDiv24 = document.getElementById('24HaverageData');");
            client.print("toggleButton.addEventListener('click', () => {");
            client.print("  if (toggleButton.textContent === 'Show Averages') {");
            client.print("    toggleButton.textContent = '- Collapse -';");
            client.print("    avgDataDiv1.style.display = 'block';");
            client.print("    avgDataDiv12.style.display = 'block';");
            client.print("    avgDataDiv24.style.display = 'block';");
            client.print("  } else {");
            client.print("    toggleButton.textContent = 'Show Averages';");
            client.print("    avgDataDiv1.style.display = 'none';");
            client.print("    avgDataDiv12.style.display = 'none';");
            client.print("    avgDataDiv24.style.display = 'none';");
            client.print("  }");
            client.print("});");
            client.print("</script>");
            client.print("<img src='");
            client.print(base64Image);
            client.print("' alt='Weather Station Image'></div>");
            client.print("<span class='material-symbols-outlined'>all_inclusive</span>");
            client.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
            client.print("</html>");
            client.flush();
            break;
          } else if (url == "/about") {
              Serial.println("About Page Requested");
              client.print("HTTP/1.1 200 OK\r\n");
              client.print("Content-Type: text/html\r\n\r\n");
              // Style/CSS Section
              client.print("<html><head><title>Ardu4Weather</title></head>");
              client.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
              client.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
              client.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
              client.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
              client.print("<nav><ul><li><a href='/'>Home</a></li>");
              client.print("<li><a href='/about'>About</a></li>");
              client.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
              client.print("<style>");
              client.print("nav { background-color: #5e3434; padding: 10px 0; font-size: 15px; transition-duration: 0.4s; }");
              client.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
              client.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
              client.print("nav a { color: #ffffff; text-decoration: none; }");
              client.print("nav a:hover { color: #ccc; }");
              client.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
              client.print("h2 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 30px; }");
              client.print("p { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 20px; }");
              client.print("body { background: linear-gradient(180deg, hsla(354, 53%, 70%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
              client.print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #7d4042; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
              client.print("img { max-width: 30%; height: auto; border-radius: 16px; margin: 30px; }");
              //client.print("img { max-width: 30%; height: auto; border-radius: 16px; margin: 30px; }");
              client.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
              client.print("</style></head>");
              client.print("<body><div class='main-container'><h1>Ardu4Weather - About</h1>");
              client.print("<h2>Arduino Uno R4 Wifi</h2>");
              client.print("<p>Arduino Uno R4 Wifi</p>");
              client.print("<img src=\'https://i.imgur.com/2Vkg8gU.jpeg' alt=\'Arduino R4 WiFi Image Close'><br>");
              client.print("<h2>Breadboard</h2>");
              client.print("<p>Breadboard</p>");
              client.print("<img src=\'https://i.imgur.com/Z8AHWJq.jpeg' alt=\'Arduino Breadboard Image'><br>");
              client.print("<img src=\'https://i.imgur.com/yuNZByw.jpeg' alt=\'Arduino Full Image'><br>");
              client.print("<img src=\'https://i.imgur.com/541JSSP.jpeg' alt=\'Microphone Sensor Image'><br>");
              client.print("<img src=\'https://i.imgur.com/uKWry8u.jpeg' alt=\'Pressure Sensor Image'><br>");
              client.print("<img src=\'https://i.imgur.com/A5zKZvm.jpeg' alt=\'DHT Sensor Image'><br>");
              client.print("<img src=\'https://i.imgur.com/7kgKpvS.jpeg' alt=\'Light Sensor Image'><br>");
              client.print("</div>");
              client.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
              client.print("</html>");
              client.flush();
              break;
              //
          } else if (url == "/data") {
              Serial.println("Data Page Requested");
              client.print("HTTP/1.1 200 OK\r\n");
              client.print("Content-Type: text/html\r\n\r\n");
              // Style/CSS Section
              client.print("<html><head><title>Ardu4Weather</title></head>");
              client.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
              client.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
              client.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
              client.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
              client.print("<nav><ul><li><a href='/'>Home</a></li>");
              client.print("<li><a href='/about'>About</a></li>");
              client.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
              client.print("<style>");
              client.print("nav { background-color: #45573b; padding: 10px 0; transition-duration: 0.4s; }");
              client.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
              client.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
              client.print("nav a { color: #ffffff; text-decoration: none; }");
              client.print("nav a:hover { color: #ccc; }");
              client.print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
              client.print("img { max-width: 30%; height: auto; border-radius: 16px; margin: 30px; }");
              client.print("body { background: linear-gradient(180deg, hsla(93, 64%, 79%, 1) 0%, hsla(96, 30%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
              client.print(".main-container { width: 90%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #5a7449; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
              client.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
              client.print("</style></head>");
              client.print("<body><div class='main-container'><h1>Ardu4Weather - Data</h1>");
              client.print("<img src=\'https://i.imgur.com/Z8AHWJq.jpeg' alt=\'Arduino Breadboard Image'><br>");
              client.print("</div>");
              client.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
              client.print("</html>");
              client.flush();
              break;
          } else { // Serve error 404 page
              Serial.println("Error 404");
              client.print("HTTP/1.1 200 OK\r\n");
              client.print("Content-Type: text/html\r\n\r\n");
              // Style/CSS Section
              client.print("<html><head><title>Ardu4Weather</title></head>");
              client.print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
              client.print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
              client.print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
              client.print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
              client.print("<nav><ul><li><a href='/'>Home</a></li>");
              client.print("<li><a href='/about'>About</a></li>");
              client.print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
              client.print("<style>");
              client.print("nav { background-color: #333; padding: 10px 0; transition-duration: 0.4s; }");
              client.print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
              client.print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
              client.print("nav a { color: #ffffff; text-decoration: none; }");
              client.print("nav a:hover { color: #ccc; }");
              client.print("body { font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
              client.print("h1 { color: #000000; text-align: center; margin-bottom: 20px; font-size: 50px; }");
              client.print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
              client.print("</style></head>");
              client.print("<body><h1>Error 404 - Page Not Found</h1>");
              client.print("</body>");
              client.print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
              client.print("</html>");
              break;
          }
        }
      }
    }
    url = "";
    refferer = "";
    client.stop();
    Serial.println("Client disconnected.");
    analogWrite(whiteLED, LOW);
  }
}