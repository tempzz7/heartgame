#include "utils.h"
#include <stdlib.h>


void TimerStart(Timer *t, int frames) { t->frames = frames; t->active = 1; }
int TimerTick(Timer *t) { if (!t->active) return 0; if (--t->frames <= 0) { t->active = 0; return 1; } return 0; }

int RandRange(int min, int max) { return min + rand() % (max-min+1); }

int RectsOverlap(Rectangle a, Rectangle b) {
    return (a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y);
}
