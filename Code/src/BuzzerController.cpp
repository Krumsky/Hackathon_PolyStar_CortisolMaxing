#include "BuzzerController.h"

#include <pitches.h>

const int kMelodyNotes[] = {
    NOTE_C5,
    NOTE_E5,
    NOTE_G5,
    NOTE_C6,
    0,
    NOTE_G5,
    NOTE_E5,
    NOTE_C5,
};

const int kMelodyDurationsMs[] = {
    160,
    160,
    160,
    280,
    120,
    160,
    160,
    280,
};

const int kMelodyLength = sizeof(kMelodyNotes) / sizeof(kMelodyNotes[0]);
const int kNoteGapMs = 40;

BuzzerController::BuzzerController(uint8_t buzzerPin) : buzzerPin_(buzzerPin) {
}

void BuzzerController::begin() {
  pinMode(buzzerPin_, OUTPUT);
  noTone(buzzerPin_);
}

void BuzzerController::startMelody() {
  currentNoteIndex_ = 0;
  isPlaying_ = true;
  playCurrentNote();
}

void BuzzerController::update() {
  if (!isPlaying_) {
    return;
  }

  const unsigned long now = millis();
  if (now - noteStartMs_ < (unsigned long)(kMelodyDurationsMs[currentNoteIndex_] + kNoteGapMs)) {
    return;
  }

  currentNoteIndex_++;
  if (currentNoteIndex_ >= kMelodyLength) {
    noTone(buzzerPin_);
    isPlaying_ = false;
    return;
  }

  playCurrentNote();
}

bool BuzzerController::isPlaying() const {
  return isPlaying_;
}

void BuzzerController::playCurrentNote() {
  noteStartMs_ = millis();

  if (kMelodyNotes[currentNoteIndex_] == 0) {
    noTone(buzzerPin_);
    return;
  }

  tone(buzzerPin_, kMelodyNotes[currentNoteIndex_],
       kMelodyDurationsMs[currentNoteIndex_]);
}
