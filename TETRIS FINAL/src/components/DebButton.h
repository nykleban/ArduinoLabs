#pragma once
#include <Arduino.h>

class DebButton
{
private:
    int pin;

public:
    DebButton(int pin)
    {
        this->pin = pin;
    }

    ~DebButton() {}

    void attach()
    {
        pinMode(pin, INPUT_PULLUP);
    }

    bool isClickedOnce()
    {
        // Перевіряємо на LOW, бо кнопка інвертована
        if (digitalRead(pin) == LOW)
        {
            delay(20); 
            if (digitalRead(pin) == LOW)
            {
                while (digitalRead(pin) == LOW)
                {
                    delay(1);
                }
                return true;
            }
        }
        return false;
    }
    
    void waitForButtonPress()
    {   
        // Чекаємо натискання (поки не стане LOW)     
        while (digitalRead(pin) != LOW)
            delay(10);
        // Чекаємо відпускання (поки не стане HIGH)
        while (digitalRead(pin) == LOW)
            delay(10);
    }
};