#include "player.h"
#include "raylib.h"
#include <stdlib.h>


void PlayerInit(Player *p, Vector2 pos) {
    p->size = 16.0f;  // Tamanho um pouco menor para facilitar desvios
    p->speed = 5.0f;  // Velocidade maior para movimentação mais rápida
    p->gravity = 0.6f;
    p->jumpStrength = 10.0f;  // Pulo mais alto
    p->maxFallSpeed = 14.0f;
    p->pos = pos;
    p->vel = (Vector2){0, 0};
    p->onGround = 0;
    p->hp = p->maxHp = 92;
    p->invulnerable = 0;
    p->invulFrames = 0;
    p->isDead = 0;
}

void PlayerUpdate(Player *p, Rectangle battleBox) {
    float dt = GetFrameTime();
    float move = 0;
    
    // Movimento normal
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) move += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) move -= 1.0f;
    
    // Dash rápido para desviar (SHIFT + direção)
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        if (move != 0) {
            move *= 2.5f; // Dash mais rápido na direção do movimento
        }
    }
    
    p->pos.x += move * p->speed * dt * 60.0f;
    
    // Pulo
    if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && p->onGround) {
        p->vel.y = -p->jumpStrength;
        p->onGround = 0;
    }
    if (!p->onGround) {
        p->vel.y += p->gravity * dt * 60.0f;
        if (p->vel.y > p->maxFallSpeed) p->vel.y = p->maxFallSpeed;
        p->pos.y += p->vel.y * dt * 60.0f;
    }
    if (p->pos.x - p->size/2 < battleBox.x)
        p->pos.x = battleBox.x + p->size/2;
    if (p->pos.x + p->size/2 > battleBox.x + battleBox.width)
        p->pos.x = battleBox.x + battleBox.width - p->size/2;
    if (p->pos.y - p->size/2 < battleBox.y)
        p->pos.y = battleBox.y + p->size/2;
    if (p->pos.y + p->size/2 >= battleBox.y + battleBox.height) {
        p->onGround = 1;
        p->pos.y = battleBox.y + battleBox.height - p->size/2;
        p->vel.y = 0;
    }
    if (p->invulnerable) {
        p->invulFrames--;
        if (p->invulFrames <= 0) p->invulnerable = 0;
    }
}

void PlayerDraw(const Player *p) {
    // Usar vermelho em vez de roxo, como na imagem de referência
    Color color = (p->invulnerable && (p->invulFrames/4)%2) ? WHITE : RED;
    // Fade out ao morrer
    int alpha = 255;
    if (p->isDead) alpha = 70;
    color.a = alpha;
    
    // Desenhar um coração pixel art como na imagem de referência
    float size = p->size * 1.5f; // Aumentar um pouco para melhor visualização
    float pixelSize = size / 8.0f;
    
    int x = p->pos.x;
    int y = p->pos.y;
    
    // Linha 1 (topo do coração)
    DrawRectangle(x - 3*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 1*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, color);
    
    // Linha 2
    DrawRectangle(x - 4*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 0*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, color);
    
    // Linha 3
    DrawRectangle(x - 4*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 0*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, color);
    
    // Linha 4
    DrawRectangle(x - 3*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 0*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, color);
    
    // Linha 5
    DrawRectangle(x - 2*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x - 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 0*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, color);
    
    // Linha 6
    DrawRectangle(x - 1*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, color);
    DrawRectangle(x + 0*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, color);
    
    // Partículas ao tomar dano
    if (p->invulnerable && (p->invulFrames > 20)) {
        for (int i = 0; i < 3; i++) {
            DrawCircleV((Vector2){p->pos.x + 15 - rand()%30, p->pos.y + 15 - rand()%30}, 2 + rand()%3, WHITE);
        }
    }
}


void PlayerTakeDamage(Player *p, int dmg) {
    if (!p->invulnerable && !p->isDead) {
        p->hp -= dmg;
        p->invulnerable = 1;
        p->invulFrames = 30;
        if (p->hp < 0) p->hp = 0;
    }
}
