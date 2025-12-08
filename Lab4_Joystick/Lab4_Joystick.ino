#include <SPI.h>
#include <RH_NRF24.h>

// Піни nRF24
#define CE_PIN 9
#define CSN_PIN 10

RH_NRF24 nrf24(CE_PIN, CSN_PIN);

// За документацією Funduino JoyStick Shield V1.A:
// D2 - A, D3 - B, D4 - C, D5 - D
const int BTN_A = 2;
const int BTN_B = 3;
const int BTN_C = 4;
const int BTN_D = 5;

// осі джойстика
const int JOY_X = A0;
const int JOY_Y = A1;

// зберігаємо останні настикання для реакції на зміни
uint8_t lastButtons = 0;
uint16_t lastX = 0;
uint16_t lastY = 0;

// поріг чутливості до змін
const uint16_t AXIS_THRESHOLD = 5;

// тут зсуваємо одиницю далі і далі в залежності від кнопки
uint8_t readButtonsMask() {
  uint8_t mask = 0;

  // bit0 -> D
  if (digitalRead(BTN_D) == LOW) {
    mask |= (1 << 0);
  }

  // bit1 -> A
  if (digitalRead(BTN_A) == LOW) {
    mask |= (1 << 1);
  }
  // bit2 -> B
  if (digitalRead(BTN_B) == LOW) {
    mask |= (1 << 2);
  }
  // bit3 -> C
  if (digitalRead(BTN_C) == LOW) {
    mask |= (1 << 3);
  }

  return mask;
}


void sendState(uint8_t buttons, uint16_t x, uint16_t y) {
  uint8_t packet[5] = { 0 };

  // 1 байт для кнопок
  packet[0] = buttons;

  // два байта під x, але вона двохбайтна uint16_t, то розбиваємо на high and low байт
  packet[1] = (x >> 8) & 0xFF;
  packet[2] = x & 0xFF;

  // теж і для y
  packet[3] = (y >> 8) & 0xFF;
  packet[4] = y & 0xFF;

  if (nrf24.send(packet, sizeof(packet))) {
    // надсилаємо пакет по радіоканалу
    nrf24.waitPacketSent();
    Serial.print("TX: buttons=0b");
    Serial.print(buttons, BIN);
    Serial.print(" X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);
  } else {
    Serial.println("Send failed");
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_C, INPUT_PULLUP);
  pinMode(BTN_D, INPUT_PULLUP);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  if (!nrf24.init()) {
    Serial.println("RH_NRF24 init failed!");
    while (1)
      ;
  }

  // УВАГА: той самий канал, що й у приймача!!!
  nrf24.setChannel(9);
  // УВАГА: швидкість даних і потужність
  nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm);

  Serial.println("Joystick TX ready (format: [buttons, X_hi, X_lo, Y_hi, Y_lo])");
}

void loop() {
  uint8_t buttons = readButtonsMask();
  uint16_t x = analogRead(JOY_X);  // 0..1023
  uint16_t y = analogRead(JOY_Y);  // 0..1023

  bool changed = false;

  if (buttons != lastButtons) {
    changed = true;
  }
  if (abs((int)x - (int)lastX) > AXIS_THRESHOLD) {
    changed = true;
  }
  if (abs((int)y - (int)lastY) > AXIS_THRESHOLD) {
    changed = true;
  }

  if (changed) {
    sendState(buttons, x, y);
    lastButtons = buttons;
    lastX = x;
    lastY = y;
  }

  // 50 Гц приблизно...
  delay(20);
}
