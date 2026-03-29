#include "BuzzerController.h"

#include <pitches.h>

const int kStartupNotes[] = {
    NOTE_C5,
    NOTE_E5,
    NOTE_G5,
    NOTE_C6,
    0,
    NOTE_G5,
    NOTE_E5,
    NOTE_C5,
};

const int kStartupDurationsMs[] = {
    160,
    160,
    160,
    280,
    120,
    160,
    160,
    280,
};

const int kHappyNotes[] = {
    NOTE_G5,
    NOTE_A5,
    NOTE_B5,
    NOTE_G6,
};

const int kHappyDurationsMs[] = {
    120,
    120,
    160,
    240,
};

const int kUnhappyNotes[] = {
    NOTE_E4,
    NOTE_DS4,
    NOTE_D4,
    NOTE_C4,
};

const int kUnhappyDurationsMs[] = {
    180,
    180,
    220,
    260,
};

const int kStartupLength = sizeof(kStartupNotes) / sizeof(kStartupNotes[0]);
const int kHappyLength = sizeof(kHappyNotes) / sizeof(kHappyNotes[0]);
const int kUnhappyLength = sizeof(kUnhappyNotes) / sizeof(kUnhappyNotes[0]);
const int kNoteGapMs = 40;

BuzzerController::BuzzerController(uint8_t buzzerPin) : buzzerPin_(buzzerPin) {
}

void BuzzerController::begin() {
  pinMode(buzzerPin_, OUTPUT);
  noTone(buzzerPin_);
}

void BuzzerController::startMelody() {
  playTune(BUZZER_TUNE_STARTUP);
}

void BuzzerController::playTune(BuzzerTune tune) {
  switch (tune) {
    case BUZZER_TUNE_STARTUP:
      setTune(kStartupNotes, kStartupDurationsMs, kStartupLength);
      break;
    case BUZZER_TUNE_HAPPY:
      setTune(kHappyNotes, kHappyDurationsMs, kHappyLength);
      break;
    case BUZZER_TUNE_UNHAPPY:
      setTune(kUnhappyNotes, kUnhappyDurationsMs, kUnhappyLength);
      break;
  }

  currentNoteIndex_ = 0;
  isPlaying_ = true;
  playCurrentNote();
}

void BuzzerController::update() {
  if (!isPlaying_) {
    return;
  }

  const unsigned long now = millis();
  if (now - noteStartMs_ <
      (unsigned long)(currentDurationsMs_[currentNoteIndex_] + kNoteGapMs)) {
    return;
  }

  currentNoteIndex_++;
  if (currentNoteIndex_ >= currentTuneLength_) {
    noTone(buzzerPin_);
    isPlaying_ = false;
    return;
  }

  playCurrentNote();
}

bool BuzzerController::isPlaying() const {
  return isPlaying_;
}

void BuzzerController::setTune(const int* notes, const int* durationsMs,
                               int noteCount) {
  currentNotes_ = notes;
  currentDurationsMs_ = durationsMs;
  currentTuneLength_ = noteCount;
}

void BuzzerController::playCurrentNote() {
  noteStartMs_ = millis();

  if (currentNotes_[currentNoteIndex_] == 0) {
    noTone(buzzerPin_);
    return;
  }

  tone(buzzerPin_, currentNotes_[currentNoteIndex_],
       currentDurationsMs_[currentNoteIndex_]);
}
