#include "FaceMatrix.h"

const byte kHappyFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100,
};

const byte kNeutralFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10000001,
    B10111101,
    B01000010,
    B00111100,
};

const byte kSadFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100,
};

const byte kSurprisedFace[8] = {
    B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10011001,
    B01000010,
    B00111100,
};

const byte kBlinkFace[8] = {
    B00111100,
    B01000010,
    B10000001,
    B10111101,
    B10000001,
    B10011001,
    B01000010,
    B00111100,
};

FaceMatrixController::FaceMatrixController(const FaceMatrixConfig& config)
    : config_(config), matrix_(config.dataPin, config.clockPin, config.csPin, 1) {}

void FaceMatrixController::begin() {
  matrix_.shutdown(config_.deviceIndex, false);
  matrix_.setScanLimit(config_.deviceIndex, 7);
  setIntensity(config_.intensity);
  clear();                      // Clear Display
  showEmotion(FACE_NEUTRAL);
}

void FaceMatrixController::showEmotion(FaceEmotion emotion) {
  switch (emotion) {
    case FACE_HAPPY:
      drawBitmap(kHappyFace);
      break;
    case FACE_NEUTRAL:
      drawBitmap(kNeutralFace);
      break;
    case FACE_SAD:
      drawBitmap(kSadFace);
      break;
    case FACE_SURPRISED:
      drawBitmap(kSurprisedFace);
      break;
    case FACE_BLINK:
      drawBitmap(kBlinkFace);
      break;
  }

  currentEmotion_ = emotion;
}

void FaceMatrixController::clear() {
  matrix_.clearDisplay(config_.deviceIndex);
}

void FaceMatrixController::setIntensity(uint8_t intensity) {
  config_.intensity = constrain(intensity, 0, 15);
  matrix_.setIntensity(config_.deviceIndex, config_.intensity);
}

FaceEmotion FaceMatrixController::currentEmotion() const {
  return currentEmotion_;
}

void FaceMatrixController::drawBitmap(const byte bitmap[8]) {
  for (uint8_t row = 0; row < 8; ++row) {
    matrix_.setRow(config_.deviceIndex, row, bitmap[row]);
  }
}
