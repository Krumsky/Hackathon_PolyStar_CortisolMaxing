#pragma once

constexpr int STEPS_PER_REVOLUTION = 2048;

constexpr long STEERING_SPEED_RPM = 10;
constexpr long DRIVE_SPEED_RPM = 12;

constexpr long STEERING_TURN_STEPS = 150;
constexpr long STEERING_CENTER = 115;
constexpr long DRIVE_FORWARD_STEPS = -750;
constexpr long DRIVE_REVERSE_STEPS = 750;

constexpr unsigned long CONTROL_PERIOD_MS = 250;
constexpr unsigned long FACE_PERIOD_MS = 1500;
constexpr unsigned long FAN_ON_DURATION_MS = 10000;
constexpr unsigned long REACTION_FACE_HOLD_MS = 3000;
constexpr unsigned long MOTION_SETTLE_MS = 400;
constexpr unsigned long MOTION_SCAN_WINDOW_MS = 1000;

constexpr float TEMPERATURE_VERY_COLD_C = 15.0f;
constexpr float TEMPERATURE_AMBIENT_MIN_C = 20.0f;
constexpr float TEMPERATURE_AMBIENT_MAX_C = 26.0f;
constexpr float TEMPERATURE_TOO_HOT_C = 30.0f;

constexpr int FACE_DATA_PIN = 19;
constexpr int FACE_CS_PIN = 20;
constexpr int FACE_CLOCK_PIN = 21;

constexpr int CLIMATE_DATA_PIN = A14;

constexpr int LEFT_ECHO_PIN = 16;
constexpr int LEFT_TRIGGER_PIN = 15;
constexpr int RIGHT_ECHO_PIN = A10;
constexpr int RIGHT_TRIGGER_PIN = A11;

constexpr int BUZZER_PIN = 7;
constexpr int MOTION_SENSOR_PIN = A13;
constexpr int FAN_PIN = A15;

constexpr int STEPPER1_IN1 = 6;
constexpr int STEPPER1_IN2 = 5;
constexpr int STEPPER1_IN3 = 4;
constexpr int STEPPER1_IN4 = 3;

constexpr int STEPPER2_IN1 = A3;
constexpr int STEPPER2_IN2 = A2;
constexpr int STEPPER2_IN3 = A1;
constexpr int STEPPER2_IN4 = A0;
