#pragma once
#include <Arduino.h>
#include "DebButton.h"

class InputSystem {
private:
    int pinVRx, pinVRy;
    DebButton btnRotate;

public:
    InputSystem(int vrx, int vry, int sw) : pinVRx(vrx), pinVRy(vry), btnRotate(sw) {}

    void init() {
        btnRotate.attach();
    }

    // Повертає -1 (ліво), 1 (право), 0 (нема руху)
    int getHorizontalMove() {
        int val = analogRead(pinVRx);
        if (val < 400) return 1;
        if (val > 600) return -1;
        return 0;
    }
    int getDropSpeed() {
        int val = analogRead(pinVRy);
        int speed = map(val, 512, 1023, 0, 690);
        speed = constrain(speed, 0, 690);
        return 700 - speed;
    }

    bool isRotatePressed() {
        return btnRotate.isClickedOnce();
    }

    void waitForClick() {
        btnRotate.waitForButtonPress();
    }
};