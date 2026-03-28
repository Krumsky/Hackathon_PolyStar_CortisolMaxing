#include "Control.h"

RobotControl::RobotControl(const ControlConfig& config) : config_(config) {
}

void RobotControl::begin() {
  Serial.println("Control logic ready");
  Serial.print("Stop distance: ");
  Serial.print(config_.stopDistanceCm);
  Serial.println(" cm");
  Serial.print("Turn distance: ");
  Serial.print(config_.turnDistanceCm);
  Serial.println(" cm");
}

void RobotControl::update(long leftDistanceCm, long rightDistanceCm) {
  const long cleanLeftDistance = sanitizeDistance(leftDistanceCm);
  const long cleanRightDistance = sanitizeDistance(rightDistanceCm);
  const RobotCommand command = chooseCommand(cleanLeftDistance, cleanRightDistance);

  printDistances(cleanLeftDistance, cleanRightDistance);
  printCommand(command);
  lastCommand_ = command;
}

RobotCommand RobotControl::lastCommand() const {
  return lastCommand_;
}

long RobotControl::sanitizeDistance(long distanceCm) const {
  if (distanceCm <= 0) {
    return 999;
  }

  return distanceCm;
}

RobotCommand RobotControl::chooseCommand(long leftDistanceCm, long rightDistanceCm) const {
  if (leftDistanceCm <= config_.stopDistanceCm &&
      rightDistanceCm <= config_.stopDistanceCm) {
    return COMMAND_REVERSE;
  }

  if (leftDistanceCm <= config_.turnDistanceCm &&
      rightDistanceCm <= config_.turnDistanceCm) {
    return COMMAND_STOP;
  }

  if (leftDistanceCm <= config_.turnDistanceCm) {
    return COMMAND_TURN_RIGHT;
  }

  if (rightDistanceCm <= config_.turnDistanceCm) {
    return COMMAND_TURN_LEFT;
  }

  return COMMAND_FORWARD;
}

void RobotControl::printDistances(long leftDistanceCm, long rightDistanceCm) const {
  Serial.print("Distances | left: ");
  Serial.print(leftDistanceCm);
  Serial.print(" cm | right: ");
  Serial.print(rightDistanceCm);
  Serial.println(" cm");
}

void RobotControl::printCommand(RobotCommand command) const {
  Serial.print("Command   | ");

  switch (command) {
    case COMMAND_FORWARD:
      Serial.println("FORWARD");
      break;
    case COMMAND_TURN_LEFT:
      Serial.println("TURN LEFT");
      break;
    case COMMAND_TURN_RIGHT:
      Serial.println("TURN RIGHT");
      break;
    case COMMAND_STOP:
      Serial.println("STOP");
      break;
    case COMMAND_REVERSE:
      Serial.println("REVERSE");
      break;
  }
}
