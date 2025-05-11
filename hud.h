#ifndef HUD_H
#define HUD_H

// Game já está definido em game.h
#include "game.h"

void HUDDraw(const Game *g);
void HUDShowMessage(const char *msg, int frames);

#endif
