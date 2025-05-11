#ifndef ATTACK_H
#define ATTACK_H
#include "raylib.h"
#include "common.h" // Definições compartilhadas

// Forward declaration para evitar dependências circulares
struct Player;
typedef struct Player Player;

#define MAX_PROJECTILES 64
#define MAX_PLATFORMS 16
#define MAX_OBSTACLES 32

typedef enum { ATK_BONE_H, ATK_BONE_V, ATK_MAGENTA, ATK_YELLOW } AttackType;

// Tipos de plataformas para o modo estilo Undertale
typedef enum {
    PLATFORM_NORMAL,    // Plataforma normal para ficar em pé
    PLATFORM_MOVING,    // Plataforma que se move
    PLATFORM_TEMPORARY, // Plataforma que desaparece após um tempo
    PLATFORM_BOUNCE     // Plataforma que faz o jogador pular mais alto
} PlatformType;

// Tipos de obstáculos para o modo estilo Undertale
typedef enum {
    OBSTACLE_SPIKE,     // Espinhos que causam dano ao tocar
    OBSTACLE_LASER,     // Laser que causa dano ao tocar
    OBSTACLE_MOVING,    // Obstáculo que se move em um padrão
    OBSTACLE_PULSE      // Obstáculo que pulsa (aparece e desaparece)
} ObstacleType;

// Estrutura para plataformas
typedef struct {
    Rectangle rect;
    PlatformType type;
    Vector2 velocity;    // Para plataformas móveis
    int lifetime;        // Para plataformas temporárias
    float bounceForce;   // Para plataformas de salto
    bool active;
} Platform;

// Estrutura para obstáculos
typedef struct {
    Rectangle rect;
    ObstacleType type;
    Vector2 velocity;    // Para obstáculos móveis
    int damage;
    int pulseTime;       // Para obstáculos pulsantes
    bool active;
} Obstacle;

typedef struct {
    Vector2 pos, vel;
    float width, height;
    int type;
    int damage;
    int active;
} Projectile;

struct AttackManager {
    Projectile projectiles[MAX_PROJECTILES];
    Platform platforms[MAX_PLATFORMS];
    int platformCount;
    Obstacle obstacles[MAX_OBSTACLES];
    int obstacleCount;
    int spawnRate;
    int spawnTimer;
    AttackType currentType;
    int phase;
};
typedef struct AttackManager AttackManager;

void AttackManagerInit(AttackManager *am, Rectangle battleBox);
void AttackManagerUpdate(AttackManager *am, Rectangle battleBox, int frameCount, GameLevel currentLevel, PlayerMoveType playerMoveType);
void AttackManagerDraw(const AttackManager *am);
int AttackManagerCheckHit(const AttackManager *am, const Rectangle *playerHitbox);

// Funções para plataformas e obstáculos no estilo Undertale
void SpawnPlatform(AttackManager *am, Rectangle rect, PlatformType type, Vector2 velocity, int lifetime, float bounceForce);
void SpawnObstacle(AttackManager *am, Rectangle rect, ObstacleType type, Vector2 velocity, int damage, int pulseTime);
void UpdatePlatforms(AttackManager *am, Rectangle battleBox, float dt);
void UpdateObstacles(AttackManager *am, Rectangle battleBox, float dt);
int CheckPlatformCollision(const AttackManager *am, Player *player);
int CheckObstacleCollision(const AttackManager *am, const Rectangle *playerHitbox);

#endif
