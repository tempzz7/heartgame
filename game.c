#include "game.h"
#include "player.h" // Incluir para definição completa de Player
#include "attack.h" // Incluir para definição completa de AttackManager
#include "hud.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>

// Configuração de um nível específico
void SetupLevel(Game *g, GameLevel level) {
    g->currentLevel = level;
    g->levelProgress = 0;
    g->levelStartScore = g->score;
    
    // Configurações específicas para cada nível
    switch (level) {
        case LEVEL_VOID:
            // Nível inicial - Vazio e escuridão
            g->levelEndScore = g->levelStartScore + 300; // 300 pontos para completar
            g->bgColorTop = (Color){5, 0, 10, 255};
            g->bgColorBottom = (Color){15, 0, 30, 255};
            g->effectIntensity = 0.3f;
            HUDShowMessage("Nível 1: O Vazio", 180);
            break;
            
        case LEVEL_MEMORY:
            // Nível de memórias fragmentadas
            g->levelEndScore = g->levelStartScore + 400; // 400 pontos para completar
            g->bgColorTop = (Color){50, 0, 50, 255};
            g->bgColorBottom = (Color){20, 0, 40, 255};
            g->effectIntensity = 0.5f;
            HUDShowMessage("Nível 2: Memórias Fragmentadas", 180);
            break;
            
        case LEVEL_REGRET:
            // Nível de arrependimentos
            g->levelEndScore = g->levelStartScore + 500; // 500 pontos para completar
            g->bgColorTop = (Color){80, 0, 20, 255};
            g->bgColorBottom = (Color){30, 0, 10, 255};
            g->effectIntensity = 0.7f;
            HUDShowMessage("Nível 3: Arrependimentos", 180);
            break;
            
        case LEVEL_FEAR:
            // Nível dos medos profundos
            g->levelEndScore = g->levelStartScore + 600; // 600 pontos para completar
            g->bgColorTop = (Color){20, 20, 20, 255};
            g->bgColorBottom = (Color){5, 5, 15, 255};
            g->effectIntensity = 0.9f;
            HUDShowMessage("Nível 4: Medos Profundos", 180);
            break;
            
        case LEVEL_HOPE:
            // Nível final - esperança
            g->levelEndScore = g->levelStartScore + 700; // 700 pontos para completar
            g->bgColorTop = (Color){40, 0, 60, 255};
            g->bgColorBottom = (Color){10, 0, 30, 255};
            g->effectIntensity = 1.0f;
            HUDShowMessage("Nível Final: Centelha de Esperança", 180);
            break;
            
        default:
            break;
    }
}

void GameInit(Game *g) {
    g->battleBox = (Rectangle){120, 100, 520, 300};
    PlayerInit(&g->player, (Vector2){g->battleBox.x + g->battleBox.width/2, g->battleBox.y + g->battleBox.height/2});
    AttackManagerInit(&g->attacks, g->battleBox);
    g->phase = PHASE_MENU;  // Começar no menu
    g->frameCount = 0;
    g->score = 0;
    g->running = 1;
    
    // Inicializar nível
    g->currentLevel = LEVEL_VOID;
    g->levelProgress = 0;
    g->levelStartScore = 0;
    g->levelEndScore = 300;
    
    // Cores de fundo iniciais
    g->bgColorTop = (Color){5, 0, 10, 255};
    g->bgColorBottom = (Color){15, 0, 30, 255};
    g->effectIntensity = 0.3f;
    
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
    
    // A verificação para reiniciar com R foi movida para o loop principal em main.c
    // para garantir que funcione em todas as situações
    
    // Verificação para reiniciar quando morto com ENTER
    if (g->player.isDead && IsKeyPressed(KEY_ENTER)) {
        // Reiniciar o jogo completamente
        g->phase = PHASE_BATTLE;
        g->running = 1;
        g->player.isDead = 0;
        g->frameCount = 0;
        g->score = 0;
        
        // Reinicializar o jogador e ataques
        PlayerInit(&g->player, (Vector2){g->battleBox.x + g->battleBox.width/2, g->battleBox.y + g->battleBox.height/2});
        AttackManagerInit(&g->attacks, g->battleBox);
        
        // Configurar o primeiro nível
        SetupLevel(g, LEVEL_VOID);
        
        // Manter a música tocando sem reiniciar
        g->musicPlaying = 1;
        return;
    }
    
    // Gerenciar transição entre níveis
    if (g->phase == PHASE_BATTLE) {
        // Calcular progresso no nível atual
        g->levelProgress = (int)(100.0f * (float)(g->score - g->levelStartScore) / (float)(g->levelEndScore - g->levelStartScore));
        
        // Verificar se completou o nível atual
        if (g->score >= g->levelEndScore) {
            // Passar para o próximo nível
            if (g->currentLevel < LEVEL_HOPE) {
                g->phase = PHASE_TRANSITION;
                g->frameCount = 0; // Reiniciar contador para a transição
                
                // Mostrar mensagem de transição
                HUDShowMessage("Nível Completo!", 120);
            } else {
                // Completou todos os níveis
                g->phase = PHASE_WIN;
            }
        }
    }
    
    // Gerenciar a fase de transição
    if (g->phase == PHASE_TRANSITION) {
        g->frameCount++;
        
        // Após 2 segundos (120 frames), passar para o próximo nível
        if (g->frameCount >= 120) {
            g->phase = PHASE_BATTLE;
            
            // Configurar o próximo nível
            SetupLevel(g, g->currentLevel + 1);
        }
        return;
    }
    
    // Lógica normal do jogo
    PlayerUpdate(&g->player, g->battleBox);
    
    // Definir o tipo de movimento do jogador com base no nível atual
    switch (g->currentLevel) {
        case LEVEL_VOID:
            // Movimento livre tradicional
            g->player.moveType = MOVE_FREE;
            break;
            
        case LEVEL_MEMORY:
            // Movimento estilo Undertale - fixo ao chão com pulo
            g->player.moveType = MOVE_PLATFORMER;
            break;
            
        case LEVEL_REGRET:
            // Movimento livre novamente
            g->player.moveType = MOVE_FREE;
            break;
            
        case LEVEL_FEAR:
            // Movimento em plataformas flutuantes
            g->player.moveType = MOVE_PLATFORMS;
            break;
            
        case LEVEL_HOPE:
            // Alterna entre todos os tipos para o desafio final
            if (g->frameCount % 600 < 200) {
                g->player.moveType = MOVE_FREE;
            } else if (g->frameCount % 600 < 400) {
                g->player.moveType = MOVE_PLATFORMER;
            } else {
                g->player.moveType = MOVE_PLATFORMS;
            }
            break;
            
        case LEVEL_COUNT:
            // Caso especial para evitar warning
            g->player.moveType = MOVE_FREE;
            break;
    }
    
    // Passar o nível atual e tipo de movimento para o gerenciador de ataques
    AttackManagerUpdate(&g->attacks, g->battleBox, g->frameCount, g->currentLevel, g->player.moveType);
    
    // Verificar colisão com plataformas se estiver no modo de plataformas
    if (g->player.moveType == MOVE_PLATFORMS) {
        CheckPlatformCollision(&g->attacks, &g->player);
    }
    
    // Verificação de colisão com hitbox menor (apenas 60% do tamanho visual)
    float hitboxSize = g->player.size * 0.6f;
    if (AttackManagerCheckHit(&g->attacks, &(Rectangle){g->player.pos.x-hitboxSize/2, g->player.pos.y-hitboxSize/2, hitboxSize, hitboxSize})) {
        PlayerTakeDamage(&g->player, 10);
        HUDShowMessage("Ouch!", 30);
    }
    
    // Incrementar pontuação a cada frame (sobreviver = pontuar)
    if (g->phase == PHASE_BATTLE && !g->player.isDead) {
        g->score++;
        
        // Otimizar o processamento de áudio em pontos críticos para evitar travamentos
        // sem reiniciar a música
        if (g->score > 1000 && g->score < 1200) {
            // Reduzir a frequência de atualização da música durante este período crítico
            if (g->frameCount % 3 != 0) {
                // Pular algumas atualizações de música para reduzir a carga
                g->musicPlaying = 0;
            } else {
                g->musicPlaying = 1;
            }
        } else {
            // Fora do período crítico, manter a música tocando normalmente
            g->musicPlaying = 1;
        }
    }
    
    // Verificação de morte
    if (g->player.hp <= 0 && !g->player.isDead) {
        g->player.isDead = 1;
        g->running = 0;
        HUDShowMessage("Press R to restart", 180);
    }
    
    // Verificação de vitória (mais difícil: sobreviver 3000 frames)
    if (g->frameCount > 3000 && g->phase == PHASE_BATTLE) {
        g->phase = PHASE_WIN;
    }
    
    // Aumentar a dificuldade gradualmente conforme o jogo avança
    if (g->phase == PHASE_BATTLE && g->frameCount % 500 == 0 && g->frameCount > 0) {
        // Mostrar mensagem perturbadora aleatória
        int msgIndex = GetRandomValue(0, 4);
        const char* messages[] = {
            "Suas memórias estão desaparecendo...",
            "Você sente o vazio se aproximando...",
            "Não há esperança no fim do caminho...",
            "Seus arrependimentos o perseguem...",
            "O coração está se fragmentando..."
        };
        HUDShowMessage(messages[msgIndex], 180);
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
    if (!g->running) return;
    
    // Desenhar fundo com gradiente baseado no nível atual
    for (int y = 0; y < GetScreenHeight(); y += 4) {
        // Calcular a cor interpolada entre o topo e o fundo
        float t = (float)y / GetScreenHeight();
        
        // Adicionar efeito pulsante baseado na intensidade do nível
        float pulse = g->effectIntensity * 0.2f * sinf(g->frameCount * 0.02f);
        
        // Interpolar entre as cores do topo e do fundo
        Color color = {
            (unsigned char)(g->bgColorTop.r * (1-t) + g->bgColorBottom.r * t + pulse * 20),
            (unsigned char)(g->bgColorTop.g * (1-t) + g->bgColorBottom.g * t),
            (unsigned char)(g->bgColorTop.b * (1-t) + g->bgColorBottom.b * t + pulse * 30),
            255
        };
        
        DrawLine(0, y, GetScreenWidth(), y, color);
    }
    
    // Efeitos visuais específicos para cada nível
    if (g->phase == PHASE_BATTLE || g->phase == PHASE_TRANSITION) {
        switch (g->currentLevel) {
            case LEVEL_VOID:
                // Efeito de partículas flutuantes no vazio
                for (int i = 0; i < 20; i++) {
                    float x = fmodf(g->frameCount * 2 + i * 50, (float)GetScreenWidth());
                    float y = 100 + 200 * sinf((g->frameCount + i * 30) * 0.01f);
                    float size = 2 + sinf(g->frameCount * 0.05f + i) * 2;
                    DrawCircle(x, y, size, (Color){80, 20, 120, 100});
                }
                break;
                
            case LEVEL_MEMORY:
                // Fragmentos de memória flutuando
                for (int i = 0; i < 30; i++) {
                    float x = fmodf(g->frameCount + i * 40, (float)GetScreenWidth());
                    float y = 150 + 100 * sinf((g->frameCount + i * 20) * 0.02f);
                    float size = 3 + cosf(g->frameCount * 0.03f + i) * 2;
                    DrawRectangle(x, y, size * 3, size, (Color){120, 0, 150, 150});
                }
                break;
                
            case LEVEL_REGRET:
                // Sombras de arrependimento
                for (int i = 0; i < 15; i++) {
                    float x = fmodf(g->frameCount * 3 + i * 60, (float)GetScreenWidth());
                    float y = 200 + 150 * sinf((g->frameCount + i * 40) * 0.01f);
                    float size = 10 + sinf(g->frameCount * 0.02f + i) * 5;
                    DrawCircle(x, y, size, (Color){100, 0, 20, 80});
                    DrawText("ERRO", x - 20, y - 10, 10, (Color){200, 0, 50, 150});
                }
                break;
                
            case LEVEL_FEAR:
                // Sombras dos medos
                for (int i = 0; i < 25; i++) {
                    float x = fmodf(g->frameCount * 1.5f + i * 70, (float)GetScreenWidth());
                    float y = 100 + 250 * sinf((g->frameCount + i * 25) * 0.015f);
                    float width = 15 + sinf(g->frameCount * 0.03f + i) * 5;
                    float height = 30 + cosf(g->frameCount * 0.02f + i) * 10;
                    DrawRectangle(x, y, width, height, (Color){20, 20, 30, 120});
                }
                break;
                
            case LEVEL_HOPE:
                // Centelhas de esperança
                for (int i = 0; i < 40; i++) {
                    float x = fmodf(g->frameCount * 2.5f + i * 30, (float)GetScreenWidth());
                    float y = 150 + 200 * sinf((g->frameCount + i * 35) * 0.01f);
                    float size = 1 + sinf(g->frameCount * 0.04f + i) * 1;
                    DrawCircle(x, y, size, (Color){200, 200, 255, 180});
                }
                break;
                
            default:
                break;
        }
    }
    
    // Menu inicial sombrio
    if (g->phase == PHASE_MENU) {
        // Efeito de pulsação como um coração agonizante
        float heartbeat = 1.0f + 0.2f * sinf(g->frameCount * 0.08f);
        if (g->frameCount % 120 < 10) heartbeat *= 1.2f; // Batida irregular ocasional
        
        int titleSize = 60 * heartbeat;
        const char *title = "HEART";
        int titleWidth = MeasureText(title, titleSize);
        
        // Desenhar título com efeito de sangue escorrendo
        DrawText(title, GetScreenWidth()/2 - titleWidth/2 + 4, 100 + 4, titleSize, (Color){20, 0, 5, 255});
        DrawText(title, GetScreenWidth()/2 - titleWidth/2, 100, titleSize, (Color){180, 0, 20, 255});
        
        // Subtítulo sombrio
        const char *subtitle = "Entre o Vazio e a Esperança";
        int subtitleWidth = MeasureText(subtitle, 20);
        DrawText(subtitle, GetScreenWidth()/2 - subtitleWidth/2, 170, 20, (Color){150, 150, 150, 255});
        
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
        
        // Efeito de sangue escorrendo aleatoriamente na tela
        for (int i = 0; i < 5; i++) {
            if ((g->frameCount + i*50) % 200 < 100) {
                int x = 100 + i * 100;
                int height = 50 + sinf(g->frameCount * 0.05f + i) * 30;
                DrawRectangleGradientV(x, 50, 2, height, (Color){180, 0, 20, 255}, (Color){100, 0, 10, 100});
            }
        }
        
        // Instruções sombrias
        const char *instructions = "Pressione ENTER para enfrentar seus medos";
        int instWidth = MeasureText(instructions, 22);
        DrawText(instructions, GetScreenWidth()/2 - instWidth/2, 300, 22, (Color){200, 200, 200, (unsigned char)(150 + sinf(g->frameCount * 0.1f) * 50)});
        
        // Mensagem perturbadora que pisca ocasionalmente
        if (g->frameCount % 180 < 30) {
            const char *warning = "Não há escapatoria";
            int warnWidth = MeasureText(warning, 18);
            DrawText(warning, GetScreenWidth()/2 - warnWidth/2, 330, 18, (Color){180, 0, 20, 150});
        }
        
        // Controles
        DrawText("Controles:", 200, 400, 20, (Color){150, 150, 150, 200});
        DrawText("Setas - Mover o que resta de sua alma", 200, 425, 18, (Color){150, 150, 150, 180});
        DrawText("Espaço - Saltar sobre seus arrependimentos", 200, 450, 18, (Color){150, 150, 150, 180});
        DrawText("Shift - Fugir de seus medos (dash)", 200, 475, 18, (Color){150, 150, 150, 180});
        DrawText("R - Tentar novamente (quando despedaçado)", 200, 500, 18, (Color){150, 150, 150, 180});
        
        return;
    }
    
    // Desenhar caixa de batalha como uma jaula de ossos e veias
    float pulseWidth = 2.0f + sinf(g->frameCount * 0.05f) * 0.5f;
    Color borderColor = (Color){150 + (int)(50 * sinf(g->frameCount * 0.02f)), 0, 20, 255};
    
    // Desenhar linhas pulsantes como veias
    DrawRectangleLinesEx(g->battleBox, pulseWidth, borderColor);
    
    // Adicionar efeito de sangue nos cantos da caixa
    if (g->frameCount % 120 < 60) {
        DrawRectangleGradientV(g->battleBox.x, g->battleBox.y, 20, 20, 
                             (Color){180, 0, 20, 200}, (Color){100, 0, 10, 0});
        DrawRectangleGradientV(g->battleBox.x + g->battleBox.width - 20, g->battleBox.y + g->battleBox.height - 20, 
                             20, 20, (Color){180, 0, 20, 200}, (Color){100, 0, 10, 0});
    }
    
    // Adicionar ossos nas bordas da caixa de batalha
    float boneLength = 20.0f;
    float boneWidth = 6.0f;
    Color boneColor = (Color){220, 220, 220, 200};
    
    // Ossos horizontais nas bordas superior e inferior
    for (int i = 0; i < g->battleBox.width; i += 40) {
        // Borda superior
        DrawRectangle(g->battleBox.x + i, g->battleBox.y - boneWidth/2, boneLength, boneWidth, boneColor);
        DrawCircle(g->battleBox.x + i + boneLength/2, g->battleBox.y, boneWidth * 0.8f, boneColor);
        
        // Borda inferior
        DrawRectangle(g->battleBox.x + i, g->battleBox.y + g->battleBox.height - boneWidth/2, boneLength, boneWidth, boneColor);
        DrawCircle(g->battleBox.x + i + boneLength/2, g->battleBox.y + g->battleBox.height, boneWidth * 0.8f, boneColor);
    }
    
    // Ossos verticais nas bordas laterais
    for (int i = 0; i < g->battleBox.height; i += 40) {
        // Borda esquerda
        DrawRectangle(g->battleBox.x - boneWidth/2, g->battleBox.y + i, boneWidth, boneLength, boneColor);
        DrawCircle(g->battleBox.x, g->battleBox.y + i + boneLength/2, boneWidth * 0.8f, boneColor);
        
        // Borda direita
        DrawRectangle(g->battleBox.x + g->battleBox.width - boneWidth/2, g->battleBox.y + i, boneWidth, boneLength, boneColor);
        DrawCircle(g->battleBox.x + g->battleBox.width, g->battleBox.y + i + boneLength/2, boneWidth * 0.8f, boneColor);
    }
    
    // Adicionar mensagens perturbadoras que aparecem e desaparecem no fundo da arena
    if (g->frameCount % 300 < 60) {
        const char* mensagens[] = {
            "NÃO HÁ ESCAPATORIA",
            "SEUS PECADOS TE PERSEGUEM",
            "ACEITE SEU DESTINO",
            "VOCÊ NUNCA ESTARÁ LIVRE",
            "DESISTA"
        };
        
        int msgIndex = (g->frameCount / 300) % 5;
        const char* msg = mensagens[msgIndex];
        int fontSize = 20;
        int textWidth = MeasureText(msg, fontSize);
        
        // Desenhar texto com efeito de sangue escorrendo
        DrawText(msg, 
                g->battleBox.x + g->battleBox.width/2 - textWidth/2, 
                g->battleBox.y + g->battleBox.height/2, 
                fontSize, 
                (Color){180, 0, 20, 50 + (int)(sinf(g->frameCount * 0.1f) * 30)});
    }
    
    // Desenhar elementos do jogo
    AttackManagerDraw(&g->attacks);
    PlayerDraw(&g->player);
    
    // Desenhar partículas sombrias (fragmentos de memórias perdidas)
    for (int i = 0; i < 12; i++) {
        float x = g->battleBox.x + GetRandomValue(0, (int)g->battleBox.width);
        float y = g->battleBox.y + GetRandomValue(0, (int)g->battleBox.height);
        float size = GetRandomValue(1, 3);
        
        // Cores alternando entre vermelho escuro e cinza (memórias de sangue e cinzas)
        Color particleColor;
        if (i % 3 == 0) {
            particleColor = Fade((Color){120, 0, 20, 255}, 0.3f); // Sangue
        } else if (i % 3 == 1) {
            particleColor = Fade((Color){70, 70, 90, 255}, 0.3f); // Cinzas
        } else {
            particleColor = Fade((Color){20, 20, 30, 255}, 0.2f); // Sombras
        }
        
        DrawCircleV((Vector2){x, y}, size, particleColor);
    }
    
    // Adicionar efeito de distorção visual ocasional (sanidade diminuindo)
    if (g->frameCount % 300 < 10) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){200, 0, 0, 30});
    }
    
    // Tela de "vitória" ambivalente - será mesmo uma vitória?
    if (g->phase == PHASE_WIN) {
        // Título com efeito de pulsação como batimentos cardíacos fracos
        float heartbeat = 1.0f + 0.15f * sinf(g->frameCount * 0.04f);
        int titleSize = 50 * heartbeat;
        
        // Alternar entre "LIBERTAÇÃO" e "DESESPERO" para criar ambiguidade
        const char *title;
        if ((g->frameCount / 120) % 2 == 0) {
            title = "LIBERTAÇÃO";
        } else {
            title = "DESESPERO";
        }
        
        int titleWidth = MeasureText(title, titleSize);
        
        // Desenhar título com cor vermelha sangue e sombra
        DrawText(title, GetScreenWidth()/2 - titleWidth/2 + 3, 140 + 3, titleSize, (Color){20, 0, 0, 255});
        DrawText(title, GetScreenWidth()/2 - titleWidth/2, 140, titleSize, (Color){180, 0, 20, 255});
        
        // Mensagem ambígua
        const char *message = "Seu coração encontrou o que procurava?";
        int messageWidth = MeasureText(message, 24);
        DrawText(message, GetScreenWidth()/2 - messageWidth/2, 210, 24, (Color){200, 200, 200, 220});
        
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
