#include "raylib.h"
#include "game.h"
#include "utils.h"

int main(void) {
    InitWindow(800, 600, "HEART - Definitive Edition");
    SetTargetFPS(60);
    Game game;
    GameInit(&game);
    while (!WindowShouldClose()) {
        // Verificar tecla R para reiniciar diretamente no loop principal
        if (IsKeyPressed(KEY_R)) {
            // Reiniciar o jogo completamente
            game.phase = PHASE_BATTLE;
            game.running = 1;
            game.player.isDead = 0;
            game.frameCount = 0;
            game.score = 0;
            
            // Reinicializar o jogador e ataques
            PlayerInit(&game.player, (Vector2){game.battleBox.x + game.battleBox.width/2, game.battleBox.y + game.battleBox.height/2});
            AttackManagerInit(&game.attacks, game.battleBox);
            
            // Configurar o primeiro nível
            SetupLevel(&game, LEVEL_VOID);
        }
        
        GameUpdate(&game);
        BeginDrawing();
        ClearBackground(BLACK);
        GameDraw(&game);
        EndDrawing();
    }
    // Liberar recursos de áudio corretamente
    if (game.musicPlaying) {
        StopMusicStream(game.bgMusic);
        UnloadMusicStream(game.bgMusic);
    }
    CloseAudioDevice();
    
    // Desligar
    CloseWindow();
    return 0;
}
