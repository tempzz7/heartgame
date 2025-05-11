#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"

typedef struct {
    Vector2 pos, vel;
    float size, speed, gravity, jumpStrength, maxFallSpeed;
    int onGround;
    int hp, maxHp;
    int invulnerable, invulFrames;
    int isDead;
} Player;

void PlayerInit(Player *p, Vector2 pos);
void PlayerUpdate(Player *p, Rectangle battleBox);
void PlayerDraw(const Player *p);
void PlayerTakeDamage(Player *p, int dmg);

#endif
