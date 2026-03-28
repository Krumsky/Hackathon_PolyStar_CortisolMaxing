#include <Arduino.h>
#include <Stepper.h>

#include "constants.h"
#include "step_control.h"
// initialize the stepper library on pins 8 through 11:
Stepper myStepper1(STEPS_PER_REVOLUTION, STEPPER1_IN4, STEPPER1_IN2,
                   STEPPER1_IN3, STEPPER1_IN1);

Stepper myStepper2(STEPS_PER_REVOLUTION, STEPPER2_IN4, STEPPER2_IN2,
                   STEPPER2_IN3, STEPPER2_IN1);

// void ISR1() { Serial.println("mouvement detected"); }

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTION_SENSOR_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR_PIN), ISR1, RISING);
  stepControlInit(myStepper1, REVOLUTIONS_PER_MINUTE);
  stepControlInit(myStepper2, STEERING_SPEED);
  Serial.begin(115200);
  Serial.println("Hello, World!");
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print("entering main");
}

void loop() {
  Serial.println("in main");
  delay(500);

  // stepClockwise(myStepper2, STEPS_PER_REVOLUTION);
  // delay(500);

  // stepClockwise(myStepper2, STEPS_PER_REVOLUTION);
  // delay(500);

  stepClockwise(myStepper1, 500);
  delay(500);

  stepClockwise(myStepper1, -500);
  delay(500);
}
