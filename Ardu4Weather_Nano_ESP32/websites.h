#include <ESPAsyncWebServer.h>
#include "DHT.h" // For DHT11 sensor
#include <Adafruit_BMP085.h> // Use Adafruit BMP085 library for BMP180
#include <time.h>
#include "var.h"
int MicLevels(); // Function declaration for microphone sensor

void mainPage(
    Adafruit_BMP085 &bmp,
    DHT &dht, 
    DHT &dht2, 
    const char *Cblue, 
    const char *Creset, 
    char *serialOutputBuffer, 
    float seaLevelPressure, 
    int LightSensor, 
    float pressure, 
    char *formattedTime, 
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
    char *hoursOnline, 
    AsyncResponseStream *response
  ) {
    sprintf(serialOutputBuffer, "%sMain Page Requested%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    // Style/CSS Section
    response->print("<head><meta charset='utf-8'>");
    response->print("<title>Ardu4Weather</title>");
    response->print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
    response->print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
    response->print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
    response->print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
    response->print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
    response->print("<style>");
    response->print("nav { background-color: #434b4f; transition-duration: 0.4s; font-size: 16px; display: flex;  align-items: center; padding: 10px 0; position: relative; }");
    response->print("nav:hover { background-color: #53636A; transition-duration: 0.4s; font-size: 18px; display: flex; align-items: center; position: relative; }");
    response->print("nav ul { list-style: none; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; flex-grow: 1; }");
    response->print("nav li { display: inline-block; margin: 0 15px; }");
    response->print("nav a { color: #ffffff; text-decoration: none; }");
    response->print("nav a:hover { color: #ccc; }");
    response->print("body { background: linear-gradient(0deg, hsla(198, 18%, 45%, 1) 0%, hsla(197, 66%, 90%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
    //display: grid; grid-template-columns: 3fr 1fr; grid-gap: 20px; 
    response->print(".main-container { width: 90%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #53636A; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
    response->print(".sidebar-container { background-color: #53636A; padding: 20px; border-radius: 10px; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); transition-duration: 0.4s; }");
    response->print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
    response->print(".data-item { display: flex; justify-content: space-between; /* Align label and value */ margin-bottom: 10px; color: #ffffff; font-size: 25px; }");
    response->print(".data-label { font-weight: bold; color: #ffffff; font-size: 25px; }");
    response->print(".slider-container { position: relative; }");
    response->print(".slider-text { position: absolute; top: 50%; text-shadow: 0.5px 0.5px 1px rgba(255, 255, 255, 0.5);  transform: translateY(10px); font-size: 12px; }");
    response->print(".slider-text.left { left: 0; color: #7293a1 }");
    response->print(".slider-text.right { right: 0; color: #b95b5e}");
    response->print(".slider-text.light-left { left: 0; color: #ffffff }");
    response->print(".slider-text.light-right { right: 0; color: #ffe98a}");
    response->print(".slider-text.humid-left { left: 0; color: #ffffff }");
    response->print(".slider-text.humid-right { right: 0; color: #7293a1}");
    response->print(".slider-text.altitude-left { left: 0; color: #7293a1 }");
    response->print(".slider-text.altitude-right { right: 0; color: #ffffff}");
    response->print(".slider-text.noise-left { left: 0; color: #ffffff }");
    response->print(".slider-text.noise-right { right: 0; color: #b95b5e}");
    response->print(".slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(198, 20%, 54%, 1) 0%,  hsla(0, 0%, 100%, 1) 50%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
    response->print(".light-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(0, 0%, 100%, 1) 0%, hsla(49, 100%, 77%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
    response->print(".humid-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(198, 20%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
    response->print(".altitude-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg, hsla(198, 20%, 54%, 1) 0%, hsla(0, 0%, 100%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
    response->print(".noise-slider-bar { width: 100%; height: 5px; background: linear-gradient(90deg,hsla(0, 0%, 100%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); border-radius: 5px; margin-bottom: 30px; position: relative; }");
    response->print(".in-temp-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
    response->print(".out-temp-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
    response->print(".light-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
    response->print(".in-humidity-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
    response->print(".out-humidity-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
    response->print(".pressure-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
    response->print(".altitude-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px; }");
    response->print(".noise-indicator { position: absolute; width: 6px; height: 20px; background-color: white; box-shadow: 0 0 2px rgba(0, 0, 0, 0.5); border-radius: 2px; top: -7px;}");
    response->print(".header-image img { max-width: 4%; height: auto; padding-left: 10px; display: block; float: left; }");
    response->print("button { background-color: #ffffff; color: #84a8b2; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transition-duration: 0.4s; }");
    response->print("button:hover { background-color: #84a8b2; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ }");
    response->print("button:active { background-color: #7293A1; color: #ffffff; /* White text */ padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; display: block; /* Make it a block element */ margin: 0 auto; /* Center horizontally */ transform: translateY(2px);}");
    response->print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
    response->print("</style>");
    response->print("<nav><div class='header-image'><img src='https://i.imgur.com/mlL3Fiw.png' alt='Ardu4Weather Logo' href='/'></div>");
    response->print("<ul><li><a href='/'>Home</a></li>");
    response->print("<li><a href='/about'>About</a></li>");
    response->print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
    response->print("</head>");
    // Javascript Section
    // Body/HTML Section
    response->print("<body><div class='main-container'><h1>Ardu4Weather - Arduino Weather Station</h1>");
    response->print("<div class='data-item'><span class='data-label'>Time:</span><span>");
    response->print(formattedTime);
    response->print("</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Indoor Temperature:</span><span>");
    response->print(formattedC);
    response->print("&deg;C / ");
    response->print(formattedF);
    response->print("&deg;F</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text left'>-5&deg;C</span>");
    response->print("<div class='slider-bar'><div class='in-temp-indicator' style='left: ");
    response->print(inTempDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text right'>30&deg;C</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Outdoor Temperature:</span><span>");
    response->print(formattedOutC);
    response->print("&deg;C / ");
    response->print(formattedOutF);
    response->print("&deg;F</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text left'>-5&deg;C</span>");
    response->print("<div class='slider-bar'><div class='out-temp-indicator' style='left: ");
    response->print(outTempDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text right'>30&deg;C</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
    response->print(formattedLightSensorData);
    response->print(" Lumens</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text light-left'>0 Lm</span>");
    response->print("<div class='light-slider-bar'><div class='light-indicator' style='left: ");
    response->print(lightDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text light-right'>1000 Lm</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Indoor Humidity:</span><span>");
    response->print(formattedHumiditySensor);
    response->print("%</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text humid-left'>0%</span>");
    response->print("<div class='humid-slider-bar'><div class='in-humidity-indicator' style='left: ");
    response->print(inHumidityDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text humid-right'>100%</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Outdoor Humidity:</span><span>");
    response->print(formattedOutHumiditySensor);
    response->print("%</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text humid-left'>0%</span>");
    response->print("<div class='humid-slider-bar'><div class='out-humidity-indicator' style='left: ");
    response->print(outHumidityDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text humid-right'>100%</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
    response->print(formattedPressureSensor);
    response->print(" mBar</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text left'>980 mBar</span>");
    response->print("<div class='slider-bar'><div class='pressure-indicator' style='left: ");
    response->print(pressureDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text right'>1030 mBar</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Approx Altitude:</span><span>");
    response->print(altitude);
    response->print(" m</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text altitude-left'>0 m</span>");
    response->print("<div class='altitude-slider-bar'><div class='altitude-indicator' style='left: ");
    response->print(altitudeDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text altitude-right'>3000 m</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
    response->print(formattedMicrophoneSensor);
    response->print(" Units</span></div>");
    response->print("<div class='slider-container'>");
    response->print("<span class='slider-text noise-left'>10 U</span>");
    response->print("<div class='noise-slider-bar'><div class='noise-indicator' style='left: ");
    response->print(noiseDisplacement);
    response->print("%;'></div></div>");
    response->print("<span class='slider-text noise-right'>700 U</span> ");
    response->print("</div>");
    response->print("<div class='data-item'><span class='data-label'>Presence Of Strong Magnetic Field:</span><span>");
    response->print(formattedMagnetSensor);
    response->print("</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Online For:</span><span>");
    response->print(secondsOnline);
    response->print(" Seconds (");
    response->print(hoursOnline);
    response->print("H)</span></div>");
    response->print("<div><button id='toggleButton'>Show Averages</button></div>");
    // Hidden Average Data, 1H
    response->print("<div id='1HaverageData' style='display: none;'><h1>1 Hour Rolling Average</h1>");
    response->print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
    response->print(formattedC);
    response->print("&deg;C / ");
    response->print(formattedF);
    response->print("&deg;F</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
    response->print(formattedLightSensorData);
    response->print(" Lumens</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
    response->print(formattedHumiditySensor);
    response->print("%</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
    response->print(formattedPressureSensor);
    response->print(" mBar</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
    response->print(formattedMicrophoneSensor);
    response->print(" dB</span></div>");
    response->print("</div>");
    // Hidden Average Data, 12H
    response->print("<div id='12HaverageData' style='display: none;'><h1>12 Hour Rolling Average</h1>");
    response->print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
    response->print(formattedC);
    response->print("&deg;C / ");
    response->print(formattedF);
    response->print("&deg;F</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
    response->print(formattedLightSensorData);
    response->print(" Lumens</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
    response->print(formattedHumiditySensor);
    response->print("%</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
    response->print(formattedPressureSensor);
    response->print(" mBar</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
    response->print(formattedMicrophoneSensor);
    response->print(" dB</span></div>");
    response->print("</div>");
    // Hidden Avarage Data, 24H
    response->print("<div id='24HaverageData' style='display: none;'><h1>24 Hour Rolling Average</h1>");
    response->print("<div class='data-item'><span class='data-label'>Temperature:</span><span>");
    response->print(formattedC);
    response->print("&deg;C / ");
    response->print(formattedF);
    response->print("&deg;F</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Brightness:</span><span>");
    response->print(formattedLightSensorData);
    response->print(" Lumens</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Humidity:</span><span>");
    response->print(formattedHumiditySensor);
    response->print("%</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Pressure:</span><span>");
    response->print(formattedPressureSensor);
    response->print(" mBar</span></div>");
    response->print("<div class='data-item'><span class='data-label'>Noise Level:</span><span>");
    response->print(formattedMicrophoneSensor);
    response->print(" dB</span></div>");
    response->print("</div>");
    response->print("<body><div class='sidebar-container'><h1>Side Container</h1>");
    response->print("</div></div>");
    // Javascript for button
    response->print("<script>");
    response->print("const toggleButton = document.getElementById('toggleButton');");
    response->print("const avgDataDiv1 = document.getElementById('1HaverageData');");
    response->print("const avgDataDiv12 = document.getElementById('12HaverageData');");
    response->print("const avgDataDiv24 = document.getElementById('24HaverageData');");
    response->print("toggleButton.addEventListener('click', () => {");
    response->print("  if (toggleButton.textContent === 'Show Averages') {");
    response->print("    toggleButton.textContent = '- Collapse -';");
    response->print("    avgDataDiv1.style.display = 'block';");
    response->print("    avgDataDiv12.style.display = 'block';");
    response->print("    avgDataDiv24.style.display = 'block';");
    response->print("  } else {");
    response->print("    toggleButton.textContent = 'Show Averages';");
    response->print("    avgDataDiv1.style.display = 'none';");
    response->print("    avgDataDiv12.style.display = 'none';");
    response->print("    avgDataDiv24.style.display = 'none';");
    response->print("  }");
    response->print("});");
    response->print("</script>");
    response->print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS<br>");
    response->print("<a href='https://github.com/Aprocryphan/Ardu4Weather' style='text-decoration: underline; display: flex; align-items: center;' target='_blank'>");
    response->print("<img src='https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png' alt='GitHub Logo' style='width: 20px; height: 20px; margin-right: 5px;'>");
    response->print("GitHub</a></p></footer>");
    response->print("</body></html>");
}

void aboutPage(
    char* serialOutputBuffer,
    const char Cblue[13],
    const char Creset[13],
    AsyncResponseStream *response
  ) {
    sprintf(serialOutputBuffer, "%sAbout Page Requested%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    // Style/CSS Section
    response->print("<head><meta charset='utf-8'>");
    response->print("<title>Ardu4Weather</title>");
    response->print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
    response->print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
    response->print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
    response->print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=all_inclusive' />");
    response->print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
    response->print("<nav><ul><li><a href='/'>Home</a></li>");
    response->print("<li><a href='/about'>About</a></li>");
    response->print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
    response->print("<style>");
    response->print("nav { background-color: #5e3434; padding: 10px 0; font-size: 15px; transition-duration: 0.4s; }");
    response->print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
    response->print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
    response->print("nav a { color: #ffffff; text-decoration: none; }");
    response->print("nav a:hover { color: #ccc; }");
    response->print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
    response->print("h2 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 30px; }");
    response->print("p { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 20px; }");
    response->print(".footer-p { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 16px; }");
    response->print("body { background: linear-gradient(180deg, hsla(354, 53%, 70%, 1) 0%, hsla(358, 40%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
    response->print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #7d4042; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
    response->print("img { max-width: 30%; height: auto; border-radius: 16px; margin: 10px; }");
    response->print(".image-text-container { display: flex; flex-direction: column; width: 80%; margin: 20px auto; }");
    response->print(".image-text-pair { display: flex; align-items: center; margin-bottom: 20px; }");
    response->print(".image-text-pair img { max-width: 30%; height: auto; border-radius: 16px; margin-right: -30px; margin-left: -40px; }");
    response->print(".image-text p { width: 110%; padding: 40px; padding-top: 0px; box-sizing: border-box; margin-left: 30px; text-align: left; }");
    response->print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
    response->print("</style></head>");
    response->print("<body><div class='main-container'><h1>Ardu4Weather - About</h1>");
    response->print("<h2>Arduino Uno R4 Wifi</h2>");
    response->print("<div class='image-text-container'>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/2Vkg8gU.jpeg' alt=\'Arduino R4 WiFi Image Close'><div class='image-text'>");
    response->print("<p>My Arduino Uno R4 WiFi is the heart of this project, hosting the website you're currently viewing, complete with all subpages. It's connected to a variety of sensors, including a DHT22 for temperature and humidity, a BMP180 for barometric pressure, and a photoresistor for light levels. The Arduino collects data constantly in the loop() function, processing it and sending it to the website in real-time.</p>");
    response->print("</div></div>");
    response->print("<h2>Breadboard</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/Z8AHWJq.jpeg' alt=\'Arduino Breadboard Image'><div class='image-text'>");
    response->print("<p>This is the breadboard that contains most of the sensors used in this project. It is a small, clear, standard 30x10 breadboard.</p>");
    response->print("</div></div>");
    response->print("<h2>Arduino Full Image</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/yuNZByw.jpeg' alt=\'Arduino Full Image'><div class='image-text'>");
    response->print("<p>This is an image of the full project with breadboard and Arduino in view. You can also see the wires that lead outside my window for the external DHT sensor.</p>");
    response->print("</div></div>");
    response->print("<h2>Microphone Sensor Image</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/541JSSP.jpeg' alt=\'Microphone Sensor Image'><div class='image-text'>");
    response->print("<p>MAX4466 Electret Microphone Amplifier for noise levels</p>");
    response->print("</div></div>");
    response->print("<h2>Pressure Sensor Image</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/uKWry8u.jpeg' alt=\'Pressure Sensor Image'><div class='image-text'>");
    response->print("<p>BMP180</p>");
    response->print("</div></div>");
    response->print("<h2>DHT Sensor Image</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/A5zKZvm.jpeg' alt=\'DHT Sensor Image'><div class='image-text'>");
    response->print("<p>This is the internal DHT22 sensor, it returns the temperature and humidity digitally.</p>");
    response->print("</div></div>");
    response->print("<h2>Light Sensor Image</h2>");
    response->print("<div class='image-text-pair'><img src=\'https://i.imgur.com/7kgKpvS.jpeg' alt=\'Light Sensor Image'><div class='image-text'>");
    response->print("<p>Photoresistor</p>");
    response->print("</div></div></div></div>");
    response->print("<footer><p class='footer-p'>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
    response->print("</body></html>");
}

void dataPage (
    char* serialOutputBuffer,
    const char Cblue[13],
    const char Creset[13],
    AsyncResponseStream *response
  ) {
    sprintf(serialOutputBuffer, "%sData Page Requested%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    // Style/CSS Section
    response->print("<head><meta charset='utf-8'>");
    response->print("<title>Ardu4Weather</title>");
    response->print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
    response->print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
    response->print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
    response->print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=construction' />");
    response->print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
    response->print("<nav><ul><li><a href='/'>Home</a></li>");
    response->print("<li><a href='/about'>About</a></li>");
    response->print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
    response->print("<style>");
    response->print("nav { background-color: #24331e; padding: 10px 0; transition-duration: 0.4s; }");
    response->print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
    response->print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
    response->print("nav a { color: #ffffff; text-decoration: none; }");
    response->print("nav a:hover { color: #ccc; }");
    response->print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
    response->print(".icon { font-size: 16px; color: #ffffff; z-index: 0; }");
    response->print("body { background: linear-gradient(180deg, hsla(93, 64%, 79%, 1) 0%, hsla(96, 30%, 54%, 1) 100%); font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
    response->print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #45573b; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); /* Subtle shadow */ border-radius: 8px; transition-duration: 0.4s; }");
    response->print("h1 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 40px; }");
    response->print("h2 { color: #ffffff; text-align: center; margin-bottom: 20px; font-size: 20px; }");
    response->print("p { color: #ffffff; text-align: center; margin-bottom: 20px; }");
    response->print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
    response->print("</style></head>");
    response->print("<body><div class='main-container'><h1>Ardu4Weather - Data</h1>");
    response->print("<h2>Historical Data</h2>");
    response->print("<p>Page Under Construction! <span class='material-symbols-outlined icon'>construction</span></p>");
    response->print("</div>");
    response->print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
    response->print("</body></html>");
}

void aprocryphanPage(
    char* serialOutputBuffer,
    const char Cblue[13],
    const char Creset[13],
    AsyncResponseStream *response
  ) {
    sprintf(serialOutputBuffer, "%sAbout Apro Page Requested%s", Cblue, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    // Style/CSS Section
    response->print("<head><meta charset='utf-8'>");
    response->print("<title>Ardu4Weather</title>");
    response->print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
    response->print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
    response->print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
    response->print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=construction' />");
    response->print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
    response->print("<nav><ul><li><a href='/'>Home</a></li>");
    response->print("<li><a href='/about'>About</a></li>");
    response->print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
    response->print("<style>");
    response->print("nav { background-color: #ffffff; padding: 10px 0; transition-duration: 0.4s; }");
    response->print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
    response->print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
    response->print("nav a { color: #000000; text-decoration: none; }");
    response->print("nav a:hover { color: #ccc; }");
    response->print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
    response->print("body { background: #000000; font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
    response->print(".icon { font-size: 16px; color: #000000; z-index: 0; }");
    response->print(".main-container { width: 110%; max-width: 800px; margin: 20px auto; padding: 20px; background-color: #ffffff; border-radius: 8px; transition-duration: 0.4s; }");
    response->print("h1 { color: #000000; text-align: center; margin-bottom: 20px; font-size: 40px; }");
    response->print("h2 { color: #000000; text-align: center; margin-bottom: 20px; font-size: 20px; }");
    response->print("p { color: #000000; }");
    response->print("footer { background-color: #ffffff; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
    response->print("</style></head>");
    response->print("<body><div class='main-container'><h1>Ardu4Weather - Aprocryphan</h1>");
    response->print("<h2>About</h2>");
    response->print("<p>Page Under Construction! <span class='material-symbols-outlined icon'>construction</span></p>");
    response->print("</div>");
    response->print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
    response->print("</body></html>");
}

void web1Page(AsyncResponseStream *response) {
}

void errPage(
    char* serialOutputBuffer,
    const char Cyellow[13],
    const char Creset[13],
    AsyncResponseStream *response
  ) {
    sprintf(serialOutputBuffer, "%sError 404%s", Cyellow, Creset);
    Serial.println(serialOutputBuffer);
    serialOutputBuffer[0] = '\0';
    // Style/CSS Section
    response->print("<head><meta charset='utf-8'>");
    response->print("<title>Ardu4Weather</title>");
    response->print("<link rel='preconnect' href='https://fonts.googleapis.com'>");
    response->print("<link rel='preconnect' href='https://fonts.gstatic.com' crossorigin>");
    response->print("<link href='https://fonts.googleapis.com/css2?family=Funnel+Display:wght@300..800&display=swap' rel='stylesheet'>");
    response->print("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:opsz,wght,FILL,GRAD@20..48,100..700,0..1,-50..200&icon_names=sentiment_very_dissatisfied' />");
    response->print("<link rel='icon' href='https://i.imgur.com/mlL3Fiw.png'>");
    response->print("<nav><ul><li><a href='/'>Home</a></li>");
    response->print("<li><a href='/about'>About</a></li>");
    response->print("<li><a href='/data'>Historical Data</a></li></ul></nav>");
    response->print("<style>");
    response->print(".material-symbols-outlined { font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24 }");
    response->print("nav { background-color: #333; padding: 10px 0; transition-duration: 0.4s; }");
    response->print("nav ul { list-style: none; margin: 0; padding: 0; text-align: center; }");
    response->print("nav li { display: inline-block; margin: 0 15px; /* Spacing between navigation items */ }");
    response->print("nav a { color: #ffffff; text-decoration: none; }");
    response->print("nav a:hover { color: #ccc; }");
    response->print("body { font-family: 'Funnel Display', serif; font-weight: 300; margin: 0; /* Remove default margins */ display: flex; flex-direction: column; min-height: 100vh; /* Ensure full viewport height */ transition-duration: 0.4s; }");
    response->print(".center-container { display: flex; justify-content: center; align-items: center; flex-grow: 1; }");
    response->print(".background-icon { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); font-size: 1000px; color: #000000; opacity: 0.03; z-index: 0; }");
    response->print("h1 { color: #999999; text-align: center; margin-bottom: 20px; font-size: 70px; }");
    response->print("h2 { color: #999999; text-align: center; margin-bottom: 20px; font-size: 50px; }");
    response->print("footer { background-color: #333; color: white; text-align: center; padding: 1px 0; margin-top: auto; /* Push footer to bottom */ transition-duration: 0.4s; }");
    response->print("* { user-select: none; }");
    response->print("</style></head>");
    response->print("<body><div class='center-container'>");
    response->print("<div>");
    response->print("<h1>Error 404 - Page Not Found</h1>");
    response->print("<h2>The page you're looking for wasn't found on the server</h2>");
    response->print("</div>");
    response->print("</div>");
    response->print("<span class='material-symbols-outlined background-icon'>sentiment_very_dissatisfied</span>");
    response->print("<footer><p>This is a website and wether station completely hosted and controlled on my Arduino R4 WiFi! - CS</p></footer>");
    response->print("</body></html>");
}