#include <Arduino.h>
#include <PID_v1.h>
#include <SSD_13XX.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define RELAY 6
#define POT A0
#define THERM A1

// Parámetros PID ajustables
double Kp = 30, Ki = 5, Kd = 50;
double Setpoint, Temp, Output;

PID myPID(&Temp, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
SSD_13XX display(SCREEN_WIDTH, SCREEN_HEIGHT);

unsigned long lastUpdate = 0; // Para evitar delays innecesarios

void setup()
{
    Serial.begin(9600);
    Serial.println("Iniciando sistema...");

    myPID.SetMode(AUTOMATIC);
    pinMode(RELAY, OUTPUT);
    pinMode(POT, INPUT);
    pinMode(THERM, INPUT);

    // Apagar el relé al inicio
    digitalWrite(RELAY, LOW);

    // Inicializar pantalla
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("RePETeR Iniciado");
    display.display();
    delay(2000);
}

void loop()
{
    unsigned long now = millis();
    
    // Actualizar cada 500ms para mejor respuesta
    if (now - lastUpdate >= 500)
    {
        lastUpdate = now;

        // Ajustar Setpoint con el potenciómetro (rango 30°C a 300°C)
        Setpoint = map(analogRead(POT), 0, 1023, 30, 300);
        Setpoint = constrain(Setpoint, 30, 300);

        // Leer temperatura desde el termistor NTC (usando Steinhart-Hart)
        int adc = analogRead(THERM);
        float resistance = 100000.0 / ((1023.0 / adc) - 1);
        Temp = 1.0 / ((log(resistance / 100000.0) / 3950) + (1.0 / 298.15)) - 273.15;

        // Calcular salida PID
        myPID.Compute();

        // Control del relé basado en la salida PID
        if (Output > 127)
        {
            digitalWrite(RELAY, HIGH);
        }
        else
        {
            digitalWrite(RELAY, LOW);
        }

        // Mostrar información en pantalla sin borrar toda la pantalla (evita parpadeo)
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Setpoint: ");
        display.print(Setpoint);
        display.println(" C");
        display.print("Temp: ");
        display.print(Temp);
        display.println(" C");
        display.print("PID: ");
        display.print(Output);
        display.display();

        // Mostrar datos en Serial Monitor
        Serial.print("Setpoint: ");
        Serial.print(Setpoint);
        Serial.print(" °C | Temp: ");
        Serial.print(Temp);
        Serial.print(" °C | PID: ");
        Serial.println(Output);
    }
}