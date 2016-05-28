#ifndef SIMULATOR
#include <Servo.h>
#else
#include <algorithm>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stub.h"
Serial Serial;
#endif

typedef enum {
  STATE_START,
  STATE_RUNNING,
  STATE_MANUAL,
  STATE_STOPPED,
} state;

typedef enum {
  SERIAL_REQUEST_LEFT_POWER  = 0,
  SERIAL_REQUEST_RIGHT_POWER = 1,
  SERIAL_REQUEST_LEFT_DELTA  = 2,
  SERIAL_REQUEST_RIGHT_DELTA = 3,
  SERIAL_UPDATE_LEFT_SPEED   = 4,
  SERIAL_UPDATE_RIGHT_SPEED  = 5,
  SERIAL_UPDATE_TARGET_SPEED = 6,
  SERIAL_REQUEST_STATE = 7,
  SERIAL_UPDATE_STATE = 8,
} serial_commands;

const int ALIGNING_SPEED = 20;
const int BASE_RUNNING_SPEED = 80;

const int RIGHT_PWM_PIN = 10;
const int LEFT_PWM_PIN = 11;

const int LEFT_SWITCH_PIN = 12;
const int RIGHT_SWITCH_PIN = 13;

const int DELAY_AFTER_ALIGNMENT = 3000;
uint32_t targetTime = 1000000;

struct Arm {
  char id;
  Servo jag;
  int switchPort;
  char speed;

  struct alignState {
    bool isAligned;
  } alignState;
  struct spinState {
    uint32_t lastSeen;
    uint32_t lastDelta;
    uint32_t badDeltaCount;
    int tempBonus;
    int frameCount;
  } spinState;
};

struct Arm leftArm;
struct Arm rightArm;

state currentState;

void displayDigit(int digit);

void spin(struct Arm *arm) {
  int speed = map(arm->speed + arm->spinState.tempBonus, -100, 100, 0, 180);

  // As long as the input speed is between -100 and 100 this should
  // never happen
  // Double check anyways since BAD things will happen if this goes wrong.
  if (speed < 0) {
    speed = 0;
  } else if (speed > 180) {
    speed = 180;
  }
  arm->jag.write(speed);
}

void initArm(Arm* arm, char id, int jagPort, int switchPort) {
  memset(arm, 0, sizeof(*arm));
  arm->id = id;

  arm->switchPort = switchPort;
  pinMode(switchPort, INPUT);

  arm->jag.attach(jagPort);
}

void transitionState(state nextState) {
  printf("Transitioning to state %d\n", nextState);
  currentState = nextState;

  memset(&leftArm.alignState, 0, sizeof(leftArm.alignState));
  memset(&rightArm.alignState, 0, sizeof(rightArm.alignState));
  memset(&leftArm.spinState, 0, sizeof(leftArm.spinState));
  memset(&rightArm.spinState, 0, sizeof(rightArm.spinState));

  switch(nextState) {
    case STATE_START:
      leftArm.speed = rightArm.speed = ALIGNING_SPEED;
      break;

    case STATE_RUNNING:
      leftArm.speed = rightArm.speed = BASE_RUNNING_SPEED;
      break;

    default:
      break;
  }
}

void setup() {
  for (int i=2; i < 10; i++) {
    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);
  Serial.write("HELO");
  initArm(&leftArm,  'L', LEFT_PWM_PIN,  LEFT_SWITCH_PIN);
  initArm(&rightArm, 'R', RIGHT_PWM_PIN, RIGHT_SWITCH_PIN);

  transitionState(STATE_START);
}

void alignArm(struct Arm *arm) {
  if (digitalRead(arm->switchPort)) {
    arm->alignState.isAligned = true;
    arm->speed = 0;
    spin(arm);
  }
}

void updateStart() {
  alignArm(&leftArm);
  alignArm(&rightArm);

  if (leftArm.alignState.isAligned && rightArm.alignState.isAligned) {
    delay(DELAY_AFTER_ALIGNMENT);
    transitionState(STATE_RUNNING);
  }
}

void onSwitchHit(uint32_t now, struct Arm *selfArm, struct Arm *otherArm) {
  struct Arm::spinState *self = &selfArm->spinState;
  struct Arm::spinState *other = &otherArm->spinState;

  uint32_t delta = now - self->lastSeen;
  if (delta < 100000) {
    // Ignore small deltas, they are probably the same button press.
    // If not, god help us all.
    return;
  }
  // printf("%ld usecs passed ", delta);
  if (self->lastDelta != 0 && (delta < .2 * self->lastDelta || delta > 1.5 * self->lastDelta)) {
    self->badDeltaCount += 1;
    if (self->badDeltaCount < 3) {
      printf("not adjusting speed (last was %ld)\n", (unsigned long)self->lastDelta);
      return;
    }
  }
  self->badDeltaCount = 0;

  if (delta < 0.7 * targetTime) {
    // printf("Way too fast %c...\n", selfArm->id);
    selfArm->speed -= 2;
  } else if (delta < targetTime) {
    // printf("Too fast %c...\n", selfArm->id);
    selfArm->speed -= 1;
  } else if (delta > targetTime * 1.5) {
    // printf("Way too slow %c...\n", selfArm->id);
    selfArm->speed += 2;
  } else {
    // printf("Too slow %c...\n", selfArm->id);
    selfArm->speed += 1;
  }

  if (selfArm->speed < 0) {
    selfArm->speed = 0;
  } else if (selfArm->speed > 100) {
    selfArm->speed = 100;
  }

  if (now - other->lastSeen < delta * 0.5 && now - other->lastSeen > delta * 0.15) {
    printf("Adjusting speed! Give a kick to %c...\n", selfArm->id);
    self->tempBonus = 5;
    self->frameCount = 10;
    other->tempBonus = -5;
    other->frameCount = 10;
  } else {
    printf("Synced! %f\n", (now - other->lastSeen)/(double)(delta));
  }

  self->lastDelta = delta;
  self->lastSeen = now;
}

void updateRunningArm(uint32_t now, struct Arm* self, struct Arm* other) {
  struct Arm::spinState *spinState = &self->spinState;

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
  uint32_t now = micros();

  updateRunningArm(now, &leftArm, &rightArm);
  updateRunningArm(now, &rightArm, &leftArm);

  if (abs(leftArm.speed - rightArm.speed) > 40) {
    transitionState(STATE_START);
  }
  if (rand() % 10 == 0)
    printf("Right: %d, Left: %d\n", leftArm.speed, rightArm.speed);
}

void handleSerial() {
  char command;
  char value;
  if (!Serial.available()) {
    return;
  }

  Serial.readBytes(&command, sizeof(command));
  Serial.readBytes(&value, sizeof(value));
  switch(command) {
    case SERIAL_REQUEST_LEFT_POWER:
      Serial.write(leftArm.speed);
      break;
    case SERIAL_REQUEST_RIGHT_POWER:
      Serial.write(rightArm.speed);
      break;
    case SERIAL_REQUEST_LEFT_DELTA:
      Serial.write((char*)&leftArm.spinState.lastDelta, sizeof(leftArm.spinState.lastDelta));
      break;
    case SERIAL_REQUEST_RIGHT_DELTA:
      Serial.write((char*)&rightArm.spinState.lastDelta, sizeof(rightArm.spinState.lastDelta));
      break;
    case SERIAL_UPDATE_LEFT_SPEED:
      leftArm.speed = value;
      break;
    case SERIAL_UPDATE_RIGHT_SPEED:
      rightArm.speed = value;
      break;
    case SERIAL_UPDATE_TARGET_SPEED:
      targetTime = value;
    case SERIAL_REQUEST_STATE:
      Serial.write((char)currentState);
      break;
    case SERIAL_UPDATE_STATE:
      transitionState((state)value);
      break;
  }
}

void loop() {
  switch (currentState) {
    case STATE_START:
      updateStart();
      break;
    case STATE_RUNNING:
      updateRunning();
      break;
    case STATE_MANUAL:
      break;
    case STATE_STOPPED:
    default:
      leftArm.speed = rightArm.speed = 0;
      break;
  }

  spin(&leftArm);
  spin(&rightArm);
  // displayDigit(currentState);

  handleSerial();
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
