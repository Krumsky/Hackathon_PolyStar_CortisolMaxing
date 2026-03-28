#pragma once

#include <Arduino.h>

struct MotionSensorConfig {
  MotionSensorConfig(uint8_t pin = 2, int interruptMode = CHANGE,
                     bool useInternalPullup = false)
      : pin(pin),
        interruptMode(interruptMode),
        useInternalPullup(useInternalPullup) {}

  uint8_t pin;
  int interruptMode;
  bool useInternalPullup;
};

class MotionSensor {
 public:
  explicit MotionSensor(
      const MotionSensorConfig& config = MotionSensorConfig{});

  bool begin();
  bool consumeStateChange(bool* motionDetected);
  bool isMotionDetected() const;

 private:
  static void handleInterrupt();
  void onInterrupt();

  MotionSensorConfig config_;
  volatile bool motionDetected_ = false;
  volatile bool stateChanged_ = false;

  static MotionSensor* activeInstance_;
};
