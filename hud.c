#include "raylib.h"
#include "game.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "hud.h"

static char hudMsg[128] = "";
static int hudMsgFrames = 0;

void HUDDraw(const Game *g) {
    // Barra de vida estilizada
    DrawRectangleRounded((Rectangle){30, 30, 210, 30}, 0.3f, 10, (Color){40, 40, 40, 200});
    
    // Calcular largura da barra de vida
    float healthPercent = g->player.hp / (float)g->player.maxHp;
    int healthWidth = (int)(200 * healthPercent);
    
    // Cor da barra de vida baseada na quantidade de vida
    Color healthColor;
    if (healthPercent > 0.7f) {
        healthColor = (Color){50, 200, 50, 255}; // Verde para vida alta
    } else if (healthPercent > 0.3f) {
        healthColor = (Color){230, 180, 40, 255}; // Amarelo para vida média
    } else {
        healthColor = (Color){230, 40, 40, 255}; // Vermelho para vida baixa
    }
    
    // Desenhar barra de vida com borda arredondada
    DrawRectangleRounded((Rectangle){35, 35, healthWidth, 20}, 0.3f, 8, healthColor);
    
    // Texto da vida com sombra
    char healthText[32];
    sprintf(healthText, "HP: %d/%d", g->player.hp, g->player.maxHp);
    DrawText(healthText, 42, 38, 18, (Color){0, 0, 0, 120}); // Sombra
    DrawText(healthText, 40, 36, 18, WHITE);
    
    // Informações de fase e pontuação
    char phaseText[64];
    const char* phaseName = "";
    
    switch(g->phase) {
        case PHASE_MENU: phaseName = "Menu"; break;
        case PHASE_INTRO: phaseName = "Intro"; break;
        case PHASE_BATTLE: phaseName = "Battle"; break;
        case PHASE_BOSS: phaseName = "Boss"; break;
        case PHASE_WIN: phaseName = "Victory"; break;
        case PHASE_LOSE: phaseName = "Defeat"; break;
    }
    
    sprintf(phaseText, "Phase: %s", phaseName);
    DrawText(phaseText, 30, 70, 20, GOLD);
    
    // Pontuação
    DrawText(TextFormat("Score: %d", g->score), GetScreenWidth() - 150, 36, 20, SKYBLUE);
    
    // Mensagem temporária com efeito de fade
    if (hudMsgFrames > 0) {
        float alpha = hudMsgFrames > 30 ? 1.0f : hudMsgFrames / 30.0f;
        float scale = 1.0f + 0.2f * (1.0f - alpha); // Efeito de escala
        int fontSize = 22 * scale;
        
        int textWidth = MeasureText(hudMsg, fontSize);
        int xPos = GetScreenWidth()/2 - textWidth/2;
        
        // Desenhar caixa de mensagem
        DrawRectangleRounded((Rectangle){xPos - 10, 32 - 5, textWidth + 20, fontSize + 10}, 0.3f, 8, Fade(BLACK, 0.7f * alpha));
        
        // Desenhar texto com sombra
        DrawText(hudMsg, xPos + 2, 34, fontSize, Fade((Color){0, 0, 0, 180}, alpha));
        DrawText(hudMsg, xPos, 32, fontSize, Fade(WHITE, alpha));
        
        hudMsgFrames--;
    }
    
    // Tela de morte estilizada
    if (g->player.isDead) {
        // Fundo escuro com gradiente
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), 
                             Fade((Color){20, 0, 0, 200}, 0.8f), 
                             Fade(BLACK, 0.9f));
        
        // Texto "GAME OVER" com efeito de sombra
        const char* gameOverText = "GAME OVER";
        int fontSize = 60;
        int textWidth = MeasureText(gameOverText, fontSize);
        
        // Sombra do texto
        DrawText(gameOverText, GetScreenWidth()/2 - textWidth/2 + 4, 180 + 4, fontSize, (Color){100, 0, 0, 255});
        DrawText(gameOverText, GetScreenWidth()/2 - textWidth/2, 180, fontSize, RED);
        
        // Instruções para reiniciar
        const char* restartText = "Press R to restart";
        int restartWidth = MeasureText(restartText, 24);
        DrawText(restartText, GetScreenWidth()/2 - restartWidth/2, 260, 24, WHITE);
        
        // Desenhar coração quebrado
        float heartSize = 40.0f;
        Vector2 heartPos = {GetScreenWidth()/2, 330};
        
        // Desenhar metades do coração separadas
        DrawCircleV((Vector2){heartPos.x - heartSize/4 - 10, heartPos.y - heartSize/8}, heartSize/3, RED);
        DrawCircleV((Vector2){heartPos.x + heartSize/4 + 10, heartPos.y - heartSize/8}, heartSize/3, RED);
        
        // Desenhar triângulos para as metades do coração
        Vector2 v1 = {heartPos.x - heartSize/2 - 10, heartPos.y - heartSize/8};
        Vector2 v2 = {heartPos.x - 10, heartPos.y - heartSize/8};
        Vector2 v3 = {heartPos.x - heartSize/4 - 5, heartPos.y + heartSize/2};
        DrawTriangle(v1, v2, v3, RED);
        
        Vector2 v4 = {heartPos.x + 10, heartPos.y - heartSize/8};
        Vector2 v5 = {heartPos.x + heartSize/2 + 10, heartPos.y - heartSize/8};
        Vector2 v6 = {heartPos.x + heartSize/4 + 5, heartPos.y + heartSize/2};
        DrawTriangle(v4, v5, v6, RED);
    }
    
    // Tela de vitória estilizada
    if (g->phase == PHASE_WIN) {
        // Fundo claro com gradiente
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(), 
                             Fade((Color){50, 50, 100, 200}, 0.7f), 
                             Fade((Color){20, 20, 50, 200}, 0.8f));
        
        // Texto "VICTORY" com efeito de sombra
        const char* victoryText = "VICTORY!";
        int fontSize = 60;
        int textWidth = MeasureText(victoryText, fontSize);
        
        // Sombra do texto
        DrawText(victoryText, GetScreenWidth()/2 - textWidth/2 + 4, 180 + 4, fontSize, (Color){0, 50, 0, 255});
        DrawText(victoryText, GetScreenWidth()/2 - textWidth/2, 180, fontSize, GREEN);
        
        // Mensagem de parabéns
        const char* congratsText = "Congratulations!";
        int congratsWidth = MeasureText(congratsText, 30);
        DrawText(congratsText, GetScreenWidth()/2 - congratsWidth/2, 260, 30, WHITE);
        
        // Pontuação final
        char scoreText[64];
        sprintf(scoreText, "Final Score: %d", g->score);
        int scoreWidth = MeasureText(scoreText, 24);
        DrawText(scoreText, GetScreenWidth()/2 - scoreWidth/2, 300, 24, GOLD);
        
        // Desenhar coração pulsante
        float pulse = sinf(GetTime() * 5.0f) * 0.2f + 1.0f;
        float heartSize = 50.0f * pulse;
        Vector2 heartPos = {GetScreenWidth()/2, 370};
        
        // Desenhar dois círculos para formar o topo do coração
        DrawCircleV((Vector2){heartPos.x - heartSize/4, heartPos.y - heartSize/8}, heartSize/3, RED);
        DrawCircleV((Vector2){heartPos.x + heartSize/4, heartPos.y - heartSize/8}, heartSize/3, RED);
        
        // Desenhar triângulo para formar a base do coração
        Vector2 v1 = {heartPos.x - heartSize/2, heartPos.y - heartSize/8};
        Vector2 v2 = {heartPos.x + heartSize/2, heartPos.y - heartSize/8};
        Vector2 v3 = {heartPos.x, heartPos.y + heartSize/2};
        DrawTriangle(v1, v2, v3, RED);
        
        // Desenhar partículas de celebração
        for (int i = 0; i < 20; i++) {
            float x = GetRandomValue(0, GetScreenWidth());
            float y = GetRandomValue(0, GetScreenHeight());
            float size = GetRandomValue(2, 5);
            Color particleColor = (Color){
                GetRandomValue(100, 255),
                GetRandomValue(100, 255),
                GetRandomValue(100, 255),
                GetRandomValue(100, 200)
            };
            DrawCircleV((Vector2){x, y}, size, particleColor);
        }
    }
}

void HUDShowMessage(const char *msg, int frames) {
    strncpy(hudMsg, msg, sizeof(hudMsg)-1);
    hudMsg[sizeof(hudMsg)-1] = '\0';
    hudMsgFrames = frames;
}

