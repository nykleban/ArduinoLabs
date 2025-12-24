#pragma once
#include <Arduino.h>


#define NOTE_B2  123
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_B4  494
#define NOTE_C5  523

class SoundSystem {
private:
    int pin;

public:
    SoundSystem(int p) : pin(p) {}

    void init() {
        pinMode(pin, OUTPUT);
        noTone(pin);
    }

    void playIntro() {
        tone(pin, NOTE_B4, 200); 
        delay(200);        
        tone(pin, NOTE_C5, 250);
        delay(250);        
        
        tone(pin, NOTE_B4, 250);
        delay(250);
        
        tone(pin, NOTE_C5, 200);
        delay(250);
        
        tone(pin, NOTE_C5, 200);
        delay(200);
        
        noTone(pin);
    }

    void playMove() {
        tone(pin, NOTE_B3, 50);
    }

    void playBadMove() {
        tone(pin, NOTE_B2, 50);
    }

    void playLock() {
        tone(pin, NOTE_C4, 100);
    }

    void playLineClear() {
        tone(pin, NOTE_B4, 200);
        delay(200);
        tone(pin, NOTE_C5, 175);
        delay(200);
        noTone(pin);
    }

    void playGameOver() {
        tone(pin, NOTE_B2, 300);
        delay(300);
        tone(pin, NOTE_B2, 300);
        delay(300);
        noTone(pin);
    }

    void stop() {
        noTone(pin);
    }
};