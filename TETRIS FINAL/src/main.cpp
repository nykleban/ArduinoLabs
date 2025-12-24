#include <Arduino.h>
#include <LedControl.h>
#include "components/TetrisGame.h"
#include "components/Display.h"
#include "components/SoundSystem.h"
#include "components/InputSystem.h"
// #include "components/Music.h"


#define DIN_PIN 6     
#define CLK_PIN 5     
#define CS_PIN 3      
#define BUZZER_PIN 8
#define VRx A0 
#define VRy A1 
#define SW 7  

#define MODULES 4     
#define SCREEN_W 8

const int GND_PIN10 = 10;
const int GND_PIN11 = 11;


// TetrisMusic music(BUZZER_PIN);


LedControl lc(DIN_PIN, CLK_PIN, CS_PIN, MODULES);
TetrisDisplay display(lc, MODULES, SCREEN_W);
SoundSystem sound(BUZZER_PIN);
InputSystem input(VRx, VRy, SW);

TetrisGame game(display, sound, input, lc);

void setup() {
  Serial.begin(9600);
  sound.init();
  long rand = random(0, 1000);
  randomSeed(analogRead(rand)); 
  game.setup();
  pinMode(GND_PIN10, OUTPUT);
  pinMode(GND_PIN11, OUTPUT);
  digitalWrite(GND_PIN10, LOW);
  digitalWrite(GND_PIN11, LOW);
}

void loop() {

  game.update();
  // music.update();
}