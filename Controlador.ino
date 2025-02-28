#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinA = 2;
const int pinB = 3;
const int Ebutton = 4;

Encoder enc(pinA, pinB);

int menuIndex = 0;
long lastPosition = 0;

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
    lcd.print("Option 1");
    lcd.setCursor(0, 1);
    lcd.print("Option 2");
}

void loop()
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

        lcd.clear();
        switch (menuIndex)
        {
        case 0:
            lcd.setCursor(0, 0);
            lcd.print(">TEMPERATURA");
            lcd.setCursor(0, 1);
            lcd.print("MOTOR");
            break;
        case 1:
            lcd.setCursor(0, 0);
            lcd.print("TEMPERATURA");
            lcd.setCursor(0, 1);
            lcd.print(">MOTOR");
            break;
        default:
            break;
        }
        lastPosition = position;
    }
    if (digitalRead(Ebutton) == LOW)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        switch (menuIndex)
        {
        case 0:
            lcd.print("TEMPERATURA");
            break;
        case 1:
            lcd.print("MOTOR");
            break;
        default:
            break;
        }
        delay(1000);
    }

    delay(100);
}