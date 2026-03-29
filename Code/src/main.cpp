#include <Arduino.h>
#include <SR04.h>
#include <Stepper.h>
#include <dht_nonblocking.h>

#include "BuzzerController.h"
#include "Control.h"
#include "FaceMatrix.h"
#include "constants.h"

FaceMatrixController faceMatrixController(
    FaceMatrixConfig(FACE_DATA_PIN, FACE_CLOCK_PIN, FACE_CS_PIN, 0, 1));

DHT_nonblocking climateSensor(CLIMATE_DATA_PIN, DHT_TYPE_11);
float currentTemperatureC = 0.0f;
float currentHumidity = 0.0f;
bool hasClimateReading = false;

SR04 leftDistanceSensor(LEFT_ECHO_PIN, LEFT_TRIGGER_PIN);
SR04 rightDistanceSensor(RIGHT_ECHO_PIN, RIGHT_TRIGGER_PIN);
RobotControl control(ControlConfig(15, 25));
BuzzerController buzzer(BUZZER_PIN);

Stepper steeringMotor(STEPS_PER_REVOLUTION, STEPPER1_IN1, STEPPER1_IN3,
                      STEPPER1_IN2, STEPPER1_IN4);
Stepper driveMotor(STEPS_PER_REVOLUTION, STEPPER2_IN1, STEPPER2_IN3,
                   STEPPER2_IN2, STEPPER2_IN4);

unsigned long reactionFaceHoldUntilMs = 0;
unsigned long fanStopAtMs = 0;
bool fanIsOn = false;

const FaceEmotion idleFaceSequence[] = {
    FACE_HAPPY,
    FACE_NEUTRAL,
    FACE_SAD,
    FACE_SURPRISED,
    FACE_BLINK,
};

void updateClimateSensor();
void updateFan();
void serviceBackgroundTasks();

void configureMotor(Stepper& motor, long speedRpm) {
  motor.setSpeed(speedRpm);
}

void moveMotorSteps(Stepper& motor, long stepCount) {
  if (stepCount == 0) {
    return;
  }

  const int direction = stepCount > 0 ? 1 : -1;
  const long totalSteps = abs(stepCount);

  for (long stepIndex = 0; stepIndex < totalSteps; ++stepIndex) {
    motor.step(direction);
    serviceBackgroundTasks();
  }
}

const char* faceName(FaceEmotion face) {
  switch (face) {
    case FACE_HAPPY:
      return "HAPPY";
    case FACE_NEUTRAL:
      return "NEUTRAL";
    case FACE_SAD:
      return "SAD";
    case FACE_SURPRISED:
      return "SURPRISED";
    case FACE_BLINK:
      return "BLINK";
  }

  return "UNKNOWN";
}

FaceEmotion chooseFaceForTemperature(float temperatureC) {
  if (temperatureC <= TEMPERATURE_VERY_COLD_C) {
    return FACE_SAD;
  }

  if (temperatureC >= TEMPERATURE_TOO_HOT_C) {
    return FACE_SAD;
  }

  if (temperatureC >= TEMPERATURE_AMBIENT_MIN_C &&
      temperatureC <= TEMPERATURE_AMBIENT_MAX_C) {
    return FACE_HAPPY;
  }

  return FACE_NEUTRAL;
}

void setReactionFace(FaceEmotion face) {
  faceMatrixController.showEmotion(face);
  reactionFaceHoldUntilMs = millis() + REACTION_FACE_HOLD_MS;
}

void startFanCycle() {
  digitalWrite(FAN_PIN, HIGH);
  fanIsOn = true;
  fanStopAtMs = millis() + FAN_ON_DURATION_MS;

  Serial.println("Fan       | ON for 10 s");
}

void updateFan() {
  if (!fanIsOn) {
    return;
  }

  if (millis() < fanStopAtMs) {
    return;
  }

  digitalWrite(FAN_PIN, LOW);
  fanIsOn = false;
  Serial.println("Fan       | OFF");
}

void updateClimateSensor() {
  if (!climateSensor.measure(&currentTemperatureC, &currentHumidity)) {
    return;
  }

  hasClimateReading = true;

  Serial.print("Climate   | temperature: ");
  Serial.print(currentTemperatureC, 2);
  Serial.print(" C | humidity: ");
  Serial.print(currentHumidity, 2);
  Serial.println(" %");
}

void serviceBackgroundTasks() {
  buzzer.update();
  updateClimateSensor();
  updateFan();
}

void updateIdleFaceAnimation() {
  static unsigned long lastFaceUpdateMs = 0;
  static int faceIndex = 0;

  if (millis() < reactionFaceHoldUntilMs) {
    return;
  }

  const unsigned long now = millis();
  if (now - lastFaceUpdateMs < FACE_PERIOD_MS) {
    return;
  }

  lastFaceUpdateMs = now;

  faceIndex = (faceIndex + 1) %
              (sizeof(idleFaceSequence) / sizeof(idleFaceSequence[0]));
  faceMatrixController.showEmotion(idleFaceSequence[faceIndex]);

  Serial.print("Face      | idle ");
  Serial.println(faceName(idleFaceSequence[faceIndex]));
}

void reactToMotion() {
  FaceEmotion reactionFace = FACE_NEUTRAL;

  if (!hasClimateReading) {
    Serial.println("Reaction  | no climate reading yet, neutral face");
    setReactionFace(reactionFace);
    buzzer.playTune(BUZZER_TUNE_UNHAPPY);
    return;
  }

  reactionFace = chooseFaceForTemperature(currentTemperatureC);
  setReactionFace(reactionFace);

  Serial.print("Reaction  | face: ");
  Serial.print(faceName(reactionFace));
  Serial.print(" | temperature: ");
  Serial.print(currentTemperatureC, 2);
  Serial.println(" C");

  if (reactionFace == FACE_HAPPY) {
    buzzer.playTune(BUZZER_TUNE_HAPPY);
  } else {
    buzzer.playTune(BUZZER_TUNE_UNHAPPY);
  }

  if (currentTemperatureC >= TEMPERATURE_TOO_HOT_C) {
    startFanCycle();
  }
}

void checkMotionAfterMove() {
  const bool motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;

  Serial.print("Motion    | ");
  if (!motionDetected) {
    Serial.println("NONE");
    return;
  }

  Serial.println("DETECTED");
  reactToMotion();
}

void executeRobotCommand(RobotCommand command) {
  switch (command) {
    case COMMAND_FORWARD:
      Serial.println("Drive     | forward step");
      moveMotorSteps(driveMotor, DRIVE_FORWARD_STEPS);
      break;

    case COMMAND_TURN_LEFT:
      Serial.println("Steering  | left");
      moveMotorSteps(steeringMotor, -STEERING_TURN_STEPS);
      Serial.println("Drive     | forward step");
      moveMotorSteps(driveMotor, DRIVE_FORWARD_STEPS);
      Serial.println("Steering  | center");
      moveMotorSteps(steeringMotor, STEERING_TURN_STEPS);
      break;

    case COMMAND_TURN_RIGHT:
      Serial.println("Steering  | right");
      moveMotorSteps(steeringMotor, STEERING_TURN_STEPS);
      Serial.println("Drive     | forward step");
      moveMotorSteps(driveMotor, DRIVE_FORWARD_STEPS);
      Serial.println("Steering  | center");
      moveMotorSteps(steeringMotor, -STEERING_TURN_STEPS);
      break;

    case COMMAND_STOP:
      Serial.println("Drive     | stop");
      break;

    case COMMAND_REVERSE:
      Serial.println("Drive     | reverse step");
      moveMotorSteps(driveMotor, DRIVE_REVERSE_STEPS);
      break;
  }
}

void updateRobotControlLoop() {
  static unsigned long lastControlUpdateMs = 0;

  const unsigned long now = millis();
  if (now - lastControlUpdateMs < CONTROL_PERIOD_MS) {
    return;
  }

  lastControlUpdateMs = now;

  const long leftDistanceCm = leftDistanceSensor.DistanceAvg(25, 3);
  const long rightDistanceCm = rightDistanceSensor.DistanceAvg(25, 3);

  control.update(leftDistanceCm, rightDistanceCm);
  executeRobotCommand(control.lastCommand());
  checkMotionAfterMove();
  Serial.println();
}

void setup() {
  Serial.begin(115200);

  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);

  control.begin();

  faceMatrixController.begin();
  faceMatrixController.setIntensity(1);

  buzzer.begin();
  buzzer.startMelody();

  configureMotor(steeringMotor, STEERING_SPEED_RPM);
  configureMotor(driveMotor, DRIVE_SPEED_RPM);

  Serial.println("Modules ready");
  Serial.print("Face pins  | data: ");
  Serial.print(FACE_DATA_PIN);
  Serial.print(" | clock: ");
  Serial.print(FACE_CLOCK_PIN);
  Serial.print(" | cs: ");
  Serial.println(FACE_CS_PIN);
  Serial.print("Climate pin| data: ");
  Serial.println(CLIMATE_DATA_PIN);
  Serial.print("Left HC-SR04  | echo: ");
  Serial.print(LEFT_ECHO_PIN);
  Serial.print(" | trigger: ");
  Serial.println(LEFT_TRIGGER_PIN);
  Serial.print("Right HC-SR04 | echo: ");
  Serial.print(RIGHT_ECHO_PIN);
  Serial.print(" | trigger: ");
  Serial.println(RIGHT_TRIGGER_PIN);
  Serial.print("Motion pin | ");
  Serial.println(MOTION_SENSOR_PIN);
  Serial.print("Fan pin    | ");
  Serial.println(FAN_PIN);
  Serial.print("Stepper 1  | steering on pins ");
  Serial.print(STEPPER1_IN1);
  Serial.print(", ");
  Serial.print(STEPPER1_IN2);
  Serial.print(", ");
  Serial.print(STEPPER1_IN3);
  Serial.print(", ");
  Serial.println(STEPPER1_IN4);
  Serial.print("Stepper 2  | drive on pins ");
  Serial.print(STEPPER2_IN1);
  Serial.print(", ");
  Serial.print(STEPPER2_IN2);
  Serial.print(", ");
  Serial.print(STEPPER2_IN3);
  Serial.print(", ");
  Serial.println(STEPPER2_IN4);
  Serial.println();
}

void loop() {
  serviceBackgroundTasks();
  updateIdleFaceAnimation();
  updateRobotControlLoop();
}
