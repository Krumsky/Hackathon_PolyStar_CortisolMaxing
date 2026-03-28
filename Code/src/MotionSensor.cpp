#include "MotionSensor.h"

MotionSensor* MotionSensor::activeInstance_ = 0;

MotionSensor::MotionSensor(const MotionSensorConfig& config) : config_(config) {
}

bool MotionSensor::begin() {
  const int interruptNumber = digitalPinToInterrupt(config_.pin);
  if (interruptNumber == NOT_AN_INTERRUPT) {
    return false;
  }

  if (config_.useInternalPullup) {
    pinMode(config_.pin, INPUT_PULLUP);
  } else {
    pinMode(config_.pin, INPUT);
  }

  motionDetected_ = digitalRead(config_.pin) == HIGH;
  stateChanged_ = false;

  activeInstance_ = this;
  attachInterrupt(interruptNumber, MotionSensor::handleInterrupt,
                  config_.interruptMode);
  return true;
}

bool MotionSensor::consumeStateChange(bool* motionDetected) {
  bool changed = false;
  bool state = false;

  noInterrupts();
  changed = stateChanged_;
  state = motionDetected_;
  stateChanged_ = false;
  interrupts();

  if (!changed) {
    return false;
  }

  if (motionDetected != 0) {
    *motionDetected = state;
  }

  return true;
}

bool MotionSensor::isMotionDetected() const {
  noInterrupts();
  const bool state = motionDetected_;
  interrupts();
  return state;
}

void MotionSensor::handleInterrupt() {
  if (activeInstance_ == 0) {
    return;
  }

  activeInstance_->onInterrupt();
}

void MotionSensor::onInterrupt() {
  motionDetected_ = digitalRead(config_.pin) == HIGH;
  stateChanged_ = true;
}
