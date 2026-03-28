#include <Stepper.h>

void stepControlInit(Stepper& s, int speed) { s.setSpeed(speed); }

void stepClockwise(Stepper& s, int step) { s.step(step); }