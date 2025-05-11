#include "player.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>


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
    
    // Inicializar novas variáveis para os diferentes tipos de movimento
    p->moveType = MOVE_FREE; // Começa com movimento livre
    p->velocityY = 0.0f;
    p->isGrounded = false;
    p->isJumping = false;
    p->jumpForce = 12.0f;
    p->currentPlatform = -1; // Nenhuma plataforma inicialmente
}

void PlayerUpdate(Player *p, Rectangle battleBox) {
    float dt = GetFrameTime();
    float move = 0;
    
    // Movimento horizontal (comum a todos os tipos de movimento)
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) move += 1.0f;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) move -= 1.0f;
    
    // Dash rápido para desviar (SHIFT + direção)
    if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
        if (move != 0) {
            move *= 2.5f; // Dash mais rápido na direção do movimento
        }
    }
    
    // Lógica específica para cada tipo de movimento
    switch (p->moveType) {
        case MOVE_FREE: // Movimento livre em todas as direções (original)
            // Movimento horizontal
            p->pos.x += move * p->speed * dt * 60.0f;
            
            // Movimento vertical
            float moveY = 0;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) moveY -= 1.0f;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) moveY += 1.0f;
            p->pos.y += moveY * p->speed * dt * 60.0f;
            
            // Limites da caixa de batalha
            p->onGround = 0; // Sempre "no ar" neste modo
            break;
            
        case MOVE_PLATFORMER: // Estilo Undertale - fixo ao chão com pulo
            // Movimento horizontal
            p->pos.x += move * p->speed * dt * 60.0f;
            
            // Pulo (estilo Undertale)
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && p->isGrounded) {
                p->velocityY = -p->jumpForce;
                p->isGrounded = false;
                p->isJumping = true;
            }
            
            // Aplicar gravidade
            if (!p->isGrounded) {
                p->velocityY += p->gravity * dt * 60.0f;
                if (p->velocityY > p->maxFallSpeed) p->velocityY = p->maxFallSpeed;
            }
            
            // Atualizar posição vertical
            p->pos.y += p->velocityY * dt * 60.0f;
            
            // Verificar se está no chão
            if (p->pos.y + p->size/2 >= battleBox.y + battleBox.height) {
                p->isGrounded = true;
                p->isJumping = false;
                p->pos.y = battleBox.y + battleBox.height - p->size/2;
                p->velocityY = 0;
                p->onGround = 1; // Para compatibilidade
            } else {
                p->onGround = 0;
            }
            break;
            
        case MOVE_PLATFORMS: // Movimento em plataformas flutuantes
            // Movimento horizontal
            p->pos.x += move * p->speed * dt * 60.0f;
            
            // Pulo entre plataformas
            if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) && p->isGrounded) {
                p->velocityY = -p->jumpForce * 1.2f; // Pulo mais alto para alcançar plataformas
                p->isGrounded = false;
                p->isJumping = true;
                p->currentPlatform = -1; // Saiu da plataforma atual
            }
            
            // Aplicar gravidade
            if (!p->isGrounded) {
                p->velocityY += p->gravity * dt * 60.0f;
                if (p->velocityY > p->maxFallSpeed) p->velocityY = p->maxFallSpeed;
            }
            
            // Atualizar posição vertical
            p->pos.y += p->velocityY * dt * 60.0f;
            
            // A detecção de colisão com plataformas é feita externamente
            // em AttackManagerUpdate ou GameUpdate
            p->onGround = p->isGrounded ? 1 : 0; // Para compatibilidade
            break;
    }
    
    // Limites da caixa de batalha (comum a todos os modos)
    if (p->pos.x - p->size/2 < battleBox.x)
        p->pos.x = battleBox.x + p->size/2;
    if (p->pos.x + p->size/2 > battleBox.x + battleBox.width)
        p->pos.x = battleBox.x + battleBox.width - p->size/2;
    if (p->pos.y - p->size/2 < battleBox.y)
        p->pos.y = battleBox.y + p->size/2;
    
    // Atualizar invulnerabilidade
    if (p->invulnerable) {
        p->invulFrames--;
        if (p->invulFrames <= 0) p->invulnerable = 0;
    }
}

void PlayerDraw(const Player *p) {
    // Cor base do coração - vermelho escuro e pulsante
    Color color;
    
    if (p->invulnerable && (p->invulFrames/4)%2) {
        // Quando ferido, pisca em branco com efeito de "choque"
        color = WHITE;
    } else {
        // Coração normal - vermelho escuro com pulsação sutil
        float pulse = sinf(GetTime() * 3.0f) * 0.2f;
        color = (Color){180 + (int)(20 * pulse), 0, 20, 255};
    }
    
    // Efeito de desvanecimento ao morrer - a alma se dissipa
    int alpha = 255;
    if (p->isDead) {
        alpha = 70 + (int)(sinf(GetTime() * 5.0f) * 30.0f); // Pulsação ao morrer
    }
    color.a = alpha;
    
    // Desenhar um coração pixel art fragmentado e pulsante
    float heartbeat = 1.0f + sinf(GetTime() * 3.0f) * 0.1f; // Batimento cardíaco
    float size = p->size * 1.5f * heartbeat; // Tamanho pulsante
    float pixelSize = size / 8.0f;
    
    // Adicionar tremor sutil quando danificado
    int tremor = 0;
    if (p->hp < 50) {
        tremor = GetRandomValue(-1, 1);
    }
    if (p->hp < 20) {
        tremor = GetRandomValue(-2, 2);
    }
    
    int x = p->pos.x + tremor;
    int y = p->pos.y + tremor;
    
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
    
    // Efeitos visuais de dano - fragmentos de emoções perdidas
    if (p->invulnerable) {
        // Partículas de sangue ao tomar dano
        for (int i = 0; i < 8; i++) {
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float dist = GetRandomValue(10, 30);
            float px = p->pos.x + cosf(angle) * dist;
            float py = p->pos.y + sinf(angle) * dist;
            float size = GetRandomValue(1, 4);
            
            // Cores alternando entre vermelho escuro e preto (sangue e vazio)
            Color particleColor;
            if (i % 2 == 0) {
                particleColor = (Color){180, 0, 20, 200 - p->invulFrames}; // Sangue
            } else {
                particleColor = (Color){20, 0, 10, 180 - p->invulFrames}; // Vazio
            }
            
            DrawCircleV((Vector2){px, py}, size, particleColor);
        }
        
        // Efeito de "memórias perdidas" - texto fragmentado que aparece brevemente
        if (p->invulFrames > 30 && p->invulFrames < 50) {
            const char* fragments[] = {"dor", "medo", "perda", "vazio", "fim"};
            int idx = GetRandomValue(0, 4);
            int textWidth = MeasureText(fragments[idx], 12);
            DrawText(fragments[idx], p->pos.x - textWidth/2, p->pos.y - 30, 12, 
                   (Color){180, 180, 180, (unsigned char)(100 + sinf(GetTime() * 10.0f) * 50.0f)});
        }
    }
    
    // Efeito de fragmentação quando a vida está baixa
    if (p->hp < 30 && !p->isDead) {
        for (int i = 0; i < 3; i++) {
            float angle = GetRandomValue(0, 360) * DEG2RAD;
            float dist = GetRandomValue(5, 15);
            float px = p->pos.x + cosf(angle) * dist;
            float py = p->pos.y + sinf(angle) * dist;
            float fragSize = GetRandomValue(1, 3);
            DrawRectangle(px, py, fragSize, fragSize, (Color){180, 0, 20, 150});
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
