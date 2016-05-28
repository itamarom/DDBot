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

const int LEFT_SWITCH_PIN = 12;
const int RIGHT_SWITCH_PIN = 13;

const int DELAY_AFTER_ALIGNMENT = 3000;
const unsigned long TARGET_TIME = 1000000;

struct Arm {
  char id;
  Servo jag;
  int switchPort;

  struct alignState {
      bool isAligned;
  } alignState;
  struct spinState {
      int speed;
      unsigned long lastSeen;
      unsigned long lastDelta;
      unsigned long badDeltaCount;
      int tempBonus;
      int frameCount;
  } spinState;
};

struct Arm leftArm;
struct Arm rightArm;

state currentState = STATE_START;

void displayDigit(int digit);

void spin(Servo s, int speed) {
  s.write(map(speed, -100, 100, 0, 180));
}

void initArm(Arm* arm, char id, int jagPort, int switchPort) {
    memset(arm, 0, sizeof(arm));
    arm->id = id;

    arm->switchPort = switchPort
    pinMode(switchPort, INPUT);

    arm->jag.attach(jagPort);
    spin(arm->jag, 0);
}

void setup() {
  for (int i=2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

  initArm(&leftArm,  'L', LEFT_PWM_PIN,  LEFT_SWITCH_PIN);
  initArm(&rightArm, 'R', RIGHT_PWM_PIN, RIGHT_SWITCH_PIN);
}

const int FACTOR = 5;

void transitionState(state nextState) {
  printf("Transitioning to state %d\n", nextState);
  currentState = nextState;
}

void alignArm(struct ArmState *arm) {
  if (digitalRead(arm->switchPort)) {
      arm->alignState->isAligned = true;
  }

  if (arm->alignState->isAligned) {
    spin(arm->jag, 0);
  } else {
    spin(arm->jag, ALIGNING_SPEED);
  }
}

void updateStart() {
  static bool initOnNextRun = true;

  if (initOnNextRun) {
    memset(&leftArm.alignState, 0, sizeof(leftArm.alignState));
    memset(&rightArm.alignState, 0, sizeof(rightArm.alignState));
    initOnNextRun = false;
  }

  alignArm(&leftArm);
  alignArm(&rightArm);

  if (leftArm.alignState.isAligned && rightArm.alignState.isAligned) {
    delay(DELAY_AFTER_ALIGNMENT);
    transitionState(STATE_RUNNING);
    initOnNextRun = true;
  }
}

void onSwitchHit(unsigned long now, struct Arm *selfArm, struct Arm *otherArm) {
  struct spinState *self = &selfArm->spinState;
  struct spinState *other = &otherArm->spinState;

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
    printf("Adjusting speed! Give a kick to %c...\n", selfArm->id);
    self->tempBonus = 10;
    self->frameCount = 20;
    other->tempBonus = 0;
    other->frameCount = 0;
  } else {
    printf("Synced! %f\n", (now - other->lastSeen)/(double)(delta));
  }

  self->lastDelta = delta;
  self->lastSeen = now;
}

void updateRunningArm(unsigned long now, struct Arm* self, struct Arm* other) {
  struct spinState *spinState = &self.spinState;

  spin(self->jag, spinState->speed + spinState->tempBonus);

  if (spinState->frameCount) {
    if(--spinState->frameCount <= 0) {
      spinState->tempBonus = 0;
    }
  }

  if (digitalRead(self->switchPort)) {
    onSwitchHit(now, self, other);
  }
}

void updateRunning() {
  static bool initOnNextRun = true;
  unsigned long now = micros();

  if (initOnNextRun) {
    memset(&leftArm->spinState, 0, sizeof(leftArm->spinState));
    memset(&rightArm->spinState, 0, sizeof(rightArm->spinState));

    leftArm->spinState.speed = rightArm->spinState.speed = BASE_RUNNING_SPEED;
    initOnNextRun = false;
  }

  updateRunningArm(now, &leftArm, &rightArm);
  updateRunningArm(now, &rightArm, &leftArm);

  if (abs(leftArm->spinState.speed - rightArm->spinState.speed) > 40) {
    transitionState(STATE_START);
    initOnNextRun = true;
  }
  if (rand() % 10 == 0)
    printf("Right: %d, Left: %d\n", rightSpin.speed, leftSpin.speed);
}

void stopAll() {
  spin(leftArm.jag, 0);
  spin(rightArm.jag, 0);
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
