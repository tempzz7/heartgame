#ifndef ATTACK_H
#define ATTACK_H
#include "raylib.h"

#define MAX_PROJECTILES 64

typedef enum { ATK_BONE_H, ATK_BONE_V, ATK_MAGENTA, ATK_YELLOW } AttackType;

typedef struct {
    Vector2 pos, vel;
    float width, height;
    int type;
    int damage;
    int active;
} Projectile;

typedef struct {
    Projectile projectiles[MAX_PROJECTILES];
    int spawnTimer;
    int spawnRate;
    AttackType currentType;
    int phase;
} AttackManager;

void AttackManagerInit(AttackManager *am, Rectangle battleBox);
void AttackManagerUpdate(AttackManager *am, Rectangle battleBox, int frameCount);
void AttackManagerDraw(const AttackManager *am);
int AttackManagerCheckHit(const AttackManager *am, const Rectangle *playerHitbox);

#endif
