#include <AFMotor.h>

class SmartCar {
private:
  AF_DCMotor MotorFR;
  AF_DCMotor MotorFL;
  AF_DCMotor MotorBL;
  AF_DCMotor MotorBR;

  // піни
  const int buzPin = A2;
  const int headingLed = A5;
  const int backLed = A4;
  const int sensorFrontPin = A0;
  const int sensorBackPin = A1;
  const int batteryPin = A3;

  // всі потрібні змінні
  int batteryThreshold = 800;      // Поріг розряду (80%)
  unsigned long previousMillis = 0;
  const long interval500 = 500;
  bool ledState = LOW;
  
  int valSpeed = 255;
  bool headlightsOn = false;

public:
  // єдиний дефолтний конструктор
  SmartCar() : MotorFR(2), MotorFL(3), MotorBL(4), MotorBR(1) {}

  // замість setup
  void init() {
    Serial.begin(9600);

    pinMode(headingLed, OUTPUT);
    pinMode(backLed, OUTPUT);
    pinMode(buzPin, OUTPUT);
    
    pinMode(sensorFrontPin, INPUT);
    pinMode(sensorBackPin, INPUT);
    pinMode(batteryPin, INPUT);

    setSpeed(valSpeed);
    stop();
    digitalWrite(buzPin, LOW);

    playStartupSound();
  }

  void run() {
    checkBattery(); 
    while (Serial.available() > 0) {
      char command = Serial.read();
      Serial.println(command); 
      processCommand(command);
    }
  }

private:
  void processCommand(char command) {
    switch (command) {
      case 'F': // Вперед
        obstacleAlert();
        if (isFrontBlocked()) stop();
        else moveForward();
        break;

      case 'B': // Назад
        obstacleAlert();
        if (isBackBlocked()) stop();
        else moveBack();
        break;

      case 'R': // Праворуч
        turnRight();
        break;

      case 'L': // Ліворуч
        turnLeft();
        break;

      case 'G': // Вперед-ліворуч
        if (isFrontBlocked()) { stop(); obstacleAlert(); }
        else moveForwardLeft();
        break;

      case 'H': // Вперед-праворуч
        if (isFrontBlocked()) { stop(); obstacleAlert(); }
        else moveForwardRight();
        break;

      case 'I': // Назад-ліворуч
        if (isBackBlocked()) { stop(); obstacleAlert(); }
        else moveBackwardLeft();
        break;

      case 'J': // Назад-праворуч
        if (isBackBlocked()) { stop(); obstacleAlert(); }
        else moveBackwardRight();
        break;

      case 'S': // Стоп
        stop();
        break;

      case 'Y': // Сигнал
        honkHorn();
        break;

      case 'U': // Фари ON
        turnOnHeadingLights();
        break;

      case 'u': // Фари OFF
        turnOffHeadingLights();
        break;

      // Швидкість
      case '1': setSpeed(65); break;
      case '2': setSpeed(130); break;
      case '3': setSpeed(195); break;
      case '4': setSpeed(255); break;

      default: 
        break;
    }
  }

  
  void setSpeed(int val) {
    valSpeed = val;
    MotorFL.setSpeed(val);
    MotorFR.setSpeed(val);
    MotorBL.setSpeed(val);
    MotorBR.setSpeed(val);
  }

  void moveForward() {
    digitalWrite(backLed, LOW);
    setSpeed(valSpeed); 
    MotorFL.run(FORWARD);
    MotorFR.run(FORWARD);
    MotorBL.run(FORWARD);
    MotorBR.run(FORWARD);
  }

  void moveBack() {
    digitalWrite(backLed, HIGH);
    setSpeed(valSpeed);
    MotorFL.run(BACKWARD);
    MotorFR.run(BACKWARD);
    MotorBL.run(BACKWARD);
    MotorBR.run(BACKWARD);
  }

  void stop() {
    digitalWrite(backLed, HIGH);
    MotorFL.run(RELEASE);
    MotorFR.run(RELEASE);
    MotorBL.run(RELEASE);
    MotorBR.run(RELEASE);
  }

  void turnRight() {
    digitalWrite(backLed, LOW);
    setSpeed(valSpeed);
    MotorFL.run(FORWARD);
    MotorBL.run(FORWARD);
    MotorFR.run(BACKWARD);
    MotorBR.run(BACKWARD);
  }

  void turnLeft() {
    digitalWrite(backLed, LOW);
    setSpeed(valSpeed);
    MotorFL.run(BACKWARD);
    MotorBL.run(BACKWARD);
    MotorFR.run(FORWARD);
    MotorBR.run(FORWARD);
  }

  void moveForwardRight() {
    digitalWrite(backLed, LOW);
    int fast = valSpeed;
    int slow = valSpeed / 4;

    MotorFL.setSpeed(fast);
    MotorBL.setSpeed(fast);
    MotorFR.setSpeed(slow);
    MotorBR.setSpeed(slow);

    MotorFL.run(FORWARD);
    MotorFR.run(FORWARD);
    MotorBL.run(FORWARD);
    MotorBR.run(FORWARD);
  }

  void moveForwardLeft() {
    digitalWrite(backLed, LOW);
    int fast = valSpeed;
    int slow = valSpeed / 4;

    MotorFL.setSpeed(slow);
    MotorBL.setSpeed(slow);
    MotorFR.setSpeed(fast);
    MotorBR.setSpeed(fast);

    MotorFL.run(FORWARD);
    MotorFR.run(FORWARD);
    MotorBL.run(FORWARD);
    MotorBR.run(FORWARD);
  }

  void moveBackwardRight() {
    digitalWrite(backLed, HIGH);
    int fast = valSpeed;
    int slow = valSpeed / 4;

    MotorFL.setSpeed(fast);
    MotorBL.setSpeed(fast);
    MotorFR.setSpeed(slow);
    MotorBR.setSpeed(slow);

    MotorFL.run(BACKWARD);
    MotorFR.run(BACKWARD);
    MotorBL.run(BACKWARD);
    MotorBR.run(BACKWARD);
  }

  void moveBackwardLeft() {
    digitalWrite(backLed, HIGH);
    int fast = valSpeed;
    int slow = valSpeed / 4;

    MotorFL.setSpeed(slow);
    MotorBL.setSpeed(slow);
    MotorFR.setSpeed(fast);
    MotorBR.setSpeed(fast);

    MotorFL.run(BACKWARD);
    MotorFR.run(BACKWARD);
    MotorBL.run(BACKWARD);
    MotorBR.run(BACKWARD);
  }

  void checkBattery() {
    int sensorValue = analogRead(batteryPin);
    if (sensorValue < batteryThreshold) {
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval500) {
        previousMillis = currentMillis;
        ledState = !ledState;
        digitalWrite(headingLed, ledState);
        digitalWrite(backLed, ledState);
      }
    } else {
      digitalWrite(headingLed, headlightsOn ? HIGH : LOW);
    }
  }

  bool isFrontBlocked() {
    return digitalRead(sensorFrontPin) == 0;
  }

  bool isBackBlocked() {
    return digitalRead(sensorBackPin) == 0;
  }

  void obstacleAlert() {
    if (isFrontBlocked() || isBackBlocked()) {
      digitalWrite(buzPin, HIGH);
    } else {
      digitalWrite(buzPin, LOW);
    }
  }

  void turnOnHeadingLights() {
    digitalWrite(headingLed, HIGH);
    headlightsOn = true;
  }

  void turnOffHeadingLights() {
    digitalWrite(headingLed, LOW);
    headlightsOn = false;
  }

  void honkHorn() {
    digitalWrite(buzPin, HIGH);
    delay(200);
    digitalWrite(buzPin, LOW);
  }

  void playStartupSound() {
    digitalWrite(buzPin, HIGH);
    delay(100);
    digitalWrite(buzPin, LOW);
    delay(100);
    digitalWrite(buzPin, HIGH);
    delay(200);
    digitalWrite(buzPin, LOW);
  }
};

SmartCar robot;

void setup() {
  robot.init();
}

void loop() {
  robot.run();
}