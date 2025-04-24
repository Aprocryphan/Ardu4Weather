#include <WiFi.h>
#include "DHT.h" // For DHT11 sensor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#include "var.h"
int MicLevels(); // Function declaration for microphone sensor

void mainPage(
    WiFiClient& WebClient,
    Adafruit_BMP085& bmp,
    DHT& dht,
    DHT& dht2,
    const char Cblue[13],
    const char Creset[13],
    char* serialOutputBuffer,
    float seaLevelPressure,
    int LightSensor,
    float pressure,
    char* formattedTime,
    float formattedC,
    float formattedF,
    int inTempDisplacement,
    float formattedOutC,
    float formattedOutF,
    int outTempDisplacement,
    int formattedLightSensorData,
    int lightDisplacement,
    float formattedHumiditySensor,
    int inHumidityDisplacement,
    float formattedOutHumiditySensor,
    int outHumidityDisplacement,
    float formattedPressureSensor,
    int pressureDisplacement,
    float altitude,
    int altitudeDisplacement,
    int formattedMicrophoneSensor,
    int noiseDisplacement,
    int formattedMagnetSensor,
    unsigned long long secondsOnline,
    char* hoursOnline
) {
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
}

void aboutPage(
    char* serialOutputBuffer,
    WiFiClient& WebClient,
    const char Cblue[13],
    const char Creset[13]
) {
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
}

void dataPage (
    char* serialOutputBuffer,
    WiFiClient& WebClient,
    const char Cblue[13],
    const char Creset[13]
) {
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
}

void aprocryphanPage(
    char* serialOutputBuffer,
    WiFiClient& WebClient,
    const char Cblue[13],
    const char Creset[13]
) {
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
}

void web1Page(WiFiClient& WebClient) {
    WebClient.flush();
}

void errPage(
    char* serialOutputBuffer,
    WiFiClient& WebClient,
    const char Cyellow[13],
    const char Creset[13]
) {
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
}