#pragma once

#include <Arduino.h>

class BuzzerController {
 public:
  explicit BuzzerController(uint8_t buzzerPin = 8);

  void begin();
  void startMelody();
  void update();
  bool isPlaying() const;

 private:
  void playCurrentNote();

  uint8_t buzzerPin_;
  unsigned long noteStartMs_ = 0;
  int currentNoteIndex_ = 0;
  bool isPlaying_ = false;
};
