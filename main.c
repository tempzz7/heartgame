#include "raylib.h"
#include "game.h"
#include "utils.h"

int main(void) {
    InitWindow(800, 600, "HEART - Definitive Edition");
    SetTargetFPS(60);
    Game game;
    GameInit(&game);
    while (!WindowShouldClose()) {
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
