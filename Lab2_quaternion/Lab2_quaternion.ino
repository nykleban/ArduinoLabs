#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include <Servo.h>

MPU6050 mpu;
Servo servoRoll, servoPitch, servoYaw;

#define PIN_ROLL 4
#define PIN_PITCH 5
#define PIN_YAW 10
#define INTERRUPT_PIN 2

bool DMPReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
float ypr[3];

float smoothRoll = 90, smoothPitch = 90, smoothYaw = 90;
float alpha = 0.1;                // коефіцієнт згладжування
unsigned long lastPrintTime = 0;  // для виводу в Serial

// переривання
volatile bool mpuInterrupt = false;
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  servoRoll.attach(PIN_ROLL);
  servoPitch.attach(PIN_PITCH);
  servoYaw.attach(PIN_YAW);
  servoRoll.write(90);
  servoPitch.write(90);
  servoYaw.write(90);
  Serial.println(F("Init MPU..."));
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  Serial.println(mpu.testConnection() ? F("MPU6050 OK") : F("MPU6050 FAIL"));
  devStatus = mpu.dmpInitialize();

  // калібрування
  mpu.setXGyroOffset(0);
  mpu.setYGyroOffset(0);
  mpu.setZGyroOffset(0);
  mpu.setZAccelOffset(0);

  if (devStatus == 0) {
    Serial.println(F("Calibrating..."));
    mpu.CalibrateAccel(6);
    mpu.CalibrateGyro(6);
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    packetSize = mpu.dmpGetFIFOPacketSize();
    DMPReady = true;
    Serial.println(F("DMP Ready!"));
  } else {
    Serial.print(F("DMP Fail: "));
    Serial.println(devStatus);
  }

  // від зависання I2C
  Wire.setWireTimeout(3000, true);
  // Це тайм-аут (захист від повного зависання). Коли сервоприводи стартували, вони створювали шум,
  // який "блокував" зв'язок до Arduino. Без цього рядка Arduino буде вічно чекати відповіді від датчика в циклі.
  // А ця команда працює щось типу watch dog,
  // бо якщо зв'язок зник на 3 мілісекунди то просто вийде з циклу і продовжить роботу.
}

void loop() {
  if (!DMPReady) return;

  // переривання або повний пакет даних
  if (mpu.dmpPacketAvailable()) {

    mpuIntStatus = mpu.getIntStatus();
    fifoCount = mpu.getFIFOCount();

    // перевірка на переповнення буфера (FIFO overflow)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
      // 0x10 в документації 4-й біт це FIFO_OFLOW_INT, тому це просто перевірка на оверфлов буферу, якщо
      // mpuIntStatus = mpu.getIntStatus();
      // дасть щось типу 10010010, то 10010010 і 00010000 дасть true, що буде значити, що буфер
      // переповнено; або(||) fifoCount == 1024, а 1024
      // це пам'ять всередині самого чіпа MPU6050.(або ми бачимо, що пам'ять повна)
      mpu.resetFIFO();
      // Serial.println(F("FIFO Overflow!"));
    }

    // обробка даних
    else if (mpuIntStatus & 0x02) {
      // Це перевірка на те, чи ми беремо готові дані.
      // Це пов'язано з роботою DMP, а саме щоб ми отримали дані вже після всіх розрахунків.
      // 0x02 означає - дані готові.
      
      // читання пакету даних з буфера
      while (fifoCount < packetSize)
        fifoCount = mpu.getFIFOCount();
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;

      // отримання кватерніонів
      mpu.dmpGetQuaternion(&q, fifoBuffer);
      mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

      // конвертація радіанів у градуси
      float targetYaw = map(ypr[0] * 180 / M_PI, -180, 180, 180, 0);
      float targetPitch = map(ypr[1] * 180 / M_PI, -90, 90, 180, 0);
      float targetRoll = map(ypr[2] * 180 / M_PI, -90, 90, 0, 180);

      smoothYaw = (smoothYaw * (1.0 - alpha)) + (targetYaw * alpha);
      smoothPitch = (smoothPitch * (1.0 - alpha)) + (targetPitch * alpha);
      smoothRoll = (smoothRoll * (1.0 - alpha)) + (targetRoll * alpha);
      servoYaw.write(constrain((int)smoothYaw, 0, 180));
      servoPitch.write(constrain((int)smoothPitch, 0, 180));
      servoRoll.write(constrain((int)smoothRoll, 0, 180));

      if (millis() - lastPrintTime > 200) {
        Serial.print("Y:");
        Serial.print((int)targetYaw);
        Serial.print("\tP:");
        Serial.print((int)targetPitch);
        Serial.print("\tR:");
        Serial.println((int)targetRoll);
        lastPrintTime = millis();
      }
    }
  }
}