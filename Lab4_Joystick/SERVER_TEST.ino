#include <SPI.h>
#include <RH_NRF24.h>
#include <ServoTimer2.h>

#define CE_PIN 7
#define CSN_PIN 8

RH_NRF24 nrf24(CE_PIN, CSN_PIN);

ServoTimer2 servoL;
ServoTimer2 servoR;

const int minPulse = 0;
const int maxPulse = 2250;
const int stopPulse = 1125;
const int pogrishnist = 200;

// змінні для автопілоту
const int autopilotSpeed = 500;
const unsigned long timer3000 = 3000;
const unsigned long timer1000 = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println("Server start - 2WD Arcade Drive");

  servoL.attach(A0);
  servoR.attach(A1);
  servoL.write(stopPulse);
  servoR.write(stopPulse);

  if (!nrf24.init()) {
    Serial.println("RH_NRF24 init failed!");
    while (1)
      ;
  }

  nrf24.setChannel(9);
  nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm);

  Serial.println("Server ready, waiting for packets...");
}

void printButtons(uint8_t buttons) {
  bool D = buttons & (1 << 0);
  bool A = buttons & (1 << 1);
  bool B = buttons & (1 << 2);
  bool C = buttons & (1 << 3);

  Serial.print("BTN:[");
  Serial.print(A);
  Serial.print(B);
  Serial.print(C);
  Serial.print(D);
  Serial.print("] ");
}

// Функція для перетворення швидкості з -512..512   в  0..2250
int speedToPulse(int val) {
  val = constrain(val, -512, 512);
  return map(val, -512, 512, minPulse, maxPulse);
}

void setMotors(int direction, int turn) {
  // лівий серво = напрям + поворот
  // правий серво = напрям - поворот
  int valL = direction + turn;
  int valR = direction - turn;
  int pulseL = speedToPulse(valL);
  int pulseR = 2250 - speedToPulse(valR);
  servoL.write(pulseL);
  servoR.write(pulseR);
}

void autopilot() {
  Serial.println("AUTOPILOT START");

  int direction, turn;
  int valL, valR;
  int pulseL, pulseR;

  // вперед на 3 с
  direction = autopilotSpeed;
  turn = 0;
  setMotors(direction, turn);
  delay(timer3000);

  // направо
  direction = 0;
  turn = autopilotSpeed;
  setMotors(direction, turn);
  delay(timer1000);

  // назад на 3 с
  direction = -autopilotSpeed;
  turn = 0;
  setMotors(direction, turn);
  delay(timer3000);

  // наліво
  direction = 0;
  turn = -autopilotSpeed;
  setMotors(direction, turn);
  delay(timer1000);

  // вперед на 3 с
  direction = autopilotSpeed;
  turn = 0;
  setMotors(direction, turn);
  delay(timer3000);

  servoL.write(stopPulse);
  servoR.write(stopPulse);

  Serial.println("AUTOPILOT END");
}

void loop() {
  if (nrf24.available()) {
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN] = { 0 };
    uint8_t len = sizeof(buf);

    if (nrf24.recv(buf, &len)) {
      if (len == 5) {
        uint8_t buttons = buf[0];

        printButtons(buttons);

        // Якщо натиснута кнопка A — запускаємо автопілот і виходимо з loop()
        if (buttons & (1 << 1)) {  // кнопка A
          autopilot();
          return;  // після автопілота чекаємо нові пакети
        }

        uint16_t X = ((uint16_t)buf[1] << 8) | buf[2];
        uint16_t Y = ((uint16_t)buf[3] << 8) | buf[4];

        // -512 .. +512
        // ця дія робиться для коректної дії рухів вперед назад та вліво вправо, щоб простіше задавати логіку
        int direction = (int)Y - 512;  // вперед назад Y
        int turn = (int)X - 512;       // вліво вправо X

        // для стопу
        if (abs(direction) < pogrishnist) direction = 0;
        if (abs(turn) < pogrishnist) turn = 0;

          int valL = direction + turn;
          int valR = direction - turn;
          int pulseL = speedToPulse(valL);
          int pulseR = 2250 - speedToPulse(valR);
          servoL.write(pulseL);
          servoR.write(pulseR);

        Serial.print(" Y:");
        Serial.print(direction);
        Serial.print(" X:");
        Serial.print(turn);
        Serial.print("         | L:");
        Serial.print(pulseL);
        Serial.print(" R:");
        Serial.println(pulseR);
      } else {
        Serial.println("Error: Wrong packet length");
      }
    }
  }
}
