#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <ArduinoJson.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DHTPIN 23
#define DHTTYPE DHT11
#define TIPO DHT DHT11
DHT dht(DHTPIN, DHTTYPE);
#define PIN_BOTON1 34
#define PIN_BOTON2 35
#define MQ_P1 0
#define E1_E1 1
#define E2_P1 2
#define E3_P1 3
#define E4_P1 4
#define E5_P1 5
#define MQ_P2 6
#define SUMAR 7
#define RSTAR 8
#define E6_P1 9
#define BOTtoken "6201286500:AAE6WTNsWuvZd3YvNTppeqXnI-b1sm9Rgvw"  // cambiar por token 
#define CHAT_ID "5111161382"
const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";
int estadoMaquinaPantallas;
int valorSwitch1;
int valorSwitch2;
int valorUmbral = 24;
String mensaje = "temperatura actual= ";
int delayRobot = 1000; /// intervalo
 unsigned long lastTimeBotRan; /// ultimo tiempo
float temp;
unsigned long msA;
unsigned long msP;
unsigned long msA2;
unsigned long msP2;
boolean flagTemperatura;
DHT_Unified dht(DHTPIN, DHTTYPE);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

sensor_t sensor;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {

  pinMode(BOTON_1, INPUT_PULLUP);
  pinMode(BOTON_2, INPUT_PULLUP);
  Serial.begin(115200);
  dht.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectandose");
  }

  Serial.println(WiFi.localIP());
  bot.sendMessage(CHAT_ID, "conexion exitosa");
}


void loop() {

  Serial.println(estadoMaquina);
  valorSwitch1 = digitalRead(BOTON_1);
  valorSwitch2 = digitalRead(BOTON_2);
  msA = millis();
  msA2 = millis();
  if ((msA2 - msP2) >= 1000) {
    lecturaTempreatura();
    msP2 = msA2;
  }
  if (valorUmbral < 0) {
    valorUmbral = 0;
  }

  if (flagTemperatura == 0) {
    if (temp > valorUmbral) {
      flagTemperatura = 1;
      bot.sendMessage(CHAT_ID, "Se supero el valor umbral");
    }
  }
 

switch (estadoMaquinaPantallas) {


case MQ_P1:

  funcionP1();
  if (valorSwitch1 == LOW) {
    estadoMaquinaPantallas = E1_P1;
  }
  break;

case E1_P1:
  funcionP1();
  if (valorSwitch1 == HIGH) {
    estadoMaquinaPantallas = E2_P1;
  }
  break;

case E2_P1:

  funcionP1();

  if (valorSwitch2 == LOW) {
    estadoMaquinaPantallas = E3_P1;
  }
  break;
case E3_P1:
  funcionP1();
  if (valorSwitch2 == HIGH) {
    msP = msA;
    estadoMaquinaPantallas = E4_P1;
  }
  break;
case E4_P1:
  funcionP1();

  if (valorSwitch1 == LOW) {
    estadoMaquinaPantallas = E5_P1;
  }
  if ((msA - msP) >= 5000) {
    msP = msA;
    estadoMaquinaPantallas = MQ_P1;
  }
  break;

case E5_P1:
  funcionP1();
  if (valorSwitch1 == HIGH) {
    estadoMaquina = MQ_P2;
  }
  break;

case MQ_P2:
  funcionP2();

  if (valorSwitch1 == LOW) {
    estadoMaquinaPantalas = SUMAR;
  }

  if (valorSwitch2 == LOW) {
    estadoMaquinaPantalas = RESTAR;
  }

  if (valorSwitch1 == LOW &&  valorSwitch2 == LOW) {
    estadoMaquinaPantallas= E6_P1;
  }
  break;

case SUMAR:
  funcionP2();
  if (valorSwitch1 == LOW && valorSwitch2 == LOW) {
    estadoMaquinaPantallas = E6_P1;
  }
  if (valorSwitch1 == HIGH) {
    valorUmbral = valorUmbral + 1;
    estadoMaquinaPantallas = MQ_P2;
  }
  break;
case RESTAR:

  funcionP2();

  if (valorSwitch1 == LOW && valorSwitch2 == LOW) {
    estadoMaquinaPantallas = E6_P1;
  }

  if (valorSwitch2 == HIGH) {
    valorUmbral = valorUmbral - 1;
    estadoMaquinaPantallas = MQ_P2;
  }
  break;
case E6_P1:
  funcionP2();
  if (valorSwitch1 == HIGH && valorSwitch2 == HIGH) {
    estadoMaquinaPantallas = MQ_P1;
  }
  break;
}
lecturaTiempo ();

}

void lecturaTiempo () {

  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
void handleNewMessages(int numNewMessages) {
  Serial.println("Mensaje nuevo");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // inicio de verificacion
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {  ////si el id no corresponde da error . en caso de que no se quiera comprobar el id se debe sacar esta parte
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String text = bot.messages[i].text;
    Serial.println(text);
    String from_name = bot.messages[i].from_name;
    if (text == "/temperatura_actual") {
      bot.sendMessage(chat_id, (String)temp, "");
    }
  }
}

void funcionP1() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temperatura Actual: ");
  display.println(temp);
  display.println("Valor Umbral: ");
  display.print(valorUmbral);
  display.display();
}

void funcionP2() {

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Valor Umbral: ");
  display.println(valorUmbral);
  display.display();

}

void lecturaTempreatura() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);
  temp = event.temperature;

}
