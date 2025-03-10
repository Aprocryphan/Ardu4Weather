// Code generated by Arduino IoT Cloud

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>
#include "arduino_secrets.h"

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)

String formattedC;
String formattedHumdiditySensor;
String formattedOutC;
String formattedOutHumdiditySensor;
String formattedPressureSensor;

void initProperties(){
  ArduinoCloud.addProperty(formattedC, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(formattedHumdiditySensor, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(formattedOutC, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(formattedOutHumdiditySensor, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(formattedPressureSensor, READ, ON_CHANGE, NULL);
  ArduinoCloud.updateCertificate(CERT_AUTHORITY, CERT_SERIAL, CERT_NOT_BEFORE, CERT_NOT_AFTER, CERT_SIGNATURE_RS);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);