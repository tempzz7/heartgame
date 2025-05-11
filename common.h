#ifndef COMMON_H
#define COMMON_H

// Fases do jogo
typedef enum {
    PHASE_MENU,
    PHASE_GAME,
    PHASE_BATTLE,
    PHASE_BOSS,
    PHASE_WIN,
    PHASE_LOSE,
    PHASE_GAMEOVER,
    PHASE_TRANSITION // Fase para transição entre níveis
} GamePhase;

// Níveis do jogo
typedef enum {
    LEVEL_VOID,      // Nível 1 - O Vazio
    LEVEL_MEMORY,    // Nível 2 - Memórias Fragmentadas
    LEVEL_REGRET,    // Nível 3 - Arrependimentos
    LEVEL_FEAR,      // Nível 4 - Medos Profundos
    LEVEL_HOPE,      // Nível 5 - Centelha de Esperança
    LEVEL_COUNT      // Total de níveis
} GameLevel;

// Tipos de movimento do jogador
typedef enum {
    MOVE_FREE,       // Movimento livre em todas as direções
    MOVE_PLATFORMER, // Fixo ao chão com pulo (estilo Undertale)
    MOVE_PLATFORMS   // Movimento em plataformas flutuantes
} PlayerMoveType;

// Sem forward declarations - as estruturas são definidas nos seus respectivos arquivos

#endif // COMMON_H
