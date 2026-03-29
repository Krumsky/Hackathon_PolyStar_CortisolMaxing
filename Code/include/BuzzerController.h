#pragma once

#include <Arduino.h>

enum BuzzerTune {
  BUZZER_TUNE_STARTUP = 0,
  BUZZER_TUNE_HAPPY = 1,
  BUZZER_TUNE_UNHAPPY = 2
};

class BuzzerController {
 public:
  explicit BuzzerController(uint8_t buzzerPin = 8);

  void begin();
  void startMelody();
  void playTune(BuzzerTune tune);
  void update();
  bool isPlaying() const;

 private:
  void playCurrentNote();
  void setTune(const int* notes, const int* durationsMs, int noteCount);

  uint8_t buzzerPin_;
  unsigned long noteStartMs_ = 0;
  int currentNoteIndex_ = 0;
  int currentTuneLength_ = 0;
  bool isPlaying_ = false;
  const int* currentNotes_ = 0;
  const int* currentDurationsMs_ = 0;
};
