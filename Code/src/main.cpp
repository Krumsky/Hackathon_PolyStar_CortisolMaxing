#include <Arduino.h>
#include <SR04.h>
#include <dht_nonblocking.h>

#include "BuzzerController.h"
#include "Control.h"
#include "FaceMatrix.h"
#include "MotionSensor.h"

const unsigned long controlPeriodMs = 250;
const unsigned long facePeriodMs = 1500;

const int faceDataPin = 22;
const int faceClockPin = 24;
const int faceCsPin = 26;

const int climateDataPin = A0;

const int leftEchoPin = 31;
const int leftTriggerPin = 33;
const int rightEchoPin = 30;
const int rightTriggerPin = 32;

const int buzzerPin = 8;
const int motionSensorPin = 2;

FaceMatrixController faceMatrixController(
    FaceMatrixConfig(faceDataPin, faceClockPin, faceCsPin, 0, 1));

DHT_nonblocking climateSensor(climateDataPin, DHT_TYPE_22);
float currentTemperatureC = 0.0f;
float currentHumidity = 0.0f;

SR04 leftDistanceSensor(leftEchoPin, leftTriggerPin);
SR04 rightDistanceSensor(rightEchoPin, rightTriggerPin);
RobotControl control(ControlConfig(15, 25));
BuzzerController buzzer(buzzerPin);
MotionSensor motionSensor(MotionSensorConfig(motionSensorPin, CHANGE, false));

const FaceEmotion faceSequence[] = {
    FACE_HAPPY,
    FACE_NEUTRAL,
    FACE_SAD,
    FACE_SURPRISED,
    FACE_BLINK,
};

void updateClimateSensor() {
  if (!climateSensor.measure(&currentTemperatureC, &currentHumidity)) {
    return;
  }

  Serial.print("Climate   | temperature: ");
  Serial.print(currentTemperatureC, 1);
  Serial.print(" C | humidity: ");
  Serial.print(currentHumidity, 1);
  Serial.println(" %");
}

void updateFace() {
  static unsigned long lastFaceUpdateMs = 0;
  static int faceIndex = 0;

  const unsigned long now = millis();
  if (now - lastFaceUpdateMs < facePeriodMs) {
    return;
  }

  lastFaceUpdateMs = now;

  faceIndex = (faceIndex + 1) %
              (sizeof(faceSequence) / sizeof(faceSequence[0]));
  faceMatrixController.showEmotion(faceSequence[faceIndex]);

  Serial.print("Face      | emotion index: ");
  Serial.println(faceIndex);
}

void updateDistanceControl() {
  static unsigned long lastControlUpdateMs = 0;

  const unsigned long now = millis();
  if (now - lastControlUpdateMs < controlPeriodMs) {
    return;
  }

  lastControlUpdateMs = now;

  const long leftDistanceCm = leftDistanceSensor.DistanceAvg(25, 3);
  const long rightDistanceCm = rightDistanceSensor.DistanceAvg(25, 3);

  control.update(leftDistanceCm, rightDistanceCm);
  Serial.println();
}

void updateMotionSensor() {
  bool motionDetected = false;

  if (!motionSensor.consumeStateChange(&motionDetected)) {
    return;
  }

  Serial.print("Motion    | ");
  if (motionDetected) {
    Serial.println("DETECTED");
  } else {
    Serial.println("CLEARED");
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTION_SENSOR_PIN, INPUT);
  // attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR_PIN), ISR1, RISING);
  stepControlInit(myStepper1, REVOLUTIONS_PER_MINUTE);
  stepControlInit(myStepper2, STEERING_SPEED);
  Serial.begin(115200);

  control.begin();

  faceMatrixController.begin();
  faceMatrixController.setIntensity(1);

  buzzer.begin();
  buzzer.startMelody();

  if (motionSensor.begin()) {
    Serial.println("Motion sensor ready");
  } else {
    Serial.println("Motion sensor error: selected pin has no interrupt");
  }

  Serial.println("Modules ready");
  Serial.print("Face pins  | data: ");
  Serial.print(faceDataPin);
  Serial.print(" | clock: ");
  Serial.print(faceClockPin);
  Serial.print(" | cs: ");
  Serial.println(faceCsPin);
  Serial.print("Climate pin| data: ");
  Serial.println(climateDataPin);
  Serial.print("Left HC-SR04  | echo: ");
  Serial.print(leftEchoPin);
  Serial.print(" | trigger: ");
  Serial.println(leftTriggerPin);
  Serial.print("Right HC-SR04 | echo: ");
  Serial.print(rightEchoPin);
  Serial.print(" | trigger: ");
  Serial.println(rightTriggerPin);
  Serial.print("Buzzer pin | ");
  Serial.println(buzzerPin);
  Serial.print("Motion pin | ");
  Serial.println(motionSensorPin);
  Serial.println();
}

void loop() {
  buzzer.update();
  updateMotionSensor();
  updateClimateSensor();
  updateFace();
  updateDistanceControl();
}
