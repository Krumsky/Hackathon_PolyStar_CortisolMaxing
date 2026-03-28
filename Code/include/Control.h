#pragma once

#include <Arduino.h>

enum RobotCommand {
  COMMAND_FORWARD = 0,
  COMMAND_TURN_LEFT = 1,
  COMMAND_TURN_RIGHT = 2,
  COMMAND_STOP = 3,
  COMMAND_REVERSE = 4
};

struct ControlConfig {
  ControlConfig(long stopDistanceCm = 15, long turnDistanceCm = 25)
      : stopDistanceCm(stopDistanceCm), turnDistanceCm(turnDistanceCm) {}

  long stopDistanceCm;
  long turnDistanceCm;
};

class RobotControl {
 public:
  explicit RobotControl(const ControlConfig& config = ControlConfig{});

  void begin();
  void update(long leftDistanceCm, long rightDistanceCm);
  RobotCommand lastCommand() const;

 private:
  long sanitizeDistance(long distanceCm) const;
  RobotCommand chooseCommand(long leftDistanceCm, long rightDistanceCm) const;
  void printDistances(long leftDistanceCm, long rightDistanceCm) const;
  void printCommand(RobotCommand command) const;

  ControlConfig config_;
  RobotCommand lastCommand_ = COMMAND_STOP;
};
