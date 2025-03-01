#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <Arduino.h>

#define THERMISTOR_R 10000
#define THERMISTOR_T 25
#define THERMISTOR_B 3950

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinA = 2;
const int pinB = 3;
const int Ebutton = 4;
const int thermistor = A0;

Encoder enc(pinA, pinB);

int menuIndex = 0;
long lastPosition = 0;

int temp = 0;
int lastTemp = 0;
int vel = 0;
int tempObj = 0;
int velObj = 0;

void setup()
{
    pinMode(Ebutton, INPUT_PULLUP);
    enc.write(0); // Reset
    lcd.init();
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
    lcd.setCursor(0, 0);
    lcd.print("Temperatura");
    lcd.setCursor(0, 1);
    lcd.print(getTemp());
}

void loop()
{
    temp = getTemp();
    lcd.setCursor(0, 1);
    if (abs(temp - lastTemp) > 2)
    {
        lcd.print(temp);
        lcd.setCursor(4, 1);
        lcd.print("/");
        lcd.setCursor(5, 1);
        lcd.print(tempObj);
        lastTemp = temp;
    }
    if (digitalRead(Ebutton) == LOW)
    {
        delay(50); // Debounce
        while (digitalRead(Ebutton) == LOW)
        {
        }
        handleMenuNavigation();

        if (digitalRead(Ebutton) == LOW)
        {
            delay(50); // Debounce
            while (digitalRead(Ebutton) == LOW)
            {
            }
            handleMenuSelection();
        }
    }
    delay(100);
}

int getTemp()
{
    int raw = analogRead(thermistor);
    float R = THERMISTOR_R * (1023.0 / raw - 1.0);
    float T = 1.0 / (1.0 / (THERMISTOR_T + 273.15) + 1.0 / THERMISTOR_B * log(R / 10000.0));
    return T - 273.15;
}

void handleMenuNavigation()
{
    long position = enc.read();
    if (position != lastPosition)
    {
        if (position > lastPosition)
        {
            menuIndex = 1;
        }
        else
        {
            menuIndex = 0;
        }
        updateDisplay();
        lastPosition = position;
    }
}

void updateDisplay()
{
    lcd.clear();
    switch (menuIndex)
    {
    case 0:
        lcd.setCursor(0, 0);
        lcd.print("> Temperatura");
        lcd.setCursor(0, 1);
        lcd.print("Velocidad");
        break;
    case 1:
        lcd.setCursor(0, 0);
        lcd.print("Temperatura");
        lcd.setCursor(0, 1);
        lcd.print("> Velocidad");
        break;
    default:
        break;
    }
}

void handleMenuSelection()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (menuIndex)
    {
    case 0:
        lcd.print("Temperatura");
        adjustValue(tempObj, 't');
        break;
    case 1:
        lcd.print("Velocidad");
        adjustValue(velObj, 'v');
        break;
    default:
        break;
    }
}

void adjustValue(int &value, char type)
{
    lcd.setCursor(0, 1); // Posición inicial para mostrar el valor
    lcd.print("    ");   // Limpia el área del valor anterior

    // Muestra el valor inicial
    lcd.setCursor(0, 1);
    lcd.print(value);

    // Muestra la unidad correspondiente
    switch (type)
    {
    case 't': // Temperatura
        lcd.print("C");
        break;
    case 'v': // Velocidad
        lcd.print("%");
        drawProgressBar(value); // Dibuja la barra de progreso
        break;
    default:
        break;
    }

    while (true)
    {
        long position = enc.read(); // Lee la posición del encoder
        if (position != lastPosition)
        {
            if (position > lastPosition)
            {
                if (value == (type == 't' ? 220 : 100))
                {                                  // Límite superior
                    value = (type == 't' ? 0 : 0); // Reinicia al mínimo
                }
                else
                {
                    value++;
                }
            }
            else
            {
                if (value <= (type == 't' ? 0 : 0))
                {                                      // Límite inferior
                    value = (type == 't' ? 220 : 100); // Reinicia al máximo
                }
                else
                {
                    value--;
                }
            }

            // Actualiza el valor en la pantalla
            lcd.setCursor(0, 1);
            lcd.print("    "); // Limpia el área del valor anterior
            lcd.setCursor(0, 1);
            lcd.print(value);

            // Muestra la unidad y la barra de progreso (si es velocidad)
            switch (type)
            {
            case 't':
                lcd.print("C");
                break;
            case 'v':
                lcd.print("%");
                drawProgressBar(value); // Actualiza la barra de progreso
                break;
            default:
                break;
            }

            lastPosition = position; // Actualiza la última posición del encoder
        }

        // Si se presiona el botón, guarda el valor y sale del bucle
        if (digitalRead(Ebutton) == LOW)
        {
            delay(50); // Debounce
            while (digitalRead(Ebutton) == LOW)
            {
            } // Espera a que se suelte el botón
            lcd.clear();
            lcd.print("Guardado!");
            delay(1000); // Muestra el mensaje por 1 segundo
            break;
        }
    }
}

// Función para dibujar una barra de progreso horizontal
void drawProgressBar(int percent)
{
    const int barLength = 16;                              // Longitud máxima de la barra (ajusta según tu LCD)
    int filledLength = map(percent, 0, 100, 0, barLength); // Calcula la longitud llena

    lcd.setCursor(0, 2); // Posición para la barra de progreso (segunda fila)
    for (int i = 0; i < barLength; i++)
    {
        if (i < filledLength)
        {
            lcd.write(255); // Carácter de bloque lleno (depende de tu LCD)
        }
        else
        {
            lcd.write(' '); // Espacio en blanco para la parte vacía
        }
    }
}