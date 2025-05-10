/*******************************************************************************************
*
*   HEART - A game about a heart lost between void and hope
*   
*   This game uses raylib library (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
********************************************************************************************/

#include "raylib.h"
#include "heart_battle.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Constants and Definitions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define BATTLE_BOX_WIDTH 250
#define BATTLE_BOX_HEIGHT 250
#define PLAYER_SIZE 20
#define MAX_PROJECTILES 300
#define PLAYER_SPEED 4.0f
#define MAX_HEALTH 100

// Game State Enumeration
typedef enum {
    INTRO,
    BATTLE,
    PHASE_TRANSITION,
    GAME_OVER
} GameState;

// Attack Pattern Enumeration
typedef enum {
    HORIZONTAL_PATTERN,
    VERTICAL_PATTERN,
    DIAGONAL_PATTERN,
    RANDOM_PATTERN,
    CIRCULAR_PATTERN,
    SPIRAL_PATTERN,
    TOTAL_PATTERNS
} AttackPattern;

// Structures
typedef struct {
    Vector2 position;
    float speed;
    int health;
    Rectangle hitbox;
    bool isInvulnerable;
    int invulnerableFrames;
    Heart heart;       // Visual representation as a heart
} Player;

typedef struct {
    Vector2 position;
    Vector2 speed;
    float size;
    bool active;
    Rectangle hitbox;
} Projectile;

typedef struct {
    AttackPattern pattern;
    int duration;      // Duration in frames
    int elapsed;       // Elapsed frames
    float intensity;   // Controls difficulty
    float spawnRate;   // How often to spawn projectiles
    float spawnCounter;
} AttackPhase;

typedef struct {
    int number;        // Phase number (1, 2, 3)
    int duration;      // Total duration of the phase in seconds
    int elapsedTime;   // Elapsed time in the current phase
    float baseIntensity; // Base difficulty for this phase
    Color backgroundColor; // Background color for this phase
    Color projectileColor; // Color of projectiles
    char* phaseMessage;   // Message to display during phase transition
} GamePhase;

// Global Variables
Player player;
Projectile projectiles[MAX_PROJECTILES];
GameState currentState;
AttackPhase currentAttack;
Rectangle battleBox;
int frameCounter;
int score;
Font gameFont;

// Game phases
GamePhase phases[3];
int currentPhaseIndex;
int phaseTransitionTimer;

// Function Prototypes
void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
void UpdatePlayer(void);
void UpdateProjectiles(void);
void CheckCollisions(void);
void DrawHUD(void);
void SpawnProjectile(Vector2 position, Vector2 speed, float size);
void UpdateAttackPhase(void);
void StartAttackPhase(AttackPattern pattern, int duration, float intensity, float spawnRate);
void HandleHorizontalPattern(void);
void HandleVerticalPattern(void);
void HandleDiagonalPattern(void);
void HandleRandomPattern(void);
void HandleCircularPattern(void);
void HandleSpiralPattern(void);
void InitGamePhases(void);
void StartGamePhase(int phaseIndex);
void UpdateGamePhase(void);
void DrawPhaseTransition(void);

// Main function
int main(void)
{
    // Initialize window and set target FPS
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HEART - Lost Between Void and Hope");
    SetTargetFPS(60);

    // Initialize game
    InitGame();

    // Main game loop
    while (!WindowShouldClose())
    {
        UpdateGame();
        BeginDrawing();
        ClearBackground(BLACK);
        DrawGame();
        EndDrawing();
    }

    // Clean up resources
    CloseWindow();
    return 0;
}

// Initialize game variables and state
void InitGame(void)
{
    // Initialize random seed
    srand(time(NULL));

    // Initialize game state
    currentState = INTRO;
    frameCounter = 0;
    score = 0;

    // Initialize player
    player.position = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
    player.speed = PLAYER_SPEED;
    player.health = MAX_HEALTH;
    player.isInvulnerable = false;
    player.invulnerableFrames = 0;
    player.heart = InitHeart(player.position, 1.0f, PURPLE);
    player.hitbox = GetHeartHitbox(player.heart);

    // Initialize battle box (centered on screen)
    battleBox = (Rectangle){ 
        (SCREEN_WIDTH - BATTLE_BOX_WIDTH) / 2,
        (SCREEN_HEIGHT - BATTLE_BOX_HEIGHT) / 2,
        BATTLE_BOX_WIDTH, 
        BATTLE_BOX_HEIGHT 
    };

    // Initialize projectiles
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        projectiles[i].active = false;
    }

    // Initialize game phases
    InitGamePhases();
    
    // Start with random attack pattern for phase 1
    StartAttackPhase(GetRandomValue(0, TOTAL_PATTERNS-1), 300, phases[0].baseIntensity, 15);
    
    // Load font
    gameFont = GetFontDefault();
}

// Update game logic
void UpdateGame(void)
{
    frameCounter++;

    switch (currentState)
    {
        case INTRO:
            // Wait for user input to start
            if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                StartGamePhase(0); // Start with phase 1
            }
            break;

        case PHASE_TRANSITION:
            // Count down timer for phase transition
            phaseTransitionTimer--;
            if (phaseTransitionTimer <= 0) {
                currentState = BATTLE;
            }
            break;

        case BATTLE:
            // Update player and projectiles
            UpdatePlayer();
            UpdateAttackPhase();
            UpdateProjectiles();
            CheckCollisions();
            UpdateGamePhase();

            // Check if player is dead
            if (player.health <= 0) {
                currentState = GAME_OVER;
            }
            break;

        case GAME_OVER:
            // Wait for restart
            if (IsKeyPressed(KEY_R)) {
                InitGame();
            }
            break;

        default:
            break;
    }
}

// Draw game elements
void DrawGame(void)
{
    switch (currentState)
    {
        case INTRO:
            // Draw intro screen
            DrawText("HEART", SCREEN_WIDTH/2 - MeasureText("HEART", 60)/2, SCREEN_HEIGHT/3, 60, PURPLE);
            DrawText("A heart lost between void and hope", SCREEN_WIDTH/2 - MeasureText("A heart lost between void and hope", 20)/2, SCREEN_HEIGHT/2, 20, WHITE);
            DrawText("Press SPACE or ENTER to start", SCREEN_WIDTH/2 - MeasureText("Press SPACE or ENTER to start", 20)/2, SCREEN_HEIGHT*2/3, 20, GRAY);
            break;

        case PHASE_TRANSITION:
            // Draw phase transition screen
            DrawPhaseTransition();
            break;

        case BATTLE:
            // Set background color based on current phase
            ClearBackground(phases[currentPhaseIndex].backgroundColor);
            
            // Draw battle box
            DrawRectangleLines(battleBox.x, battleBox.y, battleBox.width, battleBox.height, WHITE);

            // Draw player (heart)
            if (player.isInvulnerable && (frameCounter / 5) % 2 == 0) {
                // Blinking effect when invulnerable - skip drawing
            } else {
                DrawHeart(player.heart);
            }

            // Draw projectiles
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (projectiles[i].active) {
                    DrawRectangle(
                        projectiles[i].position.x - projectiles[i].size/2, 
                        projectiles[i].position.y - projectiles[i].size/2, 
                        projectiles[i].size, 
                        projectiles[i].size, 
                        phases[currentPhaseIndex].projectileColor
                    );
                }
            }

            // Draw HUD
            DrawHUD();
            
            // Draw current phase indicator
            DrawText(TextFormat("Fase %d", currentPhaseIndex + 1), SCREEN_WIDTH - 100, 50, 20, GRAY);
            break;

        case GAME_OVER:
            // Draw game over screen
            DrawText("Você perdeu tudo...", SCREEN_WIDTH/2 - MeasureText("Você perdeu tudo...", 40)/2, SCREEN_HEIGHT/2 - 40, 40, PURPLE);
            DrawText("Pressione R para recomeçar", SCREEN_WIDTH/2 - MeasureText("Pressione R para recomeçar", 20)/2, SCREEN_HEIGHT/2 + 40, 20, WHITE);
            break;

        default:
            break;
    }
}

// Update player movement and status
void UpdatePlayer(void)
{
    // Move with arrow keys
    Vector2 oldPosition = player.position;

    if (IsKeyDown(KEY_RIGHT)) player.position.x += player.speed;
    if (IsKeyDown(KEY_LEFT)) player.position.x -= player.speed;
    if (IsKeyDown(KEY_DOWN)) player.position.y += player.speed;
    if (IsKeyDown(KEY_UP)) player.position.y -= player.speed;

    // Update hitbox to match heart shape
    player.hitbox = GetHeartHitbox(player.heart);

    // Keep player inside battle box
    if (player.position.x - PLAYER_SIZE/2 < battleBox.x) player.position.x = battleBox.x + PLAYER_SIZE/2;
    if (player.position.y - PLAYER_SIZE/2 < battleBox.y) player.position.y = battleBox.y + PLAYER_SIZE/2;
    if (player.position.x + PLAYER_SIZE/2 > battleBox.x + battleBox.width) player.position.x = battleBox.x + battleBox.width - PLAYER_SIZE/2;
    if (player.position.y + PLAYER_SIZE/2 > battleBox.y + battleBox.height) player.position.y = battleBox.y + battleBox.height - PLAYER_SIZE/2;

    // Update heart position
    player.heart.position = player.position;
    
    // Update heart pulse animation
    UpdateHeartPulse(&player.heart, GetFrameTime());

    // Handle invulnerability
    if (player.isInvulnerable) {
        player.invulnerableFrames--;
        if (player.invulnerableFrames <= 0) {
            player.isInvulnerable = false;
        }
    }
}

// Update projectiles
void UpdateProjectiles(void)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (projectiles[i].active) {
            // Move projectile
            projectiles[i].position.x += projectiles[i].speed.x;
            projectiles[i].position.y += projectiles[i].speed.y;
            
            // Update hitbox
            projectiles[i].hitbox = (Rectangle){ 
                projectiles[i].position.x - projectiles[i].size/2, 
                projectiles[i].position.y - projectiles[i].size/2, 
                projectiles[i].size, 
                projectiles[i].size 
            };

            // Deactivate if outside screen
            if (projectiles[i].position.x < -50 || 
                projectiles[i].position.x > SCREEN_WIDTH + 50 || 
                projectiles[i].position.y < -50 || 
                projectiles[i].position.y > SCREEN_HEIGHT + 50) {
                projectiles[i].active = false;
            }
        }
    }
}

// Check collisions between player and projectiles
void CheckCollisions(void)
{
    if (!player.isInvulnerable) {
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                if (CheckCollisionRecs(player.hitbox, projectiles[i].hitbox)) {
                    // Player hit by projectile
                    player.health -= 10;
                    player.isInvulnerable = true;
                    player.invulnerableFrames = 30; // Half a second at 60 FPS
                    projectiles[i].active = false;
                    
                    // Play hit sound
                    PlaySound(LoadSound("resources/damage.wav")); // Ensure you create this file
                    
                    break; // Only process one hit at a time
                }
            }
        }
    }
}

// Draw the HUD with player health and score
void DrawHUD(void)
{
    // Draw health bar
    DrawText("HP", 20, 20, 20, WHITE);
    DrawRectangleLines(60, 20, 200, 20, WHITE);
    DrawRectangle(60, 20, player.health * 2, 20, PURPLE);
    DrawText(TextFormat("%d/%d", player.health, MAX_HEALTH), 270, 20, 20, WHITE);
    
    // Draw score
    DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH - 200, 20, 20, WHITE);
}

// Spawn a new projectile
void SpawnProjectile(Vector2 position, Vector2 speed, float size)
{
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!projectiles[i].active) {
            projectiles[i].position = position;
            projectiles[i].speed = speed;
            projectiles[i].size = size;
            projectiles[i].active = true;
            projectiles[i].hitbox = (Rectangle){ position.x - size/2, position.y - size/2, size, size };
            break;
        }
    }
}

// Update the current attack phase
void UpdateAttackPhase(void)
{
    currentAttack.elapsed++;
    currentAttack.spawnCounter--;
    
    // Handle the current pattern
    switch (currentAttack.pattern)
    {
        case HORIZONTAL_PATTERN:
            HandleHorizontalPattern();
            break;
        case VERTICAL_PATTERN:
            HandleVerticalPattern();
            break;
        case DIAGONAL_PATTERN:
            HandleDiagonalPattern();
            break;
        case RANDOM_PATTERN:
            HandleRandomPattern();
            break;
        case CIRCULAR_PATTERN:
            HandleCircularPattern();
            break;
        case SPIRAL_PATTERN:
            HandleSpiralPattern();
            break;
        default:
            break;
    }
    
    // Check if phase is complete
    if (currentAttack.elapsed >= currentAttack.duration) {
        // Select next attack pattern (different from current)
        int nextPattern;
        do {
            nextPattern = GetRandomValue(0, TOTAL_PATTERNS-1);
        } while (nextPattern == currentAttack.pattern);
        
        // Increase difficulty over time
        float newIntensity = currentAttack.intensity + 0.1f;
        if (newIntensity > 3.0f) newIntensity = 3.0f;
        
        // Start new phase
        StartAttackPhase(nextPattern, 
                         GetRandomValue(250, 400), // Random duration
                         newIntensity,
                         GetRandomValue(10, 20) - (newIntensity * 3)); // Faster spawns as intensity increases
    }
}

// Start a new attack phase
void StartAttackPhase(AttackPattern pattern, int duration, float intensity, float spawnRate)
{
    currentAttack.pattern = pattern;
    currentAttack.duration = duration;
    currentAttack.elapsed = 0;
    currentAttack.intensity = intensity;
    currentAttack.spawnRate = spawnRate;
    currentAttack.spawnCounter = 0;
}

// Handle horizontal projectile pattern
void HandleHorizontalPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter
        currentAttack.spawnCounter = currentAttack.spawnRate;
        
        // Determine spawn side (left/right)
        bool fromLeft = (GetRandomValue(0, 1) == 0);
        
        // Create multiple projectiles in a row
        int numProjectiles = GetRandomValue(1, 3) + (int)currentAttack.intensity;
        for (int i = 0; i < numProjectiles; i++) {
            float yPos = battleBox.y + GetRandomValue(0, (int)battleBox.height);
            
            Vector2 pos, speed;
            if (fromLeft) {
                pos = (Vector2){ battleBox.x - 10, yPos };
                speed = (Vector2){ GetRandomValue(2, 4) * currentAttack.intensity, 0 };
            } else {
                pos = (Vector2){ battleBox.x + battleBox.width + 10, yPos };
                speed = (Vector2){ -GetRandomValue(2, 4) * currentAttack.intensity, 0 };
            }
            
            SpawnProjectile(pos, speed, 10);
        }
    }
}

// Handle vertical projectile pattern
void HandleVerticalPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter
        currentAttack.spawnCounter = currentAttack.spawnRate;
        
        // Determine spawn side (top/bottom)
        bool fromTop = (GetRandomValue(0, 1) == 0);
        
        // Create multiple projectiles in a row
        int numProjectiles = GetRandomValue(1, 3) + (int)currentAttack.intensity;
        for (int i = 0; i < numProjectiles; i++) {
            float xPos = battleBox.x + GetRandomValue(0, (int)battleBox.width);
            
            Vector2 pos, speed;
            if (fromTop) {
                pos = (Vector2){ xPos, battleBox.y - 10 };
                speed = (Vector2){ 0, GetRandomValue(2, 4) * currentAttack.intensity };
            } else {
                pos = (Vector2){ xPos, battleBox.y + battleBox.height + 10 };
                speed = (Vector2){ 0, -GetRandomValue(2, 4) * currentAttack.intensity };
            }
            
            SpawnProjectile(pos, speed, 10);
        }
    }
}

// Handle diagonal projectile pattern
void HandleDiagonalPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter
        currentAttack.spawnCounter = currentAttack.spawnRate;
        
        // Pick a corner
        int corner = GetRandomValue(0, 3);
        Vector2 pos, speed;
        
        switch (corner) {
            case 0: // Top-left
                pos = (Vector2){ battleBox.x - 10, battleBox.y - 10 };
                speed = (Vector2){ GetRandomValue(1, 3) * currentAttack.intensity, GetRandomValue(1, 3) * currentAttack.intensity };
                break;
            case 1: // Top-right
                pos = (Vector2){ battleBox.x + battleBox.width + 10, battleBox.y - 10 };
                speed = (Vector2){ -GetRandomValue(1, 3) * currentAttack.intensity, GetRandomValue(1, 3) * currentAttack.intensity };
                break;
            case 2: // Bottom-left
                pos = (Vector2){ battleBox.x - 10, battleBox.y + battleBox.height + 10 };
                speed = (Vector2){ GetRandomValue(1, 3) * currentAttack.intensity, -GetRandomValue(1, 3) * currentAttack.intensity };
                break;
            case 3: // Bottom-right
                pos = (Vector2){ battleBox.x + battleBox.width + 10, battleBox.y + battleBox.height + 10 };
                speed = (Vector2){ -GetRandomValue(1, 3) * currentAttack.intensity, -GetRandomValue(1, 3) * currentAttack.intensity };
                break;
        }
        
        SpawnProjectile(pos, speed, 10);
    }
}

// Handle random projectile pattern (unpredictable)
void HandleRandomPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter (faster spawns)
        currentAttack.spawnCounter = currentAttack.spawnRate * 0.7;
        
        // Create projectile at a random position around the battle box
        int side = GetRandomValue(0, 3); // 0: top, 1: right, 2: bottom, 3: left
        Vector2 pos, speed;
        
        switch (side) {
            case 0: // Top
                pos = (Vector2){ battleBox.x + GetRandomValue(0, (int)battleBox.width), battleBox.y - 10 };
                speed = (Vector2){ GetRandomValue(-3, 3), GetRandomValue(1, 4) };
                break;
            case 1: // Right
                pos = (Vector2){ battleBox.x + battleBox.width + 10, battleBox.y + GetRandomValue(0, (int)battleBox.height) };
                speed = (Vector2){ GetRandomValue(-4, -1), GetRandomValue(-3, 3) };
                break;
            case 2: // Bottom
                pos = (Vector2){ battleBox.x + GetRandomValue(0, (int)battleBox.width), battleBox.y + battleBox.height + 10 };
                speed = (Vector2){ GetRandomValue(-3, 3), GetRandomValue(-4, -1) };
                break;
            case 3: // Left
                pos = (Vector2){ battleBox.x - 10, battleBox.y + GetRandomValue(0, (int)battleBox.height) };
                speed = (Vector2){ GetRandomValue(1, 4), GetRandomValue(-3, 3) };
                break;
        }
        
        // Scale speed by intensity
        speed.x *= currentAttack.intensity;
        speed.y *= currentAttack.intensity;
        
        // Vary projectile size for more challenge
        float size = GetRandomValue(8, 15);
        
        SpawnProjectile(pos, speed, size);
    }
}

// Handle circular pattern of projectiles
void HandleCircularPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter
        currentAttack.spawnCounter = currentAttack.spawnRate * 1.5;
        
        // Spawn projectiles in a circle
        int numProjectiles = 8 + (int)(currentAttack.intensity * 2);
        float angleStep = 360.0f / numProjectiles;
        
        // Center of the battle box
        Vector2 center = {
            battleBox.x + battleBox.width / 2,
            battleBox.y + battleBox.height / 2
        };
        
        // Spawn circle of projectiles
        for (int i = 0; i < numProjectiles; i++) {
            float angle = i * angleStep;
            float rad = angle * DEG2RAD;
            
            // Position outside the battle box
            Vector2 pos = {
                center.x + cosf(rad) * (battleBox.width/2 + 30),
                center.y + sinf(rad) * (battleBox.height/2 + 30)
            };
            
            // Speed pointing toward center
            Vector2 speed = {
                -cosf(rad) * 2 * currentAttack.intensity,
                -sinf(rad) * 2 * currentAttack.intensity
            };
            
            SpawnProjectile(pos, speed, 10);
        }
    }
}

// Handle spiral pattern of projectiles
void HandleSpiralPattern(void)
{
    if (currentAttack.spawnCounter <= 0) {
        // Reset spawn counter (faster for spiral)
        currentAttack.spawnCounter = currentAttack.spawnRate * 0.5;
        
        // Center of the battle box
        Vector2 center = {
            battleBox.x + battleBox.width / 2,
            battleBox.y + battleBox.height / 2
        };
        
        // Calculate angle based on elapsed time for spiral effect
        float angle = (currentAttack.elapsed * 10) % 360;
        float rad = angle * DEG2RAD;
        
        // Position outside the battle box
        Vector2 pos = {
            center.x + cosf(rad) * (battleBox.width/2 + 50),
            center.y + sinf(rad) * (battleBox.height/2 + 50)
        };
        
        // Speed pointing toward center with a tangential component for spiral
        Vector2 speed = {
            (-cosf(rad) * 3 + sinf(rad)) * currentAttack.intensity,
            (-sinf(rad) * 3 - cosf(rad)) * currentAttack.intensity
        };
        
        SpawnProjectile(pos, speed, 8);
    }
}

// Initialize the game phases
void InitGamePhases(void)
{
    // Phase 1: The Void
    phases[0].number = 1;
    phases[0].duration = 30; // 30 seconds
    phases[0].elapsedTime = 0;
    phases[0].baseIntensity = 1.0f;
    phases[0].backgroundColor = BLACK;
    phases[0].projectileColor = WHITE;
    phases[0].phaseMessage = "A escuridão do vazio";
    
    // Phase 2: The Struggle 
    phases[1].number = 2;
    phases[1].duration = 45; // 45 seconds
    phases[1].elapsedTime = 0;
    phases[1].baseIntensity = 1.5f;
    phases[1].backgroundColor = (Color){ 20, 20, 40, 255 }; // Dark blue
    phases[1].projectileColor = (Color){ 230, 230, 230, 255 }; // Light grey
    phases[1].phaseMessage = "A luta contra a desesperança";
    
    // Phase 3: The Hope
    phases[2].number = 3;
    phases[2].duration = 60; // 60 seconds
    phases[2].elapsedTime = 0;
    phases[2].baseIntensity = 2.0f;
    phases[2].backgroundColor = (Color){ 40, 0, 40, 255 }; // Dark purple
    phases[2].projectileColor = (Color){ 255, 255, 200, 255 }; // Yellowish white
    phases[2].phaseMessage = "Uma luz no fim do túnel";
    
    // Set initial phase
    currentPhaseIndex = 0;
    phaseTransitionTimer = 180; // 3 seconds at 60 FPS
}

// Start a specific game phase
void StartGamePhase(int phaseIndex)
{
    if (phaseIndex >= 0 && phaseIndex < 3) {
        currentPhaseIndex = phaseIndex;
        
        // Clear all active projectiles
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            projectiles[i].active = false;
        }
        
        // Reset attack phase with phase-specific difficulty
        StartAttackPhase(
            GetRandomValue(0, TOTAL_PATTERNS-1), 
            300, 
            phases[currentPhaseIndex].baseIntensity, 
            20 - (phases[currentPhaseIndex].baseIntensity * 3)
        );
        
        // Transition state
        currentState = PHASE_TRANSITION;
        phaseTransitionTimer = 180; // 3 seconds
        
        // Reset score counter for new phase if needed
        // score = 0;
    }
}

// Update the current game phase
void UpdateGamePhase(void)
{
    // Increment elapsed time (60 frames = 1 second)
    if (frameCounter % 60 == 0) {
        phases[currentPhaseIndex].elapsedTime++;
    }
    
    // Check if current phase is complete
    if (phases[currentPhaseIndex].elapsedTime >= phases[currentPhaseIndex].duration) {
        // Move to next phase if not in the final phase
        if (currentPhaseIndex < 2) {
            StartGamePhase(currentPhaseIndex + 1);
        } else {
            // Game complete!
            // Could transition to a win state, for now just make it harder
            currentAttack.intensity += 0.2f;
            if (currentAttack.intensity > 5.0f) {
                currentAttack.intensity = 5.0f;
            }
        }
    }
}

// Draw the phase transition screen
void DrawPhaseTransition(void)
{
    ClearBackground(phases[currentPhaseIndex].backgroundColor);
    
    // Draw phase information
    DrawText(TextFormat("Fase %d", phases[currentPhaseIndex].number), 
             SCREEN_WIDTH/2 - MeasureText(TextFormat("Fase %d", phases[currentPhaseIndex].number), 60)/2,
             SCREEN_HEIGHT/3, 60, PURPLE);
             
    DrawText(phases[currentPhaseIndex].phaseMessage, 
             SCREEN_WIDTH/2 - MeasureText(phases[currentPhaseIndex].phaseMessage, 24)/2,
             SCREEN_HEIGHT/2, 24, WHITE);
             
    // Draw progress to next phase
    DrawRectangleLines(SCREEN_WIDTH/4, SCREEN_HEIGHT*2/3, SCREEN_WIDTH/2, 20, WHITE);
    DrawRectangle(SCREEN_WIDTH/4, SCREEN_HEIGHT*2/3, 
                  (SCREEN_WIDTH/2) * ((float)phaseTransitionTimer / 180.0f), 20, PURPLE);
}
