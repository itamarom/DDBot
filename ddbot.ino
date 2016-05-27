#include <Servo.h>

const int STATE_START = 0;
const int STATE_RUNNING = 1;

const int ALIGNING_SPEED = 20;

const int RIGHT_PWM_PIN = 10;
const int LEFT_PWM_PIN = 11;

const int MS_LEFT_PIN = ?;
const int MS_RIGHT_PIN = ?;

const int DELAY_AFTER_ALIGNMENT = 3000;

Servo leftJag;
Servo rightJag;

bool rightAligned = false;
bool leftAligned = false;

int currentState = STATE_START;

void spin(Servo s, int speed) {
  s.write(map(speed, -100, 100, 0, 180));
}

void setup() {
  // digitalWrite(MOTOR_NEG, LOW);
 // analogWrite(PWM_PIN, 47);
  for (int i=2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

  pinMode(MS_LEFT_PIN, INPUT);
  pinMode(MS_RIGHT_PIN, INPUT);

  leftJag.attach(LEFT_PWM_PIN);
  rightJag.attach(RIGHT_PWM_PIN);
  // leftJag.writeMicroseconds(670);
  // leftJag.write(90);
  //digitalWrite(PWR_PIN, HIGH);
  //digitalWrite(GRD_PIN, LOW);
  spin(leftJag, 0);
  spin(rightJag, 20);
  //analogWrite(PWM_PIN, 128);
}

const int FACTOR = 5;

void updateStart() {
  if (!rightAligned) {
    if (digitalRead(MS_RIGHT_PIN)) {
      rightAligned = true;
      spin(rightJag, 0);
    } else {
      spin(rightJag, ALIGNING_SPEED);
    }
  } else {
    spin(rightJag, 0);
  }

  if (leftAligned && rightAligned) {
    delay(DELAY_AFTER_ALIGNMENT);
    currentState = STATE_RUNNING;
  }
}

void updateRunning() {
  spin(leftJag, RUNNING_SPEED);
}

void stopAll() {
  spin(leftJag, 0);
  spin(rightJag, 0);
}

void update() {
  if (currentState == STATE_START) {
    updateStart();
  } else if (currentState == STATE_RUNNING) {
    updateRunning();
  } else {
    stopAll();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  while (true) {
    update();
    display_digit(currentState);
    delay(20);
  }
  return;
  delay(8000);
  spin(leftJag, 0);
  spin(rightJag, 0);
  for(int i = 0;; i = (i + 1) % 10) {
    // leftJag.write(20);
    for(int j = 0; j < 1000 / FACTOR; j++) {
      //display_digit(i);
      if (digitalRead(PHOTOSWITCH_A_PIN) == HIGH) {
        display_digit(1);
      } else {
        display_digit(0);
      }
      delay(FACTOR / 5);
      clear_digit();
      delay(FACTOR * 9 / 5);
    }
  }
}

void clear_digit() {
  for (int j = 0; j < 10; j++) {
      digitalWrite(j, HIGH);
  }
}
void display_digit(int i) {
  clear_digit();
  if (i != 1 && i != 4) {
      // TOP BAR
      digitalWrite(8, LOW);
  }
  if (i != 1 && i != 2 && i != 3 && i != 7) {
    // TOP LEFT
    digitalWrite(9, LOW);
  }
  if (i != 5 && i != 6) {
    // TOP RIGHT
    digitalWrite(6, LOW);
  }
  if (i != 0 && i != 1 && i != 7) {
    // MIDDLE
    digitalWrite(3, LOW);
  }
  if (i == 0 || i == 2 || i == 6 || i == 8) {
    // BOTTOM LEFT
    digitalWrite(5, LOW);
  }
  if (i != 2) {
    // BOTTOM RIGHT
    digitalWrite(2, LOW);
  }
  if (i != 1 && i != 4 && i != 7) {
    // BOTTOM
    digitalWrite(4, LOW);
  }
}

