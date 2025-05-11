#ifndef UTILS_H
#define UTILS_H
#include "raylib.h"

// Timer simples
typedef struct { int frames, active; } Timer;
void TimerStart(Timer *t, int frames);
int TimerTick(Timer *t);

// Random helper
int RandRange(int min, int max);

// Colisão
int RectsOverlap(Rectangle a, Rectangle b);

#endif
