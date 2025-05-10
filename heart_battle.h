/*******************************************************************************************
*
*   HEART - Header file for heart customization
*   
*   Contains functions to draw a heart instead of a simple rectangle
*
********************************************************************************************/

#ifndef HEART_BATTLE_H
#define HEART_BATTLE_H

#include "raylib.h"
#include <math.h>

// Structure to represent a heart
typedef struct {
    Vector2 position;   // Center position of the heart
    float size;         // Size multiplier (1.0 = normal size)
    Color color;        // Color of the heart
    float rotation;     // Rotation angle in degrees
    float pulse;        // Pulse effect (0.0 - 1.0)
} Heart;

// Initialize a heart structure
static inline Heart InitHeart(Vector2 position, float size, Color color) {
    Heart heart = {
        .position = position,
        .size = size,
        .color = color,
        .rotation = 0.0f,
        .pulse = 0.0f
    };
    return heart;
}

// Draw a heart shape at the specified position
static inline void DrawHeart(Heart heart) {
    // Base size for the heart drawing
    float baseSize = 10.0f * heart.size;
    
    // Apply pulse effect to size
    float pulseEffect = 1.0f + 0.1f * sinf(heart.pulse * PI * 2);
    baseSize *= pulseEffect;
    
    // Points for the heart shape (centered at origin)
    Vector2 points[10];
    
    // Calculate heart shape points
    // Top curves
    points[0] = (Vector2){ -baseSize, -baseSize * 0.5f };         // Left curve start
    points[1] = (Vector2){ -baseSize * 0.5f, -baseSize * 1.2f };  // Left curve control
    points[2] = (Vector2){ 0, -baseSize * 0.5f };                 // Center top
    points[3] = (Vector2){ baseSize * 0.5f, -baseSize * 1.2f };   // Right curve control
    points[4] = (Vector2){ baseSize, -baseSize * 0.5f };          // Right curve end
    
    // Bottom point
    points[5] = (Vector2){ 0, baseSize };                         // Bottom tip
    
    // Side curves
    points[6] = (Vector2){ -baseSize, -baseSize * 0.5f };         // Back to left side
    
    // Apply rotation if needed
    if (heart.rotation != 0) {
        float cosa = cosf(heart.rotation * DEG2RAD);
        float sina = sinf(heart.rotation * DEG2RAD);
        
        for (int i = 0; i < 7; i++) {
            float x = points[i].x;
            float y = points[i].y;
            points[i].x = x * cosa - y * sina;
            points[i].y = x * sina + y * cosa;
        }
    }
    
    // Move all points to the heart position
    for (int i = 0; i < 7; i++) {
        points[i].x += heart.position.x;
        points[i].y += heart.position.y;
    }
    
    // Draw the heart
    DrawTriangle(points[0], points[2], points[6], heart.color);  // Left half
    DrawTriangle(points[2], points[4], points[5], heart.color);  // Right half
    DrawCircleV((Vector2){ points[0].x + baseSize * 0.5f, points[0].y }, baseSize * 0.5f, heart.color); // Left circle
    DrawCircleV((Vector2){ points[4].x - baseSize * 0.5f, points[4].y }, baseSize * 0.5f, heart.color); // Right circle
}

// Update a heart's pulse animation
static inline void UpdateHeartPulse(Heart* heart, float deltaTime) {
    heart->pulse += deltaTime;
    if (heart->pulse >= 1.0f) {
        heart->pulse -= 1.0f;
    }
}

// Get a rectangle representing the heart's hitbox (slightly smaller than visual representation)
static inline Rectangle GetHeartHitbox(Heart heart) {
    float hitboxSize = 8.0f * heart.size;
    return (Rectangle){
        heart.position.x - hitboxSize,
        heart.position.y - hitboxSize,
        hitboxSize * 2,
        hitboxSize * 2
    };
}

#endif // HEART_BATTLE_H
