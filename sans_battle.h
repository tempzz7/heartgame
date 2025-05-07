#ifndef SANS_BATTLE_H
#define SANS_BATTLE_H

#include "raylib.h"

// Constantes
#define MAX_BONES 100     // Número máximo de ossos na tela
#define MAX_BLASTERS 10   // Número máximo de blasters
#define MAX_PARTICLES 200 // Número máximo de partículas
#define MAX_PLATFORMS 10  // Número máximo de plataformas
#define HP_MAX 100        // HP máximo do jogador
#define KR_MAX 40         // Karma máxima (barra roxa do Sans)

// Tipos de plataforma
typedef enum {
    PLATFORM_NORMAL,
    PLATFORM_TRAP,
    PLATFORM_BOUNCE,
    PLATFORM_MOVING
} PlatformType;

// Padrões de ataque
typedef enum {
    PATTERN_NONE,
    PATTERN_BONES_BOTTOM,
    PATTERN_BONES_TOP,
    PATTERN_BONES_SIDES,
    PATTERN_BONES_CIRCLE,
    PATTERN_BLASTERS_SIDES,
    PATTERN_BLASTERS_CIRCLE,
    PATTERN_PLATFORMS
} AttackPattern;

// Estrutura para ossos
typedef struct {
    Vector2 pos;
    float speed;
    int dir;
    bool active;
    Rectangle rect;
    bool isBlue;
    float rotation;
} Bone;

// Estrutura para Gaster Blasters
typedef struct {
    Vector2 pos;
    Vector2 target;
    float rotation;
    int state;  // 0=spawn, 1=charge, 2=fire, 3=fadeout
    int timer;
    bool active;
    Rectangle beam;
} Blaster;

// Estrutura para partículas
typedef struct {
    Vector2 pos;
    Vector2 vel;
    float size;
    Color color;
    float life;
    bool active;
} Particle;

// Estrutura para plataformas
typedef struct {
    Rectangle rect;
    PlatformType type;
    Vector2 vel;
    bool active;
    Color color;
} Platform;

// Estrutura para a alma do jogador
typedef struct {
    Vector2 pos;
    Vector2 vel;
    float size;
    Color color;
    bool isBlue;  // Modo gravidade
    bool isRed;   // Modo normal
    float gravity;
    float jumpForce;
    int hp;
    int kr;
    int invFrames;
} Soul;

// Funções para ossos
void SpawnBone(Vector2 position, Vector2 velocity, bool isBlue, float rotation);
void UpdateBones(void);
void DrawBones(void);

// Funções para Gaster Blasters
void SpawnBlaster(Vector2 position, Vector2 target, float rotation);
void UpdateBlasters(void);
void DrawBlasters(void);

// Funções para partículas
void SpawnParticle(Vector2 position, Vector2 velocity, float size, Color color, float life);
void UpdateParticles(void);
void DrawParticles(void);

// Funções para plataformas
void SpawnPlatform(Rectangle rect, PlatformType type, Vector2 velocity);
void UpdatePlatforms(void);
void DrawPlatforms(void);

// Funções para a alma
void DrawSoul(Soul soul);
void UpdateSoul(void);
void HandleSoulCollision(void);

// Funções para a arena
void ChangeArenaSize(Vector2 newSize);
void UpdateArena(void);
void DrawArena(void);

// Funções de UI e gameplay
void DrawHealthBar(int hp, int maxHp);
void GenerateAttackPattern(AttackPattern pattern);
void DisplayDialogue(const char* text, bool isSans);
void DrawMenu(void);
void UpdateMenu(void);
void HandleEnemyTurn(void);
void HandlePlayerTurn(void);
void DrawGameOver(void);
void DrawVictory(void);

#endif // SANS_BATTLE_H