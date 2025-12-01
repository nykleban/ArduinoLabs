#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// ноти для мелодії нокії
#define NOTE_E5 659
#define NOTE_D5 587
#define NOTE_FS4 370
#define NOTE_GS4 415
#define NOTE_CS5 554
#define NOTE_B4 494
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_A4 440
#define NOTE_CS4 277
#define REST 0

RTC_DS1307 rtc;
const int buzzer = 4;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 LCD(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Голова кріпера 32x32 пікселі
const unsigned char creeperFace[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F,
  0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F, 0xF0, 0x0F,
  0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF,
  0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF,
  0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF,
  0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

int tempo = 180;
int melody[] = {
  NOTE_E5,
  8,
  NOTE_D5,
  8,
  NOTE_FS4,
  4,
  NOTE_GS4,
  4,
  NOTE_CS5,
  8,
  NOTE_B4,
  8,
  NOTE_D4,
  4,
  NOTE_E4,
  4,
  NOTE_B4,
  8,
  NOTE_A4,
  8,
  NOTE_CS4,
  4,
  NOTE_E4,
  4,
  NOTE_A4,
  2,
};

//змінні з допоміжного коду з музикою
bool melodyPlaying = false;
unsigned long noteStartTimestamp = 0;
int thisNote = 0;
int noteDuration = 0;
int wholenote = 0;


// функції з допоміжного коду з музикою
void startMelody() {
  melodyPlaying = true;
  thisNote = 0;
  noteStartTimestamp = millis();
  wholenote = (60000 * 4) / tempo;
  noteDuration = 0;
}
void updateMelody() {
  if (!melodyPlaying) return;
  unsigned long currentMillis = millis();
  if (currentMillis - noteStartTimestamp >= noteDuration) {
    noTone(buzzer);
    int totalNotes = sizeof(melody) / sizeof(melody[0]);
    if (thisNote >= totalNotes) {
      melodyPlaying = false;
      return;
    }
    int note = melody[thisNote];
    int divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5;
    }
    if (note != REST) {
      tone(buzzer, note, noteDuration * 0.9);
    }
    noteStartTimestamp = currentMillis;
    thisNote += 2;
  }
}

void printDigitsRight(int number) {
  if (number < 10) {
    LCD.print("0"); // це щоб не було чогось типу: 1:14:2, а було акуратно 01:14:02
  }
  LCD.print(number);
}

void drawScreen(DateTime now) {
  LCD.clearDisplay();
  LCD.setCursor(16, 2);
  printDigitsRight(now.hour());
  LCD.print(":");
  printDigitsRight(now.minute());
  LCD.print(":");
  printDigitsRight(now.second());

  if (now.second() % 2 == 0) {
    LCD.drawBitmap(48, 24, creeperFace, 32, 32, 1);  // X початок, Y початок,масив, ширина, висота, колір
  }
  LCD.display();
}

void setup() {
  Wire.begin();
  pinMode(buzzer, OUTPUT);
  
  LCD.setTextSize(2);
  LCD.setTextColor(SSD1306_WHITE);

  if (!rtc.begin()) {
    LCD.clearDisplay();
    LCD.setCursor(16, 2);
    LCD.print("error with RTC");
    LCD.display();
    while (true);
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  DateTime now = rtc.now();

  if (now.second() == 0 && !melodyPlaying) {
    startMelody();
  }

  updateMelody();
  drawScreen(now);
}