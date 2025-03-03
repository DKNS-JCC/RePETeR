#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Arduino.h>
#include <PID_v1.h>

#define THERMISTOR_R 10000
#define THERMISTOR_T 25
#define THERMISTOR_B 3950

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinA = 2;
const int pinB = 3;
const int Ebutton = 4;
const int thermistor = A0;
const int relay = 6;

Encoder enc(pinA, pinB);

long lastPosition = 0;
int newPosition = 0;

/*****************************************
ESTADOS DE
FUNCIONAMIENTO
0: Espera (revision de Temp) cambio si se presiona el boton
1: Cambio de Temp o Vel cambio si se presiona el boton (confirmacion)
*****************************************/
int state = 0;
int menuIndex = 1; // 0: Temp, 1: Vel

double temp = 0;
double lastTemp = 0;
double vel = 0;
double tempObj = 25;
double velObj = 25;

unsigned long tiempoAnterior = 0;
unsigned long interval = 500;
unsigned long currentMillis = 0;

// Crear caracter personalizado para el símbolo de grados
byte degree[] = {
    B00111,
    B00101,
    B00111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

void setup()
{
    //PIN SETUP
    pinMode(Ebutton, INPUT_PULLUP);
    pinMode(relay, OUTPUT);

    //INICIALIZACION DE LIBRERIAS
    Wire.begin();
    enc.write(0); // Reset
    lcd.init();
    lcd.createChar(0, degree);
    lcd.backlight();


    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RePETer");
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++)
    {
        lcd.print((char)255);
        delay(200);
    }
    lcd.clear();
    lcd.print("Temp ");
    lcd.print(getTemp());
    lcd.print("/");
    lcd.print(tempObj);
    lcd.write(byte(0));
    lcd.setCursor(0, 1);
    lcd.print("Vel ");
    lcd.print(velObj);
    lcd.print(" %");
}

void actualizarLCD() {
    lcd.setCursor(5, 0);
    lcd.print("        ");
    lcd.setCursor(5, 0);
    lcd.print(temp);
    lcd.print("/");
    lcd.print(tempObj);
    lcd.write(byte(0));

    lcd.setCursor(0, 1);
    if (state == 0) {
        if (menuIndex == 0) {
            lcd.print("> Temp ");
            lcd.print(tempObj);
            lcd.write(byte(0));
        } else {
            lcd.print("> Vel ");
            lcd.print(velObj);
            lcd.print(" %");
        }
    } else if (state == 1) {
        if (menuIndex == 0) {
            lcd.print("> Temp ");
            lcd.print(tempObj);
            lcd.write(byte(0));
        } else {
            lcd.print("> Vel ");
            lcd.print(velObj);
            lcd.print(" %");
        }
    }
}

void loop() {
    currentMillis = millis();

    // Actualización de temperatura cada cierto tiempo
    if (currentMillis - tiempoAnterior > interval) {
        tiempoAnterior = currentMillis;
        temp = getTemp();
        digitalWrite(relay, (temp < tempObj) ? HIGH : LOW);
        actualizarLCD();
    }

    // Cambio de estado con el botón
    if (digitalRead(Ebutton) == LOW) {
        delay(200);  // Anti-rebote
        state = (state + 1) % 2;
        actualizarLCD();
    }

    // Manejo del encoder
    newPosition = enc.read();
    if (newPosition - lastPosition > 2) {
        lastPosition = newPosition;

        if (state == 0) {
            menuIndex = (menuIndex + 1) % 2;  // Alternar entre 0 y 1
        } else if (state == 1) {
            if (menuIndex == 0) {
                tempObj = min(tempObj + 1, 220);
            } else {
                velObj = min(velObj + 1, 100);
            }
        }
        actualizarLCD();

    } else if (newPosition - lastPosition < -2) {
        lastPosition = newPosition;

        if (state == 0) {
            menuIndex = (menuIndex - 1 < 0) ? 1 : 0;  // Alternar entre 0 y 1
        } else if (state == 1) {
            if (menuIndex == 0) {
                tempObj = max(tempObj - 1, 0);
            } else {
                velObj = max(velObj - 1, 0);
            }
        }
        actualizarLCD();
    }
}

int getTemp()
{
    int raw = analogRead(thermistor);
    float R = THERMISTOR_R * ((1023.0 / raw) - 1.0);
    float T = 1.0 / (1.0 / (THERMISTOR_T + 273.15) + 1.0 / THERMISTOR_B * log(R / 10000.0));
    return T - 273.15;
}
