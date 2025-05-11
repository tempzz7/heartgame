
#include "attack.h"
#include "player.h" // Para a definição completa de Player
#include "raylib.h"
#include <stdlib.h>
#include <math.h>


void AttackManagerInit(AttackManager *am, Rectangle battleBox) {
    // Inicializar projéteis
    for (int i = 0; i < MAX_PROJECTILES; i++) am->projectiles[i].active = 0;
    
    // Inicializar plataformas
    for (int i = 0; i < MAX_PLATFORMS; i++) am->platforms[i].active = false;
    am->platformCount = 0;
    
    // Inicializar obstáculos
    for (int i = 0; i < MAX_OBSTACLES; i++) am->obstacles[i].active = false;
    am->obstacleCount = 0;
    
    // Usar o parâmetro battleBox para evitar warning
    (void)battleBox; // Suprimir warning de parâmetro não utilizado
    
    am->spawnRate = 45;
    am->spawnTimer = 0;
    am->currentType = ATK_BONE_H;
    am->phase = 1;
}

// Função para criar uma plataforma
void SpawnPlatform(AttackManager *am, Rectangle rect, PlatformType type, Vector2 velocity, int lifetime, float bounceForce) {
    if (am->platformCount >= MAX_PLATFORMS) return;
    
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (!am->platforms[i].active) {
            am->platforms[i].rect = rect;
            am->platforms[i].type = type;
            am->platforms[i].velocity = velocity;
            am->platforms[i].lifetime = lifetime;
            am->platforms[i].bounceForce = bounceForce;
            am->platforms[i].active = true;
            am->platformCount++;
            return;
        }
    }
}

// Função para criar um obstáculo
void SpawnObstacle(AttackManager *am, Rectangle rect, ObstacleType type, Vector2 velocity, int damage, int pulseTime) {
    if (am->obstacleCount >= MAX_OBSTACLES) return;
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!am->obstacles[i].active) {
            am->obstacles[i].rect = rect;
            am->obstacles[i].type = type;
            am->obstacles[i].velocity = velocity;
            am->obstacles[i].damage = damage;
            am->obstacles[i].pulseTime = pulseTime;
            am->obstacles[i].active = true;
            am->obstacleCount++;
            return;
        }
    }
}

// Função para atualizar plataformas
void UpdatePlatforms(AttackManager *am, Rectangle battleBox, float dt) {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (!am->platforms[i].active) continue;
        
        // Atualizar posição de plataformas móveis
        if (am->platforms[i].type == PLATFORM_MOVING) {
            am->platforms[i].rect.x += am->platforms[i].velocity.x * dt * 60.0f;
            am->platforms[i].rect.y += am->platforms[i].velocity.y * dt * 60.0f;
            
            // Inverter direção se atingir os limites da caixa de batalha
            if (am->platforms[i].rect.x < battleBox.x || 
                am->platforms[i].rect.x + am->platforms[i].rect.width > battleBox.x + battleBox.width) {
                am->platforms[i].velocity.x *= -1;
            }
            
            if (am->platforms[i].rect.y < battleBox.y || 
                am->platforms[i].rect.y + am->platforms[i].rect.height > battleBox.y + battleBox.height) {
                am->platforms[i].velocity.y *= -1;
            }
        }
        
        // Atualizar tempo de vida de plataformas temporárias
        if (am->platforms[i].type == PLATFORM_TEMPORARY) {
            am->platforms[i].lifetime--;
            if (am->platforms[i].lifetime <= 0) {
                am->platforms[i].active = false;
                am->platformCount--;
            }
        }
    }
}

// Função para atualizar obstáculos
void UpdateObstacles(AttackManager *am, Rectangle battleBox, float dt) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!am->obstacles[i].active) continue;
        
        // Atualizar posição de obstáculos móveis
        if (am->obstacles[i].type == OBSTACLE_MOVING) {
            am->obstacles[i].rect.x += am->obstacles[i].velocity.x * dt * 60.0f;
            am->obstacles[i].rect.y += am->obstacles[i].velocity.y * dt * 60.0f;
            
            // Inverter direção se atingir os limites da caixa de batalha
            if (am->obstacles[i].rect.x < battleBox.x || 
                am->obstacles[i].rect.x + am->obstacles[i].rect.width > battleBox.x + battleBox.width) {
                am->obstacles[i].velocity.x *= -1;
            }
            
            if (am->obstacles[i].rect.y < battleBox.y || 
                am->obstacles[i].rect.y + am->obstacles[i].rect.height > battleBox.y + battleBox.height) {
                am->obstacles[i].velocity.y *= -1;
            }
        }
        
        // Atualizar obstáculos pulsantes
        if (am->obstacles[i].type == OBSTACLE_PULSE) {
            am->obstacles[i].pulseTime--;
            if (am->obstacles[i].pulseTime <= 0) {
                // Alternar entre ativo e inativo
                am->obstacles[i].active = !am->obstacles[i].active;
                am->obstacles[i].pulseTime = 60; // Reset para 1 segundo
            }
        }
    }
}

// Função para verificar colisão do jogador com plataformas
int CheckPlatformCollision(const AttackManager *am, Player *player) {
    if (player->moveType != MOVE_PLATFORMS) return -1;
    
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (!am->platforms[i].active) continue;
        
        Rectangle playerRect = {
            player->pos.x - player->size/2,
            player->pos.y - player->size/2,
            player->size,
            player->size
        };
        
        // Verificar se o jogador está caindo (velocidade positiva)
        if (player->velocityY > 0) {
            // Verificar se a parte inferior do jogador está tocando a parte superior da plataforma
            float playerBottom = playerRect.y + playerRect.height;
            float platformTop = am->platforms[i].rect.y;
            
            // Se o jogador estiver próximo o suficiente da plataforma
            if (fabs(playerBottom - platformTop) < 10) {
                // Verificar se há sobreposição horizontal
                if (playerRect.x + playerRect.width > am->platforms[i].rect.x && 
                    playerRect.x < am->platforms[i].rect.x + am->platforms[i].rect.width) {
                    
                    // Colocar o jogador em cima da plataforma
                    player->pos.y = platformTop - player->size/2;
                    player->velocityY = 0;
                    player->isGrounded = true;
                    player->isJumping = false;
                    player->currentPlatform = i;
                    
                    // Efeito especial para plataforma de salto
                    if (am->platforms[i].type == PLATFORM_BOUNCE) {
                        player->velocityY = -am->platforms[i].bounceForce;
                        player->isGrounded = false;
                        player->isJumping = true;
                    }
                    
                    return i; // Retorna o índice da plataforma
                }
            }
        }
    }
    
    // Se o jogador estiver em uma plataforma, verificar se ainda está nela
    if (player->currentPlatform >= 0 && player->currentPlatform < MAX_PLATFORMS) {
        Rectangle playerRect = {
            player->pos.x - player->size/2,
            player->pos.y - player->size/2,
            player->size,
            player->size
        };
        
        // Verificar se o jogador ainda está na plataforma horizontalmente
        if (playerRect.x + playerRect.width <= am->platforms[player->currentPlatform].rect.x || 
            playerRect.x >= am->platforms[player->currentPlatform].rect.x + am->platforms[player->currentPlatform].rect.width) {
            player->isGrounded = false;
            player->currentPlatform = -1;
        }
    }
    
    return -1; // Nenhuma colisão
}

// Função para verificar colisão do jogador com obstáculos
int CheckObstacleCollision(const AttackManager *am, const Rectangle *playerHitbox) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!am->obstacles[i].active) continue;
        
        // Verificar colisão com a hitbox do jogador
        if (CheckCollisionRecs(*playerHitbox, am->obstacles[i].rect)) {
            // Retornar o dano causado pelo obstáculo
            return am->obstacles[i].damage;
        }
    }
    return 0; // Sem colisão
}

// Função auxiliar para criar projéteis
void SpawnProjectile(AttackManager *am, Vector2 pos, Vector2 vel, AttackType type) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!am->projectiles[i].active) {
            am->projectiles[i].active = 1;
            am->projectiles[i].pos = pos;
            am->projectiles[i].vel = vel;
            am->projectiles[i].type = type;
            
            // Configurar tamanho e dano com base no tipo
            switch (type) {
                case ATK_BONE_H:
                    am->projectiles[i].width = 40;
                    am->projectiles[i].height = 6;
                    am->projectiles[i].damage = 10;
                    break;
                    
                case ATK_BONE_V:
                    am->projectiles[i].width = 6;
                    am->projectiles[i].height = 40;
                    am->projectiles[i].damage = 10;
                    break;
                    
                case ATK_MAGENTA:
                    am->projectiles[i].width = 16;
                    am->projectiles[i].height = 16;
                    am->projectiles[i].damage = 12;
                    break;
                    
                case ATK_YELLOW:
                    am->projectiles[i].width = 20;
                    am->projectiles[i].height = 6;
                    am->projectiles[i].damage = 16;
                    break;
            }
            
            return; // Projétil criado com sucesso
        }
    }
}

void AttackManagerUpdate(AttackManager *am, Rectangle battleBox, int frameCount, GameLevel currentLevel, PlayerMoveType playerMoveType) {
    // Sistema de dificuldade progressiva baseada no nível
    float difficultyMultiplier = 1.0f + (currentLevel * 0.2f) + (frameCount / 1000.0f);
    float dt = GetFrameTime();
    
    // Atualizar projéteis existentes
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (am->projectiles[i].active) {
            // Mover o projétil
            am->projectiles[i].pos.x += am->projectiles[i].vel.x;
            am->projectiles[i].pos.y += am->projectiles[i].vel.y;
            
            // Verificar se saiu da tela
            if (am->projectiles[i].pos.x < -50 || 
                am->projectiles[i].pos.x > GetScreenWidth() + 50 ||
                am->projectiles[i].pos.y < -50 ||
                am->projectiles[i].pos.y > GetScreenHeight() + 50) {
                am->projectiles[i].active = 0;
            }
        }
    }
    
    // Atualizar plataformas e obstáculos para os modos de jogo estilo Undertale
    if (playerMoveType == MOVE_PLATFORMER || playerMoveType == MOVE_PLATFORMS) {
        UpdatePlatforms(am, battleBox, dt);
        UpdateObstacles(am, battleBox, dt);
    }
    
    // Determinar intervalo de spawn com base no nível
    int spawnInterval = (int)(60 / difficultyMultiplier);
    
    // Gerar novos ataques com base no tempo e nível atual
    if (frameCount % spawnInterval == 0) {
        // Cada nível tem seus próprios padrões de ataque, como no Geometry Dash
        switch (currentLevel) {
            case LEVEL_VOID:
                // Padrões do Vazio - ataques simples e esparsos
                if (frameCount % (spawnInterval * 3) == 0) {
                    // Linha horizontal de projéteis básicos
                    for (int i = 0; i < 5; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + i * (battleBox.width / 5), battleBox.y},
                                       (Vector2){0, 1.5f},
                                       ATK_BONE_V);
                    }
                } else {
                    // Projéteis aleatórios
                    for (int i = 0; i < 2; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){rand() % (int)battleBox.width + battleBox.x, battleBox.y},
                                       (Vector2){(rand() % 5 - 2) * 0.3f, 1.0f},
                                       ATK_MAGENTA);
                    }
                }
                break;
                
            case LEVEL_MEMORY:
                // Padrões de Memórias - fragmentos que se movem em padrões quebrados
                if (frameCount % (spawnInterval * 2) == 0) {
                    // Padrão em zigzag
                    for (int i = 0; i < 8; i++) {
                        float xOffset = sinf(i * 0.5f) * 100;
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + battleBox.width/2 + xOffset, battleBox.y},
                                       (Vector2){sinf(i * 0.8f) * 1.5f, 1.5f},
                                       ATK_MAGENTA);
                    }
                } else {
                    // Fragmentos de memória (ossos)
                    for (int i = 0; i < 3; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + (rand() % (int)battleBox.width), battleBox.y},
                                       (Vector2){(rand() % 5 - 2) * 0.4f, 1.2f},
                                       ATK_BONE_V);
                    }
                }
                break;
                
            case LEVEL_REGRET:
                // Padrões de Arrependimento - ataques mais intensos e direcionados
                if (frameCount % (spawnInterval * 4) == 0) {
                    // Espiral de arrependimentos
                    for (int i = 0; i < 12; i++) {
                        float angle = i * (2 * PI / 12);
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + battleBox.width/2, battleBox.y + battleBox.height/2},
                                       (Vector2){cosf(angle) * 2.0f, sinf(angle) * 2.0f},
                                       ATK_BONE_H);
                    }
                } else if (frameCount % (spawnInterval * 2) == 0) {
                    // Palavras de culpa
                    for (int i = 0; i < 4; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + i * (battleBox.width / 4), battleBox.y},
                                       (Vector2){0, 1.8f},
                                       ATK_YELLOW);
                    }
                } else {
                    // Ossos aleatórios
                    for (int i = 0; i < 3; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + (rand() % (int)battleBox.width), battleBox.y},
                                       (Vector2){(rand() % 5 - 2) * 0.3f, 1.5f},
                                       ATK_BONE_H);
                    }
                }
                break;
                
            case LEVEL_FEAR:
                // Padrões de Medo - ataques rápidos e imprevisíveis
                if (frameCount % (spawnInterval * 3) == 0) {
                    // Chuva de medos
                    for (int i = 0; i < 15; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + (rand() % (int)battleBox.width), battleBox.y},
                                       (Vector2){(rand() % 7 - 3) * 0.4f, 2.0f + (rand() % 3) * 0.5f},
                                       rand() % 2 == 0 ? ATK_BONE_H : ATK_YELLOW);
                    }
                } else {
                    // Padrão de ataque em X
                    for (int i = 0; i < 5; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + i * (battleBox.width / 5), battleBox.y},
                                       (Vector2){(i - 2) * 0.5f, 2.0f},
                                       ATK_BONE_V);
                        
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + i * (battleBox.width / 5), battleBox.y},
                                       (Vector2){(2 - i) * 0.5f, 2.0f},
                                       ATK_YELLOW);
                    }
                }
                break;
                
            case LEVEL_HOPE:
                // Padrões de Esperança - ataques intensos mas com padrões mais claros
                if (frameCount % (spawnInterval * 5) == 0) {
                    // Grande espiral de esperança e medo
                    for (int i = 0; i < 16; i++) {
                        float angle = i * (2 * PI / 16);
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + battleBox.width/2, battleBox.y + battleBox.height/2},
                                       (Vector2){cosf(angle) * 2.5f, sinf(angle) * 2.5f},
                                       i % 2 == 0 ? ATK_BONE_H : ATK_YELLOW);
                    }
                } else if (frameCount % (spawnInterval * 2) == 0) {
                    // Padrão de onda
                    for (int i = 0; i < 10; i++) {
                        float xPos = battleBox.x + (i * battleBox.width / 10);
                        float yOffset = sinf(frameCount * 0.05f + i * 0.5f) * 30;
                        SpawnProjectile(am, 
                                       (Vector2){xPos, battleBox.y + yOffset},
                                       (Vector2){0, 2.0f},
                                       i % 3 == 0 ? ATK_YELLOW : ATK_BONE_V);
                    }
                } else {
                    // Ataques rápidos aleatórios
                    for (int i = 0; i < 4; i++) {
                        SpawnProjectile(am, 
                                       (Vector2){battleBox.x + (rand() % (int)battleBox.width), battleBox.y},
                                       (Vector2){(rand() % 5 - 2) * 0.5f, 2.2f},
                                       rand() % 3 == 0 ? ATK_MAGENTA : ATK_BONE_H);
                    }
                }
                break;
                
            default:
                break;
        }
    }
}

// Função para desenhar plataformas
void DrawPlatforms(const AttackManager *am) {
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        if (!am->platforms[i].active) continue;
        
        Color platformColor;
        
        // Cores diferentes para cada tipo de plataforma
        switch (am->platforms[i].type) {
            case PLATFORM_NORMAL:
                platformColor = (Color){100, 200, 100, 255}; // Verde para plataformas normais
                break;
                
            case PLATFORM_MOVING:
                platformColor = (Color){100, 100, 200, 255}; // Azul para plataformas móveis
                break;
                
            case PLATFORM_TEMPORARY:
                // Piscar quando estiver prestes a desaparecer
                if (am->platforms[i].lifetime < 60) {
                    platformColor = (Color){200, 100, 100, 128 + (int)(sinf(GetTime() * 10.0f) * 127.0f)};
                } else {
                    platformColor = (Color){200, 100, 100, 200}; // Vermelho para plataformas temporárias
                }
                break;
                
            case PLATFORM_BOUNCE:
                // Efeito pulsante para plataformas de salto
                float pulse = sinf(GetTime() * 5.0f) * 0.3f + 0.7f;
                platformColor = (Color){200, 200, 0, (unsigned char)(200 * pulse)}; // Amarelo para plataformas de salto
                break;
        }
        
        // Desenhar plataforma com bordas arredondadas
        DrawRectangleRounded(am->platforms[i].rect, 0.3f, 8, platformColor);
        
        // Adicionar detalhes visuais
        if (am->platforms[i].type == PLATFORM_BOUNCE) {
            // Setas para cima indicando plataforma de salto
            float centerX = am->platforms[i].rect.x + am->platforms[i].rect.width / 2;
            float topY = am->platforms[i].rect.y - 5;
            DrawTriangle(
                (Vector2){centerX - 10, topY},
                (Vector2){centerX + 10, topY},
                (Vector2){centerX, topY - 15},
                YELLOW
            );
        }
    }
}

// Função para desenhar obstáculos
void DrawObstacles(const AttackManager *am) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!am->obstacles[i].active) continue;
        
        switch (am->obstacles[i].type) {
            case OBSTACLE_SPIKE:
                // Desenhar espinhos
                {
                    float baseX = am->obstacles[i].rect.x;
                    float baseY = am->obstacles[i].rect.y + am->obstacles[i].rect.height;
                    float width = am->obstacles[i].rect.width;
                    float height = am->obstacles[i].rect.height;
                    int spikes = (int)(width / 10);
                    float spikeWidth = width / spikes;
                    
                    for (int j = 0; j < spikes; j++) {
                        DrawTriangle(
                            (Vector2){baseX + j * spikeWidth, baseY},
                            (Vector2){baseX + (j + 1) * spikeWidth, baseY},
                            (Vector2){baseX + (j + 0.5f) * spikeWidth, baseY - height},
                            RED
                        );
                    }
                }
                break;
                
            case OBSTACLE_LASER:
                // Desenhar laser com efeito de brilho
                {
                    float pulse = sinf(GetTime() * 10.0f) * 0.3f + 0.7f;
                    Color laserColor = (Color){255, 50, 50, (unsigned char)(200 * pulse)};
                    DrawRectangleRec(am->obstacles[i].rect, laserColor);
                    
                    // Adicionar efeito de brilho no centro
                    Rectangle innerRect = {
                        am->obstacles[i].rect.x + am->obstacles[i].rect.width * 0.25f,
                        am->obstacles[i].rect.y + am->obstacles[i].rect.height * 0.25f,
                        am->obstacles[i].rect.width * 0.5f,
                        am->obstacles[i].rect.height * 0.5f
                    };
                    DrawRectangleRec(innerRect, (Color){255, 200, 200, (unsigned char)(180 * pulse)});
                }
                break;
                
            case OBSTACLE_MOVING:
                // Desenhar obstáculo móvel com efeito de movimento
                DrawRectangleRec(am->obstacles[i].rect, (Color){200, 50, 200, 200});
                
                // Adicionar setas indicando direção do movimento
                if (fabs(am->obstacles[i].velocity.x) > fabs(am->obstacles[i].velocity.y)) {
                    // Movimento horizontal
                    float arrowDir = am->obstacles[i].velocity.x > 0 ? 1.0f : -1.0f;
                    float centerX = am->obstacles[i].rect.x + am->obstacles[i].rect.width / 2;
                    float centerY = am->obstacles[i].rect.y + am->obstacles[i].rect.height / 2;
                    
                    DrawTriangle(
                        (Vector2){centerX, centerY - 5},
                        (Vector2){centerX, centerY + 5},
                        (Vector2){centerX + arrowDir * 10, centerY},
                        WHITE
                    );
                } else {
                    // Movimento vertical
                    float arrowDir = am->obstacles[i].velocity.y > 0 ? 1.0f : -1.0f;
                    float centerX = am->obstacles[i].rect.x + am->obstacles[i].rect.width / 2;
                    float centerY = am->obstacles[i].rect.y + am->obstacles[i].rect.height / 2;
                    
                    DrawTriangle(
                        (Vector2){centerX - 5, centerY},
                        (Vector2){centerX + 5, centerY},
                        (Vector2){centerX, centerY + arrowDir * 10},
                        WHITE
                    );
                }
                break;
                
            case OBSTACLE_PULSE:
                // Desenhar obstáculo pulsante com efeito de fade
                {
                    float alpha = (float)am->obstacles[i].pulseTime / 60.0f;
                    DrawRectangleRec(am->obstacles[i].rect, (Color){255, 100, 0, (unsigned char)(200 * alpha)});
                }
                break;
        }
    }
}

void AttackManagerDraw(const AttackManager *am) {
    // Desenhar plataformas e obstáculos primeiro (para que fiquem atrás dos projéteis)
    DrawPlatforms(am);
    DrawObstacles(am);
    
    // Desenhar projéteis
    Color magenta = (Color){255, 0, 255, 255};
    Color yellow = YELLOW;
    Color boneColor = (Color){220, 220, 220, 255}; // Cor de osso mais realista

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        const Projectile *p = &am->projectiles[i];
        if (!p->active) continue;
        
        // Ossos horizontais com detalhes realistas
        if (p->type == ATK_BONE_H) {
            // Base do osso
            DrawRectangleV((Vector2){p->pos.x, p->pos.y-3}, (Vector2){p->width, p->height}, boneColor);
            
            // Adicionar articulações nos ossos
            for (int j = 0; j < p->width; j += 30) {
                DrawCircle(p->pos.x + j, p->pos.y, p->height * 0.8f, (Color){200, 200, 200, 255});
            }
            
            // Palavras de culpa que aparecem nos ossos
            if ((i + (int)GetTime()) % 5 < 1) {
                const char* culpaTexts[] = {"CULPA", "FALHA", "ERRO", "MEDO", "PERDA"};
                int textIdx = i % 5;
                DrawText(culpaTexts[textIdx], p->pos.x + 50, p->pos.y - 15, 16, (Color){180, 0, 20, 200});
            }
        }
        // Ossos verticais com detalhes realistas
        else if (p->type == ATK_BONE_V) {
            // Base do osso
            DrawRectangleV((Vector2){p->pos.x-3, p->pos.y}, (Vector2){p->width, p->height}, boneColor);
            
            // Adicionar articulações nos ossos
            for (int j = 0; j < p->height; j += 30) {
                DrawCircle(p->pos.x, p->pos.y + j, p->width * 0.8f, (Color){200, 200, 200, 255});
            }
            
            // Palavras de arrependimento
            if ((i + (int)GetTime()) % 4 < 1) {
                const char* arrependimentoTexts[] = {"ABANDONO", "TRAIÇÃO", "COVARDIA", "FRAQUEZA"};
                int textIdx = i % 4;
                DrawText(arrependimentoTexts[textIdx], p->pos.x - 40, p->pos.y + 50, 16, (Color){180, 0, 20, 200});
            }
        }
        // Projetos magenta - fragmentos de memórias dolorosas
        else if (p->type == ATK_MAGENTA) {
            // Desenhar fragmento pulsante
            float pulse = sinf(GetTime() * 5.0f + i) * 0.2f + 1.0f;
            DrawRectangleV((Vector2){p->pos.x, p->pos.y}, (Vector2){p->width * pulse, p->height * pulse}, magenta);
            
            // Texto de memória fragmentada
            if (i % 3 == 0) {
                const char* memoriaTexts[] = {"LEMBRANÇA", "TRAUMA", "PESADELO"};
                DrawText(memoriaTexts[i % 3], p->pos.x - 20, p->pos.y - 20, 12, (Color){255, 100, 255, 200});
            }
        }
        // Projetos amarelos - medos profundos
        else if (p->type == ATK_YELLOW) {
            // Desenhar com efeito de distorção
            DrawRectangleV((Vector2){p->pos.x, p->pos.y-3}, (Vector2){p->width, p->height}, yellow);
            
            // Palavras de medo
            if ((i + (int)GetTime()) % 3 < 1) {
                const char* medoTexts[] = {"SOLIDÃO", "VAZIO", "FIM"};
                int textIdx = i % 3;
                DrawText(medoTexts[textIdx], p->pos.x + 100, p->pos.y - 10, 18, (Color){255, 255, 0, 200});
            }
        }
    }
}


int AttackManagerCheckHit(const AttackManager *am, const Rectangle *playerHitbox) {
    // Verificar colisão com projéteis
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        const Projectile *p = &am->projectiles[i];
        if (!p->active) continue;
        Rectangle r;
        if (p->type == ATK_BONE_H || p->type == ATK_YELLOW)
            r = (Rectangle){p->pos.x, p->pos.y-3, p->width, p->height};
        else if (p->type == ATK_BONE_V)
            r = (Rectangle){p->pos.x-3, p->pos.y, p->width, p->height};
        else
            r = (Rectangle){p->pos.x, p->pos.y, p->width, p->height};
        if (CheckCollisionRecs(*playerHitbox, r)) return 1;
    }
    
    // Verificar colisão com obstáculos
    int damage = CheckObstacleCollision(am, playerHitbox);
    if (damage > 0) return 1;
    
    return 0;
}


#include "raylib.h"
#include <stdlib.h>

