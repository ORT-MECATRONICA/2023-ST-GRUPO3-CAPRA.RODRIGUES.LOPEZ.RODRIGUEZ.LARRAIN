#include <Arduino.h>
#include <ESP32Time.h>
#include <Wire.h>   
#include "time.h"
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//********************//
//* DECLARO DE FUN                                        **//
//********************//
void pedir_lahora(void);
void setup_rtc_ntp(void);

struct tm timeinfo;
ESP32Time rtc;


/// time
long unsigned int timestamp; // hora
const char *ntpServer = "south-america.pool.ntp.org";
const long gmtOffset_sec = -10800;
const int daylightOffset_sec = 0;

const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";

void setup() {
  
Serial.begin(115200);
  Serial.println("Connecting to Wi-Fi...");
 initWiFi();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
   delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("hola, mundo");
  display.display(); 
setup_rtc_ntp();

}

void loop() {
 pedir_lahora();
delay (2000);

}

// Initialize WiFi
void initWiFi() {
  WiFi.begin(ssid , password );
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}


//********************//
//* SETUP  rtc                                           **//
//********************//
void setup_rtc_ntp(void)
{
  // init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  timestamp = time(NULL);
  rtc.setTime(timestamp + gmtOffset_sec);
}

//********************//
//* PIDE LA HORA AL SERVER O AL ESP32                     **//
//********************//
void pedir_lahora(void)
{
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("veo la hora del rtc interno ");
    timestamp = rtc.getEpoch() - gmtOffset_sec;
    timeinfo = rtc.getTimeStruct();
     Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
  else
  {
    Serial.print("NTP Time:");
    timestamp = time(NULL);
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }


 display.clearDisplay();
display.setTextSize (1);
display.setTextColor (WHITE);

display.setCursor (0, 10);
display.println (&timeinfo, "%A, %B %d");
display.setCursor (0, 30);
display.println (&timeinfo, "%Y ");
display.setCursor (0, 50);
display.println (&timeinfo, "%H:%M:%S");
display.display ();

  return;  
}
