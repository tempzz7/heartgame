#ifndef PLAYER_H
#define PLAYER_H
#include "raylib.h"
#include "common.h" // Definições compartilhadas

struct Player {
    Vector2 pos, vel;
    float size, speed;
    float gravity, jumpStrength, maxFallSpeed;
    int onGround;
    int hp, maxHp;
    int invulnerable, invulFrames;
    int isDead;
    
    // Variáveis para controle de pulo e gravidade
    PlayerMoveType moveType;
    float velocityY;
    bool isGrounded;
    bool isJumping;
    float jumpForce;
    
    // Plataforma atual (para MOVE_PLATFORMS)
    int currentPlatform;
};
typedef struct Player Player;

void PlayerInit(Player *p, Vector2 pos);
void PlayerUpdate(Player *p, Rectangle battleBox);
void PlayerDraw(const Player *p);
void PlayerTakeDamage(Player *p, int dmg);

#endif
