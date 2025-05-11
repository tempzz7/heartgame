#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "common.h" // Definições compartilhadas
#include "player.h"
#include "attack.h"

// GamePhase agora está definido em common.h

// Forward declaration para resolver problemas de dependência circular
typedef struct Game Game;

struct Game {
    Player player;
    AttackManager attacks;
    GamePhase phase;
    GameLevel currentLevel;
    int levelProgress;      // Progresso dentro do nível atual (0-100%)
    int levelStartScore;    // Pontuação no início do nível atual
    int levelEndScore;      // Pontuação alvo para completar o nível
    Rectangle battleBox;
    int frameCount;
    int score;
    int running;
    Music bgMusic;          // Música de fundo
    int musicPlaying;
    int audioResetCounter;  // Contador para reiniciar o áudio periodicamente
    
    // Efeitos visuais específicos de cada nível
    Color bgColorTop;       // Cor do topo do gradiente de fundo
    Color bgColorBottom;    // Cor do fundo do gradiente de fundo
    float effectIntensity;  // Intensidade dos efeitos visuais (0.0-1.0)
};

void SetupLevel(Game *g, GameLevel level);
void GameInit(Game *g);
void GameUpdate(Game *g);
void GameDraw(const Game *g);

#endif
