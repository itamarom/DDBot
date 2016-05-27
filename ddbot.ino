#ifndef SIMULATOR
#include <Servo.h>
#else
#include "stub.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

typedef enum {
  STATE_START,
  STATE_RUNNING,
} state;

const int ALIGNING_SPEED = 20;
const int BASE_RUNNING_SPEED = 80;

const int RIGHT_PWM_PIN = 10;
const int LEFT_PWM_PIN = 11;

const int SWITCH_LEFT_PIN = 12;
const int SWITCH_RIGHT_PIN = 13;

const int DELAY_AFTER_ALIGNMENT = 3000;
const int TARGET_TIME = 1000000;

Servo leftJag;
Servo rightJag;

state currentState = STATE_START;

void displayDigit(int digit);

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

void transitionState(state nextState) {
  printf("Transitioning to state %d\n", nextState);
  currentState = nextState;
}

void updateStart() {
  static bool leftAligned = false;
  static bool rightAligned = false;

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

  if (leftAligned) {
    spin(leftJag, 0);
  } else {
    spin(leftJag, ALIGNING_SPEED);
  }

  if (leftAligned && rightAligned) {
    delay(DELAY_AFTER_ALIGNMENT);
    transitionState(STATE_RUNNING);
    leftAligned = rightAligned = false;
  }
}

typedef struct {
  int id;
  int speed;
  unsigned long lastSeen;
  unsigned long lastDelta;
  unsigned long badDeltaCount;
  int tempBonus;
  int frameCount;
} timeInfo;

void updateSpeed(unsigned long now, timeInfo *self, timeInfo *other) {
  unsigned long delta = now - self->lastSeen;
  if (delta < 100000) {
    // Ignore small deltas, they are probably the same button press.
    // If not, god help us all.
    return;
  }
  // printf("%ld usecs passed ", delta);
  if (self->lastDelta != 0 && (delta < .2 * self->lastDelta || delta > 1.5 * self->lastDelta)) {
    self->badDeltaCount += 1;
    if (self->badDeltaCount < 3) {
      // printf("not adjusting speed (last was %ld)\n", self->lastDelta);
      return;
    }
  }
  // printf("adjusting speed\n");
  self->badDeltaCount = 0;

  if (delta < 0.7 * TARGET_TIME) {
    self->speed -= 2;
  } else if (delta < TARGET_TIME) {
    self->speed -= 1;
  } else if (delta > TARGET_TIME * 1.5) {
    self->speed += 2;
  } else {
    self->speed += 1;
  }

  if (now - other->lastSeen < delta * 0.5 && now - other->lastSeen > delta * 0.1) {
    printf("Adjusting speed! Give a kick to %d...\n", self->id);
    self->tempBonus = 10;
    self->frameCount = 20;
    other->tempBonus = 0;
    other->frameCount = 0;
  } else {
    printf("Synced! %f\n", (now - other->lastSeen)/(double)(TARGET_TIME));
  }

  self->lastDelta = now - self->lastSeen;
  self->lastSeen = now;
}

void updateRunning() {
  static timeInfo leftSpeed, rightSpeed;
  static bool initOnNextRun = true;
  unsigned long now = micros();

  if (initOnNextRun) {
    memset(&leftSpeed, 0, sizeof(leftSpeed));
    memset(&rightSpeed, 0, sizeof(rightSpeed));
    leftSpeed.speed = rightSpeed.speed = 80;
    leftSpeed.id = 1;
    initOnNextRun = false;
  }

  spin(leftJag, leftSpeed.speed + leftSpeed.tempBonus);
  spin(rightJag, rightSpeed.speed + rightSpeed.tempBonus);
  if (leftSpeed.frameCount) {
    if(--leftSpeed.frameCount == 0) {
      leftSpeed.tempBonus = 0;
    }
  }
  if (rightSpeed.frameCount) {
    if(--rightSpeed.frameCount == 0) {
      rightSpeed.tempBonus = 0;
    }
  }

  if (digitalRead(SWITCH_RIGHT_PIN)) {
    updateSpeed(now, &rightSpeed, &leftSpeed);
  }
  if (digitalRead(SWITCH_LEFT_PIN)) {
    updateSpeed(now, &leftSpeed, &rightSpeed);
  }

  if (leftSpeed.speed - rightSpeed.speed > 40 || rightSpeed.speed - leftSpeed.speed > 40) {
    transitionState(STATE_START);
    initOnNextRun = true;
  }
  if (rand() % 10 == 0)
    printf("Right: %d, Left: %d\n", rightSpeed.speed, leftSpeed.speed);

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
      break;
    }
  // displayDigit(currentState);
  delay(20);
}

void clearDigit() {
  for (int j = 0; j < 10; j++) {
      digitalWrite(j, HIGH);
  }
}
void displayDigit(int i) {
  clearDigit();
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

#ifdef SIMULATOR
int main() {
  setup();
  while(1) loop();
}
#endif
