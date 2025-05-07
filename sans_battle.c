#include "sans_battle.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_W 800
#define SCREEN_H 600
#define ARENA_W 320
#define ARENA_H 160
#define ARENA_X ((SCREEN_W/2)-(ARENA_W/2))
#define ARENA_Y ((SCREEN_H/2)-(ARENA_H/2))
#define HEART_SIZE 16
#define BONE_W 12
#define BONE_H 48
#define INV_FRAMES 60
#define SHAKE_FRAMES 18
#define DIALOGUE_MAX 8

// Diálogo
const char* dialogues[DIALOGUE_MAX] = {
    "Você sente que vai ter um tempo ruim...",
    "Sans sorri para você.",
    "Ossos surgem do chão!",
    "Você sente o peso da gravidade...",
    "A alma ficou azul! Pule para sobreviver.",
    "Sans: heh. não é fácil, né?",
    "Você sobreviveu ao impossível.",
    "Pressione ENTER para recomeçar."
};

// Nome do boss e lore
static const char* bossName = "VAZIO";
static int bossPhase = 1;
static int bossMaxPhase = 4;
static const char* loreTexts[] = {
    "Você sente um vazio profundo...",
    "O VAZIO consome memórias e emoções.",
    "Cada ataque é um fragmento do seu passado.",
    "Sobreviva. Recupere quem você é.",
    "O VAZIO está mudando... prepare-se!",
    "Você está perto do fim... ou do começo?",
    "Parabéns. Você enfrentou o VAZIO."
};
static int loreIndex = 0;
static int loreTimer = 0;
static int loreDuration = 240; // frames

// Variáveis globais
static Vector2 heartPos;
static Vector2 heartVel;
static int heartHP;
static int invFrames;
static int shakeFrames;
static bool battleOver = false;
static bool playerWon = false;
static float battleTimer = 0;
static float patternTimer = 0;
static int currentPattern = 0;
static float fadeAlpha = 1.0f;
static int dialogueIndex = 0;
static bool soulBlue = false;
static float heartYSpeed = 0;
static Bone bones[MAX_BONES];

// Variáveis de fase
static int phaseTransitionTimer = 0;
static bool inPhaseTransition = false;

// Lore por fase
static const char* phaseLore[] = {
    "Você sente um vazio profundo...", // Fase 1
    "O VAZIO distorce a gravidade...", // Fase 2
    "O VAZIO dispara fragmentos de memórias!", // Fase 3
    "O VAZIO está furioso. Prepare-se para o fim!" // Fase 4
};

// Função para desenhar o coração paramétrico
static void DrawHeartParametric(Vector2 pos, float size, Color color, bool blink) {
    int points = 100;
    float pulse = 1.0f + 0.08f * sinf(GetTime()*8);
    if (blink) pulse *= 1.1f;
    Vector2 last = {0};
    for (int i = 0; i <= points; i++) {
        float t = (float)i / points * 2 * PI;
        float x = 16 * powf(sinf(t), 3);
        float y = 13 * cosf(t) - 5 * cosf(2*t) - 2 * cosf(3*t) - cosf(4*t);
        Vector2 p = { pos.x + x * size/32 * pulse, pos.y - y * size/32 * pulse };
        if (i > 0) DrawLineV(last, p, blink ? Fade(color, 0.3f + 0.7f*sinf(GetTime()*20)) : color);
        last = p;
    }
    // Preencher
    for (int r = size/2; r > 0; r -= 2) {
        last = (Vector2){0};
        for (int i = 0; i <= points; i++) {
            float t = (float)i / points * 2 * PI;
            float x = 16 * powf(sinf(t), 3);
            float y = 13 * cosf(t) - 5 * cosf(2*t) - 2 * cosf(3*t) - cosf(4*t);
            Vector2 p = { pos.x + x * r/32 * pulse, pos.y - y * r/32 * pulse };
            if (i > 0) DrawLineV(last, p, blink ? Fade(color, 0.1f + 0.3f*sinf(GetTime()*20)) : Fade(color,0.3f));
            last = p;
        }
    }
}

// Desenhar osso estilizado
static void DrawBone(Vector2 pos, float rot, Color color) {
    Rectangle body = {pos.x-4, pos.y-20, 8, 40};
    Vector2 center = {pos.x, pos.y};
    DrawRectanglePro(body, (Vector2){4,20}, rot, color);
    for (int s = -1; s <= 1; s += 2) {
        float y = pos.y + s*20;
        DrawCircleV((Vector2){pos.x-4, y}, 4, color);
        DrawCircleV((Vector2){pos.x+4, y}, 4, color);
    }
    for (int s = -1; s <= 1; s += 2) {
        float y = pos.y + s*20;
        DrawCircleLines(pos.x-4, y, 4, BLACK);
        DrawCircleLines(pos.x+4, y, 4, BLACK);
    }
    DrawRectangleLinesEx((Rectangle){pos.x-4, pos.y-20, 8, 40}, 1, BLACK);
}

// Padrão de ossos alternando
static void SpawnBonesPattern(int pattern) {
    for (int i = 0; i < MAX_BONES; i++) {
        if (!bones[i].active) {
            if (pattern % 2 == 0) {
                bones[i].pos = (Vector2){ARENA_X + 40 + i*10, ARENA_Y + ARENA_H + BONE_H/2};
                bones[i].speed = 2.5f + (rand()%100)/80.0f;
                bones[i].dir = -1;
            } else {
                bones[i].pos = (Vector2){ARENA_X + 40 + i*10, ARENA_Y - BONE_H/2};
                bones[i].speed = 2.5f + (rand()%100)/80.0f;
                bones[i].dir = 1;
            }
            bones[i].active = true;
            bones[i].rect = (Rectangle){bones[i].pos.x - BONE_W/2, bones[i].pos.y - BONE_H/2, BONE_W, BONE_H};
        }
    }
}

static void InitBattle(void) {
    heartPos = (Vector2){ARENA_X + ARENA_W/2, ARENA_Y + ARENA_H/2};
    heartVel = (Vector2){0,0};
    heartHP = HP_MAX;
    invFrames = 0;
    shakeFrames = 0;
    battleOver = false;
    playerWon = false;
    battleTimer = 0;
    patternTimer = 0;
    currentPattern = 0;
    fadeAlpha = 1.0f;
    dialogueIndex = 0;
    soulBlue = false;
    heartYSpeed = 0;
    for (int i = 0; i < MAX_BONES; i++) bones[i].active = false;
    srand((unsigned int)time(NULL));
}

static void UpdateBattle(void) {
    if (fadeAlpha > 0.0f) fadeAlpha -= GetFrameTime()*1.5f;
    if (fadeAlpha < 0.0f) fadeAlpha = 0.0f;
    if (battleOver) {
        if (IsKeyPressed(KEY_ENTER)) InitBattle();
        return;
    }
    // Alternância de alma
    if (battleTimer > 6.0f && !soulBlue) {
        soulBlue = true;
        dialogueIndex = 4;
    }
    // Movimento do coração
    float speed = soulBlue ? 2.5f : 3.5f;
    heartVel = (Vector2){0,0};
    if (soulBlue) {
        // Alma azul: gravidade e pulo
        if (IsKeyDown(KEY_LEFT))  heartVel.x = -speed;
        if (IsKeyDown(KEY_RIGHT)) heartVel.x = speed;
        heartPos.x += heartVel.x;
        // Gravidade
        heartYSpeed += 0.35f;
        if (heartYSpeed > 6.0f) heartYSpeed = 6.0f;
        heartPos.y += heartYSpeed;
        // Pulo
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE)) {
            if (heartPos.y >= ARENA_Y + ARENA_H - HEART_SIZE) {
                heartYSpeed = -6.5f;
            }
        }
        // Chão
        if (heartPos.y > ARENA_Y + ARENA_H - HEART_SIZE) {
            heartPos.y = ARENA_Y + ARENA_H - HEART_SIZE;
            heartYSpeed = 0;
        }
        // Teto
        if (heartPos.y < ARENA_Y + HEART_SIZE) {
            heartPos.y = ARENA_Y + HEART_SIZE;
            heartYSpeed = 0;
        }
    } else {
        if (IsKeyDown(KEY_LEFT))  heartVel.x = -speed;
        if (IsKeyDown(KEY_RIGHT)) heartVel.x = speed;
        if (IsKeyDown(KEY_UP))    heartVel.y = -speed;
        if (IsKeyDown(KEY_DOWN))  heartVel.y = speed;
        heartPos.x += heartVel.x;
        heartPos.y += heartVel.y;
    }
    // Limitar dentro da arena
    if (heartPos.x < ARENA_X + HEART_SIZE) heartPos.x = ARENA_X + HEART_SIZE;
    if (heartPos.x > ARENA_X + ARENA_W - HEART_SIZE) heartPos.x = ARENA_X + ARENA_W - HEART_SIZE;
    if (heartPos.y < ARENA_Y + HEART_SIZE) heartPos.y = ARENA_Y + HEART_SIZE;
    if (heartPos.y > ARENA_Y + ARENA_H - HEART_SIZE) heartPos.y = ARENA_Y + ARENA_H - HEART_SIZE;
    // Invencibilidade
    if (invFrames > 0) invFrames--;
    // Shake
    if (shakeFrames > 0) shakeFrames--;
    // Alternar padrão de ossos a cada 2.5s
    patternTimer += GetFrameTime();
    if (patternTimer > 2.5f) {
        currentPattern++;
        patternTimer = 0;
        SpawnBonesPattern(currentPattern);
        if (dialogueIndex < 3) dialogueIndex++;
    }
    // Atualizar ossos
    for (int i = 0; i < MAX_BONES; i++) {
        if (bones[i].active) {
            bones[i].pos.y += bones[i].speed * bones[i].dir;
            bones[i].rect.x = bones[i].pos.x - BONE_W/2;
            bones[i].rect.y = bones[i].pos.y - BONE_H/2;
            if (bones[i].pos.y < ARENA_Y - BONE_H || bones[i].pos.y > ARENA_Y + ARENA_H + BONE_H) bones[i].active = false;
            if (!battleOver && invFrames == 0 && CheckCollisionCircleRec(heartPos, HEART_SIZE/2, bones[i].rect)) {
                heartHP -= 8 + rand()%6;
                invFrames = INV_FRAMES;
                shakeFrames = SHAKE_FRAMES;
                if (heartHP <= 0) {
                    heartHP = 0;
                    battleOver = true;
                    playerWon = false;
                    dialogueIndex = 7;
                }
            }
        }
    }
    // Timer da batalha
    battleTimer += GetFrameTime();
    if (battleTimer >= 18.0f && !battleOver) {
        battleOver = true;
        playerWon = true;
        dialogueIndex = 6;
    }
}

static void DrawBattle(void) {
    int shakeX = 0, shakeY = 0;
    if (shakeFrames > 0) {
        shakeX = (rand()%7)-3;
        shakeY = (rand()%7)-3;
    }
    BeginDrawing();
    ClearBackground(BLACK);
    // Arena
    DrawRectangle(ARENA_X-4+shakeX, ARENA_Y-4+shakeY, ARENA_W+8, ARENA_H+8, Fade(BLACK,0.7f));
    DrawRectangleLines(ARENA_X+shakeX, ARENA_Y+shakeY, ARENA_W, ARENA_H, WHITE);
    // Ossos
    for (int i = 0; i < MAX_BONES; i++) {
        if (bones[i].active) {
            DrawBone((Vector2){bones[i].pos.x+shakeX, bones[i].pos.y+shakeY}, 0, (Color){240,240,240,255});
        }
    }
    // Sombra do coração
    DrawHeartParametric((Vector2){heartPos.x+2+shakeX, heartPos.y+2+shakeY}, HEART_SIZE, Fade(BLACK,0.3f), false);
    // Coração paramétrico
    if (invFrames == 0 || (invFrames/4)%2==0)
        DrawHeartParametric((Vector2){heartPos.x+shakeX, heartPos.y+shakeY}, HEART_SIZE, RED, false);
    else
        DrawHeartParametric((Vector2){heartPos.x+shakeX, heartPos.y+shakeY}, HEART_SIZE, YELLOW, true);
    // Fade in
    if (fadeAlpha > 0.0f) DrawRectangle(0,0,SCREEN_W,SCREEN_H, Fade(BLACK,fadeAlpha));
    EndDrawing();
}

// Atualizar fase
static void UpdatePhase(void) {
    // Exemplo de transição automática por tempo (pode ser por HP, timer, etc)
    if (!inPhaseTransition) {
        if (bossPhase == 1 && battleTimer > 8.0f) {
            bossPhase = 2;
            inPhaseTransition = true;
            phaseTransitionTimer = 0;
        } else if (bossPhase == 2 && battleTimer > 16.0f) {
            bossPhase = 3;
            inPhaseTransition = true;
            phaseTransitionTimer = 0;
        } else if (bossPhase == 3 && battleTimer > 24.0f) {
            bossPhase = 4;
            inPhaseTransition = true;
            phaseTransitionTimer = 0;
        }
    } else {
        phaseTransitionTimer++;
        if (phaseTransitionTimer > 120) { // 2 segundos de transição
            inPhaseTransition = false;
        }
    }
}

// HUD aprimorado
static void DrawHUD(void) {
    // Barra de vida estilizada
    int barX = ARENA_X + 20;
    int barY = ARENA_Y + ARENA_H + 24;
    DrawText("HP", barX-30, barY, 20, WHITE);
    DrawRectangle(barX, barY, 184, 16, DARKGRAY);
    DrawRectangle(barX, barY, (int)(184.0f*heartHP/HP_MAX), 16, (heartHP>30)?LIME:ORANGE);
    DrawRectangleLines(barX, barY, 184, 16, WHITE);
    char hpText[16];
    sprintf(hpText, "%d / %d", heartHP, HP_MAX);
    DrawText(hpText, barX+60, barY-2, 18, WHITE);
    // Nome do boss com sombra
    int nameX = ARENA_X + ARENA_W/2 - MeasureText(bossName, 36)/2;
    int nameY = ARENA_Y - 60;
    DrawText(bossName, nameX+2, nameY+2, 36, BLACK);
    DrawText(bossName, nameX, nameY, 36, (Color){120,200,255,255});
    // Fase
    char phaseText[32];
    sprintf(phaseText, "Fase %d/%d", bossPhase, bossMaxPhase);
    DrawText(phaseText, ARENA_X + ARENA_W - 140, ARENA_Y - 40, 22, YELLOW);
    // Lore da fase
    if (!inPhaseTransition) {
        DrawRectangle(ARENA_X+10, ARENA_Y-20, ARENA_W-20, 32, Fade(BLACK,0.8f));
        DrawText(phaseLore[bossPhase-1], ARENA_X+20, ARENA_Y-12, 20, WHITE);
    } else {
        // Transição de fase
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade(BLACK, 0.7f));
        char transText[64];
        sprintf(transText, "TRANSIÇÃO PARA FASE %d!", bossPhase);
        DrawText(transText, SCREEN_W/2 - MeasureText(transText, 32)/2, SCREEN_H/2-16, 32, YELLOW);
    }
}

// Atualizar lore dinâmica
static void UpdateLore(void) {
    if (loreIndex < (int)(sizeof(loreTexts)/sizeof(loreTexts[0]))) {
        loreTimer++;
        if (loreTimer > loreDuration) {
            loreTimer = 0;
            loreIndex++;
        }
    }
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "SANS BATTLE - UNDERTALE STYLE");
    SetTargetFPS(60);
    InitBattle();
    while (!WindowShouldClose()) {
        UpdateBattle();
        UpdatePhase();
        UpdateLore();
        DrawHUD();
        DrawBattle();
    }
    CloseWindow();
    return 0;
}

// Funções não usadas, mas necessárias para linkagem
void SpawnBone(Vector2 position, Vector2 velocity, bool isBlue, float rotation) {}
void UpdateBones(void) {}
void DrawBones(void) {}
void SpawnBlaster(Vector2 position, Vector2 target, float rotation) {}
void UpdateBlasters(void) {}
void DrawBlasters(void) {}
void SpawnParticle(Vector2 position, Vector2 velocity, float size, Color color, float life) {}
void UpdateParticles(void) {}
void DrawParticles(void) {}
void SpawnPlatform(Rectangle rect, PlatformType type, Vector2 velocity) {}
void UpdatePlatforms(void) {}
void DrawPlatforms(void) {}
void DrawSoul(Soul soul) {}
void UpdateSoul(void) {}
void HandleSoulCollision(void) {}
void ChangeArenaSize(Vector2 newSize) {}
void UpdateArena(void) {}
void DrawArena(void) {}
void DrawHealthBar(int hp, int maxHp) {}
void GenerateAttackPattern(AttackPattern pattern) {}
void DisplayDialogue(const char* text, bool isSans) {}
void DrawMenu(void) {}
void UpdateMenu(void) {}
void HandleEnemyTurn(void) {}
void HandlePlayerTurn(void) {}
void DrawGameOver(void) {}
void DrawVictory(void) {} 