#ifndef GAME_H
#define GAME_H
#include "raylib.h"
#include "player.h"
#include "attack.h"

typedef enum { PHASE_MENU, PHASE_INTRO, PHASE_BATTLE, PHASE_BOSS, PHASE_WIN, PHASE_LOSE } GamePhase;

// Forward declaration para resolver problemas de dependência circular
typedef struct Game Game;

struct Game {
    Player player;
    AttackManager attacks;
    GamePhase phase;
    Rectangle battleBox;
    int frameCount;
    int score;
    int running;
    Music bgMusic;  // Música de fundo
    int musicPlaying;
    int audioResetCounter;  // Contador para reiniciar o áudio periodicamente
};

void GameInit(Game *g);
void GameUpdate(Game *g);
void GameDraw(const Game *g);

#endif
