
#include "attack.h"
#include "raylib.h"
#include <stdlib.h>


void AttackManagerInit(AttackManager *am, Rectangle battleBox) {
    // Suprimir warning de parâmetro não utilizado
    (void)battleBox;
    for (int i = 0; i < MAX_PROJECTILES; i++) am->projectiles[i].active = 0;
    am->spawnRate = 45;
    am->spawnTimer = 0;
    am->currentType = ATK_BONE_H;
    am->phase = 1;
}

void AttackManagerUpdate(AttackManager *am, Rectangle battleBox, int frameCount) {
    // Alternância de padrão a cada 600 frames
    if (frameCount % 600 == 0) {
        am->currentType = (AttackType)(((int)am->currentType + 1) % 4);
    }

    if (--am->spawnTimer <= 0) {
        am->spawnTimer = am->spawnRate;
        int num = 7;
        int safe = rand() % num;
        // Ossos horizontais
        if (am->currentType == ATK_BONE_H) {
            for (int i = 0; i < num; i++) {
                if (i == safe) continue;
                Projectile *p = NULL;
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (!am->projectiles[j].active) { p = &am->projectiles[j]; break; }
                }
                if (!p) continue;
                p->active = 1;
                p->type = ATK_BONE_H;
                p->width = battleBox.width;
                p->height = 6;
                p->pos = (Vector2){battleBox.x - battleBox.width, battleBox.y + 18 + i*(battleBox.height-36)/(num-1)};
                p->vel = (Vector2){6, 0};
                p->damage = 10;
            }
        }
        // Ossos verticais
        else if (am->currentType == ATK_BONE_V) {
            for (int i = 0; i < num; i++) {
                if (i == safe) continue;
                Projectile *p = NULL;
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (!am->projectiles[j].active) { p = &am->projectiles[j]; break; }
                }
                if (!p) continue;
                p->active = 1;
                p->type = ATK_BONE_V;
                p->width = 6;
                p->height = battleBox.height;
                p->pos = (Vector2){battleBox.x + 18 + i*(battleBox.width-36)/(num-1), battleBox.y - battleBox.height};
                p->vel = (Vector2){0, 6};
                p->damage = 10;
            }
        }
        // Magenta: segue o jogador
        else if (am->currentType == ATK_MAGENTA) {
            for (int i = 0; i < num; i++) {
                Projectile *p = NULL;
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (!am->projectiles[j].active) { p = &am->projectiles[j]; break; }
                }
                if (!p) continue;
                p->active = 1;
                p->type = ATK_MAGENTA;
                p->width = 16;
                p->height = 16;
                float px = battleBox.x + rand() % (int)battleBox.width;
                float py = battleBox.y - 20;
                p->pos = (Vector2){px, py};
                p->vel = (Vector2){0, 3 + rand()%3};
                p->damage = 12;
            }
        }
        // Amarelo: dano só se o player estiver no chão
        else if (am->currentType == ATK_YELLOW) {
            for (int i = 0; i < num; i++) {
                if (i == safe) continue;
                Projectile *p = NULL;
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (!am->projectiles[j].active) { p = &am->projectiles[j]; break; }
                }
                if (!p) continue;
                p->active = 1;
                p->type = ATK_YELLOW;
                p->width = battleBox.width;
                p->height = 6;
                p->pos = (Vector2){battleBox.x - battleBox.width, battleBox.y + 18 + i*(battleBox.height-36)/(num-1)};
                p->vel = (Vector2){8, 0};
                p->damage = 16;
            }
        }
    }
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *p = &am->projectiles[i];
        if (!p->active) continue;
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;
        // Magenta: segue o player (simples)
        if (p->type == ATK_MAGENTA && p->active) {
            // Poderia buscar o player, mas aqui só cai reto
        }
        // Fora da tela?
        if (p->type == ATK_BONE_H && (p->pos.x > battleBox.x + battleBox.width)) p->active = 0;
        if (p->type == ATK_BONE_V && (p->pos.y > battleBox.y + battleBox.height)) p->active = 0;
        if (p->type == ATK_MAGENTA && (p->pos.y > battleBox.y + battleBox.height)) p->active = 0;
        if (p->type == ATK_YELLOW && (p->pos.x > battleBox.x + battleBox.width)) p->active = 0;
    }
}

void AttackManagerDraw(const AttackManager *am) {
    Color magenta = (Color){255, 0, 255, 255};
    Color yellow = YELLOW;

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        const Projectile *p = &am->projectiles[i];
        if (!p->active) continue;
        if (p->type == ATK_BONE_H)
            DrawRectangleV((Vector2){p->pos.x, p->pos.y-3}, (Vector2){p->width, p->height}, WHITE);
        else if (p->type == ATK_BONE_V)
            DrawRectangleV((Vector2){p->pos.x-3, p->pos.y}, (Vector2){p->width, p->height}, WHITE);
        else if (p->type == ATK_MAGENTA)
            DrawRectangleV((Vector2){p->pos.x, p->pos.y}, (Vector2){p->width, p->height}, magenta);
        else if (p->type == ATK_YELLOW)
            DrawRectangleV((Vector2){p->pos.x, p->pos.y-3}, (Vector2){p->width, p->height}, yellow);
    }
}


int AttackManagerCheckHit(const AttackManager *am, const Rectangle *playerHitbox) {
    // Poderia passar o player inteiro para lógica mais rica

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        const Projectile *p = &am->projectiles[i];
        if (!p->active) continue;
        Rectangle r;
        if (p->type == ATK_BONE_H || p->type == ATK_YELLOW)
            r = (Rectangle){p->pos.x, p->pos.y-3, p->width, p->height};
        else if (p->type == ATK_BONE_V)
            r = (Rectangle){p->pos.x-3, p->pos.y, p->width, p->height};
        else
            r = (Rectangle){p->pos.x, p->pos.y, p->width, p->height};
        if (CheckCollisionRecs(*playerHitbox, r)) return 1;
    }
    return 0;
}


#include "raylib.h"
#include <stdlib.h>

