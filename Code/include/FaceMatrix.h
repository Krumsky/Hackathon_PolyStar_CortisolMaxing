#pragma once

#include <Arduino.h>
#include <LedControl.h>

enum FaceEmotion {
  FACE_HAPPY = 0,
  FACE_NEUTRAL = 1,
  FACE_SAD = 2,
  FACE_SURPRISED = 3,
  FACE_BLINK = 4
};

struct FaceMatrixConfig {
  FaceMatrixConfig(uint8_t dataPin = 12, uint8_t clockPin = 11,
                   uint8_t csPin = 10, uint8_t deviceIndex = 0,
                   uint8_t intensity = 4)
      : dataPin(dataPin),
        clockPin(clockPin),
        csPin(csPin),
        deviceIndex(deviceIndex),
        intensity(intensity) {}

  uint8_t dataPin;
  uint8_t clockPin;
  uint8_t csPin;
  uint8_t deviceIndex;
  uint8_t intensity;
};

class FaceMatrixController {
 public:
  explicit FaceMatrixController(
      const FaceMatrixConfig& config = FaceMatrixConfig{});

  void begin();
  void showEmotion(FaceEmotion emotion);
  void clear();
  void setIntensity(uint8_t intensity);
  FaceEmotion currentEmotion() const;

 private:
  void drawBitmap(const byte bitmap[8]);
  void rotateRight90(const byte source[8], byte target[8]) const;

  FaceMatrixConfig config_;
  LedControl matrix_;
  FaceEmotion currentEmotion_ = FACE_NEUTRAL;
};
