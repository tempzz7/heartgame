#include "game.h"
#include "hud.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

void GameInit(Game *g) {
    g->battleBox = (Rectangle){120, 100, 520, 300};
    PlayerInit(&g->player, (Vector2){g->battleBox.x + g->battleBox.width/2, g->battleBox.y + g->battleBox.height/2});
    AttackManagerInit(&g->attacks, g->battleBox);
    g->phase = PHASE_MENU;  // Começar no menu
    g->frameCount = 0;
    g->score = 0;
    g->running = 1;
    
    // Inicializar sistema de áudio com alta qualidade
    InitAudioDevice();
    
    // Configurar qualidade de áudio para melhor desempenho
    SetAudioStreamBufferSizeDefault(16384); // Buffer maior para música contínua
    
    // Carregar música de fundo
    g->bgMusic = LoadMusicStream("resources/Condemned Tower - Castlevania Dawn of Sorrow OST.mp3");
    g->musicPlaying = 1;
    
    // Configurar a música para melhor desempenho
    SetMusicVolume(g->bgMusic, 0.5f); // Volume mais alto para melhor experiência
    SetMusicPitch(g->bgMusic, 1.0f); // Pitch normal
    
    // Iniciar reprodução da música
    PlayMusicStream(g->bgMusic);
}

void GameUpdate(Game *g) {
    if (!g->running) return;
    
    // Gerenciamento de áudio para tocar a música completa sem interrupções
    if (g->musicPlaying) {
        // Atualizar a música em cada frame para garantir reprodução contínua
        UpdateMusicStream(g->bgMusic);
        
        // Verificar se a música chegou ao fim e reiniciá-la se necessário
        if (!IsMusicStreamPlaying(g->bgMusic)) {
            PlayMusicStream(g->bgMusic);
        }
    }
    
    // Lógica do menu
    if (g->phase == PHASE_MENU) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            g->phase = PHASE_BATTLE;
            // Reiniciar o jogo
            PlayerInit(&g->player, (Vector2){g->battleBox.x + g->battleBox.width/2, g->battleBox.y + g->battleBox.height/2});
            AttackManagerInit(&g->attacks, g->battleBox);
            g->frameCount = 0;
            g->score = 0;
        }
        // Animação no menu
        g->frameCount++;
        return;
    }
    
    // Verificação para reiniciar quando morto
    if (g->player.isDead) {
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER)) {
            // Reiniciar o jogo completamente
            g->phase = PHASE_BATTLE;
            g->running = 1;
            g->player.isDead = 0;
            g->frameCount = 0;
            g->score = 0;
            
            // Reinicializar o jogador e ataques
            PlayerInit(&g->player, (Vector2){g->battleBox.x + g->battleBox.width/2, g->battleBox.y + g->battleBox.height/2});
            AttackManagerInit(&g->attacks, g->battleBox);
            return;
        }
    }
    
    // Lógica normal do jogo
    PlayerUpdate(&g->player, g->battleBox);
    AttackManagerUpdate(&g->attacks, g->battleBox, g->frameCount);
    
    // Verificação de colisão com hitbox menor (apenas 60% do tamanho visual)
    float hitboxSize = g->player.size * 0.6f;
    if (AttackManagerCheckHit(&g->attacks, &(Rectangle){g->player.pos.x-hitboxSize/2, g->player.pos.y-hitboxSize/2, hitboxSize, hitboxSize})) {
        PlayerTakeDamage(&g->player, 10);
        HUDShowMessage("Ouch!", 30);
    }
    
    // Incrementar pontuação a cada frame (sobreviver = pontuar)
    if (g->phase == PHASE_BATTLE && !g->player.isDead) {
        g->score++;
    }
    
    // Verificação de morte
    if (g->player.hp <= 0 && !g->player.isDead) {
        g->player.isDead = 1;
        g->running = 0;
        HUDShowMessage("Press R to restart", 180);
    }
    
    // Verificação de vitória (exemplo: sobreviver 2000 frames)
    if (g->frameCount > 2000 && g->phase == PHASE_BATTLE) {
        g->phase = PHASE_WIN;
    }
    
    // Verificação para reiniciar quando na tela de vitória
    if (g->phase == PHASE_WIN) {
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            g->phase = PHASE_MENU;
        }
    }
    
    g->frameCount++;
}

void GameDraw(const Game *g) {
    // Desenhar fundo estilizado
    DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK, (Color){20, 10, 40, 255});
    
    // Menu inicial
    if (g->phase == PHASE_MENU) {
        // Título com efeito pulsante
        float scale = 1.0f + 0.1f * sinf(g->frameCount * 0.05f);
        int titleSize = 60 * scale;
        const char *title = "HEART GAME";
        int titleWidth = MeasureText(title, titleSize);
        
        // Desenhar título com sombra
        DrawText(title, GetScreenWidth()/2 - titleWidth/2 + 4, 100 + 4, titleSize, (Color){100, 0, 20, 255});
        DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, titleSize, RED);
        
        // Desenhar coração pixel art decorativo
        float heartScale = 1.0f + 0.2f * sinf(g->frameCount * 0.1f);
        float heartSize = 40.0f * heartScale;
        float pixelSize = heartSize / 8.0f;
        
        int x = GetScreenWidth()/2;
        int y = 220;
        
        Color heartColor = RED;
        
        // Linha 1 (topo do coração)
        DrawRectangle(x - 3*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 2
        DrawRectangle(x - 4*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 3
        DrawRectangle(x - 4*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 4
        DrawRectangle(x - 3*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 5
        DrawRectangle(x - 2*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 6
        DrawRectangle(x - 1*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Instruções
        const char *instructions = "Press ENTER to start";
        int instWidth = MeasureText(instructions, 24);
        DrawText(instructions, GetScreenWidth()/2 - instWidth/2, 300, 24, WHITE);
        
        // Controles
        DrawText("Controls:", 250, 380, 20, LIGHTGRAY);
        DrawText("Arrow Keys - Move", 250, 410, 18, LIGHTGRAY);
        DrawText("Space - Jump", 250, 435, 18, LIGHTGRAY);
        DrawText("R - Restart (when dead)", 250, 460, 18, LIGHTGRAY);
        
        return;
    }
    
    // Desenhar caixa de batalha com efeito
    DrawRectangleLinesEx(g->battleBox, 2, WHITE);
    
    // Desenhar elementos do jogo
    AttackManagerDraw(&g->attacks);
    PlayerDraw(&g->player);
    
    // Desenhar partículas de fundo (efeito estético)
    for (int i = 0; i < 5; i++) {
        float x = g->battleBox.x + GetRandomValue(0, (int)g->battleBox.width);
        float y = g->battleBox.y + GetRandomValue(0, (int)g->battleBox.height);
        float size = GetRandomValue(1, 3);
        DrawCircleV((Vector2){x, y}, size, Fade(WHITE, 0.3f));
    }
    
    // Tela de vitória
    if (g->phase == PHASE_WIN) {
        // Título com efeito pulsante
        float scale = 1.0f + 0.1f * sinf(g->frameCount * 0.05f);
        int titleSize = 50 * scale;
        const char *title = "VICTORY!";
        int titleWidth = MeasureText(title, titleSize);
        
        // Desenhar título com cor verde
        DrawText(title, GetScreenWidth()/2 - titleWidth/2, 140, titleSize, GREEN);
        
        // Mensagem de parabéns
        const char *congrats = "Congratulations!";
        int congratsWidth = MeasureText(congrats, 30);
        DrawText(congrats, GetScreenWidth()/2 - congratsWidth/2, 210, 30, WHITE);
        
        // Mostrar pontuação final
        char scoreText[32];
        sprintf(scoreText, "Final Score: %d", g->score);
        int scoreWidth = MeasureText(scoreText, 30);
        DrawText(scoreText, GetScreenWidth()/2 - scoreWidth/2, 250, 30, GOLD);
        
        // Desenhar coração pixel art decorativo
        float heartScale = 1.0f + 0.2f * sinf(g->frameCount * 0.1f);
        float heartSize = 30.0f * heartScale;
        float pixelSize = heartSize / 8.0f;
        
        int x = GetScreenWidth()/2;
        int y = 300;
        
        Color heartColor = RED;
        
        // Linha 1 (topo do coração)
        DrawRectangle(x - 3*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 3*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 2
        DrawRectangle(x - 4*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 3*pixelSize, y - 2*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 3
        DrawRectangle(x - 4*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 3*pixelSize, y - 1*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 4
        DrawRectangle(x - 3*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 2*pixelSize, y + 0*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 5
        DrawRectangle(x - 2*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x - 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 1*pixelSize, y + 1*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Linha 6
        DrawRectangle(x - 1*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, heartColor);
        DrawRectangle(x + 0*pixelSize, y + 2*pixelSize, pixelSize, pixelSize, heartColor);
        
        // Instruções para reiniciar
        const char *restart = "Press ENTER to restart";
        int restartWidth = MeasureText(restart, 20);
        DrawText(restart, GetScreenWidth()/2 - restartWidth/2, 350, 20, LIGHTGRAY);
        
        return;
    }
    
    // Desenhar HUD
    HUDDraw(g);
}
