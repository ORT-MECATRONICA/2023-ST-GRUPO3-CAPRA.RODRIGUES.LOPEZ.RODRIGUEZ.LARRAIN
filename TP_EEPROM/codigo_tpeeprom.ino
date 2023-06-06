#include <SPI.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
# define  DHTPIN  23      // Pin digital conectado al sensor DHT
// Feather HUZZAH ESP8266 nota: use los pines 3, 4, 5, 12, 13 o 14 --
// El pin 15 puede funcionar pero DHT debe desconectarse durante la carga del programa.
#define DHTTYPE DHT11

// ¡ Descomenta cualquier tipo que estés usando!q qqqa  1234
# define TIPO DHT DHT11    // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
#define PIN_BOTON_1 35
#define PIN_BOTON_2 34

int estadoMaquina;

#define MQ_P1 1
#define MQ_P2 0
#define MQ_E1 2
#define MQ_E2 3

#define MQ_SW1 4
#define MQ_SW2 5

#define MQ_E7 7

#define MQ_E6 6
float tempVieja;

int estadoPantalla2;
int valorUmbral = 0;

float t;
int valorSwitch1, valorSwitch2;

Preferences preferences;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_BOTON_1, INPUT_PULLUP);
  pinMode(PIN_BOTON_2, INPUT_PULLUP);
  dht.begin();
  preferences.begin("my-app", false);
  valorUmbral = preferences.getUInt("valorUmbral", 0);
  estadoMaquina = 0;
  Serial.println(F("Programa Iniciado"));

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

}

void loop() {

  valorSwitch1 = digitalRead (PIN_BOTON_1);
  valorSwitch2 = digitalRead (PIN_BOTON_2);

  Serial.println(estadoMaquina);
  switch (estadoMaquina) {



    case MQ_P2:///////////////////////////////////////////////////////////////////////////////
      {
        tempVieja = t;

        t = dht.readTemperature();
        // Read temperature as Fahrenheit (isFahrenheit = true)
        float f = dht.readTemperature(true);


        //delay(2000);////////////////////////////////////no usar
        if (tempVieja != t) {
          display.clearDisplay();
        }

        display.setTextSize(1);
        display.setTextColor (WHITE);
        display.setCursor(30, 0);
        display.println(F("% Temperature: "));
        display.println(t);
        display.println(F("Valor umbral: "));
        display.println (valorUmbral);
        display.display();


        if ( valorSwitch1 == LOW && valorSwitch2 == LOW) {

          estadoMaquina = MQ_E1;
          Serial.print("cambio");


        }
        break;
      }


    case MQ_E1:

      valorSwitch1 = digitalRead (PIN_BOTON_1);
      valorSwitch2 = digitalRead (PIN_BOTON_2);

      if ( valorSwitch1 == HIGH && valorSwitch2 == HIGH) {

        preferences.putUInt("valorUmbral", valorUmbral);
        estadoMaquina = MQ_P1;


      }
      break;

    case MQ_P1:

      display.setCursor(0, 0);
      display.clearDisplay();
      display.print("Valor umbral: ");
      display.println(valorUmbral);
      display.display();

      valorSwitch1 = digitalRead (PIN_BOTON_1);
      valorSwitch2 = digitalRead (PIN_BOTON_2);

      if (valorSwitch1 == LOW && valorSwitch2 == LOW ) {

        estadoMaquina = MQ_E2;


      }

      if (valorSwitch1 == LOW && valorSwitch2 == HIGH ) {

        estadoMaquina = MQ_SW1;

        Serial.print("cambiooooooooooooooooooooooooooooooooooooooo");

      }
      if (valorSwitch2 == LOW && valorSwitch1 == HIGH ) {

        estadoMaquina = MQ_SW2;

        Serial.print("cambio2222222222222222222");

      }

      break;


    case MQ_SW1:

      if (valorSwitch1 == HIGH && valorSwitch2 == HIGH ) {

        valorUmbral = valorUmbral + 1;
        Serial.print("cambio3");

        estadoMaquina = MQ_P1;

      }
      break;
    case MQ_SW2:
      {



        if ( valorSwitch1 == HIGH && valorSwitch2 == HIGH) {

          valorUmbral = valorUmbral - 1;

          estadoMaquina = MQ_P1;

        }
        break;
      }
    case MQ_E2:

      valorSwitch1 = digitalRead (PIN_BOTON_1);
      valorSwitch2 = digitalRead (PIN_BOTON_2);

      if ( valorSwitch1 == HIGH && valorSwitch2 == HIGH) {

        estadoMaquina = MQ_P2;

        Serial.print("cambizoszzzzzzzzzzzzzzz");
      }
      break;
  }
}
