#include "FaceMatrix.h"

const byte kHappyFace[8] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B01000010,
    B00100100,
    B00011000,
};

const byte kNeutralFace[8] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B01111110,
    B00000000,
};

const byte kSadFace[8] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
};

const byte kSurprisedFace[8] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B00100100,
    B00011000,
};

const byte kBlinkFace[8] = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00111100,
    B00000000,
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
  byte rotatedBitmap[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  rotateRight90(bitmap, rotatedBitmap);

  for (uint8_t row = 0; row < 8; ++row) {
    matrix_.setRow(config_.deviceIndex, row, rotatedBitmap[row]);
  }
}

void FaceMatrixController::rotateRight90(const byte source[8],
                                         byte target[8]) const {
  for (uint8_t row = 0; row < 8; ++row) {
    for (uint8_t col = 0; col < 8; ++col) {
      const bool pixelOn = (source[row] & (B10000000 >> col)) != 0;
      if (!pixelOn) {
        continue;
      }

      target[col] |= (B10000000 >> (7 - row));
    }
  }
}
