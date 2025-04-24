// External Variables and Constants
//extern const int placeholder;
//extern const int placeholder;
extern const int magneticSensor;
extern const int redLED; //PWM
extern const int piezo;
extern const int yellowLED; //PWM
extern const int greenLED; //PWM
//extern const int placeholder;
//extern const int placeholder;
extern const int cyanLED; //PWM
extern const int blueLED; //PWM
extern const int whiteLED; //PWM
//extern const int placeholder;
//extern const int placeholder;

extern const int TempSensor;
extern const int microphoneSensor;
//extern const int Placeholder;
extern const int LightSensor;
//extern const int Placeholder;
//extern const int Placeholder;

// Initialisation of variables used later
extern const int sampleWindow;  // Sample window width in mS (50 mS = 20Hz)
extern const int NUM_READINGS; // 24 hours of data, one reading every 5 minute
extern unsigned int signalMax;
extern unsigned int signalMin;
extern unsigned int sample; // Sample value from the pressure sensor
extern unsigned int peakToPeak;   // peak-to-peak level
extern int screensaverActive;
extern int network; // Initial network connection attempt for switch case
extern int OLEDPanel; // Initial OLED Panel
extern int readingIndex; // Keep track of the current position in the array
extern int SensorPreviousMillis;
extern int formattedLightSensorData;
extern int formattedMicrophoneSensor;
extern int signalStrength;
extern int formattedMagnetSensor;
extern int inTempDisplacement;
extern int outTempDisplacement;
extern int lightDisplacement;
extern int inHumidityDisplacement;
extern int outHumidityDisplacement;
extern int pressureDisplacement;
extern int altitudeDisplacement;
extern int noiseDisplacement;
extern int oldestIndex;
extern int whiteLightness;
extern int tPosition;

extern const float offset; // Offset For Temp Sensor (adjust as needed)
extern const float gain; // Gain For Temp Sensor (adjust as needed)
extern const float seaLevelPressure;
extern float Temp;
extern float TempAverage; // Stores average temperature
extern float LightAverage; // Stores average light level
extern float deltaP;
extern float DP24;
extern float pressure;
extern float formattedC;
extern float formattedF;
extern float formattedOutF;
extern float formattedHumiditySensor;
extern float formattedPressureSensor;
extern float formattedOutC;
extern float formattedOutHumiditySensor;
extern float altitude;

extern unsigned long long previousMillis; // for NTPUpdate function
extern unsigned long long OLEDPreviousMillis; // For OLED Panel
extern unsigned long long DPPreviousMillis;
extern unsigned long long secondsOnline;
extern unsigned long long screensaverMillis;
extern unsigned long startMillis; // Start of sample window

extern const char CRed[13];
extern const char CGreen[13];
extern const char COrange[13];
extern const char CBlue[13];
extern const char CPurple[13];
extern const char CCyan[13];
extern const char CWhite[13];
extern const char Cblack[13];
extern const char Cred[13];
extern const char Cgreen[13];
extern const char Cyellow[13];
extern const char Cblue[13];
extern const char Cmagenta[13];
extern const char Ccyan[13];
extern const char Cbright_black[13];
extern const char Cbright_red[13];
extern const char Cbright_green[13];
extern const char Cbright_yellow[13];
extern const char Cbright_blue[13];
extern const char Cbright_magenta[13];
extern const char Cbright_cyan[13];
extern const char Cbright_white[13];
extern const char Ccyan_back[13];
extern const char Cpurple_back[13];
extern const char Cwhite_back[13];
extern const char Cblue_back[13];
extern const char Corange_back[13];
extern const char Cgreen_back[13];
extern const char Cpink_back[13];
extern const char Cgrey_back[13];
extern const char Cgrey[13];
extern const char Cbold[13];
extern const char Cunderline[13];
extern const char Citalic[13];
extern const char Cdarken[13];
extern const char Cinvisible[13];
extern const char Creverse[13];
extern const char Creset[13];
extern const char Cgrey_old[13];
extern const char* weatherEmojis[];
extern char serialOutputBuffer[256]; // Buffer for serial output
extern char formattedTime[23]; // XX-XX-XX / XX:XX:XX\0
extern char dateOnly[11]; // XX-XX-XX\0
extern char timeOnly[9]; // XX:XX:XX\0
extern char hoursOnline[10]; // XX.XX\0
extern char daysOnline[10]; // XX.XX\0
extern char localIP[21]; // XXX.XXX.XXX.XXX\0
extern char subnetMask[21]; // XXX.XXX.XXX.XXX\0
extern char gatewayIP[21]; // XXX.XXX.XXX.XXX\0
extern char NTPIP[21]; // XXX.XXX.XXX.XXX\0
extern char data[128]; // <START>XX-XX-XX,XX:XX:XX,XX.X,XX.X,XXX,XX.X,XX.X,XX.X,XXX,XXXXX<END>\0