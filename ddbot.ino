#include <Servo.h>

typedef enum {
  STATE_START,
  STATE_RUNNING,
} state;

const int ALIGNING_SPEED = 20;

const int RIGHT_PWM_PIN = 10;
const int LEFT_PWM_PIN = 11;

const int SWITCH_LEFT_PIN = 12;
const int SWITCH_RIGHT_PIN = 13;

const int DELAY_AFTER_ALIGNMENT = 3000;

Servo leftJag;
Servo rightJag;

bool rightAligned = false;
bool leftAligned = false;

state currentState = STATE_START;

void spin(Servo s, int speed) {
  s.write(map(speed, -100, 100, 0, 180));
}

void setup() {
  for (int i=2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

  pinMode(SWITCH_LEFT_PIN, INPUT);
  pinMode(SWITCH_RIGHT_PIN, INPUT);

  leftJag.attach(LEFT_PWM_PIN);
  rightJag.attach(RIGHT_PWM_PIN);

  spin(leftJag, 0);
  spin(rightJag, 0);
}

const int FACTOR = 5;

void updateStart() {
  if (digitalRead(SWITCH_RIGHT_PIN)) {
      rightAligned = true;
  };
  if (digitalRead(SWITCH_LEFT_PIN)) {
      leftAligned = true;
  };

  if (rightAligned) {
    spin(rightJag, 0);
  } else {
    spin(rightJag, ALIGNING_SPEED);
  }

  if (leftAlighed) {
    spin(leftJag, 0);
  } else {
    spin(leftJag, ALIGNING_SPEED);
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

void loop() {
  switch (currentState) {
    case STATE_START:
      updateStart();
      break;
    case STATE_RUNNING:
      updateRunning();
      break;
    default:
      stopAll();
      break
    }
  display_digit(currentState);
  delay(20);
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

