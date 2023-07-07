

#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"


// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


// Insert your network credentials
#define WIFI_SSID "ORT-IoT"
#define WIFI_PASSWORD "OrtIOTnew22$2"


// Insert Firebase project API Key
#define API_KEY "AIzaSyB2kBgbPqVV_1NfMC4hfCqvWhbkbFN4cYk"


// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "finaltrabajo24@gmail.com"
#define USER_PASSWORD "proyecto10"


// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://database-pf-default-rtdb.firebaseio.com/"



// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// Variable to save USER UID
String uid;


// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String estadoBoton = "/estadoBoton";
String timePath = "/timestamp";


// Parent Node (to be updated in every loop)
String parentPath;


int timestamp;
FirebaseJson json;


const char* ntpServer = "pool.ntp.org";
#define PIN_BOTON 32
int modoBoton;
int lecturaBoton;
bool estadoFirebase;
int valorBoton;
// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 500;




// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}


// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}


void setup() {
  Serial.begin(115200);


  // Initialize BME280 sensor
  initWiFi();
  configTime(0, 0, ntpServer);


  // Assign the api key (required)
  config.api_key = API_KEY;


  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;


  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;


  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  pinMode(PIN_BOTON, INPUT_PULLUP);
  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h


  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;


  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);


  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);


  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
}


void loop() {


  // Send new readings to database
  lecturaBoton = digitalRead(PIN_BOTON);

  /* if (lecturaBoton == 0 && valorBoton == 0) {//para evitar repeticiones
    modoBoton = 1;
    
  }

  if (lecturaBoton == 1 && valorBoton == 1) {
      modoBoton = 0;
    }*/

  if (lecturaBoton == 0) {
    estadoFirebase = 1;
    Serial.println(estadoFirebase);
  }
  if (lecturaBoton == 1) {
    Serial.println(estadoFirebase);
  }

  Serial.println(estadoFirebase);


  if (estadoFirebase == 1) {
    firebase();
    estadoFirebase = 0;
  }


  // delay(500);
}


///ver si el bton esta presioando  flag 1  . por otro lador si flag esta en 1 entonces envio la info y flag =0


void firebase() {
   if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();


    //Get current timestamp
    timestamp = getTime();
    Serial.print("time: ");
    Serial.println(timestamp);


    parentPath = databasePath + "/" + String(timestamp);


    //json.set(tempPath.c_str(), String(bme.readTemperature()));
    //json.set(humPath.c_str(), String(bme.readHumidity()));
    //json.set(presPath.c_str(), String(bme.readPressure()/100.0F));


  Serial.println("boton presionado");
  json.set(estadoBoton.c_str(), String(modoBoton));
  //json.set(timePath, String(timestamp));
  Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
}
}